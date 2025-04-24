// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	define SH_RAY_GEN
#	include <glsl.h>

#	define PRIMARY_MISS
#	define SHADOW_MISS

#	define PRIMARY_OPAQUE_HIT
#	define PRIMARY_TRANSLUCENT_HIT
#	define PRIMARY_VOLUMETRIC_HIT
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

		// miss
		{
			RC<Shader>	rm = Shader();
			rm.type		= EShader::RayMiss;
			rm.LoadSelf();
			rm.Define( "PRIMARY_MISS" );
			ppln.AddGeneralShader( "PrimaryMiss", rm );
		}
		{
			RC<Shader>	rm = Shader();
			rm.type		= EShader::RayMiss;
			rm.LoadSelf();
			rm.Define( "SHADOW_MISS" );
			ppln.AddGeneralShader( "ShadowMiss", rm );
		}

		// triangle hit groups
		{
			RC<Shader>	rch = Shader();
			rch.type	= EShader::RayClosestHit;
			rch.LoadSelf();
			rch.Define( "PRIMARY_OPAQUE_HIT" );
			ppln.AddTriangleHitGroup( "PrimaryOpaque", rch, null );
		}{
			RC<Shader>	rch = Shader();
			rch.type	= EShader::RayClosestHit;
			rch.LoadSelf();
			rch.Define( "PRIMARY_TRANSLUCENT_HIT" );
			ppln.AddTriangleHitGroup( "PrimaryTranslucent", rch, null );
		}{
			RC<Shader>	rch = Shader();
			rch.type	= EShader::RayClosestHit;
			rch.LoadSelf();
			rch.Define( "PRIMARY_VOLUMETRIC_HIT" );
			ppln.AddTriangleHitGroup( "PrimaryVolumetric", rch, null );
		}

		// specialization
		{
			RC<RayTracingPipelineSpec>	spec = ppln.AddSpecialization( "spec" );

			spec.MaxRecursionDepth( 2 );
			spec.AddToRenderTech( "rtech", "main" );  // in SceneRayTracingPass

			// shader binding table
			{
				RC<RayTracingShaderBinding>		sbt = RayTracingShaderBinding( spec, "sbt" );

				sbt.BindRayGen( "Main" );

				sbt.MaxRayTypes( 2 );

				sbt.BindMiss( "PrimaryMiss",	RayIndex(0) );
				sbt.BindMiss( "ShadowMiss",		RayIndex(1) );

				sbt.BindHitGroup( "PrimaryOpaque",		InstanceIndex(0),	RayIndex(0) );
			//	sbt.BindHitGroup( "ShadowOpaque",		InstanceIndex(0),	RayIndex(1) );

				sbt.BindHitGroup( "PrimaryTranslucent",	InstanceIndex(1),	RayIndex(0) );
			//	sbt.BindHitGroup( "ShadowTranslucent",	InstanceIndex(1),	RayIndex(1) );

				sbt.BindHitGroup( "PrimaryVolumetric",	InstanceIndex(2),	RayIndex(0) );
			//	sbt.BindHitGroup( "ShadowVolumetric",	InstanceIndex(2),	RayIndex(1) );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------

#define PRIMARY_RAY		0
#define SHADOW_RAY		1

struct PrimaryRayPayload
{
	// out
	float3	color;
	float	distance;
};

struct ShadowRayPayload
{
	// out
	float	shading;	// set 0 on hit and 1 otherwise
};

//-----------------------------------------------------------------------------
#ifdef SH_RAY_GEN
	#include "InvocationID.glsl"
	#include "HWRayTracing.glsl"

	layout(location=PRIMARY_RAY)	gl::RayPayload PrimaryRayPayload  PrimaryRay;

	void Main ()
	{
		Ray		ray		= Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );
		HWRay	hwray	= HWRay_Create( ray, un_PerPass.camera.clipPlanes.y, PRIMARY_RAY );

		// hitShader = RTSceneBuild::InstanceVk::instanceSBTOffset + hwray.rayIndex
		HWTraceRay( un_RtScene, hwray, /*payload*/PRIMARY_RAY );

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(PrimaryRay.color, 1.f) );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PRIMARY_MISS
	#include "Math.glsl"

	layout(location=PRIMARY_RAY)	gl::RayPayloadIn PrimaryRayPayload  PrimaryRay;

	void Main ()
	{
		PrimaryRay.color	= float3(0.412f, 0.796f, 1.0f);
		PrimaryRay.distance	= gl.RayTmax * 2.0f;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SHADOW_MISS
	#include "Math.glsl"

	layout(location=SHADOW_RAY)		gl::RayPayloadIn ShadowRayPayload  ShadowRay;

	void Main ()
	{
		ShadowRay.shading = 1.f;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_CHIT
	#include "Math.glsl"
	#include "HWRayTracing.glsl"
	#include "ModelMaterial.glsl"

	#define TexcoordsRef	float2_AEPtr
	#define NormalsRef		packed_float3_AEPtr
	#define IndicesRef		uint_AEPtr

	struct MeshAndMaterial
	{
		float3			pos;			// world space
	//	float3			surfNormal;		// world space  // from triangle positions
		float3			smoothNormal;	// world space  // from vertex attributes
		float2			uv0;
		ModelMaterial	mtr;
	};

	ND_ MeshAndMaterial  GetMeshAndMaterial (float2 barycentrics)
	{
		ModelRTMesh_AERef	mesh		= un_RTInstances.meshesPerInstance[ gl.InstanceID ][ gl.GeometryIndex ];
		NormalsRef			norm_addr	= NormalsRef(   mesh.normals );
		IndicesRef			idx_addr	= IndicesRef(   mesh.indices );
		TexcoordsRef		uv_addr		= TexcoordsRef( mesh.texcoords );
		const uint			idx			= gl.PrimitiveID * 3;
		float3x3			norm_mat	= un_RTInstances.normalMatPerInstance[ gl.InstanceID ].data[ gl.GeometryIndex ];

		MeshAndMaterial	result;
		result.pos			= gl.WorldRayOrigin + (gl.WorldRayDirection * gl.HitT);

		result.smoothNormal	= Normalize( norm_mat *
										 BaryLerp(	Unpack( norm_addr.data[ idx_addr.data[ idx+0 ]]),
													Unpack( norm_addr.data[ idx_addr.data[ idx+1 ]]),
													Unpack( norm_addr.data[ idx_addr.data[ idx+2 ]]),
													barycentrics ));
		result.uv0			= BaryLerp(	uv_addr.data[ idx_addr.data[ idx+0 ]],
										uv_addr.data[ idx_addr.data[ idx+1 ]],
										uv_addr.data[ idx_addr.data[ idx+2 ]],
										barycentrics );

		result.mtr			= un_Materials.elements[ un_RTInstances.materialsPerInstance[ gl.InstanceID ].data[ gl.GeometryIndex ]];

		return result;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PRIMARY_OPAQUE_HIT

	layout(location=PRIMARY_RAY)	gl::RayPayloadIn PrimaryRayPayload	PrimaryRay;
	layout(location=SHADOW_RAY)		gl::RayPayload   ShadowRayPayload	ShadowRay;

	gl::HitAttribute float2  in_HitAttribs;

	const float	c_SmallOffset = 0.0001;


	ND_ float  CastShadow (const float3 origin, const float3 dir, const float tmax)
	{
		HWRay	hwray	= HWRay_Create();
		hwray.rayFlags	= gl::RayFlags::Opaque | gl::RayFlags::TerminateOnFirstHit | gl::RayFlags::SkipClosestHitShader;
		hwray.rayIndex	= SHADOW_RAY;
		hwray.rayOrigin	= origin;
		hwray.rayDir	= dir;
		hwray.tMin		= c_SmallOffset;
		hwray.tMax		= tmax * 1.01f;

		ShadowRay.shading = 0.0f;

		// hitShader = RTSceneBuild::InstanceVk::instanceSBTOffset + hwray.rayIndex
		HWTraceRay( un_RtScene, hwray, /*payload*/SHADOW_RAY );

		return Saturate( ShadowRay.shading );
	}


	ND_ float4  CalcLightingAndShading (const float3 worldPos, const float3 worldNormal)
	{
		float3	diffuse		= float3(0.0);
		float3	ray_origin	= worldPos + worldNormal * c_SmallOffset;
		float	max_dist	= 1000.f;

		for (uint i = 0; i < un_Lights.directionalCount; ++i)
		{
			const SceneDirectionalLight	light = un_Lights.directional[i];

			float	atten	= 1.0 / light.attenuation.x;		// only const attenuation
					atten	*= CastShadow( ray_origin, light.direction, max_dist );
			diffuse += UnpackRGBM( light.colorRGBM ).rgb * LambertDiffuse( light.direction, worldNormal ) * atten;
		}

		for (uint i = 0; i < un_Lights.coneCount; ++i)
		{
			const SceneConeLight	light = un_Lights.cone[i];

			float	dist	= Distance( worldPos, light.position );
			float	atten	= Attenuation( light.attenuation, dist );
					atten	*= CastShadow( ray_origin, light.direction, dist );
			// TODO: test cone
			diffuse += UnpackRGBM( light.colorRGBM ).rgb * LambertDiffuse( light.direction, worldNormal ) * atten;
		}

		for (uint i = 0; i < un_Lights.omniCount; ++i)
		{
			const SceneOmniLight	light = un_Lights.omni[i];

			float	dist	= Distance( worldPos, light.position );
			float3	dir		= (light.position - worldPos) / dist;
			float	atten	= Attenuation( light.attenuation, dist );
					atten	*= CastShadow( ray_origin, dir, dist );
			diffuse += UnpackRGBM( light.colorRGBM ).rgb * LambertDiffuse( dir, worldNormal ) * atten;
		}

		return float4( diffuse, 1.f );
	}


	void Main ()
	{
		MeshAndMaterial	mm		= GetMeshAndMaterial( in_HitAttribs );
		float4			albedo	= SampleLodAlbedo( mm.mtr, mm.uv0, 0.f );
						albedo	*= CalcLightingAndShading( mm.pos, mm.smoothNormal );

		PrimaryRay.color	= albedo.rgb;
		PrimaryRay.distance = gl.HitT;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PRIMARY_TRANSLUCENT_HIT
	#include "Math.glsl"
	#include "HWRayTracing.glsl"

	layout(location=PRIMARY_RAY)	gl::RayPayloadIn PrimaryRayPayload	PrimaryRay;

	gl::HitAttribute float2  in_HitAttribs;

	void Main ()
	{
		PrimaryRay.color	= float3(in_HitAttribs, 1.0);
		PrimaryRay.distance = gl.HitT;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PRIMARY_VOLUMETRIC_HIT
	#include "Math.glsl"
	#include "HWRayTracing.glsl"

	layout(location=PRIMARY_RAY)	gl::RayPayloadIn PrimaryRayPayload	PrimaryRay;

	gl::HitAttribute float2  in_HitAttribs;

	void Main ()
	{
		PrimaryRay.color	= float3(in_HitAttribs, 1.0);
		PrimaryRay.distance = gl.HitT;
	}

#endif
//-----------------------------------------------------------------------------
