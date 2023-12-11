// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/VFS.h"
#include "../shared/UnitTest_Shared.h"

using namespace AE::Threading;
using namespace AE::VFS;

static constexpr seconds    c_MaxTimeout {100};


struct LocalVFS
{
    LocalVFS ()
    {
        TaskScheduler::Config   cfg;
        cfg.maxIOThreads        = 1;
        cfg.mainThreadCoreId    = ECpuCoreId(0);

        TaskScheduler::InstanceCtor::Create();
        TEST( Scheduler().Setup( cfg ));

        TEST( Networking::SocketService::Instance().Initialize() );

        VirtualFileSystem::InstanceCtor::Create();
    }

    ~LocalVFS ()
    {
        Scheduler().Release();
        Networking::SocketService::Instance().Deinitialize();
        VirtualFileSystem::InstanceCtor::Destroy();
        TaskScheduler::InstanceCtor::Destroy();
    }
};

