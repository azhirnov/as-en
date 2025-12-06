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
				iter_cnt = 1<<6;		// NV: must be <= 1024, unroll is too slow
				break;

			case EGPUVendor::Intel :
				dim		 = 1<<9;
				iter_cnt = 1<<6;
				break;

			case EGPUVendor::ARM :			// Mali
				wg_size = uint2(8,8);
				iter_cnt = 1<<4;
				break;

			case EGPUVendor::Qualcomm :		// Adreno
			case EGPUVendor::ImgTech :		// PowerVR
				iter_cnt = 1<<4;
				break;

			case EGPUVendor::Apple :
				dim		 = 2<<10;
				iter_cnt = 1<<8;
				break;
		}

		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, uint2(dim) );
		RC<DynamicUInt>		count		= DynamicUInt();
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicFloat>	ops			= DynamicFloat( float(dim * dim) * float(iter_cnt) * /*half4*/4.0 * /*unroll*/16.0 * /*giga*/1.0e-9 );
		RC<DynamicFloat>	time		= DynamicFloat();
		RC<DynamicFloat>	flops		= ops.Div( time );
		const array<string>	mode_str	= {
			"V4_ADD", "V4_ADD1", "V4_ADD2",
			"V4_MUL", "V4_MUL1",
			"V4_MUL_ADD", "V2_MUL_ADD", "S_MUL_ADD",
			"V4_FMA", "V2_FMA", "S_FMA"
		};

		Slider( mode, 	"iMode", 	0,	mode_str.size()-1, 0 );
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
			pass.Output( "out_Color",	rt,	RGBA32f(0.0) );
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
	#include "InvocationID.glsl"
	#include "CodeTemplates.glsl"

	#define NONE		0
	#define V4_ADD		1
	#define V4_MUL		2
	#define V4_ADD1		3
	#define V4_MUL1		4
	#define V4_FMA		5
	#define V4_MUL_ADD	6
	#define S_FMA		7
	#define V2_FMA		8
	#define V2_MUL_ADD	9
	#define S_MUL_ADD	10
	#define V4_ADD2		11

	#define type		half
	#define type2		half2
	#define type4		half4
	#define itype		sshort

	#define FOR()		[[unroll]] for (int i = 0, cnt = COUNT; i < cnt; ++i)

	#ifdef SH_COMPUTE
	# if defined(AE_ARM_Mali_GPU)
	#	define OUTPUT(x)	gl.image.Store( un_Image, GetGlobalCoord().xy, float4(x) )
	# else
	#	define OUTPUT(x)	if ( AllLess( x, half4(-1800.0hf) )) gl.image.Store( un_Image, GetGlobalCoord().xy, float4(x) )
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

		#elif MODE == V4_ADD1
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

		#elif MODE == V4_ADD2
			type4	a = t;
			type4	b = p;

			// 16 adds
			FOR()
			{
				a += p;  b += p;
				a -= t;  b -= t;
				a += p;  b += p;
				a -= t;  b -= t;
				a += p;  b += p;
				a -= t;  b -= t;
				a += p;  b += p;
				a -= t;  b -= t;
			}
			OUTPUT(Pow( a, b ));

		#elif MODE == V4_MUL1
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

		#elif MODE == V4_ADD
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
				c += p;
				d += t;
				e += p;
				f += t;
				g += p;
				h += t;

				a += b;
				b += c;
				c += d;
				d += e;
				e += f;
				f += g;
				g += h;
				h += a;
			}
			OUTPUT(h);

		#elif MODE == V4_MUL
			type4	a = t;
			type4	b = t * type(0.111);
			type4	c = t * type(0.222);
			type4	d = t * type(0.333);
			type4	e = t * type(0.444);
			type4	f = t * type(0.555);
			type4	g = t * type(0.666);
			type4	h = t * type(0.777);

			// 16 muls
			FOR()
			{
				a *= p;
				b *= t;
				c *= p;
				d *= t;
				e *= p;
				f *= t;
				g *= p;
				h *= t;

				a *= b;
				b *= c;
				c *= d;
				d *= e;
				e *= f;
				f *= g;
				g *= h;
				h *= a;
			}
			OUTPUT(h);

		#elif MODE == V4_MUL_ADD
			type4	a = type4(1.0);

			// 16 muls, 16 adds
			FOR()
			{
				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * t) + p;
				a = (a * t) + p;
				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * t) + p;
				a = (a * t) + p;

				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * t) + p;
				a = (a * t) + p;
				a = (a * p) + t;
				a = (a * p) + t;
				a = (a * t) + p;
				a = (a * t) + p;
			}
			OUTPUT(a);

		#elif MODE == V2_MUL_ADD
			const type2	q	= p.xx;
			const type2	w	= t.zw;
			type2		a	= type2(1.0);

			// 16 muls, 16 adds, 1/2 flops
			FOR()
			{
				a = (a * q) + w;
				a = (a * q) + w;
				a = (a * w) + q;
				a = (a * w) + q;
				a = (a * q) + w;
				a = (a * q) + w;
				a = (a * w) + q;
				a = (a * w) + q;

				a = (a * q) + w;
				a = (a * q) + w;
				a = (a * w) + q;
				a = (a * w) + q;
				a = (a * q) + w;
				a = (a * q) + w;
				a = (a * w) + q;
				a = (a * w) + q;
			}
			OUTPUT(type4(a,a));

		#elif MODE == S_MUL_ADD
			const type	q	= p.x;
			const type	w	= t.y;
			type		a	= type(1.0);

			// 16 muls, 16 adds, 1/4 flops
			FOR()
			{
				a = (a * q) + w;
				a = (a * q) + w;
				a = (a * w) + q;
				a = (a * w) + q;
				a = (a * q) + w;
				a = (a * q) + w;
				a = (a * w) + q;
				a = (a * w) + q;

				a = (a * q) + w;
				a = (a * q) + w;
				a = (a * w) + q;
				a = (a * w) + q;
				a = (a * q) + w;
				a = (a * q) + w;
				a = (a * w) + q;
				a = (a * w) + q;
			}
			OUTPUT(type4(a));

		#elif MODE == V4_FMA
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

		#elif MODE == S_FMA
			const type	q	= p.x;
			const type	w	= t.y;
			type		a	= type(1.0);

			// 16 fma, 1/4 flops
			FOR()
			{
				a = fma( a, q, w );
				a = fma( a, w, q );
				a = fma( a, q, w );
				a = fma( a, w, q );
				a = fma( a, q, w );
				a = fma( a, w, q );
				a = fma( a, q, w );
				a = fma( a, w, q );

				a = fma( a, q, w );
				a = fma( a, w, q );
				a = fma( a, q, w );
				a = fma( a, w, q );
				a = fma( a, q, w );
				a = fma( a, w, q );
				a = fma( a, q, w );
				a = fma( a, w, q );
			}
			OUTPUT(type4(a));

		#elif MODE == V2_FMA
			const type2	q	= p.xx;
			const type2	w	= t.zw;
			type2		a = type2(1.0);

			// 16 fma, 1/2 flops
			FOR()
			{
				a = fma( a, q, w );
				a = fma( a, q, w );
				a = fma( a, w, q );
				a = fma( a, w, q );
				a = fma( a, q, w );
				a = fma( a, q, w );
				a = fma( a, w, q );
				a = fma( a, w, q );

				a = fma( a, q, w );
				a = fma( a, q, w );
				a = fma( a, w, q );
				a = fma( a, w, q );
				a = fma( a, q, w );
				a = fma( a, q, w );
				a = fma( a, w, q );
				a = fma( a, w, q );
			}
			OUTPUT(type4(a,a));

		#else
		#	error
		#endif
	}

#endif
//-----------------------------------------------------------------------------
