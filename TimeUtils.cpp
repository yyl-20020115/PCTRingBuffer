#include "TimeUtils.h"

#include <chrono>

long long get_current_nanoseconds() {
    auto now = std::chrono::high_resolution_clock::now();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    return nanoseconds;
}

