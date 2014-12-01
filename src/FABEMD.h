#ifndef __FABEMD_H__
#define __FABEMD_H__

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "CImg.h"
#include "Extrema.h"

enum OSFW
{
    SAME_TYPE_1,
    SAME_TYPE_2,
    SAME_TYPE_3,
    SAME_TYPE_4,
    DIFFERENT_TYPE_1,
    DIFFERENT_TYPE_2,
    DIFFERENT_TYPE_3,
    DIFFERENT_TYPE_4
};

class FABEMD
{
private:
    unsigned int _width;
    unsigned int _height;
    unsigned int _size;
    float _threshold;
    OSFW _osfwType;
    unsigned int _windowWidthMax;
    unsigned int _windowWidthMin;

    cimg_library::CImg<float> _input;
    cimg_library::CImg<float> _bimf;
    cimg_library::CImg<float> _lowerEnvelope;
    cimg_library::CImg<float> _upperEnvelope;
    cimg_library::CImg<float> _averageEnvelope;

    std::vector<Extrema> _localMinimas;
    std::vector<Extrema> _localMaximas;

    void buildExtremasMaps();
    void computeNearests(std::vector<Extrema> & extremas);
    bool hasBIMFProperties();
    unsigned int extremaCount();
    void computeWindowsWidths();
    void computeLowerEnvelope();
    void computeUpperEnvelope();
public:
    FABEMD(const cimg_library::CImg<float> & input, unsigned int size = 3, float threshold = 0.5, OSFW osfwType = OSFW::SAME_TYPE_4);
    ~FABEMD();
    cimg_library::CImg<float> execute();
};

#endif // __FABEMD_H__
