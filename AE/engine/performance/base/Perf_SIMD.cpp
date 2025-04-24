// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	[results](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-cpu/SIMD_FLOPS.md)
*/

#include "Perf_Common.h"
#include "threading/Primitives/Barrier.h"
using namespace AE::Threading;

namespace
{
	static constexpr uint	c_MaxIter	= 10;
	static constexpr usize	c_Repeat	= 1000'000'000;


	template <typename T>
	struct SimdVal_Base
	{
		using type	= T;
		using S		= UnwrapType<T>;

		T		r0, r1, r2, r3, r4, r5, r6, r7;
		T		p0, p1;

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
			
			p0 = T{_rnd.Uniform(S(0), S(1))};
			p1 = T{_rnd.Uniform(S(0), S(1))};
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
		using SimdVal_Base<T>::r7; \
		using SimdVal_Base<T>::p0; \
		using SimdVal_Base<T>::p1;


	template <typename T>
	struct VFloat_Add_8 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint opCount = 32;	// flops

		forceinline void  operator () () __NE___
		{
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
	struct VFloat_Add_4 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint opCount = 32;	// flops

		forceinline void  operator () () __NE___
		{
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
	struct VFloat_Add_5 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint opCount = 40;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;
			r4 = p0 + r4;
			
			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;
			r4 = p1 - r4;
			
			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;
			r4 = p0 + r4;
			
			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;
			r4 = p1 - r4;
			
			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;
			r4 = p0 + r4;
			
			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;
			r4 = p1 - r4;
			
			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;
			r4 = p0 + r4;
			
			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;
			r4 = p1 - r4;
		}
	};
	
	template <typename T>
	struct VFloat_Add_6 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint opCount = 36;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;
			r4 = p0 + r4;
			r5 = p0 + r5;
			
			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;
			r4 = p1 - r4;
			r5 = p1 - r5;
			
			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;
			r4 = p0 + r4;
			r5 = p0 + r5;
			
			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;
			r4 = p1 - r4;
			r5 = p1 - r5;
			
			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;
			r4 = p0 + r4;
			r5 = p0 + r5;
			
			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;
			r4 = p1 - r4;
			r5 = p1 - r5;
		}
	};
	
	template <typename T>
	struct VFloat_Add_7 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		static constexpr uint opCount = 28;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;
			r4 = p0 + r4;
			r5 = p0 + r5;
			r6 = p0 + r6;
			
			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;
			r4 = p1 - r4;
			r5 = p1 - r5;
			r6 = p1 - r6;
			
			r0 = p0 + r0;
			r1 = p0 + r1;
			r2 = p0 + r2;
			r3 = p0 + r3;
			r4 = p0 + r4;
			r5 = p0 + r5;
			r6 = p0 + r6;
			
			r0 = p1 - r0;
			r1 = p1 - r1;
			r2 = p1 - r2;
			r3 = p1 - r3;
			r4 = p1 - r4;
			r5 = p1 - r5;
			r6 = p1 - r6;
		}
	};

	template <typename T>
	struct VFloat_Mul : SimdVal_Base<T>
	{
		REUSE_MEMBERS;
		
		static constexpr uint opCount = 32;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = p0 * r0;
			r1 = p0 * r1;
			r2 = p0 * r2;
			r3 = p0 * r3;
			r4 = p0 * r4;
			r5 = p0 * r5;
			r6 = p0 * r6;
			r7 = p0 * r7;
			
			r0 = p1 * r0;
			r1 = p1 * r1;
			r2 = p1 * r2;
			r3 = p1 * r3;
			r4 = p1 * r4;
			r5 = p1 * r5;
			r6 = p1 * r6;
			r7 = p1 * r7;
			
			r0 = p0 * r0;
			r1 = p0 * r1;
			r2 = p0 * r2;
			r3 = p0 * r3;
			r4 = p0 * r4;
			r5 = p0 * r5;
			r6 = p0 * r6;
			r7 = p0 * r7;
			
			r0 = p1 * r0;
			r1 = p1 * r1;
			r2 = p1 * r2;
			r3 = p1 * r3;
			r4 = p1 * r4;
			r5 = p1 * r5;
			r6 = p1 * r6;
			r7 = p1 * r7;
		}
	};

	template <typename T>
	struct VFloat_MulAdd : SimdVal_Base<T>
	{
		REUSE_MEMBERS;
		
		static constexpr uint opCount = 32;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = p0 * r0;
			r1 = p0 * r1;
			r2 = p0 * r2;
			r3 = p0 * r3;
			r4 = p0 * r4;
			r5 = p0 * r5;
			r6 = p0 * r6;
			r7 = p0 * r7;
			
			r0 = p1 + r0;
			r1 = p1 + r1;
			r2 = p1 + r2;
			r3 = p1 + r3;
			r4 = p1 + r4;
			r5 = p1 + r5;
			r6 = p1 + r6;
			r7 = p1 + r7;
			
			r0 = p0 * r0;
			r1 = p0 * r1;
			r2 = p0 * r2;
			r3 = p0 * r3;
			r4 = p0 * r4;
			r5 = p0 * r5;
			r6 = p0 * r6;
			r7 = p0 * r7;
			
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
	struct VFloat_FMA_8 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;
		
		static constexpr uint opCount = 32;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = FusedMulAdd( r0, p0, p1 );
			r1 = FusedMulAdd( r1, p0, p1 );
			r2 = FusedMulAdd( r2, p0, p1 );
			r3 = FusedMulAdd( r3, p0, p1 );
			r4 = FusedMulAdd( r4, p0, p1 );
			r5 = FusedMulAdd( r5, p0, p1 );
			r6 = FusedMulAdd( r6, p0, p1 );
			r7 = FusedMulAdd( r7, p0, p1 );
			
			r0 = FusedMulAdd( r0, p1, p0 );
			r1 = FusedMulAdd( r1, p1, p0 );
			r2 = FusedMulAdd( r2, p1, p0 );
			r3 = FusedMulAdd( r3, p1, p0 );
			r4 = FusedMulAdd( r4, p1, p0 );
			r5 = FusedMulAdd( r5, p1, p0 );
			r6 = FusedMulAdd( r6, p1, p0 );
			r7 = FusedMulAdd( r7, p1, p0 );
		}
	};
	
	template <typename T>
	struct VFloat_FMA_4 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;
		
		static constexpr uint opCount = 32;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = FusedMulAdd( r0, p0, p1 );
			r1 = FusedMulAdd( r1, p0, p1 );
			r2 = FusedMulAdd( r2, p0, p1 );
			r3 = FusedMulAdd( r3, p0, p1 );
			
			r0 = FusedMulAdd( r0, p1, p0 );
			r1 = FusedMulAdd( r1, p1, p0 );
			r2 = FusedMulAdd( r2, p1, p0 );
			r3 = FusedMulAdd( r3, p1, p0 );
			
			r0 = FusedMulAdd( r0, p0, p1 );
			r1 = FusedMulAdd( r1, p0, p1 );
			r2 = FusedMulAdd( r2, p0, p1 );
			r3 = FusedMulAdd( r3, p0, p1 );
			
			r0 = FusedMulAdd( r0, p1, p0 );
			r1 = FusedMulAdd( r1, p1, p0 );
			r2 = FusedMulAdd( r2, p1, p0 );
			r3 = FusedMulAdd( r3, p1, p0 );
		}
	};
	
	template <typename T>
	struct VFloat_FMA_5 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;
		
		static constexpr uint opCount = 40;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = FusedMulAdd( r0, p0, p1 );
			r1 = FusedMulAdd( r1, p0, p1 );
			r2 = FusedMulAdd( r2, p0, p1 );
			r3 = FusedMulAdd( r3, p0, p1 );
			r4 = FusedMulAdd( r4, p0, p1 );
			
			r0 = FusedMulAdd( r0, p1, p0 );
			r1 = FusedMulAdd( r1, p1, p0 );
			r2 = FusedMulAdd( r2, p1, p0 );
			r3 = FusedMulAdd( r3, p1, p0 );
			r4 = FusedMulAdd( r4, p1, p0 );
			
			r0 = FusedMulAdd( r0, p0, p1 );
			r1 = FusedMulAdd( r1, p0, p1 );
			r2 = FusedMulAdd( r2, p0, p1 );
			r3 = FusedMulAdd( r3, p0, p1 );
			r4 = FusedMulAdd( r4, p0, p1 );
			
			r0 = FusedMulAdd( r0, p1, p0 );
			r1 = FusedMulAdd( r1, p1, p0 );
			r2 = FusedMulAdd( r2, p1, p0 );
			r3 = FusedMulAdd( r3, p1, p0 );
			r4 = FusedMulAdd( r4, p1, p0 );
		}
	};
	
	template <typename T>
	struct VFloat_FMA_6 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;
		
		static constexpr uint opCount = 24;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = FusedMulAdd( r0, p0, p1 );
			r1 = FusedMulAdd( r1, p0, p1 );
			r2 = FusedMulAdd( r2, p0, p1 );
			r3 = FusedMulAdd( r3, p0, p1 );
			r4 = FusedMulAdd( r4, p0, p1 );
			r5 = FusedMulAdd( r5, p0, p1 );
			
			r0 = FusedMulAdd( r0, p1, p0 );
			r1 = FusedMulAdd( r1, p1, p0 );
			r2 = FusedMulAdd( r2, p1, p0 );
			r3 = FusedMulAdd( r3, p1, p0 );
			r4 = FusedMulAdd( r4, p1, p0 );
			r5 = FusedMulAdd( r5, p1, p0 );
		}
	};
	
	template <typename T>
	struct VFloat_FMA_7 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;
		
		static constexpr uint opCount = 28;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = FusedMulAdd( r0, p0, p1 );
			r1 = FusedMulAdd( r1, p0, p1 );
			r2 = FusedMulAdd( r2, p0, p1 );
			r3 = FusedMulAdd( r3, p0, p1 );
			r4 = FusedMulAdd( r4, p0, p1 );
			r5 = FusedMulAdd( r5, p0, p1 );
			r6 = FusedMulAdd( r6, p0, p1 );
			
			r0 = FusedMulAdd( r0, p1, p0 );
			r1 = FusedMulAdd( r1, p1, p0 );
			r2 = FusedMulAdd( r2, p1, p0 );
			r3 = FusedMulAdd( r3, p1, p0 );
			r4 = FusedMulAdd( r4, p1, p0 );
			r5 = FusedMulAdd( r5, p1, p0 );
			r6 = FusedMulAdd( r6, p1, p0 );
		}
	};
	
	template <typename T>
	struct VFloat_FMA_16 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;
		
		T		r8, r9, r10, r11, r12, r13, r14, r15;

		VFloat_FMA_16 ()
		{
			r8  = r0 + r1;
			r9  = r1 + r2;
			r10 = r2 + r3;
			r11 = r4 + r5;
			r12 = r5 + r6;
			r13 = r6 + r7;
			r14 = r7 + r0;
			r15 = r2 + r5;
		}
		
		~VFloat_FMA_16 ()
		{
			r0 = r0 + r8 + r9;
			r1 = r1 + r10 + r11;
			r2 = r2 + r12 + r13;
			r3 = r3 + r14 + r15;
		}

		static constexpr uint opCount = 64;	// flops

		forceinline void  operator () () __NE___
		{
			r0  = FusedMulAdd( r0,  p0, p1 );
			r1  = FusedMulAdd( r1,  p0, p1 );
			r2  = FusedMulAdd( r2,  p0, p1 );
			r3  = FusedMulAdd( r3,  p0, p1 );
			r4  = FusedMulAdd( r4,  p0, p1 );
			r5  = FusedMulAdd( r5,  p0, p1 );
			r6  = FusedMulAdd( r6,  p0, p1 );
			r7  = FusedMulAdd( r7,  p0, p1 );
			r8  = FusedMulAdd( r8,  p0, p1 );
			r9  = FusedMulAdd( r9,  p0, p1 );
			r10 = FusedMulAdd( r10, p0, p1 );
			r11 = FusedMulAdd( r11, p0, p1 );
			r12 = FusedMulAdd( r12, p0, p1 );
			r13 = FusedMulAdd( r13, p0, p1 );
			r14 = FusedMulAdd( r14, p0, p1 );
			r15 = FusedMulAdd( r15, p0, p1 );
			
			r0  = FusedMulAdd( r0,  p1, p0 );
			r1  = FusedMulAdd( r1,  p1, p0 );
			r2  = FusedMulAdd( r2,  p1, p0 );
			r3  = FusedMulAdd( r3,  p1, p0 );
			r4  = FusedMulAdd( r4,  p1, p0 );
			r5  = FusedMulAdd( r5,  p1, p0 );
			r6  = FusedMulAdd( r6,  p1, p0 );
			r7  = FusedMulAdd( r7,  p1, p0 );
			r8  = FusedMulAdd( r8,  p1, p0 );
			r9  = FusedMulAdd( r9,  p1, p0 );
			r10 = FusedMulAdd( r10, p1, p0 );
			r11 = FusedMulAdd( r11, p1, p0 );
			r12 = FusedMulAdd( r12, p1, p0 );
			r13 = FusedMulAdd( r13, p1, p0 );
			r14 = FusedMulAdd( r14, p1, p0 );
			r15 = FusedMulAdd( r15, p1, p0 );
		}
	};
	
	template <typename T>
	struct VFloat_FMA_Add : SimdVal_Base<T>
	{
		REUSE_MEMBERS;
		
		static constexpr uint opCount = 32;	// flops, actualy +16 flops, but keep only FMA flops to compare with other tests

		forceinline void  operator () () __NE___
		{
			r0 = FusedMulAdd( r0, p0, p1 ) + p0;
			r1 = FusedMulAdd( r1, p0, p1 ) + p0;
			r2 = FusedMulAdd( r2, p0, p1 ) + p0;
			r3 = FusedMulAdd( r3, p0, p1 ) + p0;
			r4 = FusedMulAdd( r4, p0, p1 ) + p0;
			r5 = FusedMulAdd( r5, p0, p1 ) + p0;
			r6 = FusedMulAdd( r6, p0, p1 ) + p0;
			r7 = FusedMulAdd( r7, p0, p1 ) + p0;

			r0 = FusedMulAdd( r0, p1, p0 ) + p1;
			r1 = FusedMulAdd( r1, p1, p0 ) + p1;
			r2 = FusedMulAdd( r2, p1, p0 ) + p1;
			r3 = FusedMulAdd( r3, p1, p0 ) + p1;
			r4 = FusedMulAdd( r4, p1, p0 ) + p1;
			r5 = FusedMulAdd( r5, p1, p0 ) + p1;
			r6 = FusedMulAdd( r6, p1, p0 ) + p1;
			r7 = FusedMulAdd( r7, p1, p0 ) + p1;
		}
	};

	template <typename T>
	struct VFloat_Div_8 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;
		
		static constexpr uint opCount = 32;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = p0 / r0;
			r1 = p0 / r1;
			r2 = p0 / r2;
			r3 = p0 / r3;
			r4 = p0 / r4;
			r5 = p0 / r5;
			r6 = p0 / r6;
			r7 = p0 / r7;
			
			r0 = p1 / r0;
			r1 = p1 / r1;
			r2 = p1 / r2;
			r3 = p1 / r3;
			r4 = p1 / r4;
			r5 = p1 / r5;
			r6 = p1 / r6;
			r7 = p1 / r7;
			
			r0 = p0 / r0;
			r1 = p0 / r1;
			r2 = p0 / r2;
			r3 = p0 / r3;
			r4 = p0 / r4;
			r5 = p0 / r5;
			r6 = p0 / r6;
			r7 = p0 / r7;
			
			r0 = p1 / r0;
			r1 = p1 / r1;
			r2 = p1 / r2;
			r3 = p1 / r3;
			r4 = p1 / r4;
			r5 = p1 / r5;
			r6 = p1 / r6;
			r7 = p1 / r7;
		}
	};

	template <typename T>
	struct VFloat_Div_4 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;
		
		static constexpr uint opCount = 32;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = p0 / r0;
			r1 = p0 / r1;
			r2 = p0 / r2;
			r3 = p0 / r3;
			
			r0 = p1 / r0;
			r1 = p1 / r1;
			r2 = p1 / r2;
			r3 = p1 / r3;
			
			r0 = p0 / r0;
			r1 = p0 / r1;
			r2 = p0 / r2;
			r3 = p0 / r3;
			
			r0 = p1 / r0;
			r1 = p1 / r1;
			r2 = p1 / r2;
			r3 = p1 / r3;

			r0 = p0 / r0;
			r1 = p0 / r1;
			r2 = p0 / r2;
			r3 = p0 / r3;
			
			r0 = p1 / r0;
			r1 = p1 / r1;
			r2 = p1 / r2;
			r3 = p1 / r3;
			
			r0 = p0 / r0;
			r1 = p0 / r1;
			r2 = p0 / r2;
			r3 = p0 / r3;
			
			r0 = p1 / r0;
			r1 = p1 / r1;
			r2 = p1 / r2;
			r3 = p1 / r3;
		}
	};

	template <typename T>
	struct VFloat_FastDiv : SimdVal_Base<T>
	{
		REUSE_MEMBERS;
		
		static constexpr uint opCount = 32;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = p0.FastDiv( r0 );
			r1 = p0.FastDiv( r1 );
			r2 = p0.FastDiv( r2 );
			r3 = p0.FastDiv( r3 );
			r4 = p0.FastDiv( r4 );
			r5 = p0.FastDiv( r5 );
			r6 = p0.FastDiv( r6 );
			r7 = p0.FastDiv( r7 );
			
			r0 = p1.FastDiv( r0 );
			r1 = p1.FastDiv( r1 );
			r2 = p1.FastDiv( r2 );
			r3 = p1.FastDiv( r3 );
			r4 = p1.FastDiv( r4 );
			r5 = p1.FastDiv( r5 );
			r6 = p1.FastDiv( r6 );
			r7 = p1.FastDiv( r7 );
			
			r0 = p0.FastDiv( r0 );
			r1 = p0.FastDiv( r1 );
			r2 = p0.FastDiv( r2 );
			r3 = p0.FastDiv( r3 );
			r4 = p0.FastDiv( r4 );
			r5 = p0.FastDiv( r5 );
			r6 = p0.FastDiv( r6 );
			r7 = p0.FastDiv( r7 );
			
			r0 = p1.FastDiv( r0 );
			r1 = p1.FastDiv( r1 );
			r2 = p1.FastDiv( r2 );
			r3 = p1.FastDiv( r3 );
			r4 = p1.FastDiv( r4 );
			r5 = p1.FastDiv( r5 );
			r6 = p1.FastDiv( r6 );
			r7 = p1.FastDiv( r7 );
		}
	};

	template <typename T>
	struct VFloat_FMA_Div : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		T	a0, a1;

		VFloat_FMA_Div ()
		{
			a0 = r0 + r1;
			a1 = r2 + r3;
		}

		~VFloat_FMA_Div ()
		{
			r0 = r0 + a0;
			r1 = r1 + a1;
		}
		
		static constexpr uint opCount = 32;	// flops, actually +2 flops on Div

		forceinline void  operator () () __NE___
		{
			r0 = FusedMulAdd( r0, p0, p1 );
			r1 = FusedMulAdd( r1, p0, p1 );
			r2 = FusedMulAdd( r2, p0, p1 );
			r3 = FusedMulAdd( r3, p0, p1 );
			r4 = FusedMulAdd( r4, p0, p1 );
			r5 = FusedMulAdd( r5, p0, p1 );
			r6 = FusedMulAdd( r6, p0, p1 );
			r7 = FusedMulAdd( r7, p0, p1 );

			a0 = p1 / a0;

			r0 = FusedMulAdd( r0, p1, p0 );
			r1 = FusedMulAdd( r1, p1, p0 );
			r2 = FusedMulAdd( r2, p1, p0 );
			r3 = FusedMulAdd( r3, p1, p0 );
			r4 = FusedMulAdd( r4, p1, p0 );
			r5 = FusedMulAdd( r5, p1, p0 );
			r6 = FusedMulAdd( r6, p1, p0 );
			r7 = FusedMulAdd( r7, p1, p0 );
			
			a1 = p0 / a1;
		}
	};

	template <typename T>
	struct VFloat_FMA_Div2 : SimdVal_Base<T>
	{
		REUSE_MEMBERS;

		T	a0, a1, a2, a3;
		
		VFloat_FMA_Div2 ()
		{
			a0 = r0 + r1;
			a1 = r2 + r3;
			a2 = r4 + r5;
			a3 = r6 + r7;
		}

		~VFloat_FMA_Div2 ()
		{
			r0 = r0 + a0;
			r1 = r1 + a1;
			r2 = r2 + a2;
			r3 = r3 + a3;
		}

		static constexpr uint opCount = 32;	// flops, actually +4 flops on Div

		forceinline void  operator () () __NE___
		{
			r0 = FusedMulAdd( r0, p0, p1 );
			r1 = FusedMulAdd( r1, p0, p1 );
			r2 = FusedMulAdd( r2, p0, p1 );
			r3 = FusedMulAdd( r3, p0, p1 );

			a0 = p0 / a0;

			r4 = FusedMulAdd( r4, p0, p1 );
			r5 = FusedMulAdd( r5, p0, p1 );
			r6 = FusedMulAdd( r6, p0, p1 );
			r7 = FusedMulAdd( r7, p0, p1 );
			
			a1 = p1 / a1;

			r0 = FusedMulAdd( r0, p1, p0 );
			r1 = FusedMulAdd( r1, p1, p0 );
			r2 = FusedMulAdd( r2, p1, p0 );
			r3 = FusedMulAdd( r3, p1, p0 );
			
			a2 = p0 / a2;

			r4 = FusedMulAdd( r4, p1, p0 );
			r5 = FusedMulAdd( r5, p1, p0 );
			r6 = FusedMulAdd( r6, p1, p0 );
			r7 = FusedMulAdd( r7, p1, p0 );
			
			a3 = p1 / a3;
		}
	};

	template <typename T>
	struct VFloat_Sqrt : SimdVal_Base<T>
	{
		REUSE_MEMBERS;
		
		static constexpr uint opCount = 32;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = Sqrt( r0 );
			r1 = Sqrt( r1 );
			r2 = Sqrt( r2 );
			r3 = Sqrt( r3 );
			r4 = Sqrt( r4 );
			r5 = Sqrt( r5 );
			r6 = Sqrt( r6 );
			r7 = Sqrt( r7 );
			
			r0 = Sqrt( r0 );
			r1 = Sqrt( r1 );
			r2 = Sqrt( r2 );
			r3 = Sqrt( r3 );
			r4 = Sqrt( r4 );
			r5 = Sqrt( r5 );
			r6 = Sqrt( r6 );
			r7 = Sqrt( r7 );
			
			r0 = Sqrt( r0 );
			r1 = Sqrt( r1 );
			r2 = Sqrt( r2 );
			r3 = Sqrt( r3 );
			r4 = Sqrt( r4 );
			r5 = Sqrt( r5 );
			r6 = Sqrt( r6 );
			r7 = Sqrt( r7 );
			
			r0 = Sqrt( r0 );
			r1 = Sqrt( r1 );
			r2 = Sqrt( r2 );
			r3 = Sqrt( r3 );
			r4 = Sqrt( r4 );
			r5 = Sqrt( r5 );
			r6 = Sqrt( r6 );
			r7 = Sqrt( r7 );
		}
	};

	template <typename T>
	struct VFloat_FastSqrt : SimdVal_Base<T>
	{
		REUSE_MEMBERS;
		
		static constexpr uint opCount = 32;	// flops

		forceinline void  operator () () __NE___
		{
			r0 = FastSqrt( r0 );
			r1 = FastSqrt( r1 );
			r2 = FastSqrt( r2 );
			r3 = FastSqrt( r3 );
			r4 = FastSqrt( r4 );
			r5 = FastSqrt( r5 );
			r6 = FastSqrt( r6 );
			r7 = FastSqrt( r7 );
			
			r0 = FastSqrt( r0 );
			r1 = FastSqrt( r1 );
			r2 = FastSqrt( r2 );
			r3 = FastSqrt( r3 );
			r4 = FastSqrt( r4 );
			r5 = FastSqrt( r5 );
			r6 = FastSqrt( r6 );
			r7 = FastSqrt( r7 );
			
			r0 = FastSqrt( r0 );
			r1 = FastSqrt( r1 );
			r2 = FastSqrt( r2 );
			r3 = FastSqrt( r3 );
			r4 = FastSqrt( r4 );
			r5 = FastSqrt( r5 );
			r6 = FastSqrt( r6 );
			r7 = FastSqrt( r7 );
			
			r0 = FastSqrt( r0 );
			r1 = FastSqrt( r1 );
			r2 = FastSqrt( r2 );
			r3 = FastSqrt( r3 );
			r4 = FastSqrt( r4 );
			r5 = FastSqrt( r5 );
			r6 = FastSqrt( r6 );
			r7 = FastSqrt( r7 );
		}
	};
	//-------------------------------------------------------------------------


	#if 1
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

			VFloat_Op< VFloat_Mul<T> >( profiler, count, String{typeName} << " - Mul" );
			VFloat_Op< VFloat_MulAdd<T> >( profiler, count, String{typeName} << " - MulAdd" );
			
			if constexpr( HasSimdType<T> ){
				VFloat_Op< VFloat_Add_4<T> >( profiler, count, String{typeName} << " - Add ilp4" );
				VFloat_Op< VFloat_Add_5<T> >( profiler, count, String{typeName} << " - Add ilp5" );
				VFloat_Op< VFloat_Add_6<T> >( profiler, count, String{typeName} << " - Add ilp6" );
				VFloat_Op< VFloat_Add_7<T> >( profiler, count, String{typeName} << " - Add ilp7" );
				VFloat_Op< VFloat_Add_8<T> >( profiler, count, String{typeName} << " - Add ilp8" );

				if constexpr( T::Has_FusedMulAdd() ){
					VFloat_Op< VFloat_FMA_4<T> >( profiler, count, String{typeName} << " - FMA ilp4" );
					VFloat_Op< VFloat_FMA_5<T> >( profiler, count, String{typeName} << " - FMA ilp5" );
					VFloat_Op< VFloat_FMA_6<T> >( profiler, count, String{typeName} << " - FMA ilp6" );
					VFloat_Op< VFloat_FMA_7<T> >( profiler, count, String{typeName} << " - FMA ilp7" );
					VFloat_Op< VFloat_FMA_8<T> >( profiler, count, String{typeName} << " - FMA ilp8" );
					VFloat_Op< VFloat_FMA_16<T> >( profiler, count, String{typeName} << " - FMA ilp16" );

					VFloat_Op< VFloat_FMA_Add<T> >( profiler, count, String{typeName} << " - FMA ilp8 + Add" );
				}
			}else{
				VFloat_Op< VFloat_Add_8<T> >( profiler, count, String{typeName} << " - Add" );
				VFloat_Op< VFloat_FMA_8<T> >( profiler, count, String{typeName} << " - FMA" );
			}
			
		  #if 1
			if constexpr( HasSimdType<T> ){
				if constexpr( T::Has_PreciseDiv() ){
					VFloat_Op< VFloat_Div_4<T> >( profiler, count, String{typeName} << " - Div ilp4" );
					VFloat_Op< VFloat_Div_8<T> >( profiler, count, String{typeName} << " - Div ilp8" );
				}
				if constexpr( T::Has_PreciseDiv() and T::Has_FusedMulAdd() ){
					VFloat_Op< VFloat_FMA_Div<T> >( profiler, count, String{typeName} << " - FMA ilp8 + Div2" );
					VFloat_Op< VFloat_FMA_Div2<T> >( profiler, count, String{typeName} << " - FMA ilp4 + Div4" );
				}
				if constexpr( T::Has_PreciseSqrt() ){
					VFloat_Op< VFloat_Sqrt<T> >( profiler, count, String{typeName} << " - Precise Sqrt" );
				}
				if constexpr( T::Has_ApproxInvSqrt() ){
					VFloat_Op< VFloat_FastSqrt<T> >( profiler, count, String{typeName} << " - Fast Sqrt" );
				}
				if constexpr( T::Has_ApproxReciprocal() ){
					VFloat_Op< VFloat_FastDiv<T> >( profiler, count, String{typeName} << " - Fast Div" );
				}
			}else{
				VFloat_Op< VFloat_Div_8<T> >( profiler, count, String{typeName} << " - Div" );
				VFloat_Op< VFloat_Sqrt<T> >( profiler, count, String{typeName} << " - Precise Sqrt" );
				VFloat_Op< VFloat_FastSqrt<T> >( profiler, count, String{typeName} << " - Fast Sqrt" );
			}
		  #endif
		}

		static void  SIMD_SingleThread () __NE___
		{
			ForEachCoreType(
				[&] (auto& core, Function<void()> setAffinity)
				{
					setAffinity();

					IntervalProfiler	profiler{ "SIMD test, single thread, "s << ToString( core.type ) << " core",
												  IntervalProfiler::EFlags::SortByPerf | IntervalProfiler::EFlags::ExcludePerfDiff };

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

					#if 1
						// Clang converts scalar to SIMD, so test is not correct
						#if not defined(AE_COMPILER_CLANG) or not defined(AE_COMPILER_CLANG_CL)
							TestVFloat< packed_double2 >( profiler, "Scalar Double2" );
							TestVFloat< packed_double4 >( profiler, "Scalar Double4" );
							profiler.PrintAndReset();
						#endif

						#ifdef AE_SIMD_SimdDouble2
							TestVFloat< SimdDouble2 >( profiler, "Simd Double2" );
						#endif
						#ifdef AE_SIMD_SimdDouble4
							TestVFloat< SimdDouble4 >( profiler, "Simd Double4" );
						#endif
						profiler.PrintAndReset();
					#endif
				});
		}
	#endif
	//-------------------------------------------------------------------------

		
	#if 1
		static constexpr uint	max_threads = 4;


		template <typename Op>
		static void  TestVFloatOpMT (IntervalProfiler &profiler, StringView testName, CpuArchInfo::CoreBits_t coreBits) __NE___
		{
			static constexpr usize	count			= c_Repeat / sizeof(typename Op::type);
			static constexpr usize	op_count		= VecSize<typename Op::type> * count;
			const uint				thread_count	= Min( max_threads, uint(coreBits.count()) );
		
			const auto	flops = [](secondsd dt)
			{{
				return ToStringSfx( double(op_count * Op::opCount) / dt.count() ) << "FLOPS";
			}};
			profiler.BeginTest( testName, flops );

			for (uint i = 0; i < c_MaxIter; ++i)
			{
				Barrier		before_test	{thread_count+1};
				Barrier		after_test	{thread_count+1};
				ulong		core_bits	= coreBits.to_ullong();
				
				Array< StdThread >	threads;
				threads.reserve( thread_count );

				for (uint t = 0; t < thread_count; ++t)
				{
					uint	core_id = Base::ExtractBitIndex( INOUT core_bits );
					ASSERT( coreBits.test( core_id ));

					threads.push_back( StdThread{ [&before_test, &after_test, core_id]()
						{
						  #ifdef AE_PLATFORM_APPLE
							ThreadUtils::SetPriority( EThreadPriority::Highest );
						  #else
							ThreadUtils::SetAffinity( core_id );
						  #endif
						  #ifdef AE_SimdRuntimeConfig
							SimdRuntimeConfig::DenormalFlushToZero( true );	// only current thread
						  #endif

							before_test.Wait();
							
							Op	op;

							for (usize z = 0; z < count; ++z)
							{
								op();
							}

							after_test.Wait();
						}});
				}
				ThreadUtils::MilliSleep( milliseconds{10} );

				profiler.BeginIteration();
				before_test.Wait();

				after_test.Wait();
				profiler.EndIteration();

				for (auto& t : threads) {
					t.join();
				}
			}
			profiler.EndTest();
		}

		
		template <typename T>
		static void  TestVFloatMT (IntervalProfiler &profiler, StringView typeName, const CpuArchInfo::CoreBits_t coreBits) __NE___
		{
			if constexpr( HasSimdType<T> ){
				TestVFloatOpMT< VFloat_Add_4<T> >( profiler, String{typeName} << " - Add ilp4", coreBits );
				TestVFloatOpMT< VFloat_Add_5<T> >( profiler, String{typeName} << " - Add ilp5", coreBits );
				TestVFloatOpMT< VFloat_Add_6<T> >( profiler, String{typeName} << " - Add ilp6", coreBits );
				TestVFloatOpMT< VFloat_Add_7<T> >( profiler, String{typeName} << " - Add ilp7", coreBits );
				TestVFloatOpMT< VFloat_Add_8<T> >( profiler, String{typeName} << " - Add ilp8", coreBits );

				if constexpr( T::Has_FusedMulAdd() ){
					TestVFloatOpMT< VFloat_FMA_4<T> >( profiler, String{typeName} << " - FMA ilp4", coreBits );
					TestVFloatOpMT< VFloat_FMA_5<T> >( profiler, String{typeName} << " - FMA ilp5", coreBits );
					TestVFloatOpMT< VFloat_FMA_6<T> >( profiler, String{typeName} << " - FMA ilp6", coreBits );
					TestVFloatOpMT< VFloat_FMA_7<T> >( profiler, String{typeName} << " - FMA ilp7", coreBits );
					TestVFloatOpMT< VFloat_FMA_8<T> >( profiler, String{typeName} << " - FMA ilp8", coreBits );
					TestVFloatOpMT< VFloat_FMA_16<T> >( profiler, String{typeName} << " - FMA ilp16", coreBits );

					TestVFloatOpMT< VFloat_FMA_Add<T> >( profiler, String{typeName} << " - FMA ilp8 + Add", coreBits );
				}
			}else{
				TestVFloatOpMT< VFloat_Add_8<T> >( profiler, String{typeName} << " - Add", coreBits );
				TestVFloatOpMT< VFloat_FMA_8<T> >( profiler, String{typeName} << " - FMA", coreBits );
			}
		}


		static void  SIMD_MultiThread1 (const CpuArchInfo::Core &core) __NE___
		{
			const auto			core_bits	 = core.physicalBits;
			const uint			thread_count = Min( max_threads, uint(core_bits.count()) );
			IntervalProfiler	profiler{ "SIMD test, "s << ToString(thread_count) << "T, " << ToString(thread_count) <<
										  "C, on " << ToString( core.type ) << " core",
										  IntervalProfiler::EFlags::SortByPerf | IntervalProfiler::EFlags::ExcludePerfDiff };
			
			// Clang converts scalar to SIMD, so test is not correct
			#if not (defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL))
				TestVFloatMT< packed_float4 >( profiler, "Scalar Float4", core_bits );
				profiler.PrintAndReset();
			#endif

			#ifdef AE_SIMD_SimdFloat8
				TestVFloatMT< SimdFloat8 >( profiler, "Simd Float8", core_bits );
			#elif defined(AE_SIMD_SimdFloat4)
				TestVFloatMT< SimdFloat4 >( profiler, "Simd Float4", core_bits );
			#endif
			profiler.PrintAndReset();
		}


		static void  SIMD_MultiThread2 (const CpuArchInfo::Core &core) __NE___
		{
			if ( not core.HasVirtualCores() )
				return;

			const auto			core_bits	 = core.logicalBits;
			const uint			thread_count = Min( max_threads, uint(core_bits.count()) );
			IntervalProfiler	profiler{ "SIMD test, "s << ToString(thread_count) << "T, " << ToString(thread_count/2) <<
										  "C, on " << ToString( core.type ) << " core",
										  IntervalProfiler::EFlags::SortByPerf | IntervalProfiler::EFlags::ExcludePerfDiff };
			
			// Clang converts scalar to SIMD, so test is not correct
			#if not (defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL))
				TestVFloatMT< packed_float4 >( profiler, "Scalar Float4", core_bits );
				profiler.PrintAndReset();
			#endif

			#ifdef AE_SIMD_SimdFloat8
				TestVFloatMT< SimdFloat8 >( profiler, "Simd Float8", core_bits );
			#elif defined(AE_SIMD_SimdFloat4)
				TestVFloatMT< SimdFloat4 >( profiler, "Simd Float4", core_bits );
			#endif
			profiler.PrintAndReset();
		}
	#endif
	//-------------------------------------------------------------------------

} // namespace


extern void PerfTest_SIMD ()
{
	#if (defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)) and defined(AE_CFG_RELEASE)
		CHECK_MSG( false, "Clang will use aggressive optimization, results are incorrect" );
	#endif

	SIMD_SingleThread();
	
	ForEachCoreType(
		[&] (auto& core, Function<void()>)
		{
			SIMD_MultiThread1( core );
			SIMD_MultiThread2( core );
		});

	TEST_PASSED();
}
