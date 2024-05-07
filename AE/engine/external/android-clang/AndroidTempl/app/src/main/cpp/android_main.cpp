#include <android_native_app_glue.h>
#include "TestHelper.h"

extern int AE_TEST_NAME ();

void android_main (struct android_app* state)
{
	AE_TEST_NAME();
    std::exit(0);
}
