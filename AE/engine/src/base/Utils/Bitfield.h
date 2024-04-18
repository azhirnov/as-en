// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Prefer to use 'Bitfield' instead of 'BitSet' because on different implementations
	'BitSet' has different size (min 8 bytes on clang and min 4 bytes on MSVC).
*/

#pragma once

#include "base/Math/BitMath.h"

namespace AE::Base
{

	//
	// Bitfield
	//

	template <typename T>
	struct Bitfield
	{
		StaticAssert( IsUnsignedInteger<T> );

	// types
	public:
		using Self		= Bitfield<T>;
		using Value_t	= T;

	private:
		static constexpr uint	_BitCount = CT_SizeOfInBits<T>;


	// variables
	private:
		T		_bits;


	// methods
	public:
		constexpr Bitfield ()										__NE___ : _bits{0} {}

		constexpr Bitfield (const Self &)							__NE___ = default;
		constexpr Bitfield (Self &&)								__NE___ = default;

		constexpr explicit Bitfield (T bits)						__NE___ : _bits{bits} {}

			constexpr Self&  operator = (const Self &)				__NE___ = default;
			constexpr Self&  operator = (Self &&)					__NE___ = default;


		// single bit //
			template <usize Bit>
			constexpr Self&		Set ()								__NE___	{ StaticAssert( Bit < _BitCount );  _bits |= T{1} << Bit;  return *this; }

			template <usize Bit>
			constexpr Self&		Erase ()							__NE___	{ StaticAssert( Bit < _BitCount );  _bits &= ~(T{1} << Bit);  return *this; }

			template <usize Bit>
		ND_ constexpr bool		Has ()								C_NE___	{ StaticAssert( Bit < _BitCount );  return !!(_bits & (T{1} << Bit)); }

			template <typename IT>
			constexpr Self&		Set (IT bit)						__NE___	{ ASSERT( bit >= 0 and bit < IT(_BitCount) );  _bits |= T{1} << bit;  return *this; }

			template <typename IT>
			constexpr Self&		Erase (IT bit)						__NE___	{ ASSERT( bit >= 0 and bit < IT(_BitCount) );  _bits &= ~(T{1} << bit);  return *this; }

			template <typename IT>
		ND_ constexpr bool		Has (IT bit)						C_NE___	{ ASSERT( bit >= 0 and bit < IT(_BitCount) );  return !!(_bits & (T{1} << bit)); }


		// bit range //
			template <usize Bit, usize Count>
			constexpr Self&		SetRange ()							__NE___;

			template <usize Bit, usize Count>
			constexpr Self&		EraseRange ()						__NE___;

			template <usize Bit, usize Count>
		ND_	constexpr bool		HasRange ()							C_NE___;

			template <typename IT>
			constexpr Self&		SetRange (IT first, IT count)		__NE___;

			template <typename IT>
			constexpr Self&		EraseRange (IT first, IT count)		__NE___;

			template <typename IT>
		ND_	constexpr bool		HasRange (IT first, IT count)		C_NE___;


		ND_ constexpr explicit	operator T ()						C_NE___	{ return _bits; }

		ND_ constexpr Self		operator ~  ()						C_NE___	{ return Self{ ~_bits }; }

			constexpr Self&		operator |= (Self rhs)				__NE___	{ _bits |= rhs._bits;  return *this; }
			constexpr Self&		operator &= (Self rhs)				__NE___	{ _bits &= rhs._bits;  return *this; }
			constexpr Self&		operator ^= (Self rhs)				__NE___	{ _bits ^= rhs._bits;  return *this; }

		ND_ constexpr Self		operator |  (Self rhs)				C_NE___	{ return Self{ _bits | rhs._bits }; }
		ND_ constexpr Self		operator &  (Self rhs)				C_NE___	{ return Self{ _bits & rhs._bits }; }
		ND_ constexpr Self		operator ^  (Self rhs)				C_NE___	{ return Self{ _bits ^ rhs._bits }; }

		ND_ constexpr bool		operator == (Self rhs)				C_NE___	{ return _bits == rhs._bits; }
		ND_ constexpr bool		operator != (Self rhs)				C_NE___	{ return _bits != rhs._bits; }

		ND_ constexpr bool		None ()								C_NE___	{ return ! _bits; }
		ND_ constexpr bool		Any ()								C_NE___	{ return !! _bits; }
		ND_ constexpr bool		All ()								C_NE___	{ return _bits == UMax; }

		ND_ constexpr T			Get ()								C_NE___	{ return _bits; }
		ND_ constexpr T &		Ref ()								__NE___	{ return _bits; }

		ND_ constexpr T			ExtractBit ()						__NE___;
		ND_ constexpr int		ExtractBitIndex ()					__NE___	{ return IntLog2( ExtractBit() ); }

		ND_ constexpr T			SetFirstZeroBit ()					__NE___;
		ND_ constexpr int		SetFirstZeroBitIndex ()				__NE___	{ return IntLog2( SetFirstZeroBit() ); }

		ND_ constexpr T			GetFirstZeroBit ()					C_NE___;
		ND_ constexpr int		GetFirstZeroBitIndex ()				C_NE___	{ return IntLog2( GetFirstZeroBit() ); }

		ND_ usize				BitCount ()							C_NE___	{ return Math::BitCount( _bits ); }

		ND_ HashVal				CalcHash ()							C_NE___;
	};




/*
=================================================
	ExtractBit
----
	extract lowest non-zero bit
=================================================
*/
	template <typename T>
	constexpr T  Bitfield<T>::ExtractBit () __NE___
	{
		T	result = _bits & ~(_bits - T{1});
		_bits &= ~result;
		return result;
	}

/*
=================================================
	SetFirstZeroBit / GetFirstZeroBit
----
	find lowest zero bit, set it to 1 and return bit
=================================================
*/
	template <typename T>
	constexpr T  Bitfield<T>::SetFirstZeroBit () __NE___
	{
		T	inv		= ~_bits;
		T	result	= inv & ~(inv - T{1});
		_bits |= result;
		return result;
	}

	template <typename T>
	constexpr T  Bitfield<T>::GetFirstZeroBit () C_NE___
	{
		T	inv		= ~_bits;
		T	result	= inv & ~(inv - T{1});
		return result;
	}

/*
=================================================
	SetRange / EraseRange
=================================================
*/
	template <typename T>
	template <usize Bit, usize Count>
	constexpr Bitfield<T>&  Bitfield<T>::SetRange () __NE___
	{
		StaticAssert( Bit < _BitCount );
		StaticAssert( Bit+Count <= _BitCount );

		_bits |= ((T{1} << Count)-1) << Bit;
		return *this;
	}

	template <typename T>
	template <usize Bit, usize Count>
	constexpr Bitfield<T>&  Bitfield<T>::EraseRange () __NE___
	{
		StaticAssert( Bit < _BitCount );
		StaticAssert( Bit+Count <= _BitCount );

		_bits &= ~(((T{1} << Count)-1) << Bit);
		return *this;
	}

	template <typename T>
	template <usize Bit, usize Count>
	constexpr bool  Bitfield<T>::HasRange () C_NE___
	{
		StaticAssert( Bit < _BitCount );
		StaticAssert( Bit+Count <= _BitCount );

		const T	mask = ((T{1} << Count)-1) << Bit;
		return (_bits & mask) == mask;
	}

	template <typename T>
	template <typename IT>
	constexpr Bitfield<T>&  Bitfield<T>::SetRange (IT first, IT count) __NE___
	{
		ASSERT( first < _BitCount );
		ASSERT( first+count <= _BitCount );

		_bits |= ((T{1} << count)-1) << first;
		return *this;
	}

	template <typename T>
	template <typename IT>
	constexpr Bitfield<T>&  Bitfield<T>::EraseRange (IT first, IT count) __NE___
	{
		ASSERT( first < _BitCount );
		ASSERT( first+count <= _BitCount );

		_bits &= ~(((T{1} << count)-1) << first);
		return *this;
	}

	template <typename T>
	template <typename IT>
	constexpr bool  Bitfield<T>::HasRange (IT first, IT count) C_NE___
	{
		ASSERT( first < _BitCount );
		ASSERT( first+count <= _BitCount );

		const T	mask = ((T{1} << count)-1) << first;
		return (_bits & mask) == mask;
	}

/*
=================================================
	CalcHash
=================================================
*/
	template <typename T>
	HashVal  Bitfield<T>::CalcHash () C_NE___
	{
		return HashOf( _bits );
	}


} // AE::Base


template <typename T>
struct std::hash< AE::Base::Bitfield<T> > :
	AE::Base::DefaultHasher_CalcHash< AE::Base::Bitfield<T> >
{};
