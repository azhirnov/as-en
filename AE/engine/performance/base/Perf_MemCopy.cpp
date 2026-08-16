// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	[results](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-cpu/MemAccess.md)
*/

#include "Perf_Common.h"

namespace
{
#ifdef AE_PLATFORM_ANDROID
	static constexpr Bytes		c_BufSize	= 512_MiB;
#else
	static constexpr Bytes		c_BufSize	= 1_GiB;
#endif
	static constexpr Bytes		c_BufAlign	= SmallAllocationSize;


#if AE_SIMD_SSE >= 20
	static void  MemCopy16_SSE_v1 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		for (usize i = 0; i < usize(size);)
		{
			__m128i	r0 = _mm_load_si128( static_cast<__m128i const *>(src) );

			_mm_storeu_si128( OUT static_cast<__m128i *>(dst), r0 );
		//	_mm_stream_si128( OUT static_cast<__m128i *>(dst), r0 );

			i += 16;
			src += 16_b;
			dst += 16_b;
		}
	}

	static void  MemCopy16_SSE_v2 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		__m128i		r0, r1, r2, r3;	// max 8 for x86, max 16 for x64

		// ILP=4
		for (usize i = 0; i < usize(size);)
		{
			r0 = _mm_loadu_si128( static_cast<__m128i const *>(src)+0 );
			r1 = _mm_loadu_si128( static_cast<__m128i const *>(src)+1 );
			r2 = _mm_loadu_si128( static_cast<__m128i const *>(src)+2 );
			r3 = _mm_loadu_si128( static_cast<__m128i const *>(src)+3 );

			_mm_storeu_si128( OUT static_cast<__m128i *>(dst)+0, r0 );
			_mm_storeu_si128( OUT static_cast<__m128i *>(dst)+1, r1 );
			_mm_storeu_si128( OUT static_cast<__m128i *>(dst)+2, r2 );
			_mm_storeu_si128( OUT static_cast<__m128i *>(dst)+3, r3 );

			i += 64;
			src += 64_b;
			dst += 64_b;
		}
	}

	static void  MemCopy16_SSE_v3 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		__m128i		r0, r1, r2, r3;	// max 8 for x86, max 16 for x64

		// ILP=4
		for (usize i = 0; i < usize(size);)
		{
			r0 = _mm_loadu_si128( static_cast<__m128i const *>(src)+0 );
			r1 = _mm_loadu_si128( static_cast<__m128i const *>(src)+1 );
			r2 = _mm_loadu_si128( static_cast<__m128i const *>(src)+2 );
			r3 = _mm_loadu_si128( static_cast<__m128i const *>(src)+3 );

			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+0, r0 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+1, r1 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+2, r2 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+3, r3 );

			i += 64;
			src += 64_b;
			dst += 64_b;
		}
		_mm_sfence();
	}

	static void  MemCopy16_SSE_v4 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		__m128i		r0, r1, r2, r3;	// max 8 for x86, max 16 for x64

		// ILP=4
		for (usize i = 0; i < usize(size);)
		{
			r0 = _mm_loadu_si128( static_cast<__m128i const *>(src)+0 );
			r1 = _mm_loadu_si128( static_cast<__m128i const *>(src)+1 );
			r2 = _mm_loadu_si128( static_cast<__m128i const *>(src)+2 );
			r3 = _mm_loadu_si128( static_cast<__m128i const *>(src)+3 );

			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+0, r0 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+1, r1 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+2, r2 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+3, r3 );

			r0 = _mm_loadu_si128( static_cast<__m128i const *>(src)+4 );
			r1 = _mm_loadu_si128( static_cast<__m128i const *>(src)+5 );
			r2 = _mm_loadu_si128( static_cast<__m128i const *>(src)+6 );
			r3 = _mm_loadu_si128( static_cast<__m128i const *>(src)+7 );

			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+4, r0 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+5, r1 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+6, r2 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+7, r3 );

			i += 128;
			src += 128_b;
			dst += 128_b;
		}
		_mm_sfence();
	}

	static void  MemCopy16_SSE_v5 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		__m128i		r0, r1, r2, r3;	// max 8 for x86, max 16 for x64

		// ILP=4
		for (usize i = 0; i < usize(size);)
		{
			r0 = _mm_load_si128( static_cast<__m128i const *>(src)+0 );
			r1 = _mm_load_si128( static_cast<__m128i const *>(src)+1 );
			r2 = _mm_load_si128( static_cast<__m128i const *>(src)+2 );
			r3 = _mm_load_si128( static_cast<__m128i const *>(src)+3 );

			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+0, r0 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+1, r1 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+2, r2 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+3, r3 );

			r0 = _mm_load_si128( static_cast<__m128i const *>(src)+4 );
			r1 = _mm_load_si128( static_cast<__m128i const *>(src)+5 );
			r2 = _mm_load_si128( static_cast<__m128i const *>(src)+6 );
			r3 = _mm_load_si128( static_cast<__m128i const *>(src)+7 );

			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+4, r0 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+5, r1 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+6, r2 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+7, r3 );

			i += 128;
			src += 128_b;
			dst += 128_b;
		}
		_mm_sfence();
	}

	static void  MemCopy16_SSE_v6 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		__m128i		r0, r1, r2, r3, r4, r5, r6, r7;	// max 8 for x86, max 16 for x64

		// ILP=4
		for (usize i = 0; i < usize(size);)
		{
			r0 = _mm_loadu_si128( static_cast<__m128i const *>(src)+0 );
			r1 = _mm_loadu_si128( static_cast<__m128i const *>(src)+1 );
			r2 = _mm_loadu_si128( static_cast<__m128i const *>(src)+2 );
			r3 = _mm_loadu_si128( static_cast<__m128i const *>(src)+3 );
			r4 = _mm_loadu_si128( static_cast<__m128i const *>(src)+4 );
			r5 = _mm_loadu_si128( static_cast<__m128i const *>(src)+5 );
			r6 = _mm_loadu_si128( static_cast<__m128i const *>(src)+6 );
			r7 = _mm_loadu_si128( static_cast<__m128i const *>(src)+7 );

			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+0, r0 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+1, r1 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+2, r2 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+3, r3 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+4, r4 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+5, r5 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+6, r6 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+7, r7 );

			i += 128;
			src += 128_b;
			dst += 128_b;
		}
		_mm_sfence();
	}

	static void  MemCopy16_SSE_v7 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		// ILP=4
		for (usize i = 0; i < usize(size);)
		{
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+0, static_cast<__m128i const *>(src)[0] );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+1, static_cast<__m128i const *>(src)[1] );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+2, static_cast<__m128i const *>(src)[2] );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+3, static_cast<__m128i const *>(src)[3] );

			i += 64;
			src += 64_b;
			dst += 64_b;
		}
		_mm_sfence();
	}


  #if AE_SIMD_SSE >= 41
	static void  MemCopy16_SSE_v8 (OUT void* dst, const void* inSrc, const Bytes size) __NE___
	{
		__m128i		r0, r1, r2, r3;
		auto*		src = const_cast< __m128i* >( static_cast<__m128i const *>( inSrc ));

		for (usize i = 0; i < usize(size);)
		{
			r0 = _mm_stream_load_si128( src+0 );
			r1 = _mm_stream_load_si128( src+1 );
			r2 = _mm_stream_load_si128( src+2 );
			r3 = _mm_stream_load_si128( src+3 );

			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+0, r0 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+1, r1 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+2, r2 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+3, r3 );

			i += 64;
			src += 64_b;
			dst += 64_b;
		}
		_mm_sfence();
	}
  #endif


	static void  MemSet16_SSE_v1 (OUT void* dst, const Bytes size) __NE___
	{
		__m128i		r0 = _mm_setzero_si128();

		for (usize i = 0; i < usize(size);)
		{
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+0, r0 );

			i += 16;
			dst += 16_b;
		}
		_mm_sfence();
	}

	static void  MemSet16_SSE_v2 (OUT void* dst, const Bytes size) __NE___
	{
		__m128i		r0 = _mm_setzero_si128();

		for (usize i = 0; i < usize(size);)
		{
			_mm_storeu_si128( OUT static_cast<__m128i *>(dst)+0, r0 );

			i += 16;
			dst += 16_b;
		}
	}

	static void  MemSet16_SSE_v3 (OUT void* dst, const Bytes size) __NE___
	{
		__m128i		r0 = _mm_setzero_si128();

		for (usize i = 0; i < usize(size);)
		{
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+0, r0 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+1, r0 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+2, r0 );
			_mm_stream_si128( OUT static_cast<__m128i *>(dst)+3, r0 );

			i += 64;
			dst += 64_b;
		}
		_mm_sfence();
	}

	static void  MemSet16_SSE_v4 (OUT void* dst, const Bytes size) __NE___
	{
		__m128i		r0 = _mm_setzero_si128();

		for (usize i = 0; i < usize(size);)
		{
			_mm_storeu_si128( OUT static_cast<__m128i *>(dst)+0, r0 );
			_mm_storeu_si128( OUT static_cast<__m128i *>(dst)+1, r0 );
			_mm_storeu_si128( OUT static_cast<__m128i *>(dst)+2, r0 );
			_mm_storeu_si128( OUT static_cast<__m128i *>(dst)+3, r0 );

			i += 64;
			dst += 64_b;
		}
		_mm_sfence();
	}
#endif

#if AE_SIMD_AVX >= 2
	static void  MemCopy32_AVX_v1 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		for (usize i = 0; i < usize(size);)
		{
			__m256i	r0 = _mm256_load_si256( static_cast<__m256i const *>(src) );

			_mm256_stream_si256( OUT static_cast<__m256i *>(dst), r0 );

			i += 32;
			src += 32_b;
			dst += 32_b;
		}
		_mm_sfence();
	}

	static void  MemCopy32_AVX_v2 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		__m256i		r0, r1, r2, r3;	// max 16 for x64

		// ILP=4
		for (usize i = 0; i < usize(size);)
		{
			i += 128;

			r0 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+0 );
			r1 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+1 );
			r2 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+2 );
			r3 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+3 );
			src += 128_b;

			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+0, r0 );
			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+1, r1 );
			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+2, r2 );
			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+3, r3 );
			dst += 128_b;
		}
	}

	static void  MemCopy32_AVX_v3 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		__m256i		r0, r1, r2, r3;	// max 16 for x64

		// ILP=4
		for (usize i = 0; i < usize(size);)
		{
			r0 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+0 );
			r1 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+1 );
			r2 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+2 );
			r3 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+3 );

			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+0, r0 );
			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+1, r1 );
			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+2, r2 );
			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+3, r3 );

			r0 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+4 );
			r1 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+5 );
			r2 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+6 );
			r3 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+7 );

			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+4, r0 );
			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+5, r1 );
			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+6, r2 );
			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+7, r3 );

			i += 256;
			src += 256_b;
			dst += 256_b;
		}
	}


	static void  MemCopy32_AVX_v4 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		__m256i		r0, r1, r2, r3;	// max 16 for x64

		// ILP=4
		for (usize i = 0; i < usize(size);)
		{
			i += 128;

			r0 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+0 );
			r1 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+1 );
			r2 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+2 );
			r3 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+3 );
			src += 128_b;

			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+0, r0 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+1, r1 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+2, r2 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+3, r3 );
			dst += 128_b;
		}
		_mm_sfence();
	}

	// same as std::memcpy
	static void  MemCopy32_AVX_v5 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		__m256i		r0, r1, r2, r3;	// max 16 for x64

		// ILP=4
		for (usize i = 0; i < usize(size);)
		{
			r0 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+0 );
			r1 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+1 );
			r2 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+2 );
			r3 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+3 );

			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+0, r0 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+1, r1 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+2, r2 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+3, r3 );

			r0 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+4 );
			r1 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+5 );
			r2 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+6 );
			r3 = _mm256_loadu_si256( static_cast<__m256i const *>(src)+7 );

			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+4, r0 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+5, r1 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+6, r2 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+7, r3 );

			i += 256;
			src += 256_b;
			dst += 256_b;
		}
		_mm_sfence();
	}

	static void  MemCopy32_AVX_v6 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		// ILP=4
		for (usize i = 0; i < usize(size);)
		{
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+0, static_cast<__m256i const *>(src)[0] );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+1, static_cast<__m256i const *>(src)[1] );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+2, static_cast<__m256i const *>(src)[2] );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+3, static_cast<__m256i const *>(src)[3] );

			i += 128;
			src += 128_b;
			dst += 128_b;
		}
		_mm_sfence();
	}


	static void  MemCopy32_AVX_v7 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		__m256i		r0, r1, r2, r3;

		for (usize i = 0; i < usize(size);)
		{
			r0 = _mm256_stream_load_si256( static_cast<__m256i const *>(src)+0 );
			r1 = _mm256_stream_load_si256( static_cast<__m256i const *>(src)+1 );
			r2 = _mm256_stream_load_si256( static_cast<__m256i const *>(src)+2 );
			r3 = _mm256_stream_load_si256( static_cast<__m256i const *>(src)+3 );

			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+0, r0 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+1, r1 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+2, r2 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+3, r3 );

			i += 128;
			src += 128_b;
			dst += 128_b;
		}
		_mm_sfence();
	}


	static void  MemSet32_AVX_v1 (OUT void* dst, const Bytes size) __NE___
	{
		__m256i	r0 = _mm256_setzero_si256();

		for (usize i = 0; i < usize(size);)
		{
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst), r0 );

			i += 32;
			dst += 32_b;
		}
		_mm_sfence();
	}

	static void  MemSet32_AVX_v2 (OUT void* dst, const Bytes size) __NE___
	{
		__m256i	r0 = _mm256_setzero_si256();

		for (usize i = 0; i < usize(size);)
		{
			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst), r0 );

			i += 32;
			dst += 32_b;
		}
	}

	static void  MemSet32_AVX_v3 (OUT void* dst, const Bytes size) __NE___
	{
		__m256i	r0 = _mm256_setzero_si256();

		for (usize i = 0; i < usize(size);)
		{
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+0, r0 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+1, r0 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+2, r0 );
			_mm256_stream_si256( OUT static_cast<__m256i *>(dst)+3, r0 );

			i += 128;
			dst += 128_b;
		}
		_mm_sfence();
	}

	static void  MemSet32_AVX_v4 (OUT void* dst, const Bytes size) __NE___
	{
		__m256i	r0 = _mm256_setzero_si256();

		for (usize i = 0; i < usize(size);)
		{
			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+0, r0 );
			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+1, r0 );
			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+2, r0 );
			_mm256_storeu_si256( OUT static_cast<__m256i *>(dst)+3, r0 );

			i += 128;
			dst += 128_b;
		}
	}
#endif

#if AE_SIMD_NEON
	static void  MemCopy16_Neon_v1 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		for (usize i = 0; i < usize(size);)
		{
			uint64x2_t	r0 = vld1q_u64( static_cast<uint64_t const *>(src) );

			vst1q_u64( OUT static_cast<uint64_t *>(dst), r0 );

			i += 16;
			src += 16_b;
			dst += 16_b;
		}
	}

	static void  MemCopy16_Neon_v2 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		uint64x2_t		r0, r1, r2, r3;		// 16-32 registers

		// ILP=4
		for (usize i = 0; i < usize(size);)
		{
			r0 = vld1q_u64( static_cast<uint64_t const *>(src)+0 );
			r1 = vld1q_u64( static_cast<uint64_t const *>(src)+2 );
			r2 = vld1q_u64( static_cast<uint64_t const *>(src)+4 );
			r3 = vld1q_u64( static_cast<uint64_t const *>(src)+6 );

			vst1q_u64( OUT static_cast<uint64_t *>(dst)+0, r0 );
			vst1q_u64( OUT static_cast<uint64_t *>(dst)+2, r1 );
			vst1q_u64( OUT static_cast<uint64_t *>(dst)+4, r2 );
			vst1q_u64( OUT static_cast<uint64_t *>(dst)+6, r3 );

			i += 64;
			src += 64_b;
			dst += 64_b;
		}
	}

	static void  MemCopy16_Neon_v3 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		uint64x2_t		r0, r1, r2, r3, r4, r5, r6, r7;		// 16-32 registers

		// ILP=4
		for (usize i = 0; i < usize(size);)
		{
			r0 = vld1q_u64( static_cast<uint64_t const *>(src)+0 );
			r1 = vld1q_u64( static_cast<uint64_t const *>(src)+2 );
			r2 = vld1q_u64( static_cast<uint64_t const *>(src)+4 );
			r3 = vld1q_u64( static_cast<uint64_t const *>(src)+6 );
			r4 = vld1q_u64( static_cast<uint64_t const *>(src)+8 );
			r5 = vld1q_u64( static_cast<uint64_t const *>(src)+10 );
			r6 = vld1q_u64( static_cast<uint64_t const *>(src)+12 );
			r7 = vld1q_u64( static_cast<uint64_t const *>(src)+14 );

			vst1q_u64( OUT static_cast<uint64_t *>(dst)+0, r0 );
			vst1q_u64( OUT static_cast<uint64_t *>(dst)+2, r1 );
			vst1q_u64( OUT static_cast<uint64_t *>(dst)+4, r2 );
			vst1q_u64( OUT static_cast<uint64_t *>(dst)+6, r3 );
			vst1q_u64( OUT static_cast<uint64_t *>(dst)+8, r4 );
			vst1q_u64( OUT static_cast<uint64_t *>(dst)+10, r5 );
			vst1q_u64( OUT static_cast<uint64_t *>(dst)+12, r6 );
			vst1q_u64( OUT static_cast<uint64_t *>(dst)+14, r7 );

			i += 128;
			src += 128_b;
			dst += 128_b;
		}
	}

	static void  MemCopy16_Neon_v4 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		for (usize i = 0; i < usize(size);)
		{
			vst1q_u64( OUT static_cast<uint64_t *>(dst), static_cast<uint64x2_t const *>(src)[0] );

			i += 16;
			src += 16_b;
			dst += 16_b;
		}
	}

	static void  MemCopy16_Neon_v5 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		for (usize i = 0; i < usize(size);)
		{
			vst1q_u64( OUT static_cast<uint64_t *>(dst)+0, static_cast<uint64x2_t const *>(src)[0] );
			vst1q_u64( OUT static_cast<uint64_t *>(dst)+2, static_cast<uint64x2_t const *>(src)[1] );
			vst1q_u64( OUT static_cast<uint64_t *>(dst)+4, static_cast<uint64x2_t const *>(src)[2] );
			vst1q_u64( OUT static_cast<uint64_t *>(dst)+6, static_cast<uint64x2_t const *>(src)[3] );

			i += 64;
			src += 64_b;
			dst += 64_b;
		}
	}

	static void  MemSet16_Neon_v1 (OUT void* inDst, const Bytes size) __NE___
	{
		uint64x2_t	r0  = vdupq_n_u64( 0 );
		auto*		dst = static_cast< uint64_t *>( inDst );
		for (auto* end = dst + size; dst < end; dst += 64_b)
		{
			vst1q_u64( OUT dst+0, r0 );
			vst1q_u64( OUT dst+2, r0 );
			vst1q_u64( OUT dst+4, r0 );
			vst1q_u64( OUT dst+6, r0 );
		}
	}
#endif


#if AE_SIMD_NEON and AE_HAS_BUILTIN( __builtin_nontemporal_store )

	static void  MemCopy16_Neon_v6 (OUT void* inDst, const void* inSrc, const Bytes size) __NE___
	{
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
	}

	static void  MemCopy16_Neon_v7 (OUT void* inDst, const void* inSrc, const Bytes size) __NE___
	{
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
	}


	static void  MemSet16_Neon_v5 (OUT void* inDst, const Bytes size) __NE___
	{
		uint64x2_t	r0  = vdupq_n_u64( 0 );
		auto*		dst = static_cast< uint64x2_t *>( inDst );
		for (auto* end = dst + size; dst < end; dst += 64_b)
		{
			__builtin_nontemporal_store( r0, OUT dst+0 );
			__builtin_nontemporal_store( r0, OUT dst+1 );
			__builtin_nontemporal_store( r0, OUT dst+2 );
			__builtin_nontemporal_store( r0, OUT dst+3 );
		}
	}
#endif


#ifdef AE_COMPILER_MSVC
	static void  MemCopy_Movsb (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		__movsb( OUT Cast<unsigned char>(dst), Cast<unsigned char>(src), usize{size} );
	}
#endif


#ifdef AE_CPU_ARCH_ARM_BASED

	static void  MemCopy16_STM_v1 (OUT void* dst, const void* src, const Bytes size) __NE___
	{
		for (usize i = 0; i < usize(size);)
		{
			// TODO: https://github.com/ssvb/tinymembench
		}
	}

#endif
//-----------------------------------------------------------------------------


	template <uint V>
	static void  Test_MemSet (IntervalProfiler &profiler, StringView version, RstPtr<void> data, const Bytes size, const usize count)
	{
		AE_LOGI( version );
		profiler.BeginTest( version, [s=size*count](secondsd dt) { return ToStringSfx( double(usize(s)) / dt.count() ) << "B/s"; });

		HashVal	h;

		for (uint i = 0; i < 10; ++i)
		{
			profiler.BeginIteration();

			for (uint j = 0; j < count; ++j)
			{
				if constexpr( V == 0 )		std::memset( OUT data, int(i), usize(size) );
				if constexpr( V == 1 )		ZeroMem256_Cached( OUT data, size );
				if constexpr( V == 2 )		ZeroMem256_NonCached( OUT data, size );

				// SSE
			  #if AE_SIMD_SSE >= 20
				if constexpr( V == 0x11 )	MemSet16_SSE_v1( OUT data, size );
				if constexpr( V == 0x12 )	MemSet16_SSE_v2( OUT data, size );
				if constexpr( V == 0x13 )	MemSet16_SSE_v3( OUT data, size );
				if constexpr( V == 0x14 )	MemSet16_SSE_v4( OUT data, size );
			  #endif

				// AVX
			  #if AE_SIMD_AVX >= 2
				if constexpr( V == 0x21 )	MemSet32_AVX_v1( OUT data, size );
				if constexpr( V == 0x22 )	MemSet32_AVX_v2( OUT data, size );
				if constexpr( V == 0x23 )	MemSet32_AVX_v3( OUT data, size );
				if constexpr( V == 0x24 )	MemSet32_AVX_v4( OUT data, size );
			  #endif

				// NEON
			  #if AE_SIMD_NEON
				if constexpr( V == 0x31 )	MemSet16_Neon_v1( OUT data, size );
			  #endif
			  #if AE_SIMD_NEON and AE_HAS_BUILTIN( __builtin_nontemporal_store )
				if constexpr( V == 0x35 )	MemSet16_Neon_v5( OUT data, size );
			  #endif
			}

			profiler.EndIteration();

			h += HashOf( data, usize(size) );
		}

		profiler.EndTest();
	}


	template <uint V>
	static void  Test_StdMemCopy (IntervalProfiler &profiler, StringView version, RstPtr<void> data0, RstPtr<void> data1, const Bytes size, const usize count)
	{
		AE_LOGI( "std::memcpy - "s << version );
		profiler.BeginTest( "std::memcpy - "s << version, [s=size*count](secondsd dt) { return ToStringSfx( double(usize(s)) / dt.count() ) << "B/s"; });

		HashVal	h;

		for (uint i = 0; i < 10; ++i)
		{
			std::memset( OUT data0, int(i), usize(size) );
			MemoryBarrier( EMemoryOrder::AcquireRelease );

			profiler.BeginIteration();

			for (uint j = 0; j < count; ++j)
			{
				if constexpr( V == 0 )	std::memcpy( OUT data1, data0, usize(size) );
				if constexpr( V == 1 )	std::memcpy( OUT AssumeAligned<64>(data1.get()), AssumeAligned<64>(data0.get()), usize(size) );
				if constexpr( V == 2 )	std::memcpy( OUT AssumeAligned<256>(data1.get()), AssumeAligned<256>(data0.get()), usize(size) );

				Swap( data1, data0 );
			}

			profiler.EndIteration();

			h += HashOf( data1, usize(size) );
		}

		profiler.EndTest();
	}


	template <uint V>
	static void  Test_MemCopy16 (IntervalProfiler &profiler, StringView version, RstPtr<void> data0, RstPtr<void> data1, const Bytes size, const usize count)
	{
		AE_LOGI( "MemCopy16 - "s << version );
		profiler.BeginTest( "MemCopy16 - "s << version, [s=size*count](secondsd dt) { return ToStringSfx( double(usize(s)) / dt.count() ) << "B/s"; });

		HashVal	h;

		for (uint i = 0; i < 10; ++i)
		{
			std::memset( OUT data0, int(i), usize(size) );
			MemoryBarrier( EMemoryOrder::AcquireRelease );

			profiler.BeginIteration();

			for (uint j = 0; j < count; ++j)
			{
				// SSE
			  #if AE_SIMD_SSE >= 20
				if constexpr( V == 0x11 )	MemCopy16_SSE_v1( OUT data1, data0, size );
				if constexpr( V == 0x12 )	MemCopy16_SSE_v2( OUT data1, data0, size );
				if constexpr( V == 0x13 )	MemCopy16_SSE_v3( OUT data1, data0, size );
				if constexpr( V == 0x14 )	MemCopy16_SSE_v4( OUT data1, data0, size );
				if constexpr( V == 0x15 )	MemCopy16_SSE_v5( OUT data1, data0, size );
				if constexpr( V == 0x16 )	MemCopy16_SSE_v6( OUT data1, data0, size );
				if constexpr( V == 0x17 )	MemCopy16_SSE_v7( OUT data1, data0, size );
					MemCopy16_SSE_v7( OUT data1, data0, size );
				if constexpr( V == 0x18 )	MemCopy16_SSE_v8( OUT data1, data0, size );
			  #endif

				// NEON
			  #if AE_SIMD_NEON
				if constexpr( V == 0x21 )	MemCopy16_Neon_v1( OUT data1, data0, size );
				if constexpr( V == 0x22 )	MemCopy16_Neon_v2( OUT data1, data0, size );
				if constexpr( V == 0x23 )	MemCopy16_Neon_v3( OUT data1, data0, size );
				if constexpr( V == 0x24 )	MemCopy16_Neon_v4( OUT data1, data0, size );
				if constexpr( V == 0x25 )	MemCopy16_Neon_v5( OUT data1, data0, size );
			  #endif
			  #if AE_SIMD_NEON and AE_HAS_BUILTIN( __builtin_nontemporal_store )
				if constexpr( V == 0x26 )	MemCopy16_Neon_v6( OUT data1, data0, size );
				if constexpr( V == 0x27 )	MemCopy16_Neon_v7( OUT data1, data0, size );
			  #endif

				// LDM / STM
			  #ifdef AE_CPU_ARCH_ARM_BASED
				//if constexpr( V == 0x31 )	MemCopy16_STM_v1( OUT data1, data0, size );
			  #endif

				Swap( data1, data0 );
			}

			profiler.EndIteration();

			h += HashOf( data1, usize(size) );
		}

		profiler.EndTest();
	}


	template <uint V>
	static void  Test_MemCopy32 (IntervalProfiler &profiler, StringView version, RstPtr<void> data0, RstPtr<void> data1, const Bytes size, const usize count)
	{
		AE_LOGI( "MemCopy32 - "s << version );
		profiler.BeginTest( "MemCopy32 - "s << version, [s=size*count](secondsd dt) { return ToStringSfx( double(usize(s)) / dt.count() ) << "B/s"; });

		HashVal	h;

		for (uint i = 0; i < 10; ++i)
		{
			std::memset( OUT data0, int(i), usize(size) );
			MemoryBarrier( EMemoryOrder::AcquireRelease );

			profiler.BeginIteration();

			for (uint j = 0; j < count; ++j)
			{
				// AVX
			  #if AE_SIMD_AVX >= 2
				if constexpr( V == 0x11 )	MemCopy32_AVX_v1( OUT data1, data0, size );
				if constexpr( V == 0x12 )	MemCopy32_AVX_v2( OUT data1, data0, size );
				if constexpr( V == 0x13 )	MemCopy32_AVX_v3( OUT data1, data0, size );
				if constexpr( V == 0x14 )	MemCopy32_AVX_v4( OUT data1, data0, size );
				if constexpr( V == 0x15 )	MemCopy32_AVX_v5( OUT data1, data0, size );
				if constexpr( V == 0x16 )	MemCopy32_AVX_v6( OUT data1, data0, size );
				if constexpr( V == 0x17 )	MemCopy32_AVX_v7( OUT data1, data0, size );
			  #endif

				Swap( data1, data0 );
			}

			profiler.EndIteration();

			h += HashOf( data1, usize(size) );
		}

		profiler.EndTest();
	}


	template <uint V>
	static void  Test_MemCopy64 (IntervalProfiler &profiler, StringView version, RstPtr<void> data0, RstPtr<void> data1, const Bytes size, const usize count)
	{
		AE_LOGI( "MemCopy64 - "s << version );
		profiler.BeginTest( "MemCopy64 - "s << version, [s=size*count](secondsd dt) { return ToStringSfx( double(usize(s)) / dt.count() ) << "B/s"; });

		HashVal	h;

		for (uint i = 0; i < 10; ++i)
		{
			std::memset( OUT data0, int(i), usize(size) );
			MemoryBarrier( EMemoryOrder::AcquireRelease );

			profiler.BeginIteration();

			for (uint j = 0; j < count; ++j)
			{
				if constexpr( V == 0 )		MemCopy256_NonCached( OUT data1, data0, size );
				if constexpr( V == 1 )		MemCopy256_Cached( OUT data1, data0, size );

			  #ifdef AE_COMPILER_MSVC
				if constexpr( V == 0x30 )	MemCopy_Movsb( OUT data1, data0, size );
			  #endif

				Swap( data1, data0 );
			}

			profiler.EndIteration();

			h += HashOf( data1, usize(size) );
		}

		profiler.EndTest();
	}


	static void  MemCopyTests (RstPtr<void> data0, RstPtr<void> data1, const Bytes size, const usize count)
	{
		ForEachCoreType(
			[&] (auto& core, Function<void()> setAffinity)
			{
				setAffinity();

				#if 1
				{
					IntervalProfiler	profiler{ "MemSet test on "s << ToString( core.type ) << " core, block: " << ToString( size )};

					Test_MemSet<0>( profiler, "std::memset", data0, size, count );
					Test_MemSet<1>( profiler, "ZeroMem256_Cached", data0, size, count );
					Test_MemSet<2>( profiler, "ZeroMem256_NonCached", data0, size, count );

					#if AE_SIMD_SSE >= 20
						Test_MemSet<0x11>( profiler, "memset sse v1", data0, size, count );
						Test_MemSet<0x12>( profiler, "memset sse v2", data0, size, count );
						Test_MemSet<0x13>( profiler, "memset sse v3", data0, size, count );
						Test_MemSet<0x14>( profiler, "memset sse v4", data0, size, count );
					#endif
					#if AE_SIMD_AVX >= 2
						Test_MemSet<0x21>( profiler, "memset avx v1", data0, size, count );
						Test_MemSet<0x22>( profiler, "memset avx v2", data0, size, count );
						Test_MemSet<0x23>( profiler, "memset avx v3", data0, size, count );
						Test_MemSet<0x24>( profiler, "memset avx v4", data0, size, count );
					#endif
					#if AE_SIMD_NEON
						Test_MemSet<0x31>( profiler, "memset neon v1", data0, size, count );
						//Test_MemSet<0x32>( profiler, "memset neon v2", data0, size, count );
						//Test_MemSet<0x33>( profiler, "memset neon v3", data0, size, count );
						//Test_MemSet<0x34>( profiler, "memset neon v4", data0, size, count );
					#endif
					#if AE_SIMD_NEON and AE_HAS_BUILTIN( __builtin_nontemporal_store )
						Test_MemSet<0x35>( profiler, "memset neon v5", data0, size, count );
					#endif
				}
				#endif

				#if 1
				{
					IntervalProfiler	profiler{ "MemCopy test on "s << ToString( core.type ) << " core, block: " << ToString( size )};

					Test_StdMemCopy<0>( profiler, "",			data0, data1, size, count );
					Test_StdMemCopy<1>( profiler, "align64",	data0, data1, size, count );
					Test_StdMemCopy<2>( profiler, "align256",	data0, data1, size, count );

					Test_MemCopy64<0>( profiler, "non-cached",	data0, data1, size, count );
					Test_MemCopy64<1>( profiler, "cached",		data0, data1, size, count );

					#ifdef AE_COMPILER_MSVC
						Test_MemCopy64<0x30>( profiler, "movsb", data0, data1, size, count );
					#endif
					#if AE_SIMD_SSE >= 20
						Test_MemCopy16<0x11>( profiler, "sse v1", data0, data1, size, count );
						Test_MemCopy16<0x12>( profiler, "sse v2", data0, data1, size, count );
						Test_MemCopy16<0x13>( profiler, "sse v3", data0, data1, size, count );
						Test_MemCopy16<0x14>( profiler, "sse v4", data0, data1, size, count );
						Test_MemCopy16<0x15>( profiler, "sse v5", data0, data1, size, count );
						Test_MemCopy16<0x16>( profiler, "sse v6", data0, data1, size, count );
						Test_MemCopy16<0x17>( profiler, "sse v7", data0, data1, size, count );
						Test_MemCopy16<0x18>( profiler, "sse v8", data0, data1, size, count );
					#endif
					#if AE_SIMD_NEON
						Test_MemCopy16<0x21>( profiler, "neon v1", data0, data1, size, count );
						Test_MemCopy16<0x22>( profiler, "neon v2", data0, data1, size, count );
						Test_MemCopy16<0x23>( profiler, "neon v3", data0, data1, size, count );
						Test_MemCopy16<0x24>( profiler, "neon v4", data0, data1, size, count );
						Test_MemCopy16<0x25>( profiler, "neon v5", data0, data1, size, count );
					#endif
					#if AE_SIMD_NEON and AE_HAS_BUILTIN( __builtin_nontemporal_store )
						Test_MemCopy16<0x26>( profiler, "neon v6", data0, data1, size, count );
						Test_MemCopy16<0x27>( profiler, "neon v7", data0, data1, size, count );
					#endif
					#if 0 //def AE_CPU_ARCH_ARM_BASED
						Test_MemCopy16<0x31>( profiler, "stm v1", data0, data1, size, count );
					#endif
					#if AE_SIMD_AVX >= 2
						Test_MemCopy32<0x11>( profiler, "avx v1", data0, data1, size, count );
						Test_MemCopy32<0x12>( profiler, "avx v2", data0, data1, size, count );
						Test_MemCopy32<0x13>( profiler, "avx v3", data0, data1, size, count );
						Test_MemCopy32<0x14>( profiler, "avx v4", data0, data1, size, count );
						Test_MemCopy32<0x15>( profiler, "avx v5", data0, data1, size, count );
						Test_MemCopy32<0x16>( profiler, "avx v6", data0, data1, size, count );
						Test_MemCopy32<0x17>( profiler, "avx v7", data0, data1, size, count );
					#endif
				}
				#endif
			});
	}

} // namespace


extern void PerfTest_MemCopy ()
{
	DynUntypedStorage	st0, st1;
	TEST( st0.Alloc( c_BufSize, c_BufAlign, null ));
	TEST( st1.Alloc( c_BufSize, c_BufAlign, null ));
	CHECK_Eq( c_BufSize, st0.Size() );

	MemCopyTests( st0.Data(), st1.Data(), Min( c_BufSize, st0.Size() ),	4 );

  #ifdef AE_PLATFORM_ANDROID
	MemCopyTests( st0.Data(), st1.Data(), Min( 512_KiB, st0.Size() ),	1000 );
	MemCopyTests( st0.Data(), st1.Data(), Min( 4_KiB, st0.Size() ),		100000 );
  #else
	MemCopyTests( st0.Data(), st1.Data(), Min( 4_MiB, st0.Size() ),		100 );
	MemCopyTests( st0.Data(), st1.Data(), Min( 4_KiB, st0.Size() ),		10000 );
  #endif

	TEST_PASSED();
}
