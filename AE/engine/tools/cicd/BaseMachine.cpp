// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(CICD_BUILD_MACHINE) or defined(CICD_TEST_MACHINE)
# include "cicd/BaseMachine.h"

namespace AE::CICD
{

/*
=================================================
	_GitClone
=================================================
*/
	bool  BaseMachine::_GitClone (StringView repository, const Path &dstFolder, bool recurseSubmodules)
	{
		return _GitClone( Default, repository, dstFolder, recurseSubmodules );
	}

	bool  BaseMachine::_GitClone (StringView tag, StringView repository, const Path &dstFolder, bool recurseSubmodules)
	{
		AE_LOGI( "git clone '"s << repository << "' to '" << ToString(dstFolder) << "'" );

		_DeleteFolder( dstFolder );

		uint	i	= 0;
		String	cmd = "git clone";

		if ( recurseSubmodules )
			cmd << " --recurse-submodules";

		if ( not tag.empty() )
			cmd << " --branch \""s << tag << '"';

		cmd << " \"" << repository << "\" \"" << ToString(dstFolder) << '"';
		cmd << " --single-branch";
		CHECK_ERR( _Execute( cmd, Default, INOUT i ));

		cmd = "git log -1";
		CHECK_ERR( _Execute( cmd, dstFolder, INOUT i ));

		return true;
	}

/*
=================================================
	_GitCheckout
=================================================
*/
	bool  BaseMachine::_GitCheckout (const Path &dir, StringView commit)
	{
		AE_LOGI( "git checkout" );

		uint	i	= 0;
		String	cmd = "git checkout \""s << commit << '"';
		CHECK_ERR( _Execute( cmd, dir, INOUT i ));

		cmd = "git log -1";
		CHECK_ERR( _Execute( cmd, dir, INOUT i ));

		return true;
	}

/*
=================================================
	_GitPullSubmodules
=================================================
*/
	bool  BaseMachine::_GitPullSubmodules (const Path &dir)
	{
		AE_LOGI( "git pull submodules" );

		uint	i	= 0;
		String	cmd = "git submodule update --init --recursive";
		CHECK_ERR( _Execute( cmd, dir, INOUT i ));

		cmd = "git pull --recurse-submodules";
		CHECK_ERR( _Execute( cmd, dir, INOUT i ));

		return true;
	}

/*
=================================================
	_GitPull
=================================================
*/
	bool  BaseMachine::_GitPull (const Path &dir, StringView branch, StringView remote, bool rebase)
	{
		AE_LOGI( "git pull" );

		uint	i	= 0;
		String	cmd = "git pull ";

		if ( rebase )
			cmd << "--rebase ";

		cmd << remote << " " << branch;
		CHECK_ERR( _Execute( cmd, dir, INOUT i ));

		cmd = "git log -1";
		CHECK_ERR( _Execute( cmd, dir, INOUT i ));

		return true;
	}

/*
=================================================
	_GitPush
=================================================
*/
	bool  BaseMachine::_GitPush (const Path &dir, StringView branch, StringView remote, StringView dstBranch)
	{
		AE_LOGI( "git push" );

		String	cmd = "git push "s << remote << ' ';

		if ( not dstBranch.empty() )
			cmd << dstBranch << ':';

		cmd << branch;
		return _Execute( cmd, dir );
	}

/*
=================================================
	_GitHardReset
=================================================
*/
	bool  BaseMachine::_GitHardReset (const Path &dir, StringView branch, StringView remote)
	{
		AE_LOGI( "git hard reset" );

		String	cmd = "git reset --hard \""s << remote << '/' << branch << '"';
		return _Execute( cmd, dir );
	}

/*
=================================================
	_GetGetHash
=================================================
*/
	bool  BaseMachine::_GetGetHash (OUT String &hash)
	{
		using EFlags = OSProcess::EFlags;

		String		cmd		= "git rev-parse --verify HEAD";
		String		output;
		OSProcess	proc;
		CHECK_ERR( proc.Execute( cmd, OUT output ));

		usize	pos = Min( output.size(), output.find( '\n' ));
		CHECK_ERR( pos == 40 );

		output.resize( pos );
		hash = RVRef(output);

		return true;
	}

/*
=================================================
	_Execute
=================================================
*/
	bool  BaseMachine::_Execute (String &cmd, const Path &dir, OSProcess::EFlags flags, INOUT uint &i)
	{
		using EFlags = OSProcess::EFlags;

		CHECK_ERR( dir.empty() or FS::IsDirectory( dir ));

		AE_LOGI( "Execute: "s << cmd << ", in folder: " << (dir.empty() ? "current"s : ToString(dir)) );

		flags |= EFlags::ReadOutput | EFlags::NoWindow;

		OSProcess	proc;
		CHECK_ERR( proc.ExecuteAsync( cmd, dir, flags ));

		String	output;

		for (; proc.IsActive();)
		{
			if ( not proc.ReadOutput( INOUT output ))
				break;

			if ( not output.empty() )
			{
				CHECK_ERR( _SendLog( output, i++ ));
				DEBUG_ONLY( std::cout << output;)
				output.clear();
			}
		}

		CHECK_ERR( proc.WaitAndClose( INOUT output ));

		if ( not output.empty() )
		{
			CHECK_ERR( _SendLog( output, i++ ));
			DEBUG_ONLY( std::cout << output;)
		}
		return true;
	}

	bool  BaseMachine::_Execute (String &cmd, const Path &dir, OSProcess::EFlags flags)
	{
		uint	i = 0;
		return _Execute( cmd, dir, flags, INOUT i );
	}

	bool  BaseMachine::_Execute (String &cmd, const Path &dir, INOUT uint &i)
	{
		return _Execute( cmd, dir, Default, INOUT i );
	}

/*
=================================================
	_LinuxInstallApp
=================================================
*/
	bool  BaseMachine::_LinuxInstallApp (StringView name)
	{
	#ifdef AE_PLATFORM_LINUX
		// sudo apt --yes install <name>
		// sudo apt --yes --force-yes install <name>

		String	cmd = "sudo apt --yes --force-yes install "s << name;
		return _Execute( cmd );
	#else
		Unused( name );
		return false;
	#endif
	}

/*
=================================================
	_LinuxAptUpdate
=================================================
*/
	bool  BaseMachine::_LinuxAptUpdate ()
	{
	#ifdef AE_PLATFORM_LINUX
		String	cmd = "sudo apt --yes update & upgrade";
		return _Execute( cmd );
	#else
		return false;
	#endif
	}

/*
=================================================
	_InitClient
=================================================
*/
	bool  BaseMachine::_InitClient (EClientType type)
	{
		CHECK_ERR( not _initialized );

		Msg::ClientInfo		msg;
		msg.type	= type;
		msg.name	= _name;

	  #if defined(AE_PLATFORM_WINDOWS)
		msg.info.osType	= EOSType::Windows;
	  #elif defined(AE_PLATFORM_ANDROID)
		msg.info.osType = EOSType::Android;
	  #elif defined(AE_PLATFORM_LINUX)
		msg.info.osType = EOSType::Linux;
	  #elif defined(AE_PLATFORM_MACOS)
		msg.info.osType = EOSType::MacOS;
	  #elif defined(AE_PLATFORM_BSD)
		msg.info.osType = EOSType::BSD;
	  #else
		#error unknown platform!
	  #endif

		msg.info.osVersion	= Version2{PlatformUtils::GetOSVersion()};
		msg.info.osName		= PlatformUtils::GetOSName();

		// CPU
		{
			const auto&	cpu_info = CpuArchInfo::Get();

			msg.info.cpuVendor	= cpu_info.cpu.vendor;
			msg.info.cpuArch	= cpu_info.cpu.arch;

			msg.info.cpuFS.set( ECPUFeatureSet::AVX256,	cpu_info.feats.AVX256 );
			msg.info.cpuFS.set( ECPUFeatureSet::AVX512,	cpu_info.feats.AVX512 );
			msg.info.cpuFS.set( ECPUFeatureSet::Neon,	cpu_info.feats.NEON );
			msg.info.cpuFS.set( ECPUFeatureSet::SVE,	cpu_info.feats.SVE );
			msg.info.cpuFS.set( ECPUFeatureSet::SVE2,	cpu_info.feats.SVE2 );
		}

		// GPU
		#ifdef CICD_TEST_MACHINE
		if ( type == EClientType::Test )
		{
			Unused( _GetGPUInfo( INOUT msg.info ));
		}
		#endif

		_initialized = _Send( msg );
		return _initialized;
	}

/*
=================================================
	_SessionLoop
=================================================
*/
	bool  BaseMachine::_SessionLoop ()
	{
		for (uint p = 0; _looping.load();)
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
				ThreadUtils::ProgressiveSleepInf( p++ );
		}
		return true;
	}

/*
=================================================
	_BeginSession
=================================================
*/
	bool  BaseMachine::_BeginSession (uint id, const Path &subPath)
	{
		CHECK( not _HasSession() );

		if ( _HasSession() )
			_DeleteFolder( _sessionDir );

		_sessionId	= id;
		_sessionDir	= FS::Normalize( _baseDir / subPath );

		_DeleteFolder( _sessionDir );
		CHECK_ERR( FS::CreateDirectories( _sessionDir ));

		return true;
	}

/*
=================================================
	_Cb_EndSession
=================================================
*/
	bool  BaseMachine::_Cb_EndSession (const Msg::EndSession &msg)
	{
		CHECK_ERR( _HasSession() );
		CHECK_ERR( _sessionId == msg.id );

		_DeleteFolder( _sessionDir );

		_sessionId = 0;
		_sessionDir.clear();

		return _Send( msg );
	}

/*
=================================================
	_Cb_FileSystemCommand
=================================================
*/
	bool  BaseMachine::_Cb_FileSystemCommand (const Msg::FileSystemCommand &msg)
	{
		CHECK_ERR( _sessionId == msg.sessionId );

		using EType = Msg::FileSystemCommand::EType;

		switch_enum( msg.type )
		{
			case EType::RemoveDir :
				_DeleteFolder( _sessionDir / msg.arg0 );
				break;

			case EType::MakeDir :
				CHECK_ERR( FS::CreateDirectories( _sessionDir / msg.arg0 ));
				break;

			case EType::CopyDir :
				CHECK_ERR_MSG( FS::IsDirectory( _sessionDir / msg.arg0 ),
					"Src directory '"s << ToString(_sessionDir / msg.arg0) << "' is not exists" );
				CHECK_ERR( FS::CopyDirectory( _sessionDir / msg.arg0, _sessionDir / msg.arg1 ));
				break;

			case EType::DeleteFile :
				CHECK( FS::DeleteFile( _sessionDir / msg.arg0 ));
				break;

			case EType::CopyFile :
				CHECK_ERR_MSG( FS::IsFile( _sessionDir / msg.arg0 ),
					"Src file '"s << ToString(_sessionDir / msg.arg0) << "' is not exists" );
				CHECK_ERR( FS::CopyFile( _sessionDir / msg.arg0, _sessionDir / msg.arg1 ));
				break;

			case EType::Unknown :
				return false;
		}
		return true;
	}

/*
=================================================
	_GetGPUInfo
=================================================
*/
#ifndef AE_ENABLE_VULKAN
	bool  BaseMachine::_GetGPUInfo (INOUT MachineInfo &)
	{
		return true;
	}
#endif

} // AE::CICD

#endif // CICD_BUILD_MACHINE or CICD_TEST_MACHINE
