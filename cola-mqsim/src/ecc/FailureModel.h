#ifndef FAILURE_MODEL_H
#define FAILURE_MODEL_H

#include <vector>
#include <string>

using namespace std;

class FailureBase
{
public:
    FailureBase();
    FailureBase(string param);
    virtual ~FailureBase();

    virtual void generateFailure(unsigned char* c, double *recv, int length) = 0;
};

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
    Gaussian(unsigned long info_length, unsigned long parity_length, double rber);
    void generateFailure(unsigned char *c, double *recv, int length);
    //void generateFailure_3local(unsigned char *c, double *recv, int length);
    double RBERtoSNR(double rber, double rate);
};

#endif