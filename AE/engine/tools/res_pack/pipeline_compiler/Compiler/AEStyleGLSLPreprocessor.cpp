// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Compiler/AEStyleGLSLPreprocessor.h"

namespace AE::PipelineCompiler
{
namespace {
    ND_ bool  IsPartOfWord (const char c)
    {
        return  (c == '_') |
                ((c >= 'a') & (c <= 'z')) |
                ((c >= 'A') & (c <= 'Z')) |
                ((c >= '0') & (c <= '9'));
    }
}

/*
=================================================
    constructor
=================================================
*/
    AEStyleGLSLPreprocessor::AEStyleGLSLPreprocessor ()
    {
        // TODO: optimize - group by prefixes

        // types
        _typeMap.emplace( "float2",     "vec2" );
        _typeMap.emplace( "float3",     "vec3" );
        _typeMap.emplace( "float4",     "vec4" );
        _typeMap.emplace( "float2x2",   "mat2x2" );
        _typeMap.emplace( "float2x3",   "mat2x3" );
        _typeMap.emplace( "float2x4",   "mat2x4" );
        _typeMap.emplace( "float3x2",   "mat3x2" );
        _typeMap.emplace( "float3x3",   "mat3x3" );
        _typeMap.emplace( "float3x4",   "mat3x4" );
        _typeMap.emplace( "float4x2",   "mat4x2" );
        _typeMap.emplace( "float4x3",   "mat4x3" );
        _typeMap.emplace( "float4x4",   "mat4x4" );

        _typeMap.emplace( "double2",    "dvec2" );
        _typeMap.emplace( "double3",    "dvec3" );
        _typeMap.emplace( "double4",    "dvec4" );
        _typeMap.emplace( "double2x2",  "dmat2x2" );
        _typeMap.emplace( "double2x3",  "dmat2x3" );
        _typeMap.emplace( "double2x4",  "dmat2x4" );
        _typeMap.emplace( "double3x2",  "dmat3x2" );
        _typeMap.emplace( "double3x3",  "dmat3x3" );
        _typeMap.emplace( "double3x4",  "dmat3x4" );
        _typeMap.emplace( "double4x2",  "dmat4x2" );
        _typeMap.emplace( "double4x3",  "dmat4x3" );
        _typeMap.emplace( "double4x4",  "dmat4x4" );

        _typeMap.emplace( "int2",       "ivec2" );
        _typeMap.emplace( "int3",       "ivec3" );
        _typeMap.emplace( "int4",       "ivec4" );

        _typeMap.emplace( "uint2",      "uvec2" );
        _typeMap.emplace( "uint3",      "uvec3" );
        _typeMap.emplace( "uint4",      "uvec4" );

        _typeMap.emplace( "bool2",      "bvec2" );
        _typeMap.emplace( "bool3",      "bvec3" );
        _typeMap.emplace( "bool4",      "bvec4" );

        // https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GL_EXT_shader_explicit_arithmetic_types.txt
        _typeMap.emplace( "byte",       "int8_t" );
        _typeMap.emplace( "byte2",      "i8vec2" );
        _typeMap.emplace( "byte3",      "i8vec3" );
        _typeMap.emplace( "byte4",      "i8vec4" );

        _typeMap.emplace( "ubyte",      "uint8_t" );
        _typeMap.emplace( "ubyte2",     "u8vec2" );
        _typeMap.emplace( "ubyte3",     "u8vec3" );
        _typeMap.emplace( "ubyte4",     "u8vec4" );

        _typeMap.emplace( "short",      "int16_t" );
        _typeMap.emplace( "short2",     "i16vec2" );
        _typeMap.emplace( "short3",     "i16vec3" );
        _typeMap.emplace( "short4",     "i16vec4" );

        _typeMap.emplace( "ushort",     "uint16_t" );
        _typeMap.emplace( "ushort2",    "u16vec2" );
        _typeMap.emplace( "ushort3",    "u16vec3" );
        _typeMap.emplace( "ushort4",    "u16vec4" );

        _typeMap.emplace( "half",       "float16_t" );
        _typeMap.emplace( "half2",      "f16vec2" );
        _typeMap.emplace( "half3",      "f16vec3" );
        _typeMap.emplace( "half4",      "f16vec4" );
        _typeMap.emplace( "half2x2",    "f16mat2x2" );
        _typeMap.emplace( "half2x3",    "f16mat2x3" );
        _typeMap.emplace( "half2x4",    "f16mat2x4" );
        _typeMap.emplace( "half3x2",    "f16mat3x2" );
        _typeMap.emplace( "half3x3",    "f16mat3x3" );
        _typeMap.emplace( "half3x4",    "f16mat3x4" );
        _typeMap.emplace( "half4x2",    "f16mat4x2" );
        _typeMap.emplace( "half4x3",    "f16mat4x3" );
        _typeMap.emplace( "half4x4",    "f16mat4x4" );
        /*
        _typeMap.emplace( "float",      "float32_t" );
        _typeMap.emplace( "float2",     "f32vec2" );
        _typeMap.emplace( "float3",     "f32vec3" );
        _typeMap.emplace( "float4",     "f32vec4" );
        _typeMap.emplace( "float2x2",   "f32mat2x2" );
        _typeMap.emplace( "float2x3",   "f32mat2x3" );
        _typeMap.emplace( "float2x4",   "f32mat2x4" );
        _typeMap.emplace( "float3x2",   "f32mat3x2" );
        _typeMap.emplace( "float3x3",   "f32mat3x3" );
        _typeMap.emplace( "float3x4",   "f32mat3x4" );
        _typeMap.emplace( "float4x2",   "f32mat4x2" );
        _typeMap.emplace( "float4x3",   "f32mat4x3" );
        _typeMap.emplace( "float4x4",   "f32mat4x4" );
        *//*
        _typeMap.emplace( "double",     "float64_t" );
        _typeMap.emplace( "double2",    "f64vec2" );
        _typeMap.emplace( "double3",    "f64vec3" );
        _typeMap.emplace( "double4",    "f64vec4" );
        _typeMap.emplace( "double2x2",  "f64mat2x2" );
        _typeMap.emplace( "double2x3",  "f64mat2x3" );
        _typeMap.emplace( "double2x4",  "f64mat2x4" );
        _typeMap.emplace( "double3x2",  "f64mat3x2" );
        _typeMap.emplace( "double3x3",  "f64mat3x3" );
        _typeMap.emplace( "double3x4",  "f64mat3x4" );
        _typeMap.emplace( "double4x2",  "f64mat4x2" );
        _typeMap.emplace( "double4x3",  "f64mat4x3" );
        _typeMap.emplace( "double4x4",  "f64mat4x4" );

        _typeMap.emplace( "int",        "int32_t" );
        _typeMap.emplace( "int2",       "i32vec2" );
        _typeMap.emplace( "int3",       "i32vec3" );
        _typeMap.emplace( "int4",       "i32vec4" );

        _typeMap.emplace( "uint",       "uint32_t" );
        _typeMap.emplace( "uint2",      "u32vec2" );
        _typeMap.emplace( "uint3",      "u32vec3" );
        _typeMap.emplace( "uint4",      "u32vec4" );
        */
        _typeMap.emplace( "long",       "int64_t" );
        _typeMap.emplace( "long2",      "i64vec2" );
        _typeMap.emplace( "long3",      "i64vec3" );
        _typeMap.emplace( "long4",      "i64vec4" );

        _typeMap.emplace( "ulong",      "uint64_t" );
        _typeMap.emplace( "ulong2",     "u64vec2" );
        _typeMap.emplace( "ulong3",     "u64vec3" );
        _typeMap.emplace( "ulong4",     "u64vec4" );

        _typeMap.emplace( "gl::SubpassInput<float>",    "subpassInput" );
        _typeMap.emplace( "gl::SubpassInputMS<float>",  "subpassInputMS" );
        _typeMap.emplace( "gl::SubpassInput<int>",      "isubpassInput" );
        _typeMap.emplace( "gl::SubpassInputMS<int>",    "isubpassInputMS" );
        _typeMap.emplace( "gl::SubpassInput<uint>",     "usubpassInput" );
        _typeMap.emplace( "gl::SubpassInputMS<uint>",   "usubpassInputMS" );

        _typeMap.emplace( "gl::Texture1D<float>",       "texture1D" );
        _typeMap.emplace( "gl::Texture2D<float>",       "texture2D" );
        _typeMap.emplace( "gl::Texture3D<float>",       "texture3D" );
        _typeMap.emplace( "gl::TextureCube<float>",     "textureCube" );
        _typeMap.emplace( "gl::Texture1DArray<float>",  "texture1DArray" );
        _typeMap.emplace( "gl::Texture2DArray<float>",  "texture2DArray" );
        _typeMap.emplace( "gl::TextureBuffer<float>",   "textureBuffer" );
        _typeMap.emplace( "gl::Texture2DMS<float>",     "texture2DMS" );
        _typeMap.emplace( "gl::Texture2DMSArray<float>","texture2DMSArray" );
        _typeMap.emplace( "gl::TextureCubeArray<float>","textureCubeArray" );

        _typeMap.emplace( "gl::Texture1D<int>",         "itexture1D" );
        _typeMap.emplace( "gl::Texture2D<int>",         "itexture2D" );
        _typeMap.emplace( "gl::Texture3D<int>",         "itexture3D" );
        _typeMap.emplace( "gl::TextureCube<int>",       "itextureCube" );
        _typeMap.emplace( "gl::Texture1DArray<int>",    "itexture1DArray" );
        _typeMap.emplace( "gl::Texture2DArray<int>",    "itexture2DArray" );
        _typeMap.emplace( "gl::TextureBuffer<int>",     "itextureBuffer" );
        _typeMap.emplace( "gl::Texture2DMS<int>",       "itexture2DMS" );
        _typeMap.emplace( "gl::Texture2DMSArray<int>",  "itexture2DMSArray" );
        _typeMap.emplace( "gl::TextureCubeArray<int>",  "itextureCubeArray" );

        _typeMap.emplace( "gl::Texture1D<uint>",        "utexture1D" );
        _typeMap.emplace( "gl::Texture2D<uint>",        "utexture2D" );
        _typeMap.emplace( "gl::Texture3D<uint>",        "utexture3D" );
        _typeMap.emplace( "gl::TextureCube<uint>",      "utextureCube" );
        _typeMap.emplace( "gl::Texture1DArray<uint>",   "utexture1DArray" );
        _typeMap.emplace( "gl::Texture2DArray<uint>",   "utexture2DArray" );
        _typeMap.emplace( "gl::TextureBuffer<uint>",    "utextureBuffer" );
        _typeMap.emplace( "gl::Texture2DMS<uint>",      "utexture2DMS" );
        _typeMap.emplace( "gl::Texture2DMSArray<uint>", "utexture2DMSArray" );
        _typeMap.emplace( "gl::TextureCubeArray<uint>", "utextureCubeArray" );

        _typeMap.emplace( "gl::Sampler",                "sampler" );
        _typeMap.emplace( "gl::SamplerShadow",          "samplerShadow" );

        _typeMap.emplace( "gl::CombinedTex1D<float>",       "sampler1D" );
        _typeMap.emplace( "gl::CombinedTex2D<float>",       "sampler2D" );
        _typeMap.emplace( "gl::CombinedTex3D<float>",       "sampler3D" );
        _typeMap.emplace( "gl::CombinedTexCube<float>",     "samplerCube" );
        _typeMap.emplace( "gl::CombinedTex1DArray<float>",  "sampler1DArray" );
        _typeMap.emplace( "gl::CombinedTex2DArray<float>",  "sampler2DArray" );
        _typeMap.emplace( "gl::CombinedTexBuffer<float>",   "samplerBuffer" );
        _typeMap.emplace( "gl::CombinedTex2DMS<float>",     "sampler2DMS" );
        _typeMap.emplace( "gl::CombinedTex2DMSArray<float>","sampler2DMSArray" );
        _typeMap.emplace( "gl::CombinedTexCubeArray<float>","samplerCubeArray" );

        _typeMap.emplace( "gl::CombinedTex1D<int>",         "isampler1D" );
        _typeMap.emplace( "gl::CombinedTex2D<int>",         "isampler2D" );
        _typeMap.emplace( "gl::CombinedTex3D<int>",         "isampler3D" );
        _typeMap.emplace( "gl::CombinedTexCube<int>",       "isamplerCube" );
        _typeMap.emplace( "gl::CombinedTex1DArray<int>",    "isampler1DArray" );
        _typeMap.emplace( "gl::CombinedTex2DArray<int>",    "isampler2DArray" );
        _typeMap.emplace( "gl::CombinedTexBuffer<int>",     "isamplerBuffer" );
        _typeMap.emplace( "gl::CombinedTex2DMS<int>",       "isampler2DMS" );
        _typeMap.emplace( "gl::CombinedTex2DMSArray<int>",  "isampler2DMSArray" );
        _typeMap.emplace( "gl::CombinedTexCubeArray<int>",  "isamplerCubeArray" );

        _typeMap.emplace( "gl::CombinedTex1D<uint>",        "usampler1D" );
        _typeMap.emplace( "gl::CombinedTex2D<uint>",        "usampler2D" );
        _typeMap.emplace( "gl::CombinedTex3D<uint>",        "usampler3D" );
        _typeMap.emplace( "gl::CombinedTexCube<uint>",      "usamplerCube" );
        _typeMap.emplace( "gl::CombinedTex1DArray<uint>",   "usampler1DArray" );
        _typeMap.emplace( "gl::CombinedTex2DArray<uint>",   "usampler2DArray" );
        _typeMap.emplace( "gl::CombinedTexBuffer<uint>",    "usamplerBuffer" );
        _typeMap.emplace( "gl::CombinedTex2DMS<uint>",      "usampler2DMS" );
        _typeMap.emplace( "gl::CombinedTex2DMSArray<uint>", "usampler2DMSArray" );
        _typeMap.emplace( "gl::CombinedTexCubeArray<uint>", "usamplerCubeArray" );

        _typeMap.emplace( "gl::Image1D<float>",         "image1D" );
        _typeMap.emplace( "gl::Image2D<float>",         "image2D" );
        _typeMap.emplace( "gl::Image3D<float>",         "image3D" );
        _typeMap.emplace( "gl::ImageCube<float>",       "imageCube" );
        _typeMap.emplace( "gl::ImageBuffer<float>",     "bufferImage" );
        _typeMap.emplace( "gl::Image1DArray<float>",    "image1DArray" );
        _typeMap.emplace( "gl::Image2DArray<float>",    "image2DArray" );
        _typeMap.emplace( "gl::ImageCubeArray<float>",  "imageCubeArray" );
        _typeMap.emplace( "gl::Image2DMS<float>",       "image2DMS" );
        _typeMap.emplace( "gl::Image2DMSArray<float>",  "image2DMSArray" );

        _typeMap.emplace( "gl::Image1D<int>",           "iimage1D" );
        _typeMap.emplace( "gl::Image2D<int>",           "iimage2D" );
        _typeMap.emplace( "gl::Image3D<int>",           "iimage3D" );
        _typeMap.emplace( "gl::ImageCube<int>",         "iimageCube" );
        _typeMap.emplace( "gl::ImageBuffer<int>",       "ibufferImage" );
        _typeMap.emplace( "gl::Image1DArray<int>",      "iimage1DArray" );
        _typeMap.emplace( "gl::Image2DArray<int>",      "iimage2DArray" );
        _typeMap.emplace( "gl::ImageCubeArray<int>",    "iimageCubeArray" );
        _typeMap.emplace( "gl::Image2DMS<int>",         "iimage2DMS" );
        _typeMap.emplace( "gl::Image2DMSArray<int>",    "iimage2DMSArray" );

        _typeMap.emplace( "gl::Image1D<uint>",          "uimage1D" );
        _typeMap.emplace( "gl::Image2D<uint>",          "uimage2D" );
        _typeMap.emplace( "gl::Image3D<uint>",          "uimage3D" );
        _typeMap.emplace( "gl::ImageCube<uint>",        "uimageCube" );
        _typeMap.emplace( "gl::ImageBuffer<uint>",      "ubufferImage" );
        _typeMap.emplace( "gl::Image1DArray<uint>",     "uimage1DArray" );
        _typeMap.emplace( "gl::Image2DArray<uint>",     "uimage2DArray" );
        _typeMap.emplace( "gl::ImageCubeArray<uint>",   "uimageCubeArray" );
        _typeMap.emplace( "gl::Image2DMS<uint>",        "uimage2DMS" );
        _typeMap.emplace( "gl::Image2DMSArray<uint>",   "uimage2DMSArray" );

        // sync
        _typeMap.emplace( "gl.PatchBarrier",            "barrier" );
        _typeMap.emplace( "gl.WorkgroupBarrier",        "barrier" );
        _typeMap.emplace( "gl.memoryBarrier.Workgroup", "groupMemoryBarrier" );
        _typeMap.emplace( "gl.memoryBarrier.All",       "memoryBarrier" );
        _typeMap.emplace( "gl.memoryBarrier.Buffer",    "memoryBarrierBuffer" );
        _typeMap.emplace( "gl.memoryBarrier.Image",     "memoryBarrierImage" );
        _typeMap.emplace( "gl.memoryBarrier.Shared",    "memoryBarrierShared" );
        _typeMap.emplace( "gl.memoryBarrier.Subgroup",  "subgroupMemoryBarrier" );

        // derivatives
        _typeMap.emplace( "gl.dFdx",                "dFdx" );
        _typeMap.emplace( "gl.dFdy",                "dFdy" );
        _typeMap.emplace( "gl.dFdxCoarse",          "dFdxCoarse" );
        _typeMap.emplace( "gl.dFdyCoarse",          "dFdyCoarse" );
        _typeMap.emplace( "gl.dFdxFine",            "dFdxFine" );
        _typeMap.emplace( "gl.dFdyFine",            "dFdyFine" );
        _typeMap.emplace( "gl.fwidth",              "fwidth" );
        _typeMap.emplace( "gl.fwidthCoarse",        "fwidthCoarse" );
        _typeMap.emplace( "gl.fwidthFine",          "fwidthFine" );

        // geometry shader
        _typeMap.emplace( "gl.EmitStreamVertex",    "EmitStreamVertex" );
        _typeMap.emplace( "gl.EmitVertex",          "EmitVertex" );
        _typeMap.emplace( "gl.EndPrimitive",        "EndPrimitive" );
        _typeMap.emplace( "gl.EndStreamPrimitive",  "EndStreamPrimitive" );

        _typeMap.emplace( "gl.ClipDistance",        "gl_ClipDistance" );
        _typeMap.emplace( "gl.CullDistance",        "gl_CullDistance" );
        _typeMap.emplace( "gl.FragCoord",           "gl_FragCoord" );
        _typeMap.emplace( "gl.FragDepth",           "gl_FragDepth" );
        _typeMap.emplace( "gl.FrontFacing",         "gl_FrontFacing" );
        _typeMap.emplace( "gl.GlobalInvocationID",  "gl_GlobalInvocationID" );
        _typeMap.emplace( "gl.HelperInvocation",    "gl_HelperInvocation" );
        _typeMap.emplace( "gl.InvocationID",        "gl_InvocationID" );
        _typeMap.emplace( "gl.Layer",               "gl_Layer" );
        _typeMap.emplace( "gl.LocalInvocationID",   "gl_LocalInvocationID" );
        _typeMap.emplace( "gl.LocalInvocationIndex","gl_LocalInvocationIndex" );
    //  _typeMap.emplace( "gl.NumSamples",          "gl_NumSamples" );              // removed in Vulkan
        _typeMap.emplace( "gl.NumWorkGroups",       "gl_NumWorkGroups" );
        _typeMap.emplace( "gl.PatchVerticesIn",     "gl_PatchVerticesIn" );
        _typeMap.emplace( "gl.PointCoord",          "gl_PointCoord" );
        _typeMap.emplace( "gl.PointSize",           "gl_PointSize" );
        _typeMap.emplace( "gl.Position",            "gl_Position" );
        _typeMap.emplace( "gl.PrimitiveID",         "gl_PrimitiveID" );
        _typeMap.emplace( "gl.PrimitiveIDIn",       "gl_PrimitiveIDIn" );
        _typeMap.emplace( "gl.SampleID",            "gl_SampleID" );
        _typeMap.emplace( "gl.SampleMask",          "gl_SampleMask" );
        _typeMap.emplace( "gl.SampleMaskIn",        "gl_SampleMaskIn" );
        _typeMap.emplace( "gl.SamplePosition",      "gl_SamplePosition" );
        _typeMap.emplace( "gl.TessCoord",           "gl_TessCoord" );
        _typeMap.emplace( "gl.TessLevelInner",      "gl_TessLevelInner" );
        _typeMap.emplace( "gl.TessLevelOuter",      "gl_TessLevelOuter" );
    //  _typeMap.emplace( "gl.VertexID",            "gl_VertexID" );                // removed in Vulkan
        _typeMap.emplace( "gl.ViewportIndex",       "gl_ViewportIndex" );
        _typeMap.emplace( "gl.WorkGroupID",         "gl_WorkGroupID" );
        _typeMap.emplace( "gl.WorkGroupSize",       "gl_WorkGroupSize" );
        _typeMap.emplace( "gl.VertexIndex",         "gl_VertexIndex" );
        _typeMap.emplace( "gl.InstanceIndex",       "gl_InstanceIndex" );
        _typeMap.emplace( "gl.InstanceID",          "gl_InstanceID" );              // in graphics - removed in Vulkan, supported only for ray tracing
        _typeMap.emplace( "gl::PerVertex",          "gl_PerVertex" );

        // GL_ARB_shader_draw_parameters
        _typeMap.emplace( "gl.BaseInstance",        "gl_BaseInstance" );            // or gl_BaseInstanceARB
        _typeMap.emplace( "gl.BaseVertex",          "gl_BaseVertex" );              // or gl_BaseVertexARB
        _typeMap.emplace( "gl.DrawID",              "gl_DrawID" );                  // or gl_DrawIDARB

        // image
        _typeMap.emplace( "gl.image.AtomicAdd",         "imageAtomicAdd" );
        _typeMap.emplace( "gl.image.AtomicAnd",         "imageAtomicAnd" );
        _typeMap.emplace( "gl.image.AtomicCompSwap",    "imageAtomicCompSwap" );
        _typeMap.emplace( "gl.image.AtomicExchange",    "imageAtomicExchange" );
        _typeMap.emplace( "gl.image.AtomicMax",         "imageAtomicMax" );
        _typeMap.emplace( "gl.image.AtomicMin",         "imageAtomicMin" );
        _typeMap.emplace( "gl.image.AtomicOr",          "imageAtomicOr" );
        _typeMap.emplace( "gl.image.AtomicXor",         "imageAtomicXor" );
        _typeMap.emplace( "gl.image.AtomicLoad",        "imageAtomicLoad" );
        _typeMap.emplace( "gl.image.AtomicStore",       "imageAtomicStore" );
        _typeMap.emplace( "gl.image.Load",              "imageLoad" );
        _typeMap.emplace( "gl.image.GetSamples",        "imageSamples" );
        _typeMap.emplace( "gl.image.GetSize",           "imageSize" );
        _typeMap.emplace( "gl.image.Store",             "imageStore" );

        _typeMap.emplace( "gl.InterpolateAtCentroid",   "interpolateAtCentroid" );
        _typeMap.emplace( "gl.InterpolateAtOffset",     "interpolateAtOffset" );
        _typeMap.emplace( "gl.InterpolateAtSample",     "interpolateAtSample" );

        // texture
        _typeMap.emplace( "gl.texture.Fetch",               "texelFetch" );
        _typeMap.emplace( "gl.texture.FetchOffset",         "texelFetchOffset" );
        _typeMap.emplace( "gl.texture.Sample",              "texture" );
        _typeMap.emplace( "gl.texture.Gather",              "textureGather" );
        _typeMap.emplace( "gl.texture.GatherOffset",        "textureGatherOffset" );
        _typeMap.emplace( "gl.texture.GatherOffsets",       "textureGatherOffsets" );
        _typeMap.emplace( "gl.texture.SampleGrad",          "textureGrad" );
        _typeMap.emplace( "gl.texture.SampleGradOffset",    "textureGradOffset" );
        _typeMap.emplace( "gl.texture.SampleLod",           "textureLod" );
        _typeMap.emplace( "gl.texture.SampleLodOffset",     "textureLodOffset" );
        _typeMap.emplace( "gl.texture.SampleOffset",        "textureOffset" );
        _typeMap.emplace( "gl.texture.SampleProj",          "textureProj" );
        _typeMap.emplace( "gl.texture.SampleProjGrad",      "textureProjGrad" );
        _typeMap.emplace( "gl.texture.SampleProjGradOffset","textureProjGradOffset" );
        _typeMap.emplace( "gl.texture.SampleProjLod",       "textureProjLod" );
        _typeMap.emplace( "gl.texture.SampleProjLodOffset", "textureProjLodOffset" );
        _typeMap.emplace( "gl.texture.SampleProjOffset",    "textureProjOffset" );
        _typeMap.emplace( "gl.texture.QueryLevels",         "textureQueryLevels" );
        _typeMap.emplace( "gl.texture.QueryLod",            "textureQueryLod" );
        _typeMap.emplace( "gl.texture.GetSamples",          "textureSamples" );
        _typeMap.emplace( "gl.texture.GetSize",             "textureSize" );

        // subpass input
        _typeMap.emplace( "gl.subpass.Load",                "subpassLoad" );


        // subgroup
        // https://github.com/KhronosGroup/GLSL/blob/master/extensions/khr/GL_KHR_shader_subgroup.txt
        _typeMap.emplace( "gl.subgroup.GroupCount",         "gl_NumSubgroups" );
        _typeMap.emplace( "gl.subgroup.GroupIndex",         "gl_SubgroupID" );
        _typeMap.emplace( "gl.subgroup.Size",               "gl_SubgroupSize" );
        _typeMap.emplace( "gl.subgroup.Index",              "gl_SubgroupInvocationID" );
        _typeMap.emplace( "gl.subgroup.EqMask",             "gl_SubgroupEqMask" );
        _typeMap.emplace( "gl.subgroup.GeMask",             "gl_SubgroupGeMask" );
        _typeMap.emplace( "gl.subgroup.GtMask",             "gl_SubgroupGtMask" );
        _typeMap.emplace( "gl.subgroup.LeMask",             "gl_SubgroupLeMask" );
        _typeMap.emplace( "gl.subgroup.LtMask",             "gl_SubgroupLtMask" );

        _typeMap.emplace( "gl.subgroup.ExecutionBarrier",       "subgroupBarrier" );
        _typeMap.emplace( "gl.subgroup.memoryBarrier.All",      "subgroupMemoryBarrier" );
        _typeMap.emplace( "gl.subgroup.memoryBarrier.Buffer",   "subgroupMemoryBarrierBuffer" );
        _typeMap.emplace( "gl.subgroup.memoryBarrier.Shared",   "subgroupMemoryBarrierShared" );
        _typeMap.emplace( "gl.subgroup.memoryBarrier.Image",    "subgroupMemoryBarrierImage" );
        _typeMap.emplace( "gl.subgroup.Elect",                  "subgroupElect" );
        _typeMap.emplace( "gl.subgroup.All",                    "subgroupAll" );
        _typeMap.emplace( "gl.subgroup.Any",                    "subgroupAny" );
        _typeMap.emplace( "gl.subgroup.AllEqual",               "subgroupAllEqual" );
        _typeMap.emplace( "gl.subgroup.Broadcast",              "subgroupBroadcast" );
        _typeMap.emplace( "gl.subgroup.BroadcastFirst",         "subgroupBroadcastFirst" );
        _typeMap.emplace( "gl.subgroup.Ballot",                 "subgroupBallot" );
        _typeMap.emplace( "gl.subgroup.InverseBallot",          "subgroupInverseBallot" );
        _typeMap.emplace( "gl.subgroup.BallotBitExtract",       "subgroupBallotBitExtract" );
        _typeMap.emplace( "gl.subgroup.BallotBitCount",         "subgroupBallotBitCount" );
        _typeMap.emplace( "gl.subgroup.BallotInclusiveBitCount","subgroupBallotInclusiveBitCount" );
        _typeMap.emplace( "gl.subgroup.BallotExclusiveBitCount","subgroupBallotExclusiveBitCount" );
        _typeMap.emplace( "gl.subgroup.BallotFindLSB",          "subgroupBallotFindLSB" );
        _typeMap.emplace( "gl.subgroup.BallotFindMSB",          "subgroupBallotFindMSB" );
        _typeMap.emplace( "gl.subgroup.Shuffle",                "subgroupShuffle" );
        _typeMap.emplace( "gl.subgroup.ShuffleXor",             "subgroupShuffleXor" );
        _typeMap.emplace( "gl.subgroup.ShuffleUp",              "subgroupShuffleUp" );
        _typeMap.emplace( "gl.subgroup.ShuffleDown",            "subgroupShuffleDown" );
        _typeMap.emplace( "gl.subgroup.Add",                    "subgroupAdd" );
        _typeMap.emplace( "gl.subgroup.Mul",                    "subgroupMul" );
        _typeMap.emplace( "gl.subgroup.Min",                    "subgroupMin" );
        _typeMap.emplace( "gl.subgroup.Max",                    "subgroupMax" );
        _typeMap.emplace( "gl.subgroup.And",                    "subgroupAnd" );
        _typeMap.emplace( "gl.subgroup.Or",                     "subgroupOr" );
        _typeMap.emplace( "gl.subgroup.Xor",                    "subgroupXor" );
        _typeMap.emplace( "gl.subgroup.InclusiveAdd",           "subgroupInclusiveAdd" );
        _typeMap.emplace( "gl.subgroup.InclusiveMul",           "subgroupInclusiveMul" );
        _typeMap.emplace( "gl.subgroup.InclusiveMin",           "subgroupInclusiveMin" );
        _typeMap.emplace( "gl.subgroup.InclusiveMax",           "subgroupInclusiveMax" );
        _typeMap.emplace( "gl.subgroup.InclusiveAnd",           "subgroupInclusiveAnd" );
        _typeMap.emplace( "gl.subgroup.InclusiveOr",            "subgroupInclusiveOr" );
        _typeMap.emplace( "gl.subgroup.InclusiveXor",           "subgroupInclusiveXor" );
        _typeMap.emplace( "gl.subgroup.ExclusiveAdd",           "subgroupExclusiveAdd" );
        _typeMap.emplace( "gl.subgroup.ExclusiveMul",           "subgroupExclusiveMul" );
        _typeMap.emplace( "gl.subgroup.ExclusiveMin",           "subgroupExclusiveMin" );
        _typeMap.emplace( "gl.subgroup.ExclusiveMax",           "subgroupExclusiveMax" );
        _typeMap.emplace( "gl.subgroup.ExclusiveAnd",           "subgroupExclusiveAnd" );
        _typeMap.emplace( "gl.subgroup.ExclusiveOr",            "subgroupExclusiveOr" );
        _typeMap.emplace( "gl.subgroup.ExclusiveXor",           "subgroupExclusiveXor" );
        _typeMap.emplace( "gl.subgroup.ClusteredAdd",           "subgroupClusteredAdd" );
        _typeMap.emplace( "gl.subgroup.ClusteredMul",           "subgroupClusteredMul" );
        _typeMap.emplace( "gl.subgroup.ClusteredMin",           "subgroupClusteredMin" );
        _typeMap.emplace( "gl.subgroup.ClusteredMax",           "subgroupClusteredMax" );
        _typeMap.emplace( "gl.subgroup.ClusteredAnd",           "subgroupClusteredAnd" );
        _typeMap.emplace( "gl.subgroup.ClusteredOr",            "subgroupClusteredOr" );
        _typeMap.emplace( "gl.subgroup.ClusteredXor",           "subgroupClusteredXor" );
        _typeMap.emplace( "gl.subgroup.QuadBroadcast",          "subgroupQuadBroadcast" );
        _typeMap.emplace( "gl.subgroup.QuadSwapHorizontal",     "subgroupQuadSwapHorizontal" );
        _typeMap.emplace( "gl.subgroup.QuadSwapVertical",       "subgroupQuadSwapVertical" );
        _typeMap.emplace( "gl.subgroup.QuadSwapDiagonal",       "subgroupQuadSwapDiagonal" );

        // https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GL_EXT_subgroupuniform_qualifier.txt
        _typeMap.emplace( "gl::SubgroupUniform",                "subgroupuniformEXT" );

        // ray query & ray tracing
        _typeMap.emplace( "gl::RayFlags",                                   "uint" );
        _typeMap.emplace( "gl::RayFlags::None",                             "gl_RayFlagsNoneEXT" );
        _typeMap.emplace( "gl::RayFlags::Opaque",                           "gl_RayFlagsOpaqueEXT" );
        _typeMap.emplace( "gl::RayFlags::NoOpaque",                         "gl_RayFlagsNoOpaqueEXT" );
        _typeMap.emplace( "gl::RayFlags::TerminateOnFirstHit",              "gl_RayFlagsTerminateOnFirstHitEXT" );
        _typeMap.emplace( "gl::RayFlags::SkipClosestHitShader",             "gl_RayFlagsSkipClosestHitShaderEXT" );
        _typeMap.emplace( "gl::RayFlags::CullBackFacingTriangles",          "gl_RayFlagsCullBackFacingTrianglesEXT" );
        _typeMap.emplace( "gl::RayFlags::CullFrontFacingTriangles",         "gl_RayFlagsCullFrontFacingTrianglesEXT" );
        _typeMap.emplace( "gl::RayFlags::CullOpaque",                       "gl_RayFlagsCullOpaqueEXT" );
        _typeMap.emplace( "gl::RayFlags::CullNoOpaque",                     "gl_RayFlagsCullNoOpaqueEXT" );
        // https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GLSL_EXT_ray_flags_primitive_culling.txt
        _typeMap.emplace( "gl::RayFlags::SkipTriangles",                    "gl_RayFlagsSkipTrianglesEXT" );
        _typeMap.emplace( "gl::RayFlags::SkipAABB",                         "gl_RayFlagsSkipAABBEXT" );

        _typeMap.emplace( "gl::RayQueryCommittedIntersection",              "uint" );
        _typeMap.emplace( "gl::RayQueryCommittedIntersection::None",        "gl_RayQueryCommittedIntersectionNoneEXT" );
        _typeMap.emplace( "gl::RayQueryCommittedIntersection::Triangle",    "gl_RayQueryCommittedIntersectionTriangleEXT" );
        _typeMap.emplace( "gl::RayQueryCommittedIntersection::Generated",   "gl_RayQueryCommittedIntersectionGeneratedEXT" );

        _typeMap.emplace( "gl::RayQueryCandidateIntersection",              "uint" );
        _typeMap.emplace( "gl::RayQueryCandidateIntersection::Triangle",    "gl_RayQueryCandidateIntersectionTriangleEXT" );
        _typeMap.emplace( "gl::RayQueryCandidateIntersection::AABB",        "gl_RayQueryCandidateIntersectionAABBEXT" );

        _typeMap.emplace( "gl::AccelerationStructure",                      "accelerationStructureEXT" );

        // ray query
        // https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GLSL_EXT_ray_query.txt
        _typeMap.emplace( "gl::RayQuery",                                   "rayQueryEXT" );
        _typeMap.emplace( "gl.rayQuery.Initialize",                         "rayQueryInitializeEXT" );
        _typeMap.emplace( "gl.rayQuery.Proceed",                            "rayQueryProceedEXT" );
        _typeMap.emplace( "gl.rayQuery.Terminate",                          "rayQueryTerminateEXT" );
        _typeMap.emplace( "gl.rayQuery.GenerateIntersection",               "rayQueryGenerateIntersectionEXT" );
        _typeMap.emplace( "gl.rayQuery.ConfirmIntersection",                "rayQueryConfirmIntersectionEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionType",                "rayQueryGetIntersectionTypeEXT" );
        _typeMap.emplace( "gl.rayQuery.GetRayTMin",                         "rayQueryGetRayTMinEXT" );
        _typeMap.emplace( "gl.rayQuery.GetRayFlags",                        "rayQueryGetRayFlagsEXT" );
        _typeMap.emplace( "gl.rayQuery.GetWorldRayOrigin",                  "rayQueryGetWorldRayOriginEXT" );
        _typeMap.emplace( "gl.rayQuery.GetWorldRayDirection",               "rayQueryGetWorldRayDirectionEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionT",                   "rayQueryGetIntersectionTEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionInstanceCustomIndex", "rayQueryGetIntersectionInstanceCustomIndexEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionInstanceId",          "rayQueryGetIntersectionInstanceIdEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionInstanceSBTOffset",   "rayQueryGetIntersectionInstanceShaderBindingTableRecordOffsetEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionGeometryIndex",       "rayQueryGetIntersectionGeometryIndexEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionPrimitiveIndex",      "rayQueryGetIntersectionPrimitiveIndexEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionBarycentrics",        "rayQueryGetIntersectionBarycentricsEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionFrontFace",           "rayQueryGetIntersectionFrontFaceEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionCandidateAABBOpaque", "rayQueryGetIntersectionCandidateAABBOpaqueEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionObjectRayDirection",  "rayQueryGetIntersectionObjectRayDirectionEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionObjectRayOrigin",     "rayQueryGetIntersectionObjectRayOriginEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionObjectToWorld",       "rayQueryGetIntersectionObjectToWorldEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionWorldToObject",       "rayQueryGetIntersectionWorldToObjectEXT" );
        _typeMap.emplace( "gl.rayQuery.GetIntersectionTriangleVertexPositions", "rayQueryGetIntersectionTriangleVertexPositionsEXT" );

        // ray tracing
        // https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GLSL_EXT_ray_tracing.txt
        _typeMap.emplace( "gl.LaunchID",                "gl_LaunchIDEXT" );
        _typeMap.emplace( "gl.LaunchSize",              "gl_LaunchSizeEXT" );
        _typeMap.emplace( "gl.InstanceCustomIndex",     "gl_InstanceCustomIndexEXT" );
        _typeMap.emplace( "gl.GeometryIndex",           "gl_GeometryIndexEXT" );
        _typeMap.emplace( "gl.WorldRayOrigin",          "gl_WorldRayOriginEXT" );
        _typeMap.emplace( "gl.WorldRayDirection",       "gl_WorldRayDirectionEXT" );
        _typeMap.emplace( "gl.ObjectRayOrigin",         "gl_ObjectRayOriginEXT" );
        _typeMap.emplace( "gl.ObjectRayDirection",      "gl_ObjectRayDirectionEXT" );
        _typeMap.emplace( "gl.RayTmin",                 "gl_RayTminEXT" );
        _typeMap.emplace( "gl.RayTmax",                 "gl_RayTmaxEXT" );
        _typeMap.emplace( "gl.IncomingRayFlags",        "gl_IncomingRayFlagsEXT" );
        _typeMap.emplace( "gl.HitT",                    "gl_HitTEXT" );
        _typeMap.emplace( "gl.HitKind",                 "gl_HitKindEXT" );
        _typeMap.emplace( "gl.ObjectToWorld",           "gl_ObjectToWorldEXT" );
        _typeMap.emplace( "gl.WorldToObject",           "gl_WorldToObjectEXT" );
        _typeMap.emplace( "gl.WorldToObject3x4",        "gl_WorldToObject3x4EXT" );
        _typeMap.emplace( "gl.ObjectToWorld3x4",        "gl_ObjectToWorld3x4EXT" );

        _typeMap.emplace( "gl::TriangleHitKind",                "uint" );
        _typeMap.emplace( "gl::TriangleHitKind::FrontFacing",   "gl_HitKindFrontFacingTriangleEXT" );
        _typeMap.emplace( "gl::TriangleHitKind::BackFacing",    "gl_HitKindBackFacingTriangleEXT" );

        _typeMap.emplace( "gl::RayPayload",             "rayPayloadEXT" );
        _typeMap.emplace( "gl::RayPayloadIn",           "rayPayloadInEXT" );
        _typeMap.emplace( "gl::HitAttribute",           "hitAttributeEXT" );
        _typeMap.emplace( "gl::CallableData",           "callableDataEXT" );
        _typeMap.emplace( "gl::CallableDataIn",         "callableDataInEXT" );

        _typeMap.emplace( "gl.IgnoreIntersection",      "ignoreIntersectionEXT" );
        _typeMap.emplace( "gl.TerminateRay",            "terminateRayEXT" );
        _typeMap.emplace( "gl::ShaderRecord",           "shaderRecordEXT" );
        _typeMap.emplace( "gl.TraceRay",                "traceRayEXT" );
        _typeMap.emplace( "gl.ExecuteCallable",         "executeCallableEXT" );
        _typeMap.emplace( "gl.ReportIntersection",      "reportIntersectionEXT" );

        // https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GLSL_EXT_ray_cull_mask.txt
        _typeMap.emplace( "gl.CullMask",                "gl_CullMaskEXT" );     // in var

        // https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GLSL_EXT_ray_tracing_position_fetch.txt
        _typeMap.emplace( "gl.HitTriangleVertexPositions",  "gl_HitTriangleVertexPositionsEXT" );   // in var

        // for VkAccelerationStructureInstance
        _typeMap.emplace( "DeviceAddress",                              "uint64_t" );   // same as AE::Graphics::DeviceAddress
        _typeMap.emplace( "GeometryInstanceFlags",                      "uint32_t" );   // type
        _typeMap.emplace( "GeometryInstanceFlags::TriangleCullDisable", "1" );          // same as ERTInstanceOpt::TriangleCullDisable
        _typeMap.emplace( "GeometryInstanceFlags::TriangleFrontCCW",    "2" );          // same as ERTInstanceOpt::TriangleFrontCCW
        _typeMap.emplace( "GeometryInstanceFlags::ForceOpaque",         "4" );          // same as ERTInstanceOpt::ForceOpaque
        _typeMap.emplace( "GeometryInstanceFlags::ForceNonOpaque",      "8" );          // same as ERTInstanceOpt::ForceNonOpaque

        // mesh shader
        // https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GLSL_EXT_mesh_shader.txt
        _typeMap.emplace( "gl.PrimitivePointIndices",   "gl_PrimitivePointIndicesEXT" );    // var
        _typeMap.emplace( "gl.PrimitiveLineIndices",    "gl_PrimitiveLineIndicesEXT" );     // var
        _typeMap.emplace( "gl.PrimitiveTriangleIndices","gl_PrimitiveTriangleIndicesEXT" ); // var
        _typeMap.emplace( "gl::MeshPerPrimitive",       "gl_MeshPerPrimitiveEXT" );         // type
        _typeMap.emplace( "gl.MeshVertices",            "gl_MeshVerticesEXT" );             // var
        _typeMap.emplace( "gl::MeshPerVertex",          "gl_MeshPerVertexEXT" );            // type
        _typeMap.emplace( "gl.MeshPrimitives",          "gl_MeshPrimitivesEXT" );           // var
        _typeMap.emplace( "gl.SetMeshOutputs",          "SetMeshOutputsEXT" );              // fn
        _typeMap.emplace( "gl.EmitMeshTasks",           "EmitMeshTasksEXT" );               // fn
        _typeMap.emplace( "gl::TaskPayloadShared",      "taskPayloadSharedEXT" );           // qual

        // https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GL_EXT_nonuniform_qualifier.txt
        _typeMap.emplace( "gl::Nonuniform",             "nonuniformEXT" );

        // https://github.com/KhronosGroup/GLSL/blob/master/extensions/khr/GL_KHR_memory_scope_semantics.txt
        /*/ layout
        _typeMap.emplace( "gl::Coherent",               "coherent" );
        _typeMap.emplace( "gl::Devicecoherent",         "devicecoherent" );
        _typeMap.emplace( "gl::QueueFamilyCoherent",    "queuefamilycoherent" );
        _typeMap.emplace( "gl::WorkGroupCoherent",      "workgroupcoherent" );
        _typeMap.emplace( "gl::SubgroupCoherent",       "subgroupcoherent" );
        _typeMap.emplace( "gl::NonPrivate",             "nonprivate" );
        _typeMap.emplace( "gl::Volatile",               "volatile" ); */
        // sync
        _typeMap.emplace( "gl.MemoryBarrier",           "memoryBarrier" );
        _typeMap.emplace( "gl.ExecutionBarrier",        "controlBarrier" );
        _typeMap.emplace( "gl.AtomicAdd",               "atomicAdd" );
        _typeMap.emplace( "gl.AtomicMin",               "atomicMin" );
        _typeMap.emplace( "gl.AtomicMax",               "atomicMax" );
        _typeMap.emplace( "gl.AtomicAnd",               "atomicAnd" );
        _typeMap.emplace( "gl.AtomicOr",                "atomicOr" );
        _typeMap.emplace( "gl.AtomicXor",               "atomicXor" );
        _typeMap.emplace( "gl.AtomicExchange",          "atomicExchange" );
        _typeMap.emplace( "gl.AtomicCompSwap",          "atomicCompSwap" );
        _typeMap.emplace( "gl.AtomicLoad",              "atomicLoad" );
        _typeMap.emplace( "gl.AtomicStore",             "atomicStore" );

        _typeMap.emplace( "gl::Scope",                      "uint" );
        _typeMap.emplace( "gl::Scope::Device",              "gl_ScopeDevice" );
        _typeMap.emplace( "gl::Scope::QueueFamily",         "gl_ScopeQueueFamily" );
        _typeMap.emplace( "gl::Scope::Workgroup",           "gl_ScopeWorkgroup" );
        _typeMap.emplace( "gl::Scope::Subgroup",            "gl_ScopeSubgroup" );
        _typeMap.emplace( "gl::Scope::Invocation",          "gl_ScopeInvocation" );

        _typeMap.emplace( "gl::Semantics",                  "uint" );
        _typeMap.emplace( "gl::Semantics::Relaxed",         "gl_SemanticsRelaxed" );
        _typeMap.emplace( "gl::Semantics::Acquire",         "gl_SemanticsAcquire" );
        _typeMap.emplace( "gl::Semantics::Release",         "gl_SemanticsRelease" );
        _typeMap.emplace( "gl::Semantics::AcquireRelease",  "gl_SemanticsAcquireRelease" );
        _typeMap.emplace( "gl::Semantics::MakeAvailable",   "gl_SemanticsMakeAvailable" );
        _typeMap.emplace( "gl::Semantics::MakeVisible",     "gl_SemanticsMakeVisible" );
        _typeMap.emplace( "gl::Semantics::Volatile",        "gl_SemanticsVolatile" );

        _typeMap.emplace( "gl::StorageSemantics",           "uint" );
        _typeMap.emplace( "gl::StorageSemantics::None",     "gl_StorageSemanticsNone" );
        _typeMap.emplace( "gl::StorageSemantics::Buffer",   "gl_StorageSemanticsBuffer" );
        _typeMap.emplace( "gl::StorageSemantics::Shared",   "gl_StorageSemanticsShared" );
        _typeMap.emplace( "gl::StorageSemantics::Image",    "gl_StorageSemanticsImage" );
        _typeMap.emplace( "gl::StorageSemantics::Output",   "gl_StorageSemanticsOutput" );

        // GL_NV_cooperative_matrix, GL_NV_integer_cooperative_matrix
        _typeMap.emplace( "gl::FCoopMatNV",                 "fcoopMatNV" );
        _typeMap.emplace( "gl::ICoopMatNV",                 "icoopMatNV" );
        _typeMap.emplace( "gl::UCoopMatNV",                 "ucoopMatNV" );
        _typeMap.emplace( "gl.CoopMatLoadNV",               "coopMatLoadNV" );
        _typeMap.emplace( "gl.CoopMatStoreNV",              "coopMatStoreNV" );
        _typeMap.emplace( "gl.CoopMatMulAddNV",             "coopMatMulAddNV" );

        // https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GLSL_EXT_fragment_shading_rate.txt
        _typeMap.emplace( "gl.ShadingRate",                 "gl_ShadingRateEXT" );          // in
        _typeMap.emplace( "gl.PrimitiveShadingRate",        "gl_PrimitiveShadingRateEXT" ); // out
        _typeMap.emplace( "gl::ShadingRateFlag",            "uint" );
        _typeMap.emplace( "gl::ShadingRateFlag::Y1",        "0" );
        _typeMap.emplace( "gl::ShadingRateFlag::Y2",        "gl_ShadingRateFlag2VerticalPixelsEXT" );
        _typeMap.emplace( "gl::ShadingRateFlag::Y4",        "gl_ShadingRateFlag4VerticalPixelsEXT" );
        _typeMap.emplace( "gl::ShadingRateFlag::X1",        "0" );
        _typeMap.emplace( "gl::ShadingRateFlag::X2",        "gl_ShadingRateFlag2HorizontalPixelsEXT" );
        _typeMap.emplace( "gl::ShadingRateFlag::X4",        "gl_ShadingRateFlag4HorizontalPixelsEXT" );

        // https://registry.khronos.org/OpenGL/extensions/ARB/ARB_fragment_shader_interlock.txt
        _typeMap.emplace( "gl.BeginInvocationInterlock",    "beginInvocationInterlockARB" );
        _typeMap.emplace( "gl.EndInvocationInterlock",      "endInvocationInterlockARB" );

        // https://raw.githubusercontent.com/KhronosGroup/GLSL/master/extensions/ext/GLSL_EXT_fragment_shader_barycentric.txt
        _typeMap.emplace( "gl::PerVertex",                  "pervertexEXT " );
        _typeMap.emplace( "gl.BaryCoord",                   "gl_BaryCoordEXT" );
        _typeMap.emplace( "gl.BaryCoordNoPersp",            "gl_BaryCoordNoPerspEXT" );

    }

/*
=================================================
    Process
=================================================
*/
    bool  AEStyleGLSLPreprocessor::Process (EShader, const PathAndLine &fileLoc, usize headerLines, StringView inStr, OUT String &outStr)
    {
        usize   hdr_size = 0;
        Parser::MoveToLine( inStr, INOUT hdr_size, headerLines );

        StringView  header = inStr.substr( 0, hdr_size );
        StringView  source = inStr.substr( hdr_size );
        //ASSERT( (String{header} << source) == inStr );

        outStr.reserve( inStr.size() );
        outStr = source;

        #ifdef AE_CFG_DEBUG
            const auto  FindAndPrint = [source, &fileLoc] (StringView src, StringView dst)
            {{
                if ( dst.size() <= 1 )  return;
                if ( dst == "uint" )    return;

                for (usize i = 0; i < source.size();)
                {
                    usize   pos = source.find( dst, i );
                    if_unlikely( pos == StringView::npos )
                        break;

                    const char  c0 = source[ pos - 1 ];
                    const char  c1 = source[ pos + dst.size() ];

                    if ( IsPartOfWord( c0 ) or IsPartOfWord( c1 ) or (c0 == '.') )
                    {
                        i = pos + dst.size();
                        continue;
                    }

                    StringView  line_str;
                    const usize line    = Parser::CalculateNumberOfLines( source.substr( 0, pos ));

                    Parser::ReadCurrLine( source, INOUT pos, OUT line_str );

                    AE_LOG_DBG( String{line_str} << " - use '" << src << "' instead of '" << dst << "'", ToString(fileLoc.path), uint(line) );
                    i = pos;
                }
            }};

            if ( not fileLoc.path.empty() )
            {
                FindAndPrint( "gl_", "gl." );
                for (auto& [src, dst] : _typeMap)
                {
                    FindAndPrint( src, dst );
                }
            }
        #else
            Unused( fileLoc.path );
        #endif

        for (auto& [src, dst] : _typeMap)
        {
            for (usize i = 0; i < outStr.size();)
            {
                usize   pos = outStr.find( src, i );
                if_unlikely( pos == StringView::npos )
                    break;

                const char  c0 = outStr[ pos - 1 ];
                const char  c1 = outStr[ pos + src.size() ];

                if ( IsPartOfWord( c0 ) or IsPartOfWord( c1 ) or (c0 == '.') or (c1 == ':') )
                {
                    i = pos + src.size();
                    continue;
                }

                outStr.replace( pos, src.length(), dst.data() );
                i = pos + dst.length();
            }
        }

        header >> outStr;
        return true;
    }


} // AE::PipelineCompiler
