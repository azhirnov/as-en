
#ifndef AE_ENABLE_VMA
# error AE_ENABLE_VMA required
#endif

#include "vk_mem_alloc.h"

#ifdef ANDROID
# include "TestHelper.h"

extern int AE_VMA_Test ()
#else
int main ()
#endif
{
	return 0;
}
