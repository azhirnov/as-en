
Content:
* [Scripts tab](#scripts-tab)
* [Editor tab](#editor-tab)
* [Graphics tab](#graphics-tab)
* [Profiler windows](#profiler-windows)

# Editor UI

## Scripts tab

Draws a file tree for the folder that is specified in the config:
```
cfg.ScriptDir( "folder" );
```
On click, the script will be compiled and will start rendering when it is ready. Resources will be loaded asynchronously.

## Editor tab

### Debugger

Allows to run shader debugger for the pixel under cursor.

Select graphics/compute pass:<br/>
![](img/UI-Debugger-1.png)

Select debugging mode:<br/>
![](img/UI-Debugger-2.png)

Select shader stage:<br/>
![](img/UI-Debugger-3.png)

When the shader debugger is set up, you can press `G` to record the shader trace.

To enable debugging for `Postprocess` or `ComputePass` add line `pass.AddFlag( EPassFlags::Enable_ShaderTrace );`.

To enable debugging for `SceneGraphicsPass` you need to manually add debug pipelines.
See [DebugPipe](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/tests/DebugPipe.as) sample for VS and FS debugging, or see [MeshShader](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/samples-3d/MeshShader-Cubes.as) sample for TS, MS and FS debugging.

### Capture

Press `I` key to save screenshot with specified format.<br/>
![](img/UI-Capture-1.png)

Press `U` to begin/end video recording.<br/>
Hardware acceleration will be used automatically if it is supported.<br/>
![](img/UI-Capture-2.png)

### Sliders

All sliders for all passes that are defined in the current script will be added here.<br/>
![](img/UI-Sliders.png)

### Statistics

Draw some statistics:
* Cursor position.
* Screen color under the cursor.

## Graphics tab

**Surface scale** - allows to change surface size, to regulate the workload on the fragment shader.<br/>
**Linear filter** - enable linear filter when blit the image to the surface, this will add a slight blur effect.<br/>
![](img/UI-Graphics-1.png)

**Surface format** - allows to change window surface format, `RGBA16F | Extended_sRGB` - enables [HDR output](https://github.com/azhirnov/AsEn-ShaderEditor/blob/main/papers/HDR_Display-en.md).<br/>
![](img/UI-Graphics-2.png)

**Present mode** - allows to change present mode. `FIFO` - enable vertical synchronization.<br/>
![](img/UI-Graphics-3.png)


# Profiler windows

See [Engine Profiling](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/Profiling.md) docs.
