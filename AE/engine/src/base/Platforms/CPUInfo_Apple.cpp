// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_APPLE
# include <sys/types.h>
# include <sys/sysctl.h>

# include "base/Platforms/CPUInfo.h"
# include "base/Math/BitMath.h"
# include "base/Memory/MemUtils.h"
# include "base/Algorithms/StringUtils.h"

namespace AE::Base
{

/*
=================================================
	constructor
=================================================
*/
	CpuArchInfo::CpuArchInfo ()
	{
		char	buf [256];

		const auto	ReadStr = [&buf] (const char* name)
		{{
			usize	len = sizeof(buf);
			if ( ::sysctlbyname( name, OUT buf, OUT &len, null, 0 ) == 0 )
				return StringView{ buf, len };
			return StringView{};
		}};

		const auto	ReadUInt = [&buf] (const char* name)
		{{
			usize	len = sizeof(buf);
			if ( ::sysctlbyname( name, OUT buf, OUT &len, null, 0 ) == 0 )
			{
				uint	tmp = 0;
				memcpy( &tmp, buf, Min( sizeof(tmp), len ));
				return tmp;
			}
			return 0u;
		}};

		const auto	Test = [] (const char* name)
		{{
			usize	len = 0;
			return ::sysctlbyname( name, null, OUT &len, null, 0 ) == 0;
		}};

		#if defined(AE_CPU_ARCH_ARM32)
			cpu.arch = ECPUArch::ARM_32;
		#elif defined(AE_CPU_ARCH_ARM64)
			cpu.arch = ECPUArch::ARM_64;
			CHECK( Test("hw.optional.arm64") );
		#else
			cpu.arch = ECPUArch::X64;
		#endif

		cpu.physicalCoreCount = Max( ReadUInt("hw.physicalcpu"), ReadUInt("hw.physicalcpu_max") );
		cpu.logicalCoreCount  = Max( ReadUInt("hw.logicalcpu"), ReadUInt("hw.logicalcpu_max") );
		ASSERT( cpu.physicalCoreCount > 0 );

		auto&	p_cpu = cpu.coreTypes.emplace_back();
		p_cpu.name = ReadStr("machdep.cpu.brand_string");
		p_cpu.type = ECoreType::P;

		CHECK( ReadUInt("hw.cachelinesize") == AE_CACHE_LINE );

		feats.NEON		= ReadUInt("hw.optional.neon") > 0;
		feats.NEON_fp16	= ReadUInt("hw.optional.neon_fp16") > 0;
		feats.NEON_hpfp	= ReadUInt("hw.optional.neon_hpfp") > 0;

		feats.SSE2		= ReadUInt("hw.optional.sse2") > 0;
		feats.SSE3		= ReadUInt("hw.optional.sse3") > 0;
		feats.SSSE3		= ReadUInt("hw.optional.ssse3") > 0;
		feats.SSE41		= ReadUInt("hw.optional.sse4_1") > 0;
		feats.SSE42		= ReadUInt("hw.optional.sse4_2") > 0;
		feats.AVX		= ReadUInt("hw.optional.avx1_0") > 0;
		feats.AVX256	= ReadUInt("hw.optional.avx2_0") > 0;
		feats.AVX512	= ReadUInt("hw.optional.avx512f") > 0;

		feats.CRC32		= ReadUInt("hw.optional.armv8_crc32") > 0;
		feats.SHA128	= true;
		feats.AES		= (ReadUInt("hw.optional.arm.FEAT_AES") > 0) or (ReadUInt("hw.optional.aes") > 0);
		feats.SHA256	= ReadUInt("hw.optional.arm.FEAT_SHA256") > 0;
		feats.SHA512	= (ReadUInt("hw.optional.arm.FEAT_SHA512") > 0) or (ReadUInt("hw.optional.armv8_2_sha512") > 0);

		// TODO:
		//	hw.optional.arm.FEAT_FP16

		// get CPU topology
		{
			auto&	e_cpu = cpu.coreTypes.emplace_back();
			e_cpu.name = p_cpu.name;
			e_cpu.type = ECoreType::EE;

			// read E + P config (macos 12)
			if ( ReadUInt("hw.nperflevels") >= 2 )
			{
				const uint	e_phys_count	= ReadUInt("hw.perflevel1.logicalcpu");
				const uint	e_log_count 	= ReadUInt("hw.perflevel1.physicalcpu");
				ASSERT( e_phys_count == e_log_count );

				const uint	p_phys_count	= ReadUInt("hw.perflevel0.logicalcpu");
				const uint	p_log_count 	= ReadUInt("hw.perflevel0.physicalcpu");
				ASSERT( p_phys_count == p_log_count );

				ASSERT_Eq( e_phys_count + p_phys_count, cpu.physicalCoreCount );
				ASSERT_Eq( e_log_count  + p_log_count,  cpu.logicalCoreCount  );

				e_cpu.physicalBits	= {ToBitMask<ulong>( e_phys_count )};
				e_cpu.logicalBits	= {ToBitMask<ulong>( e_log_count  )};
				p_cpu.physicalBits	= {ToBitMask<ulong>( e_phys_count, p_phys_count )};
				p_cpu.logicalBits	= {ToBitMask<ulong>( e_log_count,  p_log_count  )};
			}
			else
			// detect E + P config by name (macos 11)
			{
				uint	p_phys_count	= cpu.physicalCoreCount;
				uint	p_log_count 	= cpu.logicalCoreCount;
				ASSERT( p_phys_count == p_log_count );

				uint	e_phys_count	= 0;
				uint	e_log_count		= 0;

				// E4 + P4
				if ( p_phys_count == 8 )
				{
					p_phys_count = p_log_count = 4;
					e_phys_count = e_log_count = 4;
				}
				else
				// E4 + P2
				if ( p_phys_count == 6 )
				{
					p_phys_count = p_log_count = 2;
					e_phys_count = e_log_count = 4;
				}

				e_cpu.physicalBits	= {ToBitMask<ulong>( e_phys_count )};
				e_cpu.logicalBits	= {ToBitMask<ulong>( e_log_count  )};
				p_cpu.physicalBits	= {ToBitMask<ulong>( e_phys_count, p_phys_count )};
				p_cpu.logicalBits	= {ToBitMask<ulong>( e_log_count,  p_log_count  )};
			}
		}

		if ( cpu.arch == ECPUArch::X64 )
		{
			feats.SSE2	= true;		// always supported
		}

		_Validate();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	GetFrequency
=================================================
*/
	CpuPerformance::MHz_t  CpuPerformance::GetFrequency (uint core) __NE___
	{
		Unused( core );
		return 0;
	}

	uint  CpuPerformance::GetFrequency (OUT MHz_t* result, const uint maxCount) __NE___
	{
		Unused( result, maxCount );
		return 0;
	}

/*
=================================================
	GetUsage
=================================================
*/
	uint  CpuPerformance::GetUsage (OUT float* user, OUT float* kernel, const uint maxCount) __NE___
	{
		Unused( user, kernel, maxCount );
		return 0;
	}

/*
=================================================
	GetPerfCounters
=================================================
*/
	bool  CpuPerformance::GetPerfCounters (OUT PerProcessCounters &, OUT PerThreadCounters &) __NE___
	{
		return true;
	}


} // AE::Base

#endif // AE_PLATFORM_APPLE
