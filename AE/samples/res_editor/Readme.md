## Editor for shaders and procedural content

Content:
* [Features](#features)
* [Controls](#controls)
* [Editor UI](docs/EditorUI.md)
* [Samples](docs/Samples.md)
* [Scripts](docs/Scripts.md)


## Features

* Renderer is written in scripts.
* Hot reloading.
* Sliders and color pickers for passing data to the shader.
* Built-in shader debugger and profiler:
    - [GLSL-Trace](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/res_pack/shader_trace/Readme.md).
    - RenderDoc API.
    - NSight (*in progress*)
* Various 2D & 3D cameras.
* Geometry templates:
    - Spherical cube with tangential projection (skybox, planet rendering)
    - Particles
    - Raw draw calls
    - Terrain (*in progress*)
    - Models/Scenes (loaded using Assimp).
* Compatible with shadertoy.
* Supports modern features like a hardware ray tracing and mesh shading.
* Supports async streaming (async file and partially upload in single graphics queue, async transfer queue is not used).
* Video encoder and decoder (using [ffmpeg](https://ffmpeg.org/)).
* Screenshot capture (dds, png, jpeg and other).
* Supports HDR display.
* Resource export:
    - Image export to engine internal format.
    - Buffer export to text format which reflect buffer structure in shader.
    - Scene export (*in progress*)

**Platforms:**<br/>
* Windows x64
* Linux x64 (*in progress*)
* Remote rendering on any platform (*in progress*)


## Controls

`Esc` - switch between camera and UI<br/>
`Tab` - show/hide UI<br/>
`F1` - show/hide help with key bindings<br/>


## Release

Precompiled editor with resources:<br/>
[Win64 - v23.10.199](https://drive.google.com/file/d/1kicA14gnlIwC3ah1qPszUznnFa2WCt6s/view?usp=share_link)
[Win64 - v23.09.187](https://drive.google.com/file/d/1e_TyVnyi61r8Fk0iXVc_vIxzYEuoPxZS/view?usp=drive_link)
