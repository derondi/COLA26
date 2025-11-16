#ifndef _GC_FAILUREFACTORY_HH_
#define _GC_FAILUREFACTORY_HH_

#include "GC-FailureBase.hh"
#include "GC-Gaussian.hh"

using namespace std;

class GC_FailureFactory {

    public:
        GC_FailureFactory();
        ~GC_FailureFactory();
        GC_FailureBase* createGCFailureModel(string name, string param);
};

#endif