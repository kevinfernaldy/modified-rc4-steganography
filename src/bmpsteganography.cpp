#include <include/bmpsteganography.h>

#include <fstream>
#include <bitset>
#include <sstream>
#include <iostream>
#include <random>

BMPSteganography::BMPSteganography(std::string fileInput)
{
    this->fileInput = fileInput;
}

void BMPSteganography::insertMessage(std::string message)
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

    fi.seekg(10, std::ios_base::beg);
    unsigned int offset;
    fi.read((char*) &offset, 4);

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

void BMPSteganography::insertMessageRandom(std::string key, std::string message)
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

    fi.seekg(2, std::ios_base::beg);
    unsigned int size;
    fi.read((char*) &size, 4);
    size -= 54;

    unsigned char xorKey = key[0];
    for (int i = 1; i < key.length(); i++) {
        xorKey ^= key[i];
    }

    std::default_random_engine generator;
    generator.seed(xorKey);
    std::uniform_int_distribution<int> distribution(54, (int) size);

    fi.seekg(10, std::ios_base::beg);
    unsigned int offset;
    fi.read((char*) &offset, 4);

    fi.seekg(offset, std::ios_base::beg);
    fo.seekp(offset + 4, std::ios_base::beg);

    // Set random flag
    unsigned char type[4];
    fi.read((char*) type, 4);

    unsigned int tempInt = *((unsigned int *) &type);
    std::string binaryString = std::bitset<32>(tempInt).to_string();

    binaryString[30] = '0';
    binaryString[31] = '1';
    unsigned int tempUL = std::stoull(binaryString, nullptr, 2);
    bufferString.erase(offset, 4);
    bufferString.insert(offset, (char *) &tempUL, 4);
    offset += 4;
    for (int i = 0; i < message.length(); i++) {
        fi.seekg(offset, std::ios_base::beg);
        fo.seekp(offset, std::ios_base::beg);
        unsigned char temp[4];

        fi.read((char*) temp, 4);

        unsigned int tempInt = *((unsigned int *) &temp);
        std::string binaryString = std::bitset<32>(tempInt).to_string();

        binaryString[31] = message[i];
        unsigned int tempUL = std::stoull(binaryString, nullptr, 2);
        bufferString.erase(offset, 4);
        bufferString.insert(offset, (char *) &tempUL, 4);

        offset = distribution(generator) * 4;
    }

    fo << bufferString;

    fi.close();
    fo.close();
}

std::string BMPSteganography::extractMessage(int length)
{
    std::fstream fi;

    fi.open(fileInput, std::fstream::in | std::fstream::binary);

    fi.seekg(10, std::ios_base::beg);
    unsigned int offset;
    fi.read((char*) &offset, 4);

    offset += 4;
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

std::string BMPSteganography::extractMessageRandom(std::string key, int length)
{
    std::fstream fi;

    fi.open(fileInput, std::fstream::in | std::fstream::binary);

    fi.seekg(2, std::ios_base::beg);
    unsigned int size;
    fi.read((char*) &size, 4);
    size -= 54;

    unsigned char xorKey = key[0];
    for (int i = 1; i < key.length(); i++) {
        xorKey ^= key[i];
    }

    std::default_random_engine generator;
    generator.seed(xorKey);
    std::uniform_int_distribution<int> distribution(54, (int) size);

    fi.seekg(10, std::ios_base::beg);
    unsigned int offset;
    fi.read((char*) &offset, 4);

    offset += 4;
    std::ostringstream stream;
    for (int i = 0; i < length; i++) {
        fi.seekg(offset, std::ios_base::beg);
        unsigned char temp[4];

        fi.read((char*) temp, 4);

        unsigned int tempInt = *((unsigned int *) &temp);
        std::string binaryString = std::bitset<32>(tempInt).to_string();

        stream << binaryString[31];

        offset = distribution(generator) * 4;
        std::cout << binaryString << std::endl;
    }

    fi.close();

    return stream.str();
}
