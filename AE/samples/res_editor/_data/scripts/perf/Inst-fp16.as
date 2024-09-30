// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Mali dim:		1<<7 .. 1<<10
	PowerVR dim:	1<<10
*/
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
			"V4_ADD", "V4_ADD1", "V4_MUL", "V4_MUL1",
			"V4_MUL_ADD", "V2_MUL_ADD", "S_MUL_ADD",
			"V4_FMA", "V2_FMA", "S_FMA"
		};

		Slider( mode, 	"iMode", 	0,	mode_str.size()-1, 0 );
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
	#include "GlobalIndex.glsl"
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

	#define UNROLL1		//[[unroll]]	// too slow during pipeline creation
	#define UNROLL2		[[unroll]]

	#define type		half
	#define type2		half2
	#define type4		half4
	#define itype		sshort

	#if defined(AE_Qualcomm_Adreno_GPU) || defined(AE_Intel_GPU) || defined(AE_NVidia_GPU)
	#	define FOR()	[[unroll]] for (itype i = itype(0), cnt = itype(COUNT1*COUNT2); i < cnt; ++i)	// NV: must be <= 1024, unroll is too slow
	#elif defined(AE_ARM_Mali_GPU) || defined(AE_IMG_PowerVR_GPU)
	#	define FOR()	for (itype i = itype(0), cnt = itype(COUNT1*COUNT2); i < cnt; ++i)

	#elif 0
	#	define FOR()	UNROLL1 for (type i = type(0.0), cnt = type(COUNT1); i < cnt; ++i)		UNROLL2 for (type j = type(0.0); j < type(COUNT2); ++j)
	#elif 0
	#	define FOR()	UNROLL1 for (itype i = itype(0); i < itype(COUNT1); ++i)				UNROLL2 for (itype j = itype(0); j < itype(COUNT2); ++j)
	#elif 0
	//#	define FOR()	UNROLL2 for (itype i = itype(0), cnt = itype(COUNT1*COUNT2); i < cnt; ++i)
	#	define FOR()	UNROLL2 for (type  i = type(0),  cnt = type(COUNT1*COUNT2);  i < cnt; ++i)
	#endif

	#ifdef SH_COMPUTE
	# if defined(AE_ARM_Mali_GPU)
	#	define OUTPUT(x)	gl.image.Store( un_Image, GetGlobalCoord().xy, float4(x) )
	# else
	#	define OUTPUT(x)	if ( AllLess( x, half4(-1800.0hf) )) gl.image.Store( un_Image, GetGlobalCoord().xy, float4(x) )
	# endif
	#else
	# if defined(AE_ARM_Mali_GPU)
	#	define OUTPUT(x)	out_Color = float4(x);
	# else
	#	define OUTPUT(x)	out_Color = float4(x);  if ( AllGreater( x, half4(-1800.0hf) )) Discard();
	# endif
	#endif

	#ifdef AE_NVidia_GPU
	#	define DIM			(4<<10)
	#	define COUNT1		(1<<3)
	#	define COUNT2		(1<<3)
	#elif defined(AE_Qualcomm_Adreno_GPU)
	#	define DIM			(2<<10)
	#	define COUNT1		(1<<2)
	#	define COUNT2		(1<<2)
	#elif defined(AE_ARM_Mali_GPU) || defined(AE_IMG_PowerVR_GPU)
	#	define DIM			(1<<10)
	#	define COUNT1		(1<<3)
	#	define COUNT2		(1<<2)
	#elif defined(AE_Intel_GPU)
	#	define DIM			(1<<10)
	#	define COUNT1		(1<<3)
	#	define COUNT2		(1<<2)
	#endif
	// total: RTSize * COUNT1 * COUNT2 * 16 * 4
	//		NV:			68.7 TOp/ms
	//		Adreno:		4295 GOp/ms
	//		Mali,PVR:	2147 GOp/ms


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

			// 16 muls, 16 adds
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

			// 16 muls, 16 adds
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

		#elif MODE == S_FMA
			const type	q	= p.x;
			const type	w	= t.y;
			type		a	= type(1.0);

			// 16 fma
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

			// 16 fma
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
