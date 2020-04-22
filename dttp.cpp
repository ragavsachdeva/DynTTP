#if __APPLE__
#include "bits_stdc++.h"
#else
#include <bits/stdc++.h>
#endif

#include "Instance/Instance.hpp"
#include "Algorithms/CLK.hpp"
#include "Algorithms/Packer.hpp"
#include "Algorithms/Optimiser.hpp"
#include "Solver.hpp"
#include "Solvers/S1.hpp"
#include "Solvers/S2.hpp"
#include "Solvers/S4.hpp"
#include "Solvers/S6.hpp"
#include "Timer.hpp"
#include "Mutator.hpp"
using namespace std;

int main(int argc, char *argv[])
{
	if(argc < 4) {
      cout << "correct usage: ./a.out instance solver num-runs mutation percentange(optional) reoptimiser(optional) disruptionCount(optional) f(optional) tourfilepath(optional)" << endl;
      return 0;
   }
    Instance instance(argv[1]);
    Solver *solver;
    if(strcmp(argv[2],"S1")==0) solver = new S1(instance);
    else if(strcmp(argv[2],"S2")==0) solver = new S2(instance);
    else if(strcmp(argv[2],"S4")==0) solver = new S4(instance);
    else if(strcmp(argv[2],"S6")==0) solver = new S6(instance);
    else { cout << "Invalid Solver!" << endl; return 0;}

    int num_runs = atoi(argv[3]);
    string mutation = argv[4];
    string tourfilepath = "";
    if(argc >= 10) tourfilepath = argv[9];
    int num_disruptions = 1;
    
    int f = 1;
    if (argc >= 9) f = atoi(argv[8]);
    if (argc >= 8) {
        num_disruptions = atoi(argv[7]);
        Experiment::disruptEvery = pow(instance.itemCount, f);
    }
    double dropPercent = 0; string reopter = "";
    if(argc >= 6) dropPercent = atof(argv[5]);
    if (argc >= 7) reopter = argv[6];
    if(dropPercent == 0 || reopter == "") return 0;

    string file = string(argv[1])+"/"+mutation+"/"+reopter+"/"+to_string(dropPercent)+".txt";
    for (int i = 0; i < file.length(); ++i) if(file[i]=='/') file[i]='_';

    auto sol = solver->solve(tourfilepath == "" ? "" : tourfilepath+"/00000.tour");

    Mutator mutator(instance, sol);
    freopen(("output/"+file).c_str(), "w", stdout);
    cout << "[[" << sol.objective << "],";
    Experiment::start();
    for(int i = 0; i < num_runs; i++) {
        srand(i+10); Experiment::startNewRun();
        mutator.dropAndCompute(solver, mutation, dropPercent, reopter, "temp_"+file, num_disruptions);
    }
    Experiment::finish();
    cout << "]" << endl;
    fclose(stdout);
	return 0;
}
