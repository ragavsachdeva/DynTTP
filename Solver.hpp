#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <numeric>
#include "Instance/Instance.hpp"
#include "Instance/Solution.hpp"
using namespace std;

#define PRINTALL(arr) for(int i = 0; i < arr.size(); i++) { cout << arr[i] << " "; } cout << endl;

class Solver {
    public:
    Solver() {}
    Solver(Instance _instance) {
        instance = _instance;
    }
    virtual Solution solve(string tourfile="")=0;
    Instance instance;
};

#endif