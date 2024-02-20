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
        // read CPU info
        {
            #if defined(AE_CPU_ARCH_X64)
                cpu.arch    = ECPUArch::X64;
                feats.SSE2  = true;     // always supported
            #else
                cpu.arch    = ECPUArch::X86;
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

} // AE::Base
