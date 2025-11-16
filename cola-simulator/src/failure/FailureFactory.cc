#include "FailureFactory.hh"

FailureFactory::FailureFactory() {
}

FailureFactory::~FailureFactory() {

}

FailureBase* FailureFactory::createFailureModel(string name, string param) {

    FailureBase* model;

    if (name == "gaussian") {
        model = new Gaussian(param);
    }
    
    return model;
}
