#ifndef XMEM_TIME_H
#define XMEM_TIME_H

#include <chrono>

#include "prelude.h"


namespace xmem {

class SteadyClock {
public:
    SteadyClock() = default;

    explicit SteadyClock(std::chrono::steady_clock::time_point start)
        : start(start)
    {}

    static SteadyClock now() {
        return SteadyClock(std::chrono::steady_clock::now());
    }

    auto elapsed() const {
        return std::chrono::steady_clock::now() - start;
    }

    u64 elapsed_ns() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed())
            .count();
    }

private:
    std::chrono::steady_clock::time_point start{};
};


}


#endif
