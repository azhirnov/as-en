// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	With memory access.
*/

#include "Perf_Common.h"
#include "threading/Primitives/Barrier.h"
using namespace AE::Threading;

namespace
{
	static constexpr uint	c_MaxIter	= 1000;
	static constexpr usize	c_Repeat	= 16'000;

	template <typename T>
	struct SimdVal_Base
	{
		using type	= T;
		using S		= UnwrapType<T>;

		T		r0, r1, r2, r3, r4, r5, r6, r7;

		SimdVal_Base ()
		{
			Base::Random	_rnd;
			r0 = T{_rnd.Uniform(S(0), S(1))};
			r1 = T{_rnd.Uniform(S(0), S(1))};
			r2 = T{_rnd.Uniform(S(0), S(1))};
			r3 = T{_rnd.Uniform(S(0), S(1))};
			r4 = T{_rnd.Uniform(S(0), S(1))};
			r5 = T{_rnd.Uniform(S(0), S(1))};
			r6 = T{_rnd.Uniform(S(0), S(1))};
			r7 = T{_rnd.Uniform(S(0), S(1))};
		}

		~SimdVal_Base ()
		{
			r0 = r0 + r4;
			r1 = r1 + r5;
			r2 = r2 + r6;
			r3 = r3 + r7;

			if ( All( r2 == r0 ) and not All( r3 == r1 ))
				AE_LOGI( "Result "s << ToString(r0 + r1 + r2 + r3) );
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
		using SimdVal_Base<T>::r7;


	template <typename T>
	struct VFloat_Add_8r2 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint  opCount   = 32;	// flops
		static constexpr uint  readCount = 2;

		forceinline void  operator () (ArrayView<T> arr, usize i) __NE___
		{
			i *= 2;
			const T		p0 = arr[i+0];
			const T		p1 = arr[i+1];

			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;
			r4 = p0 + r4;
			r5 = p0 + r5;
			r6 = p0 + r6;
			r7 = p0 + r7;

			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;
			r4 = p1 - r4;
			r5 = p1 - r5;
			r6 = p1 - r6;
			r7 = p1 - r7;

			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;
			r4 = p0 + r4;
			r5 = p0 + r5;
			r6 = p0 + r6;
			r7 = p0 + r7;

			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;
			r4 = p1 - r4;
			r5 = p1 - r5;
			r6 = p1 - r6;
			r7 = p1 - r7;
		}
	};

	template <typename T>
	struct VFloat_Add_8r4 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint  opCount   = 32;	// flops
		static constexpr uint  readCount = 4;

		forceinline void  operator () (ArrayView<T> arr, usize i) __NE___
		{
			i *= 4;
			const T		p0 = arr[i+0];
			const T		p1 = arr[i+1];
			const T		p2 = arr[i+2];
			const T		p3 = arr[i+3];

			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;
			r4 = p0 + r4;
			r5 = p0 + r5;
			r6 = p0 + r6;
			r7 = p0 + r7;

			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;
			r4 = p1 - r4;
			r5 = p1 - r5;
			r6 = p1 - r6;
			r7 = p1 - r7;

			r0 = p2 + r0;
			r1 = p2 + r1;
			r2 = p2 + r2;
			r3 = p2 + r3;
			r4 = p2 + r4;
			r5 = p2 + r5;
			r6 = p2 + r6;
			r7 = p2 + r7;

			r0 = p3 - r0;
			r1 = p3 - r1;
			r2 = p3 - r2;
			r3 = p3 - r3;
			r4 = p3 - r4;
			r5 = p3 - r5;
			r6 = p3 - r6;
			r7 = p3 - r7;
		}
	};

	template <typename T>
	struct VFloat_Add_8r8 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint  opCount   = 32;	// flops
		static constexpr uint  readCount = 8;

		forceinline void  operator () (ArrayView<T> arr, usize i) __NE___
		{
			i *= 8;
			const T		p0 = arr[i+0];
			const T		p1 = arr[i+1];
			const T		p2 = arr[i+2];
			const T		p3 = arr[i+3];
			const T		p4 = arr[i+4];
			const T		p5 = arr[i+5];
			const T		p6 = arr[i+6];
			const T		p7 = arr[i+7];

			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;
			r4 = p1 + r4;
			r5 = p1 + r5;
			r6 = p1 + r6;
			r7 = p1 + r7;

			r0 = p2 - r0;
			r1 = p2 - r1;
			r2 = p2 - r2;
			r3 = p2 - r3;
			r4 = p3 - r4;
			r5 = p3 - r5;
			r6 = p3 - r6;
			r7 = p3 - r7;

			r0 = p4 + r0;
			r1 = p4 + r1;
			r2 = p4 + r2;
			r3 = p4 + r3;
			r4 = p5 + r4;
			r5 = p5 + r5;
			r6 = p5 + r6;
			r7 = p5 + r7;

			r0 = p6 - r0;
			r1 = p6 - r1;
			r2 = p6 - r2;
			r3 = p6 - r3;
			r4 = p7 - r4;
			r5 = p7 - r5;
			r6 = p7 - r6;
			r7 = p7 - r7;
		}
	};

	template <typename T>
	struct VFloat_Add_8r16 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint  opCount   = 32;	// flops
		static constexpr uint  readCount = 16;

		forceinline void  operator () (ArrayView<T> arr, usize i) __NE___
		{
			i *= 16;
			const T		p0  = arr[i+0];
			const T		p1  = arr[i+1];
			const T		p2  = arr[i+2];
			const T		p3  = arr[i+3];
			const T		p4  = arr[i+4];
			const T		p5  = arr[i+5];
			const T		p6  = arr[i+6];
			const T		p7  = arr[i+7];
			const T		p8  = arr[i+8];
			const T		p9  = arr[i+9];
			const T		p10 = arr[i+10];
			const T		p11 = arr[i+11];
			const T		p12 = arr[i+12];
			const T		p13 = arr[i+13];
			const T		p14 = arr[i+14];
			const T		p15 = arr[i+15];

			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p1 + r2;
			r3 = p1 + r3;
			r4 = p2 + r4;
			r5 = p2 + r5;
			r6 = p3 + r6;
			r7 = p3 + r7;

			r0 = p4 - r0;
			r1 = p4 - r1;
			r2 = p5 - r2;
			r3 = p5 - r3;
			r4 = p6 - r4;
			r5 = p6 - r5;
			r6 = p7 - r6;
			r7 = p7 - r7;

			r0 = p8  + r0;
			r1 = p8  + r1;
			r2 = p9  + r2;
			r3 = p9  + r3;
			r4 = p10 + r4;
			r5 = p10 + r5;
			r6 = p11 + r6;
			r7 = p11 + r7;

			r0 = p12 - r0;
			r1 = p12 - r1;
			r2 = p13 - r2;
			r3 = p13 - r3;
			r4 = p14 - r4;
			r5 = p14 - r5;
			r6 = p15 - r6;
			r7 = p15 - r7;
		}
	};

	template <typename T>
	struct VFloat_Add_4r2 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint  opCount   = 16;	// flops
		static constexpr uint  readCount = 2;

		forceinline void  operator () (ArrayView<T> arr, usize i) __NE___
		{
			i *= 2;
			const T		p0 = arr[i+0];
			const T		p1 = arr[i+1];

			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;

			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;

			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;

			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;
		}
	};

	template <typename T>
	struct VFloat_Add_4r4 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint  opCount   = 16;	// flops
		static constexpr uint  readCount = 4;

		forceinline void  operator () (ArrayView<T> arr, usize i) __NE___
		{
			i *= 4;
			const T		p0 = arr[i+0];
			const T		p1 = arr[i+1];
			const T		p2 = arr[i+2];
			const T		p3 = arr[i+3];

			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;

			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;

			r0 = p2 + r0;
			r1 = p2 + r1;
			r2 = p2 + r2;
			r3 = p2 + r3;

			r0 = p3 - r0;
			r1 = p3 - r1;
			r2 = p3 - r2;
			r3 = p3 - r3;
		}
	};

	template <typename T>
	struct VFloat_Add_4r8 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint  opCount   = 16;	// flops
		static constexpr uint  readCount = 8;

		forceinline void  operator () (ArrayView<T> arr, usize i) __NE___
		{
			i *= 8;
			const T		p0 = arr[i+0];
			const T		p1 = arr[i+1];
			const T		p2 = arr[i+2];
			const T		p3 = arr[i+3];
			const T		p4 = arr[i+4];
			const T		p5 = arr[i+5];
			const T		p6 = arr[i+6];
			const T		p7 = arr[i+7];

			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p1 + r2;
			r3 = p1 + r3;

			r0 = p2 - r0;
			r1 = p2 - r1;
			r2 = p3 - r2;
			r3 = p3 - r3;

			r0 = p4 + r0;
			r1 = p4 + r1;
			r2 = p5 + r2;
			r3 = p5 + r3;

			r0 = p6 - r0;
			r1 = p6 - r1;
			r2 = p7 - r2;
			r3 = p7 - r3;
		}
	};

	template <typename T>
	struct VFloat_Add_4r16 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint  opCount   = 16;	// flops
		static constexpr uint  readCount = 16;

		forceinline void  operator () (ArrayView<T> arr, usize i) __NE___
		{
			i *= 16;
			const T		p0  = arr[i+0];
			const T		p1  = arr[i+1];
			const T		p2  = arr[i+2];
			const T		p3  = arr[i+3];
			const T		p4  = arr[i+4];
			const T		p5  = arr[i+5];
			const T		p6  = arr[i+6];
			const T		p7  = arr[i+7];
			const T		p8  = arr[i+8];
			const T		p9  = arr[i+9];
			const T		p10 = arr[i+10];
			const T		p11 = arr[i+11];
			const T		p12 = arr[i+12];
			const T		p13 = arr[i+13];
			const T		p14 = arr[i+14];
			const T		p15 = arr[i+15];

			r0 = p0 + r0;
			r1 = p1 + r1;
			r2 = p2 + r2;
			r3 = p3 + r3;

			r0 = p4 - r0;
			r1 = p5 - r1;
			r2 = p6 - r2;
			r3 = p7 - r3;

			r0 = p8  + r0;
			r1 = p9  + r1;
			r2 = p10 + r2;
			r3 = p11 + r3;

			r0 = p12 - r0;
			r1 = p13 - r1;
			r2 = p14 - r2;
			r3 = p15 - r3;
		}
	};
	//-------------------------------------------------------------------------


	template <typename Op>
	static void  VFloat_Op (IntervalProfiler &profiler, ArrayView<typename Op::type> arr, const usize count, StringView name) __NE___
	{
		const auto	flops = [count] (secondsd dt)
		{
			const usize		op_count	= VecSize<typename Op::type> * count * Op::opCount;
			const usize		read_count	= Op::readCount * count;

			return	ToStringSfx( double(op_count) / dt.count() ) << "FLOPS | " <<
					ToStringSfx( (read_count * sizeof(typename Op::type)) / dt.count() ) << "B/s";
		};
		profiler.BeginTest( name, flops );

		AE_LOGI( "Current core: "s << ToString(ThreadUtils::LogicalCoreIndex()) );

		for (uint i = 0; i < c_MaxIter; ++i)
		{
			Op	op;

			profiler.BeginIteration();

			for (usize z = 0; z < count; ++z)
			{
				op( arr, z );
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

		Array<T>	data;
		data.resize( count * 16 );

		AE_LOGI( "Data size: "s << ToString( ArraySizeOf(data) ));

		VFloat_Op< VFloat_Add_8r2<T>  >( profiler, data, count, String{typeName} << " - Add ilp8 r2" );
		VFloat_Op< VFloat_Add_8r4<T>  >( profiler, data, count, String{typeName} << " - Add ilp8 r4" );
		VFloat_Op< VFloat_Add_8r8<T>  >( profiler, data, count, String{typeName} << " - Add ilp8 r8" );
		VFloat_Op< VFloat_Add_8r16<T> >( profiler, data, count, String{typeName} << " - Add ilp8 r16" );
		VFloat_Op< VFloat_Add_4r2<T>  >( profiler, data, count, String{typeName} << " - Add ilp4 r2" );
		VFloat_Op< VFloat_Add_4r4<T>  >( profiler, data, count, String{typeName} << " - Add ilp4 r4" );
		VFloat_Op< VFloat_Add_4r8<T>  >( profiler, data, count, String{typeName} << " - Add ilp4 r8" );
		VFloat_Op< VFloat_Add_4r16<T> >( profiler, data, count, String{typeName} << " - Add ilp4 r16" );
	}


	static void  SIMD_SingleThread () __NE___
	{
		ForEachCoreType(
			[&] (auto& core, Function<void()> setAffinity)
			{
				setAffinity();

				IntervalProfiler	profiler{ "SIMD-2 test, single thread, "s << ToString( core.type ) << " core",
												IntervalProfiler::EFlags::SortByTime };

				// Clang converts scalar to SIMD, so test is not correct
				#if not defined(AE_COMPILER_CLANG) or not defined(AE_COMPILER_CLANG_CL)
					TestVFloat< packed_float4 >( profiler, "Scalar Float4" );
					profiler.PrintAndReset();
				#endif

				#ifdef AE_SIMD_SimdFloat8
					TestVFloat< SimdFloat8 >( profiler, "Simd Float8" );

				#elif defined(AE_SIMD_SimdFloat4)
					TestVFloat< SimdFloat4 >( profiler, "Simd Float4" );

				#endif
				profiler.PrintAndReset();
			});
	}

} // namespace


extern void PerfTest_SIMD2 ()
{
	SIMD_SingleThread();

	TEST_PASSED();
}
