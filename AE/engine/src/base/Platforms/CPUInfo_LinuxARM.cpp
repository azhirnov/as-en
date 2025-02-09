// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#if (defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)) and defined(AE_CPU_ARCH_ARM_BASED)
# include <fstream>
# include <sys/auxv.h>
# include <asm/hwcap.h>

# include "base/Platforms/CPUInfo.h"
# include "base/Math/BitMath.h"
# include "base/Memory/MemUtils.h"
# include "base/Algorithms/ToString.h"
# include "base/Containers/FixedSet.h"

namespace AE::Base
{
namespace
{
	ND_ static uint  ReadUint10 (StringView line)
	{
		return StringToUInt( line.substr( line.find(": ")+2, line.length() ), 10 );
	}

	ND_ static uint  ReadUint16 (StringView line)
	{
		return StringToUInt( line.substr( line.find(": ")+4, line.length() ), 16 );
	}

	ND_ static ECPUVendor  CPUImplToVendor (uint id)
	{
		switch ( id ) {
			case 0x41 :	return ECPUVendor::ARM;
			case 0x42 :	return ECPUVendor::Broadcom;
			case 0x43 : return ECPUVendor::Cavium;
			case 0x46 : return ECPUVendor::Fujitsu;
			case 0x48 : return ECPUVendor::HiSilicon;
			case 0x4e : return ECPUVendor::NVidia;
			case 0x50 : return ECPUVendor::AppliedMicro;
			case 0x51 :	return ECPUVendor::Qualcomm;
			case 0x53 : return ECPUVendor::Samsung;
			case 0x56 : return ECPUVendor::Marvell;
			case 0x61 : return ECPUVendor::Apple;
			case 0x68 : return ECPUVendor::HuaxintongSemiconductor;
			case 0x69 : return ECPUVendor::Intel;
			case 0xC0 : return ECPUVendor::Ampere;
		}
		return Default;
	}

	ND_ static ECPUVendor  GetCpuVendor ()
	{
		String			line;
		std::ifstream	stream {"/proc/cpuinfo"};
		if ( stream ) {
			while ( std::getline( stream, OUT line )) {
				if ( StartsWith( line, "CPU implementer" )) {
					stream.close();
					return CPUImplToVendor( ReadUint16( line ));
				}
			}
			stream.close();
		}
		return Default;
	}

	ND_ static uint  GetMinClockSpeed (uint id)
	{
		String			line;
		std::ifstream	stream;

		stream.open( "/sys/devices/system/cpu/cpu"s << Base::ToString(id) << "/cpufreq/cpuinfo_min_freq" );
		if ( not stream ) {
			stream.open( "/sys/devices/system/cpu/cpu"s << Base::ToString(id) << "/cpufreq/scaling_min_freq" );
		}
		if ( stream ) {
			std::getline( stream, OUT line );
			stream.close();
			return StringToUInt( line ) / 1000;	// in MHz
		}
		return 0;
	}

	ND_ static uint  GetMaxClockSpeed (uint id)
	{
		String			line;
		std::ifstream	stream;

		stream.open( "/sys/devices/system/cpu/cpu"s << Base::ToString(id) << "/cpufreq/cpuinfo_max_freq" );
		if ( not stream ) {
			stream.open( "/sys/devices/system/cpu/cpu"s << Base::ToString(id) << "/cpufreq/scaling_max_freq" );
		}
		if ( stream ) {
			std::getline( stream, OUT line );
			stream.close();
			return StringToUInt( line ) / 1000;	// in MHz
		}
		return 0;
	}

	ND_ static bool  GetMinMaxClockSpeed (uint id, OUT uint &min, OUT uint &max)
	{
		String			line;
		std::ifstream	stream {"/sys/devices/system/cpu/cpu"s << Base::ToString(id) << "/cpufreq/scaling_available_frequencies"};

		if ( not stream )
			return false;

		std::getline( stream, OUT line );
		stream.close();

		// remove tailing spaces
		for (; not line.empty() and line.back() == ' ';)
			line.pop_back();

		if ( line.empty() )
			return false;

		// read first and last values
		usize	p0	= line.find( ' ' );
		usize	p1	= line.rfind( ' ' );

		if ( p0 == UMax or p1 == UMax )
			return false;

		max	= StringToUInt( SubString( line, 0, p0 )) / 1000;			    // in MHz
		min	= StringToUInt( SubString( line, p1+1, line.size() )) / 1000;	// in MHz

		if ( min > max )
			std::swap( min, max );

		return true;
	}

#ifdef AE_ENABLE_LOGS
	// from
	// https://elixir.bootlin.com/linux/latest/source/arch/arm/include/asm/cputype.h
	// https://elixir.bootlin.com/linux/latest/source/arch/arm64/include/asm/cputype.h

	ND_ static StringView  GetCoreName_ARM (uint part)
	{
		// Neoverse	N - for core datacenter usage
		// Neoverse	E - for edge computing
		// Neoverse	V - for very high performance computing
		switch ( part ) {
			case 0xc05 : return "Cortex A5";
			case 0xc07 : return "Cortex A7";
			case 0xc08 : return "Cortex A8";
			case 0xc09 : return "Cortex A9";
			case 0xc0d : return "Cortex A12";
			case 0xc0f : return "Cortex A15";
			case 0xc0e : return "Cortex A17";
			case 0xc14 : return "Cortex R4";
			case 0xc15 : return "Cortex R5";
			case 0xc17 : return "Cortex R7";
			case 0xc18 : return "Cortex R8";
			case 0xc20 : return "Cortex M0";
			case 0xc21 : return "Cortex M1";
			case 0xc23 : return "Cortex M3";
			case 0xc24 : return "Cortex M4";
			case 0xc27 : return "Cortex M7";
			case 0xd01 : return "Cortex A32";
			case 0xd02 : return "Cortex A34";
			case 0xd03 : return "Cortex A53";
			case 0xd04 : return "Cortex A35";
			case 0xd05 : return "Cortex A55";
			case 0xd06 : return "Cortex A65";
			case 0xd07 : return "Cortex A57";
			case 0xd08 : return "Cortex A72";
			case 0xd09 : return "Cortex A73";
			case 0xd0a : return "Cortex A75";
			case 0xd0b : return "Cortex A76";
			case 0xd0c : return "Neoverse N1";
			case 0xd0d : return "Cortex A77";
			case 0xd0e : return "Cortex A76AE";
			case 0xd13 : return "Cortex R52";
			case 0xd20 : return "Cortex M23";
			case 0xd21 : return "Cortex M33";
			case 0xd22 : return "Cortex M55";
			case 0xd40 : return "Neoverse V1";
			case 0xd41 : return "Cortex A78";
			case 0xd43 : return "Cortex A65AE";
			case 0xd44 : return "Cortex X1";
			case 0xd49 : return "Neoverse N2";
			case 0xd4a : return "Neoverse E1";
			case 0xD46 : return "Cortex A510";
			case 0xD47 : return "Cortex A710";
			case 0xD4D : return "Cortex A715";
			case 0xD48 : return "Cortex X2";
			case 0xD4B : return "Cortex A78C";
		}
		return Default;
	}

	ND_ static StringView  GetCoreName_Qualcomm (uint part)
	{
		switch ( part ) {
			case 0xC00 : return "Falkor";
			case 0x200 : return "Kryo";
			case 0x800 : return "Kryo 2xx Gold";
			case 0x801 : return "Kryo 2xx Silver";
			case 0x803 : return "Kryo 3xx Silver";
			case 0x804 : return "Kryo 4xx Gold";
			case 0x805 : return "Kryo 4xx Silver";
		}
		return Default;
	}

	ND_ static StringView  GetCoreName_NVidia (uint part)
	{
		switch ( part ) {
			case 0x003 : return "Denver";
			case 0x004 : return "Carmel";
		}
		return Default;
	}

	ND_ static StringView  GetCoreName_Apple (uint part)
	{
		// https://github.com/AsahiLinux/m1n1/blob/main/src/chickens.c
		switch ( part ) {
			case 0x001 : return "A7 Cyclone";
			case 0x002 : return "A8 Typhoon";
			case 0x003 : return "A8 Typhoo/Capri";
			case 0x004 : return "A9 Twister";
			case 0x005 : return "A9 Twister/Elba/Malta";
			case 0x006 : return "A10 Hurricane";
			case 0x007 : return "A10 Hurricane/Myst";
			case 0x008 : return "A11 Monsoon";
			case 0x009 : return "A11 Mistral";
			case 0x00B : return "A12 Vortex";
			case 0x00C : return "A12 Tempest";
			case 0x010 : return "A12X Vortex Aruba";
			case 0x011 : return "A12X Tempest Aruba";
			case 0x00F : return "S4 Tempest M9";
			case 0x012 : return "A13 Lightning";
			case 0x013 : return "A13 Thunder";
			case 0x020 : return "A14 Icestorm";
			case 0x021 : return "A14 Firestorm";

			case 0x022 : return "M1 Icestorm";
			case 0x023 : return "M1 Firestorm";
			case 0x024 : return "M1 Pro Icestorm";
			case 0x025 : return "M1 Pro Firestorm";
			case 0x028 : return "M1 Max Icestorm";
			case 0x029 : return "M1 Max Firestorm";

			case 0x032 : return "M2 Blizzard";
			case 0x033 : return "M2 Avalanche";
			case 0x034 : return "M2 Pro Blizzard";
			case 0x035 : return "M2 Pro Avalanche";
			case 0x038 : return "M2 Max Blizzard";
			case 0x039 : return "M2 Max Avalanche";

			case 0x048 : return "M3 Max Sawtooth";
			case 0x049 : return "M3 Max Everest";
		}
		if ( part >= 0x30 and part < 0x40 )	return "M2";
		if ( part >= 0x40 and part < 0x50 )	return "M3";

		return Default;
	}

	ND_ static StringView  GetCoreName_Broadcom (uint part)
	{
		switch ( part ) {
			case 0x100 : return "Brahma B53";
			case 0x516 : return "Vulcan";
		}
		return Default;
	}

	ND_ static StringView  GetCoreName_Fujitsu (uint part)
	{
		switch ( part ) {
			case 0x001 : return "A64FX";
		}
		return Default;
	}

	ND_ static StringView  GetCoreName_Samsung (uint part)
	{
		switch ( part ) {
			case 0x2 : return "Exynos M3";
			case 0x3 : return "Exynos M4";
			case 0x4 : return "Exynos M5";
		}
		return Default;
	}

	ND_ static StringView  GetCoreName (ECPUVendor vendor, uint part)
	{
		// from https://github.com/JuliaLang/julia/blob/master/src/processor_arm.cpp
		switch ( vendor ) {
			case ECPUVendor::ARM :				return GetCoreName_ARM( part );
			case ECPUVendor::Qualcomm :			return GetCoreName_Qualcomm( part );
			case ECPUVendor::NVidia :			return GetCoreName_NVidia( part );
			case ECPUVendor::Apple :			return GetCoreName_Apple( part );
			case ECPUVendor::Broadcom :			return GetCoreName_Broadcom( part );
			case ECPUVendor::Fujitsu :			return GetCoreName_Fujitsu( part );
			case ECPUVendor::Samsung :			return GetCoreName_Samsung( part );
			case ECPUVendor::Cavium :
			case ECPUVendor::HiSilicon :
			case ECPUVendor::AppliedMicro :
			case ECPUVendor::Marvell :
			case ECPUVendor::HuaxintongSemiconductor :
			case ECPUVendor::Intel :			break;
		}
		return Default;
	}

#else

	ND_ static StringView  GetCoreName (ECPUVendor vendor, uint part)
	{
		return Default;
	}

#endif // AE_ENABLE_LOGS

/*
	Helper:

		idiva		- DIV instructions available in ARM mode.
		idivt		- DIV instructions available in Thumb mode.
		vfpd32		- VFP (of any version) with 32 double-precision registers d0-d31.
		lpae		- Large Physical Address Extension (physical address up to 40 bits).
		evtstrm		- generation of Event Stream by timer.
		pmull		- Polinomial Multiplication instructions.

	arm v7
		thumb-2
		neon		- NEON instructions (aka Advanced SIMD)
		VFPv3
		vfpv4		- fused multiply-add instructions.
		crc32		- CRC32 instructions.


	Arm v8.0
		fp			- floating point present
		asimd		- advanced SIMD present
		evtstrm		- timer event stream generation
		cpuid		- CPU features can be read

		sha1		- SHA1 instructions
		sha2		- SHA2 instructions
		aes			- AES instructions

	Arm v8.1
		asimdrdm	- Advanced SIMD rounding double multiply accumulate instructions
		atomics		- Atomic instructions

	Arm v8.2
		asimddp		- Advanced SIMD dot product instructions
		asimdfhm	- Floating-point half-precision multiplication instructions
		asimdhp		- Advanced SIMD with BFloat16 instructions
		bf16		- AArch64 BFloat16 instructions
		dcpodp		- DC CVADP instruction
		dcpop		- DC CVAP instruction- Data cache clean to Point of Persistence
		flagm		- Flag manipulation instructions v2
		fphp		- Half-precision floating-point data processing
		i8mm		- AArch64 Int8 matrix multiplication instructions
		sha3		- Advanced SIMD SHA3 instructions
		sha512		- Advanced SIMD SHA512 instructions
		sm3			- Advanced SIMD SM3 instructions
		sm4			- Advanced SIMD SM4 instructions
		sve			- Scalable Vector Extension
		svebf16		- AArch64 BFloat16 instructions (SVE)
		svef32mm	- Single-precision Matrix Multiplication (SVE)
		svef64mm	- Double-precision Matrix Multiplication (SVE)
		svei8mm		- AArch64 Int8 matrix multiplication instructions (SVE)
		uscat		- Unaligned single-copy atomicity and atomic functions with a 16-byte address range aligned to 16-bytes are supported

	Arm v8.3
		fcma		- Floating-point complex number instructions
		jscvt		- JavaScript conversion instructions
		lrcpc		- Load-Acquire RCpc instructions

	Arm v8.4
		dit			- Data Independent Timing instructions
		ilrcpc		- Load-Acquire RCpc instructions v2
		paca		- Faulting on AUT* instructions
		pacg		- Enhanced pointer authentication functionality

	Arm v8.5
		bti			- Branch Target Identification
		flagm2		- Enhancements to flag manipulation instructions
		frint		- Floating-point to integer instructions
		mte			- Memory Tagging Extension
		mte3		- MTE Asymmetric Fault Handling
		rng			- Random number generator

	Arm v8.6
		ecv			- Enhanced Counter Virtualization

	Arm v8.7
		afp			- Alternate floating-point behaviour
		rpres		- Increased precision of Reciprocal Estimate and Reciprocal Square Root Estimate
		wfxt		- WFE and WFI instructions with timeout

	Arm v9.0
		sve2		- Scalable Vector Extension version 2
		sveaes		- Scalable Vector AES instructions
		svebitperm	- Scalable Vector Bit Permutes instruction
		svepmull	- Scalable Vector PMULL instructions
		svesha3		- Scalable Vector SHA3 instructions
		svesm4		- Scalable Vector SM4 instructions

	Arm v9.2
		ebf16		- AArch64 Extended BFloat16 instructions
		sme			- Scalable Matrix Extension
		smeb16f32	- SME support for instructions that accumulate BFloat16 outer products into FP32 single-precision floating-point tiles
		smef16f32	- SME support for instructions that accumulate FP16 half-precision floating-point outer products into FP32 single-precision floating-point tiles
		smef32f32	- SME support for instructions that accumulate FP32 single-precision floating-point outer products into single-precision floating-point tiles
		smef64f64	- SME support for instructions that accumulate into FP64 double-precision floating-point elements in the ZA array
		smefa64		- Full Streaming SVE mode instructions
		smei8i32	- SME support for instructions that accumulate 8-bit integer outer products into 32-bit integer tiles
		smei16i64	- SME support for instructions that accumulate into 64-bit integer elements in the ZA array
		sveebf16	- AArch64 Extended BFloat16 instructions (SVE)
*/
} // namespace

/*
=================================================
	constructor
=================================================
*/
	CpuArchInfo::CpuArchInfo () __NE___
	{
		// read features
		{
			const auto	caps  = ::getauxval( AT_HWCAP );
			const auto	caps2 = ::getauxval( AT_HWCAP2 );

		  #ifdef AE_CPU_ARCH_ARM32
			feats.NEON		= AllBits( caps, HWCAP_NEON );
			Unused( caps2 );
		  #endif

		  #ifdef AE_CPU_ARCH_ARM64
			feats.NEON		= true;
			feats.NEON_fp16	= true;     // TODO
			feats.FP16C		= true;     // TODO

			feats.SVE		= AllBits( caps2, HWCAP_SVE );
			feats.SVE2		= AllBits( caps2, HWCAP2_SVE2 );	// Arm v9.0
			feats.SVE_AES	= AllBits( caps2, HWCAP2_SVEAES );	// Arm v9.0

			feats.CRC32		= AllBits( caps, HWCAP_CRC32 );		// Arm v7
			feats.AES		= AllBits( caps, HWCAP_AES );		// Arm v8.0
			feats.SHA2_256	= AllBits( caps, HWCAP_SHA2 );		// Arm v8.0
			feats.SHA2_512	= AllBits( caps, HWCAP_SHA512 );	// Arm v8.2
			feats.SHA3		= AllBits( caps, HWCAP_SHA3 );		// Arm v8.2
		  #endif

			cpu.vendor = GetCpuVendor();
		}

		// parse processors
		{
			struct TmpCore
			{
				uint	id			= UMax;
				uint	part		= 0;
				uint	vendor		= 0;
				uint	minClock	= 0;
				uint	maxClock	= 0;
			};
			constexpr uint	max_cores = 64;

			std::ifstream	stream {"/proc/cpuinfo"};
			if ( stream )
			{
				FixedArray< TmpCore, max_cores >	cores;
				String								line;

				while ( std::getline( stream, OUT line ))
				{
					if ( StartsWith( line, "processor" ))
					{
						if_unlikely( cores.IsFull() )
							break;  // overflow

						auto&	dst = cores.emplace_back();
						dst.id = ReadUint10( line );

						if ( not GetMinMaxClockSpeed( dst.id, OUT dst.minClock, OUT dst.maxClock )) {
							dst.minClock = GetMinClockSpeed( dst.id );
							dst.maxClock = GetMaxClockSpeed( dst.id );
						}

					}else
					if ( not cores.empty() )
					{
						if ( StartsWith( line, "CPU part" )) {
							cores.back().part = ReadUint16( line );
						}else
						if ( StartsWith( line, "CPU implementer" )) {
							cores.back().vendor = ReadUint16( line );
						}
					}
				}

				FixedSet< ulong, max_cores >	unique_cores;
				for (auto& core : cores) {
					unique_cores.insert( ulong(core.vendor & 0xFFFF) |
										 (ulong(core.part & 0xFFFF) << 16) |
										 (ulong(core.maxClock) << 32) );
				}

				for (auto& unique : unique_cores)
				{
					const uint	vendor	= uint(unique & 0xFFFF);
					const uint	part	= uint((unique >> 16) & 0xFFFF);
					const uint	max_cl	= uint(unique >> 32);
					auto&		dst		= cpu.coreTypes.emplace_back();

					for (auto& core : cores) {
						if ( core.vendor == vendor and core.part == part and core.maxClock == max_cl ) {
							dst.logicalBits .set( core.id );
							dst.physicalBits.set( core.id );
							dst.baseClock	= core.minClock;
							dst.maxClock	= core.maxClock;
						}
					}
					dst.name = GetCoreName( CPUImplToVendor( vendor ), part );
				}

				// sort by max clock
				std::sort( cpu.coreTypes.begin(), cpu.coreTypes.end(), [](auto& lhs, auto &rhs) { return lhs.maxClock > rhs.maxClock; });

				const ECoreType		types [] = { ECoreType::HighPerformance, ECoreType::Performance, ECoreType::EnergyEfficient };
				usize				j		 = Max( 0, 3 - int(cpu.coreTypes.size()) );

				for (usize i = 0; i < cpu.coreTypes.size(); ++i, ++j) {
					cpu.coreTypes[i].type = types[ Min( j, CountOf(types)-1 )];
				}
			}
		}

		// read CPU info
		{
			#if defined(AE_CPU_ARCH_ARM32)
				cpu.arch = ECPUArch::ARM_32;
			#elif defined(AE_CPU_ARCH_ARM64)
				cpu.arch = ECPUArch::ARM_64;
			#endif
		}

		for (auto& core : cpu.coreTypes)
		{
			cpu.physicalCoreCount	+= core.PhysicalCount();
			cpu.logicalCoreCount	+= core.LogicalCount();
		}

		// CPU cache info

		// TODO: check cpuinfo

		// TODO: sysconf() with _SC_NPROCESSORS_CONF, _SC_NPROCESSORS_ONLN

		_Validate();
	}

} // AE::Base

#endif // (LINUX or ANDROID) and (ARM32 or ARM64)
