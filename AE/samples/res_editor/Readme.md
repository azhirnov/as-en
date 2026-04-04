## Editor for shaders and procedural content

Content:
* [Features](#features)
* [Controls](#controls)
* [Editor UI](docs/EditorUI.md)
* [Samples](docs/Samples.md)
* [Scripts](docs/Scripts.md)
* [Remote](docs/Remote.md)
* [Download binaries](docs/DownloadBinaries.md)


## Features

![](docs/img/ResEditor.png)

* Renderer is written in scripts.
* Hot reloading for scripts.
* Sliders and color pickers for passing data to the shader.
* Built-in shader debugger and profiler:
	- [GLSL-Trace](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/res_pack/shader_trace/Readme.md).
	- RenderDoc.
* Built-in graphics profilers:
	- NSight, GPA (*in progress*)
	- Adreno/Mali/PowerVR/NV/AMD profilers (see [Engine Profiling](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/Profiling.md)).
* Various 2D & 3D cameras.
* Geometry templates:
	- Spherical cube with tangential projection (skybox, planet rendering)
	- Particles
	- Raw draw calls
	- Terrain (*in progress*)
	- Models/Scenes (loaded using Assimp).
* Compatible with shadertoy (use `Postprocess()` with `Shadertoy` or `ShadertoyVR` flags).
* Supports modern features like a hardware ray tracing, mesh shading, neural shader.
* Supports async streaming (async file and partially uploading in single graphics queue, async transfer queue is not used).
* Video encoder and decoder (using ffmpeg).
* Screenshot capture (dds, png, jpeg and other).
* Supports [HDR display](https://github.com/azhirnov/AsEn-ShaderEditor/blob/main/papers/HDR_Display-en.md).
* Resource export:
	- Image export to engine internal format.
	- Buffer export to binary or text format which reflect buffer structure in shader.
	- Scene export (*in progress*)

**Platforms:**<br/>
* Windows x64
* Linux x64
* MacOS arm64
* Android in progress
* Remote rendering on any platform


## Controls

`Esc` - switch between camera and UI<br/>
`Tab` - show/hide UI<br/>
`F1` - show/hide help with key bindings<br/>
