// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Draw cube with multiple reflections and refractions.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	define SH_RAY_CHIT
#	include <glsl.h>
#	define PRIMARY_MISS
#	define SHADOW_MISS
#	define PRIMARY_HIT
#	define SHADOW_HIT
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	Random	_rnd;
	float	Rnd () { return _rnd.Uniform( 0.f, 1.f ); }

	void ASmain ()
	{
		// initialize
		RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<RTScene>		scene		= RTScene();
		RC<FPVCamera>	camera		= FPVCamera();
		RC<Buffer>		cube		= Buffer();

		const string	cm_addr			= "res/humus/Teide/";	const string  cm_ext = ".jpg";	const uint2	cm_dim (2048);

		RC<Image>		cubemap			= Image( EPixelFormat::RGBA8_UNorm, cm_dim, ImageLayer(6), MipmapLevel(~0) );	cubemap.Name( "Cubemap tex" );
		RC<Image>		cubemap_view	= cubemap.CreateView( EImage::Cube );

		const uint		max_ray_types	= 1;
		const uint		max_recursion	= 16;


		// load cubemap
		{
			cubemap.LoadLayer( cm_addr+ "posx" +cm_ext, 0, ImageLoadOpFlags::GenMipmaps );	// -Z
			cubemap.LoadLayer( cm_addr+ "negx" +cm_ext, 1, ImageLoadOpFlags::GenMipmaps );	// +Z
			cubemap.LoadLayer( cm_addr+ "posy" +cm_ext, 2, ImageLoadOpFlags::GenMipmaps );	// +Y	- up
			cubemap.LoadLayer( cm_addr+ "negy" +cm_ext, 3, ImageLoadOpFlags::GenMipmaps );	// -Y	- down
			cubemap.LoadLayer( cm_addr+ "posz" +cm_ext, 4, ImageLoadOpFlags::GenMipmaps );	// -X
			cubemap.LoadLayer( cm_addr+ "negz" +cm_ext, 5, ImageLoadOpFlags::GenMipmaps );	// +X
		}

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 30.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
		}

		scene.MaxRayTypes( max_ray_types );

		// create cube
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position | EAttribute::Normal | EAttribute::Texcoord2D );
			mesh.AddCube();

			RC<RTGeometry>	geom = RTGeometry();
			geom.AddIndexedTriangles( mesh );

			scene.AddInstance( geom, Transform().Position(float3(0.f, 2.0f, 4.f)).Rotation(float3(0.f, ToRad(45.f), 0.f)) );

			@cube = mesh.ToBuffer();
		}

		// render loop
		{
			RC<RayTracingPass>		pass = RayTracingPass();
			pass.Set(	 camera );
			pass.ArgOut( "un_OutImage",		rt );
			pass.ArgIn(  "un_RtScene",		scene );
			pass.ArgIn(  "un_Geometry",		cube );
			pass.ArgIn(  "un_CubeMap",		cubemap_view,		Sampler_LinearMipmapRepeat );
			pass.Dispatch( rt.Dimension() );

			pass.ColorSelector( "iMaterialColor",		RGBA32f(0.33f, 0.93f, 0.29f, 1.f) );
			pass.ColorSelector( "iReflectionColorMask",	RGBA32f(0.22f, 0.83f, 0.93f, 1.f) );
			pass.Slider( "iAbsorptionScale",	1.f,	10.f,			2.5f );
			pass.Slider( "iAmbientLight",		0.f,	1.f,			0.2f );
			pass.Slider( "iIndexOfRefraction",	1.f,	3.f,			1.1f );
			pass.Slider( "iMaxRecursion",		1,		max_recursion,	5 );

			pass.MaxRayRecursion( max_recursion );

			// setup SBT
			pass.RayGen( RTShader("") );
			pass.MaxRayTypes( max_ray_types );

			pass.RayMiss( RayIndex(0), RTShader("") );

			pass.TriangleHit( RayIndex(0), InstanceIndex(0), RTShader("") );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------

#define PRIMARY_RAY_INDEX	0

struct PrimaryRayPayload
{
	// out
	float3	color;
	float	depth;

	// inout
	uint	recursion;
};

//-----------------------------------------------------------------------------
#if defined(SH_RAY_GEN) or defined(SH_RAY_CHIT)
	#include "Math.glsl"
	#include "HWRayTracing.glsl"

	#ifdef SH_RAY_GEN
		layout(location=PRIMARY_RAY_INDEX)	gl::RayPayload   PrimaryRayPayload  PrimaryRay;
	#else
		layout(location=PRIMARY_RAY_INDEX)	gl::RayPayloadIn PrimaryRayPayload  PrimaryRay;
	#endif

	const float	c_SmallOffset	= 0.0001;
	const float	c_AirIOR		= 1.0;		// Air index of refraction


	void  CastPrimaryRay (in HWRay ray, uint recursion)
	{
		PrimaryRay.color		= float3(0.0);
		PrimaryRay.depth		= 0.0;
		PrimaryRay.recursion	= recursion;

		if ( recursion >= iMaxRecursion )
		{
			PrimaryRay.color = float3(0.95, 0.18, 0.95);	// for debugging
			return;
		}

		// hitShader = RTSceneBuild::InstanceVk::instanceSBTOffset + ray.rayIndex
		HWTraceRay( un_RtScene, ray, /*payload*/PRIMARY_RAY_INDEX );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_GEN
	#include "InvocationID.glsl"

	void Main ()
	{
		Ray		ray		= Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );
		HWRay	hwray	= HWRay_Create( ray, un_PerPass.camera.clipPlanes.y, PRIMARY_RAY_INDEX );

		CastPrimaryRay( hwray, 0 );

		gl.image.Store( un_OutImage, int2(gl.LaunchID), float4(PrimaryRay.color, 1.f) );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_MISS
	#include "Math.glsl"

	layout(location=PRIMARY_RAY_INDEX)	gl::RayPayloadIn PrimaryRayPayload	PrimaryRay;

	void Main ()
	{
		float3	uv = gl.WorldRayDirection * float3(1.f, -1.f, -1.f);

		PrimaryRay.color	= gl.texture.SampleLod( un_CubeMap, uv, 0.f ).rgb;
		PrimaryRay.depth	= gl.RayTmax;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_CHIT
	#include "PBR.glsl"

	gl::HitAttribute float2  in_HitAttribs;


	ND_ float3  LightAbsorption (const float3 sceneColor, const float depth, const float absorption)
	{
		return ApproxLightAbsorptionInVolume( sceneColor, iMaterialColor.rgb, float3(iAmbientLight), depth, absorption * iAbsorptionScale );
	}


	void  RayTrace (float3 objNormal, const uint recursion)
	{
		const float	err			= float_epsilon;
		float		absorption	= 0.1;
		HWRay		hwray		= HWRay_Create();

		hwray.tMin	= 0.0;
		hwray.tMax	= 100.0;

		// Refraction at the interface between air and current object.
		float	eta	= c_AirIOR / iIndexOfRefraction;	// incident / transmitted

		// Refraction at the interface between current object and air.
		if ( gl.HitKind != gl::TriangleHitKind::FrontFacing )
		{
			eta			= 1.0 / eta;
			objNormal	= -objNormal;
		}

		float	fresnel				= FresnelDielectric( Dot( gl.WorldRayDirection, -objNormal ), eta );
		float3	reflection_color	= float3(0.0);
		float3	result_color		= float3(0.0);

		// Reflection
		if ( fresnel > err )
		{
			hwray.rayOrigin	= gl.WorldRayOrigin + (gl.WorldRayDirection * gl.HitT) + (objNormal * c_SmallOffset);
			hwray.rayDir	= Normalize( Reflect( gl.WorldRayDirection, objNormal ));

			CastPrimaryRay( hwray, recursion+1 );
			reflection_color = PrimaryRay.color;

			if ( gl.HitKind == gl::TriangleHitKind::BackFacing )
				reflection_color = LightAbsorption( PrimaryRay.color, PrimaryRay.depth, absorption );

			reflection_color *= iReflectionColorMask.rgb * fresnel;
		}

		// Refraction
		if ( fresnel < 1.0f-err )
		{
			hwray.rayOrigin	= gl.WorldRayOrigin + (gl.WorldRayDirection * gl.HitT) - (objNormal * c_SmallOffset);
			hwray.rayDir	= Normalize( Refract( gl.WorldRayDirection, objNormal, eta ));

			if ( AllEqual( hwray.rayDir, hwray.rayDir ))
			{
				CastPrimaryRay( hwray, recursion+1 );
				result_color = PrimaryRay.color;

				if ( gl.HitKind == gl::TriangleHitKind::FrontFacing  or  recursion == 0 )
					result_color = LightAbsorption( PrimaryRay.color, PrimaryRay.depth, absorption );

				result_color *= (1.0 - fresnel);
			}
		}

		// 'PrimaryRay' was modified in 'CastPrimaryRay()' so restore previous state
		// and set new color and depth
		PrimaryRay.color		= result_color + reflection_color;
		PrimaryRay.depth		= gl.HitT;
		PrimaryRay.recursion	= recursion;
	}


	void Main ()
	{
		const uint		idx			= gl.PrimitiveID*3;
		const float3	normal		= Normalize( float3x3(gl.ObjectToWorld) *
												 BaryLerp(	un_Geometry.normal[ un_Geometry.indices[ idx+0 ]],
															un_Geometry.normal[ un_Geometry.indices[ idx+1 ]],
															un_Geometry.normal[ un_Geometry.indices[ idx+2 ]],
															in_HitAttribs ));

		RayTrace( normal, PrimaryRay.recursion );
	}

#endif
//-----------------------------------------------------------------------------
