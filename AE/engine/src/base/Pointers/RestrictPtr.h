// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	MSVC docs: https://learn.microsoft.com/en-us/cpp/cpp/extension-restrict?view=msvc-170
*/

#pragma once

#include "base/Math/Byte.h"
#include "base/Pointers/Ptr.h"

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
		using Self	= RstPtr< T >;


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
		__Cx__ RstPtr ()									__NE___	{}
		__Cx__ RstPtr (std::nullptr_t)						__NE___ {}
		__Cx__ explicit RstPtr (T* ptr)						__NE___ : _ptr{ptr} {}

		template <typename B> requires( not IsVoid<B> )
		__Cx__ explicit RstPtr (Ptr<B> ptr)					__NE___ : _ptr{ptr.get()} {}

		template <typename B> requires( not IsVoid<B> and not IsConst<B> )
		__Cx__ explicit RstPtr (Ref<B> ref)					__NE___ : _ptr{&ref} {}

		template <typename B> requires( not IsConst<B> and IsSame< B, RemoveConst<T> >)
		__Cx__ RstPtr (RstPtr<B> other)						__NE___ : _ptr{other.get()} {}

		__Cx__ RstPtr (Self &&)								__NE___	= default;
		__Cx__ Self&	operator = (Self &&)				__NE___	= default;

		__Cx__ RstPtr (const Self &)						__NE___ = default;
		__Cx__ Self&	operator = (const Self &)			__NE___ = default;

		NdCx__ explicit operator bool ()					__NE___	{ return _ptr != null; }
		NdCx__ explicit operator bool ()					C_NE___	{ return _ptr != null; }

		NdCx__ Self		operator +  (Bytes offset)			C_NE___	{ return Self{ _ptr + offset }; }
		__Cx__ Self&	operator += (Bytes offset)			__NE___	{ _ptr += offset;  return *this; }

		template <typename B = T> requires( not IsVoid<B> )
		NdCx__ Self		operator +  (usize offset)			C_NE___	{ return Self{ _ptr + offset }; }	// offset in elements

		//template <typename B=T> requires( not IsVoid<B> and not IsConst<B> )
		//NdCx__ T&		operator [] (usize idx)				__NE___	{ return _ptr[idx]; }

		//template <typename B=T> requires( not IsVoid<B> )
		//NdCx__ T const&	operator [] (usize idx)				C_NE___	{ return _ptr[idx]; }


	  #if defined(AE_COMPILER_MSVC)

		ND_ __declspec(restrict) operator T* ()				__NE___	{ return _ptr; }
		ND_ __declspec(restrict) operator T const* ()		C_NE___	{ return _ptr; }

		ND_ __declspec(restrict) T*			get ()			__NE___	{ return _ptr; }
		ND_ __declspec(restrict) T const*	get ()			C_NE___	{ return _ptr; }


	  #elif defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)

		ND_ operator T* __restrict__ ()						__NE___	{ return _ptr; }
		ND_ operator T const* __restrict__ ()				C_NE___	{ return _ptr; }

		ND_ T* __restrict__					get ()			__NE___	{ return _ptr; }
		ND_ T const* __restrict__			get ()			C_NE___	{ return _ptr; }


	  #else
		ND_ operator T* ()									__NE___	{ return _ptr; }
		ND_ operator T const* ()							C_NE___	{ return _ptr; }

		ND_ T*								get ()			__NE___	{ return _ptr; }
		ND_ T const*						get ()			C_NE___	{ return _ptr; }

	  #endif

		__Cx__ friend void  Swap (Self &lhs, Self &rhs)		__NE___	{ std::swap( lhs._ptr, rhs._ptr ); }
	};


} // AE::Base
