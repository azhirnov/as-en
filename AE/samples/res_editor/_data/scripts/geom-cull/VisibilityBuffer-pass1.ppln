// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	define SH_RAY_GEN
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<RayTracingPipeline>	ppln = RayTracingPipeline( "templ" );
		ppln.SetLayout( "rt-model.pl" );

		// general
		{
			RC<Shader>	rg = Shader();
			rg.type		= EShader::RayGen;
			rg.LoadSelf();
			ppln.AddGeneralShader( "Main", rg );
		}

		// specialization
		{
			RC<RayTracingPipelineSpec>	spec = ppln.AddSpecialization( "spec" );

			spec.AddToRenderTech( "rtech", "main" );  // in SceneRayTracingPass

			// shader binding table
			{
				RC<RayTracingShaderBinding>		sbt = RayTracingShaderBinding( spec, "sbt" );

				sbt.BindRayGen( "Main" );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_GEN
	#include "InvocationID.glsl"
	#include "HWRayTracing.glsl"

	uint3  UnpackID (uint id)
	{
		return uint3(
				id >> 31,				// instanceId
				(id >> 20) & 0x7FF,		// geometryId
				id & 0x000FFFFF );		// primitiveId
	}

	uint  PackID (uint instanceId, uint geometryId, uint primitiveId)
	{
		uint	r =	(instanceId << 31) |
					((geometryId & 0x7FF) << 20) |
					(primitiveId & 0x000FFFFF);

		//if ( ! AllEqual( UnpackID( r ), uint3( instanceId, geometryId, primitiveId )))
		//	return 0;
		return r;
	}

	void Main ()
	{
		const int2	coord		= GetGlobalCoord().xy;
		float3		view_dir	= Ray_Perspective( un_PerPass.camera.invViewProj, float3(0.0), 0.f, GetGlobalCoordUNorm().xy ).dir;

		gl::RayQuery	ray_query;
		gl.rayQuery.Initialize( ray_query, un_RtScene, gl::RayFlags::Opaque,
							    0xFF, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x,
								view_dir, un_PerPass.camera.clipPlanes.y );

		if ( gl.rayQuery.Proceed( ray_query ))
		{
			if ( GetCandidateIntersectionType( ray_query ) == gl::RayQueryCandidateIntersection::Triangle )
				gl.rayQuery.ConfirmIntersection( ray_query );
		}

		uint	id		= ~0u;
		float	depth	= 0.f;

		if ( GetCommittedIntersectionType( ray_query ) != gl::RayQueryCommittedIntersection::None )
		{
			depth	= GetCommittedIntersectionT( ray_query );
			id		= PackID( GetCommittedIntersectionInstanceId( ray_query ),
							  GetCommittedIntersectionGeometryIndex( ray_query ),
							  GetCommittedIntersectionPrimitiveIndex( ray_query ));
		}

		gl.image.Store( un_IDBuffer, coord, uint4(id) );		// R32U
		gl.image.Store( un_Depth,    coord, float4(depth) );	// R32F
	}

#endif
//-----------------------------------------------------------------------------
