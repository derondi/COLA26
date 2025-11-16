#ifndef _PPBF_HH_
#define _PPBF_HH_

#include "../inc/include.hh"

#include "DecoderBase.hh"

using namespace std;

class PPBF : public DecoderBase {
    private: 
        int _round; // decoder max iter

    public:
        PPBF(string param);
        int decode(double* receive, unsigned char* corrected, vector<vector<int>>& _row_mat, vector<vector<int>>& _column_mat,double alpha, double beta);
        bool generateR(double p);
};

#endif