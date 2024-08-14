
#ifndef AE_ENABLE_ASTC_ENCODER
#	error AE_ENABLE_ASTC_ENCODER required
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
