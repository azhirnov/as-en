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
		Loongson
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
			bool	AVX			: 1;	// AE_SIMD_AVX	1
			bool	AVX256		: 1;	// AE_SIMD_AVX	2
			bool	AVX512		: 1;	// AE_SIMD_AVX	3

			bool	SSE2		: 1;	// AE_SIMD_SSE	20
			bool	SSE3		: 1;	// AE_SIMD_SSE	30
			bool	SSSE3		: 1;	// AE_SIMD_SSE	31
			bool	SSE41		: 1;	// AE_SIMD_SSE	41
			bool	SSE42		: 1;	// AE_SIMD_SSE	42

			bool	POPCNT		: 1;

			bool	AES			: 1;	// AE_SIMD_AES	1

			bool	CRC32		: 1;
			bool	SHA128		: 1;
			bool	SHA256		: 1;
			bool	SHA512		: 1;
			bool	SHA3		: 1;

		// ARM features
			bool	NEON		: 1;	// AE_SIMD_NEON
			bool	NEON_fp16	: 1;
			bool	NEON_hpfp	: 1;	// half precision
			bool	SVE			: 1;
			bool	SVE2		: 1;
			bool	SVEAES		: 1;

		// shared features
			bool	CmpXchg16	: 1;	// 128 bit atomic compare exchange
		};

		struct CacheGeom
		{
			uint		lineSize		: 16;	// bytes
			uint		associativity	: 16;
			Bytes32u	size;

			CacheGeom () : lineSize{0}, associativity{0} {}
		};

		enum ECacheType : ubyte
		{
			L1_Instuction,
			L1_Data,
			L2,
			L3,
			_Count
		};

		static constexpr uint	MaxLogicalCores	= 64;
		static constexpr uint	MaxCoreTypes	= 4;

		using MHz_t				= uint;
		using CoreBits_t		= BitSet< MaxLogicalCores >;
		using CacheKey_t		= Pair< ECacheType, ECoreType >;
		using CacheInfoMap_t	= FixedMap< CacheKey_t, CacheGeom, 8 >;

		struct Core
		{
		// variables
			FixedString<64>		name;
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

		ND_ static ECPUVendor  _NameToVendor (StringView)			__NE___;

	public:
		ND_ String		Print ()									C_NE___;
		ND_ bool		IsGLMSupported ()							C_NE___;

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
			case ECPUVendor::Unknown:		break;
		}
		switch_end
		return "";
	}

#endif // AE_ENABLE_LOGS

} // AE::Base
