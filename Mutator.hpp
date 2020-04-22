#if __APPLE__
#include "bits_stdc++.h"
#else
#include <bits/stdc++.h>
#endif

#include "Instance/Instance.hpp"
#include "Algorithms/CLK.hpp"
#include "Algorithms/Packer.hpp"
#include "Algorithms/Optimiser.hpp"
#include "Algorithms/ReOpter.hpp"
#include "Solver.hpp"
#include "Solvers/S1.hpp"
#include "Solvers/S2.hpp"
#include "Solvers/S4.hpp"
#include "Solvers/S6.hpp"
#include "Timer.hpp"
#include "Stats.hpp"
using namespace std;

class Mutator {
    public:
    Mutator(Instance _instance, Solution _sol) {
        instance = _instance;
        soln = _sol;
    }

    void dropAndCompute(Solver *solver, string mutation, double dropPercent, string reOpter, string tempFile = "temp.txt", int times = 1) {
        Instance mutatedInstance = instance;
        Solution sol = soln;
        Solution reOptimisedSoln;
        
        int t = times;
        while (times--) {
            if (mutation == "items") {
                int k = dropPercent * instance.itemCount;
                sol.dropKItems(mutatedInstance, k);
            } else if (mutation == "cities") {
                int k = dropPercent * instance.cityCount;
                sol.dropKCities(mutatedInstance, k, "tempfiles/"+tempFile);
            } else {
                cout << "INVALID mutation" << endl;
                return;
            }
            Experiment::startNewPhase();
            Solution::evalCount = 0;
            Experiment::addDataPoint(Solution::evalCount, sol.objective);
            if (reOpter == "viaBitflip") { // reoptimised mutated
                reOptimisedSoln = ReOpter::viaBitflip(mutatedInstance, sol);
            } else if (reOpter == "viaPackIterative") { // from scratch
                reOptimisedSoln = ReOpter::viaPackIterative(mutatedInstance, sol);
            } else if (reOpter == "viaPackIterativeAndBitflip") { // from scratch
                reOptimisedSoln = ReOpter::viaPackIterativeAndBitflip(mutatedInstance, sol);
            } else if (reOpter == "viaStructuralDiversity") { // reoptimised mutated
                reOptimisedSoln = ReOpter::viaStructuralDiversity(mutatedInstance, sol);
            } else if (reOpter == "viaCLK") {
                // fixed seed for linkern across multiple drops
                reOptimisedSoln = ReOpter::viaCLK(mutatedInstance, sol, tempFile);
            } else if (reOpter == "viaJump") {
                reOptimisedSoln = ReOpter::viaJump(mutatedInstance, sol);
            } else if (reOpter == "viaCLKAndJump") {
                reOptimisedSoln = ReOpter::viaCLKAndJump(mutatedInstance, sol, tempFile);
            } else {
                cout << "INVALID reOpter" << endl;
                return;
            }
            sol = reOptimisedSoln;
        }
    }

    Instance instance;
    Solution soln;
};
