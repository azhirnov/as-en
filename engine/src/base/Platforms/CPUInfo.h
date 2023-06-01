// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/Containers/FixedString.h"
#include "base/Containers/FixedArray.h"

namespace AE::Base
{

	enum class ECPUArch : ubyte
	{
		X86,
		X64,
		ARM_32,
		ARM_64,
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
	};

	enum class ECoreType : ubyte
	{
		// ARM: Cortex X1, X2
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

		struct CacheInfo
		{
			CacheGeom	L1_Inst;
			CacheGeom	L1_Data;
			CacheGeom	L2;
			CacheGeom	L3;
		};

		using MHz_t				= uint;
		using CoreBits_t		= BitSet< 256 >;
		static constexpr uint	MaxCores	= 4;


		struct Core
		{
			FixedString<64>	name;
			ECoreType		type			= Default;
			MHz_t			baseClock		= 0;
			MHz_t			maxClock		= 0;
			CoreBits_t		logicalBits		= {};	//  |- in global space, must not intersects with other cores
			CoreBits_t		physicalBits	= {};	// / 

			Core ()							__NE___ = default;

			ND_ uint  LogicalCount ()		C_NE___	{ return uint(logicalBits.count()); }
			ND_ uint  PhysicalCount ()		C_NE___	{ return uint(physicalBits.count()); }
			
			ND_ bool  HasVirtualCores ()	C_NE___	{ return logicalBits != physicalBits; }

			ND_ uint  FirstLogicalCore ()	C_NE___	{ return BitScanForward( logicalBits.to_ullong() ); }
			ND_ uint  LastLogicalCore ()	C_NE___	{ return BitScanReverse( logicalBits.to_ullong() ); }
		};
		using Cores_t	= FixedArray< Core, MaxCores >;


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
		Features	feats	= {};
		Processor	cpu		= {};
		CacheInfo	cache	= {};


	// methods
	private:
		CpuArchInfo ();

		void  _Validate ();

	public:
		ND_ String		Print ()					C_NE___;
		ND_ bool		IsGLMSupported ()			C_NE___;

		ND_ Core const*	GetCore (uint threadIdx)	C_NE___;
		ND_ Core const*	GetCore (ECoreType type)	C_NE___;

		ND_ CoreBits_t	LogicalCoreMask ()			C_NE___;
		ND_ CoreBits_t	PhysicalCoreMask ()			C_NE___;

		ND_ static CpuArchInfo const&  Get ()		__NE___;
	};



	//
	// CPU Performance Info 
	//

	struct CpuPerformance
	{
	// types
		using MHz_t	= uint;
		

	// methods
		ND_ static MHz_t  GetFrequency (uint core)										__NE___;
			static uint   GetFrequency (OUT MHz_t* result, uint maxCount)				__NE___;

			static uint   GetUsage (OUT float* user, OUT float* kernel, uint maxCount)	__NE___;
	};


} // AE::Base
