#ifdef USE_MODULES
export module Timer;
#define _TIMER_EXPORT export
import std;
#else
#pragma once
#define _TIMER_EXPORT
#include <chrono>
#endif

class Timer
{
public:
    inline void Start(){
        start = std::chrono::high_resolution_clock::now();
    };
    inline void Stop(){
        stop = std::chrono::high_resolution_clock::now();
    };
    inline double Elapsed(){
        stop = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::duration<double>>(stop - start).count();
    };
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start{};
    std::chrono::time_point<std::chrono::high_resolution_clock> stop{};
};

