
#ifndef AE_ENABLE_TINYGLTF
# error AE_ENABLE_TINYGLTF required
#endif

#ifdef _MSC_VER
#   pragma warning (push, 0)
#   pragma warning (disable: 4018)
#   pragma warning (disable: 4267)
#   pragma warning (disable: 4101)
#endif

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#define TINYGLTF_USE_CPP14
//#define TINYGLTF_NO_FS
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tiny_gltf.h"

#ifdef _MSC_VER
#   pragma warning (pop)
#endif

#ifdef ANDROID
# include "TestHelper.h"

extern int AE_TinyglTF_Test ()
#else
int main ()
#endif
{
    return 0;
}
