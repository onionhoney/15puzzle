#ifndef MOVE_H
#define MOVE_H

#include <iostream>
#include <vector>
using namespace std;

enum Move {
    UP = 0, 
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3,
    OTHER
};
Move oppositeMove(Move m);
std::ostream& operator<< (std::ostream& stream, const vector<Move>& moves);

#endif
