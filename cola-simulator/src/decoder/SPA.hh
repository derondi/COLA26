#ifndef _SPA_HH_
#define _SPA_HH_

#include "../inc/include.hh"

#include "DecoderBase.hh"

using namespace std;

class SPA : public DecoderBase
{
private:
    int _round; // decoder max iter

public:
    SPA(string param);
    int decode(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);
};

#endif
