#include "grader.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include <iostream>

void DropCaches()
{
#ifdef __APPLE__
    std::cerr << "DropCaches\n";
    assert(system("sudo purge") == 0);
#else
    const auto file = std::fopen("/proc/sys/vm/drop_caches", "w");
    if (!file || std::fputs("3", file) < 0) {
        throw std::runtime_error("failed to drop caches");
    }

    std::fclose(file);
#endif
}
