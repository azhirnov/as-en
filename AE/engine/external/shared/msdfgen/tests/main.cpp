
#ifndef AE_ENABLE_MSDFGEN
#   error AE_ENABLE_MSDFGEN required
#endif

#include "msdfgen.h"
#include "msdfgen-ext.h"

#ifdef ANDROID
# include "TestHelper.h"

extern int AE_Utf8Proc_Test ()
#else
int main ()
#endif
{
    return 0;
}
