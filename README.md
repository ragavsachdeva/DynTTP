# The Dynamic Travelling Thief Problem: Benchmarks and Performance of Evolutionary Algorithms

Travelling Thief Problem (**TTP**) is a hybrid of the classic **NP-Hard** Travelling Salesman Problem (**TSP**) and the Knapsack Problem (**KP**). 
**DynTTP** is an extension of the classic TTP where the problem  constraints  can  change  during  runtime.

**Conference:** Accepted at ICONIP'20

**Paper:** [pdf](https://arxiv.org/abs/2004.12045)

**Authors:** Ragav Sachdeva, Frank Neumann, Markus Wagner

## Usage

**Compile**
```
g++ -std=c++11 dttp.cpp -o dttp
```

**Run**
```
./dttp instance solver num-runs disrupted-entity percentange-toggle reoptimiser num-disruptions frequency tourfilepath(optional)
```
e.g.
```
./dttp instances/a280_instances/a280_n279_bounded-strongly-corr_01.ttp S2 30 items 0.3 viaBitflip 10 1 tours/a280/
```
Running the above will produce a .txt file in the output folder containing all the objective values and the corresponding eval count in an array-like format.

We are attaching a jupyter notebook for **visualisation** that shows how to read data from the output .txt files.

## Results

You can find all the plots for our experiments under the Results folder.
