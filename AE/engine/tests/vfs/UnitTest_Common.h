// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "pch/VFS.h"
#include "../tests/shared/UnitTest_Shared.h"

using namespace AE::Threading;
using namespace AE::VFS;

static constexpr seconds	c_MaxTimeout {100};


struct LocalVFS
{
	LocalVFS ()
	{
		using namespace AE::Networking;

		TaskScheduler::Config	cfg;
		cfg.maxIOAccessThreads	= 1;

		TaskScheduler::InstanceCtor::Create();
		TEST( Scheduler().Setup( cfg ));

		TEST( SocketService::Instance().Initialize() );

		VirtualFileSystem::InstanceCtor::Create();

	  #ifdef AE_PLATFORM_ANDROID
		SocketService::Callbacks	cb;
		cb.getRouterIPAddress	= [](void*, OUT IpAddress &outAddr) __NE___ { outAddr = IpAddress::FromInt(192,168,0,1, 0);  return true; };

		SocketService::Instance().SetCallbacks( cb );
	  #endif
	}

	~LocalVFS ()
	{
		Scheduler().Release();
		Networking::SocketService::Instance().Deinitialize();
		VirtualFileSystem::InstanceCtor::Destroy();
		TaskScheduler::InstanceCtor::Destroy();
	}
};

