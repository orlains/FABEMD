#include "FABEMD.h"

using namespace cimg_library;

/**
 * @brief Prepare a Fast and Adaptive Bidimensional Empirical Mode Decomposition of given image.
 * Algorithm can be executed using the execute() method.
 * @param input Source image
 * @param osfwType Order statistics filter width type
 * @param maximumAllowableIterations Maximal number of BIMC-ITS for the computation of a BIMC
 * @param size Size of the extrema search window
 * @param thredshold Maximal standard variation thredshold to get to next BIMC
 */
FABEMD::FABEMD(const CImg<float> & input, 
    OSFW osfwType, 
    unsigned int maximumAllowableIterations, 
    unsigned int size, 
    float threshold)
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
    _maximumAllowableIterations = maximumAllowableIterations;
    _osfwType = osfwType;
}

/**
 * @brief Build the maps of minimas and extremas of this image.
 */
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
                if (k != (unsigned int)m || l != (unsigned int)n)
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

/**
 * @brief Assign the minimal distance to another extrema for each extrema of given map.
 * @param extremas Extrema map.
 */
void FABEMD::assignNearests(std::vector<Extrema> & extremas)
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

/**
 * @brief Get standard deviation of F_{T_{j+1}}.
 * @return Standard deviation of F_{T_{j+1}}.
 */
float FABEMD::standardDeviation()
{
    float meValue = 0.0;
    float ftjValue = 0.0;
    cimg_forXY(_averageEnvelope, x, y)
    {
        meValue += _averageEnvelope(x, y) * _averageEnvelope(x, y);
        ftjValue += _bimf(x, y) * _bimf(x, y);
    }

    return meValue / ftjValue;
}

/**
 * @brief Get current extrema count.
 * @return Current extrema count.
 */
unsigned int FABEMD::extremaCount()
{
    return (unsigned int)(_localMinimas.size() + _localMaximas.size());
}

/**
 * @brief Compute order statistics filter widths.
 * The values assigned depend on fabemd's osfw type. Possible values are the following:
 * - SAME_TYPE_1: w_{en-g} = minimum{minimum{d_{adj-max}, minimum{d_{adj-min}}
 * - SAME_TYPE_2: w_{en-g} = maximum{minimum{d_{adj-max}, minimum{d_{adj-min}}
 * - SAME_TYPE_3: w_{en-g} = minimum{maximum{d_{adj-max}, maximum{d_{adj-min}}
 * - SAME_TYPE_4: w_{en-g} = maximum{maximum{d_{adj-max}, maximum{d_{adj-min}}
 * - DIFFERENT_TYPE_1: w_{minen-g} = minimum{d_{adj-min}
 *                     w_{maxen-g} = minimum{d_{adj-max}
 * - DIFFERENT_TYPE_2: w_{minen-g} = minimum{d_{adj-min}
 *                     w_{maxen-g} = maximum{d_{adj-max}
 * - DIFFERENT_TYPE_3: w_{minen-g} = maximum{d_{adj-min}
 *                     w_{maxen-g} = minimum{d_{adj-max}
 * - DIFFERENT_TYPE_4: w_{minen-g} = maximum{d_{adj-min}
 *                     w_{maxen-g} = maximum{d_{adj-max}
 */
void FABEMD::computeFiltersWidths()
{
    switch (_osfwType)
    {
    case SAME_TYPE_1:
        _windowWidthMin = (unsigned int)std::min(
            _localMinimas.rbegin()->distance(),
            _localMaximas.begin()->distance());
        _windowWidthMax = _windowWidthMin;
        break;
    case SAME_TYPE_2:
        _windowWidthMin = (unsigned int)std::max(
            _localMinimas.rbegin()->distance(),
            _localMaximas.begin()->distance());
        _windowWidthMax = _windowWidthMin;
        break;
    case SAME_TYPE_3:
        _windowWidthMin = (unsigned int)std::min(
            _localMinimas.begin()->distance(),
            _localMaximas.rbegin()->distance());
        _windowWidthMax = _windowWidthMin;
        break;
    case SAME_TYPE_4:
        _windowWidthMin = (unsigned int)std::max(
            _localMinimas.begin()->distance(),
            _localMaximas.rbegin()->distance());
        _windowWidthMax = _windowWidthMin;
        break;
    case DIFFERENT_TYPE_1:
        _windowWidthMin = (unsigned int)_localMinimas.rbegin()->distance();
        _windowWidthMax = (unsigned int)_localMaximas.begin()->distance();
        break;
    case DIFFERENT_TYPE_2:
        _windowWidthMin = (unsigned int)_localMinimas.rbegin()->distance();
        _windowWidthMax = (unsigned int)_localMaximas.rbegin()->distance();
        break;
    case DIFFERENT_TYPE_3:
        _windowWidthMin = (unsigned int)_localMinimas.begin()->distance();
        _windowWidthMax = (unsigned int)_localMaximas.begin()->distance();
        break;
    case DIFFERENT_TYPE_4:
        _windowWidthMin = (unsigned int)_localMinimas.begin()->distance();
        _windowWidthMax = (unsigned int)_localMaximas.rbegin()->distance();
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

/**
 * @brief Compute lower envelope.
 */
void FABEMD::computeLowerEnvelope()
{
    cimg_forXY(_lowerEnvelope, m, n)
    {
        unsigned int minK = (unsigned int)std::max(0, (int)(m - (_windowWidthMin - 1) / 2));
        unsigned int minL = (unsigned int)std::max(0, (int)(n - (_windowWidthMin - 1) / 2));
        unsigned int maxK = (unsigned int)std::min((int)(_width - 1), (int)(m + (_windowWidthMin - 1) / 2));
        unsigned int maxL = (unsigned int)std::min((int)(_height - 1), (int)(n + (_windowWidthMin - 1) / 2));
        unsigned int k = minK;
        unsigned int l = minL;
        float value = std::numeric_limits<float>::infinity();

        // Loop over (m,n) neighborhood
        while (k <= maxK)
        {
            while (l <= maxL)
            {
                if (_bimf(k, l) < value)
                {
                    value = _bimf(k, l);
                }
                ++l;
            }
            l = minL;
            ++k;
        }
        _lowerEnvelope(m, n) = value;
    }
}

/**
 * @brief Compute upper enveloppe.
 */
void FABEMD::computeUpperEnvelope()
{
    cimg_forXY(_upperEnvelope, m, n)
    {
        unsigned int minK = (unsigned int)std::max(0, (int)(m - (_windowWidthMax - 1) / 2));
        unsigned int minL = (unsigned int)std::max(0, (int)(n - (_windowWidthMax - 1) / 2));
        unsigned int maxK = (unsigned int)std::min((int)(_width - 1), (int)(m + (_windowWidthMax - 1) / 2));
        unsigned int maxL = (unsigned int)std::min((int)(_height - 1), (int)(n + (_windowWidthMax - 1) / 2));
        unsigned int k = minK;
        unsigned int l = minL;
        float value = -std::numeric_limits<float>::infinity();

        // Loop over (m,n) neighborhood
        while (k <= maxK)
        {
            while (l <= maxL)
            {
                if (_bimf(k, l) > value)
                {
                    value = _bimf(k, l);
                }
                ++l;
            }
            l = minL;
            ++k;
        }
        _upperEnvelope(m, n) = value;
    }
}

/**
 * @brief Execute computation of BEMC and residue.
 * @return Image composed of the following slices :
 * - The original image
 * - Every computed BEMC
 * - The residue
 */
CImg<float> FABEMD::execute()
{
    CImg<float> display = CImg<float>(_input);

    // (i) Set i = 1. Take I and set S_i = I
    unsigned int i = 1;
    do
    {
        // (ii) Set j = 1. Set F_{T_j} = S_i.
        unsigned int j = 1;
        _bimf = _input;
        do
        {
            //-----------------------------------------------------------------
            // 3.1. Detection of local extrema
            //-----------------------------------------------------------------
            // (v) Obtain the local minima map (LMMIN) of F_{T_j}, denoted as Q_j.
            // (iii) Obtain the local maxima map (LMMAX) of F_{T_j}, denoted as P_j.
            buildExtremasMaps();

            // Exit if previous created BEMC had less than 3 extremas
            if (extremaCount() < 3)
            {
                std::cout << "BIMF has less than 3 extremas" << std::endl;
                return display;
            }

            // The order statistics filters are based on maxima and minima maps of a S_i image (if j equals 1)
            if (j == 1)
            {
                //-----------------------------------------------------------------
                // 3.2. Generating upper and lower envelopes
                //-----------------------------------------------------------------
                // 3.2.1. Determining window size for order-statistics filters
                assignNearests(_localMinimas);
                std::sort(_localMinimas.begin(), _localMinimas.end(), Extrema::Greater());
                assignNearests(_localMaximas);
                std::sort(_localMaximas.begin(), _localMaximas.end(), Extrema::Less());
                computeFiltersWidths();

                // Create smoothing kernels
                _lowerKernel = CImg<float>(_windowWidthMin, _windowWidthMin, 1, 1,
                    1.0f / (_windowWidthMin * _windowWidthMin));
                _upperKernel = CImg<float>(_windowWidthMax, _windowWidthMax, 1, 1,
                    1.0f / (_windowWidthMax * _windowWidthMax));
            }

            // 3.2.2. Applying order statistics and smoothing filters
            // (vi) Form the lower envelope (LE) of F_{T_j}, denoted as L_{E_j} by interpolating the minima points in Q_j
            computeLowerEnvelope();
            // Smooth lower envelope
            _lowerEnvelope.convolve(_lowerKernel);

            // (iv) Form the upper envelope (UE) of F_{T_j}, denoted as U_{E_j} by interpolating the minima points in P_j
            computeUpperEnvelope();
            // Smooth upper envelope
            _upperEnvelope.convolve(_upperKernel);

            // (vii) Find the mean/average envelope (ME) as M_{E_j} = (U_{E_j} + L_{E_j}) / 2.
            _averageEnvelope = (_lowerEnvelope + _upperEnvelope) / 2.0;

            // Compute variance of F_{T_{j+1}}
            _variance = standardDeviation();
            std::cout << "ITS-BIMF-" << i << "-" << j << ": " << "variance of " << _variance << "." << std::endl;

            // (viii) Calculate F_{T_{j+1}} as F_{T_{j+1}} = F_{T_j} - M_{E_j}
            ++j;
            _bimf -= _averageEnvelope;

            // (ix) Check whether F_{T_{j+1}} follows the BIMF properties
        } while (_variance > _threshold && j <= _maximumAllowableIterations);

        std::cout << "BIMF-" << i << ": " << extremaCount() << " extremas." << std::endl;
        ++i;

        // (x) S_i = S_{i-1} - F_{i-1}
        _input -= _bimf;

        // Add BEMC (or residue) to output
        display.append(CImg<float>(_bimf), 'z');

        // (xi) Determine whether S_i has less than three extrema points
    } while (extremaCount() >= 3);

    return display;
}
