#include "grader.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include <xmem/exception.h>


void DropCaches()
{
#ifdef __APPLE__
    XMEM_ENSURE(
        system("sudo purge") == 0,
        "Purge failed"
    );
#else
    const auto file = std::fopen("/proc/sys/vm/drop_caches", "w");
    if (!file || std::fputs("3", file) < 0) {
        throw std::runtime_error("failed to drop caches");
    }

    std::fclose(file);
#endif
}
