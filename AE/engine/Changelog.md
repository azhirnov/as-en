
## 23.09.xxx


## 23.08.183

 - Graphics: reflection for render technique, pipeline, SBT names
 - Graphics: improved syncs in RG
 - ResLoader: KTX support
 - Scripting: function arguments and comments for C++ reflection


## 23.07.178

 - cmake: Precompiled headers
 - cmake: Prebuild libraries
 - AsyncDataSource refactoring
 - Graphics: VK_KHR_fragment_shader_barycentric
 - Graphics: VK_EXT_fragment_shader_interlock
 - Base: OrbitalCamera
 - Base: FPV/FPS camera: limit rotation on Y axis


## 23.06.175

 - Graphics: VK_KHR_fragment_shading_rate
 - Graphics: buffer reference (GLSL/MSL)
 - Platform: fixed VR emulator


## 23.05.162

 - Graphics: unified context methods validation
 - Graphics: buffer uniform type validation
 - Graphics: image uniform type validation
 - Threading: queue/thread type refactoring (Worker -> PerFrame, ...).


## 23.04.153

 - Graphics: NVPerf integration
 - Math: FPVCamera, FlightCamera
 - Profiling: ARM perf counters
 - Profiling: perf counters visualization
 - Platform: multitouch gesture recognition


## 23.03.147

 - Graphics: lock-free swapchain
 - Graphics: ycbcr sampler, multiplanar image
 - Video: ffmpeg encoder / decoder


## 23.02.143

 - Graphics: improved RenderGraph
 - Graphics/Vulkan/Debug: clear image in RP when used loadOp = don't care and in barrier when used undefined layout.
 - Graphics: recreate swapchain asynchroniuosly.
 - Threading: refactoring for coroutines
 - Emscripten initial support
 - ShaderTrace: RayQuery support, ray tracing payload logging, VS IDE output format.
 - MacOS: support for x64
 - Threading: Synchronized<> type
 - Graphics: refactoring for immediatlly/delayed resource destruction


## 23.01.138

- Graphics: deferred pipeline barriers
- Scripting: preprocessor
- Scripting: generate header to enable autocomplete in IDE
- Graphics: RenderGraph


## 22.12.136

- AsyncMutex
- EThread -> ETaskQueue, EThread2 -> EThread
- VCommandBuffer can be migrated to another thread
- Metal API
- Metal: mesh shader
- Vulkan: recursive ray tracing
- Graphics: device features/properties refactoring


## 22.11.129

- async data source (windows) + tests
- classes: SharedMem & TsSharedMem
- WorkerThread replaced by ThreadManager
- RenderTaskScheduler refactoring
- Allocator refactoring
- VK_EXT_mesh_shader instead of VK_NV_mesh_shader
- added VK_KHR_ray_tracing_maintenance1
- VK RTAS serialization/deserialization
- graphics resource manager: added GetResourcesOrThrow()
- removed GLM_CONSTEXPR