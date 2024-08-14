Supported platforms

## Windows

- [x] OS: Win7 (without some features)
- [x] OS: Win8 - Win10
- [ ] OS: Win11 (not tested)
- [ ] Architecture: x86 (not tested)
- [x] Architecture: x64
- [ ] Architecture: ARM64
- [x] Compiler: VisualStudio 2022
- [x] Compiler: VisualStudio 2022 with Clang
- [x] CPU info
- [x] Memory info
- [x] Thread affinity
- [x] Thread priority
- [x] Thread pause instruction
- [x] Thread: nano/micro sleep
- [x] Window mode: Fullscreen, Borderless
- [x] Async file read/write
- [x] Networking
- [ ] Internal resources
- [ ] Writable folder
- [x] Input: mouse, keyboard, touchscreen
- [ ] Input: gamepad
- Audio:
	- [x] BASS
	- [ ] native
- [x] Graphics:
	- Vulkan (by default requires timeline semaphore)
	- Vulkan (without timeline semaphore, compile with `-DAE_VK_TIMELINE_SEMAPHORE=OFF`)
- [x] HDR display (Win10+)
- Debugging / profiling:
	- [x] VisualStudio
	- [x] RenderDoc
	- [x] NSight
	- [ ] NvProf SDK (partially)
	- [ ] RGP (not tested)
- [ ] Crash reporting


## Linux

- [x] OS: Ubuntu 22, 23
- [x] Architecture: x64
- [ ] Architecture: ARM64
- [ ] Architecture: RISC-V 64
- [x] Compiler: GCC12
- [x] Compiler: Clang15
- [x] CPU info
- [ ] Memory info (not tested)
- [x] Thread affinity
- [x] Thread priority
- [x] Thread pause instruction
- [x] Thread: nano/micro sleep
- [ ] Window mode: Fullscreen, Borderless (not tested)
- [x] Async file read/write
- [x] Networking
- [ ] Internal resources
- [ ] Writable folder
- [x] Input: mouse, keyboard
- Audio:
	- [ ] BASS (not tested)
	- [ ] native
- [x] Graphics:
	- Vulkan (by default requires timeline semaphore)
	- Vulkan (without timeline semaphore, compile with `-DAE_VK_TIMELINE_SEMAPHORE=OFF`)
- [ ] HDR display (not tested)
- [ ] Profiling
- [ ] Distribution: Flatpack
- [ ] Crash reporting


## Android

- [x] OS: Android 7+ (SDK 24)
- [x] Architecture: ARMv8, ARMv7
- [ ] Architecture: x86, x64 - supported but not tested
- [x] Compiler: Clang
- [x] CPU info
- [ ] Memory info
- [x] Thread affinity
- [x] Thread priority
- [x] Thread pause instruction
- [x] Thread: nano/micro sleep
- [x] Window mode: Fullscreen, With status bar
- [x] Window mode: Window
- [x] Async file read/write
- [x] Networking
- [x] Internal resources
- [ ] Writable folder
- [x] Input: keyboard, touchscreen
- [ ] Input: external keyboard, gamepad (not tested)
- [x] Input: sensors
- Audio:
	- [ ] BASS (not tested)
	- [ ] native
- [x] Graphics: Vulkan (without timeline semaphore)
- [x] Display Cutout
- [x] HDR display
- Profiling:
	- [x] Internal performance counters GPU: Adreno/Mali/PowerVR
- [ ] Crash reporting


## MacOS

- [x] OS: MacOS 10.15 - 14.0
- [x] Architecture: ARM64, x64
- [x] Compiler: Clang15 (XCode)
- [x] CPU info
- [ ] Memory info
- [ ] Thread affinity
- [ ] Thread priority
- [x] Thread pause instruction
- [x] Thread: nano/micro sleep
- [x] Window mode: Fullscreen, Borderless
- [x] Async file read/write (slow)
- [x] Networking
- [ ] Internal resources
- [ ] Writable folder
- [x] Input: mouse, keyboard
- Audio:
	- [ ] BASS (not tested)
	- [ ] native
- Graphics:
	- [x] Metal API 2.x
	- [x] Metal API 3.x
	- [x] MoltenVk
- [ ] Display Cutout
- [x] HDR display (Vulkan)
- [ ] HDR display (Metal, not tested)
- [x] Offline resource packing (x64 only)
- Profiling:
	- [x] XCode profiler
- [ ] Crash reporting


## iOS

same as MacOS, but not tested


## Emscripten

- [x] TaskScheduler
- [ ] Graphics (WebGL or WebGPU + Remote)
- [ ] Networking


## OpenVR

not tested


## OpenXR

not implemented
