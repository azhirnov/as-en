
## Base

### System info

- CPU info:
	- [x] number and type of cores.
	- [x] vendor, name, supported instructions.
	- [x] cache size.
- CPU performance:
	- [x] context switch counter
	- [x] core frequency

### Data Source

- [x] In memory data source with sequential/random access.
- [x] STL file stream (sequential access).
- [x] WinAPI / Unix file with sequential/random access.
- [x] Buffered stream.
- Compressed stream:
	- [x] Brotli (only sequential access).
	- [x] ZStd (only sequential access).
	- [ ] LZ4 sequential/random access.

### Math

- [x] Float8, Float16, Bool32 types.
- [x] Vector 2/3/4, Quaternion, Matrix 2/3/4 on top of GLM.
- [ ] Low level SIMD types (SSE, AVX, Neon, SVE).
- [x] Wrappers: Bytes, Radians.
- [x] Physical dimensions.
- [x] Color types.
- [x] Fractional, PowerOf2, Percent, Range, Spline types.
- **2D:**
	- [x] Rectangle
- **3D:**
	- [x] Transform type.
	- [x] Camera types: Flight, FirstPerson, Orbital.
	- [x] Frustum type.
	- [x] Shapes: AABB, Sphere.

### Other

- [x] UTF-8 string


## Graphics

Low-level Graphics features:
- [Vulkan](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/VulkanFeatures.md)
- [Metal](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/MetalFeatures.md)

All graphics API wrappers supports:
* Multithreaded command buffer recording
* Only precompiled shaders
* Uses [FeatureSet](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/FeatureSet.md) to detect hardware capabilities

### Pipeline Compiler

Designed to compile pipelines and render techniques.
[Details](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/ResourceCompilation.md).


## Networking

- [x] Low level TCP / UDP Sockets.
- [x] High level messages on top of reliable/unreliable TCP channel.
- [ ] High level messages on top of UDP.


## Virtual File System

- [x] Used hash of filename instead of strings.
- [x] Static disk storage with fast searching.
- [x] Dynamic disk storage.
- [x] Static archive.
- [ ] Dynamic archive.
- [x] Network storage.


## Threading

- Task system:
	- [x] Lock-free task queue
	- [x] C++20 coroutine
	- [x] Async mutex
	- [x] Promise (if C++20 coroutines are not supported)
	- [x] Thread specialization: PerFrame, Renderer, Background, FileIO
- Async Files:
	- [x] WinAPI with IO Completion port
	- [x] Android/Linux with AIO
	- [x] Linux with io_uring
	- [x] Linux/macOS with POSIX AIO
- Synchronization primitives:
	- [x] Barrier
	- [x] Semaphore
	- [x] SyncEvent
	- [x] SpinLock
	- [x] Data race detector
- Thread-safe data types:
	- [x] Synchronized<>
- Thread-safe allocators:
	- [x] Lock-free linear allocator
	- [x] Lock-free dynamic/static fixed block allocator
	- [x] Stack allocator
- Thread-safe containers:
	- [x] Lock-free Chunk list
	- [x] Lock-free dynamic/static indexed pool
	- [x] Lock-free static queue


## Platform

- Input Surface abstraction for:
	- [ ] Hardware camera
- Output Surface abstraction for:
	- [x] Window
	- [ ] VR display
- Input abstraction for:
	- [x] keyboard
	- [x] mouse
	- [x] touchscreen
	- [x] sensors
	- [x] remote controls/sensors
