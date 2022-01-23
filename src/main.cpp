#include"includes.hpp"

int main(int argc, char const* argv[]) {
    // auto seq_time = run_sequential("sequential_matrix.txt");
    // auto parallel_time = run_parallel("matrix_parallel.txt");
    auto seq_time = run_sequential();
    auto parallel_time = run_parallel();
    printf("sequential execution took: %f \n", seq_time);
    printf("parallel execution took: %f \n", parallel_time);
    printf("speedup: %f \n", seq_time / parallel_time);
    return 0;
}