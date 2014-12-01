#include "Extrema.h"

Extrema::Extrema(unsigned int x, unsigned int y)
{
    _x = x;
    _y = y;
    _distance = std::numeric_limits<float>::infinity();
}

float Extrema::distanceTo(const Extrema &extrema) const
{
    unsigned int dx = this->x() - extrema.x();
    unsigned int dy = this->y() - extrema.y();
    return (float) sqrt(dx * dx + dy * dy);
}
