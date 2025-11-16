#ifndef _GC_SPA_HH_
#define _GC_SPA_HH_

#include "../inc/include.hh"

#include "GC-DecoderBase.hh"

using namespace std;

class GC_SPA : public GC_DecoderBase
{
private:
    int _round_local; // local decoder max iter
    int _round_global; // global decoder max iter

public:
    GC_SPA(string param);
    int decode_local_1(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);
    int decode_local_2(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);
    int decode_local_t(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);
    int decode_global(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);
};

#endif
