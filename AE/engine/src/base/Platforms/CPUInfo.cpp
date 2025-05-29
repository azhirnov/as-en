// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Platforms/CPUInfo.h"
#include "base/Math/BitMath.h"
#include "base/Algorithms/ToString.h"
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

			#ifdef AE_CPU_ARCH_X86_64
			str << "\n  SSE 2:       " << ToString( feats.SSE2 )
				<< "\n  SSE 3: . . . " << ToString( feats.SSE3 )
				<< "\n  SSSE 3:      " << ToString( feats.SSSE3 )
				<< "\n  SSE 4.1: . . " << ToString( feats.SSE41 )
				<< "\n  SSE 4.2:     " << ToString( feats.SSE42 )
				<< "\n  AVX: . . . . " << ToString( feats.AVX )
				<< "\n  AVX 2:       " << ToString( feats.AVX2 )
				<< "\n  AVX512F: . . " << ToString( feats.AVX512F )
				<< "\n  FMA: . . . . " << ToString( feats.FMA )
				<< "\n  POPCNT:      " << ToString( feats.POPCNT )
				<< "\n  AES: . . . . " << ToString( feats.AES )
				<< "\n  VAES:        " << ToString( feats.VAES )
				<< "\n  AESKL: . . . " << ToString( feats.AESKL )
				<< "\n  SHA2_256:    " << ToString( feats.SHA2_256 )
				<< "\n  SHA512:  . . " << ToString( feats.SHA512 )
				<< "\n  FP16C:       " << ToString( feats.FP16C );
			#endif
			#ifdef AE_CPU_ARCH_ARM_BASED
			str << "\n  NEON:  . . . " << ToString( feats.NEON )
				<< "\n  NEON_fp16:   " << ToString( feats.NEON_fp16 )
				<< "\n  FP16C: . . . " << ToString( feats.FP16C )
				<< "\n  SVE:         " << ToString( feats.SVE )
				<< "\n  SVE2:  . . . " << ToString( feats.SVE2 )
				<< "\n  BF16:        " << ToString( feats.BF16 )
				<< "\n  AES: . . . . " << ToString( feats.AES )
				<< "\n  SVE_AES:     " << ToString( feats.SVE_AES )
				<< "\n  CRC32: . . . " << ToString( feats.CRC32 )
				<< "\n  SHA2_256:    " << ToString( feats.SHA2_256 )
				<< "\n  SHA2_512:  . " << ToString( feats.SHA2_512 )
				<< "\n  SHA3:        " << ToString( feats.SHA3 )
				<< "\n  Atomics: . . " << ToString( feats.Atomics );
			#endif

			// shared features
			str	<< "\n  CmpXchg16:   " << ToString( feats.CmpXchg16 );

			str << "\n--------------------"
				<< "\nCPU info:"
				<< "\n  vendor:       " << ToString( cpu.vendor )
				<< "\n  architecture: " << ToString( cpu.arch ) << " (" << ToString( cpu.microArch ) << ")"
				<< "\n  total cores:  " << ToString( cpu.physicalCoreCount ) << " / " << ToString( cpu.logicalCoreCount );

			const auto	PrintCache = [&str] (ECacheType cacheType, const CacheGeom &g, uint logicalCoreCount)
			{{
				String	name {ToString( cacheType )};
				if ( name.length() == 2 ) name << ' ';

				if ( g.lineSize > 0 )			str	<< "\n    " << name << ".lineSize:      " << ToString( g.lineSize ) << " B";
				if ( g.associativity > 0 )		str << "\n    " << name << ".associativity: " << ToString( g.associativity );
				if ( g.logicalCoreCount > 0 )	str << "\n    " << name << ".cores          " << ToString( g.logicalCoreCount );
				if ( g.size > 0 )				str << "\n    " << name << ".size:  . . . . " << ToString( g.size );

				if ( g.size > 0 and g.logicalCoreCount > 0 and logicalCoreCount > 0 )
												str << "\n    " << name << ".total:         " << ToString( g.size * (logicalCoreCount / g.logicalCoreCount) );
			}};

			uint	total_cores = 0;
			for (auto& core : cpu.coreTypes)
			{
				str << "\n  cluster:      " << StringView{core.name}
					<< "\n    type:       " << ToString( core.type )
					<< "\n    base clock: " << ToString( core.baseClock ) << " MHz"
					<< "\n    max clock:  " << ToString( core.maxClock )  << " MHz"
					<< "\n    threads:    " << ToString( core.PhysicalCount() ) << " / " << ToString( core.LogicalCount() )
					<< "\n    IDs:        [" << ToString( core.FirstLogicalCore() ) << ", " << ToString( core.LastLogicalCore()+1 ) << ')';

				const uint	core_cnt = core.LogicalCount();
				total_cores += core_cnt;

				for (uint i = 0; i < uint(ECacheType::_Count); ++i)
				{
					if (auto* c = GetCache( ECacheType(i), core.type ))
					{
						str	<< "\n    -----";
						PrintCache( ECacheType(i), *c, core_cnt );
					}
				}
				str	<< "\n    ----------";
			}

			str << "\nCache info:";
			for (uint i = 0; i < uint(ECacheType::_Count); ++i)
			{
				if (auto* c = GetCache( ECacheType(i), ECoreType::Unknown ))
				{
					PrintCache( ECacheType(i), *c, total_cores );
					str	<< "\n    ----------";
				}
			}

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
	CheckCompilationOptions
----
	returns true if:
		- GLM configuration is compatible with current CPU features
		- engine compilation flags are compatible with current CPU features
=================================================
*/
	bool  CpuArchInfo::CheckCompilationOptions () C_NE___
	{
		#if (AE_SIMD_SSE >= 20) or (GLM_ARCH & GLM_ARCH_SSE2_BIT)
			CHECK_ERR_MSG( feats.SSE2,		"AE_SIMD_SSE=20 requires SSE2 feature" );
		#endif
		#if (AE_SIMD_SSE >= 30) or (GLM_ARCH & GLM_ARCH_SSE3_BIT)
			CHECK_ERR_MSG( feats.SSE3,		"AE_SIMD_SSE=30 requires SSE3 feature" );
		#endif
		#if (AE_SIMD_SSE >= 31)
			CHECK_ERR_MSG( feats.SSSE3,		"AE_SIMD_SSE=31 requires SSSE3 feature" );
		#endif
		#if (AE_SIMD_SSE >= 41) or (GLM_ARCH & GLM_ARCH_SSE41_BIT)
			CHECK_ERR_MSG( feats.SSE41,		"AE_SIMD_SSE=41 requires SSE41 feature" );
		#endif
		#if (AE_SIMD_SSE >= 42) or (GLM_ARCH & GLM_ARCH_SSE42_BIT)
			CHECK_ERR_MSG( feats.SSE42,		"AE_SIMD_SSE=42 requires SSE42 feature" );
		#endif
		#if (AE_SIMD_SSE >= 50)
			CHECK_ERR_MSG( feats.SSE4A,		"AE_SIMD_SSE=50 requires SSE4A feature" );
		#endif
		#if (AE_SIMD_AVX >= 1) or (GLM_ARCH & GLM_ARCH_AVX_BIT)
			CHECK_ERR_MSG( feats.AVX,		"AE_SIMD_AVX=1 requires AVX feature" );
		#endif
		#if (AE_SIMD_AVX >= 2) or (GLM_ARCH & GLM_ARCH_AVX2_BIT)
			CHECK_ERR_MSG( feats.AVX2,		"AE_SIMD_AVX=2 requires AVX2 feature" );
		#endif

		#if (AE_SIMD_AVX >= 30)
			CHECK_ERR_MSG( feats.AVX512F,	"AE_SIMD_AVX=30 requires AVX512F feature" );
		#endif
		#if (AE_SIMD_AVX >= 31)
		//	CHECK_ERR_MSG( feats.AVX512_CD,		"AE_SIMD_AVX=31 requires AVX512_CD feature" );
		//	CHECK_ERR_MSG( feats.AVX512_VL,		"AE_SIMD_AVX=31 requires AVX512_VL feature" );
		//	CHECK_ERR_MSG( feats.AVX512_DQ,		"AE_SIMD_AVX=31 requires AVX512_DQ feature" );
		//	CHECK_ERR_MSG( feats.AVX512_BW,		"AE_SIMD_AVX=31 requires AVX512_BW feature" );
		//	CHECK_ERR_MSG( feats.AVX512_IFMA,	"AE_SIMD_AVX=31 requires AVX512_IFMA feature" );
		//	CHECK_ERR_MSG( feats.AVX512_VBMI,	"AE_SIMD_AVX=31 requires AVX512_VBMI feature" );
		#endif
		#if (AE_SIMD_AVX >= 32)
		//	CHECK_ERR_MSG( feats.AVX512_VBMI2,		"AE_SIMD_AVX=32 requires AVX512_VBMI2 feature" );
		//	CHECK_ERR_MSG( feats.AVX512_VPOPCNTDQ,	"AE_SIMD_AVX=32 requires AVX512_VPOPCNTDQ feature" );
		//	CHECK_ERR_MSG( feats.AVX512_BITALG,		"AE_SIMD_AVX=32 requires AVX512_BITALG feature" );
		//	CHECK_ERR_MSG( feats.AVX512_VNNI,		"AE_SIMD_AVX=32 requires AVX512_VNNI feature" );
		//	CHECK_ERR_MSG( feats.AVX512_VPCLMULQDQ,	"AE_SIMD_AVX=32 requires AVX512_VPCLMULQDQ feature" );
		//	CHECK_ERR_MSG( feats.AVX512_GFNI,		"AE_SIMD_AVX=32 requires AVX512_GFNI feature" );
			CHECK_ERR_MSG( feats.VAES,				"AE_SIMD_AVX=32 requires VAES feature" );
		#endif
		#if (AE_SIMD_AVX >= 33)
		//	CHECK_ERR_MSG( feats.AVX512_BF16,		"AE_SIMD_AVX=33 requires AVX512_BF16 feature" );
		#endif

		#if AE_SIMD_NEON or (GLM_ARCH & GLM_ARCH_NEON_BIT)
			CHECK_ERR_MSG( feats.NEON,		"AE_SIMD_NEON requires NEON feature" );
		#endif
		#if AE_SIMD_NEON_HALF
			CHECK_ERR_MSG( feats.NEON_fp16,	"AE_SIMD_NEON_HALF requires NEON_fp16 feature" );
		#endif
		#if AE_SIMD_SVE >= 1
			CHECK_ERR_MSG( feats.SVE,		"AE_SIMD_SVE=1 requires SVE feature" );
		#endif
		#if AE_SIMD_SVE >= 2
			CHECK_ERR_MSG( feats.SVE2,		"AE_SIMD_SVE=2 requires SVE2 feature" );
		#endif

		#ifdef AE_CPU_ARCH_X86_64
		# if AE_SIMD_FMA or defined(GLM_FORCE_FMA)
			CHECK_ERR_MSG( feats.FMA,		"AE_SIMD_FMA requires FMA feature" );
		# endif
		#endif

		#if defined(AE_PLATFORM_WINDOWS)
			CHECK_ERR( feats.POPCNT );
		#endif
		#if AE_SIMD_F16C
			CHECK_ERR( feats.FP16C,			"AE_SIMD_F16C requires FP16C feature" );
		#endif
		#ifdef AE_CPU_ARCH_ARM_BASED
			CHECK_ERR( feats.CRC32 );
		#endif

		#if AE_SIMD_SHA
		# if AE_SIMD_SHA >= 20
			CHECK_ERR_MSG( feats.SHA2_256,	"AE_SIMD_SHA=20 requires SHA2_256 feature" );
		# endif
		# if AE_SIMD_SHA >= 21
			CHECK_ERR_MSG( feats.SHA2_512,	"AE_SIMD_SHA=21 requires SHA2_512 feature" );
		# endif
		# if AE_SIMD_SHA >= 30
			CHECK_ERR_MSG( feats.SHA3,		"AE_SIMD_SHA=30 requires SHA3 feature" );
		# endif
		#endif

		#if AE_SIMD_AES
			CHECK_ERR_MSG( feats.AES,		"AE_SIMD_AES=1 requires AES feature" );
		# if AE_SIMD_AES >= 2
			CHECK_ERR_MSG( feats.VAES,		"AE_SIMD_AES=2 requires VAES feature" );
		# endif
		# if AE_SIMD_AES >= 3
			CHECK_ERR_MSG( feats.AESKL,		"AE_SIMD_AES=3 requires AESKL feature" );
		# endif
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
	GetCache
=================================================
*/
	CpuArchInfo::CacheGeom const*  CpuArchInfo::GetCache (ECacheType cacheType, ECoreType coreType) C_NE___
	{
		auto	it = cache.find( MakePair( cacheType, coreType ));
		if ( it != cache.end() )
			return &it->second;

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


} // AE::Base
