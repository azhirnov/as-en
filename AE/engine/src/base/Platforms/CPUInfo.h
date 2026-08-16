// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

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
		Unknown						= 0,

		_AMD_Begin,
		AMD_Athlon2					= _AMD_Begin,
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
		AMD_Zen5c,
		_AMD_End,

		_Intel_Begin,
		_Intel_Client_Begin			= _Intel_Begin,
		Intel_Nehelem				= _Intel_Client_Begin,
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
		Intel_MeteorLake,
		Intel_ArrowLake,
		Intel_LunarLake,
		_Intel_Client_End,

		_Intel_Server_Begin			= _Intel_Client_End,
		Intel_SandyBridge_Server	= _Intel_Server_Begin,
		Intel_IvyBridge_Server,
		Intel_Haswell_Server,
		Intel_Broadwell_Server,
		Intel_Skylake_Server,
		Intel_IceLake_Server,
		Intel_SapphireRapids,
		Intel_EmeraldRapids,
		_Intel_Server_End,

		_Intel_LowPower_Begin		= _Intel_Server_End,
		Intel_Bonnell				= _Intel_LowPower_Begin,
		Intel_Saltwell,
		Intel_Silvermont,
		Intel_Airmont,
		Intel_Goldmont,
		Intel_GoldmontPlus,
		Intel_Tremont,
		_Intel_LowPower_End,
		_Intel_End					= _Intel_LowPower_End,

		_ARM_ISA_Begin,
		ARM_ISA_7_0					= _ARM_ISA_Begin,
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

		_Count
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
		_Count
	};

	// TODO
	// ARM:
	//	* Performance - per core L1, shared L2
	//	* Prime core - P with high clock, large L2, more SIMD pipes
	//	* E-core - energy efficient, low clock, low ILP, 1-2 SIMD pipes
	// x64:
	//	* Performance - per core L1 and L2, shared L3
	//	* Efficient - shared L2, lower clock, same SIMD
	//	* LP - shared L1?, low L2, low clock, less SIMD pipes - same as ARM E-core

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
			bool	SHA2_512		: 1;	// AE_SIMD_SHA	21		- SHA2-512
			// TODO: PMULL
		#endif

		// ARM features
		#ifdef AE_CPU_ARCH_ARM_BASED
			bool	NEON			: 1;	// AE_SIMD_NEON
			bool	NEON_fp16		: 1;	// AE_SIMD_NEON_HALF	- 'asimdhp', half-precision arithmetic
			bool	NEON_i8MM		: 1;	//						- matrix multiply: i8 x i8 = i32, replaced by 'smei8i32'

			bool	FP16C			: 1;	// AE_SIMD_F16C	1		- half-precision conversion on SIMD
			bool	SVE				: 1;	// AE_SIMD_SVE	1
			bool	SVE2			: 1;	// AE_SIMD_SVE	2

			bool	SME				: 1;	// AE_SIMD_SME	1

			bool	FP16			: 1;	//						- 'fphp'
			bool	BF16			: 1;

			bool	AES				: 1;	// AE_SIMD_AES	1
			bool	SVE_AES			: 1;	// AE_SIMD_AES	1 & AE_SIMD_SVE 1
			bool	SVE_SHA3		: 1;	// AE_SIMD_SHA	30 & AE_SIMD_SVE 1

			bool	CRC32			: 1;
			bool	SHA2_256		: 1;	// AE_SIMD_SHA	20		- SHA1 & SHA2-256 from Armv8-A
			bool	SHA2_512		: 1;	// AE_SIMD_SHA	21		- SHA2-512 from Armv8.2-A
			bool	SHA3			: 1;	// AE_SIMD_SHA	30		- Armv8.2
			// TODO: PMULL

			bool	Atomics			: 1;	// Armv8.1				- single instruction atomics, otherwise only load/store + emulation
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
			L1I,				// per core
			L1D,			// per core
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

			ND_ uint  FirstLogicalCore ()	C_NE___	{ return LowBitIndex( logicalBits.to_ullong() ); }
			ND_ uint  LastLogicalCore ()	C_NE___	{ return HighBitIndex( logicalBits.to_ullong() ); }
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
		CpuArchInfo ()																		__NE___;

		void  _Validate ()																	__NE___;

	public:
		ND_ String		Print ()															C_NE___;
		ND_ bool		CheckCompilationOptions ()											C_NE___;

		ND_ Core const*	GetCore (uint threadIdx)											C_NE___;
		ND_ Core const*	GetCore (ECoreType type)											C_NE___;

		ND_ CacheGeom const*  GetCache (ECacheType, ECoreType)								C_NE___;

		ND_ CoreBits_t	LogicalCoreMask (ECoreType excludeIfLessThan = ECoreType::_Count)	C_NE___;
		ND_ CoreBits_t	PhysicalCoreMask (ECoreType excludeIfLessThan = ECoreType::_Count)	C_NE___;

		ND_ static CpuArchInfo const&  Get ()												__NE___;
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

} // AE::Base
