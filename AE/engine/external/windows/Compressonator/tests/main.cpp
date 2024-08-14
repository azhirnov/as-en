
#ifndef AE_ENABLE_AMD_COMPRESSONATOR
#	error AE_ENABLE_AMD_COMPRESSONATOR required
#endif

#ifdef ANDROID
# include "TestHelper.h"

extern int AE_Compressonator_Test ()
#else
int main ()
#endif
{
	return 0;
}
