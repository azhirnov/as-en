// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Exact OIT
	Ray tracing from back to front, front and back faces with optical depth.
	Used single ray cast, all intersections recorded to array and then sorted.
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
			pass.Slider( "iAbsorption",			1.0f,	5.f,	2.5f );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "Sort.glsl"
	#include "Blend.glsl"
	#include "InvocationID.glsl"
	#include "HWRayTracing.glsl"
	#include "ColorSpace.glsl"

	const float	c_SmallOffset			= 0.0001;
	const uint	c_MaxIntersections		= 64;
	uint		g_IntersectionCount		= 0;
	uint		g_IntersectionData		[c_MaxIntersections];	// 0..30 - instanceID, 31 - frontFace
	float		g_IntersectionDistance	[c_MaxIntersections];


	ND_ uint  UnpackInstanceID (uint data)		{ return data & 0xFFFF; }
	ND_ bool  IsFrontFace (uint data)			{ return (data >> 31) == 1; }

	ND_ uint  Pack (uint instId, bool front)	{ return (uint(front) << 31) | (instId & 0xFFFF); }


	// back to front
	#define SortCmp( i, j )\
		g_IntersectionDistance[i] < g_IntersectionDistance[j]

	#define SortSwap( i, j )										\
	{																\
		const float		a			= g_IntersectionDistance[i];	\
		const uint		b			= g_IntersectionData[i];		\
																	\
		g_IntersectionDistance[i]	= g_IntersectionDistance[j];	\
		g_IntersectionData[i]		= g_IntersectionData[j];		\
																	\
		g_IntersectionDistance[j]	= a;							\
		g_IntersectionData[j]		= b;							\
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
				const uint	i				= g_IntersectionCount++;
				uint		inst_id			= GetCandidateIntersectionInstanceCustomIndex( ray_query );
				bool		front_face		= GetCandidateIntersectionFrontFace( ray_query );

				g_IntersectionData[i]		= Pack( inst_id, front_face );
				g_IntersectionDistance[i]	= GetCandidateIntersectionT( ray_query );
			}
		}

		BubbleSort( g_IntersectionCount, SortCmp, SortSwap );
	}


	void  Main ()
	{
		const Ray	ray		= Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );
		HWRay		hwray	= HWRay_Create( ray, un_PerPass.camera.clipPlanes.y );
		float4		color	= float4(0.0);

		CastRay( hwray );

		// back to front
		if ( g_IntersectionCount > 0 )
		{
			float	prev_dist	= g_IntersectionDistance[0];
			uint	prev_inst	= UnpackInstanceID( g_IntersectionData[0] );
			int		face_count	= IsFrontFace( g_IntersectionData[0] ) ? -1 : 1;

			for (uint i = 1; i < c_MaxIntersections; ++i)
			{
				if ( i >= g_IntersectionCount )
					break;

				float	dist	= g_IntersectionDistance[i];
				uint	inst	= UnpackInstanceID( g_IntersectionData[i] );

				// positive if inside volume
				if ( face_count > 0 )
				{
					float4	src		= HitShader( inst );
					float	depth	= (prev_dist - dist) * iAbsorption;

					if ( inst != prev_inst )
						src = Lerp( src, HitShader( prev_inst ), 0.5 );

					src.a *= Saturate( depth );

					SeparateBlendParams		p;
					p.srcColor		= RemoveSRGBCurve( src );	// from shader
					p.dstColor		= color;					// from render target
					p.srcBlendRGB	= EBlendFactor_SrcAlpha;
					p.srcBlendA		= EBlendFactor_One;
					p.dstBlendRGB	= EBlendFactor_OneMinusSrcAlpha;
					p.dstBlendA		= EBlendFactor_OneMinusSrcAlpha;
					p.blendOpRGB	= EBlendOp_Add;
					p.blendOpA		= EBlendOp_Add;

					color = BlendFn( p );
				}

				prev_dist	= dist;
				prev_inst	= inst;
				face_count	+= IsFrontFace( g_IntersectionData[i] ) ? -1 : 1;
			}
		}

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, ApplySRGBCurve( color ));
	}

#endif
//-----------------------------------------------------------------------------
