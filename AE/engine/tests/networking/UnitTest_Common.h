// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "pch/Networking.h"
#include "../tests/shared/UnitTest_Shared.h"

using namespace AE::Threading;
using namespace AE::Networking;


struct LocalSocketMngr
{
	LocalSocketMngr ()
	{
		TaskScheduler::InstanceCtor::Create();

		TaskScheduler::Config	cfg;
		TEST( Scheduler().Setup( cfg ));

		TEST( SocketService::Instance().Initialize() );

	  #ifdef AE_PLATFORM_ANDROID
		SocketService::Callbacks	cb;
		cb.getRouterIPAddress	= [](void*, OUT IpAddress &outAddr) __NE___ { outAddr = IpAddress::FromInt(192,168,0,1, 0);  return true; };

		SocketService::Instance().SetCallbacks( cb );
	  #endif
	}

	~LocalSocketMngr ()
	{
		Scheduler().Release();
		TaskScheduler::InstanceCtor::Destroy();

		SocketService::Instance().Deinitialize();
	}

	SocketService* operator -> ()
	{
		return &SocketService::Instance();
	}
};

