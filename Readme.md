## Async Game Engine

__AsEn__ - is a totally asynchronous engine for multicore systems.

Language: minimal C++ 17, preferred C++ 20<br/>
Platforms: Windows, Android, Linux, MacOS, Emscripten (without graphics)<br/>
GraphicsAPI: Vulkan, Metal<br/>
Shaders: GLSL, MSL<br/>

**This is a preview version, available only small part of sources, project will not link!**

Technical documentation:
 * [Build instructions](engine/docs/Build.md)
 * Graphics: [DeviceProperties](engine/docs/DeviceProperties.md)
 * Graphics: [FeatureSet](engine/docs/FeatureSet.md)
 * Graphics features: [Vulkan](engine/docs/VulkanFeatures.md), [Metal](engine/docs/MetalFeatures.md)
 * [Profiling](engine/docs/Profiling.md)
 * [Supported platforms](engine/docs/Platforms.md)

Other docs:
 * [Engine changelog](engine/Changelog.md)
 * [HDR Display](engine/docs/HDR_Display.md)
 * [Licensing information](LICENSE)

Samples and tools:
 * [Resource Editor](samples/res_editor/Readme.md) (Windows only)
 * [Demo](samples/demo/Readme.md) (Cross platform)
 * [Lock-Free Algorithms Sandbox](engine/tools/lfas/Readme.md)
 * [GLSL-Trace](engine/tools/res_pack/shader_trace/Readme.md)
 * [Vulkan: Get Image Z-Curve](engine/tools/vulkan_image_zcurve/Readme.md)
 * [Vulkan Header Generator](engine/tools/vulkan_header_gen/Readme.md)
 * [FeatureSet Generator](engine/tools/res_pack/utils/feature_set_gen/Readme.md)


## rus

__AsEn__ - полностью асинхронный движок для использования в многоядерных системах.

Язык: минимально C++ 17, рекомендуется C++ 20<br/>
Платформы: Windows, Android, Linux, MacOS, Emscripten (без графики)<br/>
Графика: Vulkan, Metal<br/>
Шейдеры: GLSL, MSL<br/>

Описание систем:
 * [Планировщик асинхронных задач](engine/docs/ru/TaskScheduler.md)
 * [Планировщик графических задач](engine/docs/ru/RenderTaskScheduler.md)
 * [Рендер граф](engine/docs/ru/RenderGraph.md)
 * [Файловая система](engine/docs/ru/VirtualFileSystem.md)
 * [HDR дисплеи](engine/docs/ru/HDR_Display.md)

Обсуждение:
 * [форум gamedev](https://gamedev.ru/flame/forum/?id=277212) 
