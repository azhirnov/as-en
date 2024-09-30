## 09.2024
- Tests: GPU performance tests.
- Sample: procedural sphere without geometry.
- Sample: Visibility buffer.


## 08.2024
- Tests with sRGB blend/filter.
- Sample: render scene to cubemap + fisheye projection


## 07.2024
- Sample: SdfFont 2d/3d, regular/bold/outline
- Sample: anti-aliased grid 2d/3d
- Sample: shading rate
- Tool: color gradient
- Tool: easing functions
- Tool: noise editor for circle/terrain/cubemap/volume
- Image compression pass + test (BC, ETC, ASTC)
- Improved shader debugger for UnifiedGeometry, SphericalCube, ModelGeomSource
- 'F4' to copy sliders state to clipboard


## 04.2024
- Remote graphics & profiling
- Tests
- Sample: MaterialDepthBuffer
- Shader debugging for UnifiedGeometry


## 01.2024
- Remote input (sensors from Android device)
- Network storage
- VideoImage2 for YCbCr texture


## 11.2023
- Sample: VR Video projection.
- Sample: VR Video player (VR to 2D).
- Curved screen support.
- Spherical cube tests.
- Use dynamic variable as per-pass constant, allow to use global slider in multiple passes.


## 10.2023
- Sample: Dispersion.
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
- Samples: RTX.


## 07.2023

- Script: replace `Input()` by `Argin/ArgInOut/ArgOut`
- RTAS indirect build
- RTAS indirect build emulation if not supported
- Buffer content history to make it compatible with RTAS indirect build emulation
- Remove `_loadOpGuard`
