// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#	define MODE		0
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, IsDiscreteGPU() ? uint2(4<<10) : uint2(1<<10) );
		RC<DynamicUInt>		count		= DynamicUInt();
		RC<DynamicUInt>		mode		= DynamicUInt();
		const array<string>	mode_str	= {
			"NONE", "ADD", "ADD1", "MUL", "MUL1", "MUL_ADD", "MUL_ADD1", "FMA", "FMA1"
		};

		Slider( mode, 	"Mode", 	0,	mode_str.size()-1, 1 );
		Slider( count,	"Repeat",	1,	32 );

		// render loop
		for (uint i = 0; i < mode_str.size(); ++i)
		{
		#if 1
			RC<ComputePass>	pass = ComputePass( "", "MODE="+mode_str[i] );
			pass.ArgOut( "un_Image",	rt );
			pass.LocalSize( 16, 16 );
			pass.DispatchThreads( rt.Dimension2() );
		#else
			RC<Postprocess>	pass = Postprocess( "", "MODE="+mode_str[i] );
			pass.Output( "out_Color",	rt,	RGBA32f(0.0) );
		#endif
			pass.EnableIfEqual( mode, i );
			pass.Repeat( count );
		}
	//	Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MODE
	//precision mediump float;

	#include "GlobalIndex.glsl"
	#include "CodeTemplates.glsl"

	#define NONE		0
	#define ADD			1
	#define MUL			2
	#define ADD1		3
	#define MUL1		4
	#define FMA			5
	#define FMA1		6
	#define MUL_ADD		7
	#define MUL_ADD1	8

	#define type		float
	#define type4		float4

	#if defined(AE_Qualcomm_Adreno_GPU) || defined(AE_Intel_GPU) || defined(AE_NVidia_GPU) || defined(AE_AMD_GPU) || defined(AE_Apple_GPU)
	#	define FOR()	[[unroll]] for (int i = 0, cnt = COUNT1*COUNT2; i < cnt; ++i)	// NV: must be <= 1024, unroll is too slow
	#elif defined(AE_ARM_Mali_GPU) || defined(AE_IMG_PowerVR_GPU)
	#	define FOR()	for (int i = 0, cnt = COUNT1*COUNT2; i < cnt; ++i)
	#endif

	#ifdef SH_COMPUTE
	# if defined(AE_ARM_Mali_GPU)
	#	define OUTPUT(x)	gl.image.Store( un_Image, GetGlobalCoord().xy, float4(x) )
	# else
	#	define OUTPUT(x)	if ( AllLess( x, float4(-1.e+20) )) gl.image.Store( un_Image, GetGlobalCoord().xy, float4(x) )
	# endif
	#else
	# if defined(AE_ARM_Mali_GPU)
	#	define OUTPUT(x)	out_Color = float4(x);
	# else
	#	define OUTPUT(x)	out_Color = float4(x);  if ( AllGreater( x, float4(-1.e+20) )) Discard();
	# endif
	#endif

	#if defined(AE_NVidia_GPU) || defined(AE_AMD_GPU)
	#	define DIM			(4<<10)
	#	define COUNT1		(1<<3)
	#	define COUNT2		(1<<3)
	#elif defined(AE_Qualcomm_Adreno_GPU)
	#	define DIM			(1<<8)
	#elif defined(AE_Apple_GPU)
	#	define DIM			(1<<10)
	#	define COUNT1		(1<<5)
	#	define COUNT2		(1<<5)
	#elif defined(AE_ARM_Mali_GPU) || defined(AE_IMG_PowerVR_GPU)
	#	define DIM			(1<<10)
	#	define COUNT1		(1<<2)
	#	define COUNT2		(1<<2)
	#elif defined(AE_Intel_GPU)
	#	define DIM			(1<<9)
	#	define COUNT1		(1<<3)
	#	define COUNT2		(1<<3)
	#endif
	// total: RTSize * COUNT1 * COUNT2 * 16 * 4
	//		NV:			68.7 TOp/ms
	//		Adreno:		4290 GOp/ms
	//		Mali,PVR:	1073 GOp/ms


	void  Main ()
	{
		const type4		p = type4(1.0 + GetGlobalCoord().xyyx / DIM * float4(0.5, 0.75, 0.4, 0.9) * 0.1);
		const type4		t = p.xwzy * type4(0.944, 1.209, 0.97, 1.08);

		#if MODE == NONE
			OUTPUT(p);

		#elif MODE == ADD1
			type4	a = t;

			// 16 adds
			FOR()
			{
				a += p;  a -= t;
				a += p;  a -= t;
				a += p;  a -= t;
				a += p;  a -= t;
				a += p;  a -= t;
				a += p;  a -= t;
				a += p;  a -= t;
				a += p;  a -= t;
			}
			OUTPUT(a);

		#elif MODE == MUL1
			type4	a = type4(1.0);

			// 16 muls
			FOR()
			{
				a *= p;  a *= a;
				a *= t;  a *= a;
				a *= p;  a *= a;
				a *= t;  a *= a;
				a *= p;  a *= a;
				a *= t;  a *= a;
				a *= p;  a *= a;
				a *= t;  a *= a;
			}
			OUTPUT(a);

		#elif MODE == ADD
			type4	a = t;
			type4	b = t * type(0.111);
			type4	c = t * type(0.222);
			type4	d = t * type(0.333);
			type4	e = t * type(0.444);
			type4	f = t * type(0.555);
			type4	g = t * type(0.666);
			type4	h = t * type(0.777);

			// 16 adds
			FOR()
			{
				a += p;
				b += t;
				c -= p;
				d -= t;
				e += p;
				f += t;
				g -= p;
				h -= t;

				a -= b;
				b += c;
				c -= d;
				d += e;
				e -= f;
				f += g;
				g -= h;
				h += a;
			}
			OUTPUT(h);

		#elif MODE == MUL
			type4	a = t;
			type4	b = t * type(0.111);
			type4	c = t * type(0.222);
			type4	d = t * type(0.333);
			type4	e = t * type(0.444);
			type4	f = t * type(0.555);
			type4	g = t * type(0.666);
			type4	h = t * type(0.777);

			// 16 muls, 6 ops latency
			FOR()
			{
				a *= p;
				b *= p;
				c *= t;
				d *= t;
				e *= p;
				f *= p;
				g *= t;
				h *= t;

				a *= t;
				b *= t;
				c *= p;
				d *= p;
				e *= t;
				f *= t;
				g *= p;
				h *= p;
			}
			OUTPUT(a+b-c+d-e+f-g+h);

		#elif MODE == MUL_ADD
			type4	a = t;
			type4	b = t * type(0.111);
			type4	c = t * type(0.222);
			type4	d = t * type(0.333);
			type4	e = t * type(0.444);
			type4	f = t * type(0.555);
			type4	g = t * type(0.666);
			type4	h = t * type(0.777);

			// 16 muls, 16 adds
			FOR()
			{
				a = (a * p) + t;
				b = (b * p) + t;
				c = (c * t) + p;
				d = (d * t) + p;
				e = (e * p) + t;
				f = (f * p) + t;
				g = (g * t) + p;
				h = (h * t) + p;

				a = (a * t) + p;
				b = (b * t) + p;
				c = (c * p) + t;
				d = (d * p) + t;
				e = (e * t) + p;
				f = (f * t) + p;
				g = (g * p) + t;
				h = (h * p) + t;
			}
			OUTPUT(a+b-c+d-e+f-g+h);

		#elif MODE == MUL_ADD1
			type4	a = type4(1.0);

			// 16 muls, 16 adds
			FOR()
			{
				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * p) + t;

				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * p) + t;
			}
			OUTPUT(a);

		#elif MODE == FMA
			type4	a = t;
			type4	b = t * type(0.111);
			type4	c = t * type(0.222);
			type4	d = t * type(0.333);
			type4	e = t * type(0.444);
			type4	f = t * type(0.555);
			type4	g = t * type(0.666);
			type4	h = t * type(0.777);

			// 16 fma
			FOR()
			{
				a = fma( a, p, t );
				b = fma( b, p, t );
				c = fma( c, t, p );
				d = fma( d, t, p );
				e = fma( e, p, t );
				f = fma( f, p, t );
				g = fma( g, t, p );
				h = fma( h, t, p );

				a = fma( a, t, p );
				b = fma( b, t, p );
				c = fma( c, p, t );
				d = fma( d, p, t );
				e = fma( e, t, p );
				f = fma( f, t, p );
				g = fma( g, p, t );
				h = fma( h, p, t );
			}
			OUTPUT(a+b-c+d-e+f-g+h);

		#elif MODE == FMA1
			type4	a = type4(1.0);

			// 16 fma
			FOR()
			{
				a = fma( a, p, t );
				a = fma( a, p, t );
				a = fma( a, t, p );
				a = fma( a, t, p );
				a = fma( a, p, t );
				a = fma( a, p, t );
				a = fma( a, t, p );
				a = fma( a, t, p );

				a = fma( a, p, t );
				a = fma( a, p, t );
				a = fma( a, t, p );
				a = fma( a, t, p );
				a = fma( a, p, t );
				a = fma( a, p, t );
				a = fma( a, t, p );
				a = fma( a, t, p );
			}
			OUTPUT(a);

		#else
		#	error
		#endif
	}

#endif
//-----------------------------------------------------------------------------
