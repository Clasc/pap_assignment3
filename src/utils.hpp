#include <time.h>
#include <chrono>
#include <functional>
using namespace std;

/**
 * @param function the function to be executed
 * @brief measures the execution time of a given function
 * @return time taken in seconds
 **/
double measure(function<void()> function) {
    auto t1 = chrono::high_resolution_clock::now();
    function();
    auto t2 = chrono::high_resolution_clock::now();
    return chrono::duration<double>(t2 - t1).count();
}
