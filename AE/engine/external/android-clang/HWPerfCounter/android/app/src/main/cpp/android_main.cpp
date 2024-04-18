#include <android_native_app_glue.h>
#include "TestHelper.h"

extern "C" int AE_HWPerfCounter_Test ();

void android_main (struct android_app* state)
{
	AE_HWPerfCounter_Test();
    std::abort();
}
