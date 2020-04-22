#ifndef OPTIMISER_HPP
#define OPTIMISER_HPP

#include <vector>
#include "../Instance/Instance.hpp"
#include "../Instance/Solution.hpp"
#include "../Stats.hpp"
#include "../Timer.hpp"
using namespace std;

#define TIMELIMIT 600000

class Optimiser {
    public:
    static Solution bitflip(Instance &instance, Solution current, bool printObjective=false) {
        Solution bestSolution = current;
        long long weight = current.getWeight(instance);
        for(int i = 0; i < current.items.size(); i++) {
            if (Timer::elasped() >= TIMELIMIT) return bestSolution;
            if(!instance.items[i].available) continue;
            int p = !current.items[i] ? 1 : -1;
            if(weight + (p*instance.items[i].weight) > instance.maxWeight) continue;
            current.items[i] = !current.items[i];
            weight += p*instance.items[i].weight;
            current.objective = Solution::calculateObjective(instance, current);
            if(current.objective > bestSolution.objective) {
                bestSolution = current;
                if(printObjective) Experiment::addDataPoint(Solution::evalCount, bestSolution.objective);
            } else {
                current.items[i] = !current.items[i];
                weight += -1*p*instance.items[i].weight;
            }
            if(disrupt()) return bestSolution; 
        }
        return bestSolution;
    }

    static Solution jump(Instance &instance, Solution current, bool all=true) {
        Solution bestSolution = current;
        if (!all) {
            int i;
            do {
                i = rand() % (instance.cityCount - 1) + 1;
            } while(!instance.cities[current.tour[i]].available);
            Solution contender = bestSolution;
            for(int j = i-1; j > 0; j--) {
                if (Timer::elasped() >= TIMELIMIT) return bestSolution;
                current = bestSolution;
                int city = current.tour[i];
                current.tour.erase(current.tour.begin()+i);
                current.tour.insert(current.tour.begin()+j, city);
                current.objective = Solution::calculateObjective(instance, current);
                if(current.objective > contender.objective) {
                    contender = current;
                    Experiment::addDataPoint(Solution::evalCount, contender.objective);
                }
                if(disrupt()) return contender; 
            }
            bestSolution = contender;
            return bestSolution;
        }
        for(int i = instance.cityCount - 1; i >= 0; i--) {
            if (!instance.cities[current.tour[i]].available) continue;
            Solution contender = bestSolution;
            for(int j = i-1; j > 0; j--) {
                if (Timer::elasped() >= TIMELIMIT) return bestSolution;
                current = bestSolution;
                int city = current.tour[i];
                current.tour.erase(current.tour.begin()+i);
                current.tour.insert(current.tour.begin()+j, city);
                current.objective = Solution::calculateObjective(instance, current);
                if(current.objective > contender.objective) {
                    contender = current;
                    Experiment::addDataPoint(Solution::evalCount, contender.objective);
                }
                if(disrupt()) return contender; 
            }
            bestSolution = contender;
        }
        return bestSolution;
    }

};

#endif