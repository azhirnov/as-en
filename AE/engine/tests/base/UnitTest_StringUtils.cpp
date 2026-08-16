// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

namespace
{
	static void  StringUtils_FindChar ()
	{
		String	s;
		s.resize( 1024 + 3, '1' );

		for (usize offset = 0; offset < 128; ++offset)
		{
			for (usize i = 256; i < s.size(); ++i)
			{
				s[i] = 'A';

				const char*	p = FindChar( s.data()+offset, s.data()+s.size(), 'A' );
				TEST_Eq( usize(p - s.data()), i );

				s[i] = '1';
			}
		}
	}


	static void  StringUtils_IsNullTerminated ()
	{
		const char	s [] = "iwuwei dksam\0sdfsdfsdf";
		StringView	a0 { s, CountOf(s)-2 };
		StringView	a1 { s, 12 };
		TEST( not IsNullTerminated( a0 ));
		TEST( IsNullTerminated( a1 ));
	}


	static void  StringUtils_EqualIC ()
	{
		const char	a0 [] = "11aabbcc";
		const char	a1 [] = "11AaBBcC";

		TEST( EqualIC( StringView{a0}, StringView{a1} ));
		TEST( not EqualIC( StringView{a0}.substr(1), StringView{a1} ));
		TEST( not EqualIC( StringView{a0}.substr(0, 7), StringView{a1} ));
	}


	static void  StringUtils_FindString1 ()
	{
		const char	a0 [] = "oqkkmxaskjndqkas123-567dn;q'smfadjksajdnalkmsa";
		const char	a1 [] = "123-567";
		const char	a2 [] = "9123-";
		const char 	a3 [] = "  fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ht syscall nx mmxext fxsr_opt pdpe1gb rdtscp lm constant_tsc rep_good amd_lbr_v2 nopl xtopology nonstop_tsc cpuid extd_apicid aperfmperf rapl pni pclmulqdq monitor ssse3 fma cx16 sse4_1 sse4_2 x2apic movbe popcnt aes xsave avx f16c rdrand lahf_lm cmp_legacy svm extapic cr8_legacy abm sse4a misalignsse 3dnowprefetch osvw ibs skinit wdt tce topoext perfctr_core perfctr_nb bpext perfctr_llc mwaitx cpb cat_l3 cdp_l3 hw_pstate ssbd mba perfmon_v2 ibrs ibpb stibp ibrs_enhanced vmmcall fsgsbase bmi1 avx2 smep bmi2 erms invpcid cqm rdt_a avx512f avx512dq rdseed adx smap avx512ifma clflushopt clwb avx512cd sha_ni avx512bw avx512vl xsaveopt xsavec xgetbv1 xsaves cqm_llc cqm_occup_llc cqm_mbm_total cqm_mbm_local user_shstk avx512_bf16 clzero irperf xsaveerptr rdpru wbnoinvd cppc arat npt lbrv svm_lock nrip_save tsc_scale vmcb_clean flushbyasid decodeassists pausefilter pfthreshold vgif x2avic v_spec_ctrl vnmi avx512vbmi";

		auto*	b0 = FindString( a0, a1, 0 );		TEST_Eq( b0-a0, 16 );
		auto*	b1 = FindString( a0, a2, 0 );		TEST_Eq( b1-a0, 46 );

		TEST( HasSubString( a0, a1 ));
		TEST( not HasSubString( a0, a2 ));
		TEST( HasSubString( a3, "avx512f" ));	// TODO: remove
	}


	static void  StringUtils_FindString2 ()
	{
		const usize	max_size	= 2048;
		const char	ch			= 'A';

		String	str;
		str.reserve( max_size );

		for (usize i = 1; i < max_size; ++i)
		{
			for (usize j = 0; j < i; ++j)
			{
				str.clear();
				str.resize( i, ' ' );

				str[j] = ch;

				#if AE_SIMD_AVX >= 31  // AVX512_BW
				{
					auto*	found = Cast<char>( Base::_hidden_::FindChar8_AVX512( Cast<sbyte>(str.data()), Cast<sbyte>(&str.data()[str.size()]), sbyte(ch) ));
					TEST( found == &str[j] );
				}
				#endif
				#if AE_SIMD_AVX >= 2
				{
					auto*	found = Cast<char>( Base::_hidden_::FindChar8_AVX2( Cast<sbyte>(str.data()), Cast<sbyte>(&str.data()[str.size()]), sbyte(ch) ));
					TEST( found == &str[j] );
				}
				#endif
				#if AE_SIMD_SSE >= 20
				{
					auto*	found = Cast<char>( Base::_hidden_::FindChar8_SSE2( Cast<sbyte>(str.data()), Cast<sbyte>(&str.data()[str.size()]), sbyte(ch) ));
					TEST( found == &str[j] );
				}
				#endif
				#if AE_SIMD_NEON
				{
					auto*	found = Cast<char>( Base::_hidden_::FindChar8_NEON( Cast<sbyte>(str.data()), Cast<sbyte>(&str.data()[str.size()]), sbyte(ch) ));
					TEST( found == &str[j] );
				}
				#endif
			}
		}
	}


	static void  StringUtils_FindStringIC ()
	{
		const char	a0 [] = "oqkkmxaskjndqkas-YbFSPD-dn;q'smfadjksajdnalkmsa";
		const char	a1 [] = "yBfsPD";
		const char	a2 [] = "-YbbFS";
		const char	a3 [] = "YbFSPD";

		auto*	b0 = FindStringIC( a0, a1, 0 );		TEST_Eq( b0-a0, 17 );
		auto*	b1 = FindStringIC( a0, a2, 0 );		TEST_Eq( b1-a0, 47 );
		auto*	b2 = FindStringIC( a0, a3, 0 );		TEST_Eq( b2-a0, 17 );

		TEST( HasSubStringIC( a0, a1 ));
		TEST( not HasSubStringIC( a0, a2 ));
		TEST( HasSubStringIC( a0, a3 ));
	}


	static void  StringUtils_StartsWith ()
	{
		const char	a0 [] = "12AbcD567-dlfsdmfls";
		const char	a1 [] = "12AbcD567";
		const char	a2 [] = "12aBCD567";
		const char	a3 [] = "23A";

		TEST( StartsWith( a0, a1 ));
		TEST( not StartsWith( a0, a2 ));
		TEST( not StartsWith( a0, a3 ));

		TEST( StartsWithIC( a0, a1 ));
		TEST( StartsWithIC( a0, a2 ));
		TEST( not StartsWithIC( a0, a3 ));
	}


	static void  StringUtils_EndsWith ()
	{
		const char	a0 [] = "dlfsdmfls-12AbcD567";
		const char	a1 [] = "12AbcD567";
		const char	a2 [] = "12aBCD567";
		const char	a3 [] = "5677";

		TEST( EndsWith( a0, a1 ));
		TEST( not EndsWith( a0, a2 ));
		TEST( not EndsWith( a0, a3 ));

		TEST( EndsWithIC( a0, a1 ));
		TEST( EndsWithIC( a0, a2 ));
		TEST( not EndsWithIC( a0, a3 ));
	}


	static void  StringView_Hash ()
	{
		StringView	a0 = "adskash;skldmdkjfnlsdkm";
		String		a1 {a0};

		'*'>> a1;
		a1 = a1.substr( 1 );

		auto	h0 = HashOf( a0 );
		auto	h1 = HashOf( a1 );

		TEST( a0 == a1 );
		TEST( h0 == h1 );
	}


#ifdef AE_ENABLE_UTF8PROC
	static void  StringUtils_Utf8_1 ()
	{
		using namespace Base::_hidden_;

		usize	count = 0;
		for (CharUtf32 c = 0; c <= 0x10'FFFF; ++c)
		{
			if ( not Utf32IsValid( c ))
				continue;

			++count;

			CharUtf8	temp1 [4];
			CharUtf8	temp2 [4];
			ssize		w1	= Utf8Encode_v1( c, 4, OUT temp1 );
			ssize		w2	= Utf8Encode_v2( c, 4, OUT temp2 );
			uint		w3	= Utf8CharWidth( temp2, 4 );

			TEST_Eq( w1, w2 );
			TEST_Eq( w1, w3 );

			auto [c1, w4] = Utf8Decode_v1( temp1, w1 );
			auto [c2, w5] = Utf8Decode_v2( temp1, w1 );

			TEST_Eq( w1, w4 );
			TEST_Eq( w1, w5 );
			TEST_Eq( c,  c1 );
			TEST_Eq( c,  c2 );

			for (ssize i = 0; i < w1; ++i)
			{
				TEST( temp1[i] == temp2[i] );
			}
		}

		TEST( count == 1112064 );
	}


	static void  StringUtils_Utf8_2 ()
	{
		const CharUtf8	u8str []	= {
			0x5a,				// Z
			0xd0, 0xaf,			// Я
			0xE2, 0xB8, 0xAE,	// ‽
			0xC3, 0xB1,			// ñ
			0xC3, 0xA9,			// é
			0xC3, 0x9F,			// ß
			0xC3, 0xA0,			// à
			0xE3, 0x81, 0x82,	// あ
			0xE4, 0xBD, 0xA0,	// 你
			0xEC, 0x95, 0x88,	// 안
			0xD8, 0xA3,			// أ
			0xD7, 0x90,			// א
			0xCE, 0xB1,			// α
			0xE0, 0xA4, 0xA8,	// न
			0xE0, 0xB8, 0x81,	// ก
			0
		};
		const usize		len			= CountOf( u8str );
		usize			pos			= 0;

		const auto	Decode = [&]() -> Pair<usize, CharUtf32>
		{{
			usize		w = Utf8CharWidth( u8str + pos, len - pos );
			CharUtf32	c = Utf8Decode( u8str, len, INOUT pos );
			return { w, c };
		}};
		const auto	Encode = [&](CharUtf32 ref, usize width)
		{{
			auto*		str = &u8str[pos - width];
			CharUtf8	tmp[4];
			usize		i = 0;
			TEST( Utf8Encode( ref, 4, tmp, i ));
			TEST( width == i );

			usize		j = 0;
			CharUtf32	c = Utf8Decode( tmp, i, INOUT j );
			TEST( c == ref );

			for (j = 0; j < width; ++j) {
				TEST( tmp[j] == str[j] );
			}
		}};

		{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x5A;		// Z
			Encode( ref, 1 );
			TEST_Eq( w, 1 );
			TEST( c == ref );
		}{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x042F;	// Я
			Encode( ref, 2 );
			TEST_Eq( w, 2 );
			TEST( c == ref );
		}{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x002E2E;	// ‽
			Encode( ref, 3 );
			TEST_Eq( w, 3 );
			TEST( c == ref );
		}{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x0000F1;	// ñ
			Encode( ref, 2 );
			TEST_Eq( w, 2 );
			TEST( c == ref );
		}{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x0000E9;	// é
			Encode( ref, 2 );
			TEST_Eq( w, 2 );
			TEST( c == ref );
		}{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x0000DF;	// ß
			Encode( ref, 2 );
			TEST_Eq( w, 2 );
			TEST( c == ref );
		}{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x0000E0;	// à
			Encode( ref, 2 );
			TEST_Eq( w, 2 );
			TEST( c == ref );
		}{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x003042;	// あ
			Encode( ref, 3 );
			TEST_Eq( w, 3 );
			TEST( c == ref );
		}{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x004f60;	// 你
			Encode( ref, 3 );
			TEST_Eq( w, 3 );
			TEST( c == ref );
		}{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x00c548;	// 안
			Encode( ref, 3 );
			TEST_Eq( w, 3 );
			TEST( c == ref );
		}{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x000623;	// أ
			Encode( ref, 2 );
			TEST_Eq( w, 2 );
			TEST( c == ref );
		}{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x0005d0;	// א
			Encode( ref, 2 );
			TEST_Eq( w, 2 );
			TEST( c == ref );
		}{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x0003B1; // α
			Encode( ref, 2 );
			TEST_Eq( w, 2 );
			TEST( c == ref );
		}{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x000928;	// न
			Encode( ref, 3 );
			TEST_Eq( w, 3 );
			TEST( c == ref );
		}{
			auto		[w, c]	= Decode();
			CharUtf32	ref		= 0x000e01;	// ก
			Encode( ref, 3 );
			TEST_Eq( w, 3 );
			TEST( c == ref );
		}
		TEST_Eq( pos+1, len );
	}


	static void  StringUtils_Utf16_1 ()
	{
		for (uint i = 0; i < 0xD800; ++i)
		{
			CharUtf16	c16[]	= { CharUtf16(i), 0 };
			uint		w1		= Utf16CharWidth( c16, 1 );

			if ( w1 == 0 )
				continue;

			auto [c, w2] = Utf16Decode( c16, 1 );

			TEST( w1 == 1 );
			TEST( w1 == w2 );

			CharUtf16	tmp [2];
			uint		w3 = Utf16Encode( c, 2, OUT tmp );

			TEST( w3 == 1 );
			TEST( tmp[0] == i );
		}

		for (uint i = 0xE000; i < 0xFFFF; ++i)
		{
			CharUtf16	c16[]	= { CharUtf16(i), 0 };
			uint		w1		= Utf16CharWidth( c16, 1 );

			if ( w1 == 0 )
				continue;

			auto [c, w2] = Utf16Decode( c16, 1 );

			TEST( w1 == 1 );
			TEST( w1 == w2 );

			CharUtf16	tmp [2];
			uint		w3 = Utf16Encode( c, 2, OUT tmp );

			TEST( w3 == 1 );
			TEST( tmp[0] == i );
		}

		for (uint i = 0xD800; i < 0xE000; ++i)
		{
			for (uint j = 0; j < 0xDC00; ++j)
			{
				CharUtf16	c16[]	= { CharUtf16(i), CharUtf16(j) };
				uint		w1		= Utf16CharWidth( c16, 2 );

				TEST( w1 == 0 );
			}

			for (uint j = 0xDC00; j <= 0xDFFF; ++j)
			{
				CharUtf16	c16[]	= { CharUtf16(i), CharUtf16(j) };
				uint		w1		= Utf16CharWidth( c16, 2 );

				if ( w1 == 0 )
					continue;

				auto [c, w2] = Utf16Decode( c16, 2 );

				TEST( c >= 0x1'0000 );
				TEST( w1 == 2 );
				TEST( w1 == w2 );

				CharUtf16	tmp [2];
				uint		w3 = Utf16Encode( c, 2, OUT tmp );

				TEST( w3 == 2 );
				TEST( tmp[0] == i );
				TEST( tmp[1] == j );
			}

			for (uint j = 0xDFFF+1; j <= 0xFFFF; ++j)
			{
				CharUtf16	c16[]	= { CharUtf16(i), CharUtf16(j) };
				uint		w1		= Utf16CharWidth( c16, 2 );

				TEST( w1 == 0 );
			}
		}
	}


	static void  StringUtils_Utf16_2 ()
	{
		const CharUtf32		u32str[] = {
			0x5A,		// Z
			0x042F,		// Я
			0x002E2E,	// ‽
			0x0000F1,	// ñ
			0x0000E9,	// é
			0x0000DF,	// ß
			0x0000E0,	// à
			0x003042,	// あ
			0x004f60,	// 你
			0x00c548,	// 안
			0x000623,	// أ
			0x0005d0,	// א
			0x0003B1,	// α
			0x000928,	// न
			0x000e01,	// ก
		};
		const CharUtf16	u16str[] = {
			0x5a,	// Z
			0x42f,	// Я
			0x2e2e,	// ‽
			0xF1,	// ñ
			0xE9,	// é
			0xDF,	// ß
			0xE0,	// à
			0x3042,	// あ
			0x4f60,	// 你
			0xc548,	// 안
			0x623,	// أ
			0x5d0,	// א
			0x3b1,	// α
			0x928,	// न
			0xe01,	// ก
		};
		StaticAssert( CountOf(u32str) == CountOf(u16str) );

		for (usize i = 0; i < CountOf(u32str); ++i)
		{
			CharUtf32	c32	= u32str[i];
			CharUtf16	c16	= u16str[i];

			auto [c, w] = Utf16Decode( &c16, 1 );
			TEST( c == c32 );
		}
	}


	static void  StringUtils_Utf16_3 ()
	{
		const CharUtf32		u32str[] = {
			0x11F10,	// 𑼐
		};

		const CharUtf16	u16str[] = {
		//| size | chars |
			2,		0xd807, 0xdf10,	// 𑼐
		};

		const CharUtf16*	str		= u16str;
		const void*			str_end	= u16str + CountOf(u16str);

		for (CharUtf32 c32 : u32str)
		{
			TEST( str < str_end );

			CharUtf16	c16 [2];
			const uint	w = Utf16Encode( c32, 2, OUT c16 );
			TEST( w == 2 );

			const uint	ref_c16len = str[0];

			for (uint j = 0; j < ref_c16len; ++j) {
				TEST( str[j+1] == c16[j] );
			}
			str += ref_c16len+1;
		}
	}


	static void  StringUtils_ConvertString ()
	{
		const CharUtf32		u32str[] = {
			0x5A,		// Z
			0x042F,		// Я
			0x002E2E,	// ‽
			0x0000F1,	// ñ
			0x0000E9,	// é
			0x0000DF,	// ß
			0x0000E0,	// à
			0x003042,	// あ
			0x004f60,	// 你
			0x00c548,	// 안
			0x000623,	// أ
			0x0005d0,	// א
			0x0003B1,	// α
			0x000928,	// न
			0x000e01,	// ก
			0x11F10,	// 𑼐
			0
		};
		const CharUtf8	u8str []	= {
			0x5a,					// Z
			0xd0, 0xaf,				// Я
			0xE2, 0xB8, 0xAE,		// ‽
			0xC3, 0xB1,				// ñ
			0xC3, 0xA9,				// é
			0xC3, 0x9F,				// ß
			0xC3, 0xA0,				// à
			0xE3, 0x81, 0x82,		// あ
			0xE4, 0xBD, 0xA0,		// 你
			0xEC, 0x95, 0x88,		// 안
			0xD8, 0xA3,				// أ
			0xD7, 0x90,				// א
			0xCE, 0xB1,				// α
			0xE0, 0xA4, 0xA8,		// न
			0xE0, 0xB8, 0x81,		// ก
			0xF0, 0x91, 0xBC, 0x90,	// 𑼐
			0
		};
		const CharUtf16	u16str[] = {
			0x5a,	// Z
			0x42f,	// Я
			0x2e2e,	// ‽
			0xF1,	// ñ
			0xE9,	// é
			0xDF,	// ß
			0xE0,	// à
			0x3042,	// あ
			0x4f60,	// 你
			0xc548,	// 안
			0x623,	// أ
			0x5d0,	// א
			0x3b1,	// α
			0x928,	// न
			0xe01,	// ก
			0xd807, 0xdf10,	// 𑼐
			0
		};

		// utf16
		{
			U16String	dst;
			TEST( ConvertString( OUT dst, U32StringView{u32str} ));
			TEST( dst == U16StringView{u16str} );
		}{
			U16String	dst;
			TEST( ConvertString( OUT dst, U8StringView{u8str} ));
			TEST( dst == U16StringView{u16str} );
		}

		// wchar
		#ifdef AE_PLATFORM_WINDOWS
		{
			WString	dst;
			TEST( ConvertString( OUT dst, U32StringView{u32str} ));
			TEST( dst == WStringView{Cast<wchar_t>(u16str)} );
		}{
			WString	dst;
			TEST( ConvertString( OUT dst, U8StringView{u8str} ));
			TEST( dst == WStringView{Cast<wchar_t>(u16str)} );
		}
		#endif

		// utf8
		{
			U8String	dst;
			TEST( ConvertString( OUT dst, U16StringView{u16str} ));
			TEST( dst == U8StringView{u8str} );
		}{
			U8String	dst;
			TEST( ConvertString( OUT dst, U32StringView{u32str} ));
			TEST( dst == U8StringView{u8str} );
		}

		// utf32
		{
			U32String	dst;
			TEST( ConvertString( OUT dst, U8StringView{u8str} ));
			TEST( dst == U32StringView{u32str} );
		}{
			U32String	dst;
			TEST( ConvertString( OUT dst, U16StringView{u16str} ));
			TEST( dst == U32StringView{u32str} );
		}
	}
#endif // AE_ENABLE_UTF8PROC


	// see AndroidLogOutput::Process
	void  DivideStringOnParts (StringView str, const usize maxPartLength, OUT Array<StringView> &parts) __NE___
	{
		parts.clear();

		usize	offset = 0;
		for (; offset < str.size();)
		{
			const usize	max		= Min( offset + maxPartLength-5, str.size() );
			usize		end		= Clamp( str.rfind( '\n', max ), offset, max );
						end		= str.size() - end < 5 ? max : end;
			//const usize	size	= end - offset;

			parts.push_back( SubStringBE( str, offset, end ));

			offset = end;
		}
	}

	static void  DivideStringOnParts_Test1 ()
	{
		static const char*	str = R"(
)";

		Array<StringView>	parts;
		DivideStringOnParts( str, 800, OUT parts );

		for (usize i = 0; i < parts.size(); ++i)
		{
			if ( i+1 < parts.size() )
			{
				char	c = *(parts[i].data() + parts[i].size());
				CHECK( c == '\n' );
			}
			//if ( i > 0 )
			{
				char	c = *(parts[i].data());
				CHECK( c == '\n' );
			}
		}
	}
}


extern void UnitTest_StringUtils ()
{
	StringUtils_FindChar();
	StringUtils_IsNullTerminated();
	StringUtils_EqualIC();

	StringUtils_FindString1();
	StringUtils_FindString2();
	StringUtils_FindStringIC();

	StringUtils_StartsWith();
	StringUtils_EndsWith();

  #ifdef AE_ENABLE_UTF8PROC
	StringUtils_Utf8_1();
	StringUtils_Utf8_2();

	StringUtils_Utf16_1();
	StringUtils_Utf16_2();
	StringUtils_Utf16_3();

	StringUtils_ConvertString();
  #endif

	StringView_Hash();

	//DivideStringOnParts_Test1();
	Unused( &DivideStringOnParts_Test1 );

	TEST_PASSED();
}
