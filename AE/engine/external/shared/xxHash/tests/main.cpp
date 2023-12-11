
#ifndef AE_ENABLE_XXHASH
#   error AE_ENABLE_XXHASH required
#endif

#include "xxhash.h"

#ifdef ANDROID
# include "TestHelper.h"

extern void AE_xxHash_Test ()
#else
int main ()
#endif
{
    char    buffer [128];
    XXH64_hash_t hash = XXH64( buffer, 128, 0 );
    (void)(hash);

    return 0;
}
