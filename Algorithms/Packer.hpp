#ifndef PACK_HPP
#define PACK_HPP

#include <vector>
#include "../Instance/Instance.hpp"
#include "../Instance/City.hpp"
#include "../Instance/Solution.hpp"
#include "../Stats.hpp"
using namespace std;

class Packer {
    public:
    struct ItemScore {
        ItemScore(int i, double s, bool avail) {
            index = i; score = s; available = avail;
        }
        bool operator<(const ItemScore &b) const {
            if(available && !b.available) return true;
            if(!available && b.available) return false;
            return score > b.score; 
        }
        
        int index; double score; bool available;
    };

    static vector<bool> pack(Instance &instance, vector<int> &tour, double a, int t = 100) {
        int n = tour.size();
        vector<ItemScore> scores;
        int dist = 0;
        for(int i = n-1; i >= 0; i--) {
            auto city = instance.cities[tour[i]];
            auto nextCity = instance.cities[tour[(i+1)%n]];
            dist += City::ttp_dist(city, nextCity);
            for(int j = 0; j < city.items.size(); j++) {
                auto item = city.items[j];
                double score = pow(item.value, a) / (pow(item.weight, a) * dist);
                scores.emplace_back(item.index, score, instance.items[item.index].available);
            }
        }
        sort(scores.begin(), scores.end());
        double mu = instance.itemCount*1.0/100;
        vector<bool> currentPacking(instance.itemCount, false);
        vector<bool> bestPacking(instance.itemCount, false);
        long long bestPackingWeight = 0;
        long long currentWeight = 0;
        double bestObjective = -1*numeric_limits<double>::max();
        int k = -1; int k_s = -1;
        while(currentWeight < instance.maxWeight && k < instance.itemCount-1 && mu > 1) {
            k++; int itemIndex = scores[k].index;
            if(!instance.items[itemIndex].available) continue;
            if(currentWeight + instance.items[itemIndex].weight > instance.maxWeight) continue;
            currentPacking[itemIndex] = true;
            currentWeight += instance.items[itemIndex].weight;
            if((k+1)%int(mu)==0) {
                double newObjective = Solution::calculateObjective(instance, Solution(tour, currentPacking)); 
                if(newObjective < bestObjective) {
                    currentPacking = bestPacking;
                    k = k_s; currentWeight = bestPackingWeight;
                    mu = ceil(mu/2);
                } else {
                    bestPacking = currentPacking;
                    bestPackingWeight = currentWeight;
                    k_s = k; bestObjective = newObjective;
                }
                if (disrupt()) return bestPacking;
             }
        }
        return bestPacking;
    }

    static vector<bool> packIteratively(Instance &instance, vector<int> &tour, double c, double d, int q) {
        vector<bool> bestPacking(instance.itemCount, false);
        double scratchObj = Solution::calculateObjective(instance, Solution(tour, bestPacking)); Solution::evalCount -= 1;
        Experiment::addDataPoint(Solution::evalCount, scratchObj);
        vector<bool> packingL = Packer::pack(instance, tour, c - d);
        vector<bool> packingM = Packer::pack(instance, tour, c);
        vector<bool> packingR = Packer::pack(instance, tour, c + d);
        double objectiveL = Solution::calculateObjective(instance, Solution(tour, packingL));
        if (disrupt()) return packingL;
        double objectiveM = Solution::calculateObjective(instance, Solution(tour, packingM));
        if (disrupt()) return objectiveL > objectiveM ? packingL : packingM;
        double objectiveR = Solution::calculateObjective(instance, Solution(tour, packingR));
        if (disrupt()) return objectiveL > objectiveM ? (objectiveL > objectiveR ? packingL : packingR) : (objectiveM > objectiveR ? packingM : packingR);
        int i = 1;
        while(i <= q) {
            if(objectiveL > objectiveM && objectiveR > objectiveM) {
                if(objectiveL > objectiveR) {
                    objectiveM = objectiveL; c -= d; bestPacking = packingL;
                } else {
                    objectiveM = objectiveR; c += d; bestPacking = packingR;
                }
                Experiment::addDataPoint(Solution::evalCount, objectiveM);
            } else if(objectiveL > objectiveM) {
                objectiveM = objectiveL; c -= d; bestPacking = packingL;
                Experiment::addDataPoint(Solution::evalCount, objectiveM);
            } else if(objectiveR > objectiveM) {
                objectiveM = objectiveR; c += d; bestPacking = packingR;
                Experiment::addDataPoint(Solution::evalCount, objectiveM);
            }
            d /= 2;
            packingL = Packer::pack(instance, tour, c - d);
            if(disrupt()) return bestPacking;
            packingR = Packer::pack(instance, tour, c + d);
            if(disrupt()) return bestPacking;
            objectiveL = Solution::calculateObjective(instance, Solution(tour, packingL));
            if(disrupt()) return bestPacking;
            objectiveR = Solution::calculateObjective(instance, Solution(tour, packingR));
            if(disrupt()) return bestPacking;
            i += 1;
            if ((abs(objectiveL - objectiveM) <= 0.1) && (abs(objectiveR - objectiveM) <= 0.1)) break;
        }
        return bestPacking;
    }
};

#endif