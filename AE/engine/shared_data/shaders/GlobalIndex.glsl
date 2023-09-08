// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Helper functions for compute shader
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

//-----------------------------------------------------------------------------
// local (inside workgroup)

// local linear index
ND_ int    GetLocalIndexSize ();
ND_ int    GetLocalIndex ();        // 0..size-1
ND_ float  GetLocalIndexUNorm ();   //  0..1
ND_ float  GetLocalIndexSNorm ();   // -1..1

// local coordinate in 3D
ND_ int3    GetLocalSize ();
ND_ int3    GetLocalCoord ();       // 0..size-1
ND_ float3  GetLocalCoordUNorm ();  //  0..1
ND_ float3  GetLocalCoordSNorm ();  // -1..1


//-----------------------------------------------------------------------------
// group

// group linear index
ND_ int    GetGroupIndexSize ();
ND_ int    GetGroupIndex ();        // 0..size-1
ND_ float  GetGroupIndexUNorm ();   //  0..1
ND_ float  GetGroupIndexSNorm ();   // -1..1

// group coordinate in 3D
ND_ int3    GetGroupSize ();
ND_ int3    GetGroupCoord ();       // 0..size-1
ND_ float3  GetGroupCoordUNorm ();  //  0..1
ND_ float3  GetGroupCoordSNorm ();  // -1..1


//-----------------------------------------------------------------------------
// global (local + group)

// global linear index
ND_ int    GetGlobalIndexSize ();
ND_ int    GetGlobalIndex ();       // 0..size-1
ND_ float  GetGlobalIndexUNorm ();  //  0..1
ND_ float  GetGlobalIndexSNorm ();  // -1..1

// global coordinate in 3D
ND_ int3    GetGlobalSize ();
ND_ int3    GetGlobalCoord ();      // 0..size-1
ND_ float3  GetGlobalCoordUNorm (); //  0..1
ND_ float3  GetGlobalCoordSNorm (); // -1..1

// global normalized coordinate in 2D with same aspect ratio
ND_ float2  GetGlobalCoordUNormCorrected ();        //  0..1
ND_ float2  GetGlobalCoordSNormCorrected ();        // -1..1
ND_ float2  GetGlobalCoordSNormCorrected2 ();       // -X..X,   X may be > 1

//-----------------------------------------------------------------------------


// map pixels to unorm coords with correct aspect ratio.
ND_ float2  MapPixCoordToUNormCorrected (const float2 posPx, const float2 sizePx);
ND_ float3  MapPixCoordToUNormCorrected (const float3 posPx, const float3 sizePx);

// map pixels to snorm coords with correct aspect ratio.
ND_ float2  MapPixCoordToSNormCorrected (const float2 posPx, const float2 sizePx);
ND_ float3  MapPixCoordToSNormCorrected (const float3 posPx, const float3 sizePx);
ND_ float2  MapPixCoordToSNormCorrected2 (const float2 posPx, const float2 sizePx);

// map pixels to unorm coords with correct aspect ratio.
ND_ float2  MapPixCoordToUNormCorrected (const float2 srcPosPx, const float2 srcSizePx, const float2 dstSizePx);
//-----------------------------------------------------------------------------



float2  MapPixCoordToUNormCorrected (const float2 posPx, const float2 sizePx)
{
    return (posPx+0.5f) / Max( sizePx.x, sizePx.y );
}

float3  MapPixCoordToUNormCorrected (const float3 posPx, const float3 sizePx)
{
    return (posPx+0.5f) / Max( sizePx.x, sizePx.y );
}


float2  MapPixCoordToSNormCorrected (const float2 posPx, const float2 sizePx)
{
    const float2    hsize = sizePx * 0.5f;
    return (posPx - hsize) / Max( hsize.x, hsize.y );
}

float3  MapPixCoordToSNormCorrected (const float3 posPx, const float3 sizePx)
{
    const float3    hsize = sizePx * 0.5f;
    return (posPx - hsize) / Max( hsize.x, hsize.y );
}

float2  MapPixCoordToSNormCorrected2 (const float2 posPx, const float2 sizePx)
{
    const float2    hsize = sizePx * 0.5f;
    return (posPx - hsize) / Min( hsize.x, hsize.y );
}


float2  MapPixCoordToUNormCorrected (const float2 srcPosPx, const float2 srcSizePx, const float2 dstSizePx)
{
    const float2    snorm       = ToSNorm( srcPosPx / srcSizePx );
    const float     src_aspect  = srcSizePx.x / srcSizePx.y;
    const float     dst_aspect  = dstSizePx.x / dstSizePx.y;
    const float     scale1      = Max( src_aspect, dst_aspect ) / dst_aspect;
    const float     scale2      = Min( src_aspect, dst_aspect ) / dst_aspect;
    const float2    scale       = src_aspect >= dst_aspect ? float2(scale1, 1.0f) : float2(1.0f, 1.0f/scale2);
    return ToUNorm( snorm * scale );
}
//-----------------------------------------------------------------------------



#ifdef SH_FRAG

// global coordinate in 3D
int3  GetGlobalCoord ()
{
    return int3( gl.FragCoord.xy, gl.Layer );
}

// implement GetGlobalSize() with 'un_PerPass.screenSize'

#endif // SH_FRAG
//-----------------------------------------------------------------------------



#if defined(SH_COMPUTE) or defined(SH_MESH_TASK) or defined(SH_MESH)

// local linear index
int  GetLocalIndex ()
{
    return int( gl.LocalInvocationIndex );
}


// global coordinate in 3D
int3  GetGlobalCoord ()
{
    return int3( gl.GlobalInvocationID );
}

int3  GetGlobalSize ()
{
    return GetGroupSize() * GetLocalSize();
}


// local coordinate in 3D
int3  GetLocalCoord ()
{
    return int3( gl.LocalInvocationID );
}

int3  GetLocalSize ()
{
    return int3( gl.WorkGroupSize );
}


// group coordinate in 3D
int3  GetGroupCoord ()
{
    return int3( gl.WorkGroupID );
}

int3  GetGroupSize ()
{
    return int3( gl.NumWorkGroups );
}

#endif // SH_COMPUTE or SH_MESH_TASK or SH_MESH
//-----------------------------------------------------------------------------



#if defined(SH_RAY_GEN) or defined(SH_RAY_AHIT) or defined(SH_RAY_CHIT) or defined(SH_RAY_INT) or defined(SH_RAY_MISS) or defined(SH_RAY_CALL)

// local linear index
int  GetLocalIndex ()
{
    return int( gl.LaunchID.x +
                (gl.LaunchID.y * gl.LaunchSize.x) +
                (gl.LaunchID.z * gl.LaunchSize.x * gl.LaunchSize.y) );
}

// global coordinate in 3D
int3  GetGlobalCoord ()
{
    return int3( gl.LaunchID );
}

int3  GetGlobalSize ()
{
    return int3( gl.LaunchSize );
}

// local coordinate in 3D
int3  GetLocalCoord ()
{
    return int3( gl.LaunchID );
}

int3  GetLocalSize ()
{
    return int3( gl.LaunchSize );
}

// group coordinate in 3D
int3  GetGroupCoord ()
{
    return int3(0);
}

int3  GetGroupSize ()
{
    return int3(1);
}

#endif
//-----------------------------------------------------------------------------



// global linear index
int  GetGlobalIndex ()
{
    int3 coord = GetGlobalCoord();
    int3 size  = GetGlobalSize();
    return coord.x + (coord.y * size.x) + (coord.z * size.x * size.y);
}

int  GetGlobalIndexSize ()
{
    int3 size  = GetGlobalSize();
    return size.x * size.y * size.z;
}

float  GetGlobalIndexUNorm ()
{
    return (float(GetGlobalIndex())+0.5f) / float(GetGlobalIndexSize());
}

float  GetGlobalIndexSNorm ()
{
    return ToSNorm( GetGlobalIndexUNorm() );
}


// local linear index
int  GetLocalIndexSize ()
{
    int3 size  = GetLocalSize();
    return size.x * size.y * size.z;
}

float  GetLocalIndexUNorm ()
{
    return (float(GetLocalIndex())+0.5f) / float(GetLocalIndexSize());
}

float  GetLocalIndexSNorm ()
{
    return ToSNorm( GetLocalIndexUNorm() );
}


// group linear index
int  GetGroupIndex ()
{
    int3 coord = GetGroupCoord();
    int3 size  = GetGroupSize();
    return coord.x + (coord.y * size.x) + (coord.z * size.x * size.y);
}

int  GetGroupIndexSize ()
{
    int3 size  = GetGroupSize();
    return size.x * size.y * size.z;
}

float  GetGroupIndexUNorm ()
{
    return (float(GetGroupIndex())+0.5f) / float(GetGroupIndexSize());
}

float  GetGroupIndexSNorm ()
{
    return ToSNorm( GetGroupIndexUNorm() );
}


// global coordinate in 3D
float3  GetGlobalCoordUNorm ()
{
    return (float3(GetGlobalCoord())+0.5f) / float3(GetGlobalSize());
}

float3  GetGlobalCoordSNorm ()
{
    return ToSNorm( GetGlobalCoordUNorm() );
}


// local coordinate in 3D
float3  GetLocalCoordUNorm ()
{
    return (float3(GetLocalCoord())+0.5f) / float3(GetLocalSize());
}

float3  GetLocalCoordSNorm ()
{
    return ToSNorm( GetLocalCoordUNorm() );
}


// group coordinate in 3D
float3  GetGroupCoordUNorm ()
{
    return (float3(GetGroupCoord())+0.5f) / float3(GetGroupSize());
}

float3  GetGroupCoordSNorm ()
{
    return ToSNorm( GetGroupCoordUNorm() );
}


// global normalized coordinate in 2D with same aspect ratio
float2  GetGlobalCoordUNormCorrected ()
{
    return MapPixCoordToUNormCorrected( float2(GetGlobalCoord().xy), float2(GetGlobalSize().xy) );
}

float2  GetGlobalCoordSNormCorrected ()
{
    return MapPixCoordToSNormCorrected( float2(GetGlobalCoord().xy), float2(GetGlobalSize().xy) );
}

float2  GetGlobalCoordSNormCorrected2 ()
{
    return MapPixCoordToSNormCorrected2( float2(GetGlobalCoord().xy), float2(GetGlobalSize().xy) );
}
