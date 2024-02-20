// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/ObjC/NS.h"

#undef null
#include <mach/mach_host.h>
#include <mach/task.h>
#include <os/proc.h>
#import <Foundation/Foundation.h>
#define null    nullptr

#include "base/Platforms/AppleUtils.h"

namespace AE::Base
{
namespace
{
/*
=================================================
    GetSystemMemorySize
=================================================
*/
    ND_ static Bytes  GetSystemMemorySize ()
    {
        mach_msg_type_number_t  info_size = HOST_BASIC_INFO_COUNT;
        host_basic_info_data_t  info;

        if ( host_info( mach_host_self(), HOST_BASIC_INFO, OUT (host_info_t)&info, &info_size ) == KERN_SUCCESS )
        {
            return Bytes{ info.max_mem };
        }
        return 0_b;
    }

/*
=================================================
    GetAvailableMemorySize
=================================================
*/
    ND_ static Bytes  GetAvailableMemorySize ()
    {
        #ifdef PLATFORM_IOS
        if ( @available( ios 13.0, *))
            return os_proc_available_memory();
        #endif

        mach_port_t             host_port   = mach_host_self();
        mach_msg_type_number_t  info_size   = HOST_VM_INFO_COUNT;
        vm_size_t               pagesize;
        vm_statistics_data_t    vm_stat;

        host_page_size( host_port, OUT &pagesize );

        if ( host_statistics( host_port, HOST_VM_INFO, OUT (host_info_t)&vm_stat, &info_size ) == KERN_SUCCESS )
        {
            return Bytes{ vm_stat.free_count * pagesize };
        }
        return 0_b;
    }

/*
=================================================
    GetUsedMemorySize
=================================================
*/
    ND_ static Bytes  GetUsedMemorySize ()
    {
        task_vm_info_data_t     task_vm_info;
        mach_msg_type_number_t  task_size = TASK_VM_INFO_COUNT;

        if ( task_info( mach_task_self(), TASK_VM_INFO, OUT (task_info_t)&task_vm_info, &task_size ) == KERN_SUCCESS )
        {
            return Bytes{ task_vm_info.phys_footprint };
        }
        return 0_b;
    }

} // namespace


/*
=================================================
    GetOSVersion
=================================================
*/
    Version3  AppleUtils::GetOSVersion () __NE___
    {
        NSOperatingSystemVersion    os_ver = [ [NSProcessInfo processInfo] operatingSystemVersion ];
        return Version3{ ushort(os_ver.majorVersion), ushort(os_ver.minorVersion), uint(os_ver.patchVersion) };
    }

/*
=================================================
    GetMemoryPageInfo
=================================================
*/
    AppleUtils::MemoryPageInfo  AppleUtils::GetMemoryPageInfo () __NE___
    {
        mach_port_t     host_port   = mach_host_self();
        vm_size_t       pagesize;

        host_page_size( host_port, OUT &pagesize );

        MemoryPageInfo  result;
        result.pageSize = Bytes{pagesize};
        return result;
    }

/*
=================================================
    GetMemorySize
=================================================
*/
    AppleUtils::MemorySize  AppleUtils::GetMemorySize () __NE___
    {
        MemorySize  result;
        result.total        = GetSystemMemorySize();
        result.available    = GetAvailableMemorySize();
        result.used         = GetUsedMemorySize();
        return result;
    }

    // TODO: https://developer.apple.com/library/archive/releasenotes/Performance/RN-AffinityAPI/index.html#//apple_ref/doc/uid/TP40006635


} // AE::Base
