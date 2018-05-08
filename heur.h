#ifndef HEUR_H
#define HEUR_H

#include "grid.h"
#include "constants.h"
class Heuristics {
public:
    virtual int h(Grid& g) = 0;
    virtual void init() = 0;
};


#endif