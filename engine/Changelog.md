
## 22.12..136

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
