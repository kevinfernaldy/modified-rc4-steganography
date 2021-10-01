#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

#include <QWidget>

namespace Ui {
    class SteganographyWidget;
}

class SteganographyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SteganographyWidget(QWidget *parent = nullptr);
    ~SteganographyWidget();

private slots:
    void on_sourceComboBox_currentIndexChanged(int index);
    void on_browseFileInput_clicked();
    void on_encryptButton_clicked();
    void on_decryptButton_clicked();
    void on_browseKey_clicked();
    void on_keySourceComboBox_currentIndexChanged(int index);
    void on_textTypeComboBox_currentIndexChanged(int index);

    void on_checkBox_clicked(bool checked);

    void on_fileTypeComboBox_currentIndexChanged(int index);

private:
    std::string readFile(std::string);
    void encrypt(std::string, std::string);
    std::string decrypt(std::string, int);
    void handleFileOutput(std::string, std::string);
    QString constructAsciiString(std::string);

    Ui::SteganographyWidget *ui;
};

#endif // STEGANOGRAPHY_H
