#include "GC-DecoderFactory.hh"

GC_DecoderFactory::GC_DecoderFactory()
{
}

GC_DecoderFactory::~GC_DecoderFactory()
{
}

GC_DecoderBase *GC_DecoderFactory::createGCDecoder(string name, string param, GC_EncoderBase &encoder, double alpha)
{

    GC_DecoderBase *decoder;

      if (name == "gc-qlmsa")
    {
        decoder = new GC_QLMSA(encoder, param, alpha);
    }

    if (name == "gc-spa")
    {
        decoder = new GC_SPA(param);
    }

    if (name == "gc-gdbf")
    {
        decoder = new GC_GDBF(param);
    }
    
    return decoder;
}
