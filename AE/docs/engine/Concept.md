
## Motivation

Most open-source frameworks have the following problems:
* Their Vulkan backends are often copy-pasted from Vulkan tutorials and, as a result, have poor performance.
* Their Vulkan backends have synchronization issues (*use vkconfig with the synchronization validation preset to see them*).
* Their Vulkan backends perform many memory allocations, introduce many unnecessary stalls, and use VMA without understanding how it is implemented internally.
* Their Vulkan backends have only a small number of compatibility checks. Without validation layers, a Vulkan driver can create a resource whose description is not supported by the hardware, and the driver may crash later. [Vulkan Profiles](https://github.com/KhronosGroup/Vulkan-Profiles) are just a first step toward guaranteeing that all used features are supported, but they do not cover limits that are not included in Vulkan properties.
* Their Vulkan backends do not support queue ownership transfer for resources, which may cause performance problems on some GPUs.
* They use MoltenVK instead of a Metal backend, but MoltenVK has some performance issues (especially on M1+ hardware) and does not support some newer features.
* Their Metal backends, if implemented, have a limited feature set. They are merely implementations for an RHI designed for desktop GPUs and do not cover some mobile GPU features.
* Async file I/O is implemented only in AAA engines.
* A task system is not used, except perhaps in Unity, but it is not open-source.
* CPU-side synchronization is implemented through mutexes, which become a bottleneck in multithreaded programs.
* They use `std::shared_ptr`, which is large, slow, not cache-friendly, and not compatible with atomics.
* They use strings, maps, and other types that require small allocations, which is slow.
* Reflection for shader structs is either not implemented or is only available at runtime.
* Many engines are not initially designed for multithreading and task systems.
* New engines uses dynamic rendering feature which is not supported on old mobile devices.


## Concept

The engine should help developers write linear code that is readable and much easier to test and debug.

For multithreading, C++20 provides coroutine support, which allows asynchronous code to be written in a linear style with pauses at `co_await`. The engine will run coroutines on top of its task system.

For graphics programming, the engine provides user-defined render techniques. A render technique is fully precompiled and validated for the target hardware. The engine guarantees that if a render technique is successfully compiled, then it is compatible with the target feature set. At runtime, if the engine returns a render technique object, then it is guaranteed that all used features are supported by the hardware. You do not need to write branches for specific features; instead, you write a renderer that uses all passes from the render technique.

For memory management, the engine supports a custom graphics memory allocator, descriptor set allocator, RAM allocator, and so on. This allows full control over memory allocations.

The engine is task-based: a file read request is a task, a GPU memory readback is a task, and command buffer recording is a task. Tasks have dependencies. All tasks can be safely canceled, and the whole task sequence can be canceled as well.

The engine should provide low-level access for strong optimization of heavily loaded code, while also offering high-level abstractions for fast prototyping.
