#ifndef _FAILUREBASE_HH_
#define _FAILUREBASE_HH_

#include "../inc/include.hh"
#include "../util/utils.hh"

using namespace std;

class FailureBase
{
public:
    FailureBase();
    FailureBase(string param);
    virtual ~FailureBase();

    virtual void generateFailure(unsigned char *info, unsigned char *parity, double *recv) = 0;
};

#endif
