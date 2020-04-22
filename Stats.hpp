#ifndef STATS_HPP
#define STATS_HPP

#include <vector>
#include <iostream>
using namespace std;

struct DataPoint {
    int evalCount;
    double obj;
    DataPoint(int e, double o) {
        evalCount = e;
        obj = o;
    }
};

class Experiment {
    public:
    static vector<vector<DataPoint>> dataPoints;
    static vector<double> finalOfEachRun;
    static bool firstRun;
    static int disruptEvery;
    static void addDataPoint(int evalCount, double obj) {
        // if there is no active phase, return
        if (Experiment::dataPoints.size() == 0) return;
        if (Experiment::dataPoints.back().size() != 0 && 
            Experiment::dataPoints.back().back().evalCount == evalCount) return;
        Experiment::dataPoints.back().push_back({evalCount, obj});
    }
    static void flushBuffer() {
        if (dataPoints.size() > 0) {
            // put a comma at the start, if it is not the first run.
            if (!firstRun) cout << ",";
            else firstRun = false;
            cout << "[";
            for (int phase = 0; phase < Experiment::dataPoints.size(); phase++) {
                cout << "[";
                for (int i = 0; i < Experiment::dataPoints[phase].size(); i++) {
                    cout << "[" << Experiment::dataPoints[phase][i].evalCount << "," << Experiment::dataPoints[phase][i].obj << "]";
                    if (i!=Experiment::dataPoints[phase].size()-1) cout << ",";
                }
                cout << "]";
                if (phase!=Experiment::dataPoints.size()-1) cout << ",";
            }
            cout << "]";
        }
    }
    static void startNewPhase() {
        Experiment::dataPoints.push_back({});
    }
    static void startNewRun() {
        if (Experiment::dataPoints.size() > 0) Experiment::finalOfEachRun.push_back(Experiment::dataPoints.back().back().obj);
        Experiment::flushBuffer();
        Experiment::dataPoints = {};
    }
    static void start() {
        cout << "[";
        Experiment::firstRun = true;
        Experiment::finalOfEachRun = {};
        Experiment::dataPoints = {};
    }
    static void finish() {
        Experiment::flushBuffer();
        cout << "]" << endl;
    }
}; 

vector<vector<DataPoint>> Experiment::dataPoints = {};
bool Experiment::firstRun = true;
int Experiment::disruptEvery = 0;
vector<double> Experiment::finalOfEachRun = {};

bool disrupt() {
    return Experiment::disruptEvery && ((Solution::evalCount+1) % Experiment::disruptEvery == 0);
}

#endif
