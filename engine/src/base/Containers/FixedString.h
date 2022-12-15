// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Memory/MemUtils.h"

namespace AE::Base
{

	//
	// Fixed Size String
	//

	template <typename CharT, usize StringSize>
	struct TFixedString
	{
		STATIC_ASSERT( StringSize <= 512 );

	// type
	public:
		using value_type		= CharT;
		using iterator			= CharT *;
		using const_iterator	= CharT const *;
		using View_t			= BasicStringView< CharT >;
		using Self				= TFixedString< CharT, StringSize >;
		
	private:
		using Length_t			= Conditional< alignof(CharT) < 4, ubyte, uint >;


	// variables
	private:
		Length_t	_length	= 0;
		CharT		_array [StringSize] = {};


	// methods
	public:
		constexpr TFixedString ()							__NE___	{ DEBUG_ONLY( DbgInitMem( _array, Sizeof(_array) )); }
		
		constexpr TFixedString (const View_t &view)			__NE___ : TFixedString{ view.data(), view.length() } {}
		
		constexpr TFixedString (const CharT *str)			__NE___
		{
			for (; str[_length] and _length < StringSize; ++_length) {
				_array[_length] = str[_length];
			}
			_array[_length] = CharT{0};
		}

		constexpr TFixedString (const CharT *str, usize length) __NE___
		{
			ASSERT( length < StringSize );
			
			for (; _length < length and _length < StringSize; ++_length) {
				_array[_length] = str[_length];
			}
			_array[_length] = CharT{0};
		}

		constexpr TFixedString (Self &&)					__NE___ = default;
		constexpr TFixedString (const Self &)				__NE___ = default;

		constexpr ~TFixedString ()							__NE___	= default;
		
		Self&  operator = (Self &&)							__NE___ = default;
		Self&  operator = (const Self &)					__NE___ = default;

		ND_ constexpr operator View_t ()					C_NE___	{ return View_t{ data(), length() }; }

		ND_ constexpr usize			size ()					C_NE___	{ return _length; }
		ND_ constexpr usize			length ()				C_NE___	{ return size(); }
		ND_ static constexpr usize	capacity ()				__NE___	{ return StringSize; }
		ND_ constexpr bool			empty ()				C_NE___	{ return _length == 0; }
		ND_ constexpr CharT const *	c_str ()				C_NE___	{ return _array; }			// always non-null
		ND_ constexpr CharT const *	data ()					C_NE___	{ return _array; }
		ND_ CharT *					data ()					__NE___	{ return _array; }

		ND_ constexpr CharT &		operator [] (usize i)	__NE___	{ ASSERT( i < _length );  return _array[i]; }
		ND_ constexpr CharT const &	operator [] (usize i)	C_NE___	{ ASSERT( i < _length );  return _array[i]; }

		ND_ constexpr bool	operator == (const View_t &rhs)	C_NE___	{ return View_t(*this) == rhs; }
		ND_ constexpr bool	operator != (const View_t &rhs)	C_NE___	{ return not (*this == rhs); }
		ND_ constexpr bool	operator >  (const View_t &rhs)	C_NE___	{ return View_t(*this) > rhs; }
		ND_ constexpr bool	operator <  (const View_t &rhs)	C_NE___	{ return View_t(*this) < rhs; }

		ND_ iterator		begin ()						__NE___	{ return &_array[0]; }
		ND_ const_iterator	begin ()						C_NE___	{ return &_array[0]; }
		ND_ iterator		end ()							__NE___	{ return &_array[_length]; }
		ND_ const_iterator	end ()							C_NE___	{ return &_array[_length]; }


		void  clear ()										__NE___
		{
			_array[0]	= CharT(0);
			_length		= 0;
		}

		void  resize (usize newSize)						__NE___
		{
			ASSERT( newSize < capacity() );

			newSize = Min( newSize, capacity()-1 );

			if ( newSize > _length )
			{
				std::memset( &_array[_length], 0, newSize - _length + 1 );
			}

			_length = Length_t(newSize);
		}

		void  push_back (CharT value)						__NE___
		{
			ASSERT( _length + 1 < capacity() );

			_array[_length]		= value;
			_array[_length + 1]	= CharT{0};
			++_length;
		}
	};


	template <usize StringSize>
	using FixedString = TFixedString< char, StringSize >;

} // AE::Base


namespace std
{

	template <typename CharT, size_t StringSize>
	struct hash< AE::Base::TFixedString< CharT, StringSize > >
	{
		ND_ size_t  operator () (const AE::Base::TFixedString<CharT, StringSize> &value) C_NE___
		{
			return hash< AE::Base::BasicStringView<CharT> >()( value );
		}
	};

} // std
