#include <android_native_app_glue.h>
#include "TestHelper.h"

extern int AE_Abseil_Test ();
extern int AE_AngelScript_Test ();
extern int AE_Brotli_Test ();
extern int AE_GLM_Test ();
extern int AE_Vulkan_Test ();
extern int AE_OpenGL_Test ();
extern int AE_FileSystem_Test ();
extern int AE_Utf8Proc_Test ();
extern int AE_FreeType_Test ();

void android_main (struct android_app* state)
{
# ifdef AE_ENABLE_ANGELSCRIPT
    printf( "AngelScript test: %i\n", AE_AngelScript_Test() );
# endif

# ifdef AE_ENABLE_ABSEIL
    printf( "Abseil test: %i\n", AE_Abseil_Test() );
# endif

# ifdef AE_ENABLE_GLM
    printf( "GLM test: %i\n", AE_GLM_Test() );
# endif

# ifdef AE_ENABLE_VULKAN
    printf( "Vulkan test: %i\n", AE_Vulkan_Test() );
# endif

# ifdef AE_ENABLE_OPENGL
    printf( "OpenGL test: %i\n", AE_OpenGL_Test() );
# endif

# ifdef AE_ENABLE_BROTLI
    printf( "Brotli test: %i\n", AE_Brotli_Test() );
# endif

# ifdef AE_ENABLE_GFS
    printf( "FileSystem test: %i\n", AE_FileSystem_Test() );
# endif

# ifdef AE_ENABLE_UTF8PROC
    printf( "Utf8Proc test: %i\n", AE_Utf8Proc_Test() );
# endif

# ifdef AE_ENABLE_FREETYPE
    printf( "Utf8Proc test: %i\n", AE_FreeType_Test() );
# endif

    std::abort();
}
