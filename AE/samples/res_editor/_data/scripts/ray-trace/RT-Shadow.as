// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Simple ray tracing with recursion.
	Draw spheres, cubes, plane and animated omni light.
	Vertex attributes passed to shader as a buffer reference.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	define SH_RAY_GEN
#	include <glsl.h>
#	define PRIMARY_MISS
#	define SHADOW_MISS
#	define PRIMARY_HIT
#	define SHADOW_HIT
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<RTScene>		scene		= RTScene();
		RC<FPVCamera>	camera		= FPVCamera();
		RC<Buffer>		geom_data	= Buffer();

		array<ulong>	normals_addr;
		array<ulong>	indices_addr;

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 30.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
		}

		const uint	max_ray_types = 2;
		scene.MaxRayTypes( max_ray_types );

		// create plane
		{
			RC<RTGeometry>	geom	= RTGeometry();
			RC<Buffer>		plane	= Buffer();

			array<float2>	texcoords;
			array<uint>		indices;
			GetGrid( 4, OUT texcoords, OUT indices );

			array<float3>	positions;
			array<float3>	normals;

			for (uint i = 0; i < texcoords.size(); ++i) {
				positions.push_back( float3( ToSNorm(texcoords[i].x), 0.f, ToSNorm(texcoords[i].y) ) * 10.f );
				normals.push_back( float3( 0.f, -1.f, 0.f ));
			}

			uint	pos_off		= plane.FloatArray(	"positions",	positions );
			uint	norm_off	= plane.FloatArray(	"normals",		normals );
			uint	idx_off		= plane.UIntArray(	"indices",		indices );

			geom.AddIndexedTriangles( plane, plane );

			scene.AddInstance( geom, float3(0.f, 0.5f, 0.f) );

			normals_addr.push_back( plane.DeviceAddress() + norm_off );
			indices_addr.push_back( plane.DeviceAddress() + idx_off );
			geom_data.AddReference( plane );
		}

		// create cube
		{
			RC<RTGeometry>	geom	= RTGeometry();
			RC<Buffer>		cube	= Buffer();

			array<float3>	positions;
			array<float3>	normals;
			array<uint>		indices;
			GetCube( OUT positions, OUT normals, OUT indices );

			uint	pos_off		= cube.FloatArray(	"positions",	positions );
			uint	norm_off	= cube.FloatArray(	"normals",		normals );
			uint	idx_off		= cube.UIntArray(	"indices",		indices );

			geom.AddIndexedTriangles( cube, cube );

			for (int i = 0; i < 4; ++i)
			{
				scene.AddInstance( geom, float3(-2.f + 4.f * (i%2), -0.5f, -2.f - 4.f * (i/2)) );

				normals_addr.push_back( cube.DeviceAddress() + norm_off );
				indices_addr.push_back( cube.DeviceAddress() + idx_off );
			}
			geom_data.AddReference( cube );
		}

		// create sphere
		{
			RC<RTGeometry>	geom	= RTGeometry();
			RC<Buffer>		sphere	= Buffer();

			array<float3>	positions;
			array<float3>	normals;
			array<float3>	tangents;
			array<float3>	bitangents;
			array<float3>	texcoords;
			array<uint>		indices;
			GetSphere( 4, OUT positions, OUT normals, OUT tangents, OUT bitangents, OUT texcoords, OUT indices );

			uint	pos_off		= sphere.FloatArray(	"positions",	positions );
			uint	norm_off	= sphere.FloatArray(	"normals",		normals );
			uint	idx_off		= sphere.UIntArray(		"indices",		indices );

			geom.AddIndexedTriangles( sphere, sphere );

			for (int i = 0; i < 4; ++i)
			{
				scene.AddInstance( geom, float3(-2.f + 4.f * (i%2), -0.5f, 2.f + 4.f * (i/2)) );

				normals_addr.push_back( sphere.DeviceAddress() + norm_off );
				indices_addr.push_back( sphere.DeviceAddress() + idx_off );
			}
			geom_data.AddReference( sphere );
		}

		// create geometry data
		{
			Assert( normals_addr.size() == indices_addr.size() );

			geom_data.ULongArray(	"normals",		normals_addr );
			geom_data.ULongArray(	"indices",		indices_addr );
			geom_data.Float(		"lightPos",		float3(0.f, -5.f, 0.f) );
		}

		// render loop
		{
			RC<RayTracingPass>		pass = RayTracingPass();
			pass.Set(	 camera );
			pass.ArgOut( "un_OutImage",	rt );
			pass.ArgIn(  "un_RtScene",	scene );
			pass.ArgIn(  "un_Geometry",	geom_data );
			pass.Dispatch( rt.Dimension() );

			// setup SBT
			pass.RayGen( RTShader("") );
			pass.MaxRayTypes( max_ray_types );

			pass.RayMiss( RayIndex(0), RTShader("", "PRIMARY_MISS") );
			pass.RayMiss( RayIndex(1), RTShader("", "SHADOW_MISS") );

			for (uint i = 0; i < indices_addr.size(); ++i)
			{
				pass.TriangleHit( RayIndex(0), InstanceIndex(i), RTShader("", "PRIMARY_HIT") );
				pass.TriangleHit( RayIndex(1), InstanceIndex(i), RTShader("", "SHADOW_HIT") );
			}
		}
		Present( rt );
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
	float	shading;
};

layout(std430, buffer_reference) buffer readonly NormalsRef	{ float3	normals	[]; };
layout(std430, buffer_reference) buffer readonly IndicesRef	{ uint		indices	[]; };

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

		gl.image.Store( un_OutImage, int2(gl.LaunchID), float4(PrimaryRay.color, 1.f) );
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

	layout(location=SHADOW_RAY)	gl::RayPayloadIn ShadowRayPayload  ShadowRay;

	void Main ()
	{
		// set 0 on hit and 1 otherwise
		ShadowRay.shading = 1.f;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PRIMARY_HIT
	#include "Math.glsl"
	#include "HWRayTracing.glsl"

	layout(location=PRIMARY_RAY)	gl::RayPayloadIn PrimaryRayPayload	PrimaryRay;
	layout(location=SHADOW_RAY)		gl::RayPayload   ShadowRayPayload	ShadowRay;

	gl::HitAttribute float2  in_HitAttribs;

	const float	c_SmallOffset = 0.00001;


	ND_ float  CastShadow (const float3 origin, const float3 dir, const float tmax)
	{
		HWRay	hwray	= HWRay_Create();
		hwray.rayFlags	= gl::RayFlags::Opaque | gl::RayFlags::TerminateOnFirstHit | gl::RayFlags::SkipClosestHitShader;
		hwray.rayIndex	= SHADOW_RAY;
		hwray.rayOrigin	= origin;
		hwray.rayDir	= dir;
		hwray.tMin		= c_SmallOffset;
		hwray.tMax		= tmax;

		ShadowRay.shading = 0.0f;

		// hitShader = RTSceneBuild::InstanceVk::instanceSBTOffset + hwray.rayIndex
		HWTraceRay( un_RtScene, hwray, /*payload*/SHADOW_RAY );

		return Saturate( ShadowRay.shading );
	}

	void Main ()
	{
		NormalsRef		norm_addr		= NormalsRef( un_Geometry.normals[ gl.InstanceID ]);
		IndicesRef		idx_addr		= IndicesRef( un_Geometry.indices[ gl.InstanceID ]);

		const uint		idx				= gl.PrimitiveID*3;
		const float3	normal			= Normalize( float3x3(gl.ObjectToWorld) *
													 BaryLerp(	norm_addr.normals[ idx_addr.indices[ idx+0 ]],
																norm_addr.normals[ idx_addr.indices[ idx+1 ]],
																norm_addr.normals[ idx_addr.indices[ idx+2 ]],
																in_HitAttribs ));

		const float3	light_pos		= un_Geometry.lightPos + float3( 0.f, Sin(un_PerPass.time) * 4.f, 0.f );

		const float3	world_pos		= gl.WorldRayOrigin + gl.WorldRayDirection * gl.HitT;
		const float3	light_dir		= Normalize( light_pos - world_pos );
		const float		dist_to_light	= Distance( light_pos, world_pos ) * 1.01f;
		const float3	ray_origin		= world_pos + normal * c_SmallOffset;

		const float		n_dot_l			= Max( 0.0, Dot( normal, light_dir ));
		const float		shading			= CastShadow( ray_origin, light_dir, dist_to_light );

		PrimaryRay.color	= float3(n_dot_l * shading);
		PrimaryRay.distance = gl.HitT;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SHADOW_HIT
	#include "Math.glsl"

	layout(location=SHADOW_RAY)	gl::RayPayloadIn ShadowRayPayload  ShadowRay;

	void Main ()
	{
		// set 0 on hit and 1 otherwise
		ShadowRay.shading = 0.f;
	}

#endif
//-----------------------------------------------------------------------------
