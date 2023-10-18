
#ifndef AE_ENABLE_CDT
#   error AE_ENABLE_CDT required
#endif

#include "CDT.h"

#ifdef ANDROID
# include "TestHelper.h"

extern void AE_CDT_Test ()
#else
int main ()
#endif
{
    return 0;
}
