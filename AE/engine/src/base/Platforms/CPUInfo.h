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
		_Count,
		Unknown	= 0xFF,
	};

	enum class ECPUMicroArch : ushort
	{
		_AMD_Begin				= 0x1000,
		AMD_Athlon2,
		AMD_Phenom2,
		AMD_Turion2,
		AMD_Llano,
		AMD_Bobcat,
		AMD_Bulldozer,
		AMD_Piledriver,
		AMD_Steamroller,
		AMD_Excavator,
		AMD_Jaguar,
		AMD_Puma,
		AMD_Zen1,
		AMD_Zen1Plus,	// Zen+
		AMD_Zen2,
		AMD_Zen3,
		AMD_Zen4,
		AMD_Zen5,
		_AMD_End,

		_Intel_Begin			= 0x2000,
		_Intel_Client_Begin		= _Intel_Begin,
		Intel_Nehelem,
		Intel_Westmere,
		Intel_SandyBridge,
		Intel_IvyBridge,
		Intel_Haswell,
		Intel_Broadwell,
		Intel_Skylake,
		Intel_KabyLake,
		Intel_CoffeeLake,
		Intel_CannonLake,
		Intel_CometLake,
		Intel_IceLake,
		Intel_TigerLake,
		Intel_RocketLake,
		Intel_AlderLake,
		Intel_ReptorLake,
		_Intel_Client_End,

		_Intel_Server_Begin		= 0x2100,
		Intel_SandyBridge_Server,
		Intel_IvyBridge_Server,
		Intel_Haswell_Server,
		Intel_Broadwell_Server,
		Intel_Skylake_Server,
		Intel_IceLake_Server,
		Intel_SapphireRapids,
		Intel_EmeraldRapids,
		_Intel_Server_End,

		_Intel_LowPower_Begin	= 0x2200,
		Intel_Bonnell,
		Intel_Saltwell,
		Intel_Silvermont,
		Intel_Airmont,
		Intel_Goldmont,
		Intel_GoldmontPlus,
		Intel_Tremont,
		_Intel_LowPower_End,

		_ARM_ISA_Begin			= 0x3000,
		ARM_ISA_7_0,
		ARM_ISA_8_0,
		ARM_ISA_8_1,
		ARM_ISA_8_2,
		ARM_ISA_8_3,
		ARM_ISA_8_4,
		ARM_ISA_8_5,
		ARM_ISA_8_6,
		ARM_ISA_8_7,
		ARM_ISA_8_8,
		ARM_ISA_8_9,
		ARM_ISA_9_0,
		ARM_ISA_9_1,
		ARM_ISA_9_2,
		ARM_ISA_9_3,
		_ARM_ISA_End,

		Unknown	= 0xFFFF,
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
		// ARM: Cortex X1, X2, X3
		HighPerformance,

		// ARM: Cortex A72 - A78
		// All cores on x64 type processors.
		Performance,

		// ARM: Cortex A52 - A57
		// E-cores on new Intel processors.
		EnergyEfficient,

		_Count,
		Unknown	= 0xFF,

		HP	= HighPerformance,
		P	= Performance,
		EE	= EnergyEfficient,
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
		//	bool	SSE4A			: 1;	// AE_SIMD_SSE	50			- not available in Intel

			bool	AVX				: 1;	// AE_SIMD_AVX	1
			bool	AVX2			: 1;	// AE_SIMD_AVX	2
			bool	AVX512F			: 1;	// AE_SIMD_AVX	3			- Foundation
			bool	AVX512_FP16		: 1;	// AE_SIMD_AVX	3|(1<<4)	- half-precision floating-point arithmetic instructions
		//	bool	AVX512_DQ		: 1;	// AE_SIMD_AVX	3|(1<<5)	- Doubleword and Quadword Instructions
		//	bool	AVX512_IFMA		: 1;	// AE_SIMD_AVX	3|(1<<6)	- Integer Fused Multiply-Add Instructions
		//	bool	AVX512_ER		: 1;	// AE_SIMD_AVX	3|(1<<7)	- Exponential and Reciprocal Instructions
		//	bool	AVX512_BW		: 1;	// AE_SIMD_AVX	3|(1<<8)	- Byte and Word Instructions
		//	bool	AVX512_VL		: 1;	// AE_SIMD_AVX	3|(1<<9)	- Vector Length Extensions
		//	bool	AVX512_VBMI		: 1;	// AE_SIMD_AVX	3|(1<<10)	- Vector Bit Manipulation Instructions
		//	bool	AVX512_VBMI2	: 1;	// AE_SIMD_AVX	3|(1<<11)	- Vector Bit Manipulation Instructions 2
		//	bool	AVX512_BITALG	: 1;	// AE_SIMD_AVX	3|(1<<12)	- BITALG instructions
		//	bool	AVX512_VPOPCNTDQ: 1;	// AE_SIMD_AVX	3|(1<<13)	- Vector Population Count Double and Quad-word
		//	bool	AVX512_4FMAPS	: 1;	// AE_SIMD_AVX	3|(1<<14)	- 4-register Multiply Accumulation Single precision

		//	bool	AMX_BF16		: 1;	// matrices with bfloat16 type
		//	bool	AMX_Tile		: 1;	// tile load/store
		//	bool	AMX_I8			: 1;	// matrices with 8bit int type
		//	bool	AMX_FP16		: 1;	// matrices with float16 type

			bool	FP16C			: 1;	// AE_SIMD_F16C	1			- half-precision conversion
			bool	FMA				: 1;	// AE_SIMD_FMA	1
			bool	POPCNT			: 1;

			bool	AES				: 1;	// AE_SIMD_AES	1
			bool	VAES			: 1;	// AE_SIMD_AES	2			- AVX512-VAES
			bool	AESKL			: 1;	// AE_SIMD_AES	3			- AES Key Locker

			bool	SHA2_256		: 1;	// AE_SIMD_SHA	20			- SHA2-256 and SHA-1
			bool	SHA512			: 1;	// AE_SIMD_SHA	21			- SHA2-512 ?
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
			Bytes32u	size;

			CacheGeom () : lineSize{0}, associativity{0}, logicalCoreCount{0} {}
		};

		enum ECacheType : ubyte
		{
			L1_Instuction,		// per core
			L1_Data,			// per core
			L2,					// per core
			L3,
			_Count,
			Unknown = _Count,
		};

		static constexpr uint	MaxLogicalCores	= 64;
		static constexpr uint	MaxCoreTypes	= 4;

		using MHz_t				= uint;
		using CoreBits_t		= BitSet< MaxLogicalCores >;
		using CacheKey_t		= Pair< ECacheType, ECoreType >;
		using CacheInfoMap_t	= FixedMap< CacheKey_t, CacheGeom, 8 >;
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

			ND_ bool  HasVirtualCores ()	C_NE___	{ return logicalBits != physicalBits; }

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
	ND_ inline StringView  ToString (ECPUArch value) __NE___
	{
		switch_enum( value )
		{
			case ECPUArch::X86 :		return "x86";
			case ECPUArch::X64 :		return "x64";
			case ECPUArch::ARM_32 :		return "ARM-32";
			case ECPUArch::ARM_64 :		return "ARM-64";
			case ECPUArch::RISCV :		return "RISC-V";
			case ECPUArch::RISCV_64 :	return "RISC-V 64";
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
	ND_ inline StringView  ToString (ECoreType value) __NE___
	{
		switch_enum( value )
		{
			case ECoreType::HighPerformance :	return "HighPerformance";
			case ECoreType::Performance :		return "Performance";
			case ECoreType::EnergyEfficient :	return "EnergyEfficient";
			case ECoreType::_Count :
			case ECoreType::Unknown:			break;
		}
		switch_end
		return "";
	}

/*
=================================================
	ToString (ECPUVendor)
=================================================
*/
	ND_ inline StringView  ToString (ECPUVendor value) __NE___
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

#endif // AE_ENABLE_LOGS

} // AE::Base
