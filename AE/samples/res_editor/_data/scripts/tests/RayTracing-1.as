// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw triangles using ray query in compute shader.
	Hit and miss shaders are emulated.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	define AE_ray_query
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<RTScene>	scene	= RTScene();

		// create geometry
		{
			RC<RTGeometry>	geom		= RTGeometry();
			RC<Buffer>		triangles	= Buffer();
			array<float2>	positions	= { float2(0.f, -0.5f), float2(0.5f, 0.5f), float2(-0.5f, 0.5f) };
			array<uint>		indices		= { 0, 1, 2 };

			triangles.FloatArray(	"positions",	positions );
			triangles.UIntArray(	"indices",		indices );

		//	geom.AddTriangles( triangles );
			geom.AddIndexedTriangles( triangles, triangles );

			scene.AddInstance( geom, float3(-0.5f, 0.00f, 0.0f), RTInstanceCustomIndex(0) );
			scene.AddInstance( geom, float3( 0.5f, 0.00f, 0.0f), RTInstanceCustomIndex(1) );
			scene.AddInstance( geom, RTInstanceTransform( float3(0.f), float3(0.f, 0.f, ToRad(180.f)), 1.f ), RTInstanceCustomIndex(2) );
		}

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgOut( "un_OutImage",	rt );
			pass.ArgIn(  "un_RtScene",	scene );
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

	ND_ float4  MissShader ()
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	ND_ float4  HitShader (const float3 barycentric, uint id)
	{
		return float4(barycentric, 1.0f);
	}

	void  Main ()
	{
		const float2	uv			= GetGlobalCoordSNormCorrected() * 1.5f;
		const float3	origin		= float3(uv.x, uv.y, -1.0f);
		const float3	direction	= float3(0.0f, 0.0f, 1.0f);
		gl::RayQuery	ray_query;
		float4			color;

		gl.rayQuery.Initialize( ray_query, un_RtScene, gl::RayFlags::None,
							    /*cullMask*/0xFF, origin, /*Tmin*/0.0f, direction, /*Tmax*/10.0f );

		while ( gl.rayQuery.Proceed( ray_query ))
		{
			if ( GetCandidateIntersectionType( ray_query ) == gl::RayQueryCandidateIntersection::Triangle )
				gl.rayQuery.ConfirmIntersection( ray_query );
		}

		if ( GetCommittedIntersectionType( ray_query ) == gl::RayQueryCommittedIntersection::None )
		{
			color = MissShader();
		}
		else
		{
			float2	attribs		= GetCommittedIntersectionBarycentrics( ray_query );
			float3	barycentric = TriangleHitAttribsToBaricentrics( attribs );
					color		= HitShader( barycentric, GetCommittedIntersectionInstanceCustomIndex( ray_query ));
		}

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, color );
	}

#endif
//-----------------------------------------------------------------------------
