#ifndef WAVSTEGANOGRAPHY_H
#define WAVSTEGANOGRAPHY_H

#include <string>

class WAVSteganography
{
public:
    WAVSteganography(std::string);

    void insertMessage(std::string);
    void insertMessageRandom(std::string, std::string);
    std::string extractMessage(int);
    std::string extractMessageRandom(std::string, int);
private:
    std::string fileInput;
    std::string fileOutput;
};

#endif // WAVSTEGANOGRAPHY_H
