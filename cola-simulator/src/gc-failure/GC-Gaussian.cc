#include "GC-Gaussian.hh"

GC_Gaussian::GC_Gaussian(string param)
{
    // parse param for Gaussian
    vector<string> v;
    splitString(param, v, ";");
    _info_length = stoi(v[0]);
    _parity_length = stoi(v[1]);
    _rber = stod(v[2]);
    _rate = (double)_info_length / (double)(_info_length + _parity_length);
    _snr = RBERtoSNR(_rber, _rate);
    _Es = 1.0;
    _N0 = _Es / (pow(10.0, (_snr / 10)) * _rate);
}

// Based on BPSK modulate with AWGN Noise
void GC_Gaussian::GCgenerateFailure(unsigned char *c, double *recv, int length)
{
    _Noise.resize(length);
    for (int i = 0; i < length; i++){
        _Noise[i] = g(e);
        recv[i] = (c[i] == 0 ? 1 : -1) + _Noise[i] * sqrt(_N0 / 2);
    }
}

double GC_Gaussian::RBERtoSNR(double rber, double rate)
{
    // initialize snr
    double snr_high = 8.00;
    double snr_low = 3.50;
    double snr_mid = 0.00;
    // initialize rber
    double f_rber = rber;
    while ((snr_high - snr_low) > 0.0001)
    {
        snr_mid = snr_low + (snr_high - snr_low) / 2.0;
        double a_k = pow(10.0, snr_mid / 20.0) * pow(rate, 0.5);
        if (std::fabs(0.5 * erfc(a_k) - f_rber) < 0.000001)
        {
            return std::round(snr_mid * 1000) / 1000;
        }

        if (0.5 * erfc(a_k) > f_rber)
        {
            snr_low = snr_mid;
        }
        else
        {
            snr_high = snr_mid;
        }
    }
    return std::round(snr_mid * 1000) / 1000;
    ;
}