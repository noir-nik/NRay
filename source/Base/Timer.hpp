#pragma once
#include <chrono>
class Timer
{
public:
    void Start(){
        start = std::chrono::high_resolution_clock::now();
    };
    void Stop(){
        stop = std::chrono::high_resolution_clock::now();
    };
    double Elapsed(){
        stop = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::duration<double>>(stop - start).count();
    };
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::chrono::time_point<std::chrono::high_resolution_clock> stop;
};

