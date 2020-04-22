#ifndef CITY_HPP
#define CITY_HPP

#include <vector>
#include <complex>
#include "Item.hpp"
using namespace std;

class City {
    public:
    City() {}
    City(int i, double xval, double yval) {
        index = i;
        x = xval;
        y = yval;
        available = true;
    }
    static double euc_dist(City &me, City &other) {
        double xd = me.x - other.x;
        double yd = me.y - other.y;
        return sqrt(xd * xd + yd * yd);
    }
    static int ttp_dist(City &me, City &other) {
        return (int) ceil(euc_dist(me, other));
    }
    int index;
    bool available;
    vector <Item> items;
    double x, y;
};

#endif
