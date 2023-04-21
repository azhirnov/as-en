// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_WINDOWS
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
		void*		_handle	= null;		// HMODULE


	// methods
	public:
		WindowsLibrary ()									__NE___	{}
		~WindowsLibrary ()									__NE___	{ Unload(); }

		bool  Load (void* lib)								__NE___;
		bool  Load (NtStringView libName)					__NE___;
		bool  Load (const char *libName)					__NE___	{ return Load( NtStringView{libName} ); }
		bool  Load (const String &libName)					__NE___	{ return Load( NtStringView{libName} ); }
		bool  Load (const Path &libName)					__NE___;
		void  Unload ()										__NE___;

		template <typename T>
		bool  GetProcAddr (NtStringView name, OUT T &result)C_NE___;

		ND_ Path  GetPath ()								C_NE___;

		ND_ explicit operator bool ()						C_NE___	{ return _handle != null; }

	private:
		ND_ void*  _GetProcAddress (NtStringView name)		C_NE___;
	};

	
/*
=================================================
	GetProcAddr
=================================================
*/
	template <typename T>
	inline bool  WindowsLibrary::GetProcAddr (NtStringView name, OUT T &result) C_NE___
	{
		result = BitCast<T>( _GetProcAddress( name.c_str() ));
		return result != null;
	}


} // AE::Base

#endif // AE_PLATFORM_WINDOWS
