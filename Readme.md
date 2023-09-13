## Async Game Engine

__AsEn__ - is a totally asynchronous engine for multicore systems.

Language: minimal C++ 17, preferred C++ 20<br/>
Platforms: Windows, Android, Linux, MacOS, Emscripten (without graphics)<br/>
GraphicsAPI: Vulkan, Metal<br/>
Shaders: GLSL, MSL<br/>

**Public version**: some features are removed.

Technical documentation:
 * [Build instructions](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/Build.md)
 * [Licensing information](https://github.com/azhirnov/as-en/blob/dev/AE/LICENSE.md)
 * [Engine Concept](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/Concept.md)
 * Graphics: [DeviceProperties](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/DeviceProperties.md)
 * Graphics: [FeatureSet](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/FeatureSet.md)
 * Graphics features: [Vulkan](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/VulkanFeatures.md), [Metal](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/MetalFeatures.md)
 * [Graphics Resources](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/GraphicsResources.md)
 * [Supported platforms](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/Platforms.md)
 * [Profiling](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/Profiling.md)
 * [Resource Compilation](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/ResourceCompilation.md)
 * [HDR Display](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/HDR_Display.md)

Samples and tools:
 * [Resource Editor](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/Readme.md) (Windows only)
 * [Demo](https://github.com/azhirnov/as-en/blob/dev/AE/samples/demo/Readme.md) (Cross platform)
 * [Lock-Free Algorithms Sandbox](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/lfas/Readme.md)
 * [GLSL-Trace](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/res_pack/shader_trace/Readme.md)
 * [Detect Image Z-Curve](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/vulkan_image_zcurve/Readme.md)
 * [Vulkan Header Generator](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/vulkan_header_gen/Readme.md)
 * [Vulkan Synchronizations Logger](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/vulkan_sync_log/Readme.md)
 * [FeatureSet Generator](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/feature_set_gen/Readme.md)


## rus

__AsEn__ - полностью асинхронный движок для использования в многоядерных системах.

Язык: минимально C++ 17, рекомендуется C++ 20<br/>
Платформы: Windows, Android, Linux, MacOS, Emscripten (без графики)<br/>
Графика: Vulkan, Metal<br/>
Шейдеры: GLSL, MSL<br/>

Статьи:
 * [Планировщик асинхронных задач](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/ru/TaskScheduler.md)
 * [Планировщик графических задач](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/ru/RenderTaskScheduler.md)
 * [Рендер граф](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/ru/RenderGraph.md)
 * [Файловая система](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/ru/VirtualFileSystem.md)
 * [HDR дисплеи](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/ru/HDR_Display.md)
 * [Многопоточное программирование](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/ru/AsyncProgramming.md)

Обсуждение:
 * [форум gamedev](https://gamedev.ru/flame/forum/?id=277212) 
