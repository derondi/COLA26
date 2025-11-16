#ifndef _UTILS_HH_
#define _UTILS_HH_

#include "../inc/include.hh"

using namespace std;

extern int64_t seed;
extern mt19937 e;
extern uniform_int_distribution<int> u;
extern normal_distribution<double> g;

void splitString(const string& s, vector<string>& v, const string& c);

#endif
