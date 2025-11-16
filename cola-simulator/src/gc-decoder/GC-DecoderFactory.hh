#ifndef _GC_DECODERFACTORY_HH_
#define _GC_DECODERFACTORY_HH_

#include "../gc-encoder/GC-EncoderBase.hh"
#include "GC-DecoderBase.hh"
#include "GC-QLMSA.hh"
#include "GC-SPA.hh"
#include "GC-GDBF.hh"

using namespace std;

class GC_DecoderFactory
{

public:
    GC_DecoderFactory();
    ~GC_DecoderFactory();
    GC_DecoderBase *createGCDecoder(string name, string param, GC_EncoderBase &encoder, double alpha);
};

#endif
