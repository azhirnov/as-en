// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Memory/MemUtils.h"
#include "base/Utils/Helpers.h"

namespace AE::Base
{

	//
	// Fixed Capacity String
	//

	template <typename CharT,
			  usize StringSize
			 >
	struct TFixedString : NothrowAllocatable
	{
		StaticAssert( StringSize <= 512 );

	// type
	public:
		using value_type		= CharT;
		using iterator			= CharT *;
		using const_iterator	= CharT const *;
		using View_t			= BasicStringView< CharT >;
		using Self				= TFixedString< CharT, StringSize >;

	private:
		using Length_t			= Conditional< alignof(CharT) == 1 and StringSize <= 256, ubyte, ushort >;


	// variables
	private:
		Length_t	_length				= 0;
		CharT		_array [StringSize]	= {};


	// methods
	public:
		__Cx__ TFixedString ()								__NE___	= default;
		__Cx__ TFixedString (const View_t &view)			__NE___ : TFixedString{ view.data(), view.length() } {}
		__Cx__ TFixedString (const CharT* str)				__NE___;
		__Cz__ TFixedString (const CharT* str, usize length)__NE___;

		__Cx__ TFixedString (Self &&)						__NE___ = default;
		__Cx__ TFixedString (const Self &)					__NE___ = default;

		__Cx__ Self&  operator = (Self &&)					__NE___ = default;
		__Cx__ Self&  operator = (const Self &)				__NE___ = default;

		NdCx__ operator View_t ()							C_NE___	{ return View_t{ data(), length() }; }
		NdCx__ explicit operator BasicString<CharT> ()		C_NE___	{ return BasicString<CharT>{ data(), length() }; }

		NdCx__ usize		size ()							C_NE___	{ return _length; }
		NdCx__ usize		length ()						C_NE___	{ return size(); }
		NdCx__ static usize	capacity ()						__NE___	{ return StringSize; }
		NdCx__ bool			empty ()						C_NE___	{ return _length == 0; }
		NdCx__ CharT const*	c_str ()						C_NE___	{ return _array; }			// always non-null
		NdCx__ CharT const*	data ()							C_NE___	{ return _array; }
		NdCx__ CharT *		data ()							__NE___	{ return _array; }

		NdCz__ CharT &		operator [] (usize i)			__NE___	{ ASSERT( i < _length );  return _array[i]; }
		NdCz__ CharT const&	operator [] (usize i)			C_NE___	{ ASSERT( i < _length );  return _array[i]; }

		NdCx__ bool	operator == (const View_t &rhs)			C_NE___	{ return View_t(*this) == rhs; }
		NdCx__ bool	operator != (const View_t &rhs)			C_NE___	{ return not (*this == rhs); }
		NdCx__ bool	operator >  (const View_t &rhs)			C_NE___	{ return View_t(*this) > rhs; }
		NdCx__ bool	operator <  (const View_t &rhs)			C_NE___	{ return View_t(*this) < rhs; }

		__Cx__ Self&	operator << (CharT value)			__NE___	{ push_back( value );  return *this; }

		NdCx__ iterator			begin ()					__NE___	{ return std::addressof(_array[0]); }
		NdCx__ const_iterator	begin ()					C_NE___	{ return std::addressof(_array[0]); }
		NdCx__ iterator			end ()						__NE___	{ return std::addressof(_array[_length]); }
		NdCx__ const_iterator	end ()						C_NE___	{ return std::addressof(_array[_length]); }

		__Cx__ void  clear ()								__NE___;
		__Cz__ void  resize (usize newSize)					__NE___;
		__Cz__ void  push_back (CharT value)				__NE___;
	};


	template <usize StringSize>
	using FixedString = TFixedString< char, StringSize >;



/*
=================================================
	constructor
=================================================
*/
	template <typename T, usize S>
	__Cx__ TFixedString<T,S>::TFixedString (const T* str) __NE___
	{
		for (; str[_length] and _length < capacity(); ++_length) {
			_array[_length] = str[_length];
		}
		_array[_length] = T{0};
	}

	template <typename T, usize S>
	__Cz__ TFixedString<T,S>::TFixedString (const T* str, usize length) __NE___
	{
		ASSERT( length < capacity() );

		for (; _length < length and _length < capacity(); ++_length) {
			_array[_length] = str[_length];
		}
		_array[_length] = T{0};
	}

/*
=================================================
	clear
=================================================
*/
	template <typename T, usize S>
	__Cx__ void  TFixedString<T,S>::clear () __NE___
	{
		_array[0]	= T(0);
		_length		= 0;
	}

/*
=================================================
	resize
=================================================
*/
	template <typename T, usize S>
	__Cz__ void  TFixedString<T,S>::resize (usize newSize) __NE___
	{
		ASSERT( newSize < capacity() );

		newSize = Min( newSize, capacity()-1 );

		if ( newSize > _length )
		{
			std::memset( std::addressof(_array[_length]), 0, newSize - _length + 1 );
		}

		_length = Length_t(newSize);
	}

/*
=================================================
	push_back
=================================================
*/
	template <typename T, usize S>
	__Cz__ void  TFixedString<T,S>::push_back (T value) __NE___
	{
		ASSERT( usize{_length} + 1u < capacity() );

		_array[_length]		= value;
		_array[_length + 1]	= T{0};
		++_length;
	}
//-----------------------------------------------------------------------------


	template <typename T, usize S>	struct TMemCopyAvailable< TFixedString<T,S> >	: CT_True {};
	template <typename T, usize S>	struct TZeroMemAvailable< TFixedString<T,S> >	: CT_True {};

} // AE::Base


template <typename CharT, size_t StringSize>
struct std::hash< AE::Base::TFixedString< CharT, StringSize > >
{
	ND_ size_t  operator () (const AE::Base::TFixedString<CharT, StringSize> &value) C_NE___
	{
		return hash< AE::Base::BasicStringView<CharT> >()( value );
	}
};
