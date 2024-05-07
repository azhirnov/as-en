// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/FileSystem/Path.h"
#include "base/Utils/Threading.h"
#include "base/Math/BitMath.h"

#ifdef AE_PLATFORM_UNIX_BASED

namespace AE::Base
{

	//
	// Unix Process
	//

	class UnixProcess final
	{
	// types
	public:
		enum class EFlags
		{
			None			= 0,
			NoWindow		= 1 << 0,
			ReadOutput		= 1 << 1,	// optional for sync execution, not compatible with 'NoWindow' flag
			Unknown			= None,
		};


	// variables
	private:
		FILE*	_file	= null;

		static constexpr auto	_DefTimeout = milliseconds{60'000};
		static constexpr auto	_Flags1		= EFlags::NoWindow;
		static constexpr auto	_Flags2		= EFlags(uint(EFlags::NoWindow) | uint(EFlags::ReadOutput));
		static constexpr usize	_BufSize	= 1 << 12;


	// methods
	public:
		UnixProcess () {}
		~UnixProcess ();

		ND_ bool  ExecuteAsync (StringView commandLine, EFlags flags = _Flags1);
		ND_ bool  ExecuteAsync (StringView commandLine, const Path &currentDir, EFlags flags = _Flags1);

		ND_ bool  IsActive () C_NE___;

		bool  Terminate (milliseconds timeout = _DefTimeout);

		bool  WaitAndClose (milliseconds timeout = _DefTimeout);
		bool  WaitAndClose (INOUT String &output, milliseconds timeout = _DefTimeout);

		bool  ReadOutput (INOUT String &output);

		static bool  Execute (StringView commandLine, EFlags flags = _Flags1, milliseconds timeout = _DefTimeout);
		static bool  Execute (StringView commandLine, INOUT String &output, EFlags flags = _Flags2, milliseconds timeout = _DefTimeout);
		static bool  Execute (StringView commandLine, const Path &currentDir, EFlags flags = _Flags1, milliseconds timeout = _DefTimeout);
		static bool  Execute (StringView commandLine, const Path &currentDir, INOUT String &output, EFlags flags = _Flags2, milliseconds timeout = _DefTimeout);

	private:
		bool  _ExecuteAsync (StringView commandLine, const Path* currentDir, EFlags flags);
	};

	AE_BIT_OPERATORS( UnixProcess::EFlags );


} // AE::Base

#endif // AE_PLATFORM_UNIX_BASED
