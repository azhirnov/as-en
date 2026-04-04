// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	iMode=3 used to detect micro-triangle order.

	Also see [OMM-TriangleSubdiv](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/ray-trace/test-OMM-TriangleSubdiv.as).

	NV order, subdiv=2:
			   [0]
			    0
		     3  1  2
	    14   4  5  7   8
[2]	15  13  12  6  11  9  10  [1]


	NV order, subdiv=3:
								[0]
								 0
							 3   1   2
						14   4   5   7   8
					15  13  12   6  11   9  10
				58  16  17  19  20  30  29  31  32
			59  57  56  18  23  21  22  28  35  33  34
		60  54  53  55  50  24  25  27  46  36  37  39  40
[2]	63  61  62  52  51  49  48  26  47  45  44  38  43  41  42  [1]
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	define SH_RAY_GEN
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		Assert( GetFeatureSet().hasOpacityMicromap(), "require 'opacityMicromap' feature" );

		// initialize
		RC<Image>		rt					= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<RTScene>		scene				= RTScene();
		RC<RTGeometry>	geom				= RTGeometry();
		RC<Buffer>		triangles			= Buffer();												triangles.Name( "Triangles" );
		RC<Buffer>		opacity_data		= Buffer();												opacity_data.Name( "Opacity data" );
		RC<Buffer>		micro_triangles		= Buffer();												micro_triangles.Name( "Micro-triangles" );
		RC<RTMicromap>	micromap			= RTMicromap( EMicromapType::Opacity );
		uint			opacity_data_size	= 0;
		const uint		subdiv_level		= 3;
		const uint		max_tris			= Square( 1 << subdiv_level );

		const uint	max_ray_types = 1;
		scene.MaxRayTypes( max_ray_types );

		// create micromap
		{
			opacity_data_size += micromap.AddTriangleType( /*total count*/1, subdiv_level, EOpacityMicromapFormat::TwoState );

			micromap.SetData( opacity_data );
			micromap.SetTriangles( micro_triangles );
		}

		opacity_data.ArrayLayout(
			"OpacityData",
			"	uint	opacityBits;",
			DivCeil( opacity_data_size, 4 ));

		micro_triangles.ArrayLayout(
			"MicromapTriangle",
			1 );

		// create geometry
		{
			array<float2>	positions	= { float2(0.0f, -0.5f),  float2(0.5f, 0.5f),    float2(-0.5f, 0.5f) };
			array<float3>	colors		= { float3(1.f, 0.f, 0.f), float3(0.f, 1.f, 0.f), float3(0.f, 0.f, 1.f) };
			array<uint>		indices		= { 0, 1, 2 };

			triangles.FloatArray(	"position",		positions );
			triangles.FloatArray(	"colors",		colors );
			triangles.UIntArray(	"indices",		indices );

			geom.AddIndexedTriangles( triangles, "position", triangles, "indices" );
			geom.AddMicromap( micromap );

			scene.AddInstance( geom );
		}

		// render loop
		{
			RC<ComputePass>			pass = ComputePass();
			pass.ArgInOut(	"un_OpacityData",	opacity_data );
			pass.ArgInOut(	"un_TriangleArray",	micro_triangles );
			pass.Slider(	"iMode",			0,	3,			2 );
			pass.Slider(	"iSelect",			0,	max_tris-1 );		// for iMode==3
			pass.Constant(	"iSubdiv",			subdiv_level );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
		}{
			// update micromap
			BuildRTMicromap( micromap );
			BuildRTGeometry( geom );
			BuildRTScene( scene );
		}{
			RC<RayTracingPass>		pass = RayTracingPass();
			pass.ArgOut( "un_OutImage",		rt );
			pass.ArgIn(  "un_RtScene",		scene );
			pass.ArgIn(  "un_Triangles",	triangles );
			pass.Dispatch( rt.Dimension() );

			// setup SBT
			pass.MaxRayTypes( max_ray_types );
			pass.RayGen( RTShader("") );
			pass.RayMiss( RayIndex(0), RTShader("") );
			pass.TriangleHit( RayIndex(0), InstanceIndex(0), RTShader("") );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "Hash.glsl"
	#include "IndirectCmd.glsl"

	void Main ()
	{
		for (uint i = 0; i < un_OpacityData.elements.length(); ++i)
		{
			uint	value = 0;
			switch ( iMode )
			{
				case 1 :	value = ~0u; break;
				case 2 :	value = HEHash11i( i );  break;
				case 3 :
				{
					uint	idx = iSelect - i*32;
					if ( idx < 32 )
						value = ~(1u << idx);
					else
						value = ~0u;
					break;
				}
			}

			un_OpacityData.elements[i].opacityBits = value;
		}

		for (uint i = 0; i < un_TriangleArray.elements.length(); ++i)
		{
			un_TriangleArray.elements[i] = MicromapTriangle_Create( 0, iSubdiv, EOpacityMicromapFormat::TwoState );
		}
	}

#endif
//-----------------------------------------------------------------------------

#define RAY_INDEX	0

//-----------------------------------------------------------------------------
#ifdef SH_RAY_GEN
	#include "InvocationID.glsl"
	#include "HWRayTracing.glsl"

	layout(location=RAY_INDEX) gl::RayPayload float4  payload;

	void Main ()
	{
		HWRay	hwray = HWRay_Create();
		hwray.rayFlags	= gl::RayFlags::None;
		hwray.rayIndex	= RAY_INDEX;
		hwray.rayOrigin	= float3(GetGlobalCoordSNormCorrected() * 1.5f, -1.0f);
		hwray.rayDir	= float3(0.0f, 0.0f, 1.0f);
		hwray.tMin		= 0.0f;
		hwray.tMax		= 10.0f;

		payload = float4(0.0);

		// hitShader = RTSceneBuild::InstanceVk::instanceSBTOffset + hwray.rayIndex

		HWTraceRay( un_RtScene, hwray, /*payload*/RAY_INDEX );

		gl.image.Store( un_OutImage, int2(gl.LaunchID), payload );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_MISS
	layout(location=RAY_INDEX) gl::RayPayloadIn float4  payload;

	void Main ()
	{
		payload = float4(0.0, 0.1, 0.1, 1.0);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_CHIT
	#include "Math.glsl"

	layout(location=RAY_INDEX) gl::RayPayloadIn float4  payload;

	gl::HitAttribute float2  in_HitAttribs;

	ND_ float4  HitShader (const float2 barycentric, const uint primitiveId)
	{
		float3	c0		= un_Triangles.colors[ un_Triangles.indices[ primitiveId * 3 + 0 ]];
		float3	c1		= un_Triangles.colors[ un_Triangles.indices[ primitiveId * 3 + 1 ]];
		float3	c2		= un_Triangles.colors[ un_Triangles.indices[ primitiveId * 3 + 2 ]];
		float3	color	= BaryLerp( c0, c1, c2, barycentric );
		return float4(color, 1.f);
	}

	void Main ()
	{
		payload = HitShader( in_HitAttribs, gl.PrimitiveID );
	}

#endif
//-----------------------------------------------------------------------------
