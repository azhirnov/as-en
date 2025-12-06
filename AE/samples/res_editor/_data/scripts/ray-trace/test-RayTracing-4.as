// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw triangles using ray query in compute shader.
	Instance transformation updated every frame in compute shader.
	TLAS is being rebuild with indirect command.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	define AE_ray_query
#	define AE_RTAS_BUILD
#	include <glsl.h>
#	define UPD_INSTANCES
#	define TRACE_RAYS
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
			array<float2>	positions	= { float2(0.f, -0.2f), float2(0.2f, 0.2f), float2(-0.2f, 0.2f) };
			array<uint>		indices		= { 0, 1, 2 };

			triangles.FloatArray(	"positions",	positions );
			triangles.UIntArray(	"indices",		indices );

			geom.AddIndexedTriangles( triangles, triangles );

			for (uint i = 0; i < 12; ++i) {
				scene.AddInstance( geom );
			}
		}

		// render loop
		{
			RC<ComputePass>		inst_pass = ComputePass( "", "UPD_INSTANCES" );
			inst_pass.ArgInOut( "un_Instances",		scene.InstanceBuffer() );
			inst_pass.ArgInOut( "un_IndirectCmd",	scene.IndirectBuffer() );
			inst_pass.LocalSize( 1 );
			inst_pass.DispatchThreads( scene.InstanceCount() );
		}{
			// update RTScene using instance data from prev pass
			BuildRTSceneIndirect( scene );
		}{
			RC<ComputePass>		rt_pass = ComputePass( "", "TRACE_RAYS" );
			rt_pass.ArgOut( "un_OutImage",	rt );
			rt_pass.ArgIn(  "un_RtScene",	scene );
			rt_pass.LocalSize( 8, 8 );
			rt_pass.DispatchThreads( rt.Dimension() );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef UPD_INSTANCES
	#include "InvocationID.glsl"
	#include "HWRayTracing.glsl"
	#include "Matrix.glsl"

	void  Main ()
	{
		const int	idx			= GetGlobalIndex();
		const int	inst_count	= un_Instances.elements.length();
		float		time		= float_Pi * un_PerPass.time * 0.2f + float(idx);
		float4x3	transform	= float4x3(f3x3_Rotate( time, float3(0.f, 0.f, 1.f) ));
		float3		pos			= float3( ToSNorm( float(idx % 4) * 0.3 ),
										  ToSNorm( float(idx / 4) * 0.3 ) + 0.5,
										  0.0 );

		SetTranslation( INOUT transform, pos );

		un_Instances.elements[idx].transform = MatTranspose( transform );

		if ( idx == 0 )
		{
			un_IndirectCmd.primitiveCount =
				Clamp( int(Fract(un_PerPass.time * 0.5) * inst_count + 0.5), 1, inst_count );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef TRACE_RAYS
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
		const float3	origin		= float3(uv, -1.0f);
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
			float3	barycentric = float3( 1.0f - attribs.x - attribs.y, attribs.x, attribs.y );
					color		= HitShader( barycentric, GetCommittedIntersectionInstanceCustomIndex( ray_query ));
		}

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, color );
	}

#endif
//-----------------------------------------------------------------------------
