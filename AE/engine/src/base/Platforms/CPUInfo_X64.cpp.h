// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Base
{
namespace
{
#ifdef AE_PLATFORM_WINDOWS
	inline void  CPUID (uint ext, OUT StaticArray<uint, 4> &arr) __NE___
	{
		__cpuid( OUT Cast<int>(arr.data()), ext );
	}

	inline void  CPUIDExt (uint ext, uint ext2, OUT StaticArray<uint, 4> &arr) __NE___
	{
		__cpuidex( OUT Cast<int>(arr.data()), ext, ext2 );
	}

#else
	inline void  CPUID (uint ext, OUT StaticArray<uint, 4> &arr) __NE___
	{
		uint*	a = &arr[0];	// eax
		uint*	b = &arr[1];	// ebx
		uint*	c = &arr[2];	// ecx
		uint*	d = &arr[3];	// edx
		__get_cpuid( ext, a, b, c, d );
	}

	inline void  CPUIDExt (uint ext, uint ext2, OUT StaticArray<uint, 4> &arr) __NE___
	{
		uint*	a = &arr[0];	// eax
		uint*	b = &arr[1];	// ebx
		uint*	c = &arr[2];	// ecx
		uint*	d = &arr[3];	// edx
		__cpuid_count( ext, ext2, a, b, c, d );
	}
#endif


/*
=================================================
	_NameToVendor
=================================================
*/
	ND_ static ECPUVendor  _NameToVendor (StringView name) __NE___
	{
		if ( HasSubString( name, "AMD" ))		return ECPUVendor::AMD;
		if ( HasSubString( name, "ARM" ))		return ECPUVendor::ARM;
		if ( HasSubString( name, "Apple" ))		return ECPUVendor::Apple;
		if ( HasSubString( name, "Intel" ))		return ECPUVendor::Intel;

		return Default;
	}

/*
=================================================
	_ManufacturerIDToVendor
----
	https://en.wikipedia.org/wiki/CPUID#EAX=0:_Highest_Function_Parameter_and_Manufacturer_ID
=================================================
*/
	ND_ static ECPUVendor  _ManufacturerIDToVendor (StringView name) __NE___
	{
		if ( name == "AuthenticAMD" )	return ECPUVendor::AMD;
		if ( name == "GenuineIntel" )	return ECPUVendor::Intel;

		if ( name == "MicrosoftXTA" )	return ECPUVendor::Virtual;		// Microsoft x86-to-ARM
		if ( name == "GenuineIntel" or
			 name == "VirtualApple" )	return ECPUVendor::Virtual;		// Apple Rosetta 2

		return Default;
	}

/*
=================================================
	_DetectMicroArch_AMD
----
	https://en.wikichip.org/wiki/amd/cpuid
=================================================
*/
	ND_ static ECPUMicroArch  _DetectMicroArch_AMD (const uint family, const uint model)
	{
		const int	ext_model = model >> 4;

		switch ( family )
		{
			case 0x10 :
				switch ( model )
				{
					case 0x6 :	// II X2
					case 0x5 :	// II X2/X3/X4
						return ECPUMicroArch::AMD_Athlon2;

					case 0xA :	// II X6
					case 0x4 :	// II X2/X3/X4
						return ECPUMicroArch::AMD_Phenom2;
				}
				break;

			case 0x11 :
				return ECPUMicroArch::AMD_Turion2;	// TODO: mismatch between wikipedia and wikichip

			case 0x12 :		return ECPUMicroArch::AMD_Llano;
			case 0x14 :		return ECPUMicroArch::AMD_Bobcat;

			case 0x15 :
				if ( AnyEqual( ext_model, 0x6, 0x7 ))
					return ECPUMicroArch::AMD_Excavator;

				if ( ext_model == 0x3 )
					return ECPUMicroArch::AMD_Steamroller;

				if ( model == 0x01 )
					return ECPUMicroArch::AMD_Bulldozer;

				if ( AnyEqual( ext_model, 0x0, 0x1 ))
					return ECPUMicroArch::AMD_Piledriver;
				break;

			case 0x16 :
				if ( ext_model == 0x3 )
					return ECPUMicroArch::AMD_Puma;

				if ( ext_model == 0 )
					return ECPUMicroArch::AMD_Jaguar;
				break;

			case 0x17 :
				switch ( model )
				{
					case 0x08 :
					//case 0x18 :
						return ECPUMicroArch::AMD_Zen1Plus;

					case 0x20 :
					case 0x18 :
					case 0x11 :
					case 0x01 :
						return ECPUMicroArch::AMD_Zen1;

					default :
						return ECPUMicroArch::AMD_Zen2;
				}
				break;

			case 0x18 :
				return ECPUMicroArch::AMD_Zen1;

			case 0x19 :
				switch ( ext_model )
				{
					case 0 :
					case 0x2 :
					case 0x4 :
					case 0x5 :
						return ECPUMicroArch::AMD_Zen3;
					case 0x1 :
					case 0x6 :
					case 0x7 :
						return ECPUMicroArch::AMD_Zen4;
				}
				break;

			case 0x1A :
				return ECPUMicroArch::AMD_Zen5;
		}
		return Default;
	}

/*
=================================================
	_DetectMicroArch_Intel
----
	https://en.wikichip.org/wiki/intel/cpuid
=================================================
*/
	ND_ static ECPUMicroArch  _DetectMicroArch_Intel (const uint family, const uint model)
	{
		if ( family == 0x6 )
		{
			switch ( model )
			{
			// client
				case 0xB7 : // S
				case 0xBA : // P
					return ECPUMicroArch::Intel_ReptorLake;

				case 0x97 : // S
				case 0x9A : // P
					return ECPUMicroArch::Intel_AlderLake;

				case 0xA7 : // S
					return ECPUMicroArch::Intel_RocketLake;

				case 0x8D : // H
				case 0x8C : // U
					return ECPUMicroArch::Intel_TigerLake;

				case 0x7E : // U, Y
					return ECPUMicroArch::Intel_IceLake;

				case 0xA5 : // Comet Lake S,H
				case 0x8E : // Comet Lake U, Amber Lake Y, Whiskey Lake U, Coffee Lake U, Kaby Lake U
					return ECPUMicroArch::Intel_CometLake;

				case 0x66 :
					return ECPUMicroArch::Intel_CannonLake;

				case 0x9E : // S,H,E
				//case 0x8E : // U
					return ECPUMicroArch::Intel_CoffeeLake;

				//case 0x9E :
				//case 0x8E : // Y,U
				//	return ECPUMicroArch::Intel_KabyLake;

				case 0x5E :
				case 0x4E : // Y,U
					return ECPUMicroArch::Intel_Skylake;

				case 0x47 : // C,W,H
				case 0x3D : // U,Y,S
					return ECPUMicroArch::Intel_Broadwell;

				case 0x46 :
				case 0x45 :
				case 0x3C :
					return ECPUMicroArch::Intel_Haswell;

				case 0x3A :	return ECPUMicroArch::Intel_IvyBridge;
				case 0x2A :	return ECPUMicroArch::Intel_SandyBridge;
				case 0x25 :	return ECPUMicroArch::Intel_Westmere;
				case 0x1E :	return ECPUMicroArch::Intel_Nehelem;


			// server
				case 0xCF :	return ECPUMicroArch::Intel_EmeraldRapids;
				case 0x8F :	return ECPUMicroArch::Intel_SapphireRapids;
				case 0x6C :	return ECPUMicroArch::Intel_IceLake_Server;
				case 0x55 :	return ECPUMicroArch::Intel_Skylake_Server;
				case 0x4F :	return ECPUMicroArch::Intel_Broadwell_Server;
				case 0x3F :	return ECPUMicroArch::Intel_Haswell_Server;
				case 0x3E :	return ECPUMicroArch::Intel_IvyBridge_Server;
				case 0x2D :	return ECPUMicroArch::Intel_SandyBridge_Server;


			// client small cores (low power SoCs)
				case 0x9C :
				case 0x96 :
				case 0x8A :
					return ECPUMicroArch::Intel_Tremont;

				case 0x7A :	return ECPUMicroArch::Intel_GoldmontPlus;

				case 0x5F :
				case 0x5C :
					return ECPUMicroArch::Intel_Goldmont;

				case 0x4C :	return ECPUMicroArch::Intel_Airmont;

				case 0x5D :
				case 0x5A :
				case 0x4D :
				case 0x4A :
				case 0x37 :
					return ECPUMicroArch::Intel_Silvermont;

				case 0x36 :
				case 0x35 :
				case 0x27 :
					return ECPUMicroArch::Intel_Saltwell;

				case 0x26 :
				case 0x1C :
					return ECPUMicroArch::Intel_Bonnell;
			}
		}
		return Default;
	}

/*
=================================================
	_DetectMicroArch
=================================================
*/
	ND_ static ECPUMicroArch  _DetectMicroArch (ECPUVendor vendor, uint family, uint model)
	{
		switch ( vendor )
		{
			case ECPUVendor::AMD :		return _DetectMicroArch_AMD( family, model );
			case ECPUVendor::Intel :	return _DetectMicroArch_Intel( family, model );
		}
		return Default;
	}

/*
=================================================
	_DetectAMDCacheHierarchy
=================================================
*/
	static void  _DetectAMDCacheHierarchy (OUT CpuArchInfo::CacheInfoMap_t &cacheInfo)
	{
		enum class AMDCacheType
		{
			Unknown		= 0,
			Data		= 1,
			Instruction	= 2,
			Unified		= 3,
			_Count
		};
		using ECacheType	= CpuArchInfo::ECacheType;
		using CacheKey_t	= CpuArchInfo::CacheKey_t;

		StaticArray<uint, 4>	cpui = {};

		CPUID( 0x8000'0001, OUT cpui );

		// has AMD topology extensions
		if ( HasBit< 23 >( cpui[2] ))
		{

			for (uint i = 0;; ++i)
			{
				CPUIDExt( 0x8000'001D, i, OUT cpui );	// cache info for Zen

				auto	type			= AMDCacheType( cpui[0] & 0x1F );
				uint	level			= (cpui[0] >> 5) & 0x7;
				uint	cores			= ((cpui[0] >> 14) & 0xFFF) + 1;
				uint	sets			= cpui[2] + 1;
				uint	line_size		= (cpui[1] & 0xFFF) + 1;
				uint	associativity	= (cpui[1] >> 22) + 1;
				uint	parts			= ((cpui[1] >> 12) & 0x3FF) + 1;
				uint	size			= associativity * parts * line_size * sets;

				if ( type == Default )
					break;

				if ( type >= AMDCacheType::_Count or level > 3 )
				{
					AE_LOGW_DBG( "skip unknown cache type" );
					continue;
				}

				ECacheType	cache_type = Default;
				switch ( level ) {
					case 1 :	cache_type = (type == AMDCacheType::Instruction ? ECacheType::L1_Instuction : ECacheType::L1_Data);  break;
					case 2 :
					case 3 :	cache_type = (level == 2 ? ECacheType::L2 : ECacheType::L3);  break;
				}

				if ( cache_type != Default )
				{
					auto&	c = cacheInfo( CacheKey_t{ cache_type, ECoreType::Unknown });
					c.lineSize			= line_size;
					c.associativity		= associativity;
					c.logicalCoreCount	= cores;
					c.size				= Bytes32u{size};
				}
			}
		}

		//CPUIDExt( 0x8000'001E, i, OUT cpui );		// cpu topology for Zen
	}

/*
=================================================
	_DetectIntelCacheHierarchy
=================================================
*/
	static void  _DetectIntelCacheHierarchy (OUT CpuArchInfo::CacheInfoMap_t &cacheInfo)
	{
		enum class IntelCacheType
		{
			Unknown		= 0,
			Data		= 1,
			Instruction	= 2,
			Unified		= 3,
			_Count
		};
		using ECacheType	= CpuArchInfo::ECacheType;
		using CacheKey_t	= CpuArchInfo::CacheKey_t;

		StaticArray<uint, 4>	cpui = {};

		for (uint i = 0;; ++i)
		{
			CPUIDExt( 0x4, i, OUT cpui );

			auto	type			= IntelCacheType(cpui[0] & 0x1F);
			uint	level			= (cpui[0] >> 5) & 0x7;
		//	uint	max_cores		= (cpui[0] >> 26) + 1;
			uint	cores			= ((cpui[0] >> 14) & 0xFFF) + 1;
			uint	line_size		= (cpui[1] & 0xFFF) + 1;
			uint	parts			= ((cpui[1] >> 12) & 0x3FF) + 1;
			uint	associativity	= (cpui[1] >> 22) + 1;
			uint	sets			= cpui[2] + 1;
		//	bool	inclusive		= HasBit< 1 >( cpui[3] );
		//	bool	complex_idx		= HasBit< 2 >( cpui[3] );
			uint	size			= associativity * parts * line_size * sets;

			if ( type == Default )
				break;

			if ( type >= IntelCacheType::_Count or level > 3 )
			{
				AE_LOGW_DBG( "skip unknown cache type" );
				continue;
			}

			ECacheType	cache_type = Default;
			switch ( level ) {
				case 1 :	cache_type = (type == IntelCacheType::Instruction ? ECacheType::L1_Instuction : ECacheType::L1_Data);  break;
				case 2 :
				case 3 :	cache_type = (level == 2 ? ECacheType::L2 : ECacheType::L3);  break;
			}

			if ( cache_type != Default )
			{
				auto&	c = cacheInfo( CacheKey_t{ cache_type, ECoreType::Unknown });
				c.lineSize			= line_size;
				c.associativity		= associativity;
				c.logicalCoreCount	= cores;
				c.size				= Bytes32u{size};
			}
		}
	}

/*
=================================================
	ReadX64CPUFeatures
=================================================
*/
	static void  ReadX64CPUFeatures (OUT CpuArchInfo::Features &feats, OUT ECPUMicroArch &uarch,
									 OUT ECPUVendor &vendor, OUT CpuArchInfo::CPUName_t &cpuName, OUT CpuArchInfo::CacheInfoMap_t &cacheInfo) __NE___
	{
		#ifdef AE_CPU_ARCH_X64
			feats.SSE2	= true;		// always supported
		#endif

		StaticArray<uint, 4>	cpui = {};

		CPUID( 0, OUT cpui );
		const int count = cpui[0];

		// get CPU vendor string
		{
			char	vendor_name [64] = {};
			std::memcpy( OUT &vendor_name[0], &cpui[1], sizeof(cpui[1]) );
			std::memcpy( OUT &vendor_name[4], &cpui[3], sizeof(cpui[3]) );
			std::memcpy( OUT &vendor_name[8], &cpui[2], sizeof(cpui[2]) );

			vendor = _ManufacturerIDToVendor( vendor_name );
		}

		CPUID( 0x8000'0000, OUT cpui );
		const uint ex_count = cpui[0];

		if ( count >= 0x1 )
		{
			CPUID( 0x1, OUT cpui );

			feats.SSE2		= HasBit< 26 >( cpui[3] );
			feats.SSE3		= HasBit<  0 >( cpui[2] );
			feats.SSSE3		= HasBit<  9 >( cpui[2] );
			feats.POPCNT	= HasBit< 23 >( cpui[2] );
			feats.AES		= HasBit< 25 >( cpui[2] );
			feats.SSE41		= HasBit< 19 >( cpui[2] );
			feats.SSE42		= HasBit< 20 >( cpui[2] );
			feats.AVX		= HasBit< 28 >( cpui[2] );
			feats.FP16C		= HasBit< 29 >( cpui[2] );

			feats.FMA		= HasBit< 12 >( cpui[2] );
			feats.CmpXchg16 = HasBit< 13 >( cpui[2] );

			// see 'CPUID EAX=1: Processor Family IDs' https://en.wikipedia.org/wiki/CPUID#EAX=1:_Processor_Info_and_Feature_Bits
			uint	family	= (cpui[0] >> 8) & 0xF;
					family	+= (cpui[0] >> 20) & 0xFF;

			uint	model	= (cpui[0] >> 4) & 0xF;
					model	|= (cpui[0] >> 12) & 0xF0;

			uarch = _DetectMicroArch( vendor, family, model );
		}

		if ( count >= 0x7 )
		{
			CPUID( 0x7, OUT cpui );

			feats.AVX2				= HasBit<  5 >( cpui[1] );
			feats.AVX512F			= HasBit< 16 >( cpui[1] );
		//	feats.AVX512_DQ			= HasBit< 17 >( cpui[1] );
		//	feats.AVX512_IFMA		= HasBit< 21 >( cpui[1] );
		//	feats.AVX512_ER			= HasBit< 27 >( cpui[1] );
		//	feats.AVX512_BW			= HasBit< 30 >( cpui[1] );
		//	feats.AVX512_VL			= HasBit< 31 >( cpui[1] );

		//	feats.AVX512_VBMI		= HasBit<  1 >( cpui[2] );
		//	feats.AVX512_VBMI2		= HasBit<  6 >( cpui[2] );
			feats.VAES				= HasBit<  9 >( cpui[2] );
		//	feats.AVX512_VNNI		= HasBit< 11 >( cpui[2] );
		//	feats.AVX512_BITALG		= HasBit< 12 >( cpui[2] );
		//	feats.AVX512_VPOPCNTDQ	= HasBit< 14 >( cpui[2] );
			feats.AESKL				= HasBit< 23 >( cpui[2] );

		//	feats.AVX512_4FMAPS		= HasBit<  2 >( cpui[3] );
			feats.AVX512_FP16		= HasBit< 23 >( cpui[3] );

			feats.SHA2_256			= HasBit< 29 >( cpui[1] );

			// ECX=1
			CPUIDExt( 0x7, 0x1, OUT cpui );

			feats.SHA512			= HasBit< 0 >( cpui[0] );
		//	feats.AVX512_BF16		= HasBit< 5 >( cpui[0] );
		}

	//	if ( ex_count >= 0x8000'0001 and vendor == ECPUVendor::AMD )
	//	{
	//		CPUID( 0x8000'0001, OUT cpui );
	//
	//		feats.SSE4A				= HasBit< 6 >( cpui[2] );
	//	}

		if ( ex_count >= 0x8000'001E and vendor == ECPUVendor::AMD )
		{
			_DetectAMDCacheHierarchy( OUT cacheInfo );
		}
		else
		if ( count >= 0x4 and vendor == ECPUVendor::Intel )
		{
			_DetectIntelCacheHierarchy( OUT cacheInfo );
		}

		// get CPU brand name
		if ( ex_count >= 0x8000'0002 )
		{
			char	cpu_name [64] = {};

			CPUID( 0x8000'0002, OUT cpui );
			std::memcpy( OUT cpu_name, cpui.data(), sizeof(cpui) );

			CPUID( 0x8000'0003, OUT cpui );
			std::memcpy( OUT cpu_name + sizeof(cpui), cpui.data(), sizeof(cpui) );

			CPUID( 0x8000'0004, OUT cpui );
			std::memcpy( OUT cpu_name + sizeof(cpui)*2, cpui.data(), sizeof(cpui) );

			for (usize i = CountOf(cpu_name)-1; i > 0; --i)
			{
				const char	c = cpu_name[i];
				if ( (c == '\0') or (c == ' ') )
					cpu_name[i] = '\0';
				else
					break;
			}

			if ( vendor == Default )
				vendor = _NameToVendor( StringView{cpu_name} );

			cpuName = cpu_name;
		}
	}

/*
=================================================
	ReadX64CPUClock
=================================================
*/
	static void  ReadX64CPUClock (INOUT CpuArchInfo::Cores_t &coreTypes) __NE___
	{
		StaticArray<uint, 4>	cpui = {};

		CPUID( 0, OUT cpui );
		const uint	count = cpui[0];

		if ( count >= 0x16 )
		{
			CPUID( 0x16, OUT cpui );

			for (auto& core : coreTypes)
			{
				core.baseClock	= cpui[0];
				core.maxClock	= cpui[1];
			}
		}
	}

} // namespace
} // AE::Base
