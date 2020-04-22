#ifndef ReOpter_HPP
#define ReOpter_HPP

#include <vector>
#include "../Instance/Instance.hpp"
#include "../Instance/Solution.hpp"
#include "../Stats.hpp"
#include "Optimiser.hpp"
#include "FRVSD.hpp"
using namespace std;

#define TIMELIMIT 600000

class ReOpter {
    public:
    static Solution viaBitflip(Instance &instance, Solution sol) {
        Timer::start();
        sol.objective = Solution::calculateObjective(instance, sol); Solution::evalCount -= 1;
        double lastObjective;
        do {
            lastObjective = sol.objective;
            sol = Optimiser::bitflip(instance, sol, true);
        } while(sol.objective != lastObjective && Timer::elasped() < TIMELIMIT && !disrupt());
        return sol;
    }
    static Solution viaJump(Instance &instance, Solution sol) {
        Timer::start();
        sol.objective = Solution::calculateObjective(instance, sol); Solution::evalCount -= 1;
        double lastObjective;
        do {
            lastObjective = sol.objective;
            sol = Optimiser::jump(instance, sol, false);
        } while(sol.objective != lastObjective && Timer::elasped() < TIMELIMIT && !disrupt());
        return sol;
    }
    static Solution viaPackIterative(Instance &instance, Solution sol) {
        sol.items = Packer::packIteratively(instance, sol.tour, 5, 2.5, 20);
        sol.objective = Solution::calculateObjective(instance, sol); Solution::evalCount -= 1;
        Experiment::addDataPoint(Solution::evalCount, sol.objective);
        return sol;
    }
    static Solution viaCLK(Instance &instance, Solution sol, string toOpen = "temp.txt") {
        // dropKItems would write an instance to toOpen file. Read from that and save a tour in tour_toOpen..
        system(("./Algorithms/linkern-macos -s 0 -o tempfiles/tour_" + toOpen + " " + "tempfiles/"+toOpen + " > tempfiles/garbage.txt").c_str());
        // read the new tour;
        freopen(("tempfiles/tour_"+toOpen).c_str(), "r", stdin); // TODO: change the way of reading from a file
        int n; int ignore; cin >> n; cin >> ignore;
        vector<int> tour(n);
        for(auto &x : tour) { cin >> x; cin >> ignore; cin >> ignore;}
        // map the indices of the new tour to actual city indicies
        vector<int> newToOldIndex(n);
        int newIndex = 0; int oldIndex = 0;
        for(auto city : instance.cities) {
            if (city.available) newToOldIndex[newIndex++] = city.index;
            oldIndex++;
        }
        int availIndex = 0;
        for(int i = 0; i < sol.tour.size(); i++) {
            // for if an item in the current tour is not available, don't touch it
            // otherwise update the current index in sol.tour with next item in the new tour.
            if (instance.cities[sol.tour[i]].available) sol.tour[i] = newToOldIndex[tour[availIndex++]];
        }
        sol.objective = Solution::calculateObjective(instance, sol);
        Experiment::addDataPoint(Solution::evalCount, sol.objective);
        return sol;
    }

    static Solution viaCLKAndJump(Instance &instance, Solution sol, string toOpen = "temp.txt") {
        sol = ReOpter::viaCLK(instance, sol, toOpen);
        return ReOpter::viaJump(instance, sol);
    }

    static Solution viaPackIterativeAndBitflip(Instance &instance, Solution sol) {
        sol.items = Packer::packIteratively(instance, sol.tour, 5, 2.5, 20);
        sol.objective = Solution::calculateObjective(instance, sol); Solution::evalCount -= 1;
        Experiment::addDataPoint(Solution::evalCount, sol.objective);
        if (disrupt()) return sol;
        return ReOpter::viaBitflip(instance, sol);
    }
    static Solution viaStructuralDiversity(Instance &instance, Solution sol) {
        return FRVSD::REA(instance, sol);
    }


};

#endif