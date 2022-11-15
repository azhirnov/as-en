// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/Containers/FixedString.h"
#include "base/Containers/FixedArray.h"

namespace AE::Base
{

	enum class ECPUArch : ubyte
	{
		X64,
		ARM_32,
		ARM_64,
		_Count,
		Unknown	= 0xFF,
	};

	enum class ECoreType : ubyte
	{
		// Android: Cortex X1
		HighPerformance,

		// Android: Cortex A72 - A78
		// All cores on x64 type processors.
		Performance,

		// Android: Cortex A52 - A57
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
	private:
		struct Features
		{
		// x86-x64 features
			bool	AVX			: 1;	// AE_SIMD_AVX 1
			bool	AVX256		: 1;	// AE_SIMD_AVX 2
			bool	AVX512		: 1;

			bool	SSE2		: 1;	// AE_SIMD_SSE 2
			bool	SSE3		: 1;	// AE_SIMD_SSE 3
			bool	SSE41		: 1;	// AE_SIMD_SSE 4.1
			bool	SSE42		: 1;	// AE_SIMD_SSE 4.2

			bool	POPCNT		: 1;

			bool	AES			: 1;	// AE_SIMD_AES 1

			bool	SHA256		: 1;
			bool	SHA512		: 1;

		// ARM features
			bool	NEON		: 1;	// AE_SIMD_NEON
			bool	NEON_fp16	: 1;
			bool	NEON_hpfp	: 1;	// half precission

		// shared features
			bool	CmpXchg16	: 1;	// 128 bit atomic compare exchange
		};

		using CoreBits_t		= BitSet< 256 >;
		static constexpr uint	MaxCores	= 4;

		struct Core
		{
			FixedString<64>	name;
			ECoreType		type			= Default;
			uint			baseClock		= 0;	// MHz
			uint			maxClock		= 0;	// MHz
			CoreBits_t		logicalBits		= {};
			CoreBits_t		physicalBits	= {};

			Core () __NE___ = default;
		};
		using Cores_t	= FixedArray< Core, MaxCores >;

		struct Processor
		{
			ECPUArch		arch				= Default;
			uint			physicalCoreCount	= 0;
			uint			logicalCoreCount	= 0;
			Cores_t			coreTypes;
		};


	// variables
	public:
		Features	feats	= {};
		Processor	cpu		= {};


	// methods
	private:
		CpuArchInfo ();

	public:
		ND_ String  Print ()			C______;
		ND_ bool	IsGLMSupported ()	C_NE___;

		ND_ static CpuArchInfo const&  Get ();
	};



	//
	// CPU Performance Info 
	//

	struct CpuPerformance
	{
		// TODO:
		//	- frequency
		//	- temperature
	};

	
	template <> struct TZeroMemAvailable< CpuArchInfo > { static constexpr bool  value = true; };

} // AE::Base
