// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
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
		const uint			dim				= 2<<10;
		const uint			iter_cnt		= 128;
		RC<DynamicDim>		dyn_dim			= DynamicDim( uint3(dim, dim, 1) );

		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, dyn_dim );	rt.Name( "RT-Color" );
		RC<FPVCamera>		camera			= FPVCamera();
		RC<Buffer>			sphere			= Buffer();
		RC<Buffer>			color_per_inst	= Buffer();
		RC<RTGeometry>		geom			= RTGeometry();
		RC<RTScene>			scene			= RTScene();
		RC<DynamicUInt>		repeat			= DynamicUInt();
		RC<DynamicFloat>	ops				= DynamicFloat( float(dim * dim) * float(iter_cnt) * 1.0e-9 );
		RC<DynamicFloat>	time			= DynamicFloat();
		RC<DynamicFloat>	grays_per_sec	= ops.Div( time );

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 15.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Position( float3(0.f, 0.f, -4.2f));
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
			array<float2x4>	draw_tasks = GenColoredSpheresDrawTasks( int3(16) );
			for (uint i = 0; i < draw_tasks.size(); ++i)
			{
				float2x4 task = draw_tasks[i];
				colors.push_back( task.col1 );
				scene.AddInstance( geom, RTInstanceTransform( float3(task.col0), float3(0.f), task.col0.w ), RTInstanceCustomIndex(i) );
			}
			color_per_inst.FloatArray( "colors", colors );
		}

		Slider( repeat, "Repeat",	1,	32,		1 );

		Label(  grays_per_sec,	"GigaRays/s" );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "MAX_ITER=" + iter_cnt );
			pass.Set(	 camera );
			pass.ArgOut( "un_OutImage",			rt );
			pass.ArgIn(  "un_RtScene",			scene );
			pass.ArgIn(  "un_Geometry",			sphere );
			pass.ArgIn(  "un_ColorPerInstance",	color_per_inst );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
			pass.Repeat( repeat );
			pass.MeasureTime( time );
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

		// front to back
		hwray.rayFlags	= gl::RayFlags::CullBackFacingTriangles;

		for (uint i = 0; i < MAX_ITER; ++i)
		{
			float4	src;
			if ( ! CastRay( hwray, OUT src ))
				break;

			SeparateBlendParams		p;
			p.srcColor		= src * src.a;	// from shader
			p.dstColor		= color;		// from render target
			p.srcBlendRGB	= EBlendFactor_DstAlpha;
			p.srcBlendA		= EBlendFactor_One;
			p.dstBlendRGB	= EBlendFactor_One;
			p.dstBlendA		= EBlendFactor_SrcAlpha;
			p.blendOpRGB	= EBlendOp_Add;
			p.blendOpA		= EBlendOp_Add;

			color = BlendFn( p );
		}

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, color );
	}

#endif
//-----------------------------------------------------------------------------
