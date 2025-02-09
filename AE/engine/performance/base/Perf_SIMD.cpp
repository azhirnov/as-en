// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"

namespace
{
	static constexpr uint	c_MaxIter	= 10;

#ifdef AE_PLATFORM_ANDROID
	static constexpr usize	c_ArrSize	= 40'000'000;		// 1.2GB
#else
	static constexpr usize	c_ArrSize	= 160'000'000;		// 2.4GB
#endif

	template <typename T>
	struct SimdVal_Base
	{
		using type = T;

		T		accum0;
		T		accum1;
		T		accum2;
		T		accum3;

		SimdVal_Base () {}
		SimdVal_Base (UnwrapType<T> init) : accum0{init}, accum1{init}, accum2{init}, accum3{init} {}
		SimdVal_Base (UnwrapType<T> a0, UnwrapType<T> a1, UnwrapType<T> a2, UnwrapType<T> a3) : accum0{a0}, accum1{a1}, accum2{a2}, accum3{a3} {}

		~SimdVal_Base ()
		{
			if ( All( accum2 == accum0 ) and not All( accum3 == accum1 ))
				AE_LOGI( "Result "s << ToString(accum0 + accum1 + accum2 + accum3) );
		}
	};


	template <typename T, typename S = UnwrapType<T>>
	struct VFloat_Add : SimdVal_Base<T>
	{
		VFloat_Add () : SimdVal_Base<T>{ S(1.1) } {}

		// X*8 flops
		void  operator () (const T* d) __NE___
		{
			this->accum0 = (this->accum0 + d[0]) + d[1];
			this->accum1 = (this->accum1 + d[0]) - d[1];
			this->accum2 = (this->accum2 - d[0]) - d[1];
			this->accum3 = (this->accum3 - d[0]) + d[1];
		}
	};

	template <typename T, typename S = UnwrapType<T>>
	struct VFloat_AddSeq2 : SimdVal_Base<T>
	{
		VFloat_AddSeq2 () : SimdVal_Base<T>{ S(2.1) } {}

		// X*8 flops
		void  operator () (const T* d) __NE___
		{
			this->accum0 = (this->accum2 + d[0]) + d[1];
			this->accum1 = (this->accum3 + d[0]) - d[1];
			this->accum2 = (this->accum0 - d[0]) - d[1];
			this->accum3 = (this->accum1 - d[0]) + d[1];
		}
	};

	template <typename T, typename S = UnwrapType<T>>
	struct VFloat_AddSeq1 : SimdVal_Base<T>
	{
		VFloat_AddSeq1 () : SimdVal_Base<T>{ S(2.1) } {}

		// X*8 flops
		void  operator () (const T* d) __NE___
		{
			this->accum0 = (this->accum3 + d[0]) + d[1];
			this->accum1 = (this->accum0 + d[0]) - d[1];
			this->accum2 = (this->accum1 - d[0]) - d[1];
			this->accum3 = (this->accum2 - d[0]) + d[1];
		}
	};

	template <typename T, typename S = UnwrapType<T>>
	struct VFloat_Mul : SimdVal_Base<T>
	{
		VFloat_Mul () : SimdVal_Base<T>{ S(1.0), S(1.2), S(0.8), S(0.99) } {}

		// X*8 flops
		void  operator () (const T* d) __NE___
		{
			this->accum0 = (this->accum0 * d[0]) * d[1];
			this->accum1 = (this->accum1 * d[0]) * d[1];
			this->accum2 = (this->accum2 * d[0]) * d[1];
			this->accum3 = (this->accum3 * d[0]) * d[1];
		}
	};

	template <typename T, typename S = UnwrapType<T>>
	struct VFloat_MulAdd : SimdVal_Base<T>
	{
		VFloat_MulAdd () : SimdVal_Base<T>{ S(1.0), S(1.2), S(0.8), S(0.99) } {}

		// X*8 flops
		void  operator () (const T* d) __NE___
		{
			this->accum0 = (this->accum0 * d[0]) + d[1];
			this->accum1 = (this->accum1 * d[1]) - d[0];
			this->accum2 = (this->accum2 * d[0]) + d[0];
			this->accum3 = (this->accum3 * d[1]) - d[1];
		}
	};

	template <typename T, typename S = UnwrapType<T>>
	struct VFloat_AddMul : SimdVal_Base<T>
	{
		VFloat_AddMul () : SimdVal_Base<T>{ S(1.0), S(1.2), S(0.8), S(0.99) } {}

		// X*8 flops
		void  operator () (const T* d) __NE___
		{
			this->accum0 = (this->accum0 + d[0]) * d[1];
			this->accum1 = (this->accum1 - d[1]) * d[0];
			this->accum2 = (this->accum2 + d[0]) * d[0];
			this->accum3 = (this->accum3 - d[1]) * d[1];
		}
	};

	template <typename T, typename S = UnwrapType<T>>
	struct VFloat_AddMul2 : SimdVal_Base<T>
	{
		VFloat_AddMul2 () : SimdVal_Base<T>{ S(1.0), S(1.2), S(0.8), S(0.99) } {}

		// X*8 flops
		void  operator () (const T* d) __NE___
		{
			this->accum0 = (this->accum0 + d[0]);
			this->accum1 = (this->accum1 - d[1]);
			this->accum2 = (this->accum2 + d[0]);
			this->accum3 = (this->accum3 - d[1]);

			this->accum0 = (this->accum0 * d[1]);
			this->accum1 = (this->accum1 * d[0]);
			this->accum2 = (this->accum2 * d[0]);
			this->accum3 = (this->accum3 * d[1]);
		}
	};

	template <typename T, typename S = UnwrapType<T>>
	struct VFloat_MulAddSeq1 : SimdVal_Base<T>
	{
		VFloat_MulAddSeq1 () : SimdVal_Base<T>{ S(1.0), S(1.2), S(0.8), S(0.99) } {}

		// X*8 flops
		void  operator () (const T* d) __NE___
		{
			this->accum0 = this->accum3 * d[0];
			this->accum1 = this->accum0 * d[1];
			this->accum2 = this->accum1 * d[0];
			this->accum3 = this->accum2 * d[1];

			this->accum0 = this->accum3 + d[1];
			this->accum1 = this->accum0 - d[0];
			this->accum2 = this->accum1 + d[0];
			this->accum3 = this->accum2 - d[1];
		}
	};

	template <typename T, typename S = UnwrapType<T>>
	struct VFloat_FMA : SimdVal_Base<T>
	{
		VFloat_FMA () : SimdVal_Base<T>{ S(1.0), S(1.2), S(0.8), S(0.99) } {}

		// X*8 flops
		void  operator () (const T* d) __NE___
		{
			this->accum0 = FusedMulAdd( this->accum0, d[0], d[1] );
			this->accum1 = FusedMulAdd( this->accum1, d[1], d[0] );
			this->accum2 = FusedMulAdd( this->accum2, d[0], d[0] );
			this->accum3 = FusedMulAdd( this->accum3, d[1], d[1] );
		}
	};

	template <typename T, typename S = UnwrapType<T>>
	struct VFloat_Div : SimdVal_Base<T>
	{
		VFloat_Div () : SimdVal_Base<T>{ S(1.0), S(1.2), S(0.8), S(0.99) } {}

		// X*4 flops
		void  operator () (const T* d) __NE___
		{
			this->accum0 = this->accum0 + d[0];
			this->accum1 = this->accum1 + d[1];
			this->accum2 = d[0] + this->accum2;
			this->accum3 = d[1] + this->accum3;

			this->accum0 = this->accum0 / d[0];
			this->accum1 = this->accum1 / d[1];
			this->accum2 = d[0] / this->accum2;
			this->accum3 = d[1] / this->accum3;
		}
	};

	template <typename T, typename S = UnwrapType<T>>
	struct VFloat_DivSeq1 : SimdVal_Base<T>
	{
		VFloat_DivSeq1 () : SimdVal_Base<T>{ S(1.0), S(1.2), S(0.8), S(0.99) } {}

		// X*4 flops
		void  operator () (const T* d) __NE___
		{
			this->accum0 = this->accum3 / d[0];
			this->accum1 = this->accum0 / d[1];
			this->accum2 = d[0] / this->accum1;
			this->accum3 = d[1] / this->accum2;
		}
	};

	template <typename T, typename S = UnwrapType<T>>
	struct VFloat_Sqrt : SimdVal_Base<T>
	{
		VFloat_Sqrt () : SimdVal_Base<T>{ S(1.0), S(1.2), S(0.8), S(0.99) } {}

		// X*4 flops
		void  operator () (const T* d) __NE___
		{
			this->accum0 = this->accum0 + d[0];
			this->accum1 = this->accum1 + d[1];
			this->accum2 = d[0] + this->accum2;
			this->accum3 = d[1] + this->accum3;

			this->accum0 = Sqrt( this->accum0 );
			this->accum1 = Sqrt( this->accum1 );
			this->accum2 = Sqrt( this->accum2 );
			this->accum3 = Sqrt( this->accum3 );
		}
	};

	template <usize N, typename FN, usize I = 0>
	forceinline AE_FLATTEN_FN void  Unroll (const FN &fn) __NE___
	{
		if constexpr( N > 0 ) {
			fn( I );
			Unroll< N-1, FN, I+1 >( fn );
		}
	}
	//-------------------------------------------------------------------------



	#if 1
		template <typename Op>
		static void  VFloat4_Op (IntervalProfiler &profiler, typename Op::type* ptr, const usize count) __NE___
		{
			using S = UnwrapType< typename Op::type >;

			AE_LOGI( "Current core: "s << ToString(ThreadUtils::LogicalCoreIndex()) );

			for (uint i = 0; i < c_MaxIter; ++i)
			{
				for (usize j = 0; j < count; ++j) {
					ptr[j] = typename Op::type{ S(j) / S(count) + S(0.2) };
				}

				Op	op;

				profiler.BeginIteration();

			  #ifdef AE_COMPILER_CLANG
				for (usize j = 0; j < count-1; ++j)
				{
					op( ptr+j );
				}

				// loop unrolling for MSVC, increase AVX performance up to 2x
			  #elif 0
				for (usize j = 0; j < count-1; j += 16)
				{
					// MSVC: 36 GFLOPS
					Unroll<16>( [&op, ptr, j] (usize c) { op( ptr+j+c ); });
				}
			  #else
				for (usize j = 0; j < count-1; j += 16)
				{
					// MSVC: 40 GFLOPS
					op( ptr+j );
					op( ptr+j+1 );
					op( ptr+j+2 );
					op( ptr+j+3 );

					op( ptr+j+4 );
					op( ptr+j+5 );
					op( ptr+j+6 );
					op( ptr+j+7 );

					op( ptr+j+8 );
					op( ptr+j+9 );
					op( ptr+j+10 );
					op( ptr+j+11 );

					op( ptr+j+12 );
					op( ptr+j+13 );
					op( ptr+j+14 );
					op( ptr+j+15 );
				}
			  #endif

				profiler.EndIteration();
			}
			profiler.EndTest();
		}

		template <typename T>
		static void  TestVFloat (IntervalProfiler &profiler, StringView coreNameAndType) __NE___
		{
			static constexpr usize	arr_size	= (sizeof(T) == 16 ? c_ArrSize :
												   sizeof(T) == 32 ? c_ArrSize/2 :
												   sizeof(T) == 64 ? c_ArrSize/4 :
												   0) + 1;
			StaticAssert( arr_size > 1 );
			StaticAssert( IsMultipleOf( arr_size-1, 16 ));

			static constexpr usize	op_count	= (arr_size - 1) * VecSize<T>;
			auto&					cpu_info	= CpuArchInfo::Get();

			Array<T>	data;
			data.resize( arr_size );

			const usize	count = Max( data.size(), arr_size );	// MSVC can optimize only const size
			//const usize	count = arr_size;

			const auto	flops8 = [](secondsd dt)
			{{
				return	" - "s << ToStringSfx( double(op_count * 8) / dt.count() ) << "FLOPS,  " <<
						ToStringSfx( double(arr_size * sizeof(T)) / dt.count() ) << "B/s";
			}};
			const auto	flops4 = [](secondsd dt)
			{{
				return	" - "s << ToStringSfx( double(op_count * 4) / dt.count() ) << "FLOPS,  " <<
						ToStringSfx( double(arr_size * sizeof(T)) / dt.count() ) << "B/s";
			}};


			profiler.BeginTest( String{coreNameAndType} << " - Add",		flops8 );
			VFloat4_Op< VFloat_Add<T> >( profiler, data.data(), count );

			profiler.BeginTest( String{coreNameAndType} << " - Mul",		flops8 );
			VFloat4_Op< VFloat_Mul<T> >( profiler, data.data(), count );

			profiler.BeginTest( String{coreNameAndType} << " - MulAdd",		flops8 );
			VFloat4_Op< VFloat_MulAdd<T> >( profiler, data.data(), count );

		  #ifdef AE_CPU_ARCH_ARM_BASED
			if ( cpu_info.feats.NEON )
			{
				if constexpr( HasSimdType<T> ) {
					if constexpr( T::Has_PreciseSqrt() ) {
						profiler.BeginTest( String{coreNameAndType} << " - FMA",	flops8 );
						VFloat4_Op< VFloat_FMA<T> >( profiler, data.data(), count );
					}
				}else{
					profiler.BeginTest( String{coreNameAndType} << " - FMA",	flops8 );
					VFloat4_Op< VFloat_FMA<T> >( profiler, data.data(), count );
				}
			}
		  #endif

		  #if 1
			profiler.BeginTest( String{coreNameAndType} << " - seq1 Add",	flops8 );
			VFloat4_Op< VFloat_AddSeq1<T> >( profiler, data.data(), count );

			//profiler.BeginTest( String{coreNameAndType} << " - seq2 Add",	flops8 );
			//VFloat4_Op< VFloat_AddSeq2<T> >( profiler, data.data(), count );

			profiler.BeginTest( String{coreNameAndType} << " - AddMul",		flops8 );
			VFloat4_Op< VFloat_AddMul<T> >( profiler, data.data(), count );

			profiler.BeginTest( String{coreNameAndType} << " - AddMul2",	flops8 );
			VFloat4_Op< VFloat_AddMul2<T> >( profiler, data.data(), count );

			profiler.BeginTest( String{coreNameAndType} << " - seq1 MulAdd", flops8 );
			VFloat4_Op< VFloat_MulAddSeq1<T> >( profiler, data.data(), count );

			profiler.BeginTest( String{coreNameAndType} << " - Div",		flops4 );
			VFloat4_Op< VFloat_Div<T> >( profiler, data.data(), count );

			profiler.BeginTest( String{coreNameAndType} << " - seq1 Div",	flops4 );
			VFloat4_Op< VFloat_DivSeq1<T> >( profiler, data.data(), count );

			if constexpr( HasSimdType<T> ) {
				if constexpr( T::Has_PreciseSqrt() ) {
					profiler.BeginTest( String{coreNameAndType} << " - Sqrt",		flops4 );
					VFloat4_Op< VFloat_Sqrt<T> >( profiler, data.data(), count );
				}
			}else{
				profiler.BeginTest( String{coreNameAndType} << " - Sqrt",		flops4 );
				VFloat4_Op< VFloat_Sqrt<T> >( profiler, data.data(), count );
			}
		  #endif
		}
	#endif
	//-------------------------------------------------------------------------

} // namespace


extern void PerfTest_SIMD ()
{
	IntervalProfiler	profiler{ "SIMD test" };

	ForEachCoreType(
		[&] (ECoreType coreType, Function<void()> setAffinity)
		{
			const String	name {ToString( coreType )};

			TestVFloat< packed_float4 >( profiler, name + ", Scalar Float4" );		setAffinity();
			profiler.PrintAndReset();

			#ifdef AE_SIMD_SimdFloat8
				TestVFloat< SimdFloat8 >( profiler, name + ", Simd Float8" );		setAffinity();
			#endif
			#ifdef AE_SIMD_SimdFloat4
				TestVFloat< SimdFloat4 >( profiler, name + ", Simd Float4" );		setAffinity();
			#endif
			profiler.PrintAndReset();

			TestVFloat< packed_double2 >( profiler, name + ", Scalar Double2" );	setAffinity();
			TestVFloat< packed_double4 >( profiler, name + ", Scalar Double4" );	setAffinity();
			profiler.PrintAndReset();

			#ifdef AE_SIMD_SimdDouble2
				TestVFloat< SimdDouble2 >( profiler, name + ", Simd Double2" );		setAffinity();
			#endif
			#ifdef AE_SIMD_SimdDouble4
				TestVFloat< SimdDouble4 >( profiler, name + ", Simd Double4" );		setAffinity();
			#endif
			profiler.PrintAndReset();
		});

	TEST_PASSED();
}
