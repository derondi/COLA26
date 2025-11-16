#ifndef _DECODERFACTORY_HH_
#define _DECODERFACTORY_HH_

#include "../encoder/EncoderBase.hh"
#include "DecoderBase.hh"
#include "SPA.hh"
#include "MSA.hh"
#include "LMSA.hh"
#include "QLMSA.hh"
#include "BF.hh"
#include "GDBF.hh"
#include "PPBF.hh"

using namespace std;

class DecoderFactory
{

public:
    DecoderFactory();
    ~DecoderFactory();
    DecoderBase *createDecoder(string name, string param, EncoderBase &encoder, double alpha);
};

#endif
