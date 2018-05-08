#include "premove.h"


vector<Move> Premover::decode_move(ll code) {
        vector<Move> moves;
        for (int i = 0; i < depthCap; i++) {
            int moveInt = code & 7;
            if ((moveInt) == 7) break;
            moves.push_back(Move(moveInt));
            code = code >> 3;
        }
        reverse(moves.begin(), moves.end());
        return moves;
    }


void Premover::clr() {
    memset(bit, 0, n);
}
void Premover::upd(char pos) {
    while (pos <= n) {
        bit[(int)pos] += 1;
        pos += (pos & -pos);
    }
}

char Premover::query(char pos) {
    char s = 0;
    for (; pos > 0; pos -= (pos & -pos) ) {
        s += bit[(int)pos];
    }
    return s;
}

int Premover::nth(char* arr, int k, int n) {
    int sum = 0;
    // if the top digit is k, we add (k) * (n-1)! / (n - k) !
    // then for the next digit, we add l, l where is the # of numbers smaller than itself
    clr();
    for (int i = 0; i < k; i ++) {
        int val = arr[i];
        int rk = val - query(val);
        //cerr << arr[i] << " with rank " << rk << endl;
        sum += rk * P[n - i - 1][n - k];
        /// how many left? (n - i - 1). how many to choose? (k - i - 1). 
        upd(val + 1);
    }
    return sum;
}
