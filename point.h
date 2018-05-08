#ifndef POINT_H
#define POINT_H

struct Point {
    int x, y;
    Point(int x, int y): x(x), y(y) {}
    Point(): x(0), y(0) {}
    Point operator+ (const Point& other) const{
        return Point(x + other.x, y + other.y);
    }
    bool operator== (const Point& other) const {
        return x == other.x && y == other.y;
    }
};

#endif