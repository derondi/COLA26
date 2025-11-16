#ifndef _GAUSSIAN_HH_
#define _GAUSSIAN_HH_

#include "../inc/include.hh"

#include "FailureBase.hh"

using namespace std;

class Gaussian : public FailureBase
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
    Gaussian(string param);
    void generateFailure(unsigned char *info, unsigned char *parity, double *recv);
    double RBERtoSNR(double rber, double rate);
};

#endif
