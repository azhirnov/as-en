// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#if (defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)) and defined(AE_CPU_ARCH_X86_64)
# include <fstream>
# include <sys/auxv.h>
# include <cpuid.h>

# include "base/Platforms/CPUInfo.h"
# include "base/Math/BitMath.h"
# include "base/Memory/MemUtils.h"
# include "base/Algorithms/StringUtils.h"
# include "base/Containers/FixedSet.h"

namespace AE::Base
{
namespace
{
	ND_ static uint  ReadUint10 (StringView line)
	{
		return StringToUInt( line.substr( line.find(": ")+2, line.length() ), 10 );
	}

	ND_ static uint  ReadUint16 (StringView line)
	{
		return StringToUInt( line.substr( line.find(": ")+4, line.length() ), 16 );
	}

	inline void  CPUID (uint eax, OUT uint* data) __NE___
	{
		uint*	a = data+0;
		uint*	b = data+1;
		uint*	c = data+2;
		uint*	d = data+3;
		__get_cpuid( eax, a, b, c, d );
	}

} // namespace

/*
=================================================
	constructor
=================================================
*/
	CpuArchInfo::CpuArchInfo () __NE___
	{
		// read CPU info
		{
			#if defined(AE_CPU_ARCH_X64)
				cpu.arch	= ECPUArch::X64;
				feats.SSE2	= true;		// always supported
			#else
				cpu.arch	= ECPUArch::X86;
			#endif
		}

		char	cpu_name [64] = {};

		// read CPU features (only x86/x64)
		if ( cpu.arch == ECPUArch::X64 )
		{
			StaticArray<uint, 4>	cpui = {};

			CPUID( 0, OUT cpui.data() );
			const int count = cpui[0];

			if ( count >= 0x1 )
			{
				CPUID( 0x1, OUT cpui.data() );

				feats.SSE2		= HasBit( cpui[3], 26 );
				feats.SSE3		= HasBit( cpui[2],  0 );
				feats.SSSE3		= HasBit( cpui[2],  9 );
				feats.POPCNT	= HasBit( cpui[2], 23 );
				feats.AES		= HasBit( cpui[2], 25 );
				feats.SSE41		= HasBit( cpui[2], 19 );
				feats.SSE42		= HasBit( cpui[2], 20 );
				feats.AVX		= HasBit( cpui[2], 28 );

				feats.CmpXchg16 = HasBit( cpui[2], 13 );
			}

			if ( count >= 0x7 )
			{
				CPUID( 0x7, OUT cpui.data() );

				feats.AVX256	= HasBit( cpui[1],  5 );
				feats.AVX512	= HasBit( cpui[1], 16 );

				feats.SHA256	= HasBit( cpui[1], 29 );
				feats.SHA128	= feats.SHA256;
			}

			// get CPU brand name
			CPUID( 0x80000000, OUT cpui.data() );
			const uint ex_count = cpui[0];

			if ( ex_count >= 0x8000'0002 )
			{
				CPUID( 0x8000'0002, OUT cpui.data() );
				std::memcpy( OUT cpu_name, cpui.data(), sizeof(cpui) );

				CPUID( 0x8000'0003, OUT cpui.data() );
				std::memcpy( OUT cpu_name + sizeof(cpui), cpui.data(), sizeof(cpui) );

				CPUID( 0x8000'0004, OUT cpui.data() );
				std::memcpy( OUT cpu_name + sizeof(cpui)*2, cpui.data(), sizeof(cpui) );

				for (usize i = CountOf(cpu_name)-1; i > 0; --i)
				{
					const char	c = cpu_name[i];
					if ( (c == '\0') or (c == ' ') )
						cpu_name[i] = '\0';
					else
						break;
				}
			}
		}

		cpu.vendor = _NameToVendor( StringView{cpu_name} );

		// parse processors
		{
			struct TmpCore
			{
				uint	id		= UMax;
				uint	part	= 0;
				uint	vendor	= 0;
			};

			std::ifstream	stream {"/proc/cpuinfo"};
			if ( stream )
			{
				FixedArray< TmpCore, 64 >	cores;
				String						line;

				while ( std::getline( stream, OUT line ))
				{
					if ( StartsWith( line, "processor" ))
					{
						if ( cores.size()+1 == cores.capacity() )
							break;
						cores.emplace_back().id = ReadUint10( line );
					}else
					if ( not cores.empty() )
					{
						if ( StartsWith( line, "CPU part" )) {
							cores.back().part = ReadUint16( line );
						}else
						if ( StartsWith( line, "CPU implementer" )) {
							cores.back().vendor = ReadUint16( line );
						}
					}
				}

				FixedSet< uint, 64 >	unique_cores;
				for (auto& core : cores) {
					unique_cores.insert( (core.vendor << 24) | (core.part & 0xFFFFFF) );
				}

				for (auto& unique : unique_cores)
				{
					const uint	vendor	= (unique >> 24);
					const uint	part	= (unique & 0xFFFFFF);
					auto&		dst		= cpu.coreTypes.emplace_back();

					for (auto& core : cores)
					{
						if ( core.vendor == vendor and core.part == part ) {
							dst.logicalBits.set( core.id );
							dst.physicalBits.set( core.id );
						}
					}
				}

				// sort by max clock
				std::sort( cpu.coreTypes.begin(), cpu.coreTypes.end(), [](auto& lhs, auto &rhs) { return lhs.maxClock > rhs.maxClock; });

				const ECoreType		types [] = { ECoreType::HighPerformance, ECoreType::Performance, ECoreType::EnergyEfficient };
				usize				j		 = cpu.coreTypes.size() == 1 ? 1 :
												Max( 0, 3 - int(cpu.coreTypes.size()) );

				for (usize i = 0; i < cpu.coreTypes.size(); ++i, ++j) {
					cpu.coreTypes[i].type = types[ Min( j, CountOf(types)-1 )];
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
			StaticArray<uint, 4>	cpui = {};

			CPUID( 0, OUT cpui.data() );
			const int count = cpui[0];

			if ( count >= 0x16 )
			{
				CPUID( 0x16, OUT cpui.data() );

				for (auto& core : cpu.coreTypes)
				{
					core.baseClock	= cpui[0];
					core.maxClock	= cpui[1];
				}
			}
		}


		// CPU cache info
		const auto	AddCacheInfo = [this] (ECacheType type, CacheGeom c)
		{{
			if ( c.associativity > 0 or c.lineSize > 0 or c.size > 0 )
				cache.emplace( CacheKey_t{ type, ECoreType::Unknown }, c );
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

		_Validate();
	}

} // AE::Base

#endif // (LINUX or ANDROID) and (X86 or X64)
