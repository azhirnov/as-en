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

namespace AE::Base
{

	inline void  CPUID (uint eax, OUT uint* data) __NE___
	{
		uint*	a = data+0;
		uint*	b = data+1;
		uint*	c = data+2;
		uint*	d = data+3;
		__get_cpuid( eax, a, b, c, d );
	}

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
			//const int count = cpui[0];

			CPUID( 0x80000000, OUT cpui.data() );
			//const int ex_count = cpui[0];

			CPUID( 0x00000001, OUT cpui.data() );

			feats.SSE2		= AllBits( cpui[3], 1u << 26 );
			feats.SSE3		= AllBits( cpui[2], 1u << 0  );
			feats.SSSE3		= AllBits( cpui[2], 1u << 9  );
			feats.POPCNT	= AllBits( cpui[2], 1u << 23 );
			feats.AES		= AllBits( cpui[2], 1u << 25 );
			feats.SSE41		= AllBits( cpui[2], 1u << 19 );
			feats.SSE42		= AllBits( cpui[2], 1u << 20 );
			feats.AVX		= AllBits( cpui[2], 1u << 28 );

			feats.CmpXchg16 = AllBits( cpui[2], 1u << 13 );

			CPUID( 0x00000007, OUT cpui.data() );

			feats.AVX256	= AllBits( cpui[1], 1u << 5  );
			feats.AVX512	= AllBits( cpui[1], 1u << 16 );

			feats.SHA256	= AllBits( cpui[1], 1u << 29 );
			feats.SHA128	= feats.SHA256;

			// get CPU brand name
			CPUID( 0x80000002, OUT cpui.data() );
			std::memcpy( cpu_name, cpui.data(), sizeof(cpui) );

			CPUID( 0x80000003, OUT cpui.data() );
			std::memcpy( cpu_name + sizeof(cpui), cpui.data(), sizeof(cpui) );

			CPUID( 0x80000004, OUT cpui.data() );
			std::memcpy( cpu_name + sizeof(cpui)*2, cpui.data(), sizeof(cpui) );

			for (usize i = CountOf(cpu_name)-1; i > 0; --i)
			{
				const char	c = cpu_name[i];
				if ( (c == '\0') or (c == ' ') )
					cpu_name[i] = '\0';
				else
					break;
			}

			// TODO: _may_i_use_cpu_feature
		}

		cpu.vendor = _NameToVendor( StringView{cpu_name} );

		// TODO: CPU topology

		for (auto& core : cpu.coreTypes)
		{
			cpu.physicalCoreCount	+= core.PhysicalCount();
			cpu.logicalCoreCount	+= core.LogicalCount();
		}

		// CPU cache info
		{
			cache.L1_Inst.lineSize		= ::getauxval( AT_L1I_CACHEGEOMETRY ) & 0xFFFF;
			cache.L1_Inst.associativity	= ::getauxval( AT_L1I_CACHEGEOMETRY ) >> 16;
			cache.L1_Inst.size			= Bytes32u{uint(::getauxval( AT_L1I_CACHESIZE ))};

			cache.L1_Data.lineSize		= ::getauxval( AT_L1D_CACHEGEOMETRY ) & 0xFFFF;
			cache.L1_Data.associativity	= ::getauxval( AT_L1D_CACHEGEOMETRY ) >> 16;
			cache.L1_Data.size			= Bytes32u{uint(::getauxval( AT_L1D_CACHESIZE ))};

			cache.L2.lineSize			= ::getauxval( AT_L2_CACHEGEOMETRY ) & 0xFFFF;
			cache.L2.associativity		= ::getauxval( AT_L2_CACHEGEOMETRY ) >> 16;
			cache.L2.size				= Bytes32u{uint(::getauxval( AT_L2_CACHESIZE ))};

			cache.L3.lineSize			= ::getauxval( AT_L3_CACHEGEOMETRY ) & 0xFFFF;
			cache.L3.associativity		= ::getauxval( AT_L3_CACHEGEOMETRY ) >> 16;
			cache.L3.size				= Bytes32u{uint(::getauxval( AT_L3_CACHESIZE ))};
		}

		_Validate();
	}

} // AE::Base

#endif // (LINUX or ANDROID) and (X86 or X64)
