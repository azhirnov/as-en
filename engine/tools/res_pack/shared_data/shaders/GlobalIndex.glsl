// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Helper functions for compute shader
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


// global linear index
ND_ int    GetGlobalIndexSize ();
ND_ int    GetGlobalIndex ();       // 0..size-1
ND_ float  GetGlobalIndexUNorm ();  //  0..1
ND_ float  GetGlobalIndexSNorm ();  // -1..1

// local linear index
ND_ int    GetLocalIndexSize ();
ND_ int    GetLocalIndex ();        // 0..size-1
ND_ float  GetLocalIndexUNorm ();   //  0..1
ND_ float  GetLocalIndexSNorm ();   // -1..1

// group linear index
ND_ int    GetGroupIndexSize ();
ND_ int    GetGroupIndex ();        // 0..size-1
ND_ float  GetGroupIndexUNorm ();   //  0..1
ND_ float  GetGroupIndexSNorm ();   // -1..1

// global coordinate in 3D
ND_ int3    GetGlobalSize ();
ND_ int3    GetGlobalCoord ();      // 0..size-1
ND_ float3  GetGlobalCoordUNorm (); //  0..1
ND_ float3  GetGlobalCoordSNorm (); // -1..1

// local coordinate in 3D
ND_ int3    GetLocalSize ();
ND_ int3    GetLocalCoord ();       // 0..size-1
ND_ float3  GetLocalCoordUNorm ();  //  0..1
ND_ float3  GetLocalCoordSNorm ();  // -1..1

// group coordinate in 3D
ND_ int3    GetGroupSize ();
ND_ int3    GetGroupCoord ();       // 0..size-1
ND_ float3  GetGroupCoordUNorm ();  //  0..1
ND_ float3  GetGroupCoordSNorm ();  // -1..1

// global normalized coordinate in 2D with same aspect ratio
ND_ float2  GetGlobalCoordUNormCorrected ();    //  0..1
ND_ float2  GetGlobalCoordSNormCorrected ();    // -1..1
//-----------------------------------------------------------------------------



#ifdef SH_FRAG

// global coordinate in 3D
int3  GetGlobalCoord ()
{
    return int3( gl.FragCoord.xy, gl.Layer );
}

// implement GetGlobalSize() with 'ub.screenSize'

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
    float2  size = float2(GetGlobalSize().xy);
    return (float2(GetGlobalCoord().xy)+0.5f) / Max( size.x, size.y );
}

float2  GetGlobalCoordSNormCorrected ()
{
    float2  hsize   = float2(GetGlobalSize().xy) * 0.5f;
    float   msize   = Max( hsize.x, hsize.y );
    return (float2(GetGlobalCoord().xy) - hsize) / msize;
}
