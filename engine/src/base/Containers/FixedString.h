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
		constexpr TFixedString ()
		{
			DEBUG_ONLY( DbgInitMem( _array, Bytes::SizeOf(_array) ));
		}
		
		TFixedString (const View_t &view) : TFixedString{ view.data(), view.length() }
		{}
		
		constexpr TFixedString (const CharT *str)
		{
			for (; str[_length] and _length < StringSize; ++_length) {
				_array[_length] = str[_length];
			}
			_array[_length] = CharT{0};
		}

		constexpr TFixedString (const CharT *str, usize length)
		{
			ASSERT( length < StringSize );
			
			for (; _length < length and _length < StringSize; ++_length) {
				_array[_length] = str[_length];
			}
			_array[_length] = CharT{0};
		}


		ND_ constexpr operator View_t ()					const	{ return View_t{ data(), length() }; }

		ND_ constexpr usize			size ()					const	{ return _length; }
		ND_ constexpr usize			length ()				const	{ return size(); }
		ND_ static constexpr usize	capacity ()						{ return StringSize; }
		ND_ constexpr bool			empty ()				const	{ return _length == 0; }
		ND_ constexpr CharT const *	c_str ()				const	{ return _array; }
		ND_ constexpr CharT const *	data ()					const	{ return _array; }
		ND_ CharT *					data ()							{ return _array; }

		ND_ constexpr CharT &		operator [] (usize i)			{ ASSERT( i < _length );  return _array[i]; }
		ND_ constexpr CharT const &	operator [] (usize i)	const	{ ASSERT( i < _length );  return _array[i]; }

		ND_ constexpr bool	operator == (const View_t &rhs)	const	{ return View_t(*this) == rhs; }
		ND_ constexpr bool	operator != (const View_t &rhs)	const	{ return not (*this == rhs); }
		ND_ constexpr bool	operator >  (const View_t &rhs)	const	{ return View_t(*this) > rhs; }
		ND_ constexpr bool	operator <  (const View_t &rhs)	const	{ return View_t(*this) < rhs; }

		ND_ iterator		begin ()								{ return &_array[0]; }
		ND_ const_iterator	begin ()						const	{ return &_array[0]; }
		ND_ iterator		end ()									{ return &_array[_length]; }
		ND_ const_iterator	end ()							const	{ return &_array[_length]; }


		void clear ()
		{
			_array[0]	= CharT(0);
			_length		= 0;
		}

		void resize (usize newSize)
		{
			ASSERT( newSize < capacity() );

			newSize = Min( newSize, capacity()-1 );

			if ( newSize > _length )
			{
				std::memset( &_array[_length], 0, newSize - _length + 1 );
			}

			_length = Length_t(newSize);
		}

		void push_back (CharT value)
		{
			ASSERT( _length + 1 < capacity() );

			_array[_length]		= value;
			_array[_length + 1]	= CharT{0};
			++_length;
		}
	};


	template <usize StringSize>
	using FixedString = TFixedString< char, StringSize >;

}	// AE::Base


namespace std
{

	template <typename CharT, size_t StringSize>
	struct hash< AE::Base::TFixedString< CharT, StringSize > >
	{
		ND_ size_t  operator () (const AE::Base::TFixedString<CharT, StringSize> &value) const
		{
			return hash< AE::Base::BasicStringView<CharT> >()( value );
		}
	};

}	// std
