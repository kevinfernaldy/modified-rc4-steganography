#include <include/wavsteganography.h>

#include <fstream>
#include <bitset>
#include <sstream>
#include <iostream>
#include <random>

WAVSteganography::WAVSteganography(std::string fileInput)
{
    this->fileInput = fileInput;
}

void WAVSteganography::insertMessage(std::string message)
{
    std::ifstream fi;
    std::ofstream fo;

    int lastIndex = fileInput.find_last_of(".");
    std::string fileOutput = fileInput;
    fileOutput.insert(lastIndex, "_inserted");

    fi.open(fileInput, std::fstream::binary);
    fo.open(fileOutput, std::fstream::binary | std::ios::trunc);

    std::stringstream buffer;
    buffer << fi.rdbuf();
    std::string bufferString = buffer.str();

    fi.seekg(22, std::ios_base::beg);
    unsigned short channels;
    fi.read((char *) &channels, 2);

    fi.seekg(34, std::ios_base::beg);
    unsigned short sampleSize;
    fi.read((char *) &sampleSize, 2);

    // Find data
    unsigned int offset = 36;

    fi.seekg(offset, std::ios_base::beg);
    unsigned char tempFinder[4];
    fi.read((char *) &tempFinder, 4);

    std::string tempStorage;
    tempStorage += tempFinder[0];
    tempStorage += tempFinder[1];
    tempStorage += tempFinder[2];
    tempStorage += tempFinder[3];
    while (tempStorage != "data") {
        char c;
        fi.get(c);

        tempStorage += c;
        tempStorage.erase(0, 1);

        offset += 1;
    }
    offset += 4;

    fi.seekg(offset, std::ios_base::beg);

    // Set non-random flag
    unsigned char type[4];
    fi.read((char*) type, 4);

    unsigned int tempInt = *((unsigned int *) &type);
    std::string binaryString = std::bitset<32>(tempInt).to_string();

    binaryString[30] = '0';
    binaryString[31] = '0';
    unsigned int tempUL = std::stoull(binaryString, nullptr, 2);
    bufferString.erase(offset, 4);
    bufferString.insert(offset, (char *) &tempUL, 4);
    offset += 4;
    for (int i = 0; i < message.length(); i++) {
        unsigned char temp[4];

        fi.read((char*) temp, 4);

        unsigned int tempInt = *((unsigned int *) &temp);
        std::string binaryString = std::bitset<32>(tempInt).to_string();

        binaryString[31] = message[i];
        unsigned int tempUL = std::stoull(binaryString, nullptr, 2);
        bufferString.erase(offset, 4);
        bufferString.insert(offset, (char *) &tempUL, 4);

        offset += 4;
    }

    fo << bufferString;

    fi.close();
    fo.close();
}

void WAVSteganography::insertMessageRandom(std::string key, std::string message)
{
    std::ifstream fi;
    std::ofstream fo;

    int lastIndex = fileInput.find_last_of(".");
    std::string fileOutput = fileInput;
    fileOutput.insert(lastIndex, "_inserted");

    fi.open(fileInput, std::fstream::binary);
    fo.open(fileOutput, std::fstream::binary | std::ios::trunc);

    std::stringstream buffer;
    buffer << fi.rdbuf();
    std::string bufferString = buffer.str();

    fi.seekg(22, std::ios_base::beg);
    unsigned short channels;
    fi.read((char *) &channels, 2);

    fi.seekg(34, std::ios_base::beg);
    unsigned short sampleSize;
    fi.read((char *) &sampleSize, 2);

    fi.seekg(4, std::ios_base::beg);
    unsigned int size;
    fi.read((char*) &size, 4);

    // Find data
    unsigned int offset = 36;

    fi.seekg(offset, std::ios_base::beg);
    unsigned char tempFinder[4];
    fi.read((char *) &tempFinder, 4);

    std::string tempStorage;
    tempStorage += tempFinder[0];
    tempStorage += tempFinder[1];
    tempStorage += tempFinder[2];
    tempStorage += tempFinder[3];
    while (tempStorage != "data") {
        char c;
        fi.get(c);

        tempStorage += c;
        tempStorage.erase(0, 1);

        offset += 1;
    }
    offset += 4;

    fi.seekg(offset, std::ios_base::beg);

    // Set non-random flag
    unsigned char type[4];
    fi.read((char*) type, 4);

    unsigned int tempInt = *((unsigned int *) &type);
    std::string binaryString = std::bitset<32>(tempInt).to_string();

    binaryString[30] = '0';
    binaryString[31] = '0';
    unsigned int tempUL = std::stoull(binaryString, nullptr, 2);
    bufferString.erase(offset, 4);
    bufferString.insert(offset, (char *) &tempUL, 4);
    offset += 4;

    unsigned char xorKey = key[0];
    for (int i = 1; i < key.length(); i++) {
        xorKey ^= key[i];
    }

    std::default_random_engine generator;
    generator.seed(xorKey);
    std::uniform_int_distribution<int> distribution((int) offset, (int) size);

    for (int i = 0; i < message.length(); i++) {
        unsigned char temp[4];

        fi.read((char*) temp, 4);

        unsigned int tempInt = *((unsigned int *) &temp);
        std::string binaryString = std::bitset<32>(tempInt).to_string();

        binaryString[31] = message[i];
        unsigned int tempUL = std::stoull(binaryString, nullptr, 2);
        bufferString.erase(offset, 4);
        bufferString.insert(offset, (char *) &tempUL, 4);

        offset += 4;
    }

    fo << bufferString;

    fi.close();
    fo.close();
}

std::string WAVSteganography::extractMessage(int length)
{
    std::ifstream fi;

    fi.open(fileInput, std::fstream::binary);

    fi.seekg(22, std::ios_base::beg);
    unsigned short channels;
    fi.read((char *) &channels, 2);

    fi.seekg(34, std::ios_base::beg);
    unsigned short sampleSize;
    fi.read((char *) &sampleSize, 2);

    // Find data
    unsigned int offset = 36;

    fi.seekg(offset, std::ios_base::beg);
    unsigned char tempFinder[4];
    fi.read((char *) tempFinder, 4);

    std::string tempStorage;
    tempStorage += tempFinder[0];
    tempStorage += tempFinder[1];
    tempStorage += tempFinder[2];
    tempStorage += tempFinder[3];
    while (tempStorage != "data") {
        char c;
        fi.get(c);

        tempStorage += c;
        tempStorage.erase(0, 1);

        offset += 1;
    }
    offset += 8;

    fi.seekg(offset, std::ios_base::beg);
    std::ostringstream stream;
    for (int i = 0; i < length; i++) {
        unsigned char temp[4];

        fi.read((char*) temp, 4);

        unsigned int tempInt = *((unsigned int *) &temp);
        std::string binaryString = std::bitset<32>(tempInt).to_string();

        stream << binaryString[31];

        offset += 4;
    }

    fi.close();

    return stream.str();
}

std::string WAVSteganography::extractMessageRandom(std::string key, int length)
{
    std::ifstream fi;

    fi.open(fileInput, std::fstream::binary);

    fi.seekg(22, std::ios_base::beg);
    unsigned short channels;
    fi.read((char *) &channels, 2);

    fi.seekg(34, std::ios_base::beg);
    unsigned short sampleSize;
    fi.read((char *) &sampleSize, 2);

    fi.seekg(4, std::ios_base::beg);
    unsigned int size;
    fi.read((char*) &size, 4);

    // Find data
    unsigned int offset = 36;

    fi.seekg(offset, std::ios_base::beg);
    unsigned char tempFinder[4];
    fi.read((char *) tempFinder, 4);

    std::string tempStorage;
    tempStorage += tempFinder[0];
    tempStorage += tempFinder[1];
    tempStorage += tempFinder[2];
    tempStorage += tempFinder[3];
    while (tempStorage != "data") {
        char c;
        fi.get(c);

        tempStorage += c;
        tempStorage.erase(0, 1);

        offset += 1;
    }
    offset += 8;

    unsigned char xorKey = key[0];
    for (int i = 1; i < key.length(); i++) {
        xorKey ^= key[i];
    }

    std::default_random_engine generator;
    generator.seed(xorKey);
    std::uniform_int_distribution<int> distribution((int) offset, (int) size);

    fi.seekg(offset, std::ios_base::beg);
    std::ostringstream stream;
    for (int i = 0; i < length; i++) {
        unsigned char temp[4];

        fi.read((char*) temp, 4);

        unsigned int tempInt = *((unsigned int *) &temp);
        std::string binaryString = std::bitset<32>(tempInt).to_string();

        stream << binaryString[31];

        offset += 4;
    }

    fi.close();

    return stream.str();
}
