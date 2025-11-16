#include "EncoderFactory.hh"

EncoderFactory::EncoderFactory() {
}

EncoderFactory::~EncoderFactory() {

}

EncoderBase* EncoderFactory::createEncoder(string name, string param) {

    EncoderBase* encoder;

    if (name == "roxana") {
        encoder = new Roxana(param);
    }

    if (name == "irs") {
        encoder = new IRS(param);
    }

    if (name == "ru") {
        encoder = new RU(param);
    }
    
    return encoder;
}
