// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Project omni and spot light into tiles.
	Used in tiled deferred shading.

	related:
	* [frustum culling](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/geom-cull/FrustumCulling.as)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicDim>		dim				= SurfaceSize();
		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, dim );
		RC<Buffer>			tiles_buf		= Buffer();
		RC<FPVCamera>		camera			= FPVCamera();

		RC<DynamicUInt2>	tile_count		= DynamicUInt2();

		// setup camera
		{
			camera.ClipPlanes( 1.0f, 1000.f );
			camera.FovY( 60.f );

			const float	s = 0.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Dimension( dim );
		}

		Slider( tile_count,	"TIleCount",	uint2(10),	uint2(100),		uint2(19,10) );

		RC<DynamicUInt2>	tile_size = dim.XY().Div( tile_count );
		Label( tile_size,	"TileSize" );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( camera );
			pass.Output(	"out_Color",	rt,		RGBA32f(0.0) );
			pass.Constant(	"iTileCount",	tile_count );
			pass.Slider(	"iLightPos",	float3(-1.5, -1.5, 0.0),	float3(1.5, 1.5, 1.0),	float3(0.0, 0.0, 0.6) );
			pass.Slider(	"iLightParams",	float4(0.0),				float4(1.0),			float4(0.1, 0.1, 0.0, 0.0) );
			pass.Slider(	"iLightType",	0,							1,						1 );
			pass.Slider(	"iMode",		0,							2 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "Matrix.glsl"
	#include "Cone.glsl"
	#include "Color.glsl"
	#include "Frustum.glsl"
	#include "Intersectors.glsl"

	int2  NdcToTile (float2 ndc)
	{
		return int2( ToUNorm( ndc ) * float2(iTileCount) );
	}

	int2  NdcToCeilTile (float2 ndc)
	{
		return int2( ToUNorm( ndc ) * float2(iTileCount) + 0.5 );
	}

	int2  FragCoordToTile (float2 coord)
	{
		return int2( coord * un_PerPass.invResolution * float2(iTileCount) );
	}


	uint  PointLight (const int2 tileIdx, const float3 wpos, const float3 viewPos, const float3 rayBegin, const float3 rayEnd)
	{
		Sphere	sp	= Sphere_Create( wpos, iLightParams.x * 2.0 );		// world space
		Ray		ray	= Ray_Create( rayBegin, Normalize(rayEnd - rayBegin), 0.0 );
		ray.t = Length( rayEnd - rayBegin );

		uint	result = 0;
		float2	intersection;	// unused
		result |= (Sphere_Ray_Intersect_v1( sp, ray, OUT intersection ) ? 1 : 0);

		switch ( iMode )
		{
			case 0 :	// project sphere
			{
				float4	aabb_ndc = Sphere_FastProject( Sphere_Create( viewPos, sp.radius ), un_PerPass.camera.proj[0][0], un_PerPass.camera.proj[1][1] );
				int2	min_tile = NdcToTile( aabb_ndc.xy );
				int2	max_tile = NdcToCeilTile( aabb_ndc.zw );

				if ( AllGreaterEqual( tileIdx, min_tile ) and AllLess( tileIdx, max_tile ))
					result |= 2;
				break;
			}

			case 1 :	// is sphere visible for tile frustum
			{
				Frustum		main_fr		= Frustum_Create( un_PerPass.camera.frustum );
				FrustumRays	main_rays	= Frustum_ToRays( main_fr );
				Frustum		tile_fr		= Frustum_FromTile( tileIdx, int2(iTileCount), main_rays, un_PerPass.camera.clipPlanes, un_PerPass.camera.pos );	// world space

				if ( Frustum_IsVisible( tile_fr, sp ))
					result |= 2;
				break;
			}
		}
		return result;
	}


	uint  SpotLight (const int2 tileIdx, const float3 wpos, const float3 viewPos, const float3 rayBegin, const float3 rayEnd)
	{
		Quat	q	= QRotationX( float_Pi );
		q = QMul( q, QRotationZ( float_Pi2 * iLightParams.w ));
		q = QMul( q, QRotationX( float_Pi * iLightParams.z * 0.5 ));
		q = QNormalize( q );

		const Cone	cone	= Cone_Create( viewPos, -QMul(q, float3(0.0, 1.0, 0.0)), float_Pi * iLightParams.y, iLightParams.x * 10.0 );	// world space
		uint		result	= 0;

		// find intersection with cone using sphere tracing
		{
			const float2	sincos_a	= SinCos( cone.halfAngle );
			const float3	view_dir	= Normalize( rayEnd - rayBegin );
			const float		min_d		= 0.001;
			float3			pos			= rayBegin;

			for (int i = 0; i < 128; ++i)
			{
				float3	p = SDF_Move( pos, cone.origin );
						p = SDF_Rotate( p, q );
				float	d = SDF_Cone( p, sincos_a, cone.height );

				pos += d * view_dir;

				if ( Abs(d) < min_d )
				{
					result |= 1;
					break;
				}
			}
		}

		Sphere	sp = Cone_ToBoundingSphere( cone );

		switch ( iMode )
		{
			case 0 :	// project bounding sphere
			{
				float4	aabb_ndc = Sphere_FastProject( sp, un_PerPass.camera.proj[0][0], un_PerPass.camera.proj[1][1] );
				int2	min_tile = NdcToTile( aabb_ndc.xy );
				int2	max_tile = NdcToCeilTile( aabb_ndc.zw );

				if ( AllGreaterEqual( tileIdx, min_tile ) and AllLess( tileIdx, max_tile ))
					result |= 2;
				break;
			}

			case 1 :	// is cone visible for tile frustum
			{
				Frustum		main_fr		= Frustum_Create( un_PerPass.camera.frustum );
				FrustumRays	main_rays	= Frustum_ToRays( main_fr );
				Frustum		tile_fr		= Frustum_FromTile( tileIdx, int2(iTileCount), main_rays, un_PerPass.camera.clipPlanes, un_PerPass.camera.pos );	// world space

				if ( Frustum_IsVisible( tile_fr, cone ))
					result |= 2;
				break;
			}

			case 2 :	// for debugging: draw bounding sphere
			{
				Ray		ray	= Ray_Create( rayBegin, Normalize(rayEnd - rayBegin), 0.0 );
				ray.t = Length( rayEnd - rayBegin );

				float2	intersection2;	// unused
				result |= (Sphere_Ray_Intersect_v1( sp, ray, OUT intersection2 ) ? 2 : 0);
				break;
			}
		}
		return result;
	}


	void  Main ()
	{
		const float3	light_wpos	= UnProjectNDC( un_PerPass.camera.invViewProj, iLightPos );
		const float3	light_vpos	= (un_PerPass.camera.view * float4(light_wpos, 0.0)).xyz;
		const float2	scr_snorm	= ToSNorm( gl.FragCoord.xy * un_PerPass.invResolution );
		const float3	scr_wpos0	= UnProjectNDC( un_PerPass.camera.invViewProj, float3(scr_snorm, 0.0) );
		const float3	scr_wpos1	= UnProjectNDC( un_PerPass.camera.invViewProj, float3(scr_snorm, 0.99) );
		const int2		tile_idx	= FragCoordToTile( gl.FragCoord.xy );

		{
			int2	next_tile_idx	= FragCoordToTile( gl.FragCoord.xy + 1.0 );

			if ( AnyNotEqual( tile_idx, next_tile_idx ))
			{
				// tile border
				out_Color = float4(0.0, 0.0, 0.0, 1.0);
				return;
			}
		}

		out_Color = float4(0.25);

		uint	mode = 0;
		switch ( iLightType )
		{
			case 0 :	mode = PointLight( tile_idx, light_wpos, light_vpos, scr_wpos0, scr_wpos1 );	break;
			case 1 :	mode = SpotLight( tile_idx, light_wpos, light_vpos, scr_wpos0, scr_wpos1 );		break;
		}

		if ( mode == 1 )
			out_Color = float4(1.0, 0.0, 0.0, 1.0);		// error

		if ( mode == 2 )
			out_Color = float4(0.9, 0.7, 0.3, 1.0);		// false positive

		if ( mode == 3 )
			out_Color = float4(0.3, 1.0, 0.3, 1.0);
	}

#endif
//-----------------------------------------------------------------------------
