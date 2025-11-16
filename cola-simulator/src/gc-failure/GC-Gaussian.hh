#ifndef _GC_GAUSSIAN_HH_
#define _GC_GAUSSIAN_HH_

#include "../inc/include.hh"

#include "GC-FailureBase.hh"

using namespace std;

class GC_Gaussian : public GC_FailureBase
{
private:
    int _info_length;
    int _parity_length;
    double _Es;   // default 1.0
    double _rber; // raw bit error rate
    double _snr;  // signal to noise ratio
    double _rate; // code rate
    double _N0;   // N0 = Es / (pow(10.0, (snr / 10)) * Rate)
    vector<double> _Noise;

public:
    GC_Gaussian(string param);
    void GCgenerateFailure(unsigned char *c, double *recv, int length);
    //void generateFailure_3local(unsigned char *c, double *recv, int length);
    double RBERtoSNR(double rber, double rate);
};

#endif