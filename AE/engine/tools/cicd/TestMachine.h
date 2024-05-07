// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef CICD_TEST_MACHINE
# include "cicd/BaseMachine.h"

namespace AE::CICD
{

	//
	// Test Machine
	//

	class TestMachine : public BaseMachine
	{
	// methods
	public:
		TestMachine (const Path &baseDir, StringView name);
		~TestMachine ();

		ND_ bool  Run (RC<IServerProvider> serverProvider);

	public:
		ND_ bool  _RunTest (const Path &exe, const Path &workingFolder);

		// android //
		ND_ bool  _AndroidRunTest (StringView libName, StringView entry);
		ND_ bool  _AndroidInstall (StringView apk);


	protected:
		ND_ bool  _RegisterCommands ();

			bool  _Cb_RunTest (const Msg::RunTest &);
			bool  _Cb_BeginSession (const Msg::BeginSession &);
			bool  _Cb_AndroidRunTest (const Msg::AndroidRunTest &);

			void  _OnConnected ()		override;
			void  _OnDisconnected ()	override;
	};


} // AE::CICD

#endif // CICD_TEST_MACHINE
