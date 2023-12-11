Supported platforms

## Windows

- [x] Win7 .. Win10
- [x] VisualStudio 2022
- [x] Async file read/write
- [x] Vulkan (by default requires timeline semaphore)
- [x] Vulkan (without timeline semaphore, compile with `-DAE_VK_TIMELINE_SEMAPHORE=OFF`)
- [ ] Internal resources
- [x] Architecture: x64
- [ ] Architecture: ARMv8
- [ ] Input: gamepad
- [x] HDR display (Win10+)
- [x] Window mode: Fullscreen, Borderless


## Linux

- [x] Ubuntu 22
- [ ] Async file read/write
- [ ] Flatpack
- [x] GCC12
- [x] Clang15
- [x] Architecture: x64
- [ ] Architecture: ARMv8
- [ ] HDR display


## Android

- [x] Android 7+ (SDK 24)
- [x] Architectures: ARMv8, ARMv7
- [ ] Architectures: x86, x64 - supported but not tested
- [x] Vulkan (without timeline semaphore)
- [x] Builtin file system (assets)
- [ ] Async file read/write
- [ ] Input actions: gyroscope, accelerometer, ...
- [ ] Input: hw keyboard, gamepad
- [x] Fullscreen mode
- [x] HDR display


## MacOS

- [x] Metal API 2.x
- [x] Metal API 3.x
- [ ] Async file read/write
- [x] Offline resource packing (x64 only)
- [ ] Scripting (ARM64)
- [x] HDR display (Vulkan)
- [ ] HDR display (Metal)
- [x] MoltenVk


## iOS

not tested


## Emscripten

- [x] TaskScheduler
- [ ] Graphics (WebGL or WebGPU ?)
- [ ] Network


## OpenVR


## OpenXR

