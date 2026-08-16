// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Operations on registers, but with scalar op in the middle.
*/

#include "Perf_Common.h"
#include "threading/Primitives/Barrier.h"
using namespace AE::Threading;

namespace
{
	static constexpr uint	c_MaxIter	= 10;
	static constexpr usize	c_Repeat	= 1'000'000;

	template <typename T>
	struct SimdVal_Base
	{
		using type	= T;
		using S		= UnwrapType<T>;

		union U {
			T	v;
			S	s [sizeof(T)/sizeof(S)];

			U () {}
		};

		U		r0, r1, r2, r3, r4, r5, r6, r7;
		U		p0, p1;

		SimdVal_Base ()
		{
			Base::Random	_rnd;
			r0.v = T{_rnd.Uniform(S(0), S(1))};
			r1.v = T{_rnd.Uniform(S(0), S(1))};
			r2.v = T{_rnd.Uniform(S(0), S(1))};
			r3.v = T{_rnd.Uniform(S(0), S(1))};
			r4.v = T{_rnd.Uniform(S(0), S(1))};
			r5.v = T{_rnd.Uniform(S(0), S(1))};
			r6.v = T{_rnd.Uniform(S(0), S(1))};
			r7.v = T{_rnd.Uniform(S(0), S(1))};

			p0.v = T{_rnd.Uniform(S(0), S(1))};
			p1.v = T{_rnd.Uniform(S(0), S(1))};
		}

		~SimdVal_Base ()
		{
			r0.v = r0.v + r4.v;
			r1.v = r1.v + r5.v;
			r2.v = r2.v + r6.v;
			r3.v = r3.v + r7.v;

			if ( All( r2.v == r0.v ) and not All( r3.v == r1.v ))
				AE_LOGI( "Result "s << ToString(r0.v + r1.v + r2.v + r3.v) );
		}
	};

	#define REUSE_MEMBERS \
		using SimdVal_Base<T>::r0; \
		using SimdVal_Base<T>::r1; \
		using SimdVal_Base<T>::r2; \
		using SimdVal_Base<T>::r3; \
		using SimdVal_Base<T>::r4; \
		using SimdVal_Base<T>::r5; \
		using SimdVal_Base<T>::r6; \
		using SimdVal_Base<T>::r7; \
		using SimdVal_Base<T>::p0; \
		using SimdVal_Base<T>::p1;


	template <typename T>
	struct VFloat_Add_4a : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint opCount = 8;	// flops

		forceinline void  operator () () __NE___
		{
			r0.v = p0.v + r0.v;
			r1.v = p0.v + r1.v;
			r2.v = p0.v + r2.v;
			r3.v = p0.v + r3.v;

			r0.v = p1.v - r0.v;
			r1.v = p1.v - r1.v;
			r2.v = p1.v - r2.v;
			r3.v = p1.v - r3.v;
		}
	};

	template <typename T>
	struct VFloat_Add_4b : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint opCount = 8;	// flops

		forceinline void  operator () () __NE___
		{
			r0.v = p0.v + r0.v;
			r1.v = p0.v + r1.v;
			r2.v = p0.v + r2.v;
			r3.v = p0.v + r3.v;

			r1.s[3] += p1.s[0];		// x5 performance lost

			// compiler will:
			// * extract instruction for scalar part of register
			// * add scalars
			// * store result back to SIMD

			// Note:
			// Any scalar operation that does not share a register or a memory location with the SIMD operands can be overlapped with the SIMD stream.
			// Many CPUs have separate scalar FP and ALU(integer) unit.

			r0.v = p1.v - r0.v;
			r1.v = p1.v - r1.v;
			r2.v = p1.v - r2.v;
			r3.v = p1.v - r3.v;
		}
	};

	template <typename T>
	struct VFloat_Add_8 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint opCount = 32;	// flops

		forceinline void  operator () () __NE___
		{
			r0.v = p0.v + r0.v;
			r1.v = p0.v + r1.v;
			r2.v = p0.v + r2.v;
			r3.v = p0.v + r3.v;
			r4.v = p0.v + r4.v;
			r5.v = p0.v + r5.v;
			r6.v = p0.v + r6.v;
			r7.v = p0.v + r7.v;

			r0.v = p1.v - r0.v;
			r1.v = p1.v - r1.v;
			r2.v = p1.v - r2.v;
			r3.v = p1.v - r3.v;
			r4.v = p1.v - r4.v;
			r5.v = p1.v - r5.v;
			r6.v = p1.v - r6.v;
			r7.v = p1.v - r7.v;

			r0.s[3] += p1.s[0];		// x2 performance lost

		//	r0.v = p0.v + r0.v;
			r1.v = p0.v + r1.v;
			r2.v = p0.v + r2.v;
			r3.v = p0.v + r3.v;
			r4.v = p0.v + r4.v;
			r5.v = p0.v + r5.v;
			r6.v = p0.v + r6.v;
			r7.v = p0.v + r7.v;

			r0.v = p1.v - r0.v;
			r1.v = p1.v - r1.v;
			r2.v = p1.v - r2.v;
			r3.v = p1.v - r3.v;
			r4.v = p1.v - r4.v;
			r5.v = p1.v - r5.v;
			r6.v = p1.v - r6.v;
			r7.v = p1.v - r7.v;
		}
	};

	template <typename T, bool WithScalar>
	struct VFloat_Add_16 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint opCount = 5*8;	// flops

		forceinline void  operator () () __NE___
		{
			T	t0, t1, t2, t3, t4, t5, t6, t7;

			t0 = p0.v + r0.v;
			t1 = p0.v + r1.v;
			t2 = p0.v + r2.v;
			t3 = p0.v + r3.v;
			t4 = p0.v + r4.v;
			t5 = p0.v + r5.v;
			t6 = p0.v + r6.v;
			t7 = p0.v + r7.v;

			t0 = p1.v - t0;
			t1 = p1.v - t1;
			t2 = p1.v - t2;
			t3 = p1.v - t3;
			t4 = p1.v - t4;
			t5 = p1.v - t5;
			t6 = p1.v - t6;
			t7 = p1.v - t7;

			if constexpr( WithScalar )
				r0.s[3] += p1.s[0];		// 30% performance lost

			t0 = p0.v + t0;
			t1 = p0.v + t1;
			t2 = p0.v + t2;
			t3 = p0.v + t3;
			t4 = p0.v + t4;
			t5 = p0.v + t5;
			t6 = p0.v + t6;
			t7 = p0.v + t7;

			t0 = p0.v + t0;
			t1 = p0.v + t1;
			t2 = p0.v + t2;
			t3 = p0.v + t3;
			t4 = p0.v + t4;
			t5 = p0.v + t5;
			t6 = p0.v + t6;
			t7 = p0.v + t7;

			r0.v = r0.v - t0;
			r1.v = r1.v - t1;
			r2.v = r2.v - t2;
			r3.v = r3.v - t3;
			r4.v = r4.v - t4;
			r5.v = r5.v - t5;
			r6.v = r6.v - t6;
			r7.v = r7.v - t7;
		}
	};
	//-------------------------------------------------------------------------


	template <typename Op>
	static void  VFloat_Op (IntervalProfiler &profiler, const usize count, StringView name) __NE___
	{
		const usize	op_count = VecSize<typename Op::type> * count;

		const auto	flops = [op_count] (secondsd dt){
			return ToStringSfx( double(op_count * Op::opCount) / dt.count() ) << "FLOPS";
		};
		profiler.BeginTest( name, flops );

		AE_LOGI( "Current core: "s << ToString(ThreadUtils::LogicalCoreIndex()) );

		for (uint i = 0; i < c_MaxIter; ++i)
		{
			Op	op;

			profiler.BeginIteration();

			for (usize z = 0; z < count; ++z)
			{
				op();
			}

			profiler.EndIteration();
		}
		profiler.EndTest();
	}

	template <typename T>
	static void  TestVFloat (IntervalProfiler &profiler, StringView typeName) __NE___
	{
		#ifdef AE_SimdRuntimeConfig
		SimdRuntimeConfig::DenormalFlushToZero( true );	// only current thread
		#endif

		static constexpr usize	count	= c_Repeat / sizeof(T);

		VFloat_Op< VFloat_Add_4a<T> >( profiler, count, String{typeName} << " - Add ilp4" );
		VFloat_Op< VFloat_Add_4b<T> >( profiler, count, String{typeName} << " - Add ilp4 + scalar" );
		VFloat_Op< VFloat_Add_8<T>  >( profiler, count, String{typeName} << " - Add ilp8 + scalar" );
		VFloat_Op< VFloat_Add_16<T,true> >( profiler, count, String{typeName} << " - Add ilp16 + scalar" );
		VFloat_Op< VFloat_Add_16<T,false> >( profiler, count, String{typeName} << " - Add ilp16" );
	}

	static void  SIMD_SingleThread () __NE___
	{
		ForEachCoreType(
			[&] (auto& core, Function<void()> setAffinity)
			{
				setAffinity();

				IntervalProfiler	profiler{ "SIMD-3 test, single thread, "s << ToString( core.type ) << " core",
												IntervalProfiler::EFlags::SortByTime };

				// Clang converts scalar to SIMD, so test is not correct
				#if not defined(AE_COMPILER_CLANG) or not defined(AE_COMPILER_CLANG_CL)
				//	TestVFloat< packed_float4 >( profiler, "Scalar Float4" );
				//	profiler.PrintAndReset();
				#endif

				#ifdef AE_SIMD_SimdFloat8
					TestVFloat< SimdFloat8 >( profiler, "Simd Float8" );

				#elif defined(AE_SIMD_SimdFloat4)
					TestVFloat< SimdFloat4 >( profiler, "Simd Float4" );

				#endif
			});
	}

} // namespace


extern void PerfTest_SIMD3 ()
{
	#if (defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)) and defined(AE_CFG_RELEASE)
		CHECK_MSG( false, "Clang will use aggressive optimization, results are incorrect" );
	#endif

	SIMD_SingleThread();

	TEST_PASSED();
}
