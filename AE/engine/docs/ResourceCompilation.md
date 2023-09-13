# Resource compilation

Engine specific:
* All shaders must be precompiled and validated.
* Engine uses only binary data, scripts and configs are allowed only at the resource compilation stage.
* All graphics resources must have at least one [FeatureSet](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/FeatureSet.md), which is used for validation.
* Engine guarantees that if a resource is compiled with FeatureSet and this FeatureSet is supported by the GPU then the resource is compatible with the GPU. 


# Pipeline Compiler

Some graphics resources such a shader binaries, pipeline, render pass, descriptor set layout and sampler descriptions are stored in the PipelinePack.<br/>
Pipeline compiler converts these description from multiple scripts to a single binary file.

Documentation for each script function is auto-generated and stored in '[pipeline_compiler.as](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/scripts/pipeline_compiler.as)', you can include this file in your script.

**PipelineCompiler** is a standalone dynamic library with a single function:
```cpp
extern "C" bool CompilePipelines (const PipelinesInfo *info);
```

## Render Pass

Render pass description is divided on CompatibleRenderPass and RenderPass.

**CompatibleRenderPass** - contains full render pass description but allows to change some states which doesn't break the render pass compatibility according to the [Vulkan spec](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#renderpass-compatibility).

**RenderPass** - is a specialization of the CompatibleRenderPass which overrides small number of parameters:
* Attachment layout (initial, final, in all subpasses)
* Attachment load and store operations.

## RenderTechnique

* Contains graphics and compute pass sequence which should be executed in these order.
* Contains render passes which is linked to the graphics passes.
* Contains pipeline set for passes, they will be loaded immediately when RenderTechnique is loaded and destroyed when RenderTechnique is destroyed.
* Contains relative resources: descriptor set layouts, pipeline layouts, samplers and other, which is used in render passes and pipelines.

Note: In single PipelinePack pipelines will be shared across the RenderTechniques to avoid duplication and to minimize pipeline compilation time. But you should request pipeline for the specific RenderTechnique instead of using the same pipeline for multiple RenderTechniques

### GraphicsPass

* Used to execute single subpass of the render pass.
* Sequence of graphics passes must be same as sequence of render pass subpasses.
* May contains per-pass descriptor set.
* Supports graphics, mesh and tile pipelines.

### ComputePass

* Can be used for compute dispatches or for ray tracing.
* May contains per-pass descriptor set.


## Pipelines

### PipelineTemplate and PipelineSpecialization concept

**PipelineTemplate** contains shaders, pipeline layout and some other parameters. Mutable parameters like a shader specialization constants will be defined in **PipelineSpecialization**.

In runtime you can get already created PipelineSpecialization from RenderTechnique or create new PipelineSpecialization from PipelineTemplate using `IResourceManager::Create***Pipeline (const ***PipelineDesc &)`.

There are PipelineTemplates:
* ComputePipeline
* GraphicsPipeline
* MeshPipeline
* RayTracingPipeline
* TilePipeline

There are PipelineSpecializations:
* ComputePipelineSpec
* GraphicsPipelineSpec
* MeshPipelineSpec
* TilePipelineSpec


### GLSL shaders

GLSL shaders are compatible with Vulkan and Metal backends. For Metal backend they are compiled to SPIRV, then using SPIRV-Cross they are converted to the MSL and then compiled to Metal bytecode. For Windows it requires [MetalTools for Windows](https://developer.apple.com/download/all/?q=metal%20developer%20tools%20for%20windows).

**Shaders will have additional built-ins:**

Macros for current shader type:<br/>
`SH_VERT` - vertex shader<br/>
`SH_TESS_CTRL` - tessellation control shader (*selects tessellation level*)<br/>
`SH_TESS_EVAL` - tessellation evaluation shader (*executed for each vertex after tessellation*)<br/>
`SH_GEOM` - geometry shader<br/>
`SH_FRAG` - fragment (pixel) shader<br/>
`SH_COMPUTE` - compute shader<br/>
`SH_TILE` - tile shader<br/>
`SH_MESH_TASK` - task shader (*also called amplification or object, generates mesh shader invocations*)<br/>
`SH_MESH` - mesh shader<br/>
`SH_RAY_GEN` - ray generation shader (*entry point for recursive ray tracing*)<br/>
`SH_RAY_AHIT` - ray any hit (*executed when ray intersects with any triangle*)<br/>
`SH_RAY_CHIT` - ray closest hit (*executed when ray tracer found nearest intersection*)<br/>
`SH_RAY_MISS` - ray miss (*executed when no intersection is found*)<br/>
`SH_RAY_INT` - ray intersection (*to write custom intersection function*)<br/>
`SH_RAY_CALL` - callable shader (*can be used only in ray tracing shaders*)<br/>

Extensions are added depends used FeatureSets in the shader. If feature in FeatureSet is `RequireTrue` then extension is marked as `required`.


### MSL shaders

**Shaders will have additional built-ins:**

Always added:
```cpp
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
```

If ray tracing feature is supported:
```cpp
#include <metal_raytracing>
using namespace raytracing;
```

If atomics are supported:
```cpp
#include <metal_atomic>
#define AE_HAS_ATOMICS 1
```

### AE-Style Shader Preprocessor

In scrips use:
```cpp
GlobalConfig cfg;
cfg.SetPreprocessor( EShaderPreprocessor::AEStyle );
``` 

New scalar types: bool, byte, ubyte, short, ushort, int, uint, long, ulong, float, double, half.<br/>
New vector types: same as scalar with 2, 3, 4 suffix, example: uint3.<br/>
New matrix types: float, half, double with suffix 2x2 ... 4x4, example float3x4.<br/>

GLSL-specific:<br/>
`gl::` namespace for types.<br/>
`gl.` namespace for global functions.<br/>
See [aestyle.glsl.h](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/shaders/aestyle.glsl.h) for all types, this header can be used to enable auto-complete in IDE.

MSL-specific:<br/>


## C++ Reflection

Reflection is supported for `struct` types:
* Vertex (`ShaderStructTypeUsage::VertexLayout`)
* Uniform/Storage Buffer (`ShaderStructTypeUsage::BufferLayout`)

C++ struct, GLSL buffer, MSL buffer will have exactly the same layout (field size and offset) in all platforms, otherwise compilation will fail at resource compilation stage (GLSL/MSL) or at C++ compilation stage.

RenderTechnique reflection:
* PipelineSpecialization names.
* RenderPass attachment names and count.
* RenderTechnique Pass name.
* RenderTechnique name.

<details>
<summary>Example</summary>

```cpp
namespace RenderTechs
{
    static constexpr struct _AsyncCompTestRT
    {
        constexpr operator RenderTechName () const { return RenderTechName{"AsyncCompTestRT"};}

        // graphics (0)
        static constexpr struct _Draw_1
        {
            constexpr operator RenderTechPassName () const { return RenderTechPassName{"Draw_1"};}

            // RenderPass 'DrawTest.Draw_1' subpass 'Main'

            static constexpr uint  attachmentsCount = 1;
            static constexpr AttachmentName  att_Color {"Color"};

            // pipelines
            static constexpr PipelineName  async_comp1_graphics {"async_comp1.graphics"};
        } Draw_1;

        // compute (1)
        static constexpr struct _Compute_1
        {
            constexpr operator RenderTechPassName () const { return RenderTechPassName{"Compute_1"};}

            // pipelines
            static constexpr PipelineName  async_comp1_compute {"async_comp1.compute"};
        } Compute_1;
    } AsyncCompTestRT;
}
```
</details>

# Input Actions Packer

**InputActionsBinding** is a standalone dynamic library with a single function:
```cpp
extern "C" bool ConvertInputActions (const InputActionsInfo *info);
```

Converts multiple scripts with key & mouse bindings to a single binary file.

Documentation for each script function is auto-generated and stored in '[input_actions.as](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/scripts/input_actions.as)', you can include this file in your script.

## C++ Reflection

<details>
<summary>Input bindings in script</summary>

```cpp
RC<WinAPI_BindingsMode>	bind = bindings.CreateMode( "Controller.Camera3D" );

bind.Add( WinAPI_Input::W,
          ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("+00") ));
bind.Add( WinAPI_Input::S,
          ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("-00") ));
bind.Add( WinAPI_Input::A,
          ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("0-0") ));
bind.Add( WinAPI_Input::D,
          ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("0+0") ));
bind.Add( WinAPI_Input::LeftShift,
          ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("00+") ));
bind.Add( WinAPI_Input::Space,
          ActionInfo( "Camera.Move", EValueType::Float3, EGestureType::Hold, VecSwizzle("00-") ));

bind.Add( WinAPI_Input::CursorDelta_norm,
          ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Move, float4(1.f, 1.f, 0.f, 0.f) ));
bind.Add( WinAPI_Input::R,
          ActionInfo( "Camera.Reset", EGestureType::Down ));

bind.Add( WinAPI_Input::ArrowLeft,
          ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("-0") ));
bind.Add( WinAPI_Input::ArrowRight,
          ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("+0") ));
bind.Add( WinAPI_Input::ArrowUp,
          ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("0+") ));
bind.Add( WinAPI_Input::ArrowDown,
          ActionInfo( "Camera.Rotate", EValueType::Float2, EGestureType::Hold, VecSwizzle("0-") ));
```
</details>

<details>
<summary>Result as C++ reflection</summary>

```cpp
namespace InputActions
{
    static constexpr struct _Controller_Camera3D
    {
        constexpr operator InputModeName () const { return InputModeName{"Controller.Camera3D"}; }

        static constexpr uint  actionCount = 3;
        static constexpr uint  Camera_Move  = uint{InputActionName{"Camera.Move"}};
        static constexpr uint  Camera_Reset  = uint{InputActionName{"Camera.Reset"}};
        static constexpr uint  Camera_Rotate  = uint{InputActionName{"Camera.Rotate"}};
    } Controller_Camera3D;
}
```
</details>


# Asset Packer

**AssetPacker** is a standalone dynamic library with a single function:
```cpp
extern "C" bool PackAssets (const AssetInfo *info);
```

Documentation for each script function is auto-generated and stored in '[asset_packer.as](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/scripts/asset_packer.as)', you can include this file in your script.

* Converts images to engine internal format.
    - Supported crop, swizzle and other operations on image.
    - Supported packing small images to the atlas.
* Converts TTF fonts to engine internal format.
    - Supported raster fonts - prerendered for specified resolution.
    - Supported SDF and multichannel SDF fonts.


# Offline Packer

**OfflinePacker** is a console program which takes arguments:<br/>
1 - path to the resource packing script.<br/>
2 - (optional) path to the output directory, by default used current directory.

Example:
```cpp
OfflinePacker.exe "res_pack.as" "../compiled_resources"
```

Documentation for each script function is auto-generated and stored in '[offline_packer.as](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/scripts/offline_packer.as)', you can include this file in your script.

**OfflinePacker** allows to write top-level script which runs pipeline compiler, input actions packer, asset packer and store result to an archives.
