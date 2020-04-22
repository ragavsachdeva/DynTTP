#ifndef CLK_HPP
#define CLK_HPP

#include <vector>
#include <cmath>
#include <set>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include "../Instance/Instance.hpp"
using namespace std;

pair<int, int> make_sorted_pair(int x, int y) {
  if (x < y) {
    return make_pair(x, y);
  } else {
    return make_pair(y, x);
  }
}

class CLK {
  public:
    static vector<int> generateUsingConcorde(Instance &instance) {
        system(("./Algorithms/concorde-macos -o temp/tour.txt " + instance.file).c_str());
        freopen("temp/tour.txt", "r", stdin); // TODO: change the way of reading from a file
        int n; cin >> n;
        vector<int> tour(n);
        for(auto &x : tour) { cin >> x; }
        fclose(stdin);
        system("rm *.mas \n rm *.pul \n rm *.sav \n rm *.sol");
        return tour;
    }

    static vector<int> readTour(string tourfile) {
        freopen(tourfile.c_str(), "r", stdin); // TODO: change the way of reading from a file
        char ch; cin >> ch;
        vector<int> tour;
        while(ch != ']') {
            int num; cin >> num;
            tour.push_back(num);
            cin >> ch;
        }
        fclose(stdin);
        if(tour[tour.back()]==0) tour.pop_back(); //last city is the same as first
        return tour;
    }

    static vector<int> generateUsingLinkern(Instance &instance) {
        string tour_file = "tempfiles/tour_" + instance.file;
        for (int i = 0; i < tour_file.length(); ++i) if(tour_file[i]=='/') tour_file[i]='_';
        system(("./Algorithms/linkern-macos -s 0 -o "+tour_file + " " + instance.file).c_str());
        freopen(tour_file.c_str(), "r", stdin); // TODO: change the way of reading from a file
        int n; int ignore; cin >> n; cin >> ignore;
        vector<int> tour(n);
        for(auto &x : tour) { cin >> x; cin >> ignore; cin >> ignore;}
        fclose(stdin);
        return tour;
    }

    int size;
    CLK(vector<pair<double, double>> &coords, vector<int> &ids) {
        this->coords = coords;
        this->ids = ids;
        size = ids.size();
        tour = vector<int>(size, 0);
        for (int i = 0; i < size; i++)
            tour[i] = (i + 1) % size;
        edgeDistances = vector<vector<double>> (size, vector<double> (size, 0));
        double edgeDistance;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
            edgeDistance = sqrt(pow((coords[i].first - coords[j].first), 2) + 
                pow((coords[i].second - coords[j].second), 2));
            edgeDistances[i][j] = edgeDistance;
            edgeDistances[j][i] = edgeDistance;
            }
        }
    }

    double getCurrentTourDistance() {
        int currentIndex = 0;
        double distance = 0;
        for (int i = 0; i < size; i++) {
            distance += edgeDistances[i][tour[i]];
        }
        return distance;
    }

    void optimizeTour() {
        int diff;
        int old_distance = 0;
        int new_distance = 0;
        for (int j = 0; j < 100; j++) {
            for (int i = 0; i < size; i++)
                LKMove(i);
            new_distance = getCurrentTourDistance();
            diff = old_distance - new_distance;
            if (j != 0) {
                assert(diff >= 0);
                if (diff == 0)
                    break;
            }
            old_distance = new_distance;
        }
    }

    vector<int> getTour() {
        vector<int> outTour;
        int current = 0;
        do {
            outTour.push_back(ids[current]);
            current = tour[current];
        } while (current != 0);
        return outTour;
    }


  private:
    vector<int> tour;
    vector<vector<int>> edgeFlags;
    vector<pair<double, double>> coords;
    vector<int> ids;
    void joinLocations(int i, int j);
    vector<vector<double>> edgeDistances;

    void LKMove(int tourStart) {
        set<pair<int,int> > broken_set, joined_set;
        vector<int> tour_opt = tour;
        double g_opt = 0;
        double g = 0;
        double g_local;
        int lastNextV = tourStart;
        int fromV;
        int nextV;
        int nextFromV;
        int lastPossibleNextV;
        pair<int, int> broken_edge;
        double y_opt_length;
        double broken_edge_length;
        double g_opt_local;
        fromV = tour[lastNextV];
        long initialTourDistance = getCurrentTourDistance();
        do {
            nextV = -1;
            broken_edge = make_sorted_pair(lastNextV, fromV);
            broken_edge_length = edgeDistances[broken_edge.first][broken_edge.second];
            if (joined_set.count(broken_edge) > 0) break;
            for (int possibleNextV = tour[fromV]; nextV == -1 && possibleNextV != tourStart; possibleNextV = tour[possibleNextV]) {
                g_local = broken_edge_length - edgeDistances[fromV][possibleNextV];
                if (!(
                    broken_set.count(make_sorted_pair(fromV, possibleNextV)) == 0 &&
                    g + g_local > 0 &&
                    joined_set.count(make_sorted_pair(lastPossibleNextV, possibleNextV)) == 0 &&
                    tour[possibleNextV] != 0 &&
                    possibleNextV != tour[fromV]
                )) {
                    lastPossibleNextV = possibleNextV;
                    continue;
                }
                nextV = possibleNextV;
            }
            if (nextV != -1) {
            broken_set.insert(broken_edge);
            joined_set.insert(make_sorted_pair(fromV, nextV));
            y_opt_length = edgeDistances[fromV][tourStart];
            g_opt_local = g + (broken_edge_length - y_opt_length);
            if (g_opt_local > g_opt) {
                g_opt = g_opt_local;
                tour_opt = tour;
                tour_opt[tourStart] = fromV;
            }
            g += broken_edge_length - edgeDistances[fromV][nextV];
            reverse(fromV, lastPossibleNextV);
            nextFromV = lastPossibleNextV;
            tour[fromV] = nextV;
            lastNextV = nextV;
            fromV = nextFromV;
            }
        } while (nextV != -1);
        tour = tour_opt;
        long distanceAfter = getCurrentTourDistance();
        assert(distanceAfter <= initialTourDistance);
        assert(isTour());
    }

    void reverse(int start, int end) {
        int current = start;
        int next = tour[start];
        int nextNext;
        do {
            nextNext = tour[next];
            tour[next] = current;
            current = next;
            next = nextNext;
        } while (current != end);
    }

    bool isTour() {
        int count = 1;
        int start = tour[0];
        while (start != 0) {
            start = tour[start];
            count++;
        }
        return (count == size);
    }
};

#endif