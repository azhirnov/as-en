
## Motivation

Most of opensource frameworks have problems:
* Vulkan backend is copy-pasted from vulkan tutorials and, as a result, has poor performance.
* Vulkan backend has synchronization problems (*use vkconfig with synchronization validation preset to see them*).
* Vulkan backend has many memory allocations, many unnecessary stalls, uses VMA without understanding how it implemented internally.
* Vulkan backend has small number of compatibility checks. Without validation layers Vulkan driver can create resource which description is not supported by hardware, driver will crash somewhen later. [Vulkan Profiles](https://github.com/KhronosGroup/Vulkan-Profiles) is just a first step to get guarantees that all used features are supported, but it doesn't cover limits which is not in Vulkan properties.
* Vulkan backend doesn't support queue ownership transfer for resources, this may cause performance problems on some GPUs.
* Used MoltenVk instead of Metal backend, but MVk has some performance problems (especially on M1+ hardware) and doesn't support some new features.
* Metal backend, if implemented, has low number of features, it just an implementation for RHI which is designed for desktop GPUs and doesn't cover some mobile GPU features.
* Async file IO implemented only in AAA engines.
* Task system is not used, may be except Unity but it is not opensource.
* CPU-side synchronizations implemented through mutexes which is bottleneck in multithreaded programs.
* Used `std::shared_ptr` which is large, slow, is not cache friendly, not compatible with atomics.
* Used strings, maps and other types with small allocations, which is slow.
* Reflection of shader structs is not implemented or allowed only in runtime.
* May engines are not initially designed for multithreading and task system.


## Concept

Engine should helps to write linear code, which is well readable and much easier to test and debug.

For multithreading C++20 has coroutine support, which allows to write asynchronous code as a linear code with pauses in `co_await`. Engine will run coroutines on top of it's task system.

For graphics programming engine has user-defined render techniques. Render technique is completely precompiled and validated for target hardware. Engine guarantees that if render technique is successfully compiled then it is compatible with target feature sets. In runtime, if engine return render technique object then it is guaranteed that all used features are supported by the hardware. You don't need to write branches for specific features, instead you write renderer which uses all passes from render technique.

For memory management engine supports custom gfx memory allocator, descriptor set allocator, RAM allocator, etc. It allows to get full control on memory allocations.

Engine is task-based, file reading request is a task, readback from the GPU memory is a task, command buffer recording is a task. Tasks has dependencies. All tasks can be safely canceled, the whole task sequence can be cancelled too.

Engine should have low-level access for strong optimizations for high loaded code. And should have high-level abstraction for fast prototyping.
