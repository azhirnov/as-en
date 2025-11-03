// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/Containers/FixedString.h"
#include "base/Containers/FixedArray.h"
#include "base/Containers/FixedMap.h"

namespace AE::Base
{

	enum class ECPUArch : ubyte
	{
		X86,
		X64,
		ARM_32,		// ARMv7
		ARM_64,		// ARMv8
		RISCV,
		RISCV_64,
		Loong64,
		E2K,
		_Count,
		Unknown	= 0xFF,
	};

	enum class ECPUMicroArch : ushort
	{
		#define CPU_MICRO_ARCH( _visitor_ )\
			_visitor_( AMD_Athlon2,			= _AMD_Begin )\
			_visitor_( AMD_Phenom2,			)\
			_visitor_( AMD_Turion2,			)\
			_visitor_( AMD_Llano,			)\
			_visitor_( AMD_Bobcat,			)\
			_visitor_( AMD_Bulldozer,		)\
			_visitor_( AMD_Piledriver,		)\
			_visitor_( AMD_Steamroller,		)\
			_visitor_( AMD_Excavator,		)\
			_visitor_( AMD_Jaguar,			)\
			_visitor_( AMD_Puma,			)\
			_visitor_( AMD_Zen1,			)\
			_visitor_( AMD_Zen1Plus,		)	/* Zen+ */\
			_visitor_( AMD_Zen2,			)\
			_visitor_( AMD_Zen3,			)\
			_visitor_( AMD_Zen4,			)\
			_visitor_( AMD_Zen5,			)\
			_visitor_( AMD_Zen5c,			)\
			\
			_visitor_( Intel_Nehelem,		= _Intel_Client_Begin )\
			_visitor_( Intel_Westmere,		)\
			_visitor_( Intel_SandyBridge,	)\
			_visitor_( Intel_IvyBridge,		)\
			_visitor_( Intel_Haswell,		)\
			_visitor_( Intel_Broadwell,		)\
			_visitor_( Intel_Skylake,		)\
			_visitor_( Intel_KabyLake,		)\
			_visitor_( Intel_CoffeeLake,	)\
			_visitor_( Intel_CannonLake,	)\
			_visitor_( Intel_CometLake,		)\
			_visitor_( Intel_IceLake,		)\
			_visitor_( Intel_TigerLake,		)\
			_visitor_( Intel_RocketLake,	)\
			_visitor_( Intel_AlderLake,		)\
			_visitor_( Intel_ReptorLake,	)\
			_visitor_( Intel_MeteorLake,	)\
			_visitor_( Intel_ArrowLake,		)\
			_visitor_( Intel_LunarLake,		)\
			\
			_visitor_( Intel_SandyBridge_Server,	= _Intel_Server_Begin )\
			_visitor_( Intel_IvyBridge_Server,		)\
			_visitor_( Intel_Haswell_Server,		)\
			_visitor_( Intel_Broadwell_Server,		)\
			_visitor_( Intel_Skylake_Server,		)\
			_visitor_( Intel_IceLake_Server,		)\
			_visitor_( Intel_SapphireRapids,		)\
			_visitor_( Intel_EmeraldRapids,			)\
			\
			_visitor_( Intel_Bonnell,		= _Intel_LowPower_Begin )\
			_visitor_( Intel_Saltwell,		)\
			_visitor_( Intel_Silvermont,	)\
			_visitor_( Intel_Airmont,		)\
			_visitor_( Intel_Goldmont,		)\
			_visitor_( Intel_GoldmontPlus,	)\
			_visitor_( Intel_Tremont,		)\
			\
			_visitor_( ARM_ISA_7_0,		)\
			_visitor_( ARM_ISA_8_0,		)\
			_visitor_( ARM_ISA_8_1,		)\
			_visitor_( ARM_ISA_8_2,		)\
			_visitor_( ARM_ISA_8_3,		)\
			_visitor_( ARM_ISA_8_4,		)\
			_visitor_( ARM_ISA_8_5,		)\
			_visitor_( ARM_ISA_8_6,		)\
			_visitor_( ARM_ISA_8_7,		)\
			_visitor_( ARM_ISA_8_8,		)\
			_visitor_( ARM_ISA_8_9,		)\
			_visitor_( ARM_ISA_9_0,		)\
			_visitor_( ARM_ISA_9_1,		)\
			_visitor_( ARM_ISA_9_2,		)\
			_visitor_( ARM_ISA_9_3,		)\
		
		_AMD_Begin				= 0x1000,
		_Intel_Begin			= 0x2000,
		_Intel_Client_Begin		= _Intel_Begin,
		_Intel_Server_Begin		= 0x2100,
		_Intel_LowPower_Begin	= 0x2200,
		_ARM_ISA_Begin			= 0x3000,

		#define CPU_MICRO_ARCH_VISIT( _name_, _setValue_ )		_name_	_setValue_,
		CPU_MICRO_ARCH( CPU_MICRO_ARCH_VISIT )
		#undef CPU_MICRO_ARCH_VISIT

		_AMD_End,
		_Intel_Client_End		= Intel_ReptorLake,
		_Intel_Server_End		= Intel_EmeraldRapids,
		_Intel_LowPower_End		= Intel_Tremont,
		_ARM_ISA_End			= ARM_ISA_9_3,
		Unknown					= 0xFFFF,
	};

	enum class ECPUVendor : ubyte
	{
		Unknown,
		AMD,
		ARM,
		Apple,
		Intel,
		Qualcomm,
		Broadcom,
		Cavium,
		Fujitsu,
		HiSilicon,
		NVidia,
		AppliedMicro,
		Samsung,
		Marvell,
		HuaxintongSemiconductor,
		Ampere,
		Loongson,
		Virtual,		// virtualization, origin vendor is hidden
	};

	enum class ECoreType : ubyte
	{
		// ARM: Cortex X1, X2, X925, ...
		HighPerformance,

		// ARM: Cortex A72 - A78, A720, ... 
		// All cores on x64 type processors.
		Performance,

		// ARM: Cortex A52 - A57, A520, ...
		// E-cores on new Intel processors, AMD ZenC.
		EnergyEfficient,

		// Intel LP core
		LowPower,

		_Count,
		Unknown	= 0xFF,

		HP	= HighPerformance,
		P	= Performance,
		EE	= EnergyEfficient,
		LP	= LowPower,
	};



	//
	// CPU Architecture Info
	//

	struct CpuArchInfo
	{
	// types
	public:
		struct Features
		{
		// x86-x64 features
		#ifdef AE_CPU_ARCH_X86_64
			bool	SSE2			: 1;	// AE_SIMD_SSE	20
			bool	SSE3			: 1;	// AE_SIMD_SSE	30
			bool	SSSE3			: 1;	// AE_SIMD_SSE	31
			bool	SSE41			: 1;	// AE_SIMD_SSE	41
			bool	SSE42			: 1;	// AE_SIMD_SSE	42
		//	bool	SSE4A			: 1;	// AE_SIMD_SSE	50		- not available on Intel

			bool	AVX				: 1;	// AE_SIMD_AVX	1
			bool	AVX2			: 1;	// AE_SIMD_AVX	2

			// Twin Lake //
			bool	AVX_VNNI		: 1;	// AE_SIMD_AVX	21		- 256bit vector neural network instructions
			bool	AVX_VNNI_i8		: 1;
			bool	AVX_VNNI_i16	: 1;

			// ??? //
			bool	AVX_10			: 1;	// AE_SIMD_AVX	23

			bool	AVX512F			: 1;	// AE_SIMD_AVX	30		- Foundation

			// Cannon Lake //
			bool	AVX512_CD		: 1;	// AE_SIMD_AVX	31		-
			bool	AVX512_VL		: 1;	// AE_SIMD_AVX	31		- Vector Length Extensions
			bool	AVX512_DQ		: 1;	// AE_SIMD_AVX	31		- Doubleword and Quadword Instructions
			bool	AVX512_BW		: 1;	// AE_SIMD_AVX	31		- Byte and Word Instructions
			bool	AVX512_IFMA		: 1;	// AE_SIMD_AVX	31		- Integer Fused Multiply-Add Instructions
			bool	AVX512_VBMI		: 1;	// AE_SIMD_AVX	31		- Vector Bit Manipulation Instructions

			// Ice Lake //
			bool	AVX512_VBMI2	: 1;	// AE_SIMD_AVX	32		- Vector Bit Manipulation Instructions 2
			bool	AVX512_VPOPCNTDQ: 1;	// AE_SIMD_AVX	32		- Vector Population Count Double and Quad-word
			bool	AVX512_BITALG	: 1;	// AE_SIMD_AVX	32		- BITALG instructions
			bool	AVX512_VNNI		: 1;	// AE_SIMD_AVX	32		- vector neural network instructions
			bool	AVX512_VPCLMULQDQ: 1;	// AE_SIMD_AVX	32		-
			bool	AVX512_GFNI		: 1;	// AE_SIMD_AVX	32		-
			bool	VAES			: 1;	// AE_SIMD_AVX	32	or AE_SIMD_AES	2	- AVX512-VAES

			// Zen4 //
			bool	AVX512_BF16		: 1;	// AE_SIMD_AVX	33		- bfloat16 type

		//	bool	AMX_BF16		: 1;	// AE_SIMD_AMX	1		- matrices with bfloat16 type
		//	bool	AMX_Tile		: 1;	// AE_SIMD_AMX	1		- tile load/store
		//	bool	AMX_I8			: 1;	// AE_SIMD_AMX	1		- matrices with 8bit int type
		//	bool	AMX_FP16		: 1;	// AE_SIMD_AMX	1		- matrices with float16 type

		//	bool	BMI2			: 1;
			bool	FP16C			: 1;	// AE_SIMD_F16C	1		- half-precision conversion
			bool	FMA				: 1;	// AE_SIMD_FMA	1
			bool	POPCNT			: 1;

			bool	AES				: 1;	// AE_SIMD_AES	1
			bool	AESKL			: 1;	// AE_SIMD_AES	3		- AES Key Locker

			bool	SHA2_256		: 1;	// AE_SIMD_SHA	20		- SHA2-256 and SHA-1
			bool	SHA512			: 1;	// AE_SIMD_SHA	21		- SHA2-512 ?
		#endif

		// ARM features
		#ifdef AE_CPU_ARCH_ARM_BASED
			bool	NEON			: 1;	// AE_SIMD_NEON
			bool	NEON_fp16		: 1;	// AE_SIMD_NEON_HALF	- half-precision arithmetic
			bool	FP16C			: 1;	// AE_SIMD_F16C	1		- half-precision conversion
			bool	SVE				: 1;	// AE_SIMD_SVE	1
			bool	SVE2			: 1;	// AE_SIMD_SVE	2
			bool	BF16			: 1;

			bool	AES				: 1;	// AE_SIMD_AES	1
			bool	SVE_AES			: 1;	// AE_SIMD_AES	1 & AE_SIMD_SVE 1
			bool	SVE_SHA3		: 1;	// AE_SIMD_SHA	30 & AE_SIMD_SVE 1

			bool	CRC32			: 1;
			bool	SHA2_256		: 1;	// AE_SIMD_SHA	20		- SHA1 & SHA2-256 from Armv8-A
			bool	SHA2_512		: 1;	// AE_SIMD_SHA	21		- SHA2-512 from Armv8.2-A
			bool	SHA3			: 1;	// AE_SIMD_SHA	30		- Armv8.2

			bool	Atomics			: 1;	// Armv8.1
		#endif

		// RISC-V features
			// TODO

		// shared features
			bool	CmpXchg16		: 1;	// 128 bit atomic compare exchange
		};

		struct CacheGeom
		{
			uint		lineSize			: 16;	// bytes
			uint		associativity		: 8;
			uint		logicalCoreCount	: 8;
			Bytes32u	size;						// per 'logicalCoreCount'

			CacheGeom () : lineSize{0}, associativity{0}, logicalCoreCount{0} {}
		};

		enum ECacheType : ubyte
		{
			L1_Instuction,		// per core
			L1_Data,			// per core
			L2,					// per core or shared
			L3,					// shared
			_Count,
			Unknown = _Count,
		};

		static constexpr uint	MaxLogicalCores	= 64;
		static constexpr uint	MaxCoreTypes	= 4;

		using MHz_t				= uint;
		using CoreBits_t		= BitSet< MaxLogicalCores >;
		using CacheKey_t		= Pair< ECacheType, ECoreType >;
		using CacheInfoMap_t	= FixedMap< CacheKey_t, CacheGeom, 4*MaxCoreTypes >;
		using CPUName_t			= FixedString<64>;

		struct Core
		{
		// variables
			CPUName_t			name;
			ECoreType			type			= Default;
			MHz_t				baseClock		= 0;
			MHz_t				maxClock		= 0;
			CoreBits_t			logicalBits		= {};	// \__ in global space, must not intersects with other cores
			CoreBits_t			physicalBits	= {};	// /

		// methods
			Core ()							__NE___ = default;

			ND_ uint  LogicalCount ()		C_NE___	{ return uint(logicalBits.count()); }
			ND_ uint  PhysicalCount ()		C_NE___	{ return uint(physicalBits.count()); }

			ND_ bool  HasLogicalCores ()	C_NE___	{ return logicalBits != physicalBits; }

			ND_ uint  FirstLogicalCore ()	C_NE___	{ return BitScanForward( logicalBits.to_ullong() ); }
			ND_ uint  LastLogicalCore ()	C_NE___	{ return BitScanReverse( logicalBits.to_ullong() ); }
		};
		using Cores_t	= FixedArray< Core, MaxCoreTypes >;


		struct Processor
		{
			ECPUVendor		vendor				= Default;
			ECPUArch		arch				= Default;
			ECPUMicroArch	microArch			= Default;
			uint			modelId				= 0;
			uint			physicalCoreCount	= 0;
			uint			logicalCoreCount	= 0;
			Cores_t			coreTypes;
		};


	// variables
	public:
		Features		feats	= {};
		Processor		cpu		= {};
		CacheInfoMap_t	cache;


	// methods
	private:
		CpuArchInfo ()												__NE___;

		void  _Validate ()											__NE___;

	public:
		ND_ String		Print ()									C_NE___;
		ND_ bool		CheckCompilationOptions ()					C_NE___;

		ND_ Core const*	GetCore (uint threadIdx)					C_NE___;
		ND_ Core const*	GetCore (ECoreType type)					C_NE___;

		ND_ CacheGeom const*  GetCache (ECacheType, ECoreType)		C_NE___;

		ND_ CoreBits_t	LogicalCoreMask ()							C_NE___;
		ND_ CoreBits_t	PhysicalCoreMask ()							C_NE___;

		ND_ static CpuArchInfo const&  Get ()						__NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	operator == (CacheKey_t)
=================================================
*/
	inline bool  operator == (const CpuArchInfo::CacheKey_t &lhs, const CpuArchInfo::CacheKey_t &rhs) __NE___
	{
		return	lhs.first	== rhs.first	and
				lhs.second	== rhs.second;
	}

/*
=================================================
	operator < (CacheKey_t)
=================================================
*/
	inline bool  operator < (const CpuArchInfo::CacheKey_t &lhs, const CpuArchInfo::CacheKey_t &rhs) __NE___
	{
		return	lhs.first != rhs.first ?	lhs.first	< rhs.first :
											lhs.second	< rhs.second;
	}


#ifdef AE_ENABLE_LOGS
/*
=================================================
	ToString (ECPUArch)
=================================================
*/
	Nd__In StringView  ToString (ECPUArch value) __NE___
	{
		switch_enum( value )
		{
			case ECPUArch::X86 :		return "x86";
			case ECPUArch::X64 :		return "x64";
			case ECPUArch::ARM_32 :		return "ARM-32";
			case ECPUArch::ARM_64 :		return "ARM-64";
			case ECPUArch::RISCV :		return "RISC-V";
			case ECPUArch::RISCV_64 :	return "RISC-V 64";
			case ECPUArch::Loong64 :	return "Loong64";
			case ECPUArch::E2K :		return "E2K";
			case ECPUArch::_Count :
			case ECPUArch::Unknown:		break;
		}
		switch_end
		return "";
	}

/*
=================================================
	ToString (ECoreType)
=================================================
*/
	Nd__In StringView  ToString (ECoreType value) __NE___
	{
		switch_enum( value )
		{
			case ECoreType::HighPerformance :	return "HighPerformance";
			case ECoreType::Performance :		return "Performance";
			case ECoreType::EnergyEfficient :	return "EnergyEfficient";
			case ECoreType::LowPower :			return "LowPower";
			case ECoreType::_Count :
			case ECoreType::Unknown:			break;
		}
		switch_end
		return "";
	}
	
/*
=================================================
	ToString (ECacheType)
=================================================
*/
	Nd__In StringView  ToString (CpuArchInfo::ECacheType value) __NE___
	{
		switch_enum( value )
		{
			case CpuArchInfo::ECacheType::L1_Instuction :	return "L1I";
			case CpuArchInfo::ECacheType::L1_Data :			return "L1D";
			case CpuArchInfo::ECacheType::L2 :				return "L2";
			case CpuArchInfo::ECacheType::L3 :				return "L3";
			case CpuArchInfo::ECacheType::_Count :			break;
		}
		switch_end
		return "";
	}

/*
=================================================
	ToString (ECPUVendor)
=================================================
*/
	Nd__In StringView  ToString (ECPUVendor value) __NE___
	{
		switch_enum( value )
		{
			case ECPUVendor::AMD :			return "AMD";
			case ECPUVendor::ARM :			return "ARM";
			case ECPUVendor::Apple :		return "Apple";
			case ECPUVendor::Intel :		return "Intel";
			case ECPUVendor::Qualcomm :		return "Qualcomm";
			case ECPUVendor::Broadcom :		return "Broadcom";
			case ECPUVendor::Cavium :		return "Cavium";
			case ECPUVendor::Fujitsu :		return "Fujitsu";
			case ECPUVendor::HiSilicon :	return "HiSilicon";
			case ECPUVendor::NVidia :		return "NVidia";
			case ECPUVendor::AppliedMicro :	return "AppliedMicro";
			case ECPUVendor::Samsung :		return "Samsung";
			case ECPUVendor::Marvell :		return "Marvell";
			case ECPUVendor::HuaxintongSemiconductor :	return "HuaxintongSemiconductor";
			case ECPUVendor::Ampere :		return "Ampere";
			case ECPUVendor::Loongson :		return "Loongson";
			case ECPUVendor::Virtual :		return "Virtual";
			case ECPUVendor::Unknown:		break;
		}
		switch_end
		return "";
	}
	
/*
=================================================
	ToString (ECPUMicroArch)
=================================================
*/
	Nd__In StringView  ToString (ECPUMicroArch value) __NE___
	{
		switch ( value )
		{
			#define CPU_MICRO_ARCH_VISIT( _name_, _setValue_ )		case ECPUMicroArch::_name_ :	return AE_TOSTRING( _name_ );
			CPU_MICRO_ARCH( CPU_MICRO_ARCH_VISIT )
			#undef CPU_MICRO_ARCH_VISIT
		}
		return "";
	}

#endif // AE_ENABLE_LOGS

} // AE::Base
