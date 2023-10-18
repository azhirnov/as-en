
#ifndef AE_ENABLE_KTX
#   error AE_ENABLE_KTX required
#endif

#define KHRONOS_STATIC
#include "ktx.h"

#ifdef ANDROID
# include "TestHelper.h"

extern void AE_KTX_Test ()
#else
int main ()
#endif
{
    ktxTexture2_CreateFromMemory( nullptr, 0, 0, nullptr );
    return 0;
}
