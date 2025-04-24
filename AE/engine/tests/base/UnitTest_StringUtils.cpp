// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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
					auto*	found = Base::_hidden_::FindChar_AVX512( str.data(), &str.data()[str.size()], ch );
					TEST( found == &str[j] );
				}
				#endif
				#if AE_SIMD_AVX >= 2
				{
					auto*	found = Base::_hidden_::FindChar_AVX2( str.data(), &str.data()[str.size()], ch );
					TEST( found == &str[j] );
				}
				#endif
				#if AE_SIMD_SSE >= 20
				{
					auto*	found = Base::_hidden_::FindChar_SSE2( str.data(), &str.data()[str.size()], ch );
					TEST( found == &str[j] );
				}
				#endif
				#if AE_SIMD_NEON
				{
					auto*	found = Base::_hidden_::FindChar_NEON( str.data(), &str.data()[str.size()], ch );
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

	TEST_PASSED();
}
