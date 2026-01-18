
#ifndef AE_ENABLE_OPENXR
#	error AE_ENABLE_OPENXR required
#endif

#include "openxr/openxr.h"

#ifdef ANDROID
# include "TestHelper.h"

extern int AE_OpenXR_Test ()
#else
int main ()
#endif
{
	// TODO
	return 0;
}
