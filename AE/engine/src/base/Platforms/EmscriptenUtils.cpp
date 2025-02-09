// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Platforms/EmscriptenUtils.h"

#ifdef AE_PLATFORM_EMSCRIPTEN

namespace AE::Base
{

#ifndef AE_CFG_RELEASE
/*
=================================================
	SetCurrentThreadName
=================================================
*/
	void  EmscriptenUtils::SetCurrentThreadName (NtStringView name) __NE___
	{
		StaticLogger::SetCurrentThreadName( StringView{name} );

		// TODO
	}

/*
=================================================
	GetCurrentThreadName
=================================================
*/
	String  EmscriptenUtils::GetCurrentThreadName ()
	{
		// TODO
		return Default;
	}
#endif // AE_CFG_RELEASE

/*
=================================================
	GetCurrentThreadHandle
=================================================
*/
	ThreadHandle  EmscriptenUtils::GetCurrentThreadHandle () __NE___
	{
		// TODO
		return Default;
	}

/*
=================================================
	SetThreadAffinity
=================================================
*/
	bool  EmscriptenUtils::SetThreadAffinity (const ThreadHandle &handle, const uint logicalCoreIdx) __NE___
	{
		// TODO
		Unused( handle, logicalCoreIdx );
		return false;
	}

	bool  EmscriptenUtils::SetCurrentThreadAffinity (const uint logicalCoreIdx) __NE___
	{
		// TODO
		Unused( logicalCoreIdx );
		return false;
	}

/*
=================================================
	SetThreadPriority
=================================================
*/
	bool  EmscriptenUtils::SetThreadPriority (const ThreadHandle &handle, EThreadPriority priority) __NE___
	{
		// TODO
		Unused( handle, priority );
		return false;
	}

	bool  EmscriptenUtils::SetCurrentThreadPriority (EThreadPriority priority) __NE___
	{
		// TODO
		Unused( priority );
		return false;
	}

/*
=================================================
	GetLogicalCoreIndex
=================================================
*/
	uint  EmscriptenUtils::GetLogicalCoreIndex () __NE___
	{
		// TODO
		return 0;
	}

/*
=================================================
	ThreadPause
=================================================
*/
	void  EmscriptenUtils::ThreadPause () __NE___
	{
		// not supported
	}


} // AE::Base

#endif // AE_PLATFORM_EMSCRIPTEN
