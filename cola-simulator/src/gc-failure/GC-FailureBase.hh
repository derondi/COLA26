#ifndef _GC_FAILUREBASE_HH_
#define _GC_FAILUREBASE_HH_

#include "../inc/include.hh"
#include "../util/utils.hh"

using namespace std;

class GC_FailureBase
{
public:
    GC_FailureBase();
    GC_FailureBase(string param);
    virtual ~GC_FailureBase();

    virtual void GCgenerateFailure(unsigned char* c, double *recv, int length) = 0;
};

#endif