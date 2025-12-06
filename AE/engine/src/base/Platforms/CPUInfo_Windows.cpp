// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_COMPILER_MSVC) and defined(AE_PLATFORM_WINDOWS)
# include "base/Platforms/WindowsHeader.cpp.h"

# include "base/Platforms/WindowsUtils.h"
# include "base/Platforms/WindowsLibrary.h"

# include "base/Platforms/CPUInfo.h"
# include "base/Math/BitMath.h"
# include "base/Memory/MemUtils.h"
# include "base/Algorithms/StringUtils.h"
# include "base/Containers/FixedMap.h"
# include "base/Memory/DynUntypedStorage.h"

# ifdef AE_CPU_ARCH_X86_64
#	include "base/Platforms/CPUInfo_X64.cpp.h"
# endif

namespace AE::Base
{

	struct PROCESSOR_POWER_INFORMATION
	{
		ULONG	Number;
		ULONG	MaxMhz;
		ULONG	CurrentMhz;
		ULONG	MhzLimit;
		ULONG	MaxIdleState;
		ULONG	CurrentIdleState;
	};

/*
=================================================
	constructor
----
	https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=vs-2019
	https://github.com/walbourn/directx-sdk-samples/blob/main/CoreDetection/CpuTopology.cpp
=================================================
*/
	CpuArchInfo::CpuArchInfo () __NE___
	{
		Unused( WindowsUtils::SetCurrentThreadAffinity( 0 ));

		// read CPU architecture
		{
			SYSTEM_INFO		info = {};
			::GetNativeSystemInfo( OUT &info );		// winxp

			switch ( info.wProcessorArchitecture )
			{
				case PROCESSOR_ARCHITECTURE_AMD64 :			cpu.arch = ECPUArch::X64;		break;
				case PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64:
				case PROCESSOR_ARCHITECTURE_ARM :			cpu.arch = ECPUArch::ARM_32;	break;
				case PROCESSOR_ARCHITECTURE_ARM64 :			cpu.arch = ECPUArch::ARM_64;	break;
				default :									DBG_WARNING( "unknown cpu architecture" );
			}
		}

		CPUName_t	cpu_name;

		// read CPU features (only x86/x64)
	  #ifdef AE_CPU_ARCH_X86_64
		if ( cpu.arch == ECPUArch::X64 )
		{
			ReadX64CPUFeatures( OUT feats, OUT cpu.microArch, OUT cpu.vendor, OUT cpu_name );

			feats.SSE2		= feats.SSE2	or ::IsProcessorFeaturePresent( PF_XMMI64_INSTRUCTIONS_AVAILABLE ) != 0;
			feats.SSE3		= feats.SSE3	or ::IsProcessorFeaturePresent( PF_SSE3_INSTRUCTIONS_AVAILABLE ) != 0;
			feats.SSSE3		= feats.SSSE3	or ::IsProcessorFeaturePresent( PF_SSSE3_INSTRUCTIONS_AVAILABLE ) != 0;
			feats.SSE41		= feats.SSE41	or ::IsProcessorFeaturePresent( PF_SSE4_1_INSTRUCTIONS_AVAILABLE ) != 0;
			feats.SSE42		= feats.SSE42	or ::IsProcessorFeaturePresent( PF_SSE4_2_INSTRUCTIONS_AVAILABLE ) != 0;
			feats.AVX		= feats.AVX		or ::IsProcessorFeaturePresent( PF_AVX_INSTRUCTIONS_AVAILABLE ) != 0;

			feats.AVX2		= feats.AVX2	or ::IsProcessorFeaturePresent( PF_AVX2_INSTRUCTIONS_AVAILABLE ) != 0;
			feats.AVX512F	= feats.AVX512F	or ::IsProcessorFeaturePresent( PF_AVX512F_INSTRUCTIONS_AVAILABLE ) != 0;
		}
	  #endif

	  #ifdef AE_CPU_ARCH_ARM_BASED
		if ( cpu.arch == ECPUArch::ARM_32 )
		{
			feats.NEON = ::IsProcessorFeaturePresent( PF_ARM_NEON_INSTRUCTIONS_AVAILABLE ) != 0;
		}
		if ( cpu.arch == ECPUArch::ARM_64 )
		{
			feats.NEON		= ::IsProcessorFeaturePresent( PF_ARM_V8_INSTRUCTIONS_AVAILABLE ) != 0;
			feats.CRC32		= ::IsProcessorFeaturePresent( PF_ARM_V8_CRC32_INSTRUCTIONS_AVAILABLE ) != 0;
			feats.AES		= ::IsProcessorFeaturePresent( PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE ) != 0;
		}
	  #endif

		// read core info
		if ( auto fnGetSystemCpuSetInformation = BitCast< decltype(&::GetSystemCpuSetInformation) >(WindowsUtils::_GetSystemCpuSetInformationFn());
			 fnGetSystemCpuSetInformation != null )
		{
			HANDLE	process		= ::GetCurrentProcess();	// winxp
			ULONG	buf_size	= 0;

			fnGetSystemCpuSetInformation( null, 0, OUT &buf_size, process, 0 );	// win10

			DynUntypedStorage	info_data	{ Bytes{buf_size}, AlignOf<SYSTEM_CPU_SET_INFORMATION> };
			auto*				infos		= info_data.Ptr<SYSTEM_CPU_SET_INFORMATION>();

			if ( fnGetSystemCpuSetInformation( OUT infos, uint(info_data.Size()), OUT &buf_size, process, 0 ) != FALSE )	// win10
			{
				// info for each logical core
				const uint	count = buf_size / sizeof(SYSTEM_CPU_SET_INFORMATION);

				FixedMap< ushort, Core*, MaxCoreTypes >	eff_class_map;

				for (uint i = 0; i < count; ++i)
				{
					ASSERT( infos[i].Type == CpuSetInformation );

					const auto&	info		= infos[i].CpuSet;
					ushort		eff_class	= cpu.vendor == ECPUVendor::Intel ? (info.EfficiencyClass << 8) | (info.SchedulingClass & 0xFF) : info.EfficiencyClass;
					auto [iter, inserted]	= eff_class_map.emplace( eff_class, null );

					if ( iter == Default )
						break;  // overflow

					if ( inserted )
						iter->second = &cpu.coreTypes.emplace_back();

					iter->second->logicalBits.set( info.LogicalProcessorIndex );
					iter->second->physicalBits.set( info.CoreIndex );

					// from https://gpuopen.com/gdc-presentations/2022/GDC_AMD_Ryzen_Processor_Software_Optimization.pdf
					// "Some AMD products have cores which are faster than other cores. The system BIOS describes the CPPC Highest Performance ranking for each logical processor.
					//  The Windows Kernel creates a PerformanceSchedulingClass ranking based on this information and uses it during scheduling. Logical processor 0 and CCD0 may not be the fastest."
				}

				uint	i = 0;
				for (auto [cl, core] : eff_class_map)
				{
					core->type = ECoreType(uint(ECoreType::Performance) + i);
					core->name = cpu_name;
					++i;
				}
			}

			::CloseHandle( process );
		}
		else
		{
			auto&	info		= cpu.coreTypes.emplace_back();
			info.type			= ECoreType::Performance;
			info.logicalBits	= CoreBits_t{ ToBitMask<ulong>( std::thread::hardware_concurrency() )};
			info.physicalBits	= info.logicalBits;
		}

		for (auto& core : cpu.coreTypes)
		{
			cpu.physicalCoreCount	+= core.PhysicalCount();
			cpu.logicalCoreCount	+= core.LogicalCount();
		}

		// read core frequency
		{
			bool	has_freq = false;

			#ifdef AE_CPU_ARCH_X86_64
				// will change thread affinity
				has_freq = ReadX64CPUClock( INOUT cpu.coreTypes );
			#endif

			WindowsLibrary	lib;
			if ( not has_freq and lib.Load( "PowrProf.dll" ))
			{
				StaticArray< PROCESSOR_POWER_INFORMATION, 512 >		cores = {};
				CHECK( cores.size() >= cpu.logicalCoreCount );

				decltype(CallNtPowerInformation)*	fn_CallNtPowerInformation = null;

				if ( lib.GetProcAddr( "CallNtPowerInformation", OUT fn_CallNtPowerInformation ) and
					 fn_CallNtPowerInformation( ProcessorInformation, null, 0, OUT cores.data(), uint(ArraySizeOf(cores)) ) == 0 )
				{
					for (uint i = 0; i < cpu.logicalCoreCount; ++i)
					{
						const auto&		info = cores[i];
						ASSERT( info.Number == i );

						for (auto& core : cpu.coreTypes)
						{
							if ( core.logicalBits.test( i ))
							{
								has_freq		= true;
								core.maxClock	= info.MaxMhz;
								core.baseClock	= info.CurrentMhz;
								break;
							}
						}
					}
				}
			}
		}

		// CPU cache info
	  #ifdef AE_CPU_ARCH_X86_64
		if ( cpu.arch == ECPUArch::X64 )
		{
			// will change thread affinity
			ReadX64CacheHierarchy( cpu.vendor, cpu.coreTypes, OUT cache );
		}
	  #endif

	  #if AE_PLATFORM_TARGET_VERSION_MAJOR >= 7
		if ( WindowsUtils::GetOSVersion() >= Version3{7,0,0} )
		{
			DWORD	buf_size	= 0;
			::GetLogicalProcessorInformationEx( RelationCache, null, OUT &buf_size );	// win7

			DynUntypedStorage	info_data	{ Bytes{buf_size}, AlignOf<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX> };
			auto*				info_ptr	= info_data.Ptr<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>();
			const void*			end			= info_ptr + Bytes{buf_size};

			const auto	AddCacheInfo = [this] (ECacheType type, const CacheGeom &c)
			{{
				if ( c.associativity > 0 or c.lineSize > 0 or c.size > 0 )
					cache.emplace( CacheKey_t{ type, ECoreType::Performance }, c );
			}};

			if ( ::GetLogicalProcessorInformationEx( RelationCache, OUT info_ptr, INOUT &buf_size ) == TRUE )
			{
				for (; (info_ptr + Bytes{info_ptr->Size} <= end) and (info_ptr->Size > 0);)
				{
					if ( info_ptr->Relationship == RelationCache )
					{
						const auto&		info = info_ptr->Cache;
						CacheGeom		c;
						c.lineSize		= info.LineSize;
						c.associativity	= info.Associativity;	// if 0xFF, the cache is fully associative
						c.size			= Bytes32u{info.CacheSize};

						if ( info.Level == 1 and info.Type == CacheInstruction )	AddCacheInfo( ECacheType::L1_Instuction, c );	else
						if ( info.Level == 1 and info.Type == CacheData )			AddCacheInfo( ECacheType::L1_Data, c );			else
						if ( info.Level == 2 and info.Type == CacheUnified )		AddCacheInfo( ECacheType::L2, c );				else
						if ( info.Level == 3 and info.Type == CacheUnified )		AddCacheInfo( ECacheType::L3, c );				else
																					DBG_WARNING( "unsupported cache type" );
					}
					info_ptr = info_ptr + Bytes{info_ptr->Size};
				}
			}
		}
	  #endif

		Unused( WindowsUtils::ResetCurrentThreadAffinity() );

		_Validate();
	}

} // AE::Base

#endif // AE_COMPILER_MSVC and AE_PLATFORM_WINDOWS
