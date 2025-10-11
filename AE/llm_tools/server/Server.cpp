// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "pch/LangModel.h"
#include "lang_model/Remote/Server.h"

using namespace AE;
using namespace AE::Base;

static constexpr ushort		c_ServerPort = 3000;


int main (const int argc, char* argv[])
{
	ushort	port = c_ServerPort;
	if ( argc >= 3 )
	{
		if ( argv[1] == "-port"sv )
			port = ushort(StringToInt( argv[2] ));
	}

	StaticLogger::LoggerScope	log{};
	StaticLogger::AddLogger( ILogger::CreateConsoleOutput() );
	//StaticLogger::AddLogger( ILogger::CreateDialogOutput() );
	
	CHECK_ERR( Networking::SocketService::Instance().Initialize(), -1 );

	int ret;
	{
		LangModel::LangModelServer		server;
		StaticLogger::AddLogger( server.CreateLogOutput() );

		ret = server.Run( port ) ? 0 : -1;
	}

	Networking::SocketService::Instance().Deinitialize();
	return ret;
}
