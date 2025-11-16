#ifndef _GC_IRS_HH_
#define _GC_IRS_HH_

#include "../inc/include.hh"

#include "GC-EncoderBase.hh"

using namespace std;

// Ref: Integer Ring Sieve for Constructing Compact QC-LDPC Codes With Girths 8, 10, and 12
class GC_IRS : public GC_EncoderBase {
    private:
        int _m;     // row of exponent p matrix
        int _n;     // column of exponent p matrix
        int _N;     // lifting factor of exponent p matrix
        int _t; //the number of Hlocal matrix
        int _m_local; // row of local exponent p matrix
        int _m_global; // row of global exponent p matrix
        int _n_local; // column of local exponent p matrix

        int _a;
        set<int> _Tn;
        
    public:
        GC_IRS(string param);
        void encode1(unsigned char* info, unsigned char* parity);
        void encode2(unsigned char* info, unsigned char* parity);
        void encode_2local(unsigned char *c1, unsigned char *s2, unsigned char* p2, unsigned char* p0);
        void encode_3local(unsigned char *c1, unsigned char *c2, unsigned char* s3, unsigned char* p3, unsigned char* p0);
};

#endif