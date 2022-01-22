#include <time.h>
#include <chrono>
#include <functional>
using namespace std;

double measure(function<void()> function) {
    auto t1 = chrono::high_resolution_clock::now();
    function();
    auto t2 = chrono::high_resolution_clock::now();
    return chrono::duration<double>(t2 - t1).count();
}
