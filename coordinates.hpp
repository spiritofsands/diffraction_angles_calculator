#ifndef COORDINATES_H
#define COORDINATES_H

#include <iostream>
#include <iomanip>

using namespace std;

class Coordinates
{
public:
    Coordinates(double new_x = 0.0, double new_y = 0.0)
     : x(new_x), y(new_y)
    {};

    Coordinates(const Coordinates &c)
     : x(c.x), y(c.y)
    {};

    double x;
    double y;
};

class PairOfCoords
{
    friend ostream &operator<<( ostream &out, const PairOfCoords &p )
    {
        out << setprecision(6)
            << "(" << p.first.x << ", " << p.first.y << ")\t"
               "(" << p.second.x << ", " << p.second.y << ")";
        return out;
    }

public:
    PairOfCoords(double x1 = 0.0, double y1 = 0.0,
                 double x2 = 0.0, double y2 = 0.0)
     : first(x1, y1),
       second(x2, y2)
    {};

    PairOfCoords(Coordinates new_first, Coordinates new_second)
     : first(new_first),
       second(new_second)
    {};

    const PairOfCoords &operator= (const PairOfCoords &p)
    {
        first = p.first;
        second = p.second;
        return *this;
    }

    //constructor-like initialisation
    void operator() (double x1, double y1,
                    double x2, double y2)
    {
        first.x = x1;
        first.y = y1;
        second.x = x2;
        second.y = y2;
    }

    Coordinates first;
    Coordinates second;
};

#endif // COORDINATES_H

