#include "util.h"
Point correctPosOfVal(int value) {
    return Point( (value) / SIZE, (value) % SIZE);
}

int correctValAtPos(Point p) {
        return p.x * SIZE + p.y;
}

int orthogonalIndex(int x) {
    return (x == 0) ? BLANK_VAL : x - 1;
}