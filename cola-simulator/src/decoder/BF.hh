#ifndef _BF_HH_
#define _BF_HH_

#include "../inc/include.hh"

#include "DecoderBase.hh"

using namespace std;

class BF : public DecoderBase {
    private: 
        int _round; // decoder max iter

    public:
        BF(string param);
        int decode(double* receive, unsigned char* corrected, vector<vector<int>>& _row_mat, vector<vector<int>>& _column_mat,double alpha, double beta);
};

#endif