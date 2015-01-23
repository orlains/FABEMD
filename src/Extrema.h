#ifndef __EXTREMA_H__
#define __EXTREMA_H__

#include <algorithm>
#include <limits>
#include <cmath>

class Extrema
{
public:
    struct Less
    {
        bool operator() (const Extrema & a, const Extrema & b) const
        {
            return a.distance() < b.distance();
        }
    };

    struct Greater
    {
        bool operator() (const Extrema & a, const Extrema & b) const
        {
            return a.distance() > b.distance();
        }
    };

private:
    int _x;
    int _y;
    float _distance;

public:
    Extrema(unsigned int x = 0, unsigned int y = 0);

    unsigned int x() const { return this->_x; }
    unsigned int y() const { return this->_y; }
    float distance() const { return this->_distance; }

    void setX(unsigned int x) { this->_x = x; }
    void setY(unsigned int y) { this->_y = y; }
    void setDistance(float distance) { this->_distance = distance; }

    float distanceTo(const Extrema & extrema) const;
};

#endif // __EXTREMA_H__
