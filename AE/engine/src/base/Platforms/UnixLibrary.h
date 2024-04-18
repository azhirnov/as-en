// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_UNIX_BASED
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
		UnixLibrary ()												__NE___	{}
		~UnixLibrary ()												__NE___	{ Unload(); }

		ND_ bool  Open (void* lib)									__NE___;

		ND_ bool  Load (NtStringView libName)						__NE___;
		ND_ bool  Load (StringView libName)							__NE___	{ return Load( NtStringView{libName} ); }
		ND_ bool  Load (const char* libName)						__NE___	{ return Load( NtStringView{libName} ); }
		ND_ bool  Load (const Path &libName)						__NE___;
			void  Unload ()											__NE___;

		template <typename T>
		ND_ bool  GetProcAddr (NtStringView name, OUT T &result)	C_NE___;

		ND_ Path  GetPath ()										C_NE___;

		ND_ explicit operator bool ()								C_NE___	{ return _handle != null; }

	private:
		ND_ void*  _GetProcAddr (const char* name)					C_NE___;
	};


/*
=================================================
	GetProcAddr
=================================================
*/
	template <typename T>
	inline bool  UnixLibrary::GetProcAddr (NtStringView name, OUT T &result) C_NE___
	{
		ASSERT( not name.empty() );

		result = BitCast<T>( _GetProcAddr( name.c_str() ));
		return result != null;
	}

} // AE::Base

#endif // AE_PLATFORM_UNIX_BASED
