#ifndef BMPSTEGANOGRAPHY_H
#define BMPSTEGANOGRAPHY_H

#include <string>

class BMPSteganography
{
public:
    BMPSteganography(std::string);

    void insertMessage(std::string);
    void insertMessageRandom(std::string, std::string);
    std::string extractMessage(int);
    std::string extractMessageRandom(std::string, int);

private:
    std::string fileInput;
    std::string fileOutput;
};

#endif // BMPSTEGANOGRAPHY_H
