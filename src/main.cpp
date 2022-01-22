#include "sequential_version.hpp"
#include "parallel_version.hpp"

int main(int argc, char const* argv[]) {
    const int m = 120, n = 120, p = 3;
    auto parallel_time = run_parallel(m, n, p);
    auto seq_time = run_sequential(m, n, p);
    printf("sequential execution took: %f \n", seq_time);
    printf("parallel execution took: %f \n", parallel_time);
    return 0;
}