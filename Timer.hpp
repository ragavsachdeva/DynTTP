#ifndef TIMER_HPP
#define TIMER_HPP
using namespace std;

class Timer {
    public:
    static chrono::time_point<chrono::high_resolution_clock> startTime;
    static void start() {
        Timer::startTime = chrono::high_resolution_clock::now();
    }
    static int elasped() {
        return chrono::duration_cast<std::chrono::milliseconds>(
         std::chrono::high_resolution_clock::now() - Timer::startTime
      ).count();
    }
};

chrono::time_point<chrono::high_resolution_clock> Timer::startTime = chrono::high_resolution_clock::now();

#endif
