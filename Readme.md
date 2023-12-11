## Async Game Engine

__AsEn__ - is a totally asynchronous engine for multicore systems.

Language: minimal C++ 17, preferred C++ 20<br/>
Platforms: Windows, Android, Linux, MacOS, Emscripten (without graphics)<br/>
GraphicsAPI: Vulkan, Metal<br/>
Shaders: GLSL, MSL<br/>

Not available in public version:
 * Metal backend
 * Remote graphics backend
 * Encrypted stream
 * Performance tests

Engine documentation:
 * [Build instructions](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/Build.md)
 * [Licensing information](https://github.com/azhirnov/as-en/blob/dev/AE/LICENSE)
 * [Engine Concept](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/Concept.md)
 * Graphics: [DeviceProperties](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/DeviceProperties.md)
 * Graphics: [FeatureSet](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/FeatureSet.md)
 * Low-level Graphics features: [Vulkan](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/VulkanFeatures.md), [Metal](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/MetalFeatures.md)
 * [Graphics Resources](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/GraphicsResources.md)
 * [Supported platforms](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/Platforms.md)
 * [Profiling](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/Profiling.md)
 * [Resource Compilation](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/ResourceCompilation.md)

Papers:
 * [HDR Display](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/HDR_Display.md)

Samples and tools:
 * [Shader Playground](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/Readme.md), [Samples](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/docs/Samples.md)
 * [Demo](https://github.com/azhirnov/as-en/blob/dev/AE/samples/demo/Readme.md) (in progress)
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

Документация по движку:
 * [Планировщик асинхронных задач](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/TaskScheduler-ru.md)
 * [Планировщик графических задач](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/RenderTaskScheduler-ru.md)
 * [Рендер граф](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/RenderGraph-ru.md)
 * [Файловая система](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/VirtualFileSystem-ru.md)

Статьи:
 * [HDR дисплеи](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/HDR_Display-ru.md)
 * [Асинхронное программирование](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/AsyncProgramming-ru.md)
 * [Заметки по архитектуре](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/ArchitectureNotes-ru.md)
 * [Сферический куб](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/SphericalCube-ru.md)

Обсуждение:
 * [форум gamedev](https://gamedev.ru/flame/forum/?id=277212)
