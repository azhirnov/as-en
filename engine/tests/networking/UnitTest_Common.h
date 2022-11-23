// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/Raw/SocketManager.h"
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

		TEST( SocketManager::Instance().Initialize() );
	}

	~LocalSocketMngr ()
	{
		Scheduler().Release();
		TaskScheduler::DestroyInstance();

		SocketManager::Instance().Deinitialize();
	}

	SocketManager* operator -> ()
	{
		return &SocketManager::Instance();
	}
};

