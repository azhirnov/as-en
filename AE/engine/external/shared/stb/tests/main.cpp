
#ifndef AE_ENABLE_STB
# error AE_ENABLE_STB required
#endif


#ifdef ANDROID
# include "TestHelper.h"

extern int AE_STB_Test ()
#else
int main ()
#endif
{

    return 0;
}
