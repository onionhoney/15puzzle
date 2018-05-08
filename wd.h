#ifndef WALKING_DIST_H
#define WALKING_DIST_H

#include <unordered_map>

#include "grid.h"
#include "constants.h"
#include "heur.h"

#define  WDTBL_SIZE  24964 /* WalkingDistance TableSize */
#define  IDTBL_SIZE    106 /* InvertDistance TableSize  */

#define BOARD_WIDTH SIZE




class WalkingDist : public Heuristics{
public:
    const string filename = "puz15wd.db";

    ll   WDPTN[WDTBL_SIZE];        /* パターンテーブル */
    short WDLNK[WDTBL_SIZE][2][BOARD_WIDTH];  /* リンク */
    char  WDTBL[WDTBL_SIZE];        /* WD算出テーブル   */
    int   DEPTH;                    /* 探索の深さ制限値 */

    unordered_map<long long, int> hash;

    WalkingDist() {
        init();


    }
    void init() ;

    int h(Grid& g);
    int h(Grid& g, int& idx1, int& idx2, Move dir, char pieceToMove);
    int h(Grid& g, int& idx1, int& idx2);

};

#endif 