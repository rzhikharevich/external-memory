#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <random>

#include <xmem/exception.h>
#include <xmem/file.h>
#include <xmem/prelude.h>
#include <xmem/time.h>
#include <xmem/vector.h>

#include "grader.h"

using namespace std;
using namespace xmem;


struct Result {
    double value = 0;
    double variation = 0;
};

std::ostream& operator<<(std::ostream& os, const Result& result) {
    os << result.value << " +- " << result.variation;
    return os;
}


Vector<u8>& get_bytes(usize size) {
    static Vector<u8> data;
    data.resize(size, 0x42);

#ifdef __APPLE__
    arc4random_buf(data.data(), size);
#endif

    return data;
}


double sum(const Vector<double>& points) {
    double ret = 0;

    for (double x : points) {
        ret += x;
    }

    return ret;
}


Result collect_result(Vector<double>* points) {
    std::sort(points->begin(), points->end());

    const double value = (*points)[points->size() / 2];
    // const double value = sum(*points) / points->size();
    const double min_diff = value - points->front();
    const double max_diff = points->back() - value;

    return {value, std::min(min_diff, max_diff)};
}


void prepare_for_reading(const std::string& path, usize size) {
    File file(path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    const auto& data = get_bytes(size);
    file.write(0, size, data.data());
}


u64 do_test_seq_read(const std::string& path, usize size) {
    auto& data = get_bytes(size);

    const auto start = SteadyClock::now();

    File file(path, O_RDONLY);
    file.read(0, size, data.data());

    return start.elapsed_ns();
}


Result TestSequentialRead(const std::string& path) {
    constexpr usize n_iters = 50;
    constexpr usize size_mb = 128;
    constexpr usize size = size_mb * (1 << 20);

    prepare_for_reading(path, size);

    Vector<double> points;

    for (usize i = 0; i < n_iters; ++i) {
        DropCaches();
        points.push_back(size_mb * 1e9 / do_test_seq_read(path, size));
    }

    return collect_result(&points);
}


u64 do_test_seq_write(const std::string& path, usize size) {
    const auto& data = get_bytes(size);

    const auto start = SteadyClock::now();

    File file(path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    file.write(0, size, data.data());
    file.sync_data();

    return start.elapsed_ns();
}


Result TestSequentialWrite(const std::string& path) {
    constexpr usize n_iters = 10;
    constexpr usize size_mb = 128;
    constexpr usize size = size_mb * (1 << 20);

    Vector<double> points;

    for (usize i = 0; i < n_iters; ++i) {
        points.push_back(size_mb * 1e9 / do_test_seq_write(path, size));
    }

    return collect_result(&points);
}


Result TestRandomRead(const std::string& path) {
    constexpr usize n_iters = 10;
    constexpr usize size_mb = 128;
    constexpr usize size = size_mb * (1 << 20);
    constexpr usize stride = 512 * (1 << 10);
    constexpr usize block_size = 512;

    prepare_for_reading(path, size);

    auto& data = get_bytes(block_size);
    File file(path, O_RDONLY);

    Vector<usize> offsets;

    for (usize off = 0; off < size; off += stride) {
        offsets.push_back(off);
    }

    std::random_device device;
    std::mt19937 rng(device());

    Vector<double> points;

    for (usize i = 0; i < n_iters; ++i) {
        std::shuffle(offsets.begin(), offsets.end(), rng);
        DropCaches();

        for (usize off : offsets) {
            const auto start = SteadyClock::now();
            file.read(off, block_size, data.data());

            const auto elapsed_ns = start.elapsed_ns();
            points.push_back(static_cast<double>(elapsed_ns) / 1000);
        }
    }

    return collect_result(&points);
}


Result TestRandomWrite(const std::string& path) {
    constexpr usize n_iters = 10;
    constexpr usize size_mb = 128;
    constexpr usize size = size_mb * (1 << 20);
    constexpr usize stride = 512 * (1 << 10);
    constexpr usize block_size = 512;

    prepare_for_reading(path, size);

    auto& data = get_bytes(block_size);
    File file(path, O_WRONLY);

    Vector<usize> offsets;

    for (usize off = 0; off < size; off += stride) {
        offsets.push_back(off);
    }

    std::random_device device;
    std::mt19937 rng(device());

    Vector<double> points;

    for (usize i = 0; i < n_iters; ++i) {
        std::shuffle(offsets.begin(), offsets.end(), rng);

        for (usize off : offsets) {
            const auto start = SteadyClock::now();
            file.write(off, block_size, data.data());
            file.sync_data();

            const auto elapsed_ns = start.elapsed_ns();
            points.push_back(static_cast<double>(elapsed_ns) / 1000);
        }
    }

    return collect_result(&points);
}


int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "Incorrect number of arguments: " << argc << ", expected at least 3 arguments: ./" << argv[0] << " {mode} {filename}" << endl;
        return 1;
    }

    string mode = argv[1];
    string filename = argv[2];
    if (mode == "seq-read") {
        auto result = TestSequentialRead(filename);
        cout << result << " MB/s" << endl;
    } else if (mode == "seq-write") {
        auto result = TestSequentialWrite(filename);
        cout << result << " MB/s" << endl;
    } else if (mode == "rnd-read") {
        auto result = TestRandomRead(filename);
        cout << result << " mcs" << endl;
    } else if (mode == "rnd-write") {
        auto result = TestRandomWrite(filename);
        cout << result << " mcs" << endl;
    } else {
        cerr << "Incorrect mode: " << mode << endl;
        return 1;
    }

    return 0;
}
