#pragma once

#include <chrono>
#include <ostream>

class Timer
{
public:
    Timer()
    {
        reset();
    }

    void reset()
    {
        start_ = std::chrono::high_resolution_clock::now();
    }

    long long elapsed() const
    {
        auto e = std::chrono::high_resolution_clock::now() - start_;
        return std::chrono::duration_cast<std::chrono::milliseconds>(e).count();
    }

    friend std::ostream& operator<<(std::ostream& os, const Timer& t)
    {
        os << t.elapsed() << "ms";
        return os;
    }

private:
    std::chrono::high_resolution_clock::time_point start_;
};
