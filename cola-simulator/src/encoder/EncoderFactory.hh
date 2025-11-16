#ifndef _ENCODERFACTORY_HH_
#define _ENCODERFACTORY_HH_

#include "EncoderBase.hh"
#include "Roxana.hh"
#include "IRS.hh"
#include "RU.hh"

using namespace std;

class EncoderFactory {

    public:
        EncoderFactory();
        ~EncoderFactory();
        EncoderBase* createEncoder(string name, string param);
};

#endif
