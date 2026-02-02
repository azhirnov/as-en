Supported platforms

## Windows

- [x] OS: Win7 (without some features)
- [x] OS: Win8 - Win10
- [x] OS: Win11
- [ ] Architecture: x86 (not tested)
- [x] Architecture: x64
- [ ] Architecture: ARM64 (compiled but not tested)
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
- [ ] Display Cutout

**ARM64**:
- [ ] Scripting
- [ ] SIMD


## Linux

- [x] OS: Ubuntu 23, 24, 25
- [x] Architecture: x64
- [ ] Architecture: ARM64
- [ ] Architecture: RISC-V 64
- [x] Compiler: GCC12, 13, 14
- [x] Compiler: Clang15, 16-20
- [x] CPU info
- [x] Memory info
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
	- [x] BASS
	- [ ] native
- [x] Graphics:
	- Vulkan (by default requires timeline semaphore)
	- Vulkan (without timeline semaphore, compile with `-DAE_VK_TIMELINE_SEMAPHORE=OFF`)
- Vulkan drivers:
	- [x] RADV
	- [x] AMDVLK
	- [x] AMDPRO
- Window system:
	- [x] X11
	- [x] Wayland
- [ ] HDR display (not tested)
- [ ] Profiling
- [ ] Distribution: Flatpack
- [ ] Crash reporting


## Android

- [x] OS: Android 7+ (SDK 24)
- [x] OS: PicoOS (tested on 5.13)
- [x] Architecture: ARMv8, ARMv7
- [ ] Architecture: x86, x64 - supported but not tested
- [x] Compiler: Clang
- [x] CPU info
- [x] Memory info
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
	- [x] BASS
	- [ ] native
- [x] Graphics: Vulkan (without timeline semaphore)
- [x] Display Cutout
- [x] HDR display
- [ ] Multi monitor, desktop mode
- Profiling:
	- [x] Internal performance counters GPU: Adreno/Mali/PowerVR
- [ ] Crash reporting


## MacOS

- [x] OS: MacOS 10.15 - 14.0
- [x] Architecture: ARM64, x64
- [x] Compiler: Clang15 (XCode)
- [x] CPU info
- [x] Memory info
- [ ] Thread affinity - not supported on ARM
- [x] Thread priority
- [x] Thread pause instruction
- [x] Thread: nano/micro sleep
- [x] Window mode: Fullscreen, Borderless
- [x] Async file read/write
- [x] Networking
- [ ] Internal resources
- [ ] Writable folder
- [x] Input: mouse, keyboard
- Audio:
	- [x] BASS
	- [ ] native
- Graphics:
	- [x] Metal API 2.x
	- [x] Metal API 3.x
	- [ ] Metal API 4.x
	- [x] MoltenVk
- [ ] Display Cutout
- [x] HDR display (Vulkan)
- [ ] HDR display (Metal, not tested)
- [x] Offline resource packing
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

- not tested
- deprecated

## OpenXR

in progress

- [ ] OS: PicoVR (PICO 4 Ultra since 5.12.0)
- [ ] Input
