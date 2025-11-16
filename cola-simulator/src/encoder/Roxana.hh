#ifndef _ROXANA_HH_
#define _ROXANA_HH_

#include "../inc/include.hh"

#include "EncoderBase.hh"

using namespace std;

class Roxana : public EncoderBase {
    private:
        int _nc;
        int _nv;
        int _N;
        int _girth;

    public:
        Roxana(string param);
        void encode(unsigned char* info, unsigned char* parity);
};

#endif
