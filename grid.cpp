#include "grid.h"
#include <iomanip>

std::ostream &operator<<(std::ostream &stream, const _Grid &grid)
{
    stream << endl;
    char p = grid.get_blank();
    stream << "blank at " << p / 4 << " " << p % 4 << endl;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            stream << std::setw(4);
            stream << (int)grid.get(i * SIZE + j);
        }
        if (i < SIZE - 1)
            stream << endl;
    }
    return stream;
}

std::ostream &operator<<(std::ostream &cout, const GridFast &g)
{
    cout << "Grid ";
    long long grid = g.grid;
    long long pos = g.pos;

    for (int i = 0; i < 16; i++)
    {
        cout << setw(4) << i;
    }
    cout << endl;
    cout << "     ";
    for (int i = 0; i < 16; i++)
    {
        cout << setw(4) << getgrid(i);
    }
    cout << endl;
    cout << "Pos ";
    for (int i = 0; i < 16; i++)
    {
        cout << setw(4) << i;
    }
    cout << endl;
    cout << "    ";
    for (int i = 0; i < 16; i++)
    {
        cout << setw(4) << getpos(i);
    }
    cout << endl;
    return cout;
}

Grid2d::Grid2d(const vector<int> &in, bool human)
{
    int foundBlank = 0;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            data[i][j] = (human ? orthogonalIndex(in[i * SIZE + j]) : in[i * SIZE + j]);
            if (data[i][j] == BLANK_VAL)
            {
                blank = Point(i, j);
                foundBlank++;
            }
        }
    }
    assert(foundBlank == 1);

    GridFast g(in, human);
    assert(g.hash(INT64_MIN) == hash(INT64_MIN));
    assert(g.pos == posHash());
}