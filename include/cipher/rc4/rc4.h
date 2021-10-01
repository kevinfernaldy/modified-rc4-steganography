#ifndef RC4_H
#define RC4_H

#include <string>
#include <vector>

class RC4Modified
{
public:
    RC4Modified(std::string);

    void encrypt();
    void decrypt();

    void setCipherText(std::string);
    void setPlainText(std::string);
    std::string getCipherText();
    std::string getPlainText();

private:
    std::vector<unsigned long long> generateKeyStream();

    std::string key;
    std::string keyStream;

    std::string plainText;
    std::string cipherText;
};

#endif // RC4_H
