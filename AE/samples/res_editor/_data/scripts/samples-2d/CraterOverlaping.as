// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#	define SPLINE
#	define BLEND
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicFloat4>	s_a			= DynamicFloat4();
		RC<DynamicFloat4>	s_b			= DynamicFloat4();
		RC<DynamicFloat4>	s_p01		= DynamicFloat4();
		RC<DynamicFloat4>	s_p23		= DynamicFloat4();
		RC<DynamicUInt>		s_mode		= DynamicUInt();
		RC<DynamicFloat2>	s_off		= DynamicFloat2();
		RC<DynamicFloat>	s_scale		= DynamicFloat();


		{
			const float			min	= -2.f;
			const float			max = 3.f;
			const array<float>	params	= {
				0, 											// Spline
				0.1340f, 0.2470f, 0.1610f, 0.2490f, 		// A
				0.6210f, 0.5400f, 0.6080f, -1.0000f, 		// B
				0.8400f, 0.0400f, 							// Off
				1.9050f, 									// Scale
				0.47, 0.67, 0.2, 0.99,						// P01
				0.58, 1.0, 0.825, 1.2,						// P23
			};
			uint	i = 0;

			Slider( mode,		"View",		0,					1 );
			Slider( s_mode,		"Spline",	0,					5,					int(params[i]) );												i += 1;
			Slider( s_a,		"A",		float4(-1.f),		float4(2.f),		float4( params[i], params[i+1], params[i+2], params[i+3] ));	i += 4;
			Slider( s_b,		"B",		float4(-1.f),		float4(2.f),		float4( params[i], params[i+1], params[i+2], params[i+3] ));	i += 4;
			Slider( s_off,		"Off",		float2(0.f, -0.1f),	float2(1.f, 0.1f),	float2( params[i], params[i+1] ));								i += 2;
			Slider( s_scale,	"Scale",	0.5f,				2.f,				params[i] );													i += 1;
			Slider( s_p01,		"P01",		float4(0.f),		float4(1.f),		float4( params[i], params[i+1], params[i+2], params[i+3] ));	i += 4;
			Slider( s_p23,		"P23",		float4(0.f),		float4(1.f),		float4( params[i], params[i+1], params[i+2], params[i+3] ));	i += 4;
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "SPLINE" );
			pass.Output( "out_Color",	rt );
			pass.Constant( "iMode",		s_mode );
			pass.Constant( "iA",		s_a );
			pass.Constant( "iB",		s_b );
			pass.Constant( "iOff",		s_off );
			pass.Constant( "iScale",	s_scale );
			pass.Constant( "iP01",		s_p01 );
			pass.Constant( "iP23",		s_p23 );
			pass.Slider( "iRatio",		1.0f,	4.0f,	2.2f );
			pass.Slider( "iCurve",		0,	4 );
			pass.EnableIfEqual( mode, 1 );
		}{
			const array<float>	params = {
				1, 1, 1, 								// iEnable
				1.9720f, 0.6700f, 						// iHScaleBias
				0.2860f, 0.0000f, 0.8590f, 				// iC0
				-0.2320f, 0.0000f, 0.3230f, 			// iC1
				-0.4200f, 0.3750f, 0.3940f, 			// iC2
			};
			uint	i = 0;

			RC<Postprocess>		pass = Postprocess( "", "BLEND" );
			pass.Output( "out_Color",	rt );
			pass.Slider( "iView",		0,							3,							3 );
			pass.Slider( "iBlend",		0,							6,							3 );
			pass.Slider( "iEnable",		int3(0),					int3(1),					int3( int(params[i]), int(params[i+1]), int(params[i+2]) ));	i += 3;
			pass.Slider( "iHScaleBias",	float2(0.1f, -1.f),			float2(10.f, 1.f),			float2( params[i], params[i+1] ));								i += 2;
			pass.Slider( "iC0",			float3(-1.f, -1.f, 0.f),	float3(1.f, 1.f, 1.f),		float3( params[i], params[i+1], params[i+2] ));					i += 3;
			pass.Slider( "iC1",			float3(-1.f, -1.f, 0.f),	float3(1.f, 1.f, 1.f),		float3( params[i], params[i+1], params[i+2] ));					i += 3;
			pass.Slider( "iC2",			float3(-1.f, -1.f, 0.f),	float3(1.f, 1.f, 1.f),		float3( params[i], params[i+1], params[i+2] ));					i += 3;
			pass.Constant( "iMode",		s_mode );
			pass.Constant( "iA",		s_a );
			pass.Constant( "iB",		s_b );
			pass.Constant( "iOff",		s_off );
			pass.Constant( "iScale",	s_scale );
			pass.Constant( "iP01",		s_p01 );
			pass.Constant( "iP23",		s_p23 );
			pass.EnableIfEqual( mode, 0 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	const float		c_SplineMinHeight	= -0.75;

#endif
//-----------------------------------------------------------------------------
#ifdef SPLINE
	#include "SDF.glsl"
	#include "InvocationID.glsl"
	#include "tools/SplineHelper.glsl"

	float  Graph (float x)
	{
		float	s = ApplySpline2( Saturate( x * iScale - iOff.x ), iMode, iA, iB ).x;
		s = 1.0 - Saturate( s - iOff.y );
		return s;
	}

	float  Graph1 (float x) { return 1.0 - SmoothStep( x, iP01.x, iP01.y ); }
	float  Graph2 (float x) { return 1.0 - SmoothStep( x, iP01.z, iP01.w ); }
	float  Graph3 (float x) { return 1.0 - SmoothStep( x, iP23.x, iP23.y ); }
	float  Graph4 (float x) { return 1.0 - SmoothStep( x, iP23.z, iP23.w ); }

	float  Wave (float x) { return TriangleWave( x * 50.0 ) * 1.5; }


	void Main ()
	{
		const float		a	= 1.02;
		const float2	uv	= GetGlobalCoordSNorm().xy * float2(a, iRatio);
		const float		w	= 0.002f * iRatio;

		out_Color = float4(0.25);

		if ( ! IsSNorm( uv ))
			out_Color = float4(0.0);

		if ( uv.y > 0.0 )	out_Color.rb *= 0.75;
		else				out_Color.gb *= 0.75;
		if ( uv.x < 0.0 )	out_Color.rgb *= 0.8;

		// graph
		{
			float2	p0, p1, p;
			p.x = p0.x = uv.x;
			p1.x = GetGlobalCoordSNorm( int3(1) ).x * a;
			p.y  = ToUNorm( uv.y );

			float	d2 = float_max;
			switch ( iCurve )
			{
				case 1 :
				{
					p0.y = Graph1( Abs( p0.x ));
					p1.y = Graph1( Abs( p1.x ));
					d2 = SDF2_Line( p, p0, p1 );
					break;
				}
				case 2 :
				{
					p0.y = Graph2( Abs( p0.x ));
					p1.y = Graph2( Abs( p1.x ));
					d2 = SDF2_Line( p, p0, p1 );
					break;
				}
				case 3 :
				{
					p0.y = Graph3( Abs( p0.x ));
					p1.y = Graph3( Abs( p1.x ));
					d2 = SDF2_Line( p, p0, p1 );
					break;
				}
				case 4 :
				{
					p0.y = Graph4( Abs( p0.x ));
					p1.y = Graph4( Abs( p1.x ));
					d2 = SDF2_Line( p, p0, p1 );
					break;
				}
			}

			p0.y = Graph( Abs( p0.x ));
			p1.y = Graph( Abs( p1.x ));

			float	d = SDF2_Line( p, p0, p1 );
			out_Color.rgb = Lerp( float3(1.0, 0.0, 0.0), out_Color.rgb, Saturate(d/w) );

			out_Color.rgb = Lerp( float3(0.0, 1.0, 0.0), out_Color.rgb, Saturate(d2/w) );
		}

		// grid
		{
			float2	uv3 = uv * float2(5.0, 2.0);
			out_Color.rgb *= AA_QuadGrid_dxdy( uv3, float2(1.0, 2.5) ).x;
			out_Color.rgb *= AA_QuadGrid_dxdy( uv3 * 4.0, float2(0.0, 1.5) ).x;
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BLEND
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "Easing.glsl"
	#include "Normal.glsl"
	#include "Geometry.glsl"
	#include "InvocationID.glsl"
	#include "tools/SplineHelper.glsl"

	#define MAX_POINTS		4

	struct PixHistory
	{
		float	h;		// height
		float	nd;		// normalized distance [0, 1]
		float	d;		// distance in pixels
		uint	count;
		float4	center_mh_r [MAX_POINTS];	// crater center, min height, radius
	};
	PixHistory	g_PixHistory;


	void  InitPixHistory ()
	{
		g_PixHistory.h		= 0.0;
		g_PixHistory.nd		= 10.0;
		g_PixHistory.d		= float_max;
		g_PixHistory.count	= 0;
	}


	void  AddToPixHistory (float h, const float nd, const float d, const float2 center, const float mh, const float r)
	{
		const uint	idx = g_PixHistory.count++;

		if ( idx >= MAX_POINTS )
			return;

		const float	c_Params [] = {
			iP01.x, iP01.y, iP01.z, iP01.w,
			iP23.x, iP23.y, iP23.z, iP23.w
		};

		// calculate depth for crater center
		{
			float	a = 1.0 - SmoothStep( nd, c_Params[0], c_Params[1] );

			for (uint i = 0; i < idx; ++i)
			{
				float	d = Distance( g_PixHistory.center_mh_r[i].xy, center ) / g_PixHistory.center_mh_r[i].w;
				float	h1 = (1.0 - SmoothStep( d, c_Params[2], c_Params[3] )) * g_PixHistory.center_mh_r[i].z;
				h += h1 * a;
			}
			g_PixHistory.center_mh_r[idx] = float4( center, mh, r );
		}

		// to find incorrect spline, this will break normals
		h = (nd > 1.0 ? 0.0 : h);

		float	f;
		{
			f = Saturate( RemapSrc( float2(c_Params[4], c_Params[5]), nd ));
			float	b = 1.0 - SmoothStep( g_PixHistory.nd, c_Params[6], c_Params[7] );
			f = Max( f, f*b );
		}

		switch ( iBlend )
		{
			case 0 :
			{
				g_PixHistory.h  += h;
				g_PixHistory.nd = Min( g_PixHistory.nd, nd );
				g_PixHistory.d  = Min( g_PixHistory.d, d );
				return;
			}

			case 1 :
			{
				if ( nd < 1.0 )
				{
					g_PixHistory.h	= h;
					g_PixHistory.nd	= nd;
					g_PixHistory.d	= d;
				}
				return;
			}

			case 2 :	f = HermiteEaseInOut( f );		break;
			case 3 :	f = QuadraticEaseInOut( f );	break;
			case 4 :	f = CubicEaseInOut( f );		break;
			case 5 :	f = CircularEaseInOut( f );		break;

			case 6 :
			{
				f = QuadraticEaseInOut( f );
				g_PixHistory.h  = Bezier3( h,  h + g_PixHistory.h,   g_PixHistory.h,  f );
				g_PixHistory.nd = Bezier3( nd, nd + g_PixHistory.nd, g_PixHistory.nd, f );
				g_PixHistory.d  = Bezier3( d,  d + g_PixHistory.d,   g_PixHistory.d,  f );
				return;
			}
		}

		if ( iBlend >= 2 )
		{
			g_PixHistory.h  = Lerp( h,  g_PixHistory.h,  f );
			g_PixHistory.nd = Lerp( nd, g_PixHistory.nd, f );
			g_PixHistory.d  = Lerp( d,  g_PixHistory.d,  f );
		}
	}


	void  Crater_v2 (float2 pos, float2 center, float r)
	{
		pos -= center;

		float	d  = Length( pos );
		float	nd = d / r;
		float	s;

		s = ApplySpline2( Saturate( nd * iScale - iOff.x ), iMode, iA, iB ).x;
		s = Saturate( s - iOff.y );

		AddToPixHistory( ToSNorm( s ) * r, nd, d, center, c_SplineMinHeight*r, r );
	}

	void  Crater (float2 pos, float3 center_radius)
	{
		Crater_v2( pos, center_radius.xy, center_radius.z );
	}


	void  Main ()
	{
		InitPixHistory();

		float2	pos = GetGlobalCoordSNormCorrected() * 1.5;

		if ( iEnable.x == 1 )
			Crater( pos, iC0 );

		if ( iEnable.y == 1 )
			Crater( pos, iC1 );

		if ( iEnable.z == 1 )
			Crater( pos, iC2 );

		float	h = g_PixHistory.h;
		h = h * iHScaleBias.x + iHScaleBias.y;

		float3	norm = ComputeNormalInWS_quadSg( float3( pos, h ));
		float3	light_dir = float3(0.f, 1.f, -0.4f);

		if ( un_PerPass.mouse.z > 0 )
		{
			float2	m	= un_PerPass.mouse.xy;
			light_dir	= UVtoSphereNormal( -ToSNorm( m )).xyz;
			light_dir.z = -light_dir.z;
		}
		light_dir = Normalize( light_dir );
		float	nl = Dot( norm, light_dir );

		switch ( iView )
		{
			case 0 :
				out_Color = Rainbow( ToUNorm( -h )) * TriangleWave( h * 10.0 );  break;

			case 1 :
				out_Color.rgb = ToUNorm( norm );  break;

			case 2 :
				out_Color = float4( nl );  break;

			case 3 :
				out_Color = Rainbow( ToUNorm( -h )) * nl;  break;
		}
		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
