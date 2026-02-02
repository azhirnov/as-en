// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#if (defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)) and defined(AE_CPU_ARCH_X86_64)
# include <fstream>
# include <sys/auxv.h>
# include <cpuid.h>

# include "base/Platforms/CPUInfo.h"
# include "base/Math/BitMath.h"
# include "base/Memory/MemUtils.h"
# include "base/Algorithms/ToString.h"
# include "base/Containers/FixedSet.h"

# include "base/Platforms/CPUInfo_X64.cpp.h"

namespace AE::Base
{
namespace
{
	ND_ static uint  ReadUint10 (StringView line)
	{
		return StringToUInt( SubString( line, line.find(": ")+2, line.length() ), 10 );
	}

	ND_ static uint  ReadUint16 (StringView line)
	{
		return StringToUInt( SubString( line, line.find(": ")+4, line.length() ), 16 );
	}

	ND_ static uint  GetMinClockSpeed (uint id)
	{
		String			line;
		std::ifstream	stream;

		stream.open( "/sys/devices/system/cpu/cpu"s << Base::ToString(id) << "/cpufreq/cpuinfo_min_freq" );
		if ( not stream ) {
			stream.open( "/sys/devices/system/cpu/cpu"s << Base::ToString(id) << "/cpufreq/scaling_min_freq" );
		}
		if ( stream ) {
			std::getline( stream, OUT line );
			stream.close();
			return StringToUInt( line ) / 1000;	// in MHz
		}
		return 0;
	}

	ND_ static uint  GetMaxClockSpeed (uint id)
	{
		String			line;
		std::ifstream	stream;

		stream.open( "/sys/devices/system/cpu/cpu"s << Base::ToString(id) << "/cpufreq/cpuinfo_max_freq" );
		if ( not stream ) {
			stream.open( "/sys/devices/system/cpu/cpu"s << Base::ToString(id) << "/cpufreq/scaling_max_freq" );
		}
		if ( stream ) {
			std::getline( stream, OUT line );
			stream.close();
			return StringToUInt( line ) / 1000;	// in MHz
		}
		return 0;
	}

} // namespace

/*
=================================================
	constructor
=================================================
*/
	CpuArchInfo::CpuArchInfo () __NE___
	{
		Unused( LinuxUtils::SetCurrentThreadAffinity( 0 ));

		// read CPU info
		{
			#if defined(AE_CPU_ARCH_X64)
				cpu.arch	= ECPUArch::X64;
			#else
				cpu.arch	= ECPUArch::X86;
			#endif
		}

		// read CPU features (only x86/x64)
		CPUName_t	cpu_name;
		ReadX64CPUFeatures( OUT feats, OUT cpu.microArch, OUT cpu.vendor, OUT cpu_name );

		// parse processors
		{
			struct TmpCore
			{
				uint	logicId		= UMax;		// logical core in CPU
				uint	physId		= UMax;		// physical core in CPU
				uint	cpuId		= UMax;		// physical CPU
				uint	family		= 0;
				uint	model		= 0;
				uint	minClock	= 0;
				uint	maxClock	= 0;
			};
			constexpr uint	max_cores = 64;

			// ref https://doc.callmematthi.eu/static/webArticles/Understanding%20Linux%20_proc_cpuinfo.pdf
			std::ifstream	stream {"/proc/cpuinfo"};
			if ( stream )
			{
				FixedArray< TmpCore, max_cores >	cores;
				String								line;
				String								flags_str;

				while ( std::getline( stream, OUT line ))
				{
					if ( StartsWith( line, "processor\t" ))
					{
						if ( cores.size()+1 == cores.capacity() )
							break;

						cores.emplace_back().logicId = ReadUint10( line );
					}else
					if ( not cores.empty() )
					{
						if ( StartsWith( line, "core id\t" )) {
							cores.back().physId = ReadUint10( line );
						}else
						if ( StartsWith( line, "physical id\t" )) {
							cores.back().cpuId = ReadUint10( line );
						}else
						if ( StartsWith( line, "cpu family\t" )) {
							cores.back().family = ReadUint10( line );
						}else
						if ( StartsWith( line, "model\t" )) {
							cores.back().model = ReadUint10( line );
						}
					}
					if ( flags_str.empty() and StartsWith( line, "flags\t" ))
						flags_str = SubString( line, line.find(':') );
				}

				FixedSet< uint, max_cores >	unique_cores;
				for (auto& core : cores) {
					unique_cores.insert( (core.family & 0xFF) | ((core.model & 0xFF) << 8) );
				}

				for (auto& unique : unique_cores)
				{
					const uint	family	= (unique & 0xFF);
					const uint	model	= (unique >> 8) & 0xFF;
					auto&		dst		= cpu.coreTypes.emplace_back();

					for (auto& core : cores)
					{
						if ( core.family == family and core.model == model ) {
							dst.logicalBits.set( core.logicId );
							dst.physicalBits.set( core.physId );
						}
					}
				}

				// sort by max clock
				std::sort( cpu.coreTypes.begin(), cpu.coreTypes.end(), [](auto& lhs, auto &rhs) { return lhs.maxClock > rhs.maxClock; });

				const ECoreType		types [] = { ECoreType::HighPerformance, ECoreType::Performance, ECoreType::EnergyEfficient };
				usize				j		 = cpu.coreTypes.size() == 1 ? 1 :
												Max( 0, 3 - int(cpu.coreTypes.size()) );

				for (usize i = 0; i < cpu.coreTypes.size(); ++i, ++j)
				{
					auto&	dst = cpu.coreTypes[i];
					dst.type	= types[ Min( j, CountOf(types)-1 )];
					dst.name	= cpu_name;

					uint	id = dst.FirstLogicalCore();
					dst.baseClock = GetMinClockSpeed( id );
					dst.maxClock  = GetMaxClockSpeed( id );
				}

				// parse CPU features which may be not detected by cpuid
				{
					feats.VAES				= feats.VAES 			or (flags_str.find( "vaes" ) != String::npos);
					feats.AESKL				= feats.AESKL 			or (flags_str.find( "aeskl" ) != String::npos);
					feats.SHA2_256			= feats.SHA2_256 		or (flags_str.find( "sha_ni" ) != String::npos);	// sha ???

					feats.AVX2				= feats.AVX2 			or (flags_str.find( "avx2" ) != String::npos);
					feats.AVX512F			= feats.AVX512F 		or (flags_str.find( "avx512f" ) != String::npos);
					feats.AVX512_DQ			= feats.AVX512_DQ 		or (flags_str.find( "avx512dq" ) != String::npos);
					feats.AVX512_IFMA		= feats.AVX512_IFMA		or (flags_str.find( "avx512ifma" ) != String::npos);
				//	feats.AVX512_ER			= feats.AVX512_ER 		or (flags_str.find( "avx512er" ) != String::npos);
					feats.AVX512_BW			= feats.AVX512_BW 		or (flags_str.find( "avx512bw" ) != String::npos);
					feats.AVX512_VL			= feats.AVX512_VL 		or (flags_str.find( "avx512vl" ) != String::npos);
					feats.AVX512_VBMI		= feats.AVX512_VBMI		or (flags_str.find( "avx512vbmi" ) != String::npos);
					feats.AVX512_VBMI2		= feats.AVX512_VBMI2 	or (flags_str.find( "avx512_vbmi2" ) != String::npos);
					feats.AVX512_VNNI		= feats.AVX512_VNNI 	or (flags_str.find( "avx512_vnni" ) != String::npos);
					feats.AVX512_BITALG		= feats.AVX512_BITALG 	or (flags_str.find( "avx512_bitalg" ) != String::npos);
					feats.AVX512_VPOPCNTDQ	= feats.AVX512_VPOPCNTDQ or (flags_str.find( "avx512_vpopcntdq" ) != String::npos);
				//	feats.AVX512_4FMAPS		= feats.AVX512_4FMAPS 	or (flags_str.find( "avx5124fmaps" ) != String::npos);
				//	feats.AVX512_FP16		= feats.AVX512_FP16 	or (flags_str.find( "avx512fp16" ) != String::npos);
					feats.AVX512_BF16		= feats.AVX512_BF16 	or (flags_str.find( "avx512_bf16" ) != String::npos);
				}
			}
		}

		for (auto& core : cpu.coreTypes)
		{
			cpu.physicalCoreCount	+= core.PhysicalCount();
			cpu.logicalCoreCount	+= core.LogicalCount();
		}

		// read core frequency
		{
			// will change thread affinity
			ReadX64CPUClock( INOUT cpu.coreTypes );
		}

		// CPU cache info
		{
			// will change thread affinity
			ReadX64CacheHierarchy( cpu.vendor, cpu.coreTypes, OUT cache );

			const auto	AddCacheInfo = [this] (ECacheType type, const CacheGeom &c)
			{{
				if ( c.associativity > 0 or c.lineSize > 0 or c.size > 0 )
					cache.emplace( CacheKey_t{ type, ECoreType::Performance }, c );
			}};

			{
				CacheGeom	c;
				c.lineSize		= ::getauxval( AT_L1I_CACHEGEOMETRY ) & 0xFFFF;
				c.associativity	= ::getauxval( AT_L1I_CACHEGEOMETRY ) >> 16;
				c.size			= Bytes32u{uint(::getauxval( AT_L1I_CACHESIZE ))};
				AddCacheInfo( ECacheType::L1_Instuction, c );
			}{
				CacheGeom	c;
				c.lineSize		= ::getauxval( AT_L1D_CACHEGEOMETRY ) & 0xFFFF;
				c.associativity	= ::getauxval( AT_L1D_CACHEGEOMETRY ) >> 16;
				c.size			= Bytes32u{uint(::getauxval( AT_L1D_CACHESIZE ))};
				AddCacheInfo( ECacheType::L1_Data, c );
			}{
				CacheGeom	c;
				c.lineSize		= ::getauxval( AT_L2_CACHEGEOMETRY ) & 0xFFFF;
				c.associativity	= ::getauxval( AT_L2_CACHEGEOMETRY ) >> 16;
				c.size			= Bytes32u{uint(::getauxval( AT_L2_CACHESIZE ))};
				AddCacheInfo( ECacheType::L2, c );
			}{
				CacheGeom	c;
				c.lineSize		= ::getauxval( AT_L3_CACHEGEOMETRY ) & 0xFFFF;
				c.associativity	= ::getauxval( AT_L3_CACHEGEOMETRY ) >> 16;
				c.size			= Bytes32u{uint(::getauxval( AT_L3_CACHESIZE ))};
				AddCacheInfo( ECacheType::L3, c );
			}
		}

		Unused( LinuxUtils::ResetCurrentThreadAffinity() );

		_Validate();
	}

} // AE::Base

#endif // (LINUX or ANDROID) and (X86 or X64)
