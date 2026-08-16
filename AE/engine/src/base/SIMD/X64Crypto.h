// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#if AE_SIMD_SSE > 0

namespace AE::Base
{

	//
	// SIMD Crypto
	//

	struct SimdCrypto
	{
		// requires SSE 4.2 feature //
	  #if AE_SIMD_SSE >= 42
		struct CRC32
		{
			// crc + CRC(data)
			ND_ static uint			Accum (uint crc, ubyte  data)							__NE___	{ return _mm_crc32_u8(  crc, data ); }
			ND_ static uint			Accum (uint crc, ushort data)							__NE___	{ return _mm_crc32_u16( crc, data ); }
			ND_ static uint			Accum (uint crc, uint   data)							__NE___	{ return _mm_crc32_u32( crc, data ); }
			ND_ static uint			Accum (uint crc, ulong  data)							__NE___	{ return uint(_mm_crc32_u64( crc, data )); }
		};
		static constexpr CRC32	crc32		= {};
		static constexpr bool	has_crc32	= true;
	  #else
		static constexpr bool	has_crc32	= false;
	  #endif


		// requires 'SHA256' feature //
	  #if AE_SIMD_SHA >= 20 and defined(AE_SIMD_SimdTInt128)
		struct SHA1
		{
			// Perform an Intermediate Calculation for the Next Four SHA1 Message Dwords
			ND_ static SimdUInt4	Next4Msg (const SimdUInt4 &a, const SimdUInt4 &b)		__NE___	{ return SimdUInt4{_mm_sha1msg1_epu32( a.Ref(), b.Ref() )}; }

			// Perform a Final Calculation for the Next Four SHA1 Message Dwords
			ND_ static SimdUInt4	Final4Msg (const SimdUInt4 &a, const SimdUInt4 &b)		__NE___	{ return SimdUInt4{_mm_sha1msg2_epu32( a.Ref(), b.Ref() )}; }

			// Calculate SHA1 State Variable E After Four Rounds
			ND_ static SimdUInt4	NextE (const SimdUInt4 &a, const SimdUInt4 &b)			__NE___	{ return SimdUInt4{_mm_sha1nexte_epu32( a.Ref(), b.Ref() )}; }

			// Perform Four Rounds of SHA1 Operation
			template <uint Idx>
			ND_ static SimdUInt4	Rounds4 (const SimdUInt4 &a, const SimdUInt4 &b)		__NE___	{ StaticAssert( Idx < 4 );  return SimdUInt4{_mm_sha1rnds4_epu32( a.Ref(), b.Ref(), Idx )}; }
		};

		// requires 'SHA256' feature //
		struct SHA2 // SHA2-256
		{
			// Perform an Intermediate Calculation for the Next Four SHA256 Message Dwords
			ND_ static SimdUInt4	Next4Msg (const SimdUInt4 &a, const SimdUInt4 &b)		__NE___	{ return SimdUInt4{_mm_sha256msg1_epu32( a.Ref(), b.Ref() )}; }

			// Perform a Final Calculation for the Next Four SHA256 Message Dwords
			ND_ static SimdUInt4	Final4Msg (const SimdUInt4 &a, const SimdUInt4 &b)		__NE___	{ return SimdUInt4{_mm_sha256msg2_epu32( a.Ref(), b.Ref() )}; }

			// Perform Two Rounds of SHA256 Operation
			ND_ static SimdUInt4	Rounds2 (const SimdUInt4 &a, const SimdUInt4 &b, const SimdUInt4 &c) __NE___ { return SimdUInt4{_mm_sha256rnds2_epu32( a.Ref(), b.Ref(), c.Ref() )}; }
		};

		static constexpr SHA1		sha1		= {};
		static constexpr SHA2		sha2		= {};
		static constexpr bool		has_sha1	= true;
		static constexpr bool		has_sha2	= true;
	  #else
		static constexpr bool		has_sha1	= false;
		static constexpr bool		has_sha2	= false;
	  #endif // AE_SIMD_SHA=20


		// requires 'SHA2_512' feature //
	  #if AE_SIMD_SHA >= 21 and defined(AE_SIMD_SimdTInt256)
		struct SHA2_512
		{
			// Perform an Intermediate Calculation for the Next Four SHA512 Message Qwords
			ND_ static SimdULong4	Next4Msg (const SimdULong4 &a, const SimdULong2 &b)		__NE___	{ return SimdULong4{_mm256_sha512msg1_epi64( a.Ref(), b.Ref() )}; }

			// Perform a Final Calculation for the Next Four SHA512 Message Qwords
			ND_ static SimdULong4	Final4Msg (const SimdULong4 &a, const SimdULong4 &b)	__NE___	{ return SimdULong4{_mm256_sha512msg2_epi64( a.Ref(), b.Ref() )}; }

			// Perform Two Rounds of SHA512 Operation
			ND_ static SimdULong4	Rounds2 (const SimdULong4 &a, const SimdULong4 &b, const SimdULong2 &c) __NE___ { return SimdULong4{_mm256_sha512rnds2_epi64( a.Ref(), b.Ref(), c.Ref() )}; }
		};
		static constexpr SHA2_512	sha512		= {};
		static constexpr bool		has_sha512	= true;
	  #else
		static constexpr bool		has_sha512	= false;
	  #endif // AE_SIMD_SHA=21


		// requires 'AES' feature //
	  #if AE_SIMD_AES
		struct AES
		{
		  #ifdef AE_SIMD_Int128b
			ND_ static Int128b		Encode (const Int128b &v, const Int128b &rkey)			__NE___	{ return Int128b{_mm_aesenc_si128( v.Ref(), rkey.Ref() )}; }
			ND_ static Int128b		Decode (const Int128b &v, const Int128b &rkey)			__NE___	{ return Int128b{_mm_aesdec_si128( v.Ref(), rkey.Ref() )}; }

			ND_ static Int128b		EncodeLast (const Int128b &v, const Int128b &rkey)		__NE___	{ return Int128b{_mm_aesenclast_si128( v.Ref(), rkey.Ref() )}; }
			ND_ static Int128b		DecodeLast (const Int128b &v, const Int128b &rkey)		__NE___	{ return Int128b{_mm_aesdeclast_si128( v.Ref(), rkey.Ref() )}; }

			ND_ static Int128b		InverseMixColumn (const Int128b &v)						__NE___	{ return Int128b{_mm_aesimc_si128( v.Ref() )}; }

			template <int RoundConst>
			ND_ static Int128b		RoundKeyGenAssist (const Int128b &ckey)					__NE___
			{
			  #ifdef AE_COMPILER_MSVC
				return Int128b{_mm_aeskeygenassist( ckey.Ref(), RoundConst )};
			  #else
				return Int128b{_mm_aeskeygenassist_si128( ckey.Ref(), RoundConst )};
			  #endif
			}

			// requires 'vpclmulqdq' CPU feature
		//	template <uint Idx>
		//	ND_ static Int128b		CarryLessIMul (const Int128b &a, const Int128b &b)		__NE___	{ StaticAssert( Idx < 2 );  return Int128b{_mm_clmulepi64_si128( a.Ref(), b.Ref(), Idx )}; }
		  #endif

		  #ifdef AE_SIMD_Int256b
			ND_ static Int256b		Encode (const Int256b &v, const Int256b &rkey)			__NE___	{ return Int256b{_mm256_aesenc_epi128( v.Ref(), rkey.Ref() )}; }
			ND_ static Int256b		Decode (const Int256b &v, const Int256b &rkey)			__NE___	{ return Int256b{_mm256_aesdec_epi128( v.Ref(), rkey.Ref() )}; }

			ND_ static Int256b		EncodeLast (const Int256b &v, const Int256b &rkey)		__NE___	{ return Int256b{_mm256_aesenclast_epi128( v.Ref(), rkey.Ref() )}; }
			ND_ static Int256b		DecodeLast (const Int256b &v, const Int256b &rkey)		__NE___	{ return Int256b{_mm256_aesdeclast_epi128( v.Ref(), rkey.Ref() )}; }
		  #endif

		// requires 'VAES' and 'AVX512VL' features //
		  #if defined(AE_SIMD_Int512b) and (AE_SIMD_AES >= 2)
			ND_ static Int512b		Encode (const Int512b &v, const Int512b &rkey)			__NE___	{ return Int512b{_mm512_aesenc_epi128( v.Ref(), rkey.Ref() )}; }
			ND_ static Int512b		Decode (const Int512b &v, const Int512b &rkey)			__NE___	{ return Int512b{_mm512_aesdec_epi128( v.Ref(), rkey.Ref() )}; }

			ND_ static Int512b		EncodeLast (const Int512b &v, const Int512b &rkey)		__NE___	{ return Int512b{_mm512_aesenclast_epi128( v.Ref(), rkey.Ref() )}; }
			ND_ static Int512b		DecodeLast (const Int512b &v, const Int512b &rkey)		__NE___	{ return Int512b{_mm512_aesdeclast_epi128( v.Ref(), rkey.Ref() )}; }
		  #endif
		};
		static constexpr AES		aes		= {};
		static constexpr bool		has_aes	= true;
	  #else
		static constexpr bool		has_aes	= false;
	  #endif // AE_SIMD_AES
	};


} // AE::Base

#endif // AE_SIMD_SSE
