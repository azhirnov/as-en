// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Exact OIT
	Ray tracing from back to front, only front faces.
	Used multiple ray casting.
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

	const float	c_SmallOffset	= 0.0001;
	const uint	c_MaxIterations	= 128;

	ND_ float4  HitShader (const uint instanceId)
	{
		return un_ColorPerInstance.colors[instanceId];
	}

	ND_ bool  CastRay (inout HWRay ray, out float4 outColor)
	{
		if ( ray.tMin >= ray.tMax )
			return false;

		gl::RayQuery	ray_query;
		RayQuery_Init( ray_query, un_RtScene, ray );

		while ( gl.rayQuery.Proceed( ray_query ))
		{
			if ( GetCandidateIntersectionType( ray_query ) == gl::RayQueryCandidateIntersection::Triangle )
				gl.rayQuery.ConfirmIntersection( ray_query );
		}

		if ( GetCommittedIntersectionType( ray_query ) == gl::RayQueryCommittedIntersection::Triangle )
		{
			float2	attribs		= GetCommittedIntersectionBarycentrics( ray_query );
					outColor	= HitShader( GetCommittedIntersectionInstanceCustomIndex( ray_query ));

			// 'GetCommittedIntersectionT()' - distance from 'origin' to intersection point
			ray.tMin = GetCommittedIntersectionT( ray_query ) + c_SmallOffset;
			return true;
		}
		return false;
	}


	void  Main ()
	{
		const Ray	ray		= Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );
		HWRay		hwray	= HWRay_Create( ray, un_PerPass.camera.clipPlanes.y );
		float4		color	= float4(0.0);

		// back to front
		hwray.rayOrigin	= hwray.rayOrigin + hwray.rayDir * hwray.tMax;
		hwray.tMin		= 0.0;
		hwray.rayDir	= -hwray.rayDir;
		hwray.rayFlags	= gl::RayFlags::CullBackFacingTriangles;

		for (uint i = 0; i < c_MaxIterations; ++i)
		{
			float4	src;
			if ( ! CastRay( hwray, OUT src ))
				break;

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
