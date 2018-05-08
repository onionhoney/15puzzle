#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "point.h"

const int FOUND = -1;
const int SIZE = 4;
const int BLANK_VAL = SIZE * SIZE - 1;
    const double TIMEOUT_THRESH = 20.0;
    const int BENCH_SIZE = 200;

const Point dir[4] = {
    {1, 0},
    {0, -1},
    {-1, 0},
    {0, 1}
};

const char MIRROR[16] = {
    0, 4, 8, 12, 
    1, 5, 9, 13, 
    2, 6, 10, 14, 
    3, 7, 11, 15 
};
#endif
