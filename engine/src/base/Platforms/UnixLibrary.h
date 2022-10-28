// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_UNIX_BASED

# include "base/Utils/Helpers.h"
# include "base/Algorithms/ArrayUtils.h"
# include "base/Containers/NtStringView.h"
# include "base/Utils/FileSystem.h"

# include <dlfcn.h>
//# include <linux/limits.h>

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
		UnixLibrary ()		{}
		~UnixLibrary ()		{ Unload(); }

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
	inline bool  UnixLibrary::Load (NtStringView libName)
	{
		CHECK_ERR( _handle == null );
		_handle = ::dlopen( libName.c_str(), RTLD_LAZY | RTLD_LOCAL );
		return _handle != null;
	}

	inline bool  UnixLibrary::Load (const Path &libName)
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
	inline void  UnixLibrary::Unload ()
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
	inline bool  UnixLibrary::GetProcAddr (NtStringView name, OUT T &result) const
	{
		result = BitCast<T>( ::dlsym( _handle, name.c_str() ));
		return result != null;
	}
	
/*
=================================================
	GetPath
=================================================
*/
	inline Path  UnixLibrary::GetPath () const
	{
	#ifdef AE_PLATFORM_ANDROID
		RETURN_ERR( "not supported" );
	#elif defined(AE_PLATFORM_APPLE)
		RETURN_ERR( "not supported" );
	#else

		CHECK_ERR( _handle );

		char	buf [PATH_MAX] = {};
		CHECK_ERR( ::dlinfo( _handle, RTLD_DI_ORIGIN, buf ) == 0 );

		return Path{ buf };
	#endif
	}


} // AE::Base

#endif // AE_PLATFORM_UNIX_BASED
