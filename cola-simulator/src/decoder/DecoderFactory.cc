#include "DecoderFactory.hh"

DecoderFactory::DecoderFactory()
{
}

DecoderFactory::~DecoderFactory()
{
}

DecoderBase *DecoderFactory::createDecoder(string name, string param, EncoderBase &encoder, double alpha)
{

    DecoderBase *decoder;

    if (name == "spa")
    {
        decoder = new SPA(param);
    }

    if (name == "msa")
    {
        decoder = new MSA(param);
    }

    if (name == "lmsa")
    {
        decoder = new LMSA(param);
    }

    if (name == "qlmsa")
    {
        decoder = new QLMSA(encoder, param, alpha);
    }

      if (name == "bf")
    {
        decoder = new BF(param);
    }

      if (name == "gdbf")
    {
        decoder = new GDBF(param);
    }

      if (name == "ppbf")
    {
        decoder = new PPBF(param);
    }

    return decoder;
}
