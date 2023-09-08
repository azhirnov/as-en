// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#if defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)
# include <fstream>
# include <sys/auxv.h>
#endif

#include "base/Platforms/CPUInfo.h"
#include "base/Math/BitMath.h"
#include "base/Memory/MemUtils.h"
#include "base/Algorithms/StringUtils.h"

namespace AE::Base
{

#if (defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)) and (defined(AE_CPU_ARCH_X86) or defined(AE_CPU_ARCH_X64))
/*
=================================================
    constructor
=================================================
*/
    CpuArchInfo::CpuArchInfo ()
    {
        feats.SSE2  = true;     // always supported

        // read CPU info
        {
            #if defined(AE_CPU_ARCH_X64)
                cpu.arch = ECPUArch::X64;
            #else
                cpu.arch = ECPUArch::X86;
            #endif
        }

        for (auto& core : cpu.coreTypes)
        {
            cpu.physicalCoreCount   += core.PhysicalCount();
            cpu.logicalCoreCount    += core.LogicalCount();
        }

        // CPU cache info
        {
            cache.L1_Inst.lineSize      = ::getauxval( AT_L1I_CACHEGEOMETRY ) & 0xFFFF;
            cache.L1_Inst.associativity = ::getauxval( AT_L1I_CACHEGEOMETRY ) >> 16;
            cache.L1_Inst.size          = Bytes32u{uint(::getauxval( AT_L1I_CACHESIZE ))};

            cache.L1_Data.lineSize      = ::getauxval( AT_L1D_CACHEGEOMETRY ) & 0xFFFF;
            cache.L1_Data.associativity = ::getauxval( AT_L1D_CACHEGEOMETRY ) >> 16;
            cache.L1_Data.size          = Bytes32u{uint(::getauxval( AT_L1D_CACHESIZE ))};

            cache.L2.lineSize           = ::getauxval( AT_L2_CACHEGEOMETRY ) & 0xFFFF;
            cache.L2.associativity      = ::getauxval( AT_L2_CACHEGEOMETRY ) >> 16;
            cache.L2.size               = Bytes32u{uint(::getauxval( AT_L2_CACHESIZE ))};

            cache.L3.lineSize           = ::getauxval( AT_L3_CACHEGEOMETRY ) & 0xFFFF;
            cache.L3.associativity      = ::getauxval( AT_L3_CACHEGEOMETRY ) >> 16;
            cache.L3.size               = Bytes32u{uint(::getauxval( AT_L3_CACHESIZE ))};
        }

        _Validate();
    }
#endif // (LINUX or ANDROID) and (X86 or X64)
//-----------------------------------------------------------------------------



#if defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)
/*
=================================================
    GetFrequency
=================================================
*/
    CpuPerformance::MHz_t  CpuPerformance::GetFrequency (uint core) __NE___
    {
        std::ifstream   stream {"/sys/devices/system/cpu/cpu"s << ToString(core) << "/cpufreq/scaling_cur_freq"};
        if ( stream )
        {
            String  line;
            std::getline( stream, OUT line );
            stream.close();
            return StringToUInt( line ) / 1000;
        }
        return 0;
    }

/*
=================================================
    GetFrequency
=================================================
*/
    uint  CpuPerformance::GetFrequency (OUT MHz_t* result, const uint maxCount) __NE___
    {
        String      line;
        const auto  GetCurrentClockSpeed = [&line](uint id) -> MHz_t
        {{
            std::ifstream   stream {"/sys/devices/system/cpu/cpu"s << ToString(id) << "/cpufreq/scaling_cur_freq"};
            if ( stream ) {
                std::getline( stream, OUT line );
                stream.close();
                return StringToUInt( line ) / 1000;
            }
            return 0;
        }};

        const uint  core_count = Min( CpuArchInfo::Get().cpu.logicalCoreCount, maxCount );

        for (uint i = 0; i < core_count; ++i) {
            result[i] = GetCurrentClockSpeed( i );
        }
        return core_count;
    }

/*
=================================================
    GetUsage
=================================================
*/
    uint  CpuPerformance::GetUsage (OUT float* user, OUT float* kernel, const uint maxCount) __NE___
    {
        ASSERT( user != null );
        ASSERT( kernel != null );

        const auto&     info        = CpuArchInfo::Get();
        const uint      core_count  = Min( info.cpu.logicalCoreCount, maxCount );

        for (auto& core : info.cpu.coreTypes)
        {
            MHz_t   freq    = GetFrequency( core.FirstLogicalCore() );
            float   usage   = Max( float(freq - core.baseClock) / float(core.maxClock - core.baseClock), 0.f );

            for (ulong bits = core.logicalBits.to_ullong(); bits != 0;)
            {
                uint    core_id = ExtractBitLog2( INOUT bits );
                if ( core_id < core_count )
                {
                    user[core_id]   = usage;
                    kernel[core_id] = 0.f;
                }
            }
        }
        return core_count;
    }


#endif // (LINUX or ANDROID)

} // AE::Base
