#ifndef UTIL_H
#define UTIL_H

#include <fstream>

class Util
{
public:
    Util();

    static double calculatePSNR(double, double, double, std::ifstream*, std::ifstream*);
};

#endif // UTIL_H
