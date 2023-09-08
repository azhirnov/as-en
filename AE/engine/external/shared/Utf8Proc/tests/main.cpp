
#ifndef AE_ENABLE_UTF8PROC
#   error AE_ENABLE_UTF8PROC required
#endif

#ifndef UTF8PROC_STATIC
#   error UTF8PROC_STATIC required
#endif

#include "utf8proc.h"

#ifdef ANDROID
# include "TestHelper.h"

extern int AE_Utf8Proc_Test ()
#else
int main ()
#endif
{
    return 0;
}
