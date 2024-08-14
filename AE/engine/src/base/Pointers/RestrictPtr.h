// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	MSVC docs: https://learn.microsoft.com/en-us/cpp/cpp/extension-restrict?view=msvc-170
*/

#pragma once

#include "base/Math/Byte.h"

namespace AE::Base
{

	//
	// Restrict (unique) Pointer
	//

	template <typename T>
	class RstPtr final
	{
	// types
	public:
		using Self		= RstPtr< T >;
		using CSelf		= RstPtr< const T >;


	// variables
	private:
	  #if defined(AE_COMPILER_MSVC)
		T * __restrict		_ptr	= null;

	  #elif defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)
		T * __restrict__	_ptr	= null;

	  #else
		T *					_ptr	= null;
	  #	pragma message("'restrict' attribute is not supported")
	  #endif


	// methods
	public:
		constexpr RstPtr ()								__NE___	{}
		constexpr explicit RstPtr (T* ptr)				__NE___ : _ptr{ptr} {}
		constexpr RstPtr (std::nullptr_t)				__NE___ {}

		constexpr RstPtr (Self &&)						__NE___	= default;
		constexpr Self&  operator = (Self &&)			__NE___	= default;

		constexpr RstPtr (const Self &)					__NE___ = default;
		constexpr Self&  operator = (const Self &)		__NE___ = default;

		constexpr Self&  operator = (T* ptr)			__NE___	{ _ptr = ptr;  return *this; }

		ND_ constexpr explicit operator bool ()			__NE___	{ return _ptr != null; }
		ND_ constexpr explicit operator bool ()			C_NE___	{ return _ptr != null; }

		ND_ constexpr Self	operator + (Bytes offset)	C_NE___	{ return Self{ _ptr + offset }; }


	  #if defined(AE_COMPILER_MSVC)

		ND_ __declspec(restrict) operator T* ()			__NE___	{ return _ptr; }
		ND_ __declspec(restrict) operator T const* ()	C_NE___	{ return _ptr; }

		ND_ __declspec(restrict) T*			get ()		__NE___	{ return _ptr; }
		ND_ __declspec(restrict) T const*	get ()		C_NE___	{ return _ptr; }


	  #elif defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)

		ND_ operator T* __restrict__ ()					__NE___	{ return _ptr; }
		ND_ operator T const* __restrict__ ()			C_NE___	{ return _ptr; }

		ND_ T* __restrict__					get ()		__NE___	{ return _ptr; }
		ND_ T const* __restrict__			get ()		C_NE___	{ return _ptr; }


	  #else
		ND_ operator T* ()								__NE___	{ return _ptr; }
		ND_ operator T const* ()						C_NE___	{ return _ptr; }

		ND_ T*								get ()		__NE___	{ return _ptr; }
		ND_ T const*						get ()		C_NE___	{ return _ptr; }

	  #endif
	};


} // AE::Base
