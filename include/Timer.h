#pragma once

#include <chrono>

class Timer
{
    using Clock = std::chrono::high_resolution_clock;
    using Second = std::chrono::duration<double, std::ratio<1>>;

public:
    Timer()
        : beg_(Clock::now()) {}

    auto reset()
        -> void
    {
        beg_ = Clock::now();
    }
    auto elapsed() const
        -> double
    {
        return std::chrono::duration_cast<Second>(Clock::now()
                                                  - beg_)
            .count();
    }

private:
    std::chrono::time_point<Clock> beg_;
};
