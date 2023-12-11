// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_COMPILER_MSVC) and defined(AE_PLATFORM_WINDOWS)
# include "base/Defines/StdInclude.h"

# pragma warning (push)
# pragma warning (disable: 4668)
# pragma warning (disable: 4005)
#   include <Windows.h>
#   include <winternl.h>
# pragma warning (pop)

# include "base/Platforms/WindowsUtils.h"
# include "base/Platforms/WindowsLibrary.h"

namespace AE::Base
{
namespace
{

    // https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/performance.htm
    struct SystemPerfInfo
    {
        LARGE_INTEGER IdleProcessTime;
        LARGE_INTEGER IoReadTransferCount;
        LARGE_INTEGER IoWriteTransferCount;
        LARGE_INTEGER IoOtherTransferCount;
        ULONG IoReadOperationCount;
        ULONG IoWriteOperationCount;
        ULONG IoOtherOperationCount;
        ULONG AvailablePages;
        ULONG CommittedPages;
        ULONG CommitLimit;
        ULONG PeakCommitment;
        ULONG PageFaultCount;
        ULONG CopyOnWriteCount;
        ULONG TransitionCount;
        ULONG CacheTransitionCount;
        ULONG DemandZeroCount;
        ULONG PageReadCount;
        ULONG PageReadIoCount;
        ULONG CacheReadCount;
        ULONG CacheIoCount;
        ULONG DirtyPagesWriteCount;
        ULONG DirtyWriteIoCount;
        ULONG MappedPagesWriteCount;
        ULONG MappedWriteIoCount;
        ULONG PagedPoolPages;
        ULONG NonPagedPoolPages;
        ULONG PagedPoolAllocs;
        ULONG PagedPoolFrees;
        ULONG NonPagedPoolAllocs;
        ULONG NonPagedPoolFrees;
        ULONG FreeSystemPtes;
        ULONG ResidentSystemCodePage;
        ULONG TotalSystemDriverPages;
        ULONG TotalSystemCodePages;
        ULONG NonPagedPoolLookasideHits;
        ULONG PagedPoolLookasideHits;
        ULONG AvailablePagedPoolPages;
        ULONG ResidentSystemCachePage;
        ULONG ResidentPagedPoolPage;
        ULONG ResidentSystemDriverPage;
        ULONG CcFastReadNoWait;
        ULONG CcFastReadWait;
        ULONG CcFastReadResourceMiss;
        ULONG CcFastReadNotPossible;
        ULONG CcFastMdlReadNoWait;
        ULONG CcFastMdlReadWait;
        ULONG CcFastMdlReadResourceMiss;
        ULONG CcFastMdlReadNotPossible;
        ULONG CcMapDataNoWait;
        ULONG CcMapDataWait;
        ULONG CcMapDataNoWaitMiss;
        ULONG CcMapDataWaitMiss;
        ULONG CcPinMappedDataCount;
        ULONG CcPinReadNoWait;
        ULONG CcPinReadWait;
        ULONG CcPinReadNoWaitMiss;
        ULONG CcPinReadWaitMiss;
        ULONG CcCopyReadNoWait;
        ULONG CcCopyReadWait;
        ULONG CcCopyReadNoWaitMiss;
        ULONG CcCopyReadWaitMiss;
        ULONG CcMdlReadNoWait;
        ULONG CcMdlReadWait;
        ULONG CcMdlReadNoWaitMiss;
        ULONG CcMdlReadWaitMiss;
        ULONG CcReadAheadIos;
        ULONG CcLazyWriteIos;
        ULONG CcLazyWritePages;
        ULONG CcDataFlushes;
        ULONG CcDataPages;
        ULONG ContextSwitches;
        ULONG FirstLevelTbFills;
        ULONG SecondLevelTbFills;
        ULONG SystemCalls;
        ULONGLONG CcTotalDirtyPages;
        ULONGLONG CcDirtyPageThreshold;
        LONGLONG ResidentAvailablePages;
        ULONGLONG SharedCommittedPages;
    };

    // 0x0138 bytes in versions 3.10 to 6.0;
    // 0x0148 bytes in version 6.1;
    // 0x0158 bytes in version 6.2 and higher.
    StaticAssert( sizeof(SystemPerfInfo) == 0x0158 );
//-----------------------------------------------------------------------------


    // https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/processor_performance.htm
    struct SystemProcessorPerfInfo
    {
        LONGLONG IdleTime;
        LONGLONG KernelTime;
        LONGLONG UserTime;
        LONGLONG DpcTime;
        LONGLONG InterruptTime;
        ULONG InterruptCount;
    };
    StaticAssert( sizeof(SystemProcessorPerfInfo) == 0x30 );
//-----------------------------------------------------------------------------


    struct NtdllLib
    {
    // variables
    private:
        WindowsLibrary      _lib;

    public:
        decltype(&NtQuerySystemInformation) fnQuerySystemInformation        = null;

        static constexpr uint                               MaxCores        = 64;
        StaticArray< SystemProcessorPerfInfo, MaxCores >    procPerf [2]    = {};
        uint                                                coreCount   : 9;
        uint                                                frameId     : 1;


    // methods
    public:
        NtdllLib () :
            coreCount{0}, frameId{0}
        {
            if ( _lib.Load( "Ntdll.dll" ))
            {
                if ( _lib.GetProcAddr( "NtQuerySystemInformation", OUT fnQuerySystemInformation ))
                {
                    auto&   info = CpuArchInfo::Get();
                    coreCount = info.cpu.logicalCoreCount;
                }
            }
        }
    };

    ND_ static NtdllLib&  Ntdll () __NE___
    {
        static NtdllLib lib;
        return lib;
    }
}
//-----------------------------------------------------------------------------



/*
=================================================
    GetUsage
=================================================
*/
    uint  CpuPerformance::GetUsage (OUT float* user, OUT float* kernel, const uint maxCount) __NE___
    {
        ASSERT( user != null );
        ASSERT( kernel != null );

        auto&   ntdll = Ntdll();
        if ( ntdll.fnQuerySystemInformation != null )
        {
            //SystemPerfInfo    perf = {};
            //ntdll.fnQuerySystemInformation( SystemPerformanceInformation, INOUT &perf, sizeof(perf), null );

            const auto& prev = ntdll.procPerf[ ntdll.frameId ];
            auto&       next = ntdll.procPerf[ (ntdll.frameId+1)&1 ];
            ntdll.fnQuerySystemInformation( SystemProcessorPerformanceInformation, INOUT next.data(), ULONG(ArraySizeOf(next)), null );

            for (uint i = 0, cnt = Min( ntdll.coreCount, maxCount ); i < cnt; ++i)
            {
                LONGLONG    idle_time   = next[i].IdleTime   - prev[i].IdleTime;
                LONGLONG    kernel_time = next[i].KernelTime - prev[i].KernelTime;
                LONGLONG    user_time   = next[i].UserTime   - prev[i].UserTime;
                LONGLONG    total_time  = idle_time + kernel_time + user_time;

                user[i]     = float(user_time) / float(total_time);
                kernel[i]   = float(kernel_time) / float(total_time);
            }

            ++ntdll.frameId;
            return ntdll.coreCount;
        }
        return 0;
    }

/*
=================================================
    GetFrequency
=================================================
*/
    CpuPerformance::MHz_t  CpuPerformance::GetFrequency (uint core) __NE___
    {
        // TODO
        Unused( core );
        return 0;
    }

    uint  CpuPerformance::GetFrequency (OUT MHz_t* result, const uint maxCount) __NE___
    {
        // TODO
        Unused( result, maxCount );
        return 0;
    }


} // AE::Base

#endif // AE_COMPILER_MSVC and AE_PLATFORM_WINDOWS
