#include "GC-FailureFactory.hh"

GC_FailureFactory::GC_FailureFactory() {
}

GC_FailureFactory::~GC_FailureFactory() {

}

GC_FailureBase* GC_FailureFactory::createGCFailureModel(string name, string param) {

    GC_FailureBase* model;

    if (name == "gc-gaussian") {
        model = new GC_Gaussian(param);
    }
    
    return model;
}