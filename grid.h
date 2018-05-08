#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <vector>
#include <iomanip>
#include <array>
#include <cassert>
#include <climits>
#include "constants.h"
#include "point.h"
#include "util.h"
#include "move.h"

using namespace std;

typedef unsigned long long ll;

class _Grid
{
public:
    virtual char get(int idx) const = 0;
    virtual bool can_move(Move m) const = 0;
    virtual char get_swap_content(Move m) const = 0;
    virtual bool move_to(Move m) = 0;
    virtual void move_back(Move m) = 0;
    virtual long long hash(long long mask) const = 0;
    virtual char get_blank() const = 0;
    virtual void move_to_with_confidence(Move m) = 0;

};

class GridFast;

class Grid2d : public _Grid
{
    Point blank;
    array<array<char, SIZE>, SIZE> data;

  public:

    char get_blank() const { return blank.x * SIZE + blank.y; }
    char get(int idx) const
    {
        return data[idx / SIZE][idx % SIZE];
    }
    char get(int x, int y) const {
        return data[x][y];
    }

    Grid2d()
    {
        for (int i = 0; i < SIZE; i++)
            for (int j = 0; j < SIZE; j++)
                data[i][j] = correctValAtPos(Point(i, j));
        blank = Point(SIZE - 1, SIZE - 1);
    }
    Grid2d(const vector<int> &in, bool human = true);
private:
    void _swap(Point p1, Point p2)
    {
        char t = data[p1.x][p1.y];
        data[p1.x][p1.y] = data[p2.x][p2.y];
        data[p2.x][p2.y] = t;
    }
    bool is_valid_point(Point p) const
    {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }

public:
    bool can_move(Move m) const
    {
        return is_valid_point(blank + dir[int(m)]);
    }
    char get_swap_content(Move m) const
    {
        Point p = blank + dir[int(m)];
        return data[p.x][p.y];
    }
    bool move_to(Move m)
    {
        Point dest = blank + dir[int(m)];
        if (!is_valid_point(dest))
            return false;
        _swap(dest, blank);
        blank = dest;
        return true;
    }
    void move_to_with_confidence(Move m) {
        Point dest = blank + dir[int(m)];
        _swap(dest, blank);
        blank = dest;
    }

    void move_back(Move m)
    {
        move_to_with_confidence(oppositeMove(m));
    }

    long long hash()
    {
        long long h = 0;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                h = (h << 4LL) | data[i][j];
            }
        }
        return h;
    }

    long long posHash() const {
        long long pos = 0;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                pos |= (ll(i * SIZE + j)) << (data[i][j] << 2LL);
            }
        }
        return pos;
    }
    long long hash(long long mask) const
    {
        return posHash() & mask;
    }
    int mp(int* bs) {
        int hash = 0;
        for (int i = 0; i < SIZE; i ++) {
            for (int j = 0; j < SIZE; j++) {
                int pos = bs[data[i][j]];
                if (pos >= 0) hash |= (data[i][j] << pos);
            }
        }
        return hash;
    }

};

#define gidx(g, x) ((g.grid >> (x << 2LL)) & 15)

#define gpos(pos,i) ((pos >> ( (i) << 2LL)) & 15)
#define ggrid(grid,i) ((grid >> ( (i) << 2LL)) & 15)
#define sgrid(grid, i, x) grid = ((grid & (~(15LL << ( (i) << 2LL)))) | ( ((ll)(x)) << ( (i) << 2LL)))
#define spos(pos, i, x) pos = ((pos & (~(15LL << ( (i) << 2LL)))) | ( ((ll)(x)) << ( (i) << 2LL)))

#define getpos(i) ((pos >> ( (i) << 2LL)) & 15)
#define getgrid(i) ((grid >> ( (i) << 2LL)) & 15)
#define setgrid(i, x) grid = ((grid & (~(15LL << ( (i) << 2LL)))) | ( ((ll)(x)) << ( (i) << 2LL)))
#define setpos(i, x) pos = ((pos & (~(15LL << ( (i) << 2LL)))) | ( ((ll)(x)) << ( (i) << 2LL)))

class GridFast : public _Grid
{
public:

    ll grid = 0;
    ll pos = 0;

    char manh;


    GridFast()
    {
        for (char i = 0; i < SIZE * SIZE; i++)
        {
            setgrid(i, i);
            assert(getgrid(i) == i);
            setpos(i, i);
        }
        // manh = 127;
    }
    GridFast(const vector<int> &constIn, bool human = true)
    {
        vector<int> in(constIn);

        if (human)
        {
            for (char i = 0; i < SIZE * SIZE; i++)
            {
                in[i] = orthogonalIndex(in[i]);
            }
        }
        for (char i = 0; i < SIZE * SIZE; i++)
        {
            setgrid(i, in[i]);
            assert(getgrid(i) == in[i]);

            setpos(in[i], i);
            assert(getpos(in[i]) == i);
        }
    }

    int manhattan_distance(int x, int y) {
        return abs( (x / 4) - (y / 4) ) + abs( (x % 4) - (y % 4));
    }
     
    bool can_move(Move move) const
    {
        char bl = getpos(BLANK_VAL);
        // {1, 0}, {0, -1}, {-1, 0}, {0, 1}
        switch (move)
        {
        case UP:
            return (bl < 12);
        case DOWN:
            return (bl > 3);
        case LEFT:
            return ((bl & 3) != 3);
        default:
            return ((bl & 3) != 0);
        }
    }
    inline void _swap(char bl_idx, char idx, char val)
    {
        setpos(BLANK_VAL, idx);
        setpos(val, bl_idx);
        setgrid(idx, BLANK_VAL);
        setgrid(bl_idx, val);
    }

    bool move_to(Move m)
    {
        if (!can_move(m))
            return false;
        move_to_with_confidence(m);
        return true;
    }

    void move_to_with_confidence(Move m) {
        char bl_idx = getpos(BLANK_VAL);
        switch (m)
        {
        case UP:
            _swap(bl_idx, bl_idx + 4, getgrid(bl_idx + 4));
            break;
        case DOWN:
            _swap(bl_idx, bl_idx - 4, getgrid(bl_idx - 4));
            break;
        case LEFT:
            _swap(bl_idx, bl_idx + 1, getgrid(bl_idx + 1));
            break;
        default:
            _swap(bl_idx, bl_idx - 1, getgrid(bl_idx - 1));
            break;
        }
    }
    void move_back(Move m)
    {
        move_to_with_confidence(oppositeMove(m));
    }

    char get_swap_content(Move m) const
    {
        char bl_idx = getpos(BLANK_VAL);
        switch (m)
        {
        case UP:
            return getgrid(bl_idx + 4);
        case DOWN:
            return getgrid(bl_idx - 4);
        case LEFT:
            return getgrid(bl_idx + 1);
        default:
            return getgrid(bl_idx - 1);
        }
    }

    char get_blank() const {
        return getpos(BLANK_VAL);
    }

    long long hash()
    {
        return grid;
    }
    long long hash(long long mask) const
    {
        //cout << " getting bitmask hash " << std::hex << pos << "&" << mask << "= " << (pos & mask) << endl;
        return pos & mask;
    }
    char get(int idx) const {
        return getgrid(idx);
    }
    char get(int x, int y) const {
        return getgrid( (x * SIZE + y) );
    }
    char getBlank() const { return getpos(BLANK_VAL); }

    long long posHash() {
        return pos;
    }

};




typedef Grid2d Grid;
typedef int h_func(_Grid &, bool toBeSolved[SIZE * SIZE]);
std::ostream &operator<<(std::ostream &stream, const _Grid &grid);
std::ostream &operator<<(std::ostream &stream, const GridFast &grid);

#endif
