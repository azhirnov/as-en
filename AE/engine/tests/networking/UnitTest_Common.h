// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/Networking.h"
#include "../shared/UnitTest_Shared.h"

using namespace AE::Threading;
using namespace AE::Networking;


struct LocalSocketMngr
{
	LocalSocketMngr ()
	{
		TaskScheduler::InstanceCtor::Create();

		TaskScheduler::Config	cfg;
		cfg.mainThreadCoreId	= ECpuCoreId(0);
		TEST( Scheduler().Setup( cfg ));

		TEST( SocketService::Instance().Initialize() );
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

