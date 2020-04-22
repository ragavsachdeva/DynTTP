#ifndef FRVSD_HPP
#define FRVSD_HPP

#include <vector>
#include "../Instance/Instance.hpp"
#include "../Instance/City.hpp"
#include "../Instance/Solution.hpp"
#include "../Stats.hpp"
using namespace std;

#define nTIMELIMIT 600000

class FRVSD {
    public:
    static Solution REA(Instance &instance, Solution sol) {
        Solution best = sol;
        vector<vector<bool>> population(instance.itemCount, sol.items);
        vector<long long> weights(instance.itemCount, sol.getWeight(instance));
        vector<double> objectives(instance.itemCount, sol.objective);
        Timer::start();
        while(Timer::elasped() < nTIMELIMIT) {
            vector<bool> packing; long long weight;
            if(rand() % 2) {
                packing = best.items;
                weight = best.getWeight(instance);
            } else {
                int x = rand() % instance.itemCount;
                packing = population[x];
                weight = weights[x];
            }
            for (int i = 0; i < instance.itemCount; ++i) {
                if(!instance.items[i].available || (rand()%instance.itemCount)) continue;
                int p = !packing[i] ? 1 : -1;
                if(weight + (p*instance.items[i].weight) > instance.maxWeight) continue;
                packing[i] = !packing[i];
                weight += p*instance.items[i].weight;
            }
            Solution current(sol.tour, packing);
            current.objective = Solution::calculateObjective(instance,current);
            if(current.objective > best.objective) {
                Experiment::addDataPoint(Solution::evalCount, current.objective);
                best = current;
            }
            if(disrupt()) return best;
            int hd = FRVSD::hammingDistance(sol.items, current.items);
            if(current.objective > objectives[hd]) {
                population[hd] = current.items;
                weights[hd] = weight;
                objectives[hd] = current.objective;
            }
        }
        return best;
    }

    static int hammingDistance(vector<bool> &a, vector<bool> &b) {
        int hd = 0;
        int size = min(a.size(), b.size());
        for (int i = 0; i < size; ++i) {
            hd += (a[i]!=b[i]);
        }
        return hd;
    }
};

#endif