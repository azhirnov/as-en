// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_UNIX_BASED
# include <dlfcn.h>

# include "base/Utils/Helpers.h"
# include "base/Algorithms/ArrayUtils.h"
# include "base/Containers/NtStringView.h"
# include "base/Utils/FileSystem.h"

namespace AE::Base
{

	//
	// Unix Dynamic Library
	//

	class UnixLibrary final : public Noncopyable
	{
	// variables
	private:
		void *		_handle	= null;


	// methods
	public:
		UnixLibrary ()						__NE___		{}
		~UnixLibrary ()						__NE___	{ Unload(); }

		bool  Load (NtStringView libName)	__NE___;
		bool  Load (const char *libName)	__NE___	{ return Load( NtStringView{libName} ); }
		bool  Load (const String &libName)	__NE___	{ return Load( NtStringView{libName} ); }
		bool  Load (const Path &libName)	__NE___;
		void  Unload ()						__NE___;

		template <typename T>
		bool  GetProcAddr (NtStringView name, OUT T &result) C_NE___;
		
		ND_ Path  GetPath ()				C_NE___;

		ND_ explicit operator bool ()		C_NE___		{ return _handle != null; }
	};

	
	
/*
=================================================
	Load
=================================================
*/
	inline bool  UnixLibrary::Load (NtStringView libName) __NE___
	{
		CHECK_ERR( _handle == null );
		_handle = ::dlopen( libName.c_str(), RTLD_LAZY | RTLD_LOCAL );
		return _handle != null;
	}

	inline bool  UnixLibrary::Load (const Path &libName) __NE___
	{
		CHECK_ERR( _handle == null );
		_handle = ::dlopen( libName.c_str(), RTLD_LAZY | RTLD_LOCAL );
		return _handle != null;
	}
	
/*
=================================================
	Unload
=================================================
*/
	inline void  UnixLibrary::Unload () __NE___
	{
		if ( _handle ) {
			::dlclose( _handle );
			_handle = null;
		}
	}
	
/*
=================================================
	GetProcAddr
=================================================
*/
	template <typename T>
	inline bool  UnixLibrary::GetProcAddr (NtStringView name, OUT T &result) C_NE___
	{
		result = BitCast<T>( ::dlsym( _handle, name.c_str() ));
		return result != null;
	}
	
/*
=================================================
	GetPath
=================================================
*/
	inline Path  UnixLibrary::GetPath () C_NE___
	{
	#ifdef AE_PLATFORM_ANDROID
		RETURN_ERR( "not supported" );
	#elif defined(AE_PLATFORM_APPLE)
		RETURN_ERR( "not supported" );
	#else

		CHECK_ERR( _handle );

		char	buf [PATH_MAX] = {};
		CHECK_ERR( ::dlinfo( _handle, RTLD_DI_ORIGIN, buf ) == 0 );

		CATCH_ERR( return Path{ buf };)
	#endif
	}


} // AE::Base

#endif // AE_PLATFORM_UNIX_BASED
