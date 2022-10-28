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
		WindowsLibrary ()		{}
		~WindowsLibrary ()		{ Unload(); }

			bool  Load (HMODULE lib);
			bool  Load (NtStringView libName);
			bool  Load (const char *libName)		{ return Load( NtStringView{libName} ); }
			bool  Load (const String &libName)		{ return Load( NtStringView{libName} ); }
			bool  Load (const Path &libName);
			void  Unload ();

		template <typename T>
			bool  GetProcAddr (NtStringView name, OUT T &result) const;

		ND_ Path  GetPath () const;

		ND_ explicit operator bool ()	const		{ return _handle != null; }
	};


	
/*
=================================================
	Load
=================================================
*/
	inline bool  WindowsLibrary::Load (HMODULE lib)
	{
		CHECK_ERR( _handle == null and lib != null );
		_handle = lib;
		return true;
	}

	inline bool  WindowsLibrary::Load (NtStringView libName)
	{
		CHECK_ERR( _handle == null );
		_handle = ::LoadLibraryA( libName.c_str() );
		
		if_unlikely( _handle == null )
			WindowsUtils::CheckError( "LoadLibrary error: ", __FILE__, __LINE__ );

		return _handle != null;
	}

	inline bool  WindowsLibrary::Load (const Path &libName)
	{
		CHECK_ERR( _handle == null );
		_handle = ::LoadLibraryW( libName.c_str() );
		
		if_unlikely( _handle == null )
			WindowsUtils::CheckError( "LoadLibrary error: ", __FILE__, __LINE__ );

		return _handle != null;
	}
	
/*
=================================================
	Unload
=================================================
*/
	inline void  WindowsLibrary::Unload ()
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
	inline bool  WindowsLibrary::GetProcAddr (NtStringView name, OUT T &result) const
	{
		result = BitCast<T>( ::GetProcAddress( _handle, name.c_str() ));
		return result != null;
	}
	
/*
=================================================
	GetPath
=================================================
*/
	inline Path  WindowsLibrary::GetPath () const
	{
		CHECK_ERR( _handle != null );

		wchar_t	buf[MAX_PATH] = {};
		CHECK_ERR( ::GetModuleFileNameW( _handle, buf, DWORD(CountOf(buf)) ) != FALSE );

		return Path{ buf };
	}


} // AE::Base

#endif // AE_PLATFORM_WINDOWS
