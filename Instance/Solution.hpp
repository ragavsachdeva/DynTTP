#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <vector>
#include "Instance.hpp"
#include "City.hpp"
#include "Item.hpp"
using namespace std;

#define PRINTALL(arr) for(int i = 0; i < arr.size(); i++) { cout << arr[i] << " "; } cout << endl;

class Solution {
    public:
    Solution(): tour({}), items({}), objective(-1*numeric_limits<double>::max()) {}
    Solution(vector<int> tour, vector <bool> items): tour(tour), items(items), objective(-1*numeric_limits<double>::max()) {}
    static int evalCount;
    static double calculateObjective(Instance &instance, Solution current) {
        evalCount++;
        long long collectedWeight = 0;
        double objectiveValue = 0.0;
        double speedCoef = (instance.maxSpeed - instance.minSpeed) / instance.maxWeight;
        auto &tour = current.tour;
        auto &items = current.items;
        auto nextAvailableCity = [&](int i) -> int {
            i = (i+1) % tour.size();
            while(!instance.cities[tour[i]].available) i = (i+1) % tour.size();
            return i;
        };
        int start = 0;
        int curr = start; int next;
        do {
            next = nextAvailableCity(curr);
            City &currCity = instance.cities[tour[curr]];
            City &nextCity = instance.cities[tour[next]];
            int nextCityDistance = City::ttp_dist(currCity, nextCity);
            for (Item item: currCity.items) {
                if (!items[item.index]) continue;
                collectedWeight += item.weight;
                if (collectedWeight > instance.maxWeight) {
                    cerr << "Exceeded knapsack weight" << endl;
                    return numeric_limits<double>::min();
                }
                objectiveValue += item.value;
            }
            objectiveValue -= instance.rentingRate * (nextCityDistance / (instance.maxSpeed - speedCoef * collectedWeight));
            curr = next;
        } while (next != start);
        return objectiveValue;
    }

    long long getWeight(Instance &instance) {
        long long weight = 0;
        for(int i = 0; i < items.size(); i++) {
            weight += (items[i]*instance.items[i].weight);
        }
        return weight;
    }

    void dropKItems(Instance &instance, int k) {
        if(k > instance.itemCount) {
            cout << "Cannot drop more items than available!" << endl;
            return;
        }
        vector<int> indices(instance.itemCount);
        for(int i = 0; i < instance.itemCount; ++i) indices[i] = i;
        random_shuffle(indices.begin(), indices.begin()+instance.itemCount);
        for (int i = 0; i < k; ++i) {
            int itemIndex = indices[i];
            if (instance.items[itemIndex].available) {
                instance.items[itemIndex].available = false;
                items[itemIndex] = false;
            } else {
                instance.items[itemIndex].available = true;
            }
        }
        objective = Solution::calculateObjective(instance, *this);
    }

    void dropKCities(Instance &instance, int k, string tempFile) {
        if(k > instance.cityCount - 1) {
            cout << "Cannot drop more cities than available!" << endl;
            return;
        }
        // Cannot drop the first city.
        vector<int> indices(instance.cityCount - 1);
        for(int i = 0; i < instance.cityCount - 1; ++i) indices[i] = i+1;
        random_shuffle(indices.begin(), indices.begin()+instance.cityCount-1);
        for (int i = 0; i < k; ++i) {
            int cityIndex = indices[i];
            if (instance.cities[cityIndex].available) {
                instance.cities[cityIndex].available = false;  
                // NOTE: The cities are not removed from the tour here, for now.
                // The calculateObjective function will handle skipping these cities.
                // This is done to ensure some sort of ordering in case the city re-appears.
            } else {
                instance.cities[cityIndex].available = true;  
            }
        }
        objective = Solution::calculateObjective(instance, *this);
        instance.citiesToFile(tempFile);
    }

    vector<int> tour;
    vector<bool> items;
    double objective;
};

int Solution::evalCount = 0;

#endif