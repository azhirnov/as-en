// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_WINDOWS
# include "base/Platforms/WindowsHeader.cpp.h"
# include "base/Platforms/WindowsLibrary.h"
# include "base/Algorithms/ToString.h"

namespace AE::Base
{
# if 1
#	define PUSH_ERRORMODE \
		const DWORD		old_mode = ::SetErrorMode( SEM_FAILCRITICALERRORS ); \
		::SetErrorMode( old_mode | SEM_FAILCRITICALERRORS );

#	define POP_ERRORMODE \
		::SetErrorMode( old_mode );
# else
#	define PUSH_ERRORMODE
#	define POP_ERRORMODE
# endif
	
/*
=================================================
	constructor
=================================================
*/
	WindowsLibrary::WindowsLibrary (WindowsLibrary &&other) __NE___ :
		_handle{ other._handle }, _loaded{ other._loaded }
	{
		other._handle = null;
		other._loaded = false;
	}

/*
=================================================
	Open
=================================================
*/
	bool  WindowsLibrary::Open (void* lib) __NE___
	{
		CHECK_ERR( _handle == null and lib != null );
		_handle = lib;
		_loaded = false;
		return true;
	}
	
	bool  WindowsLibrary::Open (NtStringView libName) __NE___
	{
		CHECK_ERR( _handle == null );
		_handle = ::GetModuleHandleA( libName.c_str() );
		
		if_unlikely( _handle == null )
			WIN_CHECK_DEV( "Error when opening shared library '"s << libName << "': " );
		
		_loaded = false;
		return _handle != null;
	}

/*
=================================================
	Load
=================================================
*/
	bool  WindowsLibrary::Load (NtStringView libName) __NE___
	{
		CHECK_ERR( _handle == null );

		PUSH_ERRORMODE;
		_handle = ::LoadLibraryA( libName.c_str() );
		POP_ERRORMODE;

		if_unlikely( _handle == null )
			WIN_CHECK_DEV( "Error when loading shared library '"s << libName << "': " );
		
		_loaded = (_handle != null);
		return _loaded;
	}

	bool  WindowsLibrary::Load (const Path &libName) __NE___
	{
		CHECK_ERR( _handle == null );
		
		PUSH_ERRORMODE;
		_handle = ::LoadLibraryW( libName.c_str() );
		POP_ERRORMODE;

		if_unlikely( _handle == null )
			WIN_CHECK_DEV( "Error when loading shared library '"s << ToString(libName) << "': " );
		
		_loaded = (_handle != null);
		return _loaded;
	}

/*
=================================================
	Unload
=================================================
*/
	void  WindowsLibrary::Unload () __NE___
	{
		if ( _handle != null and _loaded )
		{
			::FreeLibrary( BitCast<HMODULE>(_handle) );
		}
		_handle = null;
		_loaded = false;
	}

/*
=================================================
	GetPath
=================================================
*/
	Path  WindowsLibrary::GetPath () C_NE___
	{
		CHECK_ERR( _handle != null );

		wchar_t	buf[MAX_PATH] = {};
		CHECK_ERR( ::GetModuleFileNameW( BitCast<HMODULE>(_handle), buf, DWORD(CountOf(buf)) ) != FALSE );

		NOTHROW_ERR( return Path{ buf };)
	}

/*
=================================================
	_GetProcAddress
=================================================
*/
	void*  WindowsLibrary::_GetProcAddress (const char* name) C_NE___
	{
		return BitCast<void*>(::GetProcAddress( BitCast<HMODULE>(_handle), name ));
	}


} // AE::Base

#endif // AE_PLATFORM_WINDOWS
