#ifndef _GC_DECODERBASE_HH_
#define _GC_DECODERBASE_HH_

#include "../gc-encoder/GC-EncoderBase.hh"
#include "../inc/include.hh"
#include "../util/utils.hh"

using namespace std;

class GC_DecoderBase
{
public:
    GC_DecoderBase();
    GC_DecoderBase(string param);
    virtual ~GC_DecoderBase();

    virtual int decode_local_1(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta) = 0;
    virtual int decode_local_2(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta) = 0;
    virtual int decode_local_t(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta) = 0;
    virtual int decode_global(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta) = 0;
    bool check(unsigned char *corrected, vector<vector<int>> &_row_mat);
};

#endif
