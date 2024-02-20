## 01.2024
- Remote input (sensors from Android device)
- Network storage
- VideoImage2 for YCbCr texture


## 11.2023
- VR Video projection sample.
- VR Video player sample (VR to 2D).
- Curved screen support.
- Spherical cube tests.
- Use dynamic variable as per-pass constant, allow to use global slider in multiple passes.


## 10.2023
- Dispersion sample.
- Used Assimp to load models/scenes.
- RayTracing pass for model/scene rendering.


## 09.2023

- rename: ResourceQueue -> DataTransferQueue
- Image export pass
- Buffer export to text format (has same structure as in shader)
- `#include` in scripts, `script_inc` folder.
- 3D camera now supports ReverseZ.
- RayTracing pass.
- Stack size calculation for ray tracing.


## 08.2023

- Script: replace `Input(Controller)` by `Set()`
- Documentation for script.
- RTX samples.


## 07.2023

- Script: replace `Input()` by `Argin/ArgInOut/ArgOut`
- RTAS indirect build
- RTAS indirect build emulation if not supported
- Buffer content history to make it compatible with RTAS indirect build emulation
- Remove `_loadOpGuard`
