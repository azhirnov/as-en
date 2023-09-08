## Async Game Engine

__AsEn__ - is a totally asynchronous engine for multicore systems.

Language: minimal C++ 17, preferred C++ 20<br/>
Platforms: Windows, Android, Linux, MacOS, Emscripten (without graphics)<br/>
GraphicsAPI: Vulkan, Metal<br/>
Shaders: GLSL, MSL<br/>

**Public version**: some features are removed. See `preview` branch for all implemented features (headers only!).

Technical documentation:
 * [Build instructions](AE/engine/docs/Build.md)
 * Graphics: [DeviceProperties](AE/engine/docs/DeviceProperties.md)
 * Graphics: [FeatureSet](AE/engine/docs/FeatureSet.md)
 * Graphics features: [Vulkan](AE/engine/docs/VulkanFeatures.md), [Metal](AE/engine/docs/MetalFeatures.md)
 * [Profiling](AE/engine/docs/Profiling.md)
 * [Supported platforms](AE/engine/docs/Platforms.md)

Other docs:
 * [Engine changelog](AE/engine/Changelog.md)
 * [HDR Display](AE/engine/docs/HDR_Display.md)
 * [Licensing information](AE/LICENSE.md)

Samples and tools:
 * [Resource Editor](AE/samples/res_editor/Readme.md) (Windows only)
 * [Demo](AE/samples/demo/Readme.md) (Cross platform)
 * [Lock-Free Algorithms Sandbox](AE/engine/tools/lfas/Readme.md)
 * [GLSL-Trace](AE/engine/tools/res_pack/shader_trace/Readme.md)
 * [Vulkan: Get Image Z-Curve](AE/engine/tools/vulkan_image_zcurve/Readme.md)
 * [Vulkan Header Generator](AE/engine/tools/vulkan_header_gen/Readme.md)
 * [FeatureSet Generator](AE/engine/tools/res_pack/utils/feature_set_gen/Readme.md)


## rus

__AsEn__ - полностью асинхронный движок для использования в многоядерных системах.

Язык: минимально C++ 17, рекомендуется C++ 20<br/>
Платформы: Windows, Android, Linux, MacOS, Emscripten (без графики)<br/>
Графика: Vulkan, Metal<br/>
Шейдеры: GLSL, MSL<br/>

Описание систем:
 * [Планировщик асинхронных задач](AE/engine/docs/ru/TaskScheduler.md)
 * [Планировщик графических задач](AE/engine/docs/ru/RenderTaskScheduler.md)
 * [Рендер граф](AE/engine/docs/ru/RenderGraph.md)
 * [Файловая система](AE/engine/docs/ru/VirtualFileSystem.md)
 * [HDR дисплеи](AE/engine/docs/ru/HDR_Display.md)

Обсуждение:
 * [форум gamedev](https://gamedev.ru/flame/forum/?id=277212) 
