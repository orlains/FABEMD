#include <cmath>
#include <iostream>
#include <vector>
#include <sstream>

#include "CImg.h"
#include "FABEMD.h"

using namespace cimg_library;
using namespace std;

CImg<float> generateSample()
{
    CImg<float> sample(8, 8);

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

    return sample;
}

CImg<float> generateSinusoidal(float frequency, unsigned int width, unsigned int height)
{
    CImg<float> result(width, height);

    cimg_forXY(result, x, y)
    {
        result(x, y) = sin(x / frequency) + sin(y / frequency);
    }

    return result;
}

CImg<float> generateSynthetic(unsigned int count, unsigned int width, unsigned int height)
{
    CImg<float> result(width, height, 1U, 1U, 0.0);
    float frequency = (float)(sqrt(width * height) / 2.0);

    for (unsigned int i = 0; i < count; ++i, frequency /= 10)
    {
        result += generateSinusoidal(frequency, width, height);
    }

    return result;
}

int main(int argc, char **argv)
{
    // Retrieve informations from command line
    cimg_usage("Compute BEMC of input image using FABEMD.");
    const char* filename = cimg_option("-i", "data/elaine.bmp", "Input image file");
    const bool synthetic = (bool)cimg_option("-s", 0, "If different from 0, use synthetic data as input");
    const OSFW osfwType = (OSFW)cimg_option("-o", 3, "Order statistics filter widths type (0: SAME_TYPE_1, 1: SAME_TYPE_2, 2: SAME_TYPE_3, 3: SAME_TYPE_4, 4: DIFFERENT_TYPE_1, 5: DIFFERENT_TYPE_2, 6: DIFFERENT_TYPE_3, 7: DIFFERENT_TYPE_4). Please refer to documentation to see values assigned depending on type.");
    const unsigned int maximumAllowableIterations = cimg_option("-n", 1, "Maximal number of BIMC - ITS for the computation of a BIMC");
    const unsigned int size = cimg_option("-w", 3, "Size of the extrema search window");
    const float threshold = (float)cimg_option("-t", 0.05f, "Maximal standard variation thredshold to get to next BIMC");

    // Get input image
    CImg<float> input;
    if (synthetic)
    {
        input = generateSynthetic(3, 256, 256);
    }
    else
    {
        input = CImg<float>(filename);
    }

    // Compute BEMCs
    FABEMD fabemd(input, osfwType, maximumAllowableIterations, size, threshold);
    CImg<float> result = fabemd.execute();

    // Create frame
    CImgDisplay frame = CImgDisplay(512, 512);
    int z = 0;
    int wheelState = 1;
    ostringstream titleStream;

    while (!frame.is_closed())
    {
        // Process wheel events
        if (frame.wheel() != wheelState)
        {
            int delta = frame.wheel() - wheelState;
            z += delta;
            wheelState = frame.wheel();
            // Check if required index is in bound
            if (z < 0)
            {
                z = 0;
            }
            else if (z >= result.depth())
            {
                z = result.depth() - 1;
            }

            // Change frame's title
            if (z == 0)
            {
                titleStream << "Original image";
            }
            else
            {
                titleStream << "BEMC " << z << "/" << result.depth() - 1;
            }

            frame.set_title(titleStream.str().c_str());
            // Clear stream
            titleStream.str(std::string());

            // Display required image
            frame.display(result.get_slice(z));
        }
    }

    return 0;
}
