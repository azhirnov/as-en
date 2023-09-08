
#include <vulkan/vulkan.h>

#ifdef ANDROID
#   include "TestHelper.h"
extern int AE_Vulkan_Test ()
#else
int main ()
#endif
{
    VkInstance  inst;
    (void)(inst);
    return 0;
}
