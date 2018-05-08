#ifndef UTIL_H
#define UTIL_H

#include "point.h"
#include "constants.h"

Point correctPosOfVal(int value);

int correctValAtPos(Point p);

int orthogonalIndex(int x);

inline int nthcell(long long hash, int n) {
    n = 15 - n;
    return (hash >> (n >> 2)) & 15;
}
#endif
