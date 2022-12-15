// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/Raw/SocketService.h"
#include "../shared/UnitTest_Shared.h"

using namespace AE::Threading;
using namespace AE::Networking;


struct LocalSocketMngr
{
	LocalSocketMngr ()
	{
		TaskScheduler::CreateInstance();

		TaskScheduler::Config	cfg;
		TEST( Scheduler().Setup( cfg ));

		TEST( SocketService::Instance().Initialize() );
	}

	~LocalSocketMngr ()
	{
		Scheduler().Release();
		TaskScheduler::DestroyInstance();

		SocketService::Instance().Deinitialize();
	}

	SocketService* operator -> ()
	{
		return &SocketService::Instance();
	}
};

