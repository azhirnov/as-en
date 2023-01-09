// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_WINDOWS

# include "base/Platforms/WindowsHeader.h"
# include "base/Platforms/WindowsUtils.h"
# include "base/Containers/NtStringView.h"
# include "base/Utils/Helpers.h"
# include "base/Utils/FileSystem.h"
# include "base/Algorithms/ArrayUtils.h"

namespace AE::Base
{

	//
	// Windows Dynamic Library
	//

	class WindowsLibrary final : public Noncopyable
	{
	// variables
	private:
		HMODULE		_handle	= null;


	// methods
	public:
		WindowsLibrary ()					__NE___	{}
		~WindowsLibrary ()					__NE___	{ Unload(); }

		bool  Load (HMODULE lib)			__NE___;
		bool  Load (NtStringView libName)	__NE___;
		bool  Load (const char *libName)	__NE___	{ return Load( NtStringView{libName} ); }
		bool  Load (const String &libName)	__NE___	{ return Load( NtStringView{libName} ); }
		bool  Load (const Path &libName)	__NE___;
		void  Unload ()						__NE___;

		template <typename T>
		bool  GetProcAddr (NtStringView name, OUT T &result) C_NE___;

		ND_ Path  GetPath ()				C_NE___;

		ND_ explicit operator bool ()		C_NE___	{ return _handle != null; }
	};


	
/*
=================================================
	Load
=================================================
*/
	inline bool  WindowsLibrary::Load (HMODULE lib) __NE___
	{
		CHECK_ERR( _handle == null and lib != null );
		_handle = lib;
		return true;
	}

	inline bool  WindowsLibrary::Load (NtStringView libName) __NE___
	{
		CHECK_ERR( _handle == null );
		_handle = ::LoadLibraryA( libName.c_str() );
		
		if_unlikely( _handle == null )
			WIN_CHECK_DEV( "LoadLibrary error: " );

		return _handle != null;
	}

	inline bool  WindowsLibrary::Load (const Path &libName) __NE___
	{
		CHECK_ERR( _handle == null );
		_handle = ::LoadLibraryW( libName.c_str() );
		
		if_unlikely( _handle == null )
			WIN_CHECK_DEV( "LoadLibrary error: " );

		return _handle != null;
	}
	
/*
=================================================
	Unload
=================================================
*/
	inline void  WindowsLibrary::Unload () __NE___
	{
		if ( _handle != null )
		{
			::FreeLibrary( _handle );
			_handle = null;
		}
	}
	
/*
=================================================
	GetProcAddr
=================================================
*/
	template <typename T>
	inline bool  WindowsLibrary::GetProcAddr (NtStringView name, OUT T &result) C_NE___
	{
		result = BitCast<T>( ::GetProcAddress( _handle, name.c_str() ));
		return result != null;
	}
	
/*
=================================================
	GetPath
=================================================
*/
	inline Path  WindowsLibrary::GetPath () C_NE___
	{
		CHECK_ERR( _handle != null );

		wchar_t	buf[MAX_PATH] = {};
		CHECK_ERR( ::GetModuleFileNameW( _handle, buf, DWORD(CountOf(buf)) ) != FALSE );

		CATCH_ERR( return Path{ buf };)
	}


} // AE::Base

#endif // AE_PLATFORM_WINDOWS
