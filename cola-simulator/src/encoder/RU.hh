#ifndef _RU_HH_
#define _RU_HH_

#include "../inc/include.hh"

#include "EncoderBase.hh"

using namespace std;

// Ref: Integer Ring Sieve for Constructing Compact QC-LDPC Codes With Girths 8, 10, and 12
class RU : public EncoderBase {
    private:
        int _m;     // row of exponent p matrix
        int _n;     // column of exponent p matrix
        int _N;     // lifting factor of exponent p matrix
        int _g;     // col of lower-triangular matrix
        int _gap;   // _row - _g
        int _col;   // col of H matrix
        int _row;   // row of H matrixH
        int _k;     // masking factor of P matrix
        int _p;
        
    public:
        RU(string param);
        void encode(unsigned char* info, unsigned char* parity);
};

#endif