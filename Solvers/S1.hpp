#ifndef S1_HPP
#define S1_HPP

#include "../Solver.hpp"
#include "../Algorithms/CLK.hpp"
#include "../Algorithms/Packer.hpp"
#include "../Instance/Instance.hpp"
#include "../Instance/Solution.hpp"
using namespace std;

#define READALL(arr) for(auto &x : arr) { cin >> x; }
#define PRINTALL(arr) for(int i = 0; i < arr.size(); i++) { cout << arr[i] << " "; } cout << endl;

class S1 : public Solver {
    public:
    S1(Instance instance) : Solver(instance) {}
    Solution solve(string tourfile="") override {
        vector<int> tour;
        if(tourfile == "") tour = CLK::generateUsingLinkern(instance);
        else tour = CLK::readTour(tourfile);
        vector<bool> packing = Packer::packIteratively(instance, tour, 5, 2.5, 20);
        Solution sol(tour, packing);
        sol.objective = Solution::calculateObjective(instance, sol);
        return sol;
    }
};

#endif
