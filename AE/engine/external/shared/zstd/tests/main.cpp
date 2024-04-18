
#ifndef AE_ENABLE_ZSTD
#	error AE_ENABLE_ZSTD required
#endif

#include "zstd.h"

#ifdef ANDROID
# include "TestHelper.h"

extern int AE_zstd_Test ()
#else
int main ()
#endif
{
    (void)&ZSTD_compress;
    (void)&ZSTD_decompress;
	
	return 0;
}
