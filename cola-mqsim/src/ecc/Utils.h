#ifndef ECC_UTILS_H
#define ECC_UTILS_H

#include <random>
#include <string>
#include <vector>

using namespace std;

extern int64_t seed;
extern mt19937 e;
extern uniform_int_distribution<unsigned char> u;
extern normal_distribution<double> g;

inline void splitString(const string &s, vector<string> &v, const string &c)
{
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2 - pos1));
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
    {
        v.push_back(s.substr(pos1));
    }
}

#endif