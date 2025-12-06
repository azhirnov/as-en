// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Exact OIT
	Ray tracing from back to front, only front faces.
	Used single ray cast, all intersections recorded to array in any-hit shader and then sorted.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	define AE_ray_query
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT
	#include "samples/GenColoredSpheres.as"

	void ASmain ()
	{
		// initialize
		RC<Image>		rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<FPVCamera>	camera			= FPVCamera();
		RC<Buffer>		sphere			= Buffer();
		RC<Buffer>		color_per_inst	= Buffer();
		RC<RTGeometry>	geom			= RTGeometry();
		RC<RTScene>		scene			= RTScene();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 15.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position( float3(0.f, 0.f, -3.f));
		}

		// create sphere
		{
			array<float3>	positions;
			array<uint>		indices;
			GetSphere( 3, OUT positions, OUT indices );

			sphere.FloatArray( "positions",	positions );
			sphere.UIntArray(  "indices",	indices );

			geom.AddIndexedTriangles( sphere, sphere );
		}

		// setup draw tasks
		{
			array<float4>	colors;
			array<float2x4>	draw_tasks = GenColoredSpheresDrawTasks();
			for (uint i = 0; i < draw_tasks.size(); ++i)
			{
				float2x4 task = draw_tasks[i];
				colors.push_back( task.col1 );
				scene.AddInstance( geom, RTInstanceTransform( float3(task.col0), float3(0.f), task.col0.w ), RTInstanceCustomIndex(i) );
			}
			color_per_inst.FloatArray( "colors", colors );
		}

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.Set(	 camera );
			pass.ArgOut( "un_OutImage",			rt );
			pass.ArgIn(  "un_RtScene",			scene );
			pass.ArgIn(  "un_Geometry",			sphere );
			pass.ArgIn(  "un_ColorPerInstance",	color_per_inst );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"
	#include "HWRayTracing.glsl"
	#include "Blend.glsl"
	#include "Sort.glsl"

	const float	c_SmallOffset			= 0.0001;
	const uint	c_MaxIntersections		= 64;
	uint		g_IntersectionCount		= 0;
	uint		g_IntersectionInstances	[c_MaxIntersections];	// TODO: ubyte/ushort/mediump
	float		g_IntersectionDistance	[c_MaxIntersections];

	// back to front
	#define SortCmp( i, j )\
		g_IntersectionDistance[i] < g_IntersectionDistance[j]

	#define SortSwap( i, j )\
	{																\
		float	a	= g_IntersectionDistance[i];					\
		uint	b	= g_IntersectionInstances[i];					\
		g_IntersectionDistance[i]	= g_IntersectionDistance[j];	\
		g_IntersectionInstances[i]	= g_IntersectionInstances[j];	\
		g_IntersectionDistance[j]	= a;							\
		g_IntersectionInstances[j]	= b;							\
	}


	ND_ float4  HitShader (const uint instanceId)
	{
		return un_ColorPerInstance.colors[instanceId];
	}

	void  CastRay (const HWRay ray)
	{
		gl::RayQuery	ray_query;
		RayQuery_Init( ray_query, un_RtScene, ray );

		while ( gl.rayQuery.Proceed( ray_query ) and (g_IntersectionCount < c_MaxIntersections) )
		{
			if ( GetCandidateIntersectionType( ray_query ) == gl::RayQueryCandidateIntersection::Triangle )
			{
				const uint	i = g_IntersectionCount++;

				g_IntersectionInstances[i]	= GetCandidateIntersectionInstanceCustomIndex( ray_query );
				g_IntersectionDistance[i]	= GetCandidateIntersectionT( ray_query ) / ray.tMax;
			}
		}

		BubbleSort( g_IntersectionCount, SortCmp, SortSwap );
	}


	void  Main ()
	{
		const Ray	ray		= Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );
		HWRay		hwray	= HWRay_Create( ray, un_PerPass.camera.clipPlanes.y );
		float4		color	= float4(0.0);

		hwray.rayFlags	= gl::RayFlags::CullBackFacingTriangles;

		CastRay( hwray );

		// back to front
		for (uint i = 0; i < c_MaxIntersections; ++i)
		{
			if ( i >= g_IntersectionCount )
				break;

			float4	src = HitShader( g_IntersectionInstances[i] );

			SeparateBlendParams		p;
			p.srcColor		= src;		// from shader
			p.dstColor		= color;	// from render target
			p.srcBlendRGB	= EBlendFactor_SrcAlpha;
			p.srcBlendA		= EBlendFactor_One;
			p.dstBlendRGB	= EBlendFactor_OneMinusSrcAlpha;
			p.dstBlendA		= EBlendFactor_OneMinusSrcAlpha;
			p.blendOpRGB	= EBlendOp_Add;
			p.blendOpA		= EBlendOp_Add;

			color = BlendFn( p );
		}

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, color );
	}

#endif
//-----------------------------------------------------------------------------
