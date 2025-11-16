#ifndef _IRS_HH_
#define _IRS_HH_

#include "../inc/include.hh"

#include "EncoderBase.hh"

using namespace std;

// Ref: Integer Ring Sieve for Constructing Compact QC-LDPC Codes With Girths 8, 10, and 12
class IRS : public EncoderBase {
    private:
        int _m;     // row of exponent p matrix
        int _n;     // column of exponent p matrix
        int _N;     // lifting factor of exponent p matrix
        int _girth; // target girth of h matrix

        int _a;
        set<int> _Tn;
        
    public:
        IRS(string param);
        void encode(unsigned char* info, unsigned char* parity);
        int girthEvaluation(vector<vector<int>>& P, int N);
        int girthEvaluationV2(vector<vector<int>>& P, int N);
        int greedySearch(int m, int n, int N, int g, vector<int>& G, const string& fileMatrixP);
        void getRemainCoeff(set<int>& coeff, set<int>& S, vector<int>& P1, int N, int g);
        void recursiveSearch(set<int>& coeff, set<int>& S, int N, int n, vector<int>& P1, vector<int>& G);
};

#endif
