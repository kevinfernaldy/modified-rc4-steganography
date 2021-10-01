#ifndef RC4WIDGET_H
#define RC4WIDGET_H

#include <QWidget>
#include <include/cipher/rc4/rc4.h>

namespace Ui {
class RC4Widget;
}

class RC4Widget : public QWidget
{
    Q_OBJECT

public:
    explicit RC4Widget(QWidget *parent = nullptr);
    ~RC4Widget();

private slots:
    void on_sourceComboBox_currentIndexChanged(int index);
    void on_browseFileInput_clicked();
    void on_browseFileOutput_clicked();
    void on_encryptButton_clicked();
    void on_decryptButton_clicked();
    void on_browseKey_clicked();
    void on_keySourceComboBox_currentIndexChanged(int index);
    void on_textTypeComboBox_currentIndexChanged(int index);

private:
    std::string readFile(std::string);
    void encrypt(std::string, std::string);
    void decrypt(std::string, std::string);
    void handleFileOutput(std::string, std::string);
    QString constructAsciiString(std::string);

    Ui::RC4Widget *ui;
    RC4Modified *cipher;
};

#endif // RC4WIDGET_H
