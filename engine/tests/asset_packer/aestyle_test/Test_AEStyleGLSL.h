// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "aestyle.glsl.h"

#ifdef AE_COMPILER_MSVC
# pragma warning (push)
# pragma warning (disable: 4189)
#endif

gl::SubpassInput<int>           subpassInputInt;
gl::SubpassInput<uint>          subpassInputUint;
gl::SubpassInput<float>         subpassInputFloat;
gl::SubpassInputMS<int>         subpassInputMSInt;
gl::SubpassInputMS<uint>        subpassInputMSUint;
gl::SubpassInputMS<float>       subpassInputMSFloat;

void  SubpassTest ()
{
    int4    a0 = gl.subpass.Load( subpassInputInt );
    uint4   a1 = gl.subpass.Load( subpassInputUint );
    float4  a2 = gl.subpass.Load( subpassInputFloat );
}


gl::Texture1D<int>              texture1DInt;
gl::Texture1D<uint>             texture1DUint;
gl::Texture1D<float>            texture1DFloat;

gl::Texture2D<int>              texture2DInt;
gl::Texture2D<uint>             texture2DUint;
gl::Texture2D<float>            texture2DFloat;

gl::Texture3D<int>              texture3DInt;
gl::Texture3D<uint>             texture3DUint;
gl::Texture3D<float>            texture3DFloat;

gl::TextureCube<int>            textureCubeInt;
gl::TextureCube<uint>           textureCubeUint;
gl::TextureCube<float>          textureCubeFloat;

gl::Texture1DArray<int>         texture1DArrayInt;
gl::Texture1DArray<uint>        texture1DArrayUint;
gl::Texture1DArray<float>       texture1DArrayFloat;

gl::Texture2DArray<int>         texture2DArrayInt;
gl::Texture2DArray<uint>        texture2DArrayUint;
gl::Texture2DArray<float>       texture2DArrayFloat;

gl::TextureBuffer<int>          textureBufferInt;
gl::TextureBuffer<uint>         textureBufferUint;
gl::TextureBuffer<float>        textureBufferFloat;

gl::Texture2DMS<int>            texture2DMSInt;
gl::Texture2DMS<uint>           texture2DMSUint;
gl::Texture2DMS<float>          texture2DMSFloat;

gl::Texture2DMSArray<int>       texture2DMSArrayInt;
gl::Texture2DMSArray<uint>      texture2DMSArrayUint;
gl::Texture2DMSArray<float>     texture2DMSArrayFloat;

gl::TextureCubeArray<int>       textureCubeArrayInt;
gl::TextureCubeArray<uint>      textureCubeArrayUint;
gl::TextureCubeArray<float>     textureCubeArrayFloat;

void  TextureTest ()
{
    const int   lod     = 0;
    const int   sample  = 0;

    // GetSize
    int     a0 = gl.texture.GetSize( texture1DInt,   lod );
    int     a1 = gl.texture.GetSize( texture1DUint,  lod );
    int     a2 = gl.texture.GetSize( texture1DFloat, lod );

    int2    a3 = gl.texture.GetSize( texture2DInt,   lod );
    int2    a4 = gl.texture.GetSize( texture2DUint,  lod );
    int2    a5 = gl.texture.GetSize( texture2DFloat, lod );

    int3    a6 = gl.texture.GetSize( texture3DInt,   lod );
    int3    a7 = gl.texture.GetSize( texture3DUint,  lod );
    int3    a8 = gl.texture.GetSize( texture3DFloat, lod );

    int2    a9  = gl.texture.GetSize( textureCubeInt,   lod );
    int2    a10 = gl.texture.GetSize( textureCubeUint,  lod );
    int2    a11 = gl.texture.GetSize( textureCubeFloat, lod );

    int2    a12 = gl.texture.GetSize( texture1DArrayInt,   lod );
    int2    a13 = gl.texture.GetSize( texture1DArrayUint,  lod );
    int2    a14 = gl.texture.GetSize( texture1DArrayFloat, lod );

    int3    a15 = gl.texture.GetSize( texture2DArrayInt,   lod );
    int3    a16 = gl.texture.GetSize( texture2DArrayUint,  lod );
    int3    a17 = gl.texture.GetSize( texture2DArrayFloat, lod );

    int     a18 = gl.texture.GetSize( textureBufferInt );
    int     a19 = gl.texture.GetSize( textureBufferUint );
    int     a20 = gl.texture.GetSize( textureBufferFloat );

    int2    a21 = gl.texture.GetSize( texture2DMSInt );
    int2    a22 = gl.texture.GetSize( texture2DMSUint );
    int2    a23 = gl.texture.GetSize( texture2DMSFloat );

    int3    a24 = gl.texture.GetSize( texture2DMSArrayInt );
    int3    a25 = gl.texture.GetSize( texture2DMSArrayUint );
    int3    a26 = gl.texture.GetSize( texture2DMSArrayFloat );

    int3    a27 = gl.texture.GetSize( textureCubeArrayInt,   lod );
    int3    a28 = gl.texture.GetSize( textureCubeArrayUint,  lod );
    int3    a29 = gl.texture.GetSize( textureCubeArrayFloat, lod );

    // QueryLevels
    int     b0 = gl.texture.QueryLevels( texture1DInt );
    int     b1 = gl.texture.QueryLevels( texture1DUint );
    int     b2 = gl.texture.QueryLevels( texture1DFloat );

    int     b3 = gl.texture.QueryLevels( texture2DInt );
    int     b4 = gl.texture.QueryLevels( texture2DUint );
    int     b5 = gl.texture.QueryLevels( texture2DFloat );

    int     b6 = gl.texture.QueryLevels( texture3DInt );
    int     b7 = gl.texture.QueryLevels( texture3DUint );
    int     b8 = gl.texture.QueryLevels( texture3DFloat );

    int     b9  = gl.texture.QueryLevels( textureCubeInt );
    int     b10 = gl.texture.QueryLevels( textureCubeUint );
    int     b11 = gl.texture.QueryLevels( textureCubeFloat );

    int     b12 = gl.texture.QueryLevels( texture1DArrayInt );
    int     b13 = gl.texture.QueryLevels( texture1DArrayUint );
    int     b14 = gl.texture.QueryLevels( texture1DArrayFloat );

    int     b15 = gl.texture.QueryLevels( texture2DArrayInt );
    int     b16 = gl.texture.QueryLevels( texture2DArrayUint );
    int     b17 = gl.texture.QueryLevels( texture2DArrayFloat );

    // GetSamples
    int     c0 = gl.texture.GetSamples( texture2DMSInt );
    int     c1 = gl.texture.GetSamples( texture2DMSUint );
    int     c2 = gl.texture.GetSamples( texture2DMSFloat );

    int     c3 = gl.texture.GetSamples( texture2DMSArrayInt );
    int     c4 = gl.texture.GetSamples( texture2DMSArrayUint );
    int     c5 = gl.texture.GetSamples( texture2DMSArrayFloat );

    // Fetch
    int4    d0 = gl.texture.Fetch( texture1DInt,   int(1), lod );
    uint4   d1 = gl.texture.Fetch( texture1DUint,  int(1), lod );
    float4  d2 = gl.texture.Fetch( texture1DFloat, int(1), lod );

    int4    d3 = gl.texture.Fetch( texture2DInt,   int2(1), lod );
    uint4   d4 = gl.texture.Fetch( texture2DUint,  int2(1), lod );
    float4  d5 = gl.texture.Fetch( texture2DFloat, int2(1), lod );

    int4    d6 = gl.texture.Fetch( texture3DInt,   int3(1), lod );
    uint4   d7 = gl.texture.Fetch( texture3DUint,  int3(1), lod );
    float4  d8 = gl.texture.Fetch( texture3DFloat, int3(1), lod );

//  int4    d9  = gl.texture.Fetch( textureCubeInt,   int2(1), lod );
//  uint4   d10 = gl.texture.Fetch( textureCubeUint,  int2(1), lod );
//  float4  d11 = gl.texture.Fetch( textureCubeFloat, int2(1), lod );

    int4    d12 = gl.texture.Fetch( texture1DArrayInt,   int2(1), lod );
    uint4   d13 = gl.texture.Fetch( texture1DArrayUint,  int2(1), lod );
    float4  d14 = gl.texture.Fetch( texture1DArrayFloat, int2(1), lod );

    int4    d15 = gl.texture.Fetch( texture2DArrayInt,   int3(1), lod );
    uint4   d16 = gl.texture.Fetch( texture2DArrayUint,  int3(1), lod );
    float4  d17 = gl.texture.Fetch( texture2DArrayFloat, int3(1), lod );

    int4    d18 = gl.texture.Fetch( textureBufferInt,   int(1) );
    uint4   d19 = gl.texture.Fetch( textureBufferUint,  int(1) );
    float4  d20 = gl.texture.Fetch( textureBufferFloat, int(1) );

    int4    d21 = gl.texture.Fetch( texture2DMSInt,   int2(1), sample );
    uint4   d22 = gl.texture.Fetch( texture2DMSUint,  int2(1), sample );
    float4  d23 = gl.texture.Fetch( texture2DMSFloat, int2(1), sample );

    int4    d24 = gl.texture.Fetch( texture2DMSArrayInt,   int3(1), sample );
    uint4   d25 = gl.texture.Fetch( texture2DMSArrayUint,  int3(1), sample );
    float4  d26 = gl.texture.Fetch( texture2DMSArrayFloat, int3(1), sample );

//  int4    d27 = gl.texture.Fetch( textureCubeArrayInt,   int3(1), lod );
//  uint4   d28 = gl.texture.Fetch( textureCubeArrayUint,  int3(1), lod );
//  float4  d29 = gl.texture.Fetch( textureCubeArrayFloat, int3(1), lod );

    // FetchOffset
    int4    e0 = gl.texture.FetchOffset( texture1DInt,   int(1), lod, int(1) );
    uint4   e1 = gl.texture.FetchOffset( texture1DUint,  int(1), lod, int(1) );
    float4  e2 = gl.texture.FetchOffset( texture1DFloat, int(1), lod, int(1) );

    int4    e3 = gl.texture.FetchOffset( texture2DInt,   int2(1), lod, int2(1) );
    uint4   e4 = gl.texture.FetchOffset( texture2DUint,  int2(1), lod, int2(1) );
    float4  e5 = gl.texture.FetchOffset( texture2DFloat, int2(1), lod, int2(1) );

    int4    e6 = gl.texture.FetchOffset( texture3DInt,   int3(1), lod, int3(1) );
    uint4   e7 = gl.texture.FetchOffset( texture3DUint,  int3(1), lod, int3(1) );
    float4  e8 = gl.texture.FetchOffset( texture3DFloat, int3(1), lod, int3(1) );

    int4    e9  = gl.texture.FetchOffset( texture1DArrayInt,   int2(1), lod, int(1) );
    uint4   e10 = gl.texture.FetchOffset( texture1DArrayUint,  int2(1), lod, int(1) );
    float4  e11 = gl.texture.FetchOffset( texture1DArrayFloat, int2(1), lod, int(1) );

    int4    e12 = gl.texture.FetchOffset( texture2DArrayInt,   int3(1), lod, int2(1) );
    uint4   e13 = gl.texture.FetchOffset( texture2DArrayUint,  int3(1), lod, int2(1) );
    float4  e14 = gl.texture.FetchOffset( texture2DArrayFloat, int3(1), lod, int2(1) );
}


gl::Sampler                     sampler;
gl::SamplerShadow               samplerShadow;


gl::CombinedTex1D<int>          combinedTex1DInt        {texture1DInt, sampler};
gl::CombinedTex1D<uint>         combinedTex1DUint       {texture1DUint, sampler};
gl::CombinedTex1D<float>        combinedTex1DFloat      {texture1DFloat, sampler};

gl::CombinedTex2D<int>          combinedTex2DInt        {texture2DInt, sampler};
gl::CombinedTex2D<uint>         combinedTex2DUint       {texture2DUint, sampler};
gl::CombinedTex2D<float>        combinedTex2DFloat      {texture2DFloat, sampler};

gl::CombinedTex3D<int>          combinedTex3DInt        {texture3DInt, sampler};
gl::CombinedTex3D<uint>         combinedTex3DUint       {texture3DUint, sampler};
gl::CombinedTex3D<float>        combinedTex3DFloat      {texture3DFloat, sampler};

gl::CombinedTexCube<int>        combinedTexCubeInt      {textureCubeInt, sampler};
gl::CombinedTexCube<uint>       combinedTexCubeUint     {textureCubeUint, sampler};
gl::CombinedTexCube<float>      combinedTexCubeFloat    {textureCubeFloat, sampler};

gl::CombinedTex1DArray<int>     combinedTex1DArrayInt   {texture1DArrayInt, sampler};
gl::CombinedTex1DArray<uint>    combinedTex1DArrayUint  {texture1DArrayUint, sampler};
gl::CombinedTex1DArray<float>   combinedTex1DArrayFloat {texture1DArrayFloat, sampler};

gl::CombinedTex2DArray<int>     combinedTex2DArrayInt   {texture2DArrayInt, sampler};
gl::CombinedTex2DArray<uint>    combinedTex2DArrayUint  {texture2DArrayUint, sampler};
gl::CombinedTex2DArray<float>   combinedTex2DArrayFloat {texture2DArrayFloat, sampler};

gl::CombinedTexBuffer<int>      combinedTexBufferInt    {textureBufferInt, sampler};
gl::CombinedTexBuffer<uint>     combinedTexBufferUint   {textureBufferUint, sampler};
gl::CombinedTexBuffer<float>    combinedTexBufferFloat  {textureBufferFloat, sampler};

gl::CombinedTex2DMS<int>        combinedTex2DMSInt      {texture2DMSInt, sampler};
gl::CombinedTex2DMS<uint>       combinedTex2DMSUint     {texture2DMSUint, sampler};
gl::CombinedTex2DMS<float>      combinedTex2DMSFloat    {texture2DMSFloat, sampler};

gl::CombinedTex2DMSArray<int>   combinedTex2DMSArrayInt     {texture2DMSArrayInt, sampler};
gl::CombinedTex2DMSArray<uint>  combinedTex2DMSArrayUint    {texture2DMSArrayUint, sampler};
gl::CombinedTex2DMSArray<float> combinedTex2DMSArrayFloat   {texture2DMSArrayFloat, sampler};

gl::CombinedTexCubeArray<int>   combinedTexCubeArrayInt     {textureCubeArrayInt, sampler};
gl::CombinedTexCubeArray<uint>  combinedTexCubeArrayUint    {textureCubeArrayUint, sampler};
gl::CombinedTexCubeArray<float> combinedTexCubeArrayFloat   {textureCubeArrayFloat, sampler};

void  CombinedTexTest ()
{
    const int   lodi    = 0;
    const float lodf    = 0.f;
    const int   sample  = 0;
    const float bias    = 0.f;
    const int   comp    = 3;

    // GetSize
    int     a0 = gl.texture.GetSize( combinedTex1DInt,   lodi );
    int     a1 = gl.texture.GetSize( combinedTex1DUint,  lodi );
    int     a2 = gl.texture.GetSize( combinedTex1DFloat, lodi );

    int2    a3 = gl.texture.GetSize( combinedTex2DInt,   lodi );
    int2    a4 = gl.texture.GetSize( combinedTex2DUint,  lodi );
    int2    a5 = gl.texture.GetSize( combinedTex2DFloat, lodi );

    int3    a6 = gl.texture.GetSize( combinedTex3DInt,   lodi );
    int3    a7 = gl.texture.GetSize( combinedTex3DUint,  lodi );
    int3    a8 = gl.texture.GetSize( combinedTex3DFloat, lodi );

    int2    a9  = gl.texture.GetSize( combinedTexCubeInt,   lodi );
    int2    a10 = gl.texture.GetSize( combinedTexCubeUint,  lodi );
    int2    a11 = gl.texture.GetSize( combinedTexCubeFloat, lodi );

    int2    a12 = gl.texture.GetSize( combinedTex1DArrayInt,   lodi );
    int2    a13 = gl.texture.GetSize( combinedTex1DArrayUint,  lodi );
    int2    a14 = gl.texture.GetSize( combinedTex1DArrayFloat, lodi );

    int3    a15 = gl.texture.GetSize( combinedTex2DArrayInt,   lodi );
    int3    a16 = gl.texture.GetSize( combinedTex2DArrayUint,  lodi );
    int3    a17 = gl.texture.GetSize( combinedTex2DArrayFloat, lodi );

    int     a18 = gl.texture.GetSize( combinedTexBufferInt );
    int     a19 = gl.texture.GetSize( combinedTexBufferUint );
    int     a20 = gl.texture.GetSize( combinedTexBufferFloat );

    int2    a21 = gl.texture.GetSize( combinedTex2DMSInt );
    int2    a22 = gl.texture.GetSize( combinedTex2DMSUint );
    int2    a23 = gl.texture.GetSize( combinedTex2DMSFloat );

    int3    a24 = gl.texture.GetSize( combinedTex2DMSArrayInt );
    int3    a25 = gl.texture.GetSize( combinedTex2DMSArrayUint );
    int3    a26 = gl.texture.GetSize( combinedTex2DMSArrayFloat );

    int3    a27 = gl.texture.GetSize( combinedTexCubeArrayInt,   lodi );
    int3    a28 = gl.texture.GetSize( combinedTexCubeArrayUint,  lodi );
    int3    a29 = gl.texture.GetSize( combinedTexCubeArrayFloat, lodi );

    // QueryLevels
    int     b0 = gl.texture.QueryLevels( combinedTex1DInt );
    int     b1 = gl.texture.QueryLevels( combinedTex1DUint );
    int     b2 = gl.texture.QueryLevels( combinedTex1DFloat );

    int     b3 = gl.texture.QueryLevels( combinedTex2DInt );
    int     b4 = gl.texture.QueryLevels( combinedTex2DUint );
    int     b5 = gl.texture.QueryLevels( combinedTex2DFloat );

    int     b6 = gl.texture.QueryLevels( combinedTex3DInt );
    int     b7 = gl.texture.QueryLevels( combinedTex3DUint );
    int     b8 = gl.texture.QueryLevels( combinedTex3DFloat );

    int     b9  = gl.texture.QueryLevels( combinedTexCubeInt );
    int     b10 = gl.texture.QueryLevels( combinedTexCubeUint );
    int     b11 = gl.texture.QueryLevels( combinedTexCubeFloat );

    int     b12 = gl.texture.QueryLevels( combinedTex1DArrayInt );
    int     b13 = gl.texture.QueryLevels( combinedTex1DArrayUint );
    int     b14 = gl.texture.QueryLevels( combinedTex1DArrayFloat );

    int     b15 = gl.texture.QueryLevels( combinedTex2DArrayInt );
    int     b16 = gl.texture.QueryLevels( combinedTex2DArrayUint );
    int     b17 = gl.texture.QueryLevels( combinedTex2DArrayFloat );

    // GetSamples
    int     c0 = gl.texture.GetSamples( combinedTex2DMSInt );
    int     c1 = gl.texture.GetSamples( combinedTex2DMSUint );
    int     c2 = gl.texture.GetSamples( combinedTex2DMSFloat );

    int     c3 = gl.texture.GetSamples( combinedTex2DMSArrayInt );
    int     c4 = gl.texture.GetSamples( combinedTex2DMSArrayUint );
    int     c5 = gl.texture.GetSamples( combinedTex2DMSArrayFloat );

    // Fetch
    int4    d0 = gl.texture.Fetch( combinedTex1DInt,   int(1), lodi );
    uint4   d1 = gl.texture.Fetch( combinedTex1DUint,  int(1), lodi );
    float4  d2 = gl.texture.Fetch( combinedTex1DFloat, int(1), lodi );

    int4    d3 = gl.texture.Fetch( combinedTex2DInt,   int2(1), lodi );
    uint4   d4 = gl.texture.Fetch( combinedTex2DUint,  int2(1), lodi );
    float4  d5 = gl.texture.Fetch( combinedTex2DFloat, int2(1), lodi );

    int4    d6 = gl.texture.Fetch( combinedTex3DInt,   int3(1), lodi );
    uint4   d7 = gl.texture.Fetch( combinedTex3DUint,  int3(1), lodi );
    float4  d8 = gl.texture.Fetch( combinedTex3DFloat, int3(1), lodi );

//  int4    d9  = gl.texture.Fetch( combinedTexCubeInt,   int2(1), lodi );
//  uint4   d10 = gl.texture.Fetch( combinedTexCubeUint,  int2(1), lodi );
//  float4  d11 = gl.texture.Fetch( combinedTexCubeFloat, int2(1), lodi );

    int4    d12 = gl.texture.Fetch( combinedTex1DArrayInt,   int2(1), lodi );
    uint4   d13 = gl.texture.Fetch( combinedTex1DArrayUint,  int2(1), lodi );
    float4  d14 = gl.texture.Fetch( combinedTex1DArrayFloat, int2(1), lodi );

    int4    d15 = gl.texture.Fetch( combinedTex2DArrayInt,   int3(1), lodi );
    uint4   d16 = gl.texture.Fetch( combinedTex2DArrayUint,  int3(1), lodi );
    float4  d17 = gl.texture.Fetch( combinedTex2DArrayFloat, int3(1), lodi );

    int4    d18 = gl.texture.Fetch( combinedTexBufferInt,   int(1) );
    uint4   d19 = gl.texture.Fetch( combinedTexBufferUint,  int(1) );
    float4  d20 = gl.texture.Fetch( combinedTexBufferFloat, int(1) );

    int4    d21 = gl.texture.Fetch( combinedTex2DMSInt,   int2(1), sample );
    uint4   d22 = gl.texture.Fetch( combinedTex2DMSUint,  int2(1), sample );
    float4  d23 = gl.texture.Fetch( combinedTex2DMSFloat, int2(1), sample );

    int4    d24 = gl.texture.Fetch( combinedTex2DMSArrayInt,   int3(1), sample );
    uint4   d25 = gl.texture.Fetch( combinedTex2DMSArrayUint,  int3(1), sample );
    float4  d26 = gl.texture.Fetch( combinedTex2DMSArrayFloat, int3(1), sample );

//  int4    d27 = gl.texture.Fetch( combinedTexCubeArrayInt,   int3(1), lodi );
//  uint4   d28 = gl.texture.Fetch( combinedTexCubeArrayUint,  int3(1), lodi );
//  float4  d29 = gl.texture.Fetch( combinedTexCubeArrayFloat, int3(1), lodi );

    // FetchOffset
    int4    e0 = gl.texture.FetchOffset( combinedTex1DInt,   int(1), lodi, int(1) );
    uint4   e1 = gl.texture.FetchOffset( combinedTex1DUint,  int(1), lodi, int(1) );
    float4  e2 = gl.texture.FetchOffset( combinedTex1DFloat, int(1), lodi, int(1) );

    int4    e3 = gl.texture.FetchOffset( combinedTex2DInt,   int2(1), lodi, int2(1) );
    uint4   e4 = gl.texture.FetchOffset( combinedTex2DUint,  int2(1), lodi, int2(1) );
    float4  e5 = gl.texture.FetchOffset( combinedTex2DFloat, int2(1), lodi, int2(1) );

    int4    e6 = gl.texture.FetchOffset( combinedTex3DInt,   int3(1), lodi, int3(1) );
    uint4   e7 = gl.texture.FetchOffset( combinedTex3DUint,  int3(1), lodi, int3(1) );
    float4  e8 = gl.texture.FetchOffset( combinedTex3DFloat, int3(1), lodi, int3(1) );

    int4    e9  = gl.texture.FetchOffset( combinedTex1DArrayInt,   int2(1), lodi, int(1) );
    uint4   e10 = gl.texture.FetchOffset( combinedTex1DArrayUint,  int2(1), lodi, int(1) );
    float4  e11 = gl.texture.FetchOffset( combinedTex1DArrayFloat, int2(1), lodi, int(1) );

    int4    e12 = gl.texture.FetchOffset( combinedTex2DArrayInt,   int3(1), lodi, int2(1) );
    uint4   e13 = gl.texture.FetchOffset( combinedTex2DArrayUint,  int3(1), lodi, int2(1) );
    float4  e14 = gl.texture.FetchOffset( combinedTex2DArrayFloat, int3(1), lodi, int2(1) );

    // Sample
    int4    f0 = gl.texture.Sample( combinedTex1DInt,   float(1.f) );
    uint4   f1 = gl.texture.Sample( combinedTex1DUint,  float(1.f) );
    float4  f2 = gl.texture.Sample( combinedTex1DFloat, float(1.f) );

    int4    f3 = gl.texture.Sample( combinedTex2DInt,   float2(1.f) );
    uint4   f4 = gl.texture.Sample( combinedTex2DUint,  float2(1.f) );
    float4  f5 = gl.texture.Sample( combinedTex2DFloat, float2(1.f) );

    int4    f6 = gl.texture.Sample( combinedTex3DInt,   float3(1.f) );
    uint4   f7 = gl.texture.Sample( combinedTex3DUint,  float3(1.f) );
    float4  f8 = gl.texture.Sample( combinedTex3DFloat, float3(1.f) );

    int4    f9  = gl.texture.Sample( combinedTexCubeInt,   float3(1.f) );
    uint4   f10 = gl.texture.Sample( combinedTexCubeUint,  float3(1.f) );
    float4  f11 = gl.texture.Sample( combinedTexCubeFloat, float3(1.f) );

    int4    f12 = gl.texture.Sample( combinedTex1DArrayInt,   float2(1.f) );
    uint4   f13 = gl.texture.Sample( combinedTex1DArrayUint,  float2(1.f) );
    float4  f14 = gl.texture.Sample( combinedTex1DArrayFloat, float2(1.f) );

    int4    f15 = gl.texture.Sample( combinedTex2DArrayInt,   float3(1.f) );
    uint4   f16 = gl.texture.Sample( combinedTex2DArrayUint,  float3(1.f) );
    float4  f17 = gl.texture.Sample( combinedTex2DArrayFloat, float3(1.f) );

    int4    f18 = gl.texture.Sample( combinedTexCubeArrayInt,   float4(1.f) );
    uint4   f19 = gl.texture.Sample( combinedTexCubeArrayUint,  float4(1.f) );
    float4  f20 = gl.texture.Sample( combinedTexCubeArrayFloat, float4(1.f) );

    // Sample (bias)
    int4    g0 = gl.texture.Sample( combinedTex1DInt,   float(1.f), bias );
    uint4   g1 = gl.texture.Sample( combinedTex1DUint,  float(1.f), bias );
    float4  g2 = gl.texture.Sample( combinedTex1DFloat, float(1.f), bias );

    int4    g3 = gl.texture.Sample( combinedTex2DInt,   float2(1.f), bias );
    uint4   g4 = gl.texture.Sample( combinedTex2DUint,  float2(1.f), bias );
    float4  g5 = gl.texture.Sample( combinedTex2DFloat, float2(1.f), bias );

    int4    g6 = gl.texture.Sample( combinedTex3DInt,   float3(1.f), bias );
    uint4   g7 = gl.texture.Sample( combinedTex3DUint,  float3(1.f), bias );
    float4  g8 = gl.texture.Sample( combinedTex3DFloat, float3(1.f), bias );

    int4    g9  = gl.texture.Sample( combinedTexCubeInt,   float3(1.f), bias );
    uint4   g10 = gl.texture.Sample( combinedTexCubeUint,  float3(1.f), bias );
    float4  g11 = gl.texture.Sample( combinedTexCubeFloat, float3(1.f), bias );

    int4    g12 = gl.texture.Sample( combinedTex1DArrayInt,   float2(1.f), bias );
    uint4   g13 = gl.texture.Sample( combinedTex1DArrayUint,  float2(1.f), bias );
    float4  g14 = gl.texture.Sample( combinedTex1DArrayFloat, float2(1.f), bias );

    int4    g15 = gl.texture.Sample( combinedTex2DArrayInt,   float3(1.f), bias );
    uint4   g16 = gl.texture.Sample( combinedTex2DArrayUint,  float3(1.f), bias );
    float4  g17 = gl.texture.Sample( combinedTex2DArrayFloat, float3(1.f), bias );

    int4    g18 = gl.texture.Sample( combinedTexCubeArrayInt,   float4(1.f), bias );
    uint4   g19 = gl.texture.Sample( combinedTexCubeArrayUint,  float4(1.f), bias );
    float4  g20 = gl.texture.Sample( combinedTexCubeArrayFloat, float4(1.f), bias );

    // SampleOffset
    int4    h0 = gl.texture.SampleOffset( combinedTex1DInt,   float(1.f), int(1) );
    uint4   h1 = gl.texture.SampleOffset( combinedTex1DUint,  float(1.f), int(1) );
    float4  h2 = gl.texture.SampleOffset( combinedTex1DFloat, float(1.f), int(1) );

    int4    h3 = gl.texture.SampleOffset( combinedTex2DInt,   float2(1.f), int2(1) );
    uint4   h4 = gl.texture.SampleOffset( combinedTex2DUint,  float2(1.f), int2(1) );
    float4  h5 = gl.texture.SampleOffset( combinedTex2DFloat, float2(1.f), int2(1) );

    int4    h6 = gl.texture.SampleOffset( combinedTex3DInt,   float3(1.f), int3(1) );
    uint4   h7 = gl.texture.SampleOffset( combinedTex3DUint,  float3(1.f), int3(1) );
    float4  h8 = gl.texture.SampleOffset( combinedTex3DFloat, float3(1.f), int3(1) );

    int4    h9  = gl.texture.SampleOffset( combinedTex1DArrayInt,   float2(1.f), int(1) );
    uint4   h10 = gl.texture.SampleOffset( combinedTex1DArrayUint,  float2(1.f), int(1) );
    float4  h11 = gl.texture.SampleOffset( combinedTex1DArrayFloat, float2(1.f), int(1) );

    int4    h12 = gl.texture.SampleOffset( combinedTex2DArrayInt,   float3(1.f), int2(1) );
    uint4   h13 = gl.texture.SampleOffset( combinedTex2DArrayUint,  float3(1.f), int2(1) );
    float4  h14 = gl.texture.SampleOffset( combinedTex2DArrayFloat, float3(1.f), int2(1) );

    // SampleOffset (bias)
    int4    i0 = gl.texture.SampleOffset( combinedTex1DInt,   float(1.f), int(1), bias );
    uint4   i1 = gl.texture.SampleOffset( combinedTex1DUint,  float(1.f), int(1), bias );
    float4  i2 = gl.texture.SampleOffset( combinedTex1DFloat, float(1.f), int(1), bias );

    int4    i3 = gl.texture.SampleOffset( combinedTex2DInt,   float2(1.f), int2(1), bias );
    uint4   i4 = gl.texture.SampleOffset( combinedTex2DUint,  float2(1.f), int2(1), bias );
    float4  i5 = gl.texture.SampleOffset( combinedTex2DFloat, float2(1.f), int2(1), bias );

    int4    i6 = gl.texture.SampleOffset( combinedTex3DInt,   float3(1.f), int3(1), bias );
    uint4   i7 = gl.texture.SampleOffset( combinedTex3DUint,  float3(1.f), int3(1), bias );
    float4  i8 = gl.texture.SampleOffset( combinedTex3DFloat, float3(1.f), int3(1), bias );

    int4    i9  = gl.texture.SampleOffset( combinedTex1DArrayInt,   float2(1.f), int(1), bias );
    uint4   i10 = gl.texture.SampleOffset( combinedTex1DArrayUint,  float2(1.f), int(1), bias );
    float4  i11 = gl.texture.SampleOffset( combinedTex1DArrayFloat, float2(1.f), int(1), bias );

    int4    i12 = gl.texture.SampleOffset( combinedTex2DArrayInt,   float3(1.f), int2(1), bias );
    uint4   i13 = gl.texture.SampleOffset( combinedTex2DArrayUint,  float3(1.f), int2(1), bias );
    float4  i14 = gl.texture.SampleOffset( combinedTex2DArrayFloat, float3(1.f), int2(1), bias );

    // SampleLod
    int4    j0 = gl.texture.SampleLod( combinedTex1DInt,   float(1.f), lodf );
    uint4   j1 = gl.texture.SampleLod( combinedTex1DUint,  float(1.f), lodf );
    float4  j2 = gl.texture.SampleLod( combinedTex1DFloat, float(1.f), lodf );

    int4    j3 = gl.texture.SampleLod( combinedTex2DInt,   float2(1.f), lodf );
    uint4   j4 = gl.texture.SampleLod( combinedTex2DUint,  float2(1.f), lodf );
    float4  j5 = gl.texture.SampleLod( combinedTex2DFloat, float2(1.f), lodf );

    int4    j6 = gl.texture.SampleLod( combinedTex3DInt,   float3(1.f), lodf );
    uint4   j7 = gl.texture.SampleLod( combinedTex3DUint,  float3(1.f), lodf );
    float4  j8 = gl.texture.SampleLod( combinedTex3DFloat, float3(1.f), lodf );

    int4    j9  = gl.texture.SampleLod( combinedTexCubeInt,   float3(1.f), lodf );
    uint4   j10 = gl.texture.SampleLod( combinedTexCubeUint,  float3(1.f), lodf );
    float4  j11 = gl.texture.SampleLod( combinedTexCubeFloat, float3(1.f), lodf );

    int4    j12 = gl.texture.SampleLod( combinedTex1DArrayInt,   float2(1.f), lodf );
    uint4   j13 = gl.texture.SampleLod( combinedTex1DArrayUint,  float2(1.f), lodf );
    float4  j14 = gl.texture.SampleLod( combinedTex1DArrayFloat, float2(1.f), lodf );

    int4    j15 = gl.texture.SampleLod( combinedTex2DArrayInt,   float3(1.f), lodf );
    uint4   j16 = gl.texture.SampleLod( combinedTex2DArrayUint,  float3(1.f), lodf );
    float4  j17 = gl.texture.SampleLod( combinedTex2DArrayFloat, float3(1.f), lodf );

    int4    j18 = gl.texture.SampleLod( combinedTexCubeArrayInt,   float4(1.f), lodf );
    uint4   j19 = gl.texture.SampleLod( combinedTexCubeArrayUint,  float4(1.f), lodf );
    float4  j20 = gl.texture.SampleLod( combinedTexCubeArrayFloat, float4(1.f), lodf );

    // SampleLodOffset
    int4    k0 = gl.texture.SampleLodOffset( combinedTex1DInt,   float(1.f), lodf, int(1) );
    uint4   k1 = gl.texture.SampleLodOffset( combinedTex1DUint,  float(1.f), lodf, int(1) );
    float4  k2 = gl.texture.SampleLodOffset( combinedTex1DFloat, float(1.f), lodf, int(1) );

    int4    k3 = gl.texture.SampleLodOffset( combinedTex2DInt,   float2(1.f), lodf, int2(1) );
    uint4   k4 = gl.texture.SampleLodOffset( combinedTex2DUint,  float2(1.f), lodf, int2(1) );
    float4  k5 = gl.texture.SampleLodOffset( combinedTex2DFloat, float2(1.f), lodf, int2(1) );

    int4    k6 = gl.texture.SampleLodOffset( combinedTex3DInt,   float3(1.f), lodf, int3(1) );
    uint4   k7 = gl.texture.SampleLodOffset( combinedTex3DUint,  float3(1.f), lodf, int3(1) );
    float4  k8 = gl.texture.SampleLodOffset( combinedTex3DFloat, float3(1.f), lodf, int3(1) );

    int4    k9  = gl.texture.SampleLodOffset( combinedTex1DArrayInt,   float2(1.f), lodf, int(1) );
    uint4   k10 = gl.texture.SampleLodOffset( combinedTex1DArrayUint,  float2(1.f), lodf, int(1) );
    float4  k11 = gl.texture.SampleLodOffset( combinedTex1DArrayFloat, float2(1.f), lodf, int(1) );

    int4    k12 = gl.texture.SampleLodOffset( combinedTex2DArrayInt,   float3(1.f), lodf, int2(1) );
    uint4   k13 = gl.texture.SampleLodOffset( combinedTex2DArrayUint,  float3(1.f), lodf, int2(1) );
    float4  k14 = gl.texture.SampleLodOffset( combinedTex2DArrayFloat, float3(1.f), lodf, int2(1) );

    // Gather
    int4    l0 = gl.texture.Gather( combinedTex2DInt,   float2(1.f) );
    uint4   l1 = gl.texture.Gather( combinedTex2DUint,  float2(1.f) );
    float4  l2 = gl.texture.Gather( combinedTex2DFloat, float2(1.f) );

    int4    l3 = gl.texture.Gather( combinedTexCubeInt,   float3(1.f) );
    uint4   l4 = gl.texture.Gather( combinedTexCubeUint,  float3(1.f) );
    float4  l5 = gl.texture.Gather( combinedTexCubeFloat, float3(1.f) );

    int4    l6 = gl.texture.Gather( combinedTex2DArrayInt,   float3(1.f) );
    uint4   l7 = gl.texture.Gather( combinedTex2DArrayUint,  float3(1.f) );
    float4  l8 = gl.texture.Gather( combinedTex2DArrayFloat, float3(1.f) );

    int4    l9  = gl.texture.Gather( combinedTexCubeArrayInt,   float4(1.f) );
    uint4   l10 = gl.texture.Gather( combinedTexCubeArrayUint,  float4(1.f) );
    float4  l11 = gl.texture.Gather( combinedTexCubeArrayFloat, float4(1.f) );

    // Gather (comp)
    int4    m0 = gl.texture.Gather( combinedTex2DInt,   float2(1.f), comp );
    uint4   m1 = gl.texture.Gather( combinedTex2DUint,  float2(1.f), comp );
    float4  m2 = gl.texture.Gather( combinedTex2DFloat, float2(1.f), comp );

    int4    m3 = gl.texture.Gather( combinedTexCubeInt,   float3(1.f), comp );
    uint4   m4 = gl.texture.Gather( combinedTexCubeUint,  float3(1.f), comp );
    float4  m5 = gl.texture.Gather( combinedTexCubeFloat, float3(1.f), comp );

    int4    m6 = gl.texture.Gather( combinedTex2DArrayInt,   float3(1.f), comp );
    uint4   m7 = gl.texture.Gather( combinedTex2DArrayUint,  float3(1.f), comp );
    float4  m8 = gl.texture.Gather( combinedTex2DArrayFloat, float3(1.f), comp );

    int4    m9  = gl.texture.Gather( combinedTexCubeArrayInt,   float4(1.f), comp );
    uint4   m10 = gl.texture.Gather( combinedTexCubeArrayUint,  float4(1.f), comp );
    float4  m11 = gl.texture.Gather( combinedTexCubeArrayFloat, float4(1.f), comp );

    // GatherOffset
    int4    n0 = gl.texture.GatherOffset( combinedTex2DInt,   float2(1.f), int2(1) );
    uint4   n1 = gl.texture.GatherOffset( combinedTex2DUint,  float2(1.f), int2(1) );
    float4  n2 = gl.texture.GatherOffset( combinedTex2DFloat, float2(1.f), int2(1) );

    int4    n3 = gl.texture.GatherOffset( combinedTex2DArrayInt,   float3(1.f), int2(1) );
    uint4   n4 = gl.texture.GatherOffset( combinedTex2DArrayUint,  float3(1.f), int2(1) );
    float4  n5 = gl.texture.GatherOffset( combinedTex2DArrayFloat, float3(1.f), int2(1) );

    // GatherOffset (comp)
    int4    o0 = gl.texture.GatherOffset( combinedTex2DInt,   float2(1.f), int2(1), comp );
    uint4   o1 = gl.texture.GatherOffset( combinedTex2DUint,  float2(1.f), int2(1), comp );
    float4  o2 = gl.texture.GatherOffset( combinedTex2DFloat, float2(1.f), int2(1), comp );

    int4    o3 = gl.texture.GatherOffset( combinedTex2DArrayInt,   float3(1.f), int2(1), comp );
    uint4   o4 = gl.texture.GatherOffset( combinedTex2DArrayUint,  float3(1.f), int2(1), comp );
    float4  o5 = gl.texture.GatherOffset( combinedTex2DArrayFloat, float3(1.f), int2(1), comp );

    // GatherOffsets
    const int2  offsets[4] = {};
    int4    p0 = gl.texture.GatherOffsets( combinedTex2DInt,   float2(1.f), offsets );
    uint4   p1 = gl.texture.GatherOffsets( combinedTex2DUint,  float2(1.f), offsets );
    float4  p2 = gl.texture.GatherOffsets( combinedTex2DFloat, float2(1.f), offsets );

    int4    p3 = gl.texture.GatherOffsets( combinedTex2DArrayInt,   float3(1.f), offsets );
    uint4   p4 = gl.texture.GatherOffsets( combinedTex2DArrayUint,  float3(1.f), offsets );
    float4  p5 = gl.texture.GatherOffsets( combinedTex2DArrayFloat, float3(1.f), offsets );

    // GatherOffsets (comp)
    int4    q0 = gl.texture.GatherOffsets( combinedTex2DInt,   float2(1.f), offsets, comp );
    uint4   q1 = gl.texture.GatherOffsets( combinedTex2DUint,  float2(1.f), offsets, comp );
    float4  q2 = gl.texture.GatherOffsets( combinedTex2DFloat, float2(1.f), offsets, comp );

    int4    q3 = gl.texture.GatherOffsets( combinedTex2DArrayInt,   float3(1.f), offsets, comp );
    uint4   q4 = gl.texture.GatherOffsets( combinedTex2DArrayUint,  float3(1.f), offsets, comp );
    float4  q5 = gl.texture.GatherOffsets( combinedTex2DArrayFloat, float3(1.f), offsets, comp );

    // SampleGrad
    int4    r0 = gl.texture.SampleGrad( combinedTex1DInt,   float(2.f), float(0.f), float(1.f) );
    uint4   r1 = gl.texture.SampleGrad( combinedTex1DUint,  float(2.f), float(0.f), float(1.f) );
    float4  r2 = gl.texture.SampleGrad( combinedTex1DFloat, float(2.f), float(0.f), float(1.f) );

    int4    r3 = gl.texture.SampleGrad( combinedTex2DInt,   float2(2.f), float2(0.f), float2(1.f) );
    uint4   r4 = gl.texture.SampleGrad( combinedTex2DUint,  float2(2.f), float2(0.f), float2(1.f) );
    float4  r5 = gl.texture.SampleGrad( combinedTex2DFloat, float2(2.f), float2(0.f), float2(1.f) );

    int4    r6 = gl.texture.SampleGrad( combinedTex3DInt,   float3(2.f), float3(0.f), float3(1.f) );
    uint4   r7 = gl.texture.SampleGrad( combinedTex3DUint,  float3(2.f), float3(0.f), float3(1.f) );
    float4  r8 = gl.texture.SampleGrad( combinedTex3DFloat, float3(2.f), float3(0.f), float3(1.f) );

    int4    r9  = gl.texture.SampleGrad( combinedTexCubeInt,   float3(2.f), float3(0.f), float3(1.f) );
    uint4   r10 = gl.texture.SampleGrad( combinedTexCubeUint,  float3(2.f), float3(0.f), float3(1.f) );
    float4  r11 = gl.texture.SampleGrad( combinedTexCubeFloat, float3(2.f), float3(0.f), float3(1.f) );

    int4    r12 = gl.texture.SampleGrad( combinedTex1DArrayInt,   float2(2.f), float(0.f), float(1.f) );
    uint4   r13 = gl.texture.SampleGrad( combinedTex1DArrayUint,  float2(2.f), float(0.f), float(1.f) );
    float4  r14 = gl.texture.SampleGrad( combinedTex1DArrayFloat, float2(2.f), float(0.f), float(1.f) );

    int4    r15 = gl.texture.SampleGrad( combinedTex2DArrayInt,   float3(2.f), float2(0.f), float2(1.f) );
    uint4   r16 = gl.texture.SampleGrad( combinedTex2DArrayUint,  float3(2.f), float2(0.f), float2(1.f) );
    float4  r17 = gl.texture.SampleGrad( combinedTex2DArrayFloat, float3(2.f), float2(0.f), float2(1.f) );

    int4    r18 = gl.texture.SampleGrad( combinedTexCubeArrayInt,   float4(2.f), float3(0.f), float3(1.f) );
    uint4   r19 = gl.texture.SampleGrad( combinedTexCubeArrayUint,  float4(2.f), float3(0.f), float3(1.f) );
    float4  r20 = gl.texture.SampleGrad( combinedTexCubeArrayFloat, float4(2.f), float3(0.f), float3(1.f) );

    // SampleGradOffset
    int4    s0 = gl.texture.SampleGradOffset( combinedTex1DInt,   float(2.f), float(0.f), float(1.f), int(1) );
    uint4   s1 = gl.texture.SampleGradOffset( combinedTex1DUint,  float(2.f), float(0.f), float(1.f), int(1) );
    float4  s2 = gl.texture.SampleGradOffset( combinedTex1DFloat, float(2.f), float(0.f), float(1.f), int(1) );

    int4    s3 = gl.texture.SampleGradOffset( combinedTex2DInt,   float2(2.f), float2(0.f), float2(1.f), int2(1) );
    uint4   s4 = gl.texture.SampleGradOffset( combinedTex2DUint,  float2(2.f), float2(0.f), float2(1.f), int2(1) );
    float4  s5 = gl.texture.SampleGradOffset( combinedTex2DFloat, float2(2.f), float2(0.f), float2(1.f), int2(1) );

    int4    s6 = gl.texture.SampleGradOffset( combinedTex3DInt,   float3(2.f), float3(0.f), float3(1.f), int3(1) );
    uint4   s7 = gl.texture.SampleGradOffset( combinedTex3DUint,  float3(2.f), float3(0.f), float3(1.f), int3(1) );
    float4  s8 = gl.texture.SampleGradOffset( combinedTex3DFloat, float3(2.f), float3(0.f), float3(1.f), int3(1) );

    int4    s9  = gl.texture.SampleGradOffset( combinedTex1DArrayInt,   float2(2.f), float(0.f), float(1.f), int(1) );
    uint4   s10 = gl.texture.SampleGradOffset( combinedTex1DArrayUint,  float2(2.f), float(0.f), float(1.f), int(1) );
    float4  s11 = gl.texture.SampleGradOffset( combinedTex1DArrayFloat, float2(2.f), float(0.f), float(1.f), int(1) );

    int4    s12 = gl.texture.SampleGradOffset( combinedTex2DArrayInt,   float3(2.f), float2(0.f), float2(1.f), int2(1) );
    uint4   s13 = gl.texture.SampleGradOffset( combinedTex2DArrayUint,  float3(2.f), float2(0.f), float2(1.f), int2(1) );
    float4  s14 = gl.texture.SampleGradOffset( combinedTex2DArrayFloat, float3(2.f), float2(0.f), float2(1.f), int2(1) );

    // SampleProj
    int4    t0 = gl.texture.SampleProj( combinedTex1DInt,   float4(2.f) );
    uint4   t1 = gl.texture.SampleProj( combinedTex1DUint,  float4(2.f) );
    float4  t2 = gl.texture.SampleProj( combinedTex1DFloat, float4(2.f) );

    int4    t3 = gl.texture.SampleProj( combinedTex2DInt,   float4(2.f) );
    uint4   t4 = gl.texture.SampleProj( combinedTex2DUint,  float4(2.f) );
    float4  t5 = gl.texture.SampleProj( combinedTex2DFloat, float4(2.f) );

    int4    t6 = gl.texture.SampleProj( combinedTex3DInt,   float4(2.f) );
    uint4   t7 = gl.texture.SampleProj( combinedTex3DUint,  float4(2.f) );
    float4  t8 = gl.texture.SampleProj( combinedTex3DFloat, float4(2.f) );

    // SampleProj (bias)
    int4    y0 = gl.texture.SampleProj( combinedTex1DInt,   float4(2.f), bias );
    uint4   y1 = gl.texture.SampleProj( combinedTex1DUint,  float4(2.f), bias );
    float4  y2 = gl.texture.SampleProj( combinedTex1DFloat, float4(2.f), bias );

    int4    y3 = gl.texture.SampleProj( combinedTex2DInt,   float4(2.f), bias );
    uint4   y4 = gl.texture.SampleProj( combinedTex2DUint,  float4(2.f), bias );
    float4  y5 = gl.texture.SampleProj( combinedTex2DFloat, float4(2.f), bias );

    int4    y6 = gl.texture.SampleProj( combinedTex3DInt,   float4(2.f), bias );
    uint4   y7 = gl.texture.SampleProj( combinedTex3DUint,  float4(2.f), bias );
    float4  y8 = gl.texture.SampleProj( combinedTex3DFloat, float4(2.f), bias );

    // SampleProjLod
    int4    x0 = gl.texture.SampleProjLod( combinedTex1DInt,   float4(2.f), lodf );
    uint4   x1 = gl.texture.SampleProjLod( combinedTex1DUint,  float4(2.f), lodf );
    float4  x2 = gl.texture.SampleProjLod( combinedTex1DFloat, float4(2.f), lodf );

    int4    x3 = gl.texture.SampleProjLod( combinedTex2DInt,   float4(2.f), lodf );
    uint4   x4 = gl.texture.SampleProjLod( combinedTex2DUint,  float4(2.f), lodf );
    float4  x5 = gl.texture.SampleProjLod( combinedTex2DFloat, float4(2.f), lodf );

    int4    x6 = gl.texture.SampleProjLod( combinedTex3DInt,   float4(2.f), lodf );
    uint4   x7 = gl.texture.SampleProjLod( combinedTex3DUint,  float4(2.f), lodf );
    float4  x8 = gl.texture.SampleProjLod( combinedTex3DFloat, float4(2.f), lodf );

    // SampleProjLodOffset
    int4    z0 = gl.texture.SampleProjLodOffset( combinedTex1DInt,   float4(2.f), lodf, int(1) );
    uint4   z1 = gl.texture.SampleProjLodOffset( combinedTex1DUint,  float4(2.f), lodf, int(1) );
    float4  z2 = gl.texture.SampleProjLodOffset( combinedTex1DFloat, float4(2.f), lodf, int(1) );

    int4    z3 = gl.texture.SampleProjLodOffset( combinedTex2DInt,   float4(2.f), lodf, int2(1) );
    uint4   z4 = gl.texture.SampleProjLodOffset( combinedTex2DUint,  float4(2.f), lodf, int2(1) );
    float4  z5 = gl.texture.SampleProjLodOffset( combinedTex2DFloat, float4(2.f), lodf, int2(1) );

    int4    z6 = gl.texture.SampleProjLodOffset( combinedTex3DInt,   float4(2.f), lodf, int3(1) );
    uint4   z7 = gl.texture.SampleProjLodOffset( combinedTex3DUint,  float4(2.f), lodf, int3(1) );
    float4  z8 = gl.texture.SampleProjLodOffset( combinedTex3DFloat, float4(2.f), lodf, int3(1) );

    // SampleProjGrad
    int4    v0 = gl.texture.SampleProjGrad( combinedTex1DInt,   float4(2.f), float(0.f), float(1.f) );
    uint4   v1 = gl.texture.SampleProjGrad( combinedTex1DUint,  float4(2.f), float(0.f), float(1.f) );
    float4  v2 = gl.texture.SampleProjGrad( combinedTex1DFloat, float4(2.f), float(0.f), float(1.f) );

    int4    v3 = gl.texture.SampleProjGrad( combinedTex2DInt,   float4(2.f), float2(0.f), float2(1.f) );
    uint4   v4 = gl.texture.SampleProjGrad( combinedTex2DUint,  float4(2.f), float2(0.f), float2(1.f) );
    float4  v5 = gl.texture.SampleProjGrad( combinedTex2DFloat, float4(2.f), float2(0.f), float2(1.f) );

    int4    v6 = gl.texture.SampleProjGrad( combinedTex3DInt,   float4(2.f), float3(0.f), float3(1.f) );
    uint4   v7 = gl.texture.SampleProjGrad( combinedTex3DUint,  float4(2.f), float3(0.f), float3(1.f) );
    float4  v8 = gl.texture.SampleProjGrad( combinedTex3DFloat, float4(2.f), float3(0.f), float3(1.f) );

    // SampleProjGradOffset
    int4    u0 = gl.texture.SampleProjGradOffset( combinedTex1DInt,   float4(2.f), float(0.f), float(1.f), int(1) );
    uint4   u1 = gl.texture.SampleProjGradOffset( combinedTex1DUint,  float4(2.f), float(0.f), float(1.f), int(1) );
    float4  u2 = gl.texture.SampleProjGradOffset( combinedTex1DFloat, float4(2.f), float(0.f), float(1.f), int(1) );

    int4    u3 = gl.texture.SampleProjGradOffset( combinedTex2DInt,   float4(2.f), float2(0.f), float2(1.f), int2(1) );
    uint4   u4 = gl.texture.SampleProjGradOffset( combinedTex2DUint,  float4(2.f), float2(0.f), float2(1.f), int2(1) );
    float4  u5 = gl.texture.SampleProjGradOffset( combinedTex2DFloat, float4(2.f), float2(0.f), float2(1.f), int2(1) );

    int4    u6 = gl.texture.SampleProjGradOffset( combinedTex3DInt,   float4(2.f), float3(0.f), float3(1.f), int3(1) );
    uint4   u7 = gl.texture.SampleProjGradOffset( combinedTex3DUint,  float4(2.f), float3(0.f), float3(1.f), int3(1) );
    float4  u8 = gl.texture.SampleProjGradOffset( combinedTex3DFloat, float4(2.f), float3(0.f), float3(1.f), int3(1) );

    // SampleProjOffset
    int4    w0 = gl.texture.SampleProjOffset( combinedTex1DInt,   float4(2.f), int(1) );
    uint4   w1 = gl.texture.SampleProjOffset( combinedTex1DUint,  float4(2.f), int(1) );
    float4  w2 = gl.texture.SampleProjOffset( combinedTex1DFloat, float4(2.f), int(1) );

    int4    w3 = gl.texture.SampleProjOffset( combinedTex2DInt,   float4(2.f), int2(1) );
    uint4   w4 = gl.texture.SampleProjOffset( combinedTex2DUint,  float4(2.f), int2(1) );
    float4  w5 = gl.texture.SampleProjOffset( combinedTex2DFloat, float4(2.f), int2(1) );

    int4    w6 = gl.texture.SampleProjOffset( combinedTex3DInt,   float4(2.f), int3(1) );
    uint4   w7 = gl.texture.SampleProjOffset( combinedTex3DUint,  float4(2.f), int3(1) );
    float4  w8 = gl.texture.SampleProjOffset( combinedTex3DFloat, float4(2.f), int3(1) );

    // SampleProjOffset (bias)
    int4    w10 = gl.texture.SampleProjOffset( combinedTex1DInt,   float4(2.f), int(1), bias );
    uint4   w11 = gl.texture.SampleProjOffset( combinedTex1DUint,  float4(2.f), int(1), bias );
    float4  w12 = gl.texture.SampleProjOffset( combinedTex1DFloat, float4(2.f), int(1), bias );

    int4    w13 = gl.texture.SampleProjOffset( combinedTex2DInt,   float4(2.f), int2(1), bias );
    uint4   w14 = gl.texture.SampleProjOffset( combinedTex2DUint,  float4(2.f), int2(1), bias );
    float4  w15 = gl.texture.SampleProjOffset( combinedTex2DFloat, float4(2.f), int2(1), bias );

    int4    w16 = gl.texture.SampleProjOffset( combinedTex3DInt,   float4(2.f), int3(1), bias );
    uint4   w17 = gl.texture.SampleProjOffset( combinedTex3DUint,  float4(2.f), int3(1), bias );
    float4  w18 = gl.texture.SampleProjOffset( combinedTex3DFloat, float4(2.f), int3(1), bias );

    // QueryLod
    float2  _0 = gl.texture.QueryLod( combinedTex1DInt,   float(1.f) );
    float2  _1 = gl.texture.QueryLod( combinedTex1DUint,  float(1.f) );
    float2  _2 = gl.texture.QueryLod( combinedTex1DFloat, float(1.f) );

    float2  _3 = gl.texture.QueryLod( combinedTex2DInt,   float2(1.f) );
    float2  _4 = gl.texture.QueryLod( combinedTex2DUint,  float2(1.f) );
    float2  _5 = gl.texture.QueryLod( combinedTex2DFloat, float2(1.f) );

    float2  _6 = gl.texture.QueryLod( combinedTex3DInt,   float3(1.f) );
    float2  _7 = gl.texture.QueryLod( combinedTex3DUint,  float3(1.f) );
    float2  _8 = gl.texture.QueryLod( combinedTex3DFloat, float3(1.f) );

    float2  _9  = gl.texture.QueryLod( combinedTexCubeInt,   float3(1.f) );
    float2  _10 = gl.texture.QueryLod( combinedTexCubeUint,  float3(1.f) );
    float2  _11 = gl.texture.QueryLod( combinedTexCubeFloat, float3(1.f) );

    float2  _12 = gl.texture.QueryLod( combinedTex1DArrayInt,   float(1.f) );
    float2  _13 = gl.texture.QueryLod( combinedTex1DArrayUint,  float(1.f) );
    float2  _14 = gl.texture.QueryLod( combinedTex1DArrayFloat, float(1.f) );

    float2  _15 = gl.texture.QueryLod( combinedTex2DArrayInt,   float2(1.f) );
    float2  _16 = gl.texture.QueryLod( combinedTex2DArrayUint,  float2(1.f) );
    float2  _17 = gl.texture.QueryLod( combinedTex2DArrayFloat, float2(1.f) );

    float2  _18 = gl.texture.QueryLod( combinedTexCubeArrayInt,   float3(1.f) );
    float2  _19 = gl.texture.QueryLod( combinedTexCubeArrayUint,  float3(1.f) );
    float2  _20 = gl.texture.QueryLod( combinedTexCubeArrayFloat, float3(1.f) );
}


gl::Image1D<int>                image1DInt;
gl::Image1D<uint>               image1DUint;
gl::Image1D<float>              image1DFloat;

gl::Image2D<int>                image2DInt;
gl::Image2D<uint>               image2DUint;
gl::Image2D<float>              image2DFloat;

gl::Image3D<int>                image3DInt;
gl::Image3D<uint>               image3DUint;
gl::Image3D<float>              image3DFloat;

gl::ImageCube<int>              imageCubeInt;
gl::ImageCube<uint>             imageCubeUint;
gl::ImageCube<float>            imageCubeFloat;

gl::ImageBuffer<int>            imageBufferInt;
gl::ImageBuffer<uint>           imageBufferUint;
gl::ImageBuffer<float>          imageBufferFloat;

gl::Image1DArray<int>           image1DArrayInt;
gl::Image1DArray<uint>          image1DArrayUint;
gl::Image1DArray<float>         image1DArrayFloat;

gl::Image2DArray<int>           image2DArrayInt;
gl::Image2DArray<uint>          image2DArrayUint;
gl::Image2DArray<float>         image2DArrayFloat;

gl::ImageCubeArray<int>         imageCubeArrayInt;
gl::ImageCubeArray<uint>        imageCubeArrayUint;
gl::ImageCubeArray<float>       imageCubeArrayFloat;

gl::Image2DMS<int>              image2DMSInt;
gl::Image2DMS<uint>             image2DMSUint;
gl::Image2DMS<float>            image2DMSFloat;

gl::Image2DMSArray<int>         image2DMSArrayInt;
gl::Image2DMSArray<uint>        image2DMSArrayUint;
gl::Image2DMSArray<float>       image2DMSArrayFloat;

void  ImageTest ()
{
    const int   datai       = 1;
    const uint  datau       = 2;
    const float dataf       = 3.f;

    const int4   data4i     = int4(1);
    const uint4  data4u     = uint4(2);
    const float4 data4f     = float4(3.f);

    const int   comparei    = 3;
    const uint  compareu    = 4;

    const int   sample      = 0;

    // AtomicAdd
    int     a0 = gl.image.AtomicAdd( image1DInt,  int(1), datai );
    uint    a1 = gl.image.AtomicAdd( image1DUint, int(1), datau );

    int     a2 = gl.image.AtomicAdd( image2DInt,  int2(1), datai );
    uint    a3 = gl.image.AtomicAdd( image2DUint, int2(1), datau );

    int     a4 = gl.image.AtomicAdd( image3DInt,  int3(1), datai );
    uint    a5 = gl.image.AtomicAdd( image3DUint, int3(1), datau );

    int     a6 = gl.image.AtomicAdd( imageCubeInt,  int3(1), datai );
    uint    a7 = gl.image.AtomicAdd( imageCubeUint, int3(1), datau );

    int     a8 = gl.image.AtomicAdd( imageBufferInt,  int(1), datai );
    uint    a9 = gl.image.AtomicAdd( imageBufferUint, int(1), datau );

    int     a10 = gl.image.AtomicAdd( image1DArrayInt,  int2(1), datai );
    uint    a11 = gl.image.AtomicAdd( image1DArrayUint, int2(1), datau );

    int     a12 = gl.image.AtomicAdd( image2DArrayInt,  int3(1), datai );
    uint    a13 = gl.image.AtomicAdd( image2DArrayUint, int3(1), datau );

    int     a14 = gl.image.AtomicAdd( imageCubeArrayInt,  int3(1), datai );
    uint    a15 = gl.image.AtomicAdd( imageCubeArrayUint, int3(1), datau );

    int     a16 = gl.image.AtomicAdd( image2DMSInt,  int2(1), sample, datai );
    uint    a17 = gl.image.AtomicAdd( image2DMSUint, int2(1), sample, datau );

    int     a18 = gl.image.AtomicAdd( image2DMSArrayInt,  int3(1), sample, datai );
    uint    a19 = gl.image.AtomicAdd( image2DMSArrayUint, int3(1), sample, datau );

    // AtomicAnd
    int     b0 = gl.image.AtomicAnd( image1DInt,  int(1), datai );
    uint    b1 = gl.image.AtomicAnd( image1DUint, int(1), datau );

    int     b2 = gl.image.AtomicAnd( image2DInt,  int2(1), datai );
    uint    b3 = gl.image.AtomicAnd( image2DUint, int2(1), datau );

    int     b4 = gl.image.AtomicAnd( image3DInt,  int3(1), datai );
    uint    b5 = gl.image.AtomicAnd( image3DUint, int3(1), datau );

    int     b6 = gl.image.AtomicAnd( imageCubeInt,  int3(1), datai );
    uint    b7 = gl.image.AtomicAnd( imageCubeUint, int3(1), datau );

    int     b8 = gl.image.AtomicAnd( imageBufferInt,  int(1), datai );
    uint    b9 = gl.image.AtomicAnd( imageBufferUint, int(1), datau );

    int     b10 = gl.image.AtomicAnd( image1DArrayInt,  int2(1), datai );
    uint    b11 = gl.image.AtomicAnd( image1DArrayUint, int2(1), datau );

    int     b12 = gl.image.AtomicAnd( image2DArrayInt,  int3(1), datai );
    uint    b13 = gl.image.AtomicAnd( image2DArrayUint, int3(1), datau );

    int     b14 = gl.image.AtomicAnd( imageCubeArrayInt,  int3(1), datai );
    uint    b15 = gl.image.AtomicAnd( imageCubeArrayUint, int3(1), datau );

    int     b16 = gl.image.AtomicAnd( image2DMSInt,  int2(1), sample, datai );
    uint    b17 = gl.image.AtomicAnd( image2DMSUint, int2(1), sample, datau );

    int     b18 = gl.image.AtomicAnd( image2DMSArrayInt,  int3(1), sample, datai );
    uint    b19 = gl.image.AtomicAnd( image2DMSArrayUint, int3(1), sample, datau );

    // AtomicOr
    int     c0 = gl.image.AtomicOr( image1DInt,  int(1), datai );
    uint    c1 = gl.image.AtomicOr( image1DUint, int(1), datau );

    int     c2 = gl.image.AtomicOr( image2DInt,  int2(1), datai );
    uint    c3 = gl.image.AtomicOr( image2DUint, int2(1), datau );

    int     c4 = gl.image.AtomicOr( image3DInt,  int3(1), datai );
    uint    c5 = gl.image.AtomicOr( image3DUint, int3(1), datau );

    int     c6 = gl.image.AtomicOr( imageCubeInt,  int3(1), datai );
    uint    c7 = gl.image.AtomicOr( imageCubeUint, int3(1), datau );

    int     c8 = gl.image.AtomicOr( imageBufferInt,  int(1), datai );
    uint    c9 = gl.image.AtomicOr( imageBufferUint, int(1), datau );

    int     c10 = gl.image.AtomicOr( image1DArrayInt,  int2(1), datai );
    uint    c11 = gl.image.AtomicOr( image1DArrayUint, int2(1), datau );

    int     c12 = gl.image.AtomicOr( image2DArrayInt,  int3(1), datai );
    uint    c13 = gl.image.AtomicOr( image2DArrayUint, int3(1), datau );

    int     c14 = gl.image.AtomicOr( imageCubeArrayInt,  int3(1), datai );
    uint    c15 = gl.image.AtomicOr( imageCubeArrayUint, int3(1), datau );

    int     c16 = gl.image.AtomicOr( image2DMSInt,  int2(1), sample, datai );
    uint    c17 = gl.image.AtomicOr( image2DMSUint, int2(1), sample, datau );

    int     c18 = gl.image.AtomicOr( image2DMSArrayInt,  int3(1), sample, datai );
    uint    c19 = gl.image.AtomicOr( image2DMSArrayUint, int3(1), sample, datau );

    // AtomicXor
    int     d0 = gl.image.AtomicXor( image1DInt,  int(1), datai );
    uint    d1 = gl.image.AtomicXor( image1DUint, int(1), datau );

    int     d2 = gl.image.AtomicXor( image2DInt,  int2(1), datai );
    uint    d3 = gl.image.AtomicXor( image2DUint, int2(1), datau );

    int     d4 = gl.image.AtomicXor( image3DInt,  int3(1), datai );
    uint    d5 = gl.image.AtomicXor( image3DUint, int3(1), datau );

    int     d6 = gl.image.AtomicXor( imageCubeInt,  int3(1), datai );
    uint    d7 = gl.image.AtomicXor( imageCubeUint, int3(1), datau );

    int     d8 = gl.image.AtomicXor( imageBufferInt,  int(1), datai );
    uint    d9 = gl.image.AtomicXor( imageBufferUint, int(1), datau );

    int     d10 = gl.image.AtomicXor( image1DArrayInt,  int2(1), datai );
    uint    d11 = gl.image.AtomicXor( image1DArrayUint, int2(1), datau );

    int     d12 = gl.image.AtomicXor( image2DArrayInt,  int3(1), datai );
    uint    d13 = gl.image.AtomicXor( image2DArrayUint, int3(1), datau );

    int     d14 = gl.image.AtomicXor( imageCubeArrayInt,  int3(1), datai );
    uint    d15 = gl.image.AtomicXor( imageCubeArrayUint, int3(1), datau );

    int     d16 = gl.image.AtomicXor( image2DMSInt,  int2(1), sample, datai );
    uint    d17 = gl.image.AtomicXor( image2DMSUint, int2(1), sample, datau );

    int     d18 = gl.image.AtomicXor( image2DMSArrayInt,  int3(1), sample, datai );
    uint    d19 = gl.image.AtomicXor( image2DMSArrayUint, int3(1), sample, datau );

    // AtomicMin
    int     e0 = gl.image.AtomicMin( image1DInt,  int(1), datai );
    uint    e1 = gl.image.AtomicMin( image1DUint, int(1), datau );

    int     e2 = gl.image.AtomicMin( image2DInt,  int2(1), datai );
    uint    e3 = gl.image.AtomicMin( image2DUint, int2(1), datau );

    int     e4 = gl.image.AtomicMin( image3DInt,  int3(1), datai );
    uint    e5 = gl.image.AtomicMin( image3DUint, int3(1), datau );

    int     e6 = gl.image.AtomicMin( imageCubeInt,  int3(1), datai );
    uint    e7 = gl.image.AtomicMin( imageCubeUint, int3(1), datau );

    int     e8 = gl.image.AtomicMin( imageBufferInt,  int(1), datai );
    uint    e9 = gl.image.AtomicMin( imageBufferUint, int(1), datau );

    int     e10 = gl.image.AtomicMin( image1DArrayInt,  int2(1), datai );
    uint    e11 = gl.image.AtomicMin( image1DArrayUint, int2(1), datau );

    int     e12 = gl.image.AtomicMin( image2DArrayInt,  int3(1), datai );
    uint    e13 = gl.image.AtomicMin( image2DArrayUint, int3(1), datau );

    int     e14 = gl.image.AtomicMin( imageCubeArrayInt,  int3(1), datai );
    uint    e15 = gl.image.AtomicMin( imageCubeArrayUint, int3(1), datau );

    int     e16 = gl.image.AtomicMin( image2DMSInt,  int2(1), sample, datai );
    uint    e17 = gl.image.AtomicMin( image2DMSUint, int2(1), sample, datau );

    int     e18 = gl.image.AtomicMin( image2DMSArrayInt,  int3(1), sample, datai );
    uint    e19 = gl.image.AtomicMin( image2DMSArrayUint, int3(1), sample, datau );

    // AtomicMax
    int     f0 = gl.image.AtomicMax( image1DInt,  int(1), datai );
    uint    f1 = gl.image.AtomicMax( image1DUint, int(1), datau );

    int     f2 = gl.image.AtomicMax( image2DInt,  int2(1), datai );
    uint    f3 = gl.image.AtomicMax( image2DUint, int2(1), datau );

    int     f4 = gl.image.AtomicMax( image3DInt,  int3(1), datai );
    uint    f5 = gl.image.AtomicMax( image3DUint, int3(1), datau );

    int     f6 = gl.image.AtomicMax( imageCubeInt,  int3(1), datai );
    uint    f7 = gl.image.AtomicMax( imageCubeUint, int3(1), datau );

    int     f8 = gl.image.AtomicMax( imageBufferInt,  int(1), datai );
    uint    f9 = gl.image.AtomicMax( imageBufferUint, int(1), datau );

    int     f10 = gl.image.AtomicMax( image1DArrayInt,  int2(1), datai );
    uint    f11 = gl.image.AtomicMax( image1DArrayUint, int2(1), datau );

    int     f12 = gl.image.AtomicMax( image2DArrayInt,  int3(1), datai );
    uint    f13 = gl.image.AtomicMax( image2DArrayUint, int3(1), datau );

    int     f14 = gl.image.AtomicMax( imageCubeArrayInt,  int3(1), datai );
    uint    f15 = gl.image.AtomicMax( imageCubeArrayUint, int3(1), datau );

    int     f16 = gl.image.AtomicMax( image2DMSInt,  int2(1), sample, datai );
    uint    f17 = gl.image.AtomicMax( image2DMSUint, int2(1), sample, datau );

    int     f18 = gl.image.AtomicMax( image2DMSArrayInt,  int3(1), sample, datai );
    uint    f19 = gl.image.AtomicMax( image2DMSArrayUint, int3(1), sample, datau );

    // AtomicExchange
    int     g0 = gl.image.AtomicExchange( image1DInt,  int(1), datai );
    uint    g1 = gl.image.AtomicExchange( image1DUint, int(1), datau );

    int     g2 = gl.image.AtomicExchange( image2DInt,  int2(1), datai );
    uint    g3 = gl.image.AtomicExchange( image2DUint, int2(1), datau );

    int     g4 = gl.image.AtomicExchange( image3DInt,  int3(1), datai );
    uint    g5 = gl.image.AtomicExchange( image3DUint, int3(1), datau );

    int     g6 = gl.image.AtomicExchange( imageCubeInt,  int3(1), datai );
    uint    g7 = gl.image.AtomicExchange( imageCubeUint, int3(1), datau );

    int     g8 = gl.image.AtomicExchange( imageBufferInt,  int(1), datai );
    uint    g9 = gl.image.AtomicExchange( imageBufferUint, int(1), datau );

    int     g10 = gl.image.AtomicExchange( image1DArrayInt,  int2(1), datai );
    uint    g11 = gl.image.AtomicExchange( image1DArrayUint, int2(1), datau );

    int     g12 = gl.image.AtomicExchange( image2DArrayInt,  int3(1), datai );
    uint    g13 = gl.image.AtomicExchange( image2DArrayUint, int3(1), datau );

    int     g14 = gl.image.AtomicExchange( imageCubeArrayInt,  int3(1), datai );
    uint    g15 = gl.image.AtomicExchange( imageCubeArrayUint, int3(1), datau );

    int     g16 = gl.image.AtomicExchange( image2DMSInt,  int2(1), sample, datai );
    uint    g17 = gl.image.AtomicExchange( image2DMSUint, int2(1), sample, datau );

    int     g18 = gl.image.AtomicExchange( image2DMSArrayInt,  int3(1), sample, datai );
    uint    g19 = gl.image.AtomicExchange( image2DMSArrayUint, int3(1), sample, datau );

    // AtomicCompSwap
    int     h0 = gl.image.AtomicCompSwap( image1DInt,  int(1), comparei, datai );
    uint    h1 = gl.image.AtomicCompSwap( image1DUint, int(1), compareu, datau );

    int     h2 = gl.image.AtomicCompSwap( image2DInt,  int2(1), comparei, datai );
    uint    h3 = gl.image.AtomicCompSwap( image2DUint, int2(1), compareu, datau );

    int     h4 = gl.image.AtomicCompSwap( image3DInt,  int3(1), comparei, datai );
    uint    h5 = gl.image.AtomicCompSwap( image3DUint, int3(1), compareu, datau );

    int     h6 = gl.image.AtomicCompSwap( imageCubeInt,  int3(1), comparei, datai );
    uint    h7 = gl.image.AtomicCompSwap( imageCubeUint, int3(1), compareu, datau );

    int     h8 = gl.image.AtomicCompSwap( imageBufferInt,  int(1), comparei, datai );
    uint    h9 = gl.image.AtomicCompSwap( imageBufferUint, int(1), compareu, datau );

    int     h10 = gl.image.AtomicCompSwap( image1DArrayInt,  int2(1), comparei, datai );
    uint    h11 = gl.image.AtomicCompSwap( image1DArrayUint, int2(1), compareu, datau );

    int     h12 = gl.image.AtomicCompSwap( image2DArrayInt,  int3(1), comparei, datai );
    uint    h13 = gl.image.AtomicCompSwap( image2DArrayUint, int3(1), compareu, datau );

    int     h14 = gl.image.AtomicCompSwap( imageCubeArrayInt,  int3(1), comparei, datai );
    uint    h15 = gl.image.AtomicCompSwap( imageCubeArrayUint, int3(1), compareu, datau );

    int     h16 = gl.image.AtomicCompSwap( image2DMSInt,  int2(1), sample, comparei, datai );
    uint    h17 = gl.image.AtomicCompSwap( image2DMSUint, int2(1), sample, compareu, datau );

    int     h18 = gl.image.AtomicCompSwap( image2DMSArrayInt,  int3(1), sample, comparei, datai );
    uint    h19 = gl.image.AtomicCompSwap( image2DMSArrayUint, int3(1), sample, compareu, datau );

    // AtomicAdd (float)
    float   i0  = gl.image.AtomicAdd( image1DFloat,         int(1),  dataf );
    float   i2  = gl.image.AtomicAdd( image2DFloat,         int2(1), dataf );
    float   i4  = gl.image.AtomicAdd( image3DFloat,         int3(1), dataf );
    float   i6  = gl.image.AtomicAdd( imageCubeFloat,       int3(1), dataf );
    float   i8  = gl.image.AtomicAdd( imageBufferFloat,     int(1),  dataf );
    float   i10 = gl.image.AtomicAdd( image1DArrayFloat,    int2(1), dataf );
    float   i12 = gl.image.AtomicAdd( image2DArrayFloat,    int3(1), dataf );
    float   i14 = gl.image.AtomicAdd( imageCubeArrayFloat,  int3(1), dataf );
    float   i16 = gl.image.AtomicAdd( image2DMSFloat,       int2(1), sample, dataf );
    float   i18 = gl.image.AtomicAdd( image2DMSArrayFloat,  int3(1), sample, dataf );

    // AtomicExchange (float)
    float   j0  = gl.image.AtomicExchange( image1DFloat,        int(1),  dataf );
    float   j2  = gl.image.AtomicExchange( image2DFloat,        int2(1), dataf );
    float   j4  = gl.image.AtomicExchange( image3DFloat,        int3(1), dataf );
    float   j6  = gl.image.AtomicExchange( imageCubeFloat,      int3(1), dataf );
    float   j8  = gl.image.AtomicExchange( imageBufferFloat,    int(1),  dataf );
    float   j10 = gl.image.AtomicExchange( image1DArrayFloat,   int2(1), dataf );
    float   j12 = gl.image.AtomicExchange( image2DArrayFloat,   int3(1), dataf );
    float   j14 = gl.image.AtomicExchange( imageCubeArrayFloat, int3(1), dataf );
    float   j16 = gl.image.AtomicExchange( image2DMSFloat,      int2(1), sample, dataf );
    float   j18 = gl.image.AtomicExchange( image2DMSArrayFloat, int3(1), sample, dataf );

    // AtomicMin (float)
    float   k0  = gl.image.AtomicMin( image1DFloat,         int(1),  dataf );
    float   k2  = gl.image.AtomicMin( image2DFloat,         int2(1), dataf );
    float   k4  = gl.image.AtomicMin( image3DFloat,         int3(1), dataf );
    float   k6  = gl.image.AtomicMin( imageCubeFloat,       int3(1), dataf );
    float   k8  = gl.image.AtomicMin( imageBufferFloat,     int(1),  dataf );
    float   k10 = gl.image.AtomicMin( image1DArrayFloat,    int2(1), dataf );
    float   k12 = gl.image.AtomicMin( image2DArrayFloat,    int3(1), dataf );
    float   k14 = gl.image.AtomicMin( imageCubeArrayFloat,  int3(1), dataf );
    float   k16 = gl.image.AtomicMin( image2DMSFloat,       int2(1), sample, dataf );
    float   k18 = gl.image.AtomicMin( image2DMSArrayFloat,  int3(1), sample, dataf );

    // AtomicMax (float)
    float   l0  = gl.image.AtomicMax( image1DFloat,         int(1),  dataf );
    float   l2  = gl.image.AtomicMax( image2DFloat,         int2(1), dataf );
    float   l4  = gl.image.AtomicMax( image3DFloat,         int3(1), dataf );
    float   l6  = gl.image.AtomicMax( imageCubeFloat,       int3(1), dataf );
    float   l8  = gl.image.AtomicMax( imageBufferFloat,     int(1),  dataf );
    float   l10 = gl.image.AtomicMax( image1DArrayFloat,    int2(1), dataf );
    float   l12 = gl.image.AtomicMax( image2DArrayFloat,    int3(1), dataf );
    float   l14 = gl.image.AtomicMax( imageCubeArrayFloat,  int3(1), dataf );
    float   l16 = gl.image.AtomicMax( image2DMSFloat,       int2(1), sample, dataf );
    float   l18 = gl.image.AtomicMax( image2DMSArrayFloat,  int3(1), sample, dataf );

    // Load
    int4    m0 = gl.image.Load( image1DInt,   int(1) );
    uint4   m1 = gl.image.Load( image1DUint,  int(1) );
    float4  m2 = gl.image.Load( image1DFloat, int(1) );

    int4    m3 = gl.image.Load( image2DInt,   int2(1) );
    uint4   m4 = gl.image.Load( image2DUint,  int2(1) );
    float4  m5 = gl.image.Load( image2DFloat, int2(1) );

    int4    m6 = gl.image.Load( image3DInt,   int3(1) );
    uint4   m7 = gl.image.Load( image3DUint,  int3(1) );
    float4  m8 = gl.image.Load( image3DFloat, int3(1) );

    int4    m9  = gl.image.Load( imageCubeInt,   int3(1) );
    uint4   m10 = gl.image.Load( imageCubeUint,  int3(1) );
    float4  m11 = gl.image.Load( imageCubeFloat, int3(1) );

    int4    m12 = gl.image.Load( imageBufferInt,   int(1) );
    uint4   m13 = gl.image.Load( imageBufferUint,  int(1) );
    float4  m14 = gl.image.Load( imageBufferFloat, int(1) );

    int4    m15 = gl.image.Load( image1DArrayInt,   int2(1) );
    uint4   m16 = gl.image.Load( image1DArrayUint,  int2(1) );
    float4  m17 = gl.image.Load( image1DArrayFloat, int2(1) );

    int4    m18 = gl.image.Load( image2DArrayInt,   int3(1) );
    uint4   m19 = gl.image.Load( image2DArrayUint,  int3(1) );
    float4  m20 = gl.image.Load( image2DArrayFloat, int3(1) );

    int4    m21 = gl.image.Load( imageCubeArrayInt,   int3(1) );
    uint4   m22 = gl.image.Load( imageCubeArrayUint,  int3(1) );
    float4  m23 = gl.image.Load( imageCubeArrayFloat, int3(1) );

    int4    m24 = gl.image.Load( image2DMSInt,   int2(1), sample );
    uint4   m25 = gl.image.Load( image2DMSUint,  int2(1), sample );
    float4  m26 = gl.image.Load( image2DMSFloat, int2(1), sample );

    int4    m27 = gl.image.Load( image2DMSArrayInt,   int3(1), sample );
    uint4   m28 = gl.image.Load( image2DMSArrayUint,  int3(1), sample );
    float4  m29 = gl.image.Load( image2DMSArrayFloat, int3(1), sample );

    // Store
    gl.image.Store( image1DInt,   int(1), data4i );
    gl.image.Store( image1DUint,  int(1), data4u );
    gl.image.Store( image1DFloat, int(1), data4f );

    gl.image.Store( image2DInt,   int2(1), data4i );
    gl.image.Store( image2DUint,  int2(1), data4u );
    gl.image.Store( image2DFloat, int2(1), data4f );

    gl.image.Store( image3DInt,   int3(1), data4i );
    gl.image.Store( image3DUint,  int3(1), data4u );
    gl.image.Store( image3DFloat, int3(1), data4f );

    gl.image.Store( imageCubeInt,   int3(1), data4i );
    gl.image.Store( imageCubeUint,  int3(1), data4u );
    gl.image.Store( imageCubeFloat, int3(1), data4f );

    gl.image.Store( imageBufferInt,   int(1), data4i );
    gl.image.Store( imageBufferUint,  int(1), data4u );
    gl.image.Store( imageBufferFloat, int(1), data4f );

    gl.image.Store( image1DArrayInt,   int2(1), data4i );
    gl.image.Store( image1DArrayUint,  int2(1), data4u );
    gl.image.Store( image1DArrayFloat, int2(1), data4f );

    gl.image.Store( image2DArrayInt,   int3(1), data4i );
    gl.image.Store( image2DArrayUint,  int3(1), data4u );
    gl.image.Store( image2DArrayFloat, int3(1), data4f );

    gl.image.Store( imageCubeArrayInt,   int3(1), data4i );
    gl.image.Store( imageCubeArrayUint,  int3(1), data4u );
    gl.image.Store( imageCubeArrayFloat, int3(1), data4f );

    gl.image.Store( image2DMSInt,   int2(1), sample, data4i );
    gl.image.Store( image2DMSUint,  int2(1), sample, data4u );
    gl.image.Store( image2DMSFloat, int2(1), sample, data4f );

    gl.image.Store( image2DMSArrayInt,   int3(1), sample, data4i );
    gl.image.Store( image2DMSArrayUint,  int3(1), sample, data4u );
    gl.image.Store( image2DMSArrayFloat, int3(1), sample, data4f );
}



#ifdef AE_COMPILER_MSVC
# pragma warning (pop)
#endif
