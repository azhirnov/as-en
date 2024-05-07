#include <android_native_app_glue.h>
#include "TestHelper.h"

extern int AE_Abseil_Test ();

void android_main (struct android_app* state)
{
	AE_Abseil_Test();
    std::exit(0);
}
