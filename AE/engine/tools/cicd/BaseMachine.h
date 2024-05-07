// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#if defined(CICD_BUILD_MACHINE) or defined(CICD_TEST_MACHINE)
# include "cicd/NetBase.h"

namespace AE::CICD
{

	//
	// Base Machine
	//

	class BaseMachine : public NetBase
	{
	// variables
	protected:
		Atomic<bool>	_looping		{false};
		bool			_initialized	= false;

		const Path		_baseDir;
		const String	_name;


	// methods
	public:

		// git //
		ND_ bool  _GitClone (StringView repository, const Path &dstFolder, bool recurseSubmodules);
		ND_ bool  _GitClone (StringView tag, StringView repository, const Path &dstFolder, bool recurseSubmodules);
		ND_ bool  _GitPullSubmodules (const Path &dir);
		ND_ bool  _GitCheckout (const Path &dir, StringView commit);
		ND_ bool  _GitPull (const Path &dir, StringView branch, StringView remote, bool rebase);
		ND_ bool  _GitPush (const Path &dir, StringView localBranch, StringView remote, StringView dstBranch = Default);
		ND_ bool  _GitHardReset (const Path &dir, StringView branch, StringView remote);
		ND_ bool  _GetGetHash (OUT String &hash);

		// linux //
		ND_ bool  _LinuxInstallApp (StringView name);
		ND_ bool  _LinuxAptUpdate ();


	protected:
		BaseMachine (const Path &baseDir, StringView name) : _baseDir{FS::ToAbsolute(baseDir)}, _name{name} {}

		ND_ bool  _Execute (String &cmd, const Path &dir = Default, OSProcess::EFlags flags = Default);
		ND_ bool  _Execute (String &cmd, const Path &dir, INOUT uint &i);
		ND_ bool  _Execute (String &cmd, const Path &dir, OSProcess::EFlags flags, INOUT uint &i);

		ND_ bool  _InitClient (EClientType);
		ND_ bool  _SessionLoop ();

			bool  _BeginSession (uint id, const Path &subPath);
			bool  _Cb_EndSession (const Msg::EndSession &);
			bool  _Cb_FileSystemCommand (const Msg::FileSystemCommand &);

	private:
		ND_ static bool  _GetGPUInfo (INOUT MachineInfo &);
	};


	#define C_LOG_GROUP( ... )				\
	{										\
		const String	log = __VA_ARGS__;	\
		AE_LOGI( log );						\
		CHECK_ERR( _SendLogGroup( log ));	\
	}

	#define C_LOGI( ... )					\
	{										\
		const String	log = __VA_ARGS__;	\
		AE_LOGI( log );						\
		CHECK_ERR( _SendLog( log, 0 ));		\
	}

	#define C_LOGE( ... )					\
	{										\
		const String	log = __VA_ARGS__;	\
		AE_LOGW( log );					\
		CHECK_ERR( _SendLog( log, 0 ));		\
	}

} // AE::CICD

#endif // CICD_BUILD_MACHINE or CICD_TEST_MACHINE
