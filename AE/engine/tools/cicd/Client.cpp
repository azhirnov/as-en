// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef CICD_CLIENT
# include "cicd/Client.h"

namespace AE::CICD
{
/*
=================================================
	constructor / destructor
=================================================
*/
	Client::Client (StringView name) : _name{name}
	{}

	Client::~Client ()
	{}

/*
=================================================
	Run
=================================================
*/
	bool  Client::Run (RC<IServerProvider> serverProvider)
	{
		CHECK_ERR( _RegisterCommands() );
		CHECK_ERR( _StartClient( RVRef(serverProvider) ));

		std::cout << "Wait for server response..." << std::endl;
		CHECK_ERR( _WaitResponse() );

		std::cout << "Available scripts:" << std::endl;
		for (auto& s : _scripts) {
			std::cout << "  '" << s << "'" << std::endl;
		}

		String	user_input;
		for (;;)
		{
			user_input.clear();

			std::cout << ">";
			std::getline( std::cin, OUT user_input );

			if ( _ParseCommand( user_input ))
				CHECK_ERR( _WaitResponse() );

			for (bool ok = true; ok ;)
			{
				ok = _Receive();

				if ( auto msg = _Encode() )
				{
					CHECK_ERR( _ProcessCommand( this, *msg ));
					ok = true;
				}
			}
		}
		return true;
	}

/*
=================================================
	_RegisterCommands
----
	same as [_InitUserClient line:360](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/cicd/Server.cpp)
=================================================
*/
	bool  Client::_RegisterCommands ()
	{
		return	_Register( SerializedID{"Log"},						&Client::_Cb_Log )						and
				_Register( SerializedID{"ServerInfo"},				&Client::_Cb_ServerInfo )				and
				_Register( SerializedID{"ServerScriptSubmitted"},	&Client::_Cb_ServerScriptSubmitted )	and
				_Register< Msg::ServerRunScript >( SerializedID{"ServerRunScript"} )						and
				_Register< Msg::ServerCancelAll >( SerializedID{"ServerCancelAll"} )						and
				_Register< Msg::LogGroup		>( SerializedID{"LogGroup"} )								and
				_Register< Msg::ClientInfo		>( SerializedID{"ClientInfo"} );
	}

/*
=================================================
	_Cb_ServerInfo
=================================================
*/
	bool  Client::_Cb_ServerInfo (Msg::ServerInfo &msg)
	{
		_scripts		= RVRef(msg.scripts);
		_waitResponse	= false;
		return true;
	}

/*
=================================================
	_Cb_ServerScriptSubmitted
=================================================
*/
	bool  Client::_Cb_ServerScriptSubmitted (const Msg::ServerScriptSubmitted &)
	{
		_waitResponse = false;
		return true;
	}

/*
=================================================
	_WaitResponse
=================================================
*/
	bool  Client::_WaitResponse ()
	{
		_waitResponse = true;

		for (uint p = 0;;)
		{
			bool	ok = _Receive();

			if ( auto msg = _Encode() )
			{
				CHECK_ERR( _ProcessCommand( this, *msg ));
				ok = true;
			}

			if ( ok )
				p = 0;
			else
			if ( not _waitResponse )
				break;
			else
				ThreadUtils::ProgressiveSleepInf( p++ );
		}
		return true;
	}

/*
=================================================
	_ParseCommand
=================================================
*/
	bool  Client::_ParseCommand (StringView userInput)
	{
		if ( userInput.empty() )
			return false;

		usize		cmd_pos	= Min( userInput.size(), userInput.find( ' ' ));
		StringView	cmd		= userInput.substr( 0, cmd_pos );
		StringView	args;

		if ( EqualIC( cmd, "cancel" ))
			return _CancelAll();

		// if has args
		if ( cmd_pos+1 < userInput.size() )
		{
			args = userInput.substr( cmd_pos+1 );

			if ( EqualIC( cmd, "run" ))
				return _RunScript( args );
		}

		RETURN_ERR( "unknown command '"s << cmd << "', args: " << args );
	}

/*
=================================================
	_RunScript
=================================================
*/
	bool  Client::_RunScript (StringView args)
	{
		usize		pos = 0;
		StringView	name;

		if ( Parser::ReadString( args, INOUT pos, OUT name ))
			return _RunScript2( name );

		return _RunScript2( args );
	}

	bool  Client::_RunScript2 (StringView name)
	{
		CHECK_ERR( ArrayContains( _scripts.begin(), _scripts.end(), name ));

		Msg::ServerRunScript	msg;
		msg.script	= name;
		return _Send( msg );
	}

/*
=================================================
	_CancelAll
=================================================
*/
	bool  Client::_CancelAll ()
	{
		Msg::ServerCancelAll	msg;
		return _Send( msg );
	}

/*
=================================================
	_OnConnected
=================================================
*/
	void  Client::_OnConnected ()
	{
		CHECK_ERRV( not _initialized );

		Msg::ClientInfo		msg;
		msg.type = EClientType::User;

		_initialized = _Send( msg );
	}

/*
=================================================
	_OnDisconnected
=================================================
*/
	void  Client::_OnDisconnected ()
	{
		_initialized = false;
	}


} // AE::CICD

#endif // CICD_CLIENT
