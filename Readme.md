
## Async Game Engine

__AsEn__ - is a totally asynchronous engine for desktop and mobile.

Language: C++ 20<br/>
Platforms: Windows, Android, Linux, MacOS<br/>
GraphicsAPI: Vulkan, Metal<br/>
Shaders: GLSL, MSL, SLang<br/>
Stable version: see [Tags](https://github.com/azhirnov/as-en/tags)<br/>

Key features:
 * Scalable task system with stackless coroutines.
 * Multi-threaded command buffer recording.
 * Staging buffer memory manager, per-frame limits for stable FPS.
 * Async file IO on all platforms.
 * Lock-free algorithms and sync primitives.
 * Built-in profiling tools and cross-platform shader debugger.
 * CI/CD to run all tests on all platforms, including graphics tests.

Samples:
 * [Shader Playground](AE/samples/res_editor/Readme.md) with high level scripting, see [Samples overview](AE/samples/res_editor/docs/Samples.md).
 * Low level [Demos](AE/samples/demo/Readme.md) (in progress).

Engine documentation:
 * [Build instructions](AE/docs/engine/Build.md)
 * [Licensing information](AE/LICENSE.md)
 * [Engine Concept](AE/docs/engine/Concept.md)
 * [Changelog](AE/engine/Changelog.md)
 * Render hardware interface: [Vulkan](AE/docs/engine/VulkanRHI.md), [Remote](AE/docs/engine/RemoteRHI.md), [Metal](AE/docs/engine/MetalRHI.md)
 * Graphics: [DeviceProperties](AE/docs/engine/DeviceProperties.md), [FeatureSet](AE/docs/engine/FeatureSet.md)
 * [Graphics Resources](AE/docs/engine/GraphicsResources.md)
 * [Supported platforms](AE/docs/engine/Platforms.md)
 * [Profiling](AE/docs/engine/Profiling.md)
 * [Resource Compilation](AE/docs/engine/ResourceCompilation.md)

Source code:
 * [Engine](AE/engine/src)
 * [Engine tests](AE/engine/tests)
 * [Engine tools](AE/engine/tools)
 * [All samples](AE/samples)

Tools:
 * [Lock-Free Algorithms Sandbox](AE/engine/tools/lfas/Readme.md)
 * [GLSL-Trace](AE/engine/tools/res_pack/shader_trace/Readme.md)
 * [Detect Image Z-Curve](AE/engine/tools/vulkan_image_zcurve/Readme.md)
 * [Vulkan/OpenXR Header Generator](AE/engine/tools/header_gen/Readme.md)
 * [Vulkan Synchronizations Logger](AE/engine/tools/vulkan_sync_log/Readme.md)
 * [FeatureSet Generator](AE/engine/tools/feature_set_gen/Readme.md)
 * [Remote Graphics Device](AE/rem_tools/remote_graphics_device/Readme.md)
 * [Android Remote Control](AE/samples/android_remote_control/Readme.md)


## rus

__AsEn__ - полностью асинхронный движок.

Язык: C++ 20<br/>
Платформы: Windows, Android, Linux, MacOS<br/>
Графика: Vulkan, Metal<br/>
Шейдеры: GLSL, MSL, SLang<br/>
Стабильная версия: [помечена тэгом](https://github.com/azhirnov/as-en/tags)<br/>

Документация по движку:
 * [Планировщик асинхронных задач](AE/docs/engine/TaskScheduler-ru.md)
 * [Планировщик графических задач](AE/docs/engine/RenderTaskScheduler-ru.md)
 * [Рендер граф](AE/docs/engine/RenderGraph-ru.md)
 * [Файловая система](AE/docs/engine/VirtualFileSystem-ru.md)

Заметки:
 * [Асинхронное программирование](AE/docs/papers/AsyncProgramming-ru.md)
 * [Заметки по архитектуре](AE/docs/papers/ArchitectureNotes-ru.md)
 * [Заметки по программированию графики](AE/docs/papers/GraphicsNotes-ru.md)
 * [Заметки по ECS/DOD](AE/docs/papers/ECSNotes-ru.md)
 * [Примеры по С++](AE/docs/papers/CPP_Snipets-ru.md)

Обсуждение:
 * [форум gamedev](https://gamedev.ru/flame/forum/?id=277212)
