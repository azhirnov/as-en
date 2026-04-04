// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Copy of [RT-Model](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/ray-trace/RT-Model.as) script with
	[Model-RT](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Model-RT.as) pipeline, ported to RayQuery.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>		ds		= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );

		RC<Scene>		scene	= Scene();
		RC<FPVCamera>	camera	= FPVCamera();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );

			const float	s = 1.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
		}

		// setup model
		{
			RC<Model>	model = Model( "res/models/Sponza/Sponza.gltf" );

			model.InitialTransform( Transform().Position( 0.f, 1.f, 0.f ).Rotation( 0.f, ToRad(90.f), ToRad(180.f) ).Scale( 100.f ));

			model.AddOmniLight( float3(0.f, -5.f, 0.f), float3(0.f, 0.f, 0.05f), RGBA32f(1.f) );

			scene.Add( model );
		}

		// render loop
		{
			RC<SceneRayQueryPass>	pass = scene.AddRayQueryPass( "rtrace" );
			pass.SetPipelineWithDefine( "" );
			pass.ArgOut( "un_OutImage",	rt );
			pass.LocalSize( 32*4 );
			pass.DispatchThreads( rt.Dimension() );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"
	#include "HWRayTracing.glsl"
	#include "ModelMaterial.glsl"

	const float	c_SmallOffset = 0.0001;


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

	ND_ MeshAndMaterial  GetMeshAndMaterial (gl::RayQuery rayQuery)
	{
		const float2		barycentrics= GetCommittedIntersectionBarycentrics( rayQuery );
		const uint			instance_id	= GetCommittedIntersectionInstanceId( rayQuery );
		const uint			geom_idx	= GetCommittedIntersectionGeometryIndex( rayQuery );
		const uint			prim_id		= GetCommittedIntersectionPrimitiveIndex( rayQuery );
		const float			hit_t		= GetCommittedIntersectionT( rayQuery );
		const float4x3		obj_to_world= GetCommittedIntersectionObjectToWorld( rayQuery );
		const float3		ray_origin	= obj_to_world * float4( GetCommittedIntersectionObjectRayOrigin( rayQuery ), 1.0 );
		const float3		ray_dir		= Normalize( obj_to_world * float4( GetCommittedIntersectionObjectRayDirection( rayQuery ), 0.0 ));

		ModelRTMesh_AERef	mesh		= un_RTInstances.meshesPerInstance[ instance_id ][ geom_idx ];
		NormalsRef			norm_addr	= NormalsRef(   mesh.normals );
		IndicesRef			idx_addr	= IndicesRef(   mesh.indices );
		TexcoordsRef		uv_addr		= TexcoordsRef( mesh.texcoords );
		const uint			idx			= prim_id * 3;
		float3x3			norm_mat	= un_RTInstances.normalMatPerInstance[ instance_id ].data[ geom_idx ];

		MeshAndMaterial	result;
		result.pos			= ray_origin + (ray_dir * hit_t);

		result.smoothNormal	= Normalize( norm_mat *
										 BaryLerp(	Unpack( norm_addr.data[ idx_addr.data[ idx+0 ]]),
													Unpack( norm_addr.data[ idx_addr.data[ idx+1 ]]),
													Unpack( norm_addr.data[ idx_addr.data[ idx+2 ]]),
													barycentrics ));
		result.uv0			= BaryLerp(	uv_addr.data[ idx_addr.data[ idx+0 ]],
										uv_addr.data[ idx_addr.data[ idx+1 ]],
										uv_addr.data[ idx_addr.data[ idx+2 ]],
										barycentrics );

		result.mtr			= un_Materials.elements[ un_RTInstances.materialsPerInstance[ instance_id ].data[ geom_idx ]];

		return result;
	}


	ND_ float  CastShadow (const float3 origin, const float3 dir, const float tmax)
	{
		HWRay	hwray	= HWRay_Create();
		hwray.rayFlags	= gl::RayFlags::Opaque | gl::RayFlags::TerminateOnFirstHit;
		hwray.rayOrigin	= origin;
		hwray.rayDir	= dir;
		hwray.tMin		= c_SmallOffset;
		hwray.tMax		= tmax * 1.01f;

		gl::RayQuery	ray_query;
		RayQuery_Init( ray_query, un_RtScene, hwray );

		while ( gl.rayQuery.Proceed( ray_query ))
		{}

		return GetCommittedIntersectionType( ray_query ) != gl::RayQueryCommittedIntersection::None ? 0.0 : 1.0;
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


	void  CastPrimaryRay (HWRay ray, out float3 outColor)
	{
		gl::RayQuery	ray_query;
		RayQuery_Init( ray_query, un_RtScene, ray );

		while ( gl.rayQuery.Proceed( ray_query ))
		{}

		if ( GetCommittedIntersectionType( ray_query ) == gl::RayQueryCommittedIntersection::Triangle )
		{
			MeshAndMaterial	mm		= GetMeshAndMaterial( ray_query );
			float4			albedo	= SampleLodAlbedo( mm.mtr, mm.uv0, 0.f );
							albedo	*= CalcLightingAndShading( mm.pos, mm.smoothNormal );

			outColor = albedo.rgb;
		}
		else
		{
			// miss
			outColor = float3(0.412f, 0.796f, 1.0f);
		}
	}


	void Main ()
	{
		Ray		ray		= Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );
		HWRay	hwray	= HWRay_Create( ray, un_PerPass.camera.clipPlanes.y, 0 );
		float3	color;

		CastPrimaryRay( hwray, OUT color );

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(color, 1.0) );
	}

#endif
//-----------------------------------------------------------------------------
