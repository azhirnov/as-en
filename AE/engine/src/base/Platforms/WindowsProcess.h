// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_WINDOWS
# include "base/Math/BitMath.h"
# include "base/FileSystem/Path.h"
# include "base/Utils/Threading.h"

namespace AE::Base
{

	//
	// WinAPI Process
	//

	class WindowsProcess final
	{
	// types
	public:
		enum class EFlags
		{
			None				= 0,
			NoWindow			= 1 << 0,
			ReadOutput			= 1 << 1,	// optional for sync execution, not compatible with 'NoWindow' flag
			UseCommandPrompt	= 1 << 2,
			UsePowerShell		= 1 << 3,
		//	UseWSL				= 1 << 4,	// windows subsystem for linux	// TODO
			Unknown				= None,
		};


	// variables
	private:
		void *		_thread			= null;
		void *		_process		= null;
		void *		_streamOutRead	= null;
		void *		_streamInWrite	= null;
		EFlags		_flags			= Default;

		static constexpr auto	_DefTimeout = milliseconds{60'000};
		static constexpr auto	_Flags1		= EFlags::NoWindow;
		static constexpr auto	_Flags2		= EFlags(uint(EFlags::NoWindow) | uint(EFlags::ReadOutput));
		static constexpr usize	_BufSize	= 1 << 12;


	// methods
	public:
		WindowsProcess ()	{}
		~WindowsProcess ();

		ND_ bool  ExecuteAsync (String &commandLine, EFlags flags = _Flags1);
		ND_ bool  ExecuteAsync (WString &commandLine, EFlags flags = _Flags1);
		ND_ bool  ExecuteAsync (String &commandLine, const Path &currentDir, EFlags flags = _Flags1);
		ND_ bool  ExecuteAsync (WString &commandLine, const Path &currentDir, EFlags flags = _Flags1);

		ND_ bool  IsActive () C_NE___;

		bool  Terminate (milliseconds timeout = _DefTimeout);

		bool  WaitAndClose (milliseconds timeout = _DefTimeout);
		bool  WaitAndClose (INOUT String &output, milliseconds timeout = _DefTimeout);

		bool  ReadOutput (INOUT String &output);

		static bool  Execute (String &commandLine, EFlags flags = _Flags1, milliseconds timeout = _DefTimeout);
		static bool  Execute (String &commandLine, INOUT String &output, EFlags flags = _Flags2, milliseconds timeout = _DefTimeout);
		static bool  Execute (String &commandLine, const Path &currentDir, EFlags flags = _Flags1, milliseconds timeout = _DefTimeout);
		static bool  Execute (String &commandLine, const Path &currentDir, INOUT String &output, EFlags flags = _Flags2, milliseconds timeout = _DefTimeout);

	private:
		template <typename T>
		bool  _ExecuteAsync (BasicString<T> &commandLine, const Path* currentDir, EFlags flags);
	};

	AE_BIT_OPERATORS( WindowsProcess::EFlags );


} // AE::Base

#endif // AE_PLATFORM_WINDOWS
