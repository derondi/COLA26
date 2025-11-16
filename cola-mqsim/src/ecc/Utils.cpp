#include "ECC.h"
#include <chrono>

int64_t seed =
    chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
mt19937 e(seed);
uniform_int_distribution<unsigned char> u(0, 9);
normal_distribution<double> g(0.0, 1.0);
