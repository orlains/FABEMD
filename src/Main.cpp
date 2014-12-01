#include <cmath>
#include <iostream>
#include <vector>

#include "CImg.h"
#include "FABEMD.h"

using namespace cimg_library;

int main(int argc, char **argv)
{
    CImg<float> sample(8, 8);

#pragma region "Sample definition"
    // 0
    sample(0, 0) = 8;
    sample(1, 0) = 8;
    sample(2, 0) = 4;
    sample(3, 0) = 1;
    sample(4, 0) = 5;
    sample(5, 0) = 2;
    sample(6, 0) = 6;
    sample(7, 0) = 3;

    // 1
    sample(0, 1) = 6;
    sample(1, 1) = 3;
    sample(2, 1) = 2;
    sample(3, 1) = 3;
    sample(4, 1) = 7;
    sample(5, 1) = 3;
    sample(6, 1) = 9;
    sample(7, 1) = 3;

    // 2
    sample(0, 2) = 7;
    sample(1, 2) = 8;
    sample(2, 2) = 3;
    sample(3, 2) = 2;
    sample(4, 2) = 1;
    sample(5, 2) = 4;
    sample(6, 2) = 3;
    sample(7, 2) = 7;

    // 3
    sample(0, 3) = 4;
    sample(1, 3) = 1;
    sample(2, 3) = 2;
    sample(3, 3) = 4;
    sample(4, 3) = 3;
    sample(5, 3) = 5;
    sample(6, 3) = 7;
    sample(7, 3) = 8;

    // 4
    sample(0, 4) = 6;
    sample(1, 4) = 2;
    sample(2, 4) = 4;
    sample(3, 4) = 1;
    sample(4, 4) = 2;
    sample(5, 4) = 5;
    sample(6, 4) = 3;
    sample(7, 4) = 4;

    // 5
    sample(0, 5) = 1;
    sample(1, 5) = 3;
    sample(2, 5) = 7;
    sample(3, 5) = 9;
    sample(4, 5) = 9;
    sample(5, 5) = 8;
    sample(6, 5) = 7;
    sample(7, 5) = 8;

    // 6
    sample(0, 6) = 9;
    sample(1, 6) = 2;
    sample(2, 6) = 6;
    sample(3, 6) = 7;
    sample(4, 6) = 6;
    sample(5, 6) = 8;
    sample(6, 6) = 7;
    sample(7, 6) = 7;

    // 7
    sample(0, 7) = 8;
    sample(1, 7) = 2;
    sample(2, 7) = 1;
    sample(3, 7) = 9;
    sample(4, 7) = 7;
    sample(5, 7) = 9;
    sample(6, 7) = 1;
    sample(7, 7) = 1;
#pragma endregion

    CImg<float> input("data/elaine.bmp");
    FABEMD fabemd(input);
    fabemd.execute().display();

    return 0;
}
