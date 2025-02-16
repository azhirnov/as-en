// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define CLEAR_ACCUM
#	define FRONT_VIEW
#	define MIPMAP_1
#	define MIPMAP_2
#	define MIPMAP_3
#	define FINAL
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-2" );
		RC<Image>			light_accum		= Image( EPixelFormat::R32U,		SurfaceSize() );	light_accum.Name( "Light Accum" );
		RC<Image>			light_buf_0		= Image( EPixelFormat::R32U,		SurfaceSize() );	light_accum.Name( "Light buffer" );

		// RG - min/max, B - average, A - count
		const uint			k_size			= 8;
		const uint			k2_size			= 64;
		RC<Image>			light_buf_1		= Image( EPixelFormat::RGBA16F,		SurfaceSize().Div( k_size ));	light_buf_1.Name( "Light buffer /8" );
		RC<Image>			light_buf_2		= Image( EPixelFormat::RGBA16F,		SurfaceSize().Div( k2_size ));	light_buf_2.Name( "Light buffer /64" );
		RC<Image>			light_buf_3		= Image( EPixelFormat::RGBA16F,		uint2(1,1) );					light_buf_3.Name( "Light buffer 1x1" );

		const uint			accum_size		= 16;

		RC<DynamicFloat>	cone_radius		= DynamicFloat();
		RC<DynamicFloat>	inv_light_speed	= DynamicFloat();

		const array<float>	params = {
			90, 					// iMaxIter
			2, 						// iLensCount
			20.0000f, 				// iViewPlane
			0.0000f, -0.2830f, 		// iLens1Pos
			-0.5810f, 				// iLens1Rot
			0.2520f, 0.5120f, 		// iLens2Pos
			-0.2450f, 				// iLens2Rot

			1.9290f, 				// iConeRadius
			0.0160f, 				// iInvLightSpeed

			1, 						// iMode
			0.2330f, 				// iLerp
		};

		Slider(	cone_radius,		"iConeRadius",		0.01f,		4.f,	params[9] );
		Slider( inv_light_speed,	"iInvLightSpeed",	0.001f,		1.f,	params[10] );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "CLEAR_ACCUM;FCOUNT=" + (accum_size * accum_size) );
			pass.ArgInOut(	"un_LightAccum",	light_accum );
			pass.ArgInOut(	"un_LightBuf",		light_buf_0 );

			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( light_accum.Dimension() );
		}{
			RC<ComputePass>		pass = ComputePass( "", "FRONT_VIEW;FCOUNT=" + accum_size );

			pass.ArgInOut(	"un_LightAccum",	light_accum );

			pass.Slider(	"iMaxIter",			0,				128,			int(params[0]) );
			pass.Slider( 	"iLensCount",		1,				2,				int(params[1]) );
			pass.Slider(	"iViewPlane",		-4.f,			20.f,			params[2] );
			pass.Slider(	"iLens1Pos",		float2(-1.f),	float2(1.f),	float2(params[3], params[4]) );
			pass.Slider(	"iLens1Rot",		-1.f,			1.f,			params[5] );
			pass.Slider(	"iLens2Pos",		float2(-1.f),	float2(1.f),	float2(params[6], params[7]) );
			pass.Slider(	"iLens2Rot",		-1.f,			1.f,			params[8] );

			pass.Constant(	"iConeRadius",		cone_radius		);
			pass.Constant(	"iInvLightSpeed",	inv_light_speed	);

			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( light_accum.Dimension() );
		}{
			RC<ComputePass>		pass = ComputePass( "", "MIPMAP_1;SIZE=" + k_size );

			pass.ArgIn(		"un_LightBuf",	light_buf_0 );
			pass.ArgOut(	"un_Dst",		light_buf_1 );

			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( light_buf_1.Dimension() );
		}{
			RC<ComputePass>		pass = ComputePass( "", "MIPMAP_2;SIZE=" + k_size );

			pass.ArgIn(		"un_Src",	light_buf_1 );
			pass.ArgOut(	"un_Dst",	light_buf_2 );

			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( light_buf_2.Dimension() );
		}{
			RC<ComputePass>		pass = ComputePass( "", "MIPMAP_3" );

			pass.ArgIn(		"un_Src",	light_buf_2 );
			pass.ArgOut(	"un_Dst",	light_buf_3 );

			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "FINAL" );

			pass.ArgIn(		"un_LightBuf0",		light_buf_0 );
			pass.ArgIn(		"un_LightBuf1",		light_buf_1,	Sampler_LinearClamp );
			pass.ArgIn(		"un_LightBuf2",		light_buf_2,	Sampler_LinearClamp );
			pass.ArgIn(		"un_LightBuf3",		light_buf_3,	Sampler_NearestClamp );

			pass.Output(	"out_Color",		rt,			RGBA32f(0.0) );

			pass.Slider(	"iMode",	0,		2,		int(params[11]) );
			pass.Slider(	"iLerp",	0.f,	1.f,	params[12] );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CLEAR_ACCUM
	#include "InvocationID.glsl"

	void  Main ()
	{
		if ( un_PerPass.frame % FCOUNT == 0 )
		{
			int2	coord	= GetGlobalCoord().xy;
			uint4	color	= gl.image.Load( un_LightAccum, coord );

			gl.image.Store( un_LightAccum, coord, uint4(0) );
			gl.image.Store( un_LightBuf, coord, color );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef FRONT_VIEW
	#include "InvocationID.glsl"
	#include "SDF.glsl"

	struct LightRay
	{
		float3	dir;		// normalized direction
		float3	pos;		// current position
		float	path;
	};

	float3	c_LensPos1	= float3(iLens1Pos, 0.f);
	float3	c_LensPos2	= float3(iLens2Pos, 0.f);


	float  DistToLens (float3 rayPos, const float3 lensPos, float angle)
	{
		rayPos = SDF_Move( rayPos, lensPos );
		rayPos.xy = SDF_Rotate2D( rayPos.xy, angle * float_Pi );

		return Min( SDF_Sphere( rayPos, 0.05f ),
					SDF_Ellipsoid( rayPos, float3( 0.1f, 0.01f, 0.1f ) ));
	}

	float  Scene (const float3 rayPos)
	{
		float	d = DistToLens( rayPos, c_LensPos1, iLens1Rot );

		if ( iLensCount > 1 )
			d = Min( d, DistToLens( rayPos, c_LensPos2, iLens2Rot ));

		return d;
	}


	float3  GetGravityForce (const float3 prev, const float3 pos, const float3 lensPos, float angle)
	{
		float	r	= DistToLens( pos, lensPos, angle );
		float	g	= 1.0 / (r * r);
		float	dt	= r * iInvLightSpeed;		// distance / speed of light
		float3	dir	= Normalize( lensPos - pos );

		return (dir * g * dt * dt * 0.5);
	}

	float3  ApplyGravity (const float3 prev, float3 pos)
	{
		pos += GetGravityForce( prev, pos, c_LensPos1, iLens1Rot );

		if ( iLensCount > 1 )
			pos += GetGravityForce( prev, pos, c_LensPos2, iLens2Rot );

		return pos;
	}


	void  Main ()
	{
		LightRay	ray;
		{
			ray.dir		= float3(0.f, 0.f, 1.f);
			ray.pos		= float3( GetGlobalCoord().xy, -1.0f );
			ray.pos.xy	+= float2( un_PerPass.frame % FCOUNT, (un_PerPass.frame / FCOUNT) % FCOUNT ) / float(FCOUNT);
			ray.pos.xy	= ToSNorm( ray.pos.xy / float2(GetGlobalSize().xy) );
			ray.pos.xy	*= iConeRadius;
			ray.path	= 0.f;
		}

		const float	min_dist	= 0.001f;
		const float	max_dist	= 0.5f;

		for (uint i = 0; i < iMaxIter; ++i)
		{
			float	dist = Scene( ray.pos );
			float3	prev = ray.pos;

			if ( dist < 0.000001f )
				return;

			dist = Clamp( dist * 0.5f, min_dist, max_dist );

			ray.pos  = ApplyGravity( prev, FusedMulAdd( ray.dir, float3(dist), ray.pos ));

			ray.path += Distance( ray.pos, prev );
			ray.dir   = Normalize( ray.pos - prev );

		//	if ( prev.z > ray.pos.z )
		//		return;

			if ( (prev.z < iViewPlane) and (ray.pos.z >= iViewPlane) )
			{
				float f = (iViewPlane - prev.z) / (ray.pos.z - prev.z);
				ray.pos = Lerp( prev, ray.pos, Saturate(f) );
				break;
			}
		}

		const int2	coord = int2( ToUNorm( ray.pos.xy ) * float2(gl.image.GetSize( un_LightAccum )) );
		gl.image.AtomicAdd( un_LightAccum, coord, 1 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MIPMAP_1
	#include "InvocationID.glsl"

	void  Main ()
	{
		float4		result	= float4( float_max, 0.f, 0.f, 0.f );	// min/max, average, px count
		const int2	coord	= GetGlobalCoord().xy;

		for (int y = 0; y < SIZE; ++y)
		for (int x = 0; x < SIZE; ++x)
		{
			float	c = float(gl.image.Load( un_LightBuf, coord*SIZE + int2(x,y) ).r);

			result.r = Min( result.r, c );
			result.g = Max( result.g, c );
			result.b += c;
			result.a += (c > 0.f ? 1.f : 0.f);
		}

		result.b /= result.a;
		result = Clamp( result, 0.f, 65504.0f );	// half max

		gl.image.Store( un_Dst, coord, result );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MIPMAP_2
	#include "InvocationID.glsl"

	void  Main ()
	{
		float4		result	= float4( float_max, 0.f, 0.f, 0.f );	// min/max, average, px count
		const int2	coord	= GetGlobalCoord().xy;

		for (int y = 0; y < SIZE; ++y)
		for (int x = 0; x < SIZE; ++x)
		{
			float4	c = gl.image.Load( un_Src, coord*SIZE + int2(x,y) );

			result.r = Min( result.r, c.r );
			result.g = Max( result.g, c.g );
			result.b += c.b * c.a;
			result.a += c.a;
		}

		result.b /= result.a;
		result = Clamp( result, 0.f, 65504.0f );	// half max

		gl.image.Store( un_Dst, coord, result );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MIPMAP_3
	#include "Math.glsl"

	void  Main ()
	{
		float4		result	= float4( float_max, 0.f, 0.f, 0.f );	// min/max, average, px count
		const int2	size	= gl.image.GetSize( un_Src );

		for (int y = 0; y < size.y; ++y)
		for (int x = 0; x < size.x; ++x)
		{
			float4	c = gl.image.Load( un_Src, int2(x,y) );

			result.r = Min( result.r, c.r );
			result.g = Max( result.g, c.g );
			result.b += c.b * c.a;
			result.a += c.a;
		}

		result.b /= result.a;
		result = Clamp( result, 0.f, 65504.0f );	// half max

		gl.image.Store( un_Dst, int2(0), result );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef FINAL
	#include "InvocationID.glsl"
	#include "Color.glsl"

	void  Main ()
	{
		float2	uv		= GetGlobalCoordUNorm().xy;
		float4	buf1x1	= gl.texture.Fetch( un_LightBuf3, int2(0), 0 );
		float4	buf64	= gl.texture.Sample( un_LightBuf2, uv );
		float4	buf8	= gl.texture.Sample( un_LightBuf1, uv );
		uint	samples	= gl.image.Load( un_LightBuf0, int2(gl.FragCoord.xy) ).r;

		// denoise
		float	light	= 0.f;
		{
			// high density
			if ( buf8.a > 50.f and iMode <= 0 )
			{
				light = float(samples);
			}
			else
			// low density
			if ( buf8.a > 16.f and iMode <= 1 )
			{
				light = buf8.b;
			}
			else
			// very low density
			{
				light = buf64.b;
			}

			float	min_l = buf1x1.r;
			float	max_l = Lerp( Max( 0.f, buf1x1.b - min_l ), buf1x1.g - min_l, iLerp );

			light = (light - min_l) / max_l;
		}

		out_Color = Rainbow( 1.0 - light ) * Saturate(light + 0.1);

		if ( light > 1.0 )
			out_Color = float4(1.0);

		out_Color.a = buf8.a;
	}

#endif
//-----------------------------------------------------------------------------
