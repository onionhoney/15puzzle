#include "wd.h"

void WalkingDist::init()
{
    int i, j, k, nextd;
    ll table;
    const char *filename = this->filename.c_str();
    FILE *fp;

    /* WDPTN[], WDTBL[], WDLNK[][][] */
    fp = fopen(filename, "rb");
    for (i = 0; i < WDTBL_SIZE; i++)
    {
        /* WDPTN */
        table = 0;
        for (j = 0; j < 8; j++)
            table = (table << 8) | getc(fp);
        WDPTN[i] = table;
        /* WDTBL */
        WDTBL[i] = (char)getc(fp);
        /* WDLNK */
        for (j = 0; j < 2; j++)
            for (k = 0; k < 4; k++)
            {
                nextd = getc(fp);
                WDLNK[i][j][k] = (short)((nextd << 8) | getc(fp));
            }
    }
    fclose(fp);

    for (i = 0; i < WDTBL_SIZE; i++) {
        hash[WDPTN[i]] = i;
    }
    cout << "successfully initialized WD" << endl;
}

int WalkingDist::h(Grid& g, int& idx1, int& idx2) {
    long long table = 0;
    int i, j;
    int work[BOARD_WIDTH];
    int  num1, num2;

    for (i=0; i<BOARD_WIDTH; i++) {
        for (j=0; j<BOARD_WIDTH; j++) work[j] = 0;
        for (j=0; j<BOARD_WIDTH; j++) {
            num1 = g.get(i, j);
            if (num1 == BLANK_VAL) continue;
            work[num1 >> 2]++;
        }
        for (j=0; j<BOARD_WIDTH; j++)
            table = (table << 3) | work[j];
    }
    idx1 = hash[table];

    table = 0;
    for (i=0; i<BOARD_WIDTH; i++) {
        for (j=0; j<BOARD_WIDTH; j++) work[j] = 0;
        for (j=0; j<BOARD_WIDTH; j++) {
            num2 = MIRROR[g.get(j, i)];
            if (num2 == BLANK_VAL) continue;
            work[num2  >> 2]++;
        }
        for (j=0; j<BOARD_WIDTH; j++)
            table = (table << 3) | work[j];
    }
    idx2 = hash[table];

    //cout << "successfully initialized WD" << endl;

    return WDTBL[idx1] + WDTBL[idx2];
}

int WalkingDist::h(Grid& g) {
    int idx1, idx2;
    return h(g, idx1, idx2);
}

int WalkingDist::h(Grid& g, int& idx1, int& idx2, Move dir, char pieceToMove) {

    //cout << "pre upd = " << idx1 << " " << idx2 << endl;
    char n = pieceToMove;
    switch (dir) {
        case UP: idx1 = WDLNK[idx1][0][n >> 2]; break;
        case DOWN: idx1 = WDLNK[idx1][1][n >> 2]; break;
        case LEFT: idx2 = WDLNK[idx2][0][MIRROR[n] >> 2]; break;
        case RIGHT: idx2 = WDLNK[idx2][1][MIRROR[n] >> 2]; break;
        default: break;
    }
    //cout << "upd wd = " << (int)WDTBL[idx1] << " " << (int)WDTBL[idx2] << endl;
    //cout << "post upd = " << idx1 << " " << idx2 << endl;

    return WDTBL[idx1] + WDTBL[idx2];
}
