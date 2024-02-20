
#ifndef AE_ENABLE_LZ4
#   error AE_ENABLE_LZ4 required
#endif

#include "lz4.h"
#include "lz4hc.h"

#ifdef ANDROID
# include "TestHelper.h"

extern int AE_lz4_Test ()
#else
int main ()
#endif
{
    (void)&LZ4_compress_default;
    (void)&LZ4_compress_HC;
    (void)&LZ4_decompress_safe;

    return 0;
}
