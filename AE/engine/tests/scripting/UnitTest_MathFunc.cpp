// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static uint			test1_trace_idx = 0;
	static const int	test1_trace [] = {
		/*a0*/2, /*a1*/-4
	};
	static void  Test1_Trace (ScriptArgList args)
	{
		TEST( test1_trace_idx < CountOf(test1_trace) );

		const int	ref	= test1_trace[ test1_trace_idx ];
		int			i	= args.Arg<int>(0);

		TEST( ref == i );
		++test1_trace_idx;
	}

	static void  ScriptMath_Test1 (const ScriptEnginePtr &se)
	{
		AS_CHECK_THROW( se->Get()->RegisterGlobalFunction( "int Trace1 (int)", asFUNCTION(FnUnsafeCast<AngelScript::asGENFUNC_t>( &Test1_Trace )), AngelScript::asCALL_GENERIC ));

		bool res = Run< void() >( se, R"#(
			void ASmain () {
				const uint a0 = Max( 2, -4 );		Trace1( a0 );
				const uint a1 = Min( 2, -4 );		Trace1( a1 );
			})#", "ASmain" );
		TEST( res );
	}
	//-----------------------------------------------------


	static uint			test2_trace_idx = 0;
	static const uint2	test2_trace [] = {
		/*a0*/{1,2}, /*a1*/{5,6}, /*a2*/{1,0}, /*a3*/{6,6}, /*a4*/{5,4}, /*a5*/{15,12}, /*a6*/{15,24}, /*a7*/{7,6}, /*a8*/{3,2}, /*a9*/{3,4},
		/*b0*/{0x100, 0x200}, /*b0*/{0x200, 0x400}, /*b1*/{0x200, 0x400}, /*b0*/{0x40, 0x100}, /*b2*/{0x40, 0x100}
	};
	static void  Test2_Trace (ScriptArgList args)
	{
		TEST( test2_trace_idx < CountOf(test2_trace) );

		const uint2		ref	= test2_trace[ test2_trace_idx ];
		uint2			v	= args.Arg<packed_uint2 const &>(0);

		TEST( AllEqual( ref, v ));
		++test2_trace_idx;
	}

	static void  ScriptMath_Test2 (const ScriptEnginePtr &se)
	{
		AS_CHECK_THROW( se->Get()->RegisterGlobalFunction( "uint2 Trace2 (const uint2 &in)", asFUNCTION(FnUnsafeCast<AngelScript::asGENFUNC_t>( &Test2_Trace )), AngelScript::asCALL_GENERIC ));

		bool res = Run< void() >( se, R"#(
			void ASmain () {
				const uint2 a0( 1, 2 );				Trace2( a0 );
				const uint2 a1 = a0 + 4;			Trace2( a1 );
				const uint2 a2 = a1 & 1;			Trace2( a2 );
				const uint2 a3 = a1 + a2;			Trace2( a3 );
				const uint2 a4 = a3 - a0;			Trace2( a4 );
				const uint2 a5 = a4 * 3;			Trace2( a5 );
				const uint2 a6 = a5 * a0;			Trace2( a6 );
				const uint2 a7 = a5 / 2;			Trace2( a7 );
				const uint2 a8 = a5 / a1;			Trace2( a8 );
				const uint2 a9 = DivCeil( a6, a7 );	Trace2( a9 );
				uint2 b0 (0x100, 0x200);			Trace2( b0 );
				uint2 b1 = b0 <<= 1;				Trace2( b0 );	Trace2( b1 );
				uint2 b2 = b0 >>= a8;				Trace2( b0 );	Trace2( b2 );
			})#", "ASmain" );
		TEST( res );
	}
	//-----------------------------------------------------

	
	static uint			test3_trace_idx = 0;
	static const RectI	test3_trace [] = {
		/*a0*/{1,2, 3,4}
	};
	static void  Test3_Trace (ScriptArgList args)
	{
		TEST( test3_trace_idx < CountOf(test3_trace) );

		const RectI		ref	= test3_trace[ test3_trace_idx ];
		RectI			r	= args.Arg<RectI const &>(0);

		TEST( All( ref == r ));
		++test3_trace_idx;
	}

	static void  ScriptMath_Test3 (const ScriptEnginePtr &se)
	{
		AS_CHECK_THROW( se->Get()->RegisterGlobalFunction( "RectI Trace3 (const RectI &in)", asFUNCTION(FnUnsafeCast<AngelScript::asGENFUNC_t>( &Test3_Trace )), AngelScript::asCALL_GENERIC ));

		bool res = Run< void() >( se, R"#(
			void ASmain () {
				RectI	a0 (1,2, 3,4);		Trace3( a0 );
			})#", "ASmain" );
		TEST( res );
	}
	//-----------------------------------------------------

	
	static uint			test4_trace_idx = 0;
	static const float2	test4_trace [] = {
		/*s*/{1.1f, 1.1f}, /*m*/{2.f, 2.f}, /*ms*/{1.82f, 1.82f}
	};
	static void  Test4_Trace (ScriptArgList args)
	{
		TEST( test4_trace_idx < CountOf(test4_trace) );

		const float2	ref	= test4_trace[ test4_trace_idx ];
		float			f0	= args.Arg<float>(0);
		float			f1	= args.Arg<float>(1);

		TEST( Equal( ref.x, f0, 5_pct ));
		TEST( Equal( ref.y, f1, 5_pct ));
		++test4_trace_idx;
	}

	static void  ScriptMath_Test4 (const ScriptEnginePtr &se)
	{
		AS_CHECK_THROW( se->Get()->RegisterGlobalFunction( "float Trace4 (float, float)", asFUNCTION(FnUnsafeCast<AngelScript::asGENFUNC_t>( &Test4_Trace )), AngelScript::asCALL_GENERIC ));

		bool res = Run< void() >( se, R"#(
			void ASmain () {
				Second			s (1.1);		Trace4( s.GetNonScaled(), s.GetScaled() );
				Meter			m (2.0);		Trace4( m.GetNonScaled(), m.GetScaled() );
				MeterPerSecond	ms = m / s;		Trace4( ms.GetNonScaled(), ms.GetScaled() );
			})#", "ASmain" );
		TEST( res );
	}
}


extern void UnitTest_MathFunc ()
{
	auto	se = MakeRC<ScriptEngine>();
	TEST( se->Create() );

	TEST_NOTHROW(
		CoreBindings::BindScalarMath( se );
		CoreBindings::BindVectorMath( se );
		CoreBindings::BindColor( se );
		CoreBindings::BindRect( se );
		CoreBindings::BindPhysicalTypes( se );
	)

	ScriptMath_Test1( se );
	ScriptMath_Test2( se );
	ScriptMath_Test3( se );
	ScriptMath_Test4( se );

	TEST_PASSED();
}
