#ifndef HEUR_LC_H
#define HEUR_LC_H

#include "grid.h"
#include "point.h"
#include "constants.h"
#include "heur.h"
#include <vector>
#include "heur_lc.h"

class LinearConflict : public Heuristics {
public:
int global_manh[SIZE*SIZE][SIZE*SIZE];
int toBeSolved[SIZE*SIZE];
bool initialized = false;

void init() {
    if (initialized) return;
    initialized = true;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            for (int i1 = 0; i1 < SIZE; i1++) {
                for (int j1 = 0; j1 < SIZE; j1++) {
                    global_manh[i*SIZE+j][i1*SIZE+j1] = manhattan_distance_calc(Point(i,j), Point(i1, j1));
                }
            }
        }
    }
}



LinearConflict(std::vector<int>& pieces) {
    for (int i = 0; i < SIZE * SIZE; i++) {
        if (find(pieces.begin(), pieces.end(), i) != pieces.end()) {
            this->toBeSolved[i] = true;
        } else {
            this->toBeSolved[i] = false;
        }
    }

}


int manhattan_distance_calc(Point a, Point b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}
int manhattan_distance(int i1, int i2) {
    return global_manh[i1][i2];
    //return abs(i1 / 4 - i2 / 4) + abs(i1 % 4 - i2 % 4);
}

int h(Grid& g)
{
    int d = 0;
    // assume g is orthonized
    
    for (int i = 0; i < SIZE * SIZE; i++) {
        char tile = g.get(i);
        if (tile == BLANK_VAL || !toBeSolved[tile]) continue;
        d += manhattan_distance(i, tile);
    }

    for (int i = 0; i < SIZE * SIZE; i += SIZE) {
        int row = i >> 2;
        for (int j1 = i; j1 < i + SIZE - 1; j1++) {
            for (int j2 = j1 + 1; j2 < i + SIZE; j2++) {
                char idx1 = g.get(j1), idx2 = g.get(j2);
                if (idx1 == BLANK_VAL || idx2 == BLANK_VAL || !toBeSolved[idx1] || !toBeSolved[idx2]) continue;
                if ( ((idx1 >> 2) == row) && ((idx2 >> 2) == row )
                && (( idx1 & 3 ) > ( idx2 & 3) )) {
                    d += 2;
                }
            }
        }
    }
    
    for (int i = 0; i < SIZE; i++) {
        int col = i;
        for (int j1 = i; j1 < SIZE * SIZE; j1 += SIZE) {
            for (int j2 = j1 + SIZE; j2 < SIZE * SIZE; j2 += SIZE) {
                char idx1 = g.get(j1), idx2 = g.get(j2);
                if (idx1 == BLANK_VAL || idx2 == BLANK_VAL || !toBeSolved[idx1] || !toBeSolved[idx2]) continue;
                if ( ((idx1 & 3) == col) && ((idx2 & 3) == col )
                && (( idx1 >> 2 ) > ( idx2 >> 2 ) )) {
                    d += 2;
                }
            }
        }
    }

    return d;
}

};

#endif
