#include "move.h"
std::ostream& operator<< (std::ostream& stream, const vector<Move>& moves) {
    for (Move m : moves) {
        if (int(m) >= 0)
        cout << "URDL"[int(m)] << " ";
    }
    return stream;
}

Move oppositeMove(Move m) {
    switch (m) {
        case UP: return DOWN;
        case DOWN: return UP;
        case LEFT: return RIGHT;
        case RIGHT: return LEFT;
        default:
            return OTHER;
    }
}
