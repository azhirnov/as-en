// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	for screenshot test

	compare accuracy of cluster to frustum conversion

	related:
	* [view clusters in 3D](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/light-cull/test-ClusterBoundingSphere.as)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<FPVCamera>	camera	= FPVCamera();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 50.f );

			const float	s = 0.f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( camera );
			pass.Output( "out_Color",	rt );
			pass.Slider( "iScale",		0,	8,	6 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Frustum.glsl"
	#include "InvocationID.glsl"


	float2x2  ClusterCoords (const int2 clusterIdx, const int2 clusterCount, const float z)
	{
		// frustum width and height
		float	h	= 2.0 * z * Tan( un_PerPass.camera.fov.y * 0.5 );
		float	w	= h * un_PerPass.resolution.x * un_PerPass.invResolution.y;

		float2	cl_size	= float2(w,h) / float2(clusterCount.xy);

		// bounding rect
		float2	min	= float2(w,h) * -0.5 + float2(clusterIdx.xy) * cl_size;
		float2	max	= min + cl_size;

		return float2x2( min, max );
	}


	// returns {near, far} Z for cluster
	float2  ZProjection (int clusterZ, int clusterCount)
	{
		float	z_near	= un_PerPass.camera.clipPlanes[0];
		float	z_far	= un_PerPass.camera.clipPlanes[1];

		// log view-space
		float	cl_near_z	= z_near * Pow( z_far / z_near,  float(clusterZ) / float(clusterCount) );
		float	cl_far_z	= z_near * Pow( z_far / z_near,  float(clusterZ+1) / float(clusterCount) );
		return	float2( cl_near_z, cl_far_z );
	}


	void  GetClusterCorners (const int3 clusterIdx, const int3 clusterCount, out float3 outPoints[8])
	{
		float2		z_near_far	= ZProjection( clusterIdx.z, clusterCount.z );
		float		cl_near_z	= z_near_far[0];
		float		cl_far_z	= z_near_far[1];
		float		cl_center_z	= (cl_near_z + cl_far_z) * 0.5;

		float2x2	cl_near_xy	= ClusterCoords( clusterIdx.xy, clusterCount.xy, cl_near_z );
		float2x2	cl_far_xy	= ClusterCoords( clusterIdx.xy, clusterCount.xy, cl_far_z );
		float2x2	cl_cen_xy	= ClusterCoords( clusterIdx.xy, clusterCount.xy, cl_center_z );

		//  2 - 3        6 - 7
		//  | / |-near   | \ |-far
		//  0 - 1        4 - 5

		outPoints[0] = float3( cl_near_xy[0].x, cl_near_xy[1].y, cl_near_z );
		outPoints[1] = float3( cl_near_xy[1],					 cl_near_z );
		outPoints[2] = float3( cl_near_xy[0],					 cl_near_z );
		outPoints[3] = float3( cl_near_xy[1].x, cl_near_xy[0].y, cl_near_z );

		outPoints[4] = float3( cl_far_xy[0].x, cl_far_xy[1].y,	cl_far_z );
		outPoints[5] = float3( cl_far_xy[1],					cl_far_z );
		outPoints[6] = float3( cl_far_xy[0],					cl_far_z );
		outPoints[7] = float3( cl_far_xy[1].x, cl_far_xy[0].y,	cl_far_z );
	}


	void  Main ()
	{
		const int3		cluster_count	= int3(4, 4, 8);
		const int		corner_count	= 8;

		const float2	max				= float2( cluster_count.z * corner_count, cluster_count.x * cluster_count.y );
		const int2		coord			= int2(GetGlobalCoordUNorm().xy * max);
		const float		scale			= Exp10( float(iScale) );

		int3			cluster_idx;
		int				corner_idx;

		cluster_idx.x	= coord.y % cluster_count.x;
		cluster_idx.y	= (coord.y / cluster_count.x) / cluster_count.y;
		cluster_idx.z	= coord.x % cluster_count.z;
		corner_idx		= (coord.x / cluster_count.z) % corner_count;

		float3	ref_corners [8];
		GetClusterCorners( cluster_idx, cluster_count, OUT ref_corners );

		float3	test_corners [8];
		{
			Frustum	fr = Frustum_FromCornerPoints( ref_corners );
			Frustum_ToCornerPoints( fr, OUT test_corners );
		}

		out_Color.rgb = (Abs( ref_corners[corner_idx] - test_corners[corner_idx] ) / Abs(ref_corners[corner_idx])) * scale;
		out_Color.a = 1.0;

		if ( Any(IsNaN( out_Color.rgb )) or Any(IsInfinity( out_Color.rgb )))
		{
			out_Color = float4(0.2);
			return;
		}

		if ( AllNotEqual( coord, int2(GetGlobalCoordUNorm(int3(1)).xy * max) ))
		{
			out_Color = float4(0.0);
			return;
		}
	}

#endif
//-----------------------------------------------------------------------------
