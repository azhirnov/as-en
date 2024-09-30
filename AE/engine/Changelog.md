## 24.09.258

- added PerformanceStat instead of CpuPerformance class
- CMake: unity build
- Profiler: add GeneralProfiler and RemoteGeneralProfiler with cpu usage, mem usage, etc
- Serializing: Bit packing moved to separate BitSerializer
- Vulkan: add VK_NV_clip_space_w_scaling
- Vulkan: add VK_EXT_subgroup_size_control
- add AE_LICENSE_* to shader code, allow to disable code with unsupported license


## 24.08.254

- Profiling: NVML runtime profiler
- Profiling: used AMD GPUPerfAPI tp set stable clock
- Scripting: enable on MacOS ARM64
- Vulkan: add VK_KHR_shader_quad_control, GL_EXT_shader_quad
- Vulkan: add VK_KHR_shader_maximal_reconvergence, GL_EXT_maximal_reconvergence
- Vulkan: add VK_NV_ray_tracing_validation
- CMake: update to 3.18


## 24.07.249

- Graphics: remove some Host_Write -> X barriers, remove UploadMemoryBarrier, HasUploadMemoryBarrier, UploadMemory.
- Profiling: graphics profiler uses PVR timings
- ShaderTrace: float16 support


## 24.05.247

- Graphics: refactoring for staging buffer (removed per queue static buffer, fixed data race for readback).
- Profiling: used ARM HWCPipe v2
- Profiling: added Adreno GPU performance counters
- Profiling: improved UI
- AssetPacker: add support for BC, ETC, ASTC compression


## 24.04.243

- Graphics: remote graphics device v2
- Graphics: added CmdBufExeIndex to start render tasks in different order
- Profiling: added PowerVR performance counters
- Graphics: allow to skip unsupported descriptor set layout
- Android: update to NDK 26.3


## 24.03.236

- Video: improved decoding/encoding using ffmpeg
- Custom CI/CD


## 24.02.228

- UI Editor
- Threading: Unix Async file (Android/Linux/MacOS)
- Audio: BASS playback


## 24.01.222

- Base: Unix file
- Base: zstd stream
- Networking: reliable/unreliable TCP channel
- Android: remote profiler, remote input source
- Android: dynamic loading for API26 functions
- Graphics|Android: added VK_ANDROID_external_memory_android_hardware_buffer for ycbcr sampler
- Graphics: EPixelFormatExternal for ycbcr sampler
- Added support for MSVC + ClangCL compiler
- Graphics: upload/readback multiplanar image
- Graphics: ImGUI renderer


## 23.12.214

- Graphics: remote graphics server
- CMake: added AE_ENABLE_EXCEPTIONS to switch on/off exceptions and RTTI
- CMake: added AE_ENABLE_LOGS
- Graphics: added VK_KHR_pipeline_executable_properties
- Graphics: added MoltenVk (MacOS)
- Graphics: BeginFrame() refactoring, now it completes synchronously
- Android: bugfix, run unit and performance tests
- Threading: progressive sleep if no tasks to process
- Networking: AsyncConnect() for TcpSocket


## 23.11.207

- Networking: TCP channel
- Networking: IPv6 support
- LfIndexedPool2 replaced by LfIndexedPool3
- Removed: LfIndexedPool2, LfFixedBlockAllocator
- Rename LfIndexedPool3 -> LfIndexedPool, LfFixedBlockAllocator2 -> LfFixedBlockAllocator
- Added: ChunkList, MemChunkList, LfChunkList
- VFS: network storage client/server
- VFS: network storage server application


## 23.10.199

- CMake: remove AE_NO_EXCEPTIONS
- Graphics: fixed destruction for PipelinePack
- Scripting: allow to include script via `#include ""`
- Math: fixed infinity perspective and reverse Z
- Graphics: update to VMA 3.0
- Graphics: VK_KHR_cooperative_matrix instead of VK_NV_cooperative_matrix
- Graphics: added UploadBufferDesc
- Graphics: FeatureSet: renamed min* to max*.
- Graphics: in RG remove barriers: CopyDst -> CopyDst, BlitDst -> BlitDst, in most cases they are redundant.
- PipelineCompiler: improved 'packed_*' and 'DeviceAddress' types.
- Platform: added IWindow::SetMode() to switch between fullscreen and windowed mode.
- Graphics: added VDevice::GetMemoryTypeBits() to speedup memory type search.
- PipelineCompiler: rename HitGroupStride to MaxRayTypes or 0, remove sbtRecordStride


## 23.09.186

- WinAPI: fixed incorrect render area when switching to/from HDR mode.


## 23.08.183

- Graphics: reflection for render technique, pipeline, SBT names
- Graphics: improved syncs in RG
- ResLoader: KTX support
- Scripting: function arguments and comments for C++ reflection


## 23.07.178

- CMake: Precompiled headers
- CMake: Prebuild libraries
- AsyncDataSource refactoring
- Graphics: VK_KHR_fragment_shader_barycentric
- Graphics: VK_EXT_fragment_shader_interlock
- Math: OrbitalCamera
- Math: FPV/FPS camera: limit rotation on Y axis


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
- Graphics: recreate swapchain asynchronously.
- Threading: refactoring for coroutines
- Emscripten initial support
- ShaderTrace: RayQuery support, ray tracing payload logging, VS IDE output format.
- MacOS: support for x64
- Threading: Synchronized<> type
- Graphics: refactoring for immediately/delayed resource destruction


## 23.01.138

- Graphics: deferred pipeline barriers
- Scripting: preprocessor
- Scripting: generate header to enable autocomplete in IDE
- Graphics: RenderGraph


## 22.12.136

- Threading: AsyncMutex
- Threading: EThread -> ETaskQueue, EThread2 -> EThread
- Vulkan: VCommandBuffer can be migrated to another thread
- Metal API
- Metal: mesh shader
- Vulkan: recursive ray tracing
- Graphics: device features/properties refactoring


## 22.11.129

- Base: async data source (windows) + tests
- Base: classes: SharedMem & TsSharedMem
- Threading: WorkerThread replaced by ThreadManager
- Graphics: RenderTaskScheduler refactoring
- Base: Allocator refactoring
- Graphics: VK_EXT_mesh_shader instead of VK_NV_mesh_shader
- Graphics: added VK_KHR_ray_tracing_maintenance1
- Graphics: VK RTAS serialization/deserialization
- Graphics: graphics resource manager: added GetResourcesOrThrow()
- Base: removed GLM_CONSTEXPR
