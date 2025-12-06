// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_WINDOWS
# include "base/Platforms/WindowsUtils.h"
# include "base/Containers/NtStringView.h"
# include "base/Utils/Helpers.h"
# include "base/Algorithms/ArrayUtils.h"
# include "base/CompileTime/FunctionInfo.h"

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
		bool		_loaded	= false;


	// methods
	public:
		WindowsLibrary ()											__NE___	{}
		explicit WindowsLibrary (WindowsLibrary &&)					__NE___;
		~WindowsLibrary ()											__NE___	{ Unload(); }

		// open already loaded library to avoid conflicts
		ND_ bool  Open (void* lib)									__NE___;
		ND_ bool  Open (NtStringView libName)						__NE___;
		ND_ bool  Open (StringView libName)							__NE___	{ return Open( NtStringView{libName} ); }
		ND_ bool  Open (const char* libName)						__NE___	{ return Open( NtStringView{libName} ); }

		// load new library
		ND_ bool  Load (NtStringView libName)						__NE___;
		ND_ bool  Load (StringView libName)							__NE___	{ return Load( NtStringView{libName} ); }
		ND_ bool  Load (const char* libName)						__NE___	{ return Load( NtStringView{libName} ); }
		ND_ bool  Load (const Path &libName)						__NE___;
			void  Unload ()											__NE___;

		template <typename T>
		ND_ bool  GetProcAddr (NtStringView name, OUT T &result)	C_NE___;

		template <typename T>
		ND_ bool  GetVarAddr (NtStringView name, OUT T* &result)	C_NE___;

		ND_ Path  GetPath ()										C_NE___;

		ND_ explicit operator bool ()								C_NE___	{ return _handle != null; }

	private:
		ND_ void*  _GetProcAddress (const char* name)				C_NE___;
	};


/*
=================================================
	GetProcAddr
=================================================
*/
	template <typename T>
	bool  WindowsLibrary::GetProcAddr (NtStringView name, OUT T &result) C_NE___
	{
		if constexpr( not IsSame< T, void* >)
			StaticAssert( IsGlobalFunction< T >);

		NonNull( _handle );
		ASSERT( not name.empty() );

		result = BitCast<T>( _GetProcAddress( name.c_str() ));
		return result != null;
	}

/*
=================================================
	GetVarAddr
=================================================
*/
	template <typename T>
	bool  WindowsLibrary::GetVarAddr (NtStringView name, OUT T* &result) C_NE___
	{
		NonNull( _handle );
		ASSERT( not name.empty() );

		result = BitCast<T*>( _GetProcAddress( name.c_str() ));
		return result != null;
	}

} // AE::Base

#endif // AE_PLATFORM_WINDOWS
