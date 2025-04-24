// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/MemUtils.h"

namespace AE::Base
{

	void  MemCopy256_CachedLoad_NonCachedStore (OUT void* dst, const void* src, Bytes size)	__NE___;
	void  MemCopy256_NonCached (OUT void* dst, const void* src, Bytes size)					__NE___;
	void  MemCopy256_Cached (OUT void* dst, const void* src, Bytes size)					__NE___;

	void  ZeroMem256_Cached (OUT void* dst, Bytes size)										__NE___;
	void  ZeroMem256_NonCached (OUT void* dst, Bytes size)									__NE___;

	template <typename T>	void  FillMem_Cached (OUT void* dst, Bytes size, T value)		__NE___;
	template <typename T>	void  FillMem_NonCached (OUT void* dst, Bytes size, T value)	__NE___;
	template <typename T>	void  FillMem (OUT void* dst, Bytes size, T value)				__NE___;

	const char*  FindChar_SIMD (char const* begin, char const* end, char ch)				__NE___;
//-----------------------------------------------------------------------------


/*
=================================================
	MemCopy256_CachedLoad_NonCachedStore
----
	Copy memory without storing in cache, performance is limited by RAM bandwidth.
	Faster for large blocks which will be evicted from cache.
	If cache size or block size is not known use 'MemCopy()' instead.
----
	Memory must not intersects.
	Pointers must be aligned to 64 bytes.
	Size must be aligned to 256 bytes. TODO: can be minimized to 64B
=================================================
*/
	forceinline void  MemCopy256_CachedLoad_NonCachedStore (OUT void* inDst, const void* inSrc, const Bytes size) __NE___
	{
		Base::_hidden_::MemCopyChecks( inDst, inSrc, size, 64, 256 );

		#if AE_SIMD_AVX >= 30
			auto*	src	= static_cast<__m512i const *>( inSrc );
			auto*	dst = static_cast<__m512i *>( inDst );
			for (auto* end = dst + size; dst < end;)
			{
				__m512i	r0 = _mm512_load_si512( src+0 );
				__m512i	r1 = _mm512_load_si512( src+1 );
				__m512i	r2 = _mm512_load_si512( src+2 );
				__m512i	r3 = _mm512_load_si512( src+3 );

				_mm512_stream_si512( OUT dst+0, r0 );
				_mm512_stream_si512( OUT dst+1, r1 );
				_mm512_stream_si512( OUT dst+2, r2 );
				_mm512_stream_si512( OUT dst+3, r3 );

				src += 256_b;	dst += 256_b;
			}
			_mm_sfence();

		#elif AE_SIMD_AVX >= 1
			auto*	src	= static_cast<__m256i const *>( inSrc );
			auto*	dst = static_cast<__m256i *>( inDst );
			for (auto* end = dst + size; dst < end;)
			{
				__m256i	r0 = _mm256_load_si256( src+0 );
				__m256i	r1 = _mm256_load_si256( src+1 );
				__m256i	r2 = _mm256_load_si256( src+2 );
				__m256i	r3 = _mm256_load_si256( src+3 );

				_mm256_stream_si256( OUT dst+0, r0 );
				_mm256_stream_si256( OUT dst+1, r1 );
				_mm256_stream_si256( OUT dst+2, r2 );
				_mm256_stream_si256( OUT dst+3, r3 );

				src += 128_b;	dst += 128_b;
			}
			_mm_sfence();

		#elif AE_SIMD_SSE >= 20
			auto*	src	= static_cast<__m128i const *>( inSrc );
			auto*	dst = static_cast<__m128i *>( inDst );
			for (auto* end = dst + size; dst < end;)
			{
				__m128i	r0 = _mm_load_si128( src+0 );
				__m128i	r1 = _mm_load_si128( src+1 );
				__m128i	r2 = _mm_load_si128( src+2 );
				__m128i	r3 = _mm_load_si128( src+3 );

				_mm_stream_si128( OUT dst+0, r0 );
				_mm_stream_si128( OUT dst+1, r1 );
				_mm_stream_si128( OUT dst+2, r2 );
				_mm_stream_si128( OUT dst+3, r3 );

				src += 64_b;	dst += 64_b;
			}
			_mm_sfence();

		#elif AE_SIMD_NEON and AE_HAS_BUILTIN( __builtin_nontemporal_store )
			auto*	src	= static_cast< uint64x2_t const *>( inSrc );
			auto*	dst = static_cast< uint64x2_t *>( inDst );
			for (auto* end = dst + size; dst < end;)
			{
				__builtin_nontemporal_store( src[0], OUT dst+0 );
				__builtin_nontemporal_store( src[1], OUT dst+1 );
				__builtin_nontemporal_store( src[2], OUT dst+2 );
				__builtin_nontemporal_store( src[3], OUT dst+3 );

				src += 64_b;	dst += 64_b;
			}

		#elif AE_SIMD_NEON
			auto*	src	= static_cast< uint64x2_t const *>( inSrc );
			auto*	dst = static_cast< uint64_t *>( inDst );
			for (auto* end = dst + size; dst < end;)
			{
				vst1q_u64( OUT dst+0, src[0] );
				vst1q_u64( OUT dst+2, src[1] );
				vst1q_u64( OUT dst+4, src[2] );
				vst1q_u64( OUT dst+6, src[3] );

				src += 64_b;	dst += 64_b;
			}

			// TODO: __ARM_FEATURE_MOPS from Arm v8.8

		#else
			std::memcpy( OUT AssumeAligned<64>(inDst), AssumeAligned<64>(inSrc), usize(size) );
		#endif
	}

/*
=================================================
	MemCopy256_NonCached
----
	Same as 'MemCopy256_CachedLoad_NonCachedStore()',
	but loaded value will not cause anything to be evicted from the cache.
	Access to the same cache line is fast.
----
	Memory must not intersects.
	Pointers must be aligned to 64 bytes.
	Size must be aligned to 256 bytes.
=================================================
*/
	forceinline void  MemCopy256_NonCached (OUT void* inDst, const void* inSrc, const Bytes size) __NE___
	{
		Base::_hidden_::MemCopyChecks( inDst, inSrc, size, 64, 256 );

		#if AE_SIMD_AVX >= 30
			auto*	src	= static_cast<__m512i const *>( inSrc );
			auto*	dst = static_cast<__m512i *>( inDst );
			for (auto* end = dst + size; dst < end;)
			{
				__m512i	r0 = _mm512_stream_load_si512( src+0 );
				__m512i	r1 = _mm512_stream_load_si512( src+1 );
				__m512i	r2 = _mm512_stream_load_si512( src+2 );
				__m512i	r3 = _mm512_stream_load_si512( src+3 );

				_mm512_stream_si512( OUT dst+0, r0 );
				_mm512_stream_si512( OUT dst+1, r1 );
				_mm512_stream_si512( OUT dst+2, r2 );
				_mm512_stream_si512( OUT dst+3, r3 );

				src += 256_b;	dst += 256_b;
			}
			_mm_sfence();

		#elif AE_SIMD_AVX >= 2
			auto*	src	= static_cast<__m256i const *>( inSrc );
			auto*	dst = static_cast<__m256i *>( inDst );
			for (auto* end = dst + size; dst < end;)
			{
				__m256i	r0 = _mm256_stream_load_si256( src+0 );		// AVX2
				__m256i	r1 = _mm256_stream_load_si256( src+1 );
				__m256i	r2 = _mm256_stream_load_si256( src+2 );
				__m256i	r3 = _mm256_stream_load_si256( src+3 );

				_mm256_stream_si256( OUT dst+0, r0 );
				_mm256_stream_si256( OUT dst+1, r1 );
				_mm256_stream_si256( OUT dst+2, r2 );
				_mm256_stream_si256( OUT dst+3, r3 );

				src += 128_b;	dst += 128_b;
			}
			_mm_sfence();

		#elif AE_SIMD_SSE >= 41
		#  ifdef AE_COMPILER_GCC
			auto*	src	= const_cast< __m128i* >( static_cast<__m128i const *>( inSrc ));
		#  else
			auto*	src	= static_cast<__m128i const *>( inSrc );
		#  endif
			auto*	dst = static_cast<__m128i *>( inDst );
			for (auto* end = dst + size; dst < end;)
			{
				__m128i	r0 = _mm_stream_load_si128( src+0 );
				__m128i	r1 = _mm_stream_load_si128( src+1 );
				__m128i	r2 = _mm_stream_load_si128( src+2 );
				__m128i	r3 = _mm_stream_load_si128( src+3 );

				_mm_stream_si128( OUT dst+0, r0 );
				_mm_stream_si128( OUT dst+1, r1 );
				_mm_stream_si128( OUT dst+2, r2 );
				_mm_stream_si128( OUT dst+3, r3 );

				src += 64_b;	dst += 64_b;
			}
			_mm_sfence();

		#elif AE_SIMD_NEON and AE_HAS_BUILTIN( __builtin_nontemporal_store ) and AE_HAS_BUILTIN( __builtin_nontemporal_load )
			auto*	src	= static_cast< uint64x2_t const *>( inSrc );
			auto*	dst = static_cast< uint64x2_t *>( inDst );
			for (auto* end = dst + size; dst < end;)
			{
				uint64x2_t	r0 = __builtin_nontemporal_load( src+0 );
				uint64x2_t	r1 = __builtin_nontemporal_load( src+1 );
				uint64x2_t	r2 = __builtin_nontemporal_load( src+2 );
				uint64x2_t	r3 = __builtin_nontemporal_load( src+3 );

				__builtin_nontemporal_store( r0, OUT dst+0 );
				__builtin_nontemporal_store( r1, OUT dst+1 );
				__builtin_nontemporal_store( r2, OUT dst+2 );
				__builtin_nontemporal_store( r3, OUT dst+3 );

				src += 64_b;	dst += 64_b;
			}

		#else
			MemCopy256_CachedLoad_NonCachedStore( inDst, inSrc, size );
		#endif
	}

/*
=================================================
	MemCopy256_Cached
----
	Copy memory with writing to cache, performance is limited by L2/L3 cache bandwidth.
	Use it to keep result in cache for further use.
	Faster for blocks which is less than L2/L3 cache size.
	If cache size or block size is not known use 'MemCopy()' instead.
----
	Memory must not intersects.
	Pointers must be aligned to 64 bytes.
	Size must be aligned to 256 bytes.
=================================================
*/
	forceinline void  MemCopy256_Cached (OUT void* inDst, const void* inSrc, const Bytes size) __NE___
	{
		Base::_hidden_::MemCopyChecks( inDst, inSrc, size, 64, 256 );

		#if AE_SIMD_AVX >= 30
			auto*	src	= static_cast<__m512i const *>( inSrc );
			auto*	dst = static_cast<__m512i *>( inDst );
			for (auto* end = dst + size; dst < end;)
			{
				__m512i		r0 = _mm512_load_si512( src+0 );
				__m512i		r1 = _mm512_load_si512( src+1 );
				__m512i		r2 = _mm512_load_si512( src+2 );
				__m512i		r3 = _mm512_load_si512( src+3 );

				_mm512_store_si512( OUT dst+0, r0 );
				_mm512_store_si512( OUT dst+1, r1 );
				_mm512_store_si512( OUT dst+2, r2 );
				_mm512_store_si512( OUT dst+3, r3 );

				src += 256_b;	dst += 256_b;
			}

		#elif AE_SIMD_AVX >= 1
			auto*	src	= static_cast<__m256i const *>( inSrc );
			auto*	dst = static_cast<__m256i *>( inDst );
			for (auto* end = dst + size; dst < end;)
			{
				__m256i		r0 = _mm256_load_si256( src+0 );
				__m256i		r1 = _mm256_load_si256( src+1 );
				__m256i		r2 = _mm256_load_si256( src+2 );
				__m256i		r3 = _mm256_load_si256( src+3 );

				_mm256_store_si256( OUT dst+0, r0 );
				_mm256_store_si256( OUT dst+1, r1 );
				_mm256_store_si256( OUT dst+2, r2 );
				_mm256_store_si256( OUT dst+3, r3 );

				src += 128_b;	dst += 128_b;
			}

		#elif AE_SIMD_SSE >= 20
			auto*	src	= static_cast<__m128i const *>( inSrc );
			auto*	dst = static_cast<__m128i *>( inDst );
			for (auto* end = dst + size; dst < end;)
			{
				__m128i		r0 = _mm_load_si128( src+0 );
				__m128i		r1 = _mm_load_si128( src+1 );
				__m128i		r2 = _mm_load_si128( src+2 );
				__m128i		r3 = _mm_load_si128( src+3 );

				_mm_store_si128( OUT dst+0, r0 );
				_mm_store_si128( OUT dst+1, r1 );
				_mm_store_si128( OUT dst+2, r2 );
				_mm_store_si128( OUT dst+3, r3 );

				src += 64_b;	dst += 64_b;
			}

		#elif AE_SIMD_NEON
			auto*	src	= static_cast< uint64x2_t const *>( inSrc );
			auto*	dst = static_cast< uint64_t *>( inDst );
			for (auto* end = dst + size; dst < end;)
			{
				vst1q_u64( OUT dst+0, src[0] );
				vst1q_u64( OUT dst+2, src[1] );
				vst1q_u64( OUT dst+4, src[2] );
				vst1q_u64( OUT dst+6, src[3] );

				src += 64_b;	dst += 64_b;
			}

		#else
			std::memcpy( OUT AssumeAligned<64>(inDst), AssumeAligned<64>(inSrc), usize(size) );
		#endif
	}

/*
=================================================
	ZeroMem256_Cached
----
	same as 'MemCopy256_Cached', L1 cache is not used.
=================================================
*/
	forceinline void  ZeroMem256_Cached (OUT void* inDst, const Bytes size) __NE___
	{
		Base::_hidden_::ZeroMemChecks( inDst, size, 64, 256 );

		#if AE_SIMD_AVX >= 30
			__m512i	r0  = _mm512_setzero_si512();
			auto*	dst = static_cast<__m512i *>( inDst );
			for (auto* end = dst + size; dst < end; dst += 256_b)
			{
				_mm512_store_si512( OUT dst+0, r0 );
				_mm512_store_si512( OUT dst+1, r0 );
				_mm512_store_si512( OUT dst+2, r0 );
				_mm512_store_si512( OUT dst+3, r0 );
			}

		#elif AE_SIMD_AVX >= 1
			__m256i	r0  = _mm256_setzero_si256();
			auto*	dst = static_cast<__m256i *>( inDst );
			for (auto* end = dst + size; dst < end; dst += 128_b)
			{
				_mm256_store_si256( OUT dst+0, r0 );
				_mm256_store_si256( OUT dst+1, r0 );
				_mm256_store_si256( OUT dst+2, r0 );
				_mm256_store_si256( OUT dst+3, r0 );
			}

		#elif AE_SIMD_SSE >= 20
			__m128i	r0  = _mm_setzero_si128();
			auto*	dst = static_cast<__m128i *>( inDst );
			for (auto* end = dst + size; dst < end; dst += 64_b)
			{
				_mm_store_si128( OUT dst+0, r0 );
				_mm_store_si128( OUT dst+1, r0 );
				_mm_store_si128( OUT dst+2, r0 );
				_mm_store_si128( OUT dst+3, r0 );
			}

		#elif AE_SIMD_NEON
			uint64x2_t	r0  = vdupq_n_u64( 0 );
			auto*		dst = static_cast< uint64_t *>( inDst );
			for (auto* end = dst + size; dst < end; dst += 64_b)
			{
				vst1q_u64( OUT dst+0, r0 );
				vst1q_u64( OUT dst+2, r0 );
				vst1q_u64( OUT dst+4, r0 );
				vst1q_u64( OUT dst+6, r0 );
			}

		#else
			std::memset( OUT AssumeAligned<64>(inDst), 0, usize(size) );
		#endif
	}

/*
=================================================
	ZeroMem256_NonCached
----
	same as 'MemCopy256_NonCached'
=================================================
*/
	forceinline void  ZeroMem256_NonCached (OUT void* inDst, const Bytes size) __NE___
	{
		Base::_hidden_::ZeroMemChecks( inDst, size, 64, 256 );

		#if AE_SIMD_AVX >= 30
			__m512i	r0  = _mm512_setzero_si512();
			auto*	dst = static_cast<__m512i *>( inDst );
			for (auto* end = dst + size; dst < end; dst += 256_b)
			{
				_mm512_stream_si512( OUT dst+0, r0 );
				_mm512_stream_si512( OUT dst+1, r0 );
				_mm512_stream_si512( OUT dst+2, r0 );
				_mm512_stream_si512( OUT dst+3, r0 );
			}
			_mm_sfence();

		#elif AE_SIMD_AVX >= 1
			__m256i	r0  = _mm256_setzero_si256();
			auto*	dst = static_cast<__m256i *>( inDst );
			for (auto* end = dst + size; dst < end; dst += 128_b)
			{
				_mm256_stream_si256( OUT dst+0, r0 );
				_mm256_stream_si256( OUT dst+1, r0 );
				_mm256_stream_si256( OUT dst+2, r0 );
				_mm256_stream_si256( OUT dst+3, r0 );
			}
			_mm_sfence();

		#elif AE_SIMD_SSE >= 20
			__m128i	r0  = _mm_setzero_si128();
			auto*	dst = static_cast<__m128i *>( inDst );
			for (auto* end = dst + size; dst < end; dst += 64_b)
			{
				_mm_stream_si128( OUT dst+0, r0 );
				_mm_stream_si128( OUT dst+1, r0 );
				_mm_stream_si128( OUT dst+2, r0 );
				_mm_stream_si128( OUT dst+3, r0 );
			}
			_mm_sfence();

		#elif AE_SIMD_NEON and AE_HAS_BUILTIN( __builtin_nontemporal_store )
			uint64x2_t	r0  = vdupq_n_u64( 0 );
			auto*		dst = static_cast< uint64x2_t *>( inDst );
			for (auto* end = dst + size; dst < end; dst += 64_b)
			{
				__builtin_nontemporal_store( r0, OUT dst+0 );
				__builtin_nontemporal_store( r0, OUT dst+1 );
				__builtin_nontemporal_store( r0, OUT dst+2 );
				__builtin_nontemporal_store( r0, OUT dst+3 );
			}

		#elif AE_SIMD_NEON
			uint64x2_t	r0  = vdupq_n_u64( 0 );
			auto*		dst = static_cast< uint64_t *>( inDst );
			for (auto* end = dst + size; dst < end; dst += 64_b)
			{
				vst1q_u64( OUT dst+0, r0 );
				vst1q_u64( OUT dst+2, r0 );
				vst1q_u64( OUT dst+4, r0 );
				vst1q_u64( OUT dst+6, r0 );
			}

		#else
			std::memset( OUT AssumeAligned<64>(inDst), 0, usize(size) );
		#endif
	}

/*
=================================================
	FillMem_Cached
=================================================
*/
	template <typename T>
	void  FillMem_Cached (OUT void* inDst, const Bytes size, const T value) __NE___
	{
		StaticAssert( sizeof(T) <= sizeof(ulong) );
		ASSERT( IsMultipleOf( size, sizeof(T) ));

		void* const	aligned_end = inDst + AlignDown( size, sizeof(T) );

		#if AE_SIMD_AVX >= 1
			__m256i	r0;
			if constexpr( sizeof(T) == sizeof(ulong)  )	r0 = _mm256_set1_epi64x( BitCast<ulong >( value ));	else
			if constexpr( sizeof(T) == sizeof(uint)   )	r0 = _mm256_set1_epi32(  BitCast<uint  >( value ));	else
			if constexpr( sizeof(T) == sizeof(ushort) )	r0 = _mm256_set1_epi16(  BitCast<ushort>( value ));	else
			if constexpr( sizeof(T) == sizeof(ubyte)  )	r0 = _mm256_set1_epi8(   BitCast<ubyte >( value ));	else
														r0 = value;	// error

			auto*	dst = static_cast<__m256i *>( inDst );

			// unaligned head
			if_unlikely( not IsMultipleOf( dst, 32_b ))
			{
				_mm256_storeu_si256( OUT dst, r0 );
				dst = AlignUp( dst, 32_b );
			}

			for (auto* end = AlignDown( inDst, 128_b ); dst < end; dst += 128_b)
			{
				_mm256_store_si256( OUT dst+0, r0 );
				_mm256_store_si256( OUT dst+1, r0 );
				_mm256_store_si256( OUT dst+2, r0 );
				_mm256_store_si256( OUT dst+3, r0 );
			}

			// unaligned tail
			for (T* dst_t = Cast<T>(dst); dst_t < aligned_end; ++dst_t)
				*dst_t = value;

		#elif AE_SIMD_SSE >= 20
			__m128i	r0;
			if constexpr( sizeof(T) == sizeof(ulong)  )	r0 = _mm_set1_epi64x( BitCast<ulong >( value ));	else
			if constexpr( sizeof(T) == sizeof(uint)   )	r0 = _mm_set1_epi32(  BitCast<uint  >( value ));	else
			if constexpr( sizeof(T) == sizeof(ushort) )	r0 = _mm_set1_epi16(  BitCast<ushort>( value ));	else
			if constexpr( sizeof(T) == sizeof(ubyte)  )	r0 = _mm_set1_epi8(   BitCast<ubyte >( value ));	else
														r0 = value;	// error

			auto*	dst = static_cast<__m128i *>( inDst );

			// unaligned head
			if_unlikely( not IsMultipleOf( dst, 16_b ))
			{
				_mm_storeu_si128( OUT dst, r0 );
				dst = AlignUp( dst, 16_b );
			}

			for (auto* end = AlignDown( inDst, 64_b ); dst < end; dst += 64_b)
			{
				_mm_store_si128( OUT dst+0, r0 );
				_mm_store_si128( OUT dst+1, r0 );
				_mm_store_si128( OUT dst+2, r0 );
				_mm_store_si128( OUT dst+3, r0 );
			}

			// unaligned tail
			for (T* dst_t = Cast<T>(dst); dst_t < aligned_end; ++dst_t)
				*dst_t = value;

		#else // fallback
			T*	dst = Cast<T>( inDst );

			for (; dst < aligned_end; ++dst)
				*dst = value;

		#endif
	}

/*
=================================================
	FillMem_NonCached
=================================================
*/
	template <typename T>
	void  FillMem_NonCached (OUT void* inDst, const Bytes size, const T value) __NE___
	{
		StaticAssert( sizeof(T) <= sizeof(ulong) );
		ASSERT( IsMultipleOf( size, sizeof(T) ));

		void* const	aligned_end = inDst + AlignDown( size, sizeof(T) );

		#if AE_SIMD_AVX >= 1
			__m256i	r0;
			if constexpr( sizeof(T) == sizeof(ulong)  )	r0 = _mm256_set1_epi64x( BitCast<ulong >( value ));	else
			if constexpr( sizeof(T) == sizeof(uint)   )	r0 = _mm256_set1_epi32(  BitCast<uint  >( value ));	else
			if constexpr( sizeof(T) == sizeof(ushort) )	r0 = _mm256_set1_epi16(  BitCast<ushort>( value ));	else
			if constexpr( sizeof(T) == sizeof(ubyte)  )	r0 = _mm256_set1_epi8(   BitCast<ubyte >( value ));	else
														r0 = value;	// error

			auto*	dst = static_cast<__m256i *>( inDst );

			// unaligned head
			if_unlikely( not IsMultipleOf( dst, 32_b ))
			{
				_mm256_storeu_si256( OUT dst, r0 );
				// TODO: fence?

				dst = AlignUp( dst, 32_b );
			}

			for (auto* end = AlignDown( inDst, 128_b ); dst < end; dst += 128_b)
			{
				_mm256_stream_si256( OUT dst+0, r0 );
				_mm256_stream_si256( OUT dst+1, r0 );
				_mm256_stream_si256( OUT dst+2, r0 );
				_mm256_stream_si256( OUT dst+3, r0 );
			}
			_mm_sfence();

			// unaligned tail
			for (T* dst_t = Cast<T>(dst); dst_t < aligned_end; ++dst_t)
				*dst_t = value;

		#elif AE_SIMD_SSE >= 20
			__m128i	r0;
			if constexpr( sizeof(T) == sizeof(ulong)  )	r0 = _mm_set1_epi64x( BitCast<ulong >( value ));	else
			if constexpr( sizeof(T) == sizeof(uint)   )	r0 = _mm_set1_epi32(  BitCast<uint  >( value ));	else
			if constexpr( sizeof(T) == sizeof(ushort) )	r0 = _mm_set1_epi16(  BitCast<ushort>( value ));	else
			if constexpr( sizeof(T) == sizeof(ubyte)  )	r0 = _mm_set1_epi8(   BitCast<ubyte >( value ));	else
														r0 = value;	// error

			auto*	dst = static_cast<__m128i *>( inDst );

			// unaligned head
			if_unlikely( not IsMultipleOf( dst, 16_b ))
			{
				_mm_storeu_si128( OUT dst, r0 );
				// TODO: fence?

				dst = AlignUp( dst, 16_b );
			}

			for (auto* end = AlignDown( inDst, 64_b ); dst < end; dst += 64_b)
			{
				_mm_stream_si128( OUT dst+0, r0 );
				_mm_stream_si128( OUT dst+1, r0 );
				_mm_stream_si128( OUT dst+2, r0 );
				_mm_stream_si128( OUT dst+3, r0 );
			}
			_mm_sfence();

			// unaligned tail
			for (T* dst_t = Cast<T>(dst); dst_t < aligned_end; ++dst_t)
				*dst_t = value;

		#else // fallback
			T*	dst = Cast<T>( inDst );

			for (; dst < aligned_end; ++dst)
				*dst = value;

		#endif
	}

/*
=================================================
	FillMem
=================================================
*/
	template <typename T>
	void  FillMem (OUT void* dst, Bytes size, T value) __NE___
	{
		// use cached version if block size less than approximate L3 size
		if ( size < 2_MiB )
			return FillMem_Cached( OUT dst, size, value );
		else
			return FillMem_NonCached( OUT dst, size, value );
	}

/*
=================================================
	FindChar_SIMD
----
	AVX2 is 4-5 times faster than std::string::find()
	Neon is 50% faster than std::string::find()
=================================================
*/
namespace _hidden_
{
#if AE_SIMD_AVX >= 31	// AVX512_BW
	Nd__IF const char*  FindChar_AVX512 (char const* str, char const* const end, const char ch) __NE___
	{
		if_likely( str+64_b <= end )
		{
			const __m512i	v_ch = _mm512_set1_epi8( ch );

			// unaligned head
			{
				__m512i		s	= _mm512_loadu_si512( NonAlignedCast<__m512i>( str ));
				__mmask64	m	= _mm512_cmpeq_epi8_mask( s, v_ch );

				cold_if( m )
					return str + CountRZero( m );

				str = AlignUp( str, 64_b );
			}

			for_likely(; str+256_b <= end; str += 256_b)
			{
				__m512i		s0	= _mm512_load_si512( Cast<__m512i>( str ) + 0 );
				__m512i		s1	= _mm512_load_si512( Cast<__m512i>( str ) + 1 );
				__m512i		s2	= _mm512_load_si512( Cast<__m512i>( str ) + 2 );
				__m512i		s3	= _mm512_load_si512( Cast<__m512i>( str ) + 3 );

				__mmask64	m0	= _mm512_cmpeq_epi8_mask( s0, v_ch );
				__mmask64	m1	= _mm512_cmpeq_epi8_mask( s1, v_ch );
				__mmask64	m2	= _mm512_cmpeq_epi8_mask( s2, v_ch );
				__mmask64	m3	= _mm512_cmpeq_epi8_mask( s3, v_ch );

				cold_if( m0 | m1 | m2 | m3 )
				{
					if ( m0 )	return str + CountRZero( m0 );
					if ( m1 )	return str + CountRZero( m1 ) + 64;
					if ( m2 )	return str + CountRZero( m2 ) + 128;
					if ( m3 )	return str + CountRZero( m3 ) + 192;
				}
			}
		}

		// unaligned tail
		for_likely(; str < end; ++str)
		{
			cold_if( *str == ch )
				return str;
		}
		return end;
	}
#endif

#if AE_SIMD_AVX >= 2
	Nd__IF const char*  FindChar_AVX2 (char const* str, char const* const end, const char ch) __NE___
	{
		if_likely( str+32_b <= end )
		{
			const __m256i	v_ch = _mm256_set1_epi8( ch );

			// unaligned head, perf overhead: 2-5%
			{
				__m256i		s	= _mm256_loadu_si256( NonAlignedCast<__m256i>( str ));
				__m256i		eq	= _mm256_cmpeq_epi8( s, v_ch );
				uint		m	= _mm256_movemask_epi8( eq );

				cold_if( m )
					return str + CountRZero( m );

				str = AlignUp( str, 32_b );
			}

			for_likely(; str+128_b <= end; str += 128_b)
			{
				// MSVC:  always used unaligned load
				// Clang: load ignored, used cmp with memory

				// ILP=4 add 40% of perf
				__m256i		s0	= _mm256_load_si256( Cast<__m256i>( str ) + 0 );
				__m256i		s1	= _mm256_load_si256( Cast<__m256i>( str ) + 1 );
				__m256i		s2	= _mm256_load_si256( Cast<__m256i>( str ) + 2 );
				__m256i		s3	= _mm256_load_si256( Cast<__m256i>( str ) + 3 );

				__m256i		eq0	= _mm256_cmpeq_epi8( s0, v_ch );
				__m256i		eq1	= _mm256_cmpeq_epi8( s1, v_ch );
				__m256i		eq2	= _mm256_cmpeq_epi8( s2, v_ch );
				__m256i		eq3	= _mm256_cmpeq_epi8( s3, v_ch );

				uint		m0	= _mm256_movemask_epi8( eq0 );
				uint		m1	= _mm256_movemask_epi8( eq1 );
				uint		m2	= _mm256_movemask_epi8( eq2 );
				uint		m3	= _mm256_movemask_epi8( eq3 );

				cold_if( m0 | m1 | m2 | m3 )
				{
					if ( m0 )	return str + CountRZero( m0 );
					if ( m1 )	return str + CountRZero( m1 ) + 32;
					if ( m2 )	return str + CountRZero( m2 ) + 64;
					if ( m3 )	return str + CountRZero( m3 ) + 96;
				}
			}
		}

		// unaligned tail
		for_likely(; str < end; ++str)
		{
			cold_if( *str == ch )
				return str;
		}
		return end;
	}
#endif

#if AE_SIMD_SSE >= 20
	Nd__IF const char*  FindChar_SSE2 (char const* str, char const* const end, const char ch) __NE___
	{
		if_likely( str+16_b <= end )
		{
			const __m128i	v_ch = _mm_set1_epi8( ch );

			// unaligned head
			{
				__m128i		s	= _mm_loadu_si128( NonAlignedCast<__m128i>( str ));
				__m128i		eq	= _mm_cmpeq_epi8( s, v_ch );
				uint		m	= _mm_movemask_epi8( eq );

				cold_if( m )
					return str + CountRZero( m );

				str = AlignUp( str, 16_b );
			}

			for_likely(; str+64_b <= end; str += 64_b)
			{
				__m128i		s0	= _mm_load_si128( Cast<__m128i>( str ) + 0 );
				__m128i		s1	= _mm_load_si128( Cast<__m128i>( str ) + 1 );
				__m128i		s2	= _mm_load_si128( Cast<__m128i>( str ) + 2 );
				__m128i		s3	= _mm_load_si128( Cast<__m128i>( str ) + 3 );

				__m128i		eq0	= _mm_cmpeq_epi8( s0, v_ch );
				__m128i		eq1	= _mm_cmpeq_epi8( s1, v_ch );
				__m128i		eq2	= _mm_cmpeq_epi8( s2, v_ch );
				__m128i		eq3	= _mm_cmpeq_epi8( s3, v_ch );

				uint		m0	= _mm_movemask_epi8( eq0 );
				uint		m1	= _mm_movemask_epi8( eq1 );
				uint		m2	= _mm_movemask_epi8( eq2 );
				uint		m3	= _mm_movemask_epi8( eq3 );

				cold_if( m0 | m1 | m2 | m3 )
				{
					if ( m0 )	return str + CountRZero( m0 );
					if ( m1 )	return str + CountRZero( m1 ) + 16;
					if ( m2 )	return str + CountRZero( m2 ) + 32;
					if ( m3 )	return str + CountRZero( m3 ) + 48;
				}
			}
		}

		// unaligned tail
		for_likely(; str < end; ++str)
		{
			cold_if( *str == ch )
				return str;
		}
		return end;
	}
#endif

#if AE_SIMD_NEON
	Nd__IF const char*  FindChar_NEON (char const* str, char const* const end, const char ch) __NE___
	{
		const ubyte			vals[]	= { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
		const uint8x16_t	v_mask0	= vmvnq_u8( vld1q_u8(vals) );
		const uint8x16_t	v_mask1	= vmvnq_u8( vaddq_u8( vld1q_u8(vals), vdupq_n_u8(16) ));
		const uint8x16_t	v_mask2	= vmvnq_u8( vaddq_u8( vld1q_u8(vals), vdupq_n_u8(32) ));
		const uint8x16_t	v_mask3	= vmvnq_u8( vaddq_u8( vld1q_u8(vals), vdupq_n_u8(48) ));
		const uint8x16_t	v_ch	= vdupq_n_u8( ch );

		for_likely(; str+64_b <= end; str += 64_b)
		{
			// ILP=4 add 40% of perf
			uint8x16_t	s0	= vld1q_u8( Cast<ubyte>( str ));
			uint8x16_t	s1	= vld1q_u8( Cast<ubyte>( str + 16_b ));
			uint8x16_t	s2	= vld1q_u8( Cast<ubyte>( str + 32_b ));
			uint8x16_t	s3	= vld1q_u8( Cast<ubyte>( str + 48_b ));

			uint8x16_t	eq0	= vceqq_u8( s0, v_ch );
			uint8x16_t	eq1	= vceqq_u8( s1, v_ch );
			uint8x16_t	eq2	= vceqq_u8( s2, v_ch );
			uint8x16_t	eq3	= vceqq_u8( s3, v_ch );

			uint8x16_t	i0	= vandq_u8( eq0, v_mask0 );	// [xFF..xF0] or 0 if not found
			uint8x16_t	i1	= vandq_u8( eq1, v_mask1 );
			uint8x16_t	i2	= vandq_u8( eq2, v_mask2 );
			uint8x16_t	i3	= vandq_u8( eq3, v_mask3 );

						i0	= vmaxq_u8( i0, i1 );
						i1	= vmaxq_u8( i2, i3 );
						i0	= vmaxq_u8( i0, i1 );

		  #ifdef AE_CPU_ARCH_ARM64
			uint		i	= vmaxvq_u8( i0 );
			cold_if( i )
				return str + (~i & 0xFF);

		  #else
			uint8x8_t	j	= vmax_u8( vget_low_u8( i0 ), vget_high_u8( i0 ));
			ulong		i	= vget_lane_u64( vreinterpret_u64_s8( j ), 0 );
			cold_if( i )
			{
				ubyte	ids[8];
				vst1_u8( OUT ids, vmvn_u8( j ));	// [0..63] or xFF if not found

				ids[0] = Min( ids[0], ids[1] );
				ids[2] = Min( ids[2], ids[3] );
				ids[4] = Min( ids[4], ids[5] );
				ids[6] = Min( ids[6], ids[7] );

				ids[0] = Min( ids[0], ids[2] );
				ids[4] = Min( ids[4], ids[6] );

				return str + Min( ids[0], ids[4] );
			}
		  #endif
		}

		// unaligned tail
		for_likely(; str < end; ++str)
		{
			cold_if( *str == ch )
				return str;
		}
		return end;
	}
#endif
}

	Nd__IF const char*  FindChar_SIMD (char const* const begin, char const* const end, const char ch) __NE___
	{
		#if AE_SIMD_AVX >= 31  // AVX512_BW
			return Base::_hidden_::FindChar_AVX512( begin, end, ch );

		#elif AE_SIMD_AVX >= 2
			return Base::_hidden_::FindChar_AVX2( begin, end, ch );

		#elif AE_SIMD_SSE >= 20
			return Base::_hidden_::FindChar_SSE2( begin, end, ch );

		#elif AE_SIMD_NEON
			return Base::_hidden_::FindChar_NEON( begin, end, ch );

		#else
			const char* p = Cast<char>( std::memchr( begin, ch, end - begin ));
			return (p != null ? p : end);
		#endif
	}


} // AE::Base
