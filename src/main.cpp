#include "sequential_version.hpp"
#include "parallel_version.hpp"

int main(int argc, char const* argv[]) {
    const int m = 8, n = 8, p = 3;
    run_parallel(m, n, p);
    run_sequential(m, n, p);
    return 0;
}