// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef CICD_CLIENT
# include "cicd/NetBase.h"

namespace AE::CICD
{

	//
	// Client
	//

	class Client final : public NetBase
	{
	// variables
	private:
		const String		_name;
		Array<String>		_scripts;

		bool				_waitResponse	= false;
		bool				_initialized	= false;


	// methods
	public:
		Client (StringView name);
		~Client ();

		ND_ bool  Run (RC<IServerProvider> serverProvider);

	private:
		ND_ bool  _RegisterCommands ();
		ND_ bool  _ParseCommand (StringView userInput);
		ND_ bool  _WaitResponse ();

			bool  _Cb_ServerInfo (Msg::ServerInfo &);
			bool  _Cb_ServerScriptSubmitted (const Msg::ServerScriptSubmitted &);

		ND_	bool  _RunScript (StringView args);
		ND_	bool  _RunScript2 (StringView name);
		ND_ bool  _CancelAll ();

			void  _OnConnected ()		override;
			void  _OnDisconnected ()	override;
	};


} // AE::CICD

#endif // CICD_CLIENT
