## Vulkan Hader Generator

### Features

* Generates Vulkan function loader.
    - Added `[[nodiscard]]` if function returns `VkResult`.
    - Multi-device support: instance functions in global space, device functions in structure.
    - Dummy functions to avoid crashes.
* Generates Vulkan initialization with specified extensions.<br/>*See `Generator::_GetFeatures()` to configure extensions.*
* Generates constants to string functions which is used in [VulkanSyncLog](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/vulkan_sync_log/Readme.md) module.

### How to use

See [VulkanLoader](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/graphics/Vulkan/VulkanLoader.h).
