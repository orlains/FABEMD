#include "FABEMD.h"

using namespace cimg_library;

FABEMD::FABEMD(const CImg<float> & input, unsigned int size, float threshold, OSFW osfwType)
{
    _input = input.get_channel(0);
    _width = (unsigned int)_input.width();
    _height = (unsigned int)_input.height();

    _bimf = CImg<float>(_input);
    _lowerEnvelope = CImg<float>(_width, _height);
    _upperEnvelope = CImg<float>(_width, _height);
    _averageEnvelope = CImg<float>(_width, _height);

    _size = size;
    _threshold = threshold;
    _osfwType = osfwType;
}

FABEMD::~FABEMD() {}

void FABEMD::buildExtremasMaps()
{
    _localMinimas.clear();
    _localMaximas.clear();

    // Loop over image pixels
    cimg_forXY(_bimf, m, n)
    {
        bool isMaxima = true;
        bool isMinima = true;
        unsigned int minK = (unsigned int)std::max(0, (int)(m - (_size - 1) / 2));
        unsigned int minL = (unsigned int)std::max(0, (int)(n - (_size - 1) / 2));
        unsigned int maxK = (unsigned int)std::min((int)(_width - 1), (int)(m + (_size - 1) / 2));
        unsigned int maxL = (unsigned int)std::min((int)(_height - 1), (int)(n + (_size - 1) / 2));
        unsigned int k = minK;
        unsigned int l = minL;

        // Loop over (m,n) neighborhood
        while ((isMinima || isMaxima) && k <= maxK)
        {
            while ((isMinima || isMaxima) && l <= maxL)
            {
                if (k != m || l != n)
                {
                    if (_bimf(k, l) >= _bimf(m, n))
                    {
                        isMaxima = false;
                    }
                    if (_bimf(k, l) <= _bimf(m, n))
                    {
                        isMinima = false;
                    }

                }
                ++l;
            }
            l = minL;
            ++k;
        }

        // Add point to local maxima (minima) map if strictly higher (lower) than each of its neighbour
        if (isMaxima)
        {
            _localMaximas.push_back(Extrema(m, n));
        }
        if (isMinima)
        {
            _localMinimas.push_back(Extrema(m, n));
        }
    }
}

void FABEMD::computeNearests(std::vector<Extrema> & extremas)
{
    // Get distance to nearest extrema for each extrema
    for (std::vector<Extrema>::iterator i = extremas.begin(); i != extremas.end(); ++i)
    {
        for (std::vector<Extrema>::iterator j = i + 1; j != extremas.end(); ++j)
        {
            float distance = i->distanceTo(*j);

            if (i->distance() > distance)
            {
                i->setDistance(distance);
            }

            if (j->distance() > distance)
            {
                j->setDistance(distance);
            }
        }
    }
}

bool FABEMD::hasBIMFProperties()
{
    float standardDeviation = 0.0;
    cimg_forXY(_averageEnvelope, x, y)
    {
        standardDeviation += (_averageEnvelope(x, y) * _averageEnvelope(x, y)) / (_bimf(x, y) * _bimf(x, y));
    }
    std::cout << "Standard deviation: " << standardDeviation << std::endl;
    return standardDeviation < _threshold;
}

unsigned int FABEMD::extremaCount()
{
    return (unsigned int)(_localMinimas.size() + _localMaximas.size());
}

void FABEMD::computeWindowsWidths()
{
    switch (_osfwType)
    {
    case OSFW::SAME_TYPE_1:
        _windowWidthMin = (unsigned int)std::min(
            _localMinimas.crbegin()->distance(),
            _localMaximas.cbegin()->distance());
        _windowWidthMax = _windowWidthMin;
        break;
    case OSFW::SAME_TYPE_2:
        _windowWidthMin = (unsigned int)std::max(
            _localMinimas.crbegin()->distance(),
            _localMaximas.cbegin()->distance());
        _windowWidthMax = _windowWidthMin;
        break;
    case OSFW::SAME_TYPE_3:
        _windowWidthMin = (unsigned int)std::min(
            _localMinimas.cbegin()->distance(),
            _localMaximas.crbegin()->distance());
        _windowWidthMax = _windowWidthMin;
        break;
    case OSFW::SAME_TYPE_4:
        _windowWidthMin = (unsigned int)std::max(
            _localMinimas.cbegin()->distance(),
            _localMaximas.crbegin()->distance());
        _windowWidthMax = _windowWidthMin;
        break;
    case OSFW::DIFFERENT_TYPE_1:
        _windowWidthMin = (unsigned int)_localMinimas.crbegin()->distance();
        _windowWidthMax = (unsigned int)_localMaximas.cbegin()->distance();
        break;
    case OSFW::DIFFERENT_TYPE_2:
        _windowWidthMin = (unsigned int)_localMinimas.crbegin()->distance();
        _windowWidthMax = (unsigned int)_localMaximas.crbegin()->distance();
        break;
    case OSFW::DIFFERENT_TYPE_3:
        _windowWidthMin = (unsigned int)_localMinimas.cbegin()->distance();
        _windowWidthMax = (unsigned int)_localMaximas.cbegin()->distance();
        break;
    case OSFW::DIFFERENT_TYPE_4:
        _windowWidthMin = (unsigned int)_localMinimas.cbegin()->distance();
        _windowWidthMax = (unsigned int)_localMaximas.crbegin()->distance();
        break;
    default:
        _windowWidthMin = 3;
        _windowWidthMax = 3;
        break;
    }

    if (_windowWidthMin % 2 == 0)
    {
        ++_windowWidthMin;
    }
    if (_windowWidthMax % 2 == 0)
    {
        ++_windowWidthMax;
    }
}

void FABEMD::computeLowerEnvelope()
{
    _lowerEnvelope.fill(_bimf(_localMinimas.cbegin()->x(), _localMinimas.cbegin()->y()));
    // For each minima
    for (std::vector<Extrema>::iterator it = _localMinimas.begin(); it != _localMinimas.end(); ++it)
    {
        Extrema extrema = *it;
        unsigned int m = extrema.x();
        unsigned int n = extrema.y();
        float extremaValue = _bimf(m, n);
        unsigned int minK = (unsigned int)std::max(0, (int)(m - (_windowWidthMin - 1) / 2));
        unsigned int minL = (unsigned int)std::max(0, (int)(n - (_windowWidthMin - 1) / 2));
        unsigned int maxK = (unsigned int)std::min((int)(_width - 1), (int)(m + (_windowWidthMin - 1) / 2));
        unsigned int maxL = (unsigned int)std::min((int)(_height - 1), (int)(n + (_windowWidthMin - 1) / 2));
        unsigned int k = minK;
        unsigned int l = minL;

        // Loop over (m,n) neighborhood
        while (k <= maxK)
        {
            while (l <= maxL)
            {
                if (_lowerEnvelope(k, l) > extremaValue)
                {
                    _lowerEnvelope(k, l) = extremaValue;
                }
                ++l;
            }
            l = minL;
            ++k;
        }
    }
}

void FABEMD::computeUpperEnvelope()
{
    _upperEnvelope.fill(_bimf(_localMaximas.cbegin()->x(), _localMaximas.cbegin()->y()));
    // For each maxima
    for (std::vector<Extrema>::iterator it = _localMaximas.begin(); it != _localMaximas.end(); ++it)
    {
        Extrema extrema = *it;
        unsigned int m = extrema.x();
        unsigned int n = extrema.y();
        float extremaValue = _bimf(m, n);
        unsigned int minK = (unsigned int)std::max(0, (int)(m - (_windowWidthMax - 1) / 2));
        unsigned int minL = (unsigned int)std::max(0, (int)(n - (_windowWidthMax - 1) / 2));
        unsigned int maxK = (unsigned int)std::min((int)(_width - 1), (int)(m + (_windowWidthMax - 1) / 2));
        unsigned int maxL = (unsigned int)std::min((int)(_height - 1), (int)(n + (_windowWidthMax - 1) / 2));
        unsigned int k = minK;
        unsigned int l = minL;

        // Loop over (m,n) neighborhood
        while (k <= maxK)
        {
            while (l <= maxL)
            {
                if (_upperEnvelope(k, l) < extremaValue)
                {
                    _upperEnvelope(k, l) = extremaValue;
                }
                ++l;
            }
            l = minL;
            ++k;
        }
    }
}

CImg<float> FABEMD::execute()
{
    // (i) Set i = 1. Take I and set S_i = I
    unsigned int i = 1;
    do
    {
        std::cout << "BIMF-" << i << std::endl;

        // (ii) Set j = 1. Set F_{T_j} = S_i.
        unsigned int j = 1;
        _bimf = _input;
        do
        {
            std::cout << "ITS-BIMF-" << i << "-" << j << std::endl;
            //-----------------------------------------------------------------
            // 3.1. Detection of local extrema
            //-----------------------------------------------------------------
            // (v) Obtain the local minima map (LMMIN) of F_{T_j}, denoted as Q_j.
            // (iii) Obtain the local maxima map (LMMAX) of F_{T_j}, denoted as P_j.
            this->buildExtremasMaps();

            //CImg<float> minimas = CImg<float>(_width, _height);
            //minimas.fill(0.0);
            //for (auto it = _localMinimas.begin(); it != _localMinimas.end(); ++it)
            //{
            //    minimas(it->x(), it->y()) = _bimf(it->x(), it->y());
            //}
            //minimas.display("Minimas");

            //CImg<float> maximas = CImg<float>(_width, _height);
            //maximas.fill(0.0);
            //for (auto it = _localMaximas.begin(); it != _localMaximas.end(); ++it)
            //{
            //    maximas(it->x(), it->y()) = _bimf(it->x(), it->y());
            //}
            //maximas.display("Maximas");

            //-----------------------------------------------------------------
            // 3.2. Generating upper and lower envelopes
            //-----------------------------------------------------------------
            // 3.2.1. Determining window size for order-statistics filters
            this->computeNearests(_localMinimas);
            std::sort(_localMinimas.begin(), _localMinimas.end(), Extrema::Greater());
            this->computeNearests(_localMaximas);
            std::sort(_localMaximas.begin(), _localMaximas.end(), Extrema::Less());
            this->computeWindowsWidths();

            // 3.2.2. Applying order statistics and smoothing filters
            // (vi) Form the lower envelope (LE) of F_{T_j}, denoted as L_{E_j} by interpolating the minima points in Q_j
            this->computeLowerEnvelope();
            // Smooth lower envelope
            CImg<float> minKernel(_windowWidthMin, _windowWidthMin);
            minKernel.fill(1.0f / (_windowWidthMin * _windowWidthMin));
            _lowerEnvelope.convolve(minKernel);



            // (iv) Form the upper envelope (UE) of F_{T_j}, denoted as U_{E_j} by interpolating the minima points in P_j
            this->computeUpperEnvelope();
            // Smooth upper envelope
            CImg<float> maxKernel(_windowWidthMax, _windowWidthMax);
            maxKernel.fill(1.0f / (_windowWidthMax * _windowWidthMax));
            _upperEnvelope.convolve(maxKernel);



            // (vii) Find the mean/average envelope (ME) as M_{E_j} = (U_{E_j} + L_{E_j}) / 2.
            _averageEnvelope = _lowerEnvelope + _upperEnvelope / 2.0;


            // (viii) Calculate F_{T_{j+1}} as F_{T_{j+1}} = F_{T_j} - M_{E_j}
            ++j;
            _bimf = _bimf - _averageEnvelope;
            _bimf.display("bimf");
            // (ix) Check whether F_{T_{j+1}} follows the BIMF properties
        } while (!this->hasBIMFProperties());

        // (x) S_i = S_{i-1} - F_{i-1}
        ++i;
        _input = _input - _bimf;



        _input.display("BIMF");
        std::cout << "Extrema count: " << this->extremaCount() << std::endl;
        // (xi) Determine whether S_i has less than three extrema points
    } while (this->extremaCount() >= 3);

    return _input;
}
