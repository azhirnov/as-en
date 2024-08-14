// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_COMPILER_MSVC) and defined(AE_PLATFORM_WINDOWS)
# include "base/Platforms/WindowsHeader.cpp.h"
# include <intrin.h>
# include <powerbase.h>

# include "base/Platforms/WindowsUtils.h"
# include "base/Platforms/WindowsLibrary.h"
# include "base/Containers/UntypedStorage.h"

# include "base/Platforms/CPUInfo.h"
# include "base/Math/BitMath.h"
# include "base/Memory/MemUtils.h"
# include "base/Algorithms/StringUtils.h"
# include "base/Containers/FixedMap.h"

namespace AE::Base
{

	struct PROCESSOR_POWER_INFORMATION
	{
		ULONG Number;
		ULONG MaxMhz;
		ULONG CurrentMhz;
		ULONG MhzLimit;
		ULONG MaxIdleState;
		ULONG CurrentIdleState;
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

		char	cpu_name [64] = {};

		// read CPU features (only x86/x64)
		if ( cpu.arch == ECPUArch::X64 )
		{
			StaticArray<int, 4>	cpui = {};

			__cpuid( OUT cpui.data(), 0 );
			const int count = cpui[0];

			if ( count >= 0x1 )
			{
				__cpuid( OUT cpui.data(), 0x1 );

				feats.SSE2		= HasBit( cpui[3], 26 ) or ::IsProcessorFeaturePresent( PF_XMMI64_INSTRUCTIONS_AVAILABLE ) != 0;
				feats.SSE3		= HasBit( cpui[2],  0 ) or ::IsProcessorFeaturePresent( PF_SSE3_INSTRUCTIONS_AVAILABLE ) != 0;
				feats.SSSE3		= HasBit( cpui[2],  9 ) or ::IsProcessorFeaturePresent( PF_SSSE3_INSTRUCTIONS_AVAILABLE ) != 0;
				feats.POPCNT	= HasBit( cpui[2], 23 );
				feats.AES		= HasBit( cpui[2], 25 );
				feats.SSE41		= HasBit( cpui[2], 19 ) or ::IsProcessorFeaturePresent( PF_SSE4_1_INSTRUCTIONS_AVAILABLE ) != 0;
				feats.SSE42		= HasBit( cpui[2], 20 ) or ::IsProcessorFeaturePresent( PF_SSE4_2_INSTRUCTIONS_AVAILABLE ) != 0;
				feats.AVX		= HasBit( cpui[2], 28 ) or ::IsProcessorFeaturePresent( PF_AVX_INSTRUCTIONS_AVAILABLE ) != 0;

				feats.CmpXchg16 = HasBit( cpui[2], 13 );
			}

			if ( count >= 0x7 )
			{
				__cpuid( OUT cpui.data(), 0x7 );

				feats.AVX256	= HasBit( cpui[1],  5 ) or ::IsProcessorFeaturePresent( PF_AVX2_INSTRUCTIONS_AVAILABLE ) != 0;
				feats.AVX512	= HasBit( cpui[1], 16 ) or ::IsProcessorFeaturePresent( PF_AVX512F_INSTRUCTIONS_AVAILABLE ) != 0;

				feats.SHA256	= HasBit( cpui[1], 29 );
				feats.SHA128	= feats.SHA256;
			}

			// get CPU brand name
			__cpuid( OUT cpui.data(), 0x8000'0000 );
			const uint ex_count = cpui[0];

			if ( ex_count >= 0x8000'0002 )
			{
				__cpuid( OUT cpui.data(), 0x8000'0002 );
				std::memcpy( OUT cpu_name, cpui.data(), sizeof(cpui) );

				__cpuid( OUT cpui.data(), 0x8000'0003 );
				std::memcpy( OUT cpu_name + 16, cpui.data(), sizeof(cpui) );

				__cpuid( OUT cpui.data(), 0x8000'0004 );
				std::memcpy( OUT cpu_name + 32, cpui.data(), sizeof(cpui) );

				for (usize i = CountOf(cpu_name)-1; i > 0; --i)
				{
					const char	c = cpu_name[i];
					if ( (c == '\0') or (c == ' ') )
						cpu_name[i] = '\0';
					else
						break;
				}
			}
			// TODO: _may_i_use_cpu_feature
		}

		cpu.vendor = _NameToVendor( StringView{cpu_name} );

		if ( cpu.arch == ECPUArch::ARM_32 )
		{
			feats.NEON = ::IsProcessorFeaturePresent( PF_ARM_NEON_INSTRUCTIONS_AVAILABLE ) != 0;
		}
		if ( cpu.arch == ECPUArch::ARM_64 )
		{
			feats.NEON		= ::IsProcessorFeaturePresent( PF_ARM_V8_INSTRUCTIONS_AVAILABLE ) != 0;
			feats.CRC32		= ::IsProcessorFeaturePresent( PF_ARM_V8_CRC32_INSTRUCTIONS_AVAILABLE ) != 0;
			feats.AES		= ::IsProcessorFeaturePresent( PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE ) != 0;
			feats.SHA128	= feats.AES;
			feats.SHA256	= feats.AES;
		}

		// read core info
	  #if AE_PLATFORM_TARGET_VERSION_MAJOR >= 10
		if ( auto fnGetSystemCpuSetInformation = BitCast< decltype(&::GetSystemCpuSetInformation) >(WindowsUtils::_GetSystemCpuSetInformationFn());
			 fnGetSystemCpuSetInformation != null )
		{
			HANDLE	process		= ::GetCurrentProcess();	// winxp
			ULONG	buf_size	= 0;

			fnGetSystemCpuSetInformation( null, 0, OUT &buf_size, process, 0 );	// win10

			DynUntypedStorage	info_data	{ Bytes{buf_size}, AlignOf<SYSTEM_CPU_SET_INFORMATION> };
			auto*				infos		= info_data.Ptr<SYSTEM_CPU_SET_INFORMATION>();

			CHECK( fnGetSystemCpuSetInformation( OUT infos, uint(info_data.Size()), OUT &buf_size, process, 0 ) != FALSE );	// win10

			// info for each logical core
			const uint	count = buf_size / sizeof(SYSTEM_CPU_SET_INFORMATION);

			FixedMap< BYTE, Core*, MaxCores >	eff_class_map;

			for (uint i = 0; i < count; ++i)
			{
				ASSERT( infos[i].Type == CpuSetInformation );

				const auto&	info		= infos[i].CpuSet;
				auto [iter, inserted]	= eff_class_map.emplace( info.EfficiencyClass, null );

				if ( inserted )
				{
					iter->second		= &cpu.coreTypes.emplace_back();
					iter->second->name	= cpu_name;
					iter->second->type	= ECoreType::Performance;
				}

				iter->second->logicalBits.set( info.LogicalProcessorIndex );
				iter->second->physicalBits.set( info.CoreIndex );
			}

			::CloseHandle( process );
		}
		else
	  #endif
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
			WindowsLibrary	lib;
			if ( lib.Load( "PowrProf.dll" ))
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
								core.maxClock	= info.MaxMhz;
								core.baseClock	= info.CurrentMhz;
								break;
							}
						}
					}
				}
			}
			else
			{
				StaticArray<int, 4>	cpui = {};

				__cpuid( OUT cpui.data(), 0 );
				const int count = cpui[0];

				if ( count >= 0x16 )
				{
					__cpuid( OUT cpui.data(), 0x16 );

					for (auto& core : cpu.coreTypes)
					{
						core.baseClock	= cpui[0];
						core.maxClock	= cpui[1];
					}
				}
			}
		}

		// CPU cache info
	  #if AE_PLATFORM_TARGET_VERSION_MAJOR >= 7
		if ( WindowsUtils::GetOSVersion() >= Version3{7,0,0} )
		{
			DWORD	buf_size	= 0;
			::GetLogicalProcessorInformationEx( RelationCache, null, OUT &buf_size );	// win7

			DynUntypedStorage	info_data	{ Bytes{buf_size}, AlignOf<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX> };
			auto*				infos		= info_data.Ptr<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>();

			if ( ::GetLogicalProcessorInformationEx( RelationCache, OUT infos, INOUT &buf_size ) == TRUE )
			{
				const uint	count = buf_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX);
				for (uint i = 0; i < count; ++i)
				{
					if ( infos[i].Relationship != RelationCache )
						continue;

					const auto&		info = infos[i].Cache;

					if ( info.Level == 1 and info.Type == CacheInstruction ) {
						cache.L1_Inst.lineSize		= info.LineSize;
						cache.L1_Inst.associativity	= info.Associativity;
						cache.L1_Inst.size			= Bytes32u{info.CacheSize};
					}else
					if ( info.Level == 1 and info.Type == CacheData ) {
						cache.L1_Data.lineSize		= info.LineSize;
						cache.L1_Data.associativity	= info.Associativity;
						cache.L1_Data.size			= Bytes32u{info.CacheSize};
					}else
					if ( info.Level == 2 and info.Type == CacheData ) {
						cache.L2.lineSize			= info.LineSize;
						cache.L2.associativity		= info.Associativity;
						cache.L2.size				= Bytes32u{info.CacheSize};
					}else
					if ( info.Level == 3 and info.Type == CacheData ) {
						cache.L3.lineSize			= info.LineSize;
						cache.L3.associativity		= info.Associativity;
						cache.L3.size				= Bytes32u{info.CacheSize};
					}
				}
			}
		}
	  #endif

		_Validate();
	}

} // AE::Base

#endif // AE_COMPILER_MSVC and AE_PLATFORM_WINDOWS
