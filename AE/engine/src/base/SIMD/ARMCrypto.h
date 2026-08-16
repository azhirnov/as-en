// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Pending features:
		__ARM_FEATURE_SHA512 - SHA2-512
		__ARM_FEATURE_SM3, __ARM_FEATURE_SM4, __ARM_FEATURE_SVE2_SHA3
*/

#pragma once

#ifdef AE_CPU_ARCH_ARM_BASED

namespace AE::Base
{

	//
	// SIMD Crypto
	//

	struct SimdCrypto
	{
		// requires 'CRC32' feature, added to Armv7 //
      #ifdef __ARM_FEATURE_CRC32
		struct CRC32
		{
			// crc + CRC(data)
			ND_ static uint			Accum (uint crc, ubyte  data)						__NE___	{ return __crc32cb( crc, data ); }
			ND_ static uint			Accum (uint crc, ushort data)						__NE___	{ return __crc32ch( crc, data ); }
			ND_ static uint			Accum (uint crc, uint   data)						__NE___	{ return __crc32cw( crc, data ); }
			ND_ static uint			Accum (uint crc, ulong  data)						__NE___	{ return __crc32cd( crc, data ); }
		};
		static constexpr CRC32	crc32		= {};
		static constexpr bool	has_crc32	= true;
	  #else
		static constexpr bool	has_crc32	= false;
      #endif


		// requires 'SHA2' feature, added to Armv8-A //
	  #if defined(__ARM_FEATURE_SHA2) and defined(AE_SIMD_SimdTInt128)
		struct SHA1
		{
			ND_ static uint			FixedRotate (uint a)								__NE___	{ return vsha1h_u32( a ); }

			// SHA1 hash update (majority).
			ND_ static SimdUInt4	UpdateMaj (SimdUInt4 a, uint b, SimdUInt4 c)		__NE___	{ return SimdUInt4{ vsha1mq_u32( a.Ref(), b, c.Ref() )}; }

			// SHA1 hash update (parity).
			ND_ static SimdUInt4	UpdatePar (SimdUInt4 a, uint b, SimdUInt4 c)		__NE___	{ return SimdUInt4{ vsha1pq_u32( a.Ref(), b, c.Ref() )}; }

			// SHA1 schedule update 0.
			ND_ static SimdUInt4	SheduleUpd0 (SimdUInt4 a, SimdUInt4 b, SimdUInt4 c)	__NE___	{ return SimdUInt4{ vsha1su0q_u32( a.Ref(), b.Ref(), c.Ref() )}; }

			// SHA1 schedule update 1.
			ND_ static SimdUInt4	SheduleUpd1 (SimdUInt4 a, SimdUInt4 b)				__NE___	{ return SimdUInt4{ vsha1su1q_u32( a.Ref(), b.Ref() )}; }
		};

		struct SHA2	// SHA2-256
		{
			// SHA256 hash update (part 1).
			ND_ static SimdUInt4	Update1 (SimdUInt4 a, SimdUInt4 b, SimdUInt4 c)		__NE___	{ return SimdUInt4{ vsha256hq_u32( a.Ref(), b.Ref(), c.Ref() )}; }

			// SHA256 hash update (part 2).
			ND_ static SimdUInt4	Update2 (SimdUInt4 a, SimdUInt4 b, SimdUInt4 c)		__NE___	{ return SimdUInt4{ vsha256h2q_u32( a.Ref(), b.Ref(), c.Ref() )}; }

			// SHA256 schedule update 0.
			ND_ static SimdUInt4	SheduleUpd0 (SimdUInt4 a, SimdUInt4 b)				__NE___	{ return SimdUInt4{ vsha256su0q_u32( a.Ref(), b.Ref() )}; }

			// SHA256 schedule update 1.
			ND_ static SimdUInt4	SheduleUpd1 (SimdUInt4 a, SimdUInt4 b, SimdUInt4 c)	__NE___	{ return SimdUInt4{ vsha256su1q_u32( a.Ref(), b.Ref(), c.Ref() )}; }
		};

		static constexpr SHA1	sha1		= {};
		static constexpr SHA2	sha2		= {};
		static constexpr bool	has_sha1	= true;
		static constexpr bool	has_sha2	= true;
	  #else
		static constexpr bool	has_sha1	= false;
		static constexpr bool	has_sha2	= false;
	  #endif


		// requires 'SHA3' feature, added to Armv8.2-A //
	  #if defined(__ARM_FEATURE_SHA3) and defined(AE_SIMD_SimdTInt128)
		struct SHA3
		{
			// SHA512 hash update (part 1).
			ND_ static SimdULong2	Update1 (SimdULong2 a, SimdULong2 b, SimdULong2 c)		__NE___	{ return SimdULong2{ vsha512hq_u64( a.Ref(), b.Ref(), c.Ref() )}; }

			// SHA512 hash update (part 2).
			ND_ static SimdULong2	Update2 (SimdULong2 a, SimdULong2 b, SimdULong2 c)		__NE___	{ return SimdULong2{ vsha512h2q_u64( a.Ref(), b.Ref(), c.Ref() )}; }

			// SHA256 schedule update 0.
			ND_ static SimdULong2	SheduleUpd0 (SimdULong2 a, SimdULong2 b)				__NE___	{ return SimdULong2{ vsha512su0q_u64( a.Ref(), b.Ref() )}; }

			// SHA256 schedule update 1.
			ND_ static SimdULong2	SheduleUpd1 (SimdULong2 a, SimdULong2 b, SimdULong2 c)	__NE___	{ return SimdULong2{ vsha512su1q_u64( a.Ref(), b.Ref(), c.Ref() )}; }
		};
		static constexpr SHA3	sha3		= {};
		static constexpr bool	has_sha3	= true;
	  #else
		static constexpr bool	has_sha3	= false;
	  #endif


		// requires 'SVE_AES' feature //
	  #if defined(__ARM_FEATURE_AES) and defined(AE_SIMD_Int128b)
		struct AES
		{
			ND_ static Int128b		Encode (Int128b a, Int128b b)						__NE___	{ return Int128b{ vaeseq_u8( a.Ref(), b.Ref() )}; }
			ND_ static Int128b		Decode (Int128b a, Int128b b)						__NE___	{ return Int128b{ vaesdq_u8( a.Ref(), b.Ref() )}; }

			ND_ static Int128b		MixColumns (Int128b a)								__NE___	{ return Int128b{ vaesmcq_u8( a.Ref() )}; }
			ND_ static Int128b		InvMixColumns (Int128b a)							__NE___	{ return Int128b{ vaesimcq_u8( a.Ref() )}; }
		};
		static constexpr AES	aes		= {};
		static constexpr bool	has_aes	= true;
	  #else
		static constexpr bool	has_aes	= false;
	  #endif
	};


} // AE::Base

#endif // AE_CPU_ARCH_ARM_BASED
