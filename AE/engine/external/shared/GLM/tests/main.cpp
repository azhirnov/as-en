
#ifndef AE_ENABLE_GLM
# error AE_ENABLE_GLM required
#endif

#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_CXX17
#define GLM_FORCE_VEC_EQUAL_OP
#define GLM_FORCE_CTOR_INIT
#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS

#include "glm.hpp"

#ifdef ANDROID
# include "TestHelper.h"

extern int AE_GLM_Test ()
#else
int main ()
#endif
{
    using namespace glm;

    (void)(vec2(1.0f) * 2.0f);

# if GLM_CONFIG_SIMD == GLM_DISABLE
    static_assert(all(ivec2(1) == ivec2(1,1)), "1");
# else
    if (! all(ivec2(1) == ivec2(1,1)))
        return 1;
# endif

    return 0;
}
