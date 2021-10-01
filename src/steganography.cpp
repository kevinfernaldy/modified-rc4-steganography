#include <include/steganography.h>
#include "ui_steganographywidget.h"
#include <include/cipher/rc4/rc4.h>
#include <include/bmpsteganography.h>
#include <include/wavsteganography.h>
#include <include/util.h>

#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <bitset>

SteganographyWidget::SteganographyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SteganographyWidget)
{
    ui->setupUi(this);

    ui->keySourceComboBox->addItem("keyboard");
    ui->keySourceComboBox->addItem("file");

    ui->fileTypeComboBox->addItem("BMP (32-bit)");
    ui->fileTypeComboBox->addItem("WAV (32-bit)");

    ui->browseKey->setVisible(false);

    ui->multiTextBox->setVisible(true);
    ui->multiTextBoxText->setVisible(true);
    ui->encryptButton->setVisible(true);
    ui->decryptButton->setVisible(false);
    ui->lengthText->setVisible(false);
    ui->lengthTextBox->setVisible(false);
}

SteganographyWidget::~SteganographyWidget()
{
    delete ui;
}

void SteganographyWidget::encrypt(std::string key, std::string plainText)
{
    std::string localPlainText = plainText;
    if (ui->isEncrypted->isChecked()) {
        RC4Modified *cipher = new RC4Modified(key);

        cipher->setPlainText(localPlainText);

        cipher->encrypt();

        localPlainText = cipher->getCipherText();

        delete cipher;
    }

    std::ostringstream stream;
    for (int i = 0; i < plainText.length(); i++) {
        stream << (std::bitset<8>(localPlainText[i]).to_string());
    }

    if (ui->fileTypeComboBox->currentIndex() == 0) {
        BMPSteganography Steganography(ui->fileInputText->text().toStdString());

        if (ui->isRandom->isChecked()) {
            Steganography.insertMessageRandom(key, stream.str());
        } else {
            Steganography.insertMessage(stream.str());
        }

        int lastIndex = ui->fileInputText->text().toStdString().find_last_of(".");
        std::string fileOutput = ui->fileInputText->text().toStdString();
        fileOutput.insert(lastIndex, "_inserted");
        std::ifstream fi(ui->fileInputText->text().toStdString(), std::ios::binary);
        std::ifstream fo(fileOutput, std::ios::binary);

        fi.seekg(18, std::ios_base::beg);
        unsigned int width;
        unsigned int height;
        fi.read((char *) &width, 4);
        fi.read((char *) &height, 4);

        fi.seekg(10, std::ios_base::beg);
        unsigned int offset;
        fi.read((char *) &offset, 4);

        fi.seekg(0, std::ios_base::beg);
        ui->psnrTextBox->setText(QString(std::to_string(Util::calculatePSNR(width, height, offset, &fi, &fo)).c_str()));

        fi.close();
        fo.close();
    } else {
        WAVSteganography Steganography(ui->fileInputText->text().toStdString());

        if (ui->isRandom->isChecked()) {
            Steganography.insertMessageRandom(key, stream.str());
        } else {
            Steganography.insertMessage(stream.str());
        }
    }
}

std::string SteganographyWidget::decrypt(std::string key, int length)
{
    BMPSteganography Steganography(ui->fileInputText->text().toStdString());

    std::ifstream fi;
    fi.open(ui->fileInputText->text().toStdString(), std::fstream::in | std::fstream::binary);

    fi.seekg(10, std::ios_base::beg);
    unsigned int offset;
    fi.read((char*) &offset, 4);

    fi.seekg(offset, std::ios_base::beg);
    unsigned char type[4];
    fi.read((char*) type, 4);

    unsigned int tempInt = *((unsigned int *) &type);
    std::string binaryString = std::bitset<32>(tempInt).to_string();

    fi.close();

    std::string binaryText;

    if (ui->fileTypeComboBox->currentIndex() == 0) {
        BMPSteganography Steganography(ui->fileInputText->text().toStdString());

        if (binaryString.substr(30, 2) == "00") {
            binaryText = Steganography.extractMessage(length * 8);
        } else {
            binaryText = Steganography.extractMessageRandom(key, length * 8);
        }
    } else {
        WAVSteganography Steganography(ui->fileInputText->text().toStdString());

        if (binaryString.substr(30, 2) == "00") {
            binaryText = Steganography.extractMessage(length * 8);
        } else {
            binaryText = Steganography.extractMessageRandom(key, length * 8);
        }
    }

    std::ostringstream resultTextStream;
    int i = 0;
    while (i + 7 < binaryText.length()) {
        std::string temp = binaryText.substr(i, 8);

        resultTextStream << (char) std::stoull(temp, nullptr, 2);

        i += 8;
    }

    std::string localPlainText = resultTextStream.str();

    if (ui->isEncrypted->isChecked()) {
        RC4Modified *cipher = new RC4Modified(key);

        cipher->setCipherText(localPlainText);

        cipher->decrypt();

        localPlainText = cipher->getPlainText();

        delete cipher;
    }

    return localPlainText;
}

std::string SteganographyWidget::readFile(std::string fileName)
{
    std::ifstream fileInput(fileName.c_str(), std::ios::binary);
    std::ostringstream buffer;

    buffer << fileInput.rdbuf();

    std::string content = buffer.str();

    fileInput.close();

    return content;
}

void SteganographyWidget::handleFileOutput(std::string fileName, std::string content)
{
    std::ofstream fileOutput(fileName, std::ios::binary | std::ios::trunc);

    fileOutput << content;

    fileOutput.close();
}

QString SteganographyWidget::constructAsciiString(std::string text)
{
    QString s;
    for (int i = 0; i < text.length(); i++) {
        s += QChar(text[i]);
    }

    return s;
}

void SteganographyWidget::on_sourceComboBox_currentIndexChanged(int index)
{
    switch(index) {
        case 0: {
            ui->verticalSpacer->changeSize(20, 40, QSizePolicy::Preferred, QSizePolicy::Expanding);

            ui->decryptButton->setVisible(true);
            ui->encryptButton->setVisible(true);
            break;
        }
        case 1: {
            ui->verticalSpacer->changeSize(20, 9, QSizePolicy::Minimum, QSizePolicy::Fixed);
            break;
        }
    }
}

void SteganographyWidget::on_keySourceComboBox_currentIndexChanged(int index)
{
    ui->keyTextBox->clear();
    switch(index) {
        case 0: {
            ui->browseKey->setVisible(false);
            break;
        }
        case 1: {
            ui->browseKey->setVisible(true);
            break;
        }
    }
}

void SteganographyWidget::on_textTypeComboBox_currentIndexChanged(int index)
{
    switch(index) {
        case 0: {
            ui->decryptButton->setVisible(false);
            ui->encryptButton->setVisible(true);
            break;
        }
        case 1: {
            ui->decryptButton->setVisible(true);
            ui->encryptButton->setVisible(false);
            break;
        }
    }
}


void SteganographyWidget::on_browseFileInput_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString fileName = dialog->getOpenFileName();
    if (!fileName.isNull()) {
        ui->fileInputText->setText(fileName);
    }

    delete dialog;
}

void SteganographyWidget::on_browseKey_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString fileName = dialog->getOpenFileName();
    if (!fileName.isNull()) {
        ui->keyTextBox->setText(fileName);
    }

    delete dialog;
}

void SteganographyWidget::on_encryptButton_clicked()
{
    try {
        std::string content = ui->multiTextBox->text().toLatin1().data();

        std::string key;
        if (ui->keySourceComboBox->currentIndex() == 0) {
            key = ui->keyTextBox->text().toLatin1().data();
        } else {
            if (ui->keyTextBox->text().toStdString().length() == 0) {
                throw std::runtime_error("Key file input is empty");
            }
            key = readFile(ui->keyTextBox->text().toStdString());
        }

        if (key.length() == 0) {
            throw std::runtime_error("Key length is zero");
        }
        if (content.length() == 0) {
            throw std::runtime_error("Plaintext length is zero");
        }

        encrypt(key, content);

        QMessageBox errorBox(QMessageBox::Icon::Information, QString("Info"), QString("Message berhasil dimasukkan"), QMessageBox::StandardButton::Ok);

        errorBox.exec();

    }  catch (std::runtime_error message) {
        QMessageBox errorBox(QMessageBox::Icon::Critical, QString("Error"), QString(message.what()), QMessageBox::StandardButton::Ok);

        errorBox.exec();
    }
}


void SteganographyWidget::on_decryptButton_clicked()
{
    try {
        int length = ui->lengthTextBox->text().toInt();

        std::string key;
        if (ui->keySourceComboBox->currentIndex() == 0) {
            key = ui->keyTextBox->text().toLatin1().data();
        } else {
            if (ui->keyTextBox->text().toStdString().length() == 0) {
                throw std::runtime_error("Key file input is empty");
            }
            key = readFile(ui->keyTextBox->text().toStdString());
        }

        if (key.length() == 0) {
            throw std::runtime_error("Key length is zero");
        }


        std::string plainText = decrypt(key, length);

        QMessageBox errorBox(QMessageBox::Icon::Information, QString("Result"), QString(plainText.c_str()), QMessageBox::StandardButton::Ok);

        errorBox.exec();


    } catch (std::runtime_error message) {
        QMessageBox errorBox(QMessageBox::Icon::Critical, QString("Error"), QString(message.what()), QMessageBox::StandardButton::Ok);

        errorBox.exec();
    }
}


void SteganographyWidget::on_checkBox_clicked(bool checked)
{
    if (checked) {
        ui->multiTextBox->setVisible(false);
        ui->multiTextBoxText->setVisible(false);
        ui->encryptButton->setVisible(false);
        ui->decryptButton->setVisible(true);
        ui->lengthText->setVisible(true);
        ui->lengthTextBox->setVisible(true);
        ui->isRandom->setVisible(false);
    } else {
        ui->multiTextBox->setVisible(true);
        ui->multiTextBoxText->setVisible(true);
        ui->encryptButton->setVisible(true);
        ui->decryptButton->setVisible(false);
        ui->lengthText->setVisible(false);
        ui->lengthTextBox->setVisible(false);
        ui->isRandom->setVisible(true);
    }
}


void SteganographyWidget::on_fileTypeComboBox_currentIndexChanged(int index)
{
    switch(index) {
        case 0: {
            ui->psnrTextBox->setVisible(true);
            ui->label_8->setVisible(true);
            break;
        }
        case 1: {
            ui->psnrTextBox->setVisible(false);
            ui->label_8->setVisible(false);
            break;
        }
    }
}

