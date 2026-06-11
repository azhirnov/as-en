// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/ThreadManager.h"
#include "UnitTest_Common.h"
#include "../threading/UnitTest_Common.h"

namespace
{
	static const ushort		c_Port = AE_TEST_TCP_PORT_1;


	struct ExeOrder
	{
		Mutex	guard;
		String	str;

		ExeOrder ()
		{
			EXLOCK( guard );
			str.reserve( 128 );
			str = "0";
		}
	};


	static void  SocketDep_Test1 ()
	{
		LocalSocketMngr		socket_mngr;
		TEST( SocketDependencyManager::Register() );

		auto			client_addr	= IpAddress::FromHostPortTCP( "localhost", c_Port );
		auto			server_addr	= IpAddress::FromLocalhostTCP( c_Port );
		Atomic<bool>	wake_up		{false};

		StdThread	listener{ [&] ()
			{
				TcpSocket	server;
				TEST( server.Listen( server_addr ));
				TEST( server.IsOpen() );

				TcpSocket	client;		// client socket on server side

				// wait for connection
				for (uint i = 0; i < 1'000; ++i)	// max: 100s
				{
					IpAddress	addr;
					if ( client.Accept( server, OUT addr ))
					{
						AE_LOGI( "TCP: connected to "s << addr.ToString() );
						break;
					}
					ThreadUtils::MilliSleep( milliseconds{100} );
				}

				if ( client.IsOpen() )
				{
					for (uint i = 0; i < 1'000; ++i)	// max: 100s
					{
						if ( wake_up.load() )
						{
							char	data_to_send[]	= "ABC";
							auto	[err, sent]		= client.Send( data_to_send, Sizeof(data_to_send) );

							TEST( err == SocketSendError::Sent and sent == Sizeof(data_to_send) );
							client.Close();
							break;
						}
						ThreadUtils::MilliSleep( milliseconds{100} );
					}
				}
				server.Close();
			}};

		ThreadUtils::Sleep_15ms();

		TcpSocket	client;
		TEST( client.Connect( client_addr ));
		TEST( client.IsOpen() );

		TEST( client.ConnectionStatus() == TcpSocket::EStatus::Connected );


		ExeOrder	value;	// access to value protected by internal synchronizations

		AsyncTask	task1 =	[] (ExeOrder &val, TcpSocket &client) -> AsyncCoro
							{
								TEST( not Coro_IsCanceled );

								char	buf[32];
								auto	[err, recv] = client.Receive( buf, Sizeof(buf) );

								TEST( err == SocketReceiveError::Received );
								TEST( recv == 4 );

								DeferExLock  guard {val.guard};
								TEST( guard.try_lock() );

								val.str += buf;
								co_return;
							}( value, client );

		Scheduler().Run( ETaskQueue::Background, task1, Tuple{ SocketDependency{client} });

		// now task may be executed, but waits for socket event
		Scheduler().AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{ EThreadArray{EThread::PerFrame, EThread::Background, EThread::IO}, "thread" } ));

		ThreadUtils::Sleep_15ms();
		{
			DeferExLock  guard {value.guard};
			TEST( guard.try_lock() );
			value.str += '1';
		}

		// server will send new data, client must wake up
		wake_up.store( true );

		TEST( Scheduler().Wait( List{ task1 }, c_MaxTimeout ));
		TEST( task1->Status() == ETaskStatus::Completed );

		listener.join();
		client.Close();

		DeferExLock  guard {value.guard};
		TEST( guard.try_lock() );
		TEST( value.str == "01ABC" );
	}
}


extern void UnitTest_SocketDep ()
{
	SocketDep_Test1();

	TEST_PASSED();
}
