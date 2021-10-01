#include <include/rc4widget.h>
#include "ui_rc4widget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <sstream>

RC4Widget::RC4Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RC4Widget)
{
    ui->setupUi(this);

    ui->sourceComboBox->addItem("keyboard");
    ui->sourceComboBox->addItem("file");

    ui->keySourceComboBox->addItem("keyboard");
    ui->keySourceComboBox->addItem("file");

    ui->browseKey->setVisible(false);

    ui->textTypeContainer_2->setVisible(false);
    ui->textTypeComboBox->addItem("plaintext");
    ui->textTypeComboBox->addItem("ciphertext");

    ui->fileContainer->setVisible(false);
    ui->keyboardContainer->setVisible(true);

    ui->decryptButton->setVisible(true);
    ui->encryptButton->setVisible(true);

    cipher = nullptr;
}

RC4Widget::~RC4Widget()
{
    delete ui;
    if (cipher != nullptr) delete cipher;
}

void RC4Widget::encrypt(std::string key, std::string plainText)
{
    if (cipher != nullptr) delete cipher;
    cipher = new RC4Modified(key);

    cipher->setPlainText(plainText);

    cipher->encrypt();
}

void RC4Widget::decrypt(std::string key, std::string cipherText)
{
    if (cipher != nullptr) delete cipher;
    cipher = new RC4Modified(key);

    cipher->setCipherText(cipherText);

    cipher->decrypt();
}

std::string RC4Widget::readFile(std::string fileName)
{
    std::ifstream fileInput(fileName.c_str(), std::ios::binary);
    std::ostringstream buffer;

    buffer << fileInput.rdbuf();

    std::string content = buffer.str();

    fileInput.close();

    return content;
}

void RC4Widget::handleFileOutput(std::string fileName, std::string content)
{
    std::ofstream fileOutput(fileName, std::ios::binary | std::ios::trunc);

    fileOutput << content;

    fileOutput.close();
}

QString RC4Widget::constructAsciiString(std::string text)
{
    QString s;
    for (int i = 0; i < text.length(); i++) {
        s += QChar(text[i]);
    }

    return s;
}

void RC4Widget::on_sourceComboBox_currentIndexChanged(int index)
{
    switch(index) {
        case 0: {
            ui->textTypeContainer_2->setVisible(false);
            ui->fileContainer->setVisible(false);
            ui->keyboardContainer->setVisible(true);
            ui->verticalSpacer->changeSize(20, 40, QSizePolicy::Preferred, QSizePolicy::Expanding);

            ui->decryptButton->setVisible(true);
            ui->encryptButton->setVisible(true);
            break;
        }
        case 1: {
            ui->textTypeContainer_2->setVisible(true);
            ui->fileContainer->setVisible(true);
            ui->keyboardContainer->setVisible(false);
            ui->verticalSpacer->changeSize(20, 9, QSizePolicy::Minimum, QSizePolicy::Fixed);

            on_textTypeComboBox_currentIndexChanged(ui->textTypeComboBox->currentIndex());
            break;
        }
    }
}

void RC4Widget::on_keySourceComboBox_currentIndexChanged(int index)
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

void RC4Widget::on_textTypeComboBox_currentIndexChanged(int index)
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


void RC4Widget::on_browseFileInput_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString fileName = dialog->getOpenFileName();
    if (!fileName.isNull()) {
        ui->fileInputText->setText(fileName);
    }

    delete dialog;
}


void RC4Widget::on_browseFileOutput_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString fileName = dialog->getOpenFileName();
    if (!fileName.isNull()) {
        ui->fileOutputText->setText(fileName);
    }

    delete dialog;
}

void RC4Widget::on_browseKey_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString fileName = dialog->getOpenFileName();
    if (!fileName.isNull()) {
        ui->keyTextBox->setText(fileName);
    }

    delete dialog;
}

void RC4Widget::on_encryptButton_clicked()
{
    try {
        std::string content;
        int contentSourceFlag = ui->sourceComboBox->currentIndex();
        if (contentSourceFlag == 0) {
            content = ui->plainTextBox->toPlainText().toStdString();
        } else {
            if (ui->fileInputText->text().toStdString().length() == 0) {
                throw std::runtime_error("File input is empty");
            }
            if (ui->fileOutputText->text().toStdString().length() == 0) {
                throw std::runtime_error("File output is empty");
            }
            content = readFile(ui->fileInputText->text().toStdString());
        }

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

        std::string transformedCipherText = cipher->getCipherText();

        if (contentSourceFlag == 0) {
            ui->cipherTextBox->setPlainText(constructAsciiString(transformedCipherText));
        } else {
            handleFileOutput(ui->fileOutputText->text().toStdString(), transformedCipherText);
        }
    }  catch (std::runtime_error message) {
        QMessageBox errorBox(QMessageBox::Icon::Critical, QString("Error"), QString(message.what()), QMessageBox::StandardButton::Ok);

        errorBox.exec();
    }
}


void RC4Widget::on_decryptButton_clicked()
{
    try {
        std::string content;
        int contentSourceFlag = ui->sourceComboBox->currentIndex();
        if (contentSourceFlag == 0) {
            content = ui->cipherTextBox->toPlainText().toLatin1().data();
        } else {
            if (ui->fileInputText->text().toStdString().length() == 0) {
                throw std::runtime_error("File input is empty");
            }
            if (ui->fileOutputText->text().toStdString().length() == 0) {
                throw std::runtime_error("File output is empty");
            }
            content = readFile(ui->fileInputText->text().toStdString());
        }

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
            throw std::runtime_error("Ciphertext length is zero");
        }

        decrypt(key, content);

        std::string plainText = cipher->getPlainText();

        if (contentSourceFlag == 0) {
            ui->plainTextBox->setPlainText(QString(plainText.c_str()));
        } else {
            handleFileOutput(ui->fileOutputText->text().toStdString(), plainText);
        }
    } catch (std::runtime_error message) {
        QMessageBox errorBox(QMessageBox::Icon::Critical, QString("Error"), QString(message.what()), QMessageBox::StandardButton::Ok);

        errorBox.exec();
    }
}
