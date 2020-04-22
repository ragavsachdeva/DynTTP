#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <map>
#include <vector>
#include <exception>
#include <iostream>
#include "City.hpp"
#include "Item.hpp"
#include "../Eval.hpp"
using namespace std;

// Statics from Eval::ProblemTTP
using EvalCity = ::EvalCity;
using EvalItem = ::EvalItem;
vector <EvalCity> ProblemTTP::cities;
size_t ProblemTTP::n;
size_t ProblemTTP::mItems;
long long ProblemTTP::maxWeight;
double ProblemTTP::r;
double ProblemTTP::minSpeed;
double ProblemTTP::maxSpeed;
map <int,int> ProblemTTP::itemsAll;

class Instance {
    public:
    Instance() {}
    Instance(string filename) {
        file = filename;
        if (!ProblemTTP::readTTPInstance(filename)) {
            throw std::runtime_error("Could not open file");
        }
        cityCount = ProblemTTP::n;
        itemCount = ProblemTTP::mItems;
        rentingRate = ProblemTTP::r;
        minSpeed = ProblemTTP::minSpeed;
        maxSpeed = ProblemTTP::maxSpeed;
        maxWeight = ProblemTTP::maxWeight;
        cities = vector<City>(cityCount);
        items = vector<Item>(itemCount);
        for (auto c : ProblemTTP::cities) {
            cities[c.index - 1] = City(c.index - 1, c.positionX, c.positionY);
            for (auto i : c.items) {
                cities[c.index - 1].items.emplace_back(i.index - 1, i.value, i.weight, c.index - 1);
                items[i.index - 1] = Item(i.index - 1, i.value, i.weight, c.index - 1);
            }
        }
    }

    void citiesToFile(string toOpen = "temp.txt") {
        FILE* fp = fopen(toOpen.c_str(),"w");
        int dimension = 0; for (auto city : cities) if (city.available) dimension++;
        fputs(("DIMENSION: "+ to_string(dimension) + "\n").c_str(), fp); 
        // cout << "DIMENSION: " << dimension << endl;
        fputs("EDGE_WEIGHT_TYPE: CEIL_2D\n", fp); 
        // cout << "EDGE_WEIGHT_TYPE: CEIL_2D" << endl;
        fputs("NODE_COORD_SECTION	(INDEX, X, Y):\n", fp);
        // cout << "NODE_COORD_SECTION	(INDEX, X, Y):" << endl;
        int i = 1;
        for(auto city : cities) {
            if (city.available) {
                fputs((to_string(i++) + " " + to_string(int(city.x)) + " " + to_string(int(city.y))+ "\n").c_str(), fp);
                // cout << i++ << " " << city.x << " " << city.y << endl;
            }
        }
        fclose(fp);
    }
    string file;            // Path to instance file
    vector <City> cities;   // Set of cities in the instance
    vector <Item> items;    // Set of items in the instance
    int cityCount;       // Number of cities in the instance
    int itemCount;       // Number of items in the instance
    double rentingRate;     // Renting rate
    double minSpeed;        // Minimal speed
    double maxSpeed;        // Maximal speed
    long long maxWeight;    // Maximal knapsack's weight

};

#endif