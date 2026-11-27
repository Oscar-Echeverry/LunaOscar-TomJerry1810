#ifndef VECTOR2_H
#define VECTOR2_H
#include <cmath>

using namespace std;

struct Vector2 {
    double x = 0;
    double y = 0;

    double distancia(const Vector2 &otro) const
    {
        return hypot(x - otro.x, y - otro.y);
    }
};
#endif
