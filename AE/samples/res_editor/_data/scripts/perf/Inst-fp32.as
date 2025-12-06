// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define MODE		0
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		uint	dim			= 1<<10;
		uint	iter_cnt	= 32;
		uint2	wg_size		= uint2( 16, 16 );

		switch ( GPUVendor() )
		{
			case EGPUVendor::NVidia :
			case EGPUVendor::AMD :
				dim		 = 4<<10;
				iter_cnt = 1<<6;			// NV: must be <= 1024, unroll is too slow
				break;

			case EGPUVendor::Intel :
				dim		 = 1<<9;
				iter_cnt = 1<<6;
				break;

			case EGPUVendor::ARM :			// Mali
				wg_size = uint2(8,8);
				dim		 = 1<<10;
				iter_cnt = 1<<8;
				break;

			case EGPUVendor::Qualcomm :		// Adreno
			case EGPUVendor::ImgTech :		// PowerVR
				dim		 = 1<<10;
				iter_cnt = 1<<8;
				break;

			case EGPUVendor::Apple :
				dim		 = 2<<10;
				iter_cnt = 1<<8;
				break;
		}

		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, uint2(dim) );
		RC<DynamicUInt>		count		= DynamicUInt();
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicFloat>	ops			= DynamicFloat( float(dim * dim) * float(iter_cnt) * /*float4*/4.0 * /*unroll*/16.0 * /*giga*/1.0e-9 );
		RC<DynamicFloat>	time		= DynamicFloat();
		RC<DynamicFloat>	flops		= ops.Div( time );
		const array<string>	mode_str	= {
			"NONE",
			"ADD", "ADD1", "ADD2",
			"MUL", "MUL1",
			"MUL_ADD", "MUL_ADD1",
			"FMA", "FMA1", "FMA2"
		};

		Slider( mode, 	"Mode", 	0,	mode_str.size()-1, 1 );
		Slider( count,	"Repeat",	1,	32 );
		Label(  flops,	"GOPS" );	// x2 for FMA and MulAdd

		// render loop
		for (uint i = 0; i < mode_str.size(); ++i)
		{
		#if 1
			RC<ComputePass>	pass = ComputePass( "", "MODE="+mode_str[i]+";  DIM="+dim+";COUNT="+iter_cnt );
			pass.ArgOut( "un_Image",	rt );
			pass.LocalSize( wg_size );
			pass.DispatchThreads( rt.Dimension2() );
		#else
			RC<Postprocess>	pass = Postprocess( "", "MODE="+mode_str[i]+";  DIM="+dim+";COUNT="+iter_cnt );
			pass.OutputLS( "out_Color",	rt, EAttachmentLoadOp::Invalidate, EAttachmentStoreOp::Invalidate );
		#endif
			pass.EnableIfEqual( mode, i );
			pass.Repeat( count );
			pass.MeasureTime( time );
		}
	//	Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MODE
	//precision mediump float;

	#include "InvocationID.glsl"
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
	#define ADD2		9
	#define FMA2		10

	#define type		float
	#define type4		float4

	#define FOR()		[[unroll]] for (int i = 0, cnt = COUNT; i < cnt; ++i)

	#ifdef SH_COMPUTE
	# if defined(AE_ARM_Mali_GPU)
	#	define OUTPUT(x)	gl.image.Store( un_Image, GetGlobalCoord().xy, float4(x) )
	# else
	#	define OUTPUT(x)	if ( AllLess( x, float4(-1.e+20) )) gl.image.Store( un_Image, GetGlobalCoord().xy, float4(x) )
	# endif
	#else
	#	define OUTPUT(x)	out_Color = Saturate(float4(x)) * 0.001;	// for high compression
	#endif

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

		#elif MODE == ADD2
			const type4	p0 = p * 1.1234;
			const type4	t0 = t * 0.8463;

			type4	a = t;
			type4	b = p;

			// 16 adds
			FOR()
			{
				a += p;  b += p0;
				a -= t;  b -= t0;
				a += p;  b += p0;
				a -= t;  b -= t0;
				a += p;  b += p0;
				a -= t;  b -= t0;
				a += p;  b += p0;
				a -= t;  b -= t0;
			}
			OUTPUT(a*b);

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
				a = fma( a, p, t );		a = fma( a, p, t );
				a = fma( a, t, p );		a = fma( a, t, p );
				a = fma( a, p, t );		a = fma( a, p, t );
				a = fma( a, t, p );		a = fma( a, t, p );
				a = fma( a, p, t );		a = fma( a, p, t );
				a = fma( a, t, p );		a = fma( a, t, p );
				a = fma( a, p, t );		a = fma( a, p, t );
				a = fma( a, t, p );		a = fma( a, t, p );
			}
			OUTPUT(a);

		#elif MODE == FMA2
			type4	a = type4(1.23);
			type4	b = type4(2.11);

			// 16 fma
			FOR()
			{
				a = fma( a, p, t );		b = fma( b, p, t );
				a = fma( a, t, p );		b = fma( b, t, p );
				a = fma( a, p, t );		b = fma( b, p, t );
				a = fma( a, t, p );		b = fma( b, t, p );
				a = fma( a, p, t );		b = fma( b, p, t );
				a = fma( a, t, p );		b = fma( b, t, p );
				a = fma( a, p, t );		b = fma( b, p, t );
				a = fma( a, t, p );		b = fma( b, t, p );
			}
			OUTPUT(a-b);

		#else
		#	error
		#endif
	}

#endif
//-----------------------------------------------------------------------------
