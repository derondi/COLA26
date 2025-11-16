#ifndef _GC_ENCODERFACTORY_HH_
#define _GC_ENCODERFACTORY_HH_

#include "GC-EncoderBase.hh"
#include "GC-IRS.hh"

using namespace std;

class GC_EncoderFactory {

    public:
        GC_EncoderFactory();
        ~GC_EncoderFactory();
        GC_EncoderBase* createGCEncoder(string name, string param);
};

#endif
