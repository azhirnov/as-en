Source folder: [RemoteGraphics backend](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/graphics/Remote/)

## Features

- [x] Device
- [x] Surface, Swapchain
	- [ ] Present to window
- [x] RenderTaskScheduler
- CommandBuffer, Contexts:
	- [x] Transfer
	- [x] Compute
	- [x] Graphics
	- [x] Draw
	- [x] RayTracing
	- [x] ASBuild
	- [ ] Video
	- [ ] AsyncDraw
- [x] ResourceManager
- [x] QueryManager
- [x] ShaderTrace
- Profiling
	- [x] WriteTimestamp
	- [x] ARM CPU Perf counters
	- [x] Adreno GPU Perf counters
	- [x] Mali GPU Perf counters
	- [x] PowerVR GPU Perf counters
- [x] Draw UI & Profiling on Host


## Platforms

Device:
- [x] Windows
- [x] MacOS Vulkan
- [ ] MacOS Metal
- [x] Android 32bit
- [x] Android 64bit
- [ ] Linux

Host:
- [x] Windows
- [ ] Linux
- [ ] MacOS
- [ ] Android


## Issues

- When hostQueryReset=false CmdBufPool::IsFirst may works incorrect
