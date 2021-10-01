#include <include/cipher/rc4/rc4.h>

#include <sstream>
#include <bitset>
#include <vector>
#include <iostream>

RC4Modified::RC4Modified(std::string key)
{
    this->key = key;
}

void RC4Modified::encrypt()
{
    std::vector<unsigned long long> keyStream = this->generateKeyStream();
    int i = 0;
    int j = 0;

    std::ostringstream result;
    for (int index = 0; index < this->plainText.length(); index++) {
        i = (i + 1) % 256;
        j = (j + keyStream[i]) % 256;

        unsigned long long temp = keyStream[i];
        keyStream[i] = keyStream[j];
        keyStream[j] = temp;

        unsigned long long t = (keyStream[i] + keyStream[j]) % 256;
        unsigned long long u = keyStream[t];

        result << (char) (u ^ this->plainText[index]);
    }

    this->cipherText = result.str();
}

void RC4Modified::decrypt()
{
    std::vector<unsigned long long> keyStream = this->generateKeyStream();
    int i = 0;
    int j = 0;

    std::ostringstream result;
    for (int index = 0; index < this->cipherText.length(); index++) {
        i = (i + 1) % 256;
        j = (j + keyStream[i]) % 256;

        unsigned long long temp = keyStream[i];
        keyStream[i] = keyStream[j];
        keyStream[j] = temp;

        unsigned long long t = (keyStream[i] + keyStream[j]) % 256;
        unsigned long long u = keyStream[t];

        result << (char) (u ^ this->cipherText[index]);
    }

    this->plainText = result.str();
}

std::string RC4Modified::getCipherText() { return this->cipherText; }
std::string RC4Modified::getPlainText() { return this->plainText; }
void RC4Modified::setCipherText(std::string cipherText) { this->cipherText = cipherText; }
void RC4Modified::setPlainText(std::string plainText) { this->plainText = plainText; }

std::vector<unsigned long long> RC4Modified::generateKeyStream()
{
    // Adds LFSR to KSA
    std::vector<unsigned long long> stream;
    for (int i = 0; i <= 255; i++) {
        std::string state = std::bitset<8>(i).to_string();
        std::string startState = state;

        std::ostringstream resultStream;
        do {
            int lastTap = state[0] - '0';
            int firstTap = state[state.length() - 1] - '0';

            resultStream << state[state.length() - 1];
            state.pop_back();
            state.insert(0, 1, (char) (lastTap ^ firstTap + '0'));
        } while (state != startState);

        stream.push_back(std::stoull(resultStream.str(), nullptr, 2));
    }

    int j = 0;
    for (int i = 0; i <= 255; i++) {
        j = (j + stream[i] + this->key[i % this->key.length()]) % 256;

        char temp = stream[j];
        stream[j] = stream[i];
        stream[i] = temp;
    }

    return stream;
}
