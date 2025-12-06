// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Helper functions to get workgroup coords in compute shader and
	global coord for fragment/compute/ray_tracing/mesh/task shaders.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

//-----------------------------------------------------------------------------
// local (inside workgroup)

// local linear index
ND_ int    GetLocalIndexSize ();
ND_ int    GetLocalIndex ();						// 0..size-1
ND_ float  GetLocalIndexUNorm ();					//  0..1
ND_ float  GetLocalIndexSNorm ();					// -1..1

// local coordinate in 3D
ND_ int3    GetLocalSize ();
ND_ int3    GetLocalCoord ();						// 0..size-1
ND_ float3  GetLocalCoordUNorm ();					//  0..1
ND_ float3  GetLocalCoordSNorm ();					// -1..1
ND_ float3  GetLocalCoordUNorm (int3 offset);		//  0..1
ND_ float3  GetLocalCoordSNorm (int3 offset);		// -1..1

ND_ int2    GetLocalCoordQuadCorrected ();			// 0..size-1	- for quad subgroup operations

//-----------------------------------------------------------------------------
// group

// group linear index
ND_ int    GetGroupIndexSize ();
ND_ int    GetGroupIndex ();						// 0..size-1
ND_ float  GetGroupIndexUNorm ();					//  0..1
ND_ float  GetGroupIndexSNorm ();					// -1..1

// group coordinate in 3D
ND_ int3    GetGroupSize ();
ND_ int3    GetGroupCoord ();						// 0..size-1
ND_ float3  GetGroupCoordUNorm ();					//  0..1
ND_ float3  GetGroupCoordSNorm ();					// -1..1
ND_ float3  GetGroupCoordUNorm (int3 offset);		//  0..1
ND_ float3  GetGroupCoordSNorm (int3 offset);		// -1..1


//-----------------------------------------------------------------------------
// global (local + group)

// global linear index
ND_ int    GetGlobalIndexSize ();
ND_ int    GetGlobalIndex ();						// 0..size-1
ND_ float  GetGlobalIndexUNorm ();					//  0..1
ND_ float  GetGlobalIndexSNorm ();					// -1..1

// global coordinate in 3D
ND_ int3    GetGlobalSize ();
ND_ int3    GetGlobalCoord ();						// 0..size-1
ND_ int3    GetGlobalCoordSI ();					// -size/2 .. +size/2-1
ND_ float3  GetGlobalCoordUNorm ();					//  0..1
ND_ float3  GetGlobalCoordSNorm ();					// -1..1
ND_ float3  GetGlobalCoordUNorm (int3 offset);		//  0..1
ND_ float3  GetGlobalCoordSNorm (int3 offset);		// -1..1
ND_ float3  GetGlobalCoordSF ();					// -size/2 .. +size/2
ND_ float3  GetGlobalCoordUF ();					// 0..size-1
ND_ float3  GetGlobalSizeRcp ();					// 1/size

// global normalized coordinate in 2D with same aspect ratio
ND_ float2  GetGlobalCoordUNormCorrected ();		//  0..1
ND_ float2  GetGlobalCoordSNormCorrected ();		// -1..1
ND_ float2  GetGlobalCoordSNormCorrected2 ();		// -X..X,	X may be > 1

ND_ int3    GetGlobalCoordQuadCorrected ();			// 0..size-1	- for quad subgroup operations

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
ND_ float2  MapPixCoordToUNormCorrected (const float2 srcPosPx, const float2 srcSizePx, const float2 dstSizePx, const float snormScale);
//-----------------------------------------------------------------------------



float2  MapPixCoordToUNormCorrected (const float2 posPx, const float2 sizePx)
{
	return Saturate( (posPx+0.5f) / Max( sizePx.x, sizePx.y ));
}

float3  MapPixCoordToUNormCorrected (const float3 posPx, const float3 sizePx)
{
	return Saturate( (posPx+0.5f) / Max( sizePx.x, sizePx.y ));
}


float2  MapPixCoordToSNormCorrected (const float2 posPx, const float2 sizePx)
{
	const float2	hsize = sizePx * 0.5f;
	return Clamp( (posPx - hsize) / Max( hsize.x, hsize.y ), -1.0, 1.0 );
}

float3  MapPixCoordToSNormCorrected (const float3 posPx, const float3 sizePx)
{
	const float3	hsize = sizePx * 0.5f;
	return Clamp( (posPx - hsize) / Max( hsize.x, hsize.y ), -1.0, 1.0 );
}

float2  MapPixCoordToSNormCorrected2 (const float2 posPx, const float2 sizePx)
{
	const float2	hsize = sizePx * 0.5f;
	return (posPx - hsize) / Min( hsize.x, hsize.y );
}


float2  MapPixCoordToUNormCorrected (const float2 srcPosPx, const float2 srcSizePx, const float2 dstSizePx)
{
	return MapPixCoordToUNormCorrected( srcPosPx, srcSizePx, dstSizePx, 1.f );
}

float2  MapPixCoordToUNormCorrected (const float2 srcPosPx, const float2 srcSizePx, const float2 dstSizePx, const float snormScale)
{
	const float2	snorm		= ToSNorm( srcPosPx / srcSizePx );
	const float		src_ratio	= srcSizePx.x / srcSizePx.y;
	const float		dst_ratio	= dstSizePx.x / dstSizePx.y;
	const float		scale1		= Max( src_ratio, dst_ratio ) / dst_ratio;
	const float		scale2		= Min( src_ratio, dst_ratio ) / dst_ratio;
	const float2	scale		= src_ratio >= dst_ratio ? float2(scale1, 1.0f) : float2(1.0f, 1.0f/scale2);
	return ToUNorm( snorm * scale * snormScale );
}
//-----------------------------------------------------------------------------



#ifdef SH_FRAG

// global coordinate in 3D
int3  GetGlobalCoord ()
{
  #if 0 //def AE_GEOMETRY_SHADER
	return int3( gl.FragCoord.xy, gl.Layer );	// error on Adreno
  #else
	return int3( gl.FragCoord.xy, 0 );
  #endif
}

float3  GetGlobalCoordUF ()
{
  #if 0 //def AE_GEOMETRY_SHADER
	return float3( Floor(gl.FragCoord.xy), gl.Layer );	// error on Adreno
  #else
	return float3( Floor(gl.FragCoord.xy), 0 );
  #endif
}

int3  GetGlobalCoordQuadCorrected ()
{
	return GetGlobalCoord();
}

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


#if defined(SH_COMPUTE) and defined(AE_shader_subgroup_basic)
	// correct index order to make quad as in FS
	// TODO: tested only on 8x8

	int2  GetLocalCoordQuadCorrected ()
	{
		int	idx = int(gl.subgroup.Index + gl.subgroup.Size * gl.subgroup.GroupIndex) / 2;
		return int2( int(idx / gl.WorkGroupSize.x) * 2 + int(gl.subgroup.Index & 1),
					 int(idx % gl.WorkGroupSize.x) );
	}

	int3  GetGlobalCoordQuadCorrected ()
	{
		int2	pos	 = int2(gl.WorkGroupID.xy * gl.WorkGroupSize.xy);
				pos += GetLocalCoordQuadCorrected();
		return int3( pos, gl.GlobalInvocationID.z );
	}
#endif
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



#if defined(SH_COMPUTE) or defined(SH_MESH_TASK) or defined(SH_MESH) or \
	defined(SH_RAY_GEN) or defined(SH_RAY_AHIT) or defined(SH_RAY_CHIT) or defined(SH_RAY_INT) or defined(SH_RAY_MISS) or defined(SH_RAY_CALL)

float3  GetGlobalCoordUF ()
{
	return float3(GetGlobalCoord());
}

float3  GetGlobalSizeRcp ()
{
	return 1.0 / float3(GetGlobalSize());
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
	return (float3(GetGlobalCoord())+0.5f) * GetGlobalSizeRcp();
}

float3  GetGlobalCoordSNorm ()
{
	return ToSNorm( GetGlobalCoordUNorm() );
}

float3  GetGlobalCoordUNorm (int3 offset)
{
	return (float3(GetGlobalCoord()+offset)+0.5f) * GetGlobalSizeRcp();
}

float3  GetGlobalCoordSNorm (int3 offset)
{
	return ToSNorm( GetGlobalCoordUNorm( offset ));
}

int3  GetGlobalCoordSI ()
{
	return GetGlobalCoord() - GetGlobalSize()/2;
}

float3  GetGlobalCoordSF ()
{
	return float3(GetGlobalCoord()) - float3(GetGlobalSize()-1) * 0.5f;
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

float3  GetLocalCoordUNorm (int3 offset)
{
	return (float3(GetLocalCoord()+offset)+0.5f) / float3(GetLocalSize());
}

float3  GetLocalCoordSNorm (int3 offset)
{
	return ToSNorm( GetLocalCoordUNorm( offset ));
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

float3  GetGroupCoordUNorm (int3 offset)
{
	return (float3(GetGroupCoord()+offset)+0.5f) / float3(GetGroupSize());
}

float3  GetGroupCoordSNorm (int3 offset)
{
	return ToSNorm( GetGroupCoordUNorm( offset ));
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
