#ifndef _FAILUREFACTORY_HH_
#define _FAILUREFACTORY_HH_

#include "FailureBase.hh"
#include "Gaussian.hh"

using namespace std;

class FailureFactory {

    public:
        FailureFactory();
        ~FailureFactory();
        FailureBase* createFailureModel(string name, string param);
};

#endif
