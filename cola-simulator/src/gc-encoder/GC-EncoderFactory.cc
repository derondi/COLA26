#include "GC-EncoderFactory.hh"

GC_EncoderFactory::GC_EncoderFactory() {
}

GC_EncoderFactory::~GC_EncoderFactory() {

}

GC_EncoderBase* GC_EncoderFactory::createGCEncoder(string name, string param) {

    GC_EncoderBase* encoder;

    if (name == "gc-irs") {
        encoder = new GC_IRS(param);
    }
    
    return encoder;
}
