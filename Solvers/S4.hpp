#ifndef S4_HPP
#define S4_HPP

#include "../Solver.hpp"
#include "../Algorithms/CLK.hpp"
#include "../Algorithms/Packer.hpp"
#include "../Algorithms/Optimiser.hpp"
#include "../Instance/Instance.hpp"
#include "../Instance/Solution.hpp"
#include "../Timer.hpp"
#include "../Stats.hpp"

using namespace std;

#define TIMELIMIT 600000
#define READALL(arr) for(auto &x : arr) { cin >> x; }
#define PRINTALL(arr) for(int i = 0; i < arr.size(); i++) { cout << arr[i] << " "; } cout << endl;

class S4 : public Solver {
    public:
    S4(Instance instance) : Solver(instance) {}
    Solution solve(string tourfile="") override {
        vector<int> tour;
        if(tourfile == "") tour = CLK::generateUsingLinkern(instance);
        else tour = CLK::readTour(tourfile);

        Timer::start();
        vector<bool> packing = Packer::packIteratively(instance, tour, 5, 2.5, 20);
        Solution sol(tour, packing);
        sol.objective = Solution::calculateObjective(instance, sol);
        double lastObjective;
        do {
            lastObjective = sol.objective;
            Experiment::addDataPoint(Solution::evalCount, lastObjective);
            sol = Optimiser::jump(instance, sol);
        } while(sol.objective != lastObjective && Timer::elasped() < TIMELIMIT);
        return sol;
    }
};

#endif
