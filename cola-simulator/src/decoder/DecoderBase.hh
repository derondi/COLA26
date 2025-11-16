#ifndef _DECODERBASE_HH_
#define _DECODERBASE_HH_

#include "../encoder/EncoderBase.hh"
#include "../inc/include.hh"
#include "../util/utils.hh"

using namespace std;

class DecoderBase
{
public:
    DecoderBase();
    DecoderBase(string param);
    virtual ~DecoderBase();

    virtual int decode(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta) = 0;
    bool check(unsigned char *corrected, vector<vector<int>> &_row_mat);
};

#endif
