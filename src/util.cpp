#include <include/util.h>
#include <cmath>
#include <iostream>

Util::Util()
{

}

double Util::calculatePSNR(double width, double height, double offset, std::ifstream* fileInput, std::ifstream* fileOutput)
{
    fileInput->seekg(offset, std::ios_base::beg);
    fileOutput->seekg(offset, std::ios_base::beg);

    double sumRatio = 0;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            unsigned int input;
            unsigned int output;
            fileInput->read((char *) &input, 4);
            fileOutput->read((char *) &output, 4);

            sumRatio += std::pow((double)input - output, 2);
        }
    }

    double rms = std::sqrt(sumRatio / (width * height));

    return 20 * std::log10(255/rms);
}
