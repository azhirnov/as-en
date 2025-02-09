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

# include "base/Platforms/CPUInfo_X64.cpp.h"

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
			#else
				cpu.arch	= ECPUArch::X86;
			#endif
		}

		// read CPU features (only x86/x64)
		CPUName_t	cpu_name;
		ReadX64CPUFeatures( OUT feats, OUT cpu.microArch, OUT cpu.vendor, OUT cpu_name, OUT cache );

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

				for (usize i = 0; i < cpu.coreTypes.size(); ++i, ++j)
				{
					auto&	dst = cpu.coreTypes[i];
					dst.type	= types[ Min( j, CountOf(types)-1 )];
					dst.name	= cpu_name;
				}
			}
		}

		for (auto& core : cpu.coreTypes)
		{
			cpu.physicalCoreCount	+= core.PhysicalCount();
			cpu.logicalCoreCount	+= core.LogicalCount();
		}

		// read core frequency
		ReadX64CPUClock( INOUT cpu.coreTypes );


		// CPU cache info
		const auto	AddCacheInfo = [this] (ECacheType type, const CacheGeom &c)
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
