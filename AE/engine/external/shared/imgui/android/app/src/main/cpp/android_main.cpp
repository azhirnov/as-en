#include <android_native_app_glue.h>
#include "TestHelper.h"

extern int AE_imgui_Test ();

void android_main (struct android_app* state)
{
	AE_imgui_Test();
    std::exit(0);
}
