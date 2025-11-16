#ifndef ECC_H
#define ECC_H

#include "Config.h"
#include <cmath>
#include <cstdint>
#include <random>

// Utils
extern int64_t seed;
extern mt19937 e;
extern uniform_int_distribution<unsigned char> u;
extern normal_distribution<double> g;

inline double get_rber(uint64_t cycles, double retention_time, uint64_t reads)
{
    double rber;
    rber = RBER_THETA + RBER_ALPHA * pow(cycles, RBER_K) +
           RBER_BETA * pow(cycles, RBER_M) * pow(retention_time, RBER_N) +
           RBER_GAMMA * pow(cycles, RBER_P) * pow(reads, RBER_Q);
    return rber;
}

#endif