// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor>
#	define AE_RAY_QUERY
#	include <aestyle.glsl.h>
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
			RC<RTGeometry>	geom		= RTGeometry();;
			RC<Buffer>		triangles	= Buffer();
			array<float2>	positions	= { float2(0.25f, 0.25f), float2(0.75f, 0.25f), float2(0.50f, 0.75f) };
			array<uint>		indices		= { 0, 1, 2 };

			triangles.Float2Array(	"positions",	positions );
			triangles.UInt1Array(	"indices",		indices );
			
			//geom.AddTriangles( triangles );
			geom.AddIndexedTriangles( triangles, triangles );

			scene.AddInstance( geom );
		}
		
		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", EPassFlags::Enable_ShaderTrace);
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

	#include "GlobalIndex.glsl"

	ND_ float4  MissShader ()
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	ND_ float4  HitShader (const float3 barycentrics)
	{
		return float4(barycentrics, 1.0f);
	}

	void  Main ()
	{
		const float2	uv			= GetGlobalCoordUNorm().xy;
		const float3	origin		= float3(uv.x, 1.0f - uv.y, -1.0f);
		const float3	direction	= float3(0.0f, 0.0f, 1.0f);
		gl::RayQuery	ray_query;
		float4			color;
		
		gl.rayQuery.Initialize( ray_query, un_RtScene, gl::RayFlags::None,
							    /*cullMask*/0xFF, origin, /*Tmin*/0.0f, direction, /*Tmax*/10.0f );
	
		while ( gl.rayQuery.Proceed( ray_query ))
		{
			if ( gl::RayQueryCandidateIntersection(gl.rayQuery.GetIntersectionType( ray_query, false )) == gl::RayQueryCandidateIntersection::Triangle )
				gl.rayQuery.ConfirmIntersection( ray_query );
		}
	
		if ( gl::RayQueryCommittedIntersection(gl.rayQuery.GetIntersectionType( ray_query, true )) == gl::RayQueryCommittedIntersection::None )
		{
			color = MissShader();
		}
		else
		{
			// hit shader
			float2	attribs		 = gl.rayQuery.GetIntersectionBarycentrics( ray_query, true );
			float3	barycentrics = float3( 1.0f - attribs.x - attribs.y, attribs.x, attribs.y );
			color = HitShader( barycentrics );
		}

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, color );
	}

#endif
//-----------------------------------------------------------------------------
