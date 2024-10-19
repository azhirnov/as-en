[![](https://github.com/azhirnov/as-en/actions/workflows/windows.yml/badge.svg)](https://github.com/azhirnov/as-en/actions/workflows/windows.yml)
[![](https://github.com/azhirnov/as-en/actions/workflows/linux.yml/badge.svg)](https://github.com/azhirnov/as-en/actions/workflows/linux.yml)
[![](https://github.com/azhirnov/as-en/actions/workflows/android.yml/badge.svg)](https://github.com/azhirnov/as-en/actions/workflows/android.yml)
[![](https://github.com/azhirnov/as-en/actions/workflows/macos.yml/badge.svg)](https://github.com/azhirnov/as-en/actions/workflows/macos.yml)

## Async Game Engine

__AsEn__ - is a totally asynchronous engine for desktop and mobile.

Language: minimal C++ 17, preferred C++ 20<br/>
Platforms: Windows, Android, Linux, MacOS, Emscripten (without graphics)<br/>
GraphicsAPI: Vulkan, Metal<br/>
Shaders: GLSL, MSL<br/>

Key features:
 * Scalable task system with optional C++20 coroutine support.
 * Multi-threaded command buffer recording.
 * Staging buffer memory manager, per-frame limits for stable FPS.
 * Async file IO on all platforms.
 * Lock-free algorithms and sync primitives.
 * Built-in profiling tools and cross-platform shader debugger.
 * CI/CD to run all tests on all platforms, including graphics tests.

Engine documentation:
 * [Build instructions](AE/docs/engine/Build.md)
 * [Licensing information](AE/LICENSE.md)
 * [Engine Concept](AE/docs/engine/Concept.md)
 * [Changelog](AE/engine/Changelog.md)
 * Low-level Graphics features: [Vulkan](AE/docs/engine/VulkanFeatures.md), [Remote](AE/docs/engine/RemoteGraphicsFeatures.md), [Metal](AE/docs/engine/MetalFeatures.md)
 * Graphics: [DeviceProperties](AE/docs/engine/DeviceProperties.md)
 * Graphics: [FeatureSet](AE/docs/engine/FeatureSet.md)
 * [Graphics Resources](AE/docs/engine/GraphicsResources.md)
 * [Supported platforms](AE/docs/engine/Platforms.md)
 * [Profiling](AE/docs/engine/Profiling.md)
 * [Resource Compilation](AE/docs/engine/ResourceCompilation.md)

Papers:
 * [HDR Display](AE/docs/papers/HDR_Display.md)
 * [GPU Benchmarks](AE/docs/papers/GPU_Benchmarks.md)

Samples:
 * [Shader Playground](AE/samples/res_editor/Readme.md) with high level scripting, see [Samples overview](AE/samples/res_editor/docs/Samples.md).
 * Low level [Demos](AE/samples/demo/Readme.md) (in progress).

Tools:
 * [Lock-Free Algorithms Sandbox](AE/engine/tools/lfas/Readme.md)
 * [GLSL-Trace](AE/engine/tools/res_pack/shader_trace/Readme.md)
 * [Detect Image Z-Curve](AE/engine/tools/vulkan_image_zcurve/Readme.md)
 * [Vulkan Header Generator](AE/engine/tools/vulkan_header_gen/Readme.md)
 * [Vulkan Synchronizations Logger](AE/engine/tools/vulkan_sync_log/Readme.md)
 * [FeatureSet Generator](AE/engine/tools/feature_set_gen/Readme.md)
 * [Remote Graphics Device](AE/engine/tools/remote_graphics_device/Readme.md)


## rus

__AsEn__ - полностью асинхронный движок.

Язык: минимально C++ 17, рекомендуется C++ 20<br/>
Платформы: Windows, Android, Linux, MacOS, Emscripten (без графики)<br/>
Графика: Vulkan, Metal<br/>
Шейдеры: GLSL, MSL<br/>

Документация по движку:
 * [Планировщик асинхронных задач](AE/docs/engine/TaskScheduler-ru.md)
 * [Планировщик графических задач](AE/docs/engine/RenderTaskScheduler-ru.md)
 * [Рендер граф](AE/docs/engine/RenderGraph-ru.md)
 * [Файловая система](AE/docs/engine/VirtualFileSystem-ru.md)

Статьи:
 * [HDR дисплеи](AE/docs/papers/HDR_Display-ru.md)
 * [Сферический куб](AE/docs/papers/SphericalCube-ru.md)

Заметки:
 * [Асинхронное программирование](AE/docs/papers/AsyncProgramming-ru.md)
 * [Заметки по архитектуре](AE/docs/papers/ArchitectureNotes-ru.md)
 * [Заметки по программированию графики](AE/docs/papers/GraphicsNotes-ru.md)

Обсуждение:
 * [форум gamedev](https://gamedev.ru/flame/forum/?id=277212)
