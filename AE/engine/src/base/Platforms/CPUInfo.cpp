// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Platforms/CPUInfo.h"
#include "base/Math/BitMath.h"
#include "base/Memory/MemUtils.h"
#include "base/Algorithms/StringUtils.h"
#include "base/Containers/FixedMap.h"
#include "base/Containers/FixedSet.h"

namespace AE::Base
{

#ifdef AE_PLATFORM_EMSCRIPTEN
/*
=================================================
	constructor
=================================================
*/
	CpuArchInfo::CpuArchInfo () __NE___
	{
		cpu.arch = ECPUArch::X86;

		_Validate();
	}
#endif // AE_PLATFORM_EMSCRIPTEN
//-----------------------------------------------------------------------------



/*
=================================================
	Get
=================================================
*/
	CpuArchInfo const&  CpuArchInfo::Get () __NE___
	{
		static const CpuArchInfo  info;
		return info;
	}

/*
=================================================
	Print
=================================================
*/
	String  CpuArchInfo::Print () C_NE___
	{
	#ifdef AE_ENABLE_LOGS
		TRY{
			String	str;

			str << "CPU features:";

			#if defined(AE_CPU_ARCH_X86) or defined(AE_CPU_ARCH_X64)
			str << "\n  SSE 2:     " << ToString( feats.SSE2 )
				<< "\n  SSE 3: . . " << ToString( feats.SSE3 )
				<< "\n  SSSE 3:    " << ToString( feats.SSSE3 )
				<< "\n  SSE 4.1: . " << ToString( feats.SSE41 )
				<< "\n  SSE 4.2:   " << ToString( feats.SSE42 )
				<< "\n  AVX: . . . " << ToString( feats.AVX )
				<< "\n  AVX 256:   " << ToString( feats.AVX256 )
				<< "\n  AVX 512: . " << ToString( feats.AVX512 )
				<< "\n  POPCNT:    " << ToString( feats.POPCNT )
				<< "\n  CmpXchg16: " << ToString( feats.CmpXchg16 );
			#endif
			#ifdef AE_CPU_ARCH_ARM_BASED
			str << "\n  NEON:  . . " << ToString( feats.NEON )
				<< "\n  NEON_fp16: " << ToString( feats.NEON_fp16 )
				<< "\n  NEON_hpfp: " << ToString( feats.NEON_hpfp )
				<< "\n  SVE:       " << ToString( feats.SVE )
				<< "\n  SVE2:  . . " << ToString( feats.SVE2 )
				<< "\n  SVEAES:    " << ToString( feats.SVEAES );
			#endif

			str << "\n  AES: . . . " << ToString( feats.AES )
				<< "\n  CRC32:     " << ToString( feats.CRC32 )
				<< "\n  SHA 128: . " << ToString( feats.SHA128 )
				<< "\n  SHA 256:   " << ToString( feats.SHA256 )
				<< "\n  SHA 512: . " << ToString( feats.SHA512 );

			str << "\n--------------------"
				<< "\nCPU info:"
				<< "\n  vendor:       " << ToString( cpu.vendor )
				<< "\n  architecture: " << ToString( cpu.arch )
				<< "\n  total cores:  " << ToString( cpu.physicalCoreCount ) << " / " << ToString( cpu.logicalCoreCount );

			for (auto& core : cpu.coreTypes)
			{
				str << "\n  core: " << StringView{core.name}
					<< "\n    type:       " << ToString( core.type )
					<< "\n    base clock: " << ToString( core.baseClock ) << " MHz"
					<< "\n    max clock:  " << ToString( core.maxClock ) << " MHz"
					<< "\n    threads:    " << ToString( core.PhysicalCount() ) << " / " << ToString( core.LogicalCount() )
					<< "\n    IDs:        [" << ToString( core.FirstLogicalCore() ) << ", " << ToString( core.LastLogicalCore()+1 ) << ')'
					<< "\n    ----------";
			}

			str << "\nCache info:";
			const auto	PrintCache = [&str] (StringView name, const CacheGeom &g)
			{{
				if ( g.size == 0 ) return;
				str << "\n    " << name << ".lineSize:      " << ToString( g.lineSize )
					<< "\n    " << name << ".associativity: " << ToString( g.associativity )
					<< "\n    " << name << ".size:          " << ToString( g.size )
					<< "\n    ----------";
			}};
			PrintCache( "L1_Inst", cache.L1_Inst );
			PrintCache( "L1_Data", cache.L1_Data );
			PrintCache( "L2", cache.L2 );
			PrintCache( "L3", cache.L3 );

			return str;
		}
		CATCH_ALL(
			return Default;
		)
	#else
		return Default;
	#endif
	}

/*
=================================================
	IsGLMSupported
----
	returns true if GLM configuration
	is supported with current CPU features
=================================================
*/
	bool  CpuArchInfo::IsGLMSupported () C_NE___
	{
		#if (GLM_ARCH & GLM_ARCH_SSE2_BIT)
			CHECK_ERR( feats.SSE2 );
		#endif
		#if (GLM_ARCH & GLM_ARCH_SSE3_BIT)
			CHECK_ERR( feats.SSE3 );
		#endif
		#if (GLM_ARCH & GLM_ARCH_SSE41_BIT)
			CHECK_ERR( feats.SSE41 );
		#endif
		#if (GLM_ARCH & GLM_ARCH_SSE42_BIT)
			CHECK_ERR( feats.SSE42 );
		#endif
		#if (GLM_ARCH & GLM_ARCH_AVX_BIT)
			CHECK_ERR( feats.AVX );
		#endif
		#if (GLM_ARCH & GLM_ARCH_AVX2_BIT)
			CHECK_ERR( feats.AVX256 );
		#endif
		#if (GLM_ARCH & GLM_ARCH_NEON_BIT)
			CHECK_ERR( feats.NEON );
		#endif

		#if defined(AE_PLATFORM_WINDOWS)
			CHECK_ERR( feats.POPCNT );
		#endif

		return true;
	}

/*
=================================================
	GetCore
=================================================
*/
	CpuArchInfo::Core const*  CpuArchInfo::GetCore (const uint idx) C_NE___
	{
		for (auto& core : cpu.coreTypes) {
			if ( core.logicalBits.test( idx ))
				return &core;
		}
		return null;
	}

	CpuArchInfo::Core const*  CpuArchInfo::GetCore (ECoreType type) C_NE___
	{
		for (auto& core : cpu.coreTypes) {
			if ( core.type == type )
				return &core;
		}
		return null;
	}

/*
=================================================
	LogicalCoreMask
=================================================
*/
	CpuArchInfo::CoreBits_t  CpuArchInfo::LogicalCoreMask () C_NE___
	{
		CoreBits_t	mask;
		for (auto& core : cpu.coreTypes) {
			mask |= core.logicalBits;
		}
		return mask;
	}

/*
=================================================
	PhysicalCoreMask
=================================================
*/
	CpuArchInfo::CoreBits_t  CpuArchInfo::PhysicalCoreMask () C_NE___
	{
		CoreBits_t	mask;
		for (auto& core : cpu.coreTypes) {
			mask |= core.physicalBits;
		}
		return mask;
	}

/*
=================================================
	_Validate
=================================================
*/
	void  CpuArchInfo::_Validate () __NE___
	{
	#ifdef AE_DEBUG

		for (auto& core : cpu.coreTypes)
		{
			ASSERT( (core.LogicalCount() == core.PhysicalCount())	or
					(core.LogicalCount() == core.PhysicalCount()*2) );
			ASSERT( AllBits( core.logicalBits.to_ullong(), core.physicalBits.to_ullong() ));
		}

		const uint	num_threads		= std::thread::hardware_concurrency();
		const uint	logical_cores	= uint(LogicalCoreMask().count());
		ASSERT( num_threads == logical_cores );

	#endif
	}

/*
=================================================
	_NameToVendor
=================================================
*/
	ECPUVendor  CpuArchInfo::_NameToVendor (StringView name) __NE___
	{
		if ( HasSubString( name, "AMD" ))		return ECPUVendor::AMD;
		if ( HasSubString( name, "ARM" ))		return ECPUVendor::ARM;
		if ( HasSubString( name, "Apple" ))		return ECPUVendor::Apple;
		if ( HasSubString( name, "Intel" ))		return ECPUVendor::Intel;

		return Default;
	}


} // AE::Base
