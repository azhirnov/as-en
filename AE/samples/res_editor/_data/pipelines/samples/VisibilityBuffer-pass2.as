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
	#include "ModelMaterial.glsl"
	#include "../3party_shaders/VisibilityBuffer.glsl"

	#define PositionsRef	packed_float3_AEPtr
	#define NormalsRef		packed_float3_AEPtr
	#define TexcoordsRef	float2_AEPtr
	#define IndicesRef		uint_AEPtr

	struct MeshAndMaterial
	{
		float3			pos;			// world space
	//	float3			surfNormal;		// world space  // from triangle positions
		float3			smoothNormal;	// world space  // from vertex attributes
		float2			uv0;
		float2			uv0_dx;
		float2			uv0_dy;
		ModelMaterial	mtr;
	};


	uint3  UnpackID (uint id)
	{
		return uint3(
				id >> 31,				// instanceId
				(id >> 20) & 0x7FF,		// geometryId
				id & 0x000FFFFF );		// primitiveId
	}

	float3  LocalPosToWorldSpace (float4x4 mat, float3 pos) {
		return (mat * float4(pos, 1.0f)).xyz - un_PerPass.camera.pos;
	}

	float4  LocalPosToClipSpace (float4x4 mat, float3 pos) {
		return un_PerPass.camera.viewProj * float4(LocalPosToWorldSpace( mat, pos ), 1.0);
	}

	float4  WorldPosToClipSpace (float3 pos) {
		return un_PerPass.camera.viewProj * float4(pos, 1.0);
	}


	MeshAndMaterial  UnpackTriangle (uint3 id)
	{
		ModelRTMesh_AERef	mesh		= un_RTInstances.meshesPerInstance[ id.x ][ id.y ];
		PositionsRef		pos_addr	= PositionsRef( mesh.positions );
		NormalsRef			norm_addr	= NormalsRef(   mesh.normals );
		IndicesRef			idx_addr	= IndicesRef(   mesh.indices );
		TexcoordsRef		uv_addr		= TexcoordsRef( mesh.texcoords );
		const uint3			idx			= uint3( idx_addr.data[ id.z*3+0 ], idx_addr.data[ id.z*3+1 ], idx_addr.data[ id.z*3+2 ]);
		float4x4			model_mat	= un_RTInstances.modelMatPerInstance[ id.x ].data[ id.y ];
		float3x3			norm_mat	= un_RTInstances.normalMatPerInstance[ id.x ].data[ id.y ];
		MeshAndMaterial		result;

		float3				wpos0		= LocalPosToWorldSpace( model_mat, Unpack( pos_addr.data[ idx.x ]) );
		float3				wpos1		= LocalPosToWorldSpace( model_mat, Unpack( pos_addr.data[ idx.y ]) );
		float3				wpos2		= LocalPosToWorldSpace( model_mat, Unpack( pos_addr.data[ idx.z ]) );

	# if 0
		BarycentricDeriv	deriv		= CalcRayBary(	wpos0, wpos1, wpos2,
														float3(GetGlobalCoordSNorm().xy, 0.0), float3(0.0),
														MatInverse(un_PerPass.camera.view), MatInverse(un_PerPass.camera.proj),
														2.0 * GetGlobalSizeRcp().xy );
	# else
		// faster
		BarycentricDeriv	deriv		= CalcFullBary( WorldPosToClipSpace( wpos0 ),
														WorldPosToClipSpace( wpos1 ),
														WorldPosToClipSpace( wpos2 ),
														GetGlobalCoordSNorm().xy,
														2.0 * GetGlobalSizeRcp().xy );
	# endif
		GradientInterpolationResults uv_res = Interpolate2DWithDeriv( deriv, uv_addr.data[idx.x], uv_addr.data[idx.y], uv_addr.data[idx.z] );

		result.uv0			= uv_res.interp;
		result.uv0_dx		= uv_res.dx;
		result.uv0_dy		= uv_res.dy;

		result.smoothNormal	= Normalize( norm_mat *
										 BaryLerp(	Unpack( norm_addr.data[ idx.x ]),
													Unpack( norm_addr.data[ idx.y ]),
													Unpack( norm_addr.data[ idx.z ]),
													deriv.m_lambda ));
		result.pos			= BaryLerp( wpos0, wpos1, wpos2, deriv.m_lambda );

		result.mtr			= un_Materials.elements[ un_RTInstances.materialsPerInstance[ id.x ].data[ id.y ]];

		return result;
	}


	void Main ()
	{
		const int2		coord	= GetGlobalCoord().xy;
		const uint		id		= gl.image.Load( un_IDBuffer, coord ).r;

		if ( id == ~0u )
		{
			gl.image.Store( un_ColorBuf, coord, float4(0.0f, 1.f, 1.f, 1.f) );
			return;
		}

		MeshAndMaterial	mm		= UnpackTriangle( UnpackID( id ));
		float4			albedo	= SampleGradAlbedo( mm.mtr, mm.uv0, mm.uv0_dx, mm.uv0_dy );

		albedo *= CalcLighting( mm.pos, mm.smoothNormal );

		gl.image.Store( un_ColorBuf, coord, albedo );
	}

#endif
//-----------------------------------------------------------------------------
