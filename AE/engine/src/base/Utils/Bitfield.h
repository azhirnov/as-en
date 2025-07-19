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
		__Cx__ Bitfield ()									__NE___ : _bits{0} {}

		__Cx__ Bitfield (const Self &)						__NE___ = default;
		__Cx__ Bitfield (Self &&)							__NE___ = default;

		__Cx__ explicit Bitfield (T bits)					__NE___ : _bits{bits} {}

		__Cx__ Self&  operator = (const Self &)				__NE___ = default;
		__Cx__ Self&  operator = (Self &&)					__NE___ = default;


		// single bit //
		template <usize Bit>
		__Cx__ Self&	Set ()								__NE___	{ StaticAssert( Bit < _BitCount );  _bits |= T{1} << Bit;  return *this; }

		template <usize Bit>
		__Cx__ Self&	Erase ()							__NE___	{ StaticAssert( Bit < _BitCount );  _bits &= ~(T{1} << Bit);  return *this; }

		template <usize Bit>
		NdCx__ bool		Has ()								C_NE___	{ StaticAssert( Bit < _BitCount );  return !!(_bits & (T{1} << Bit)); }

		template <typename IT> requires( IsInteger<IT> )
		__Cx__ Self&	Set (IT bit)						__NE___	{ ASSERT( bit >= 0 and bit < IT(_BitCount) );  _bits |= T{1} << bit;  return *this; }

		template <typename IT> requires( IsInteger<IT> )
		__Cx__ Self&	Erase (IT bit)						__NE___	{ ASSERT( bit >= 0 and bit < IT(_BitCount) );  _bits &= ~(T{1} << bit);  return *this; }

		template <typename IT> requires( IsInteger<IT> )
		NdCx__ bool		Has (IT bit)						C_NE___	{ ASSERT( bit >= 0 and bit < IT(_BitCount) );  return !!(_bits & (T{1} << bit)); }


		// bit range //
		template <usize Bit, usize Count>
		__Cx__ Self&	SetRange ()							__NE___;

		template <usize Bit, usize Count>
		__Cx__ Self&	EraseRange ()						__NE___;

		template <usize Bit, usize Count>
		NdCx__ bool		HasRange ()							C_NE___;

		template <typename IT> requires( IsInteger<IT> )
		__Cx__ Self&	SetRange (IT first, IT count)		__NE___;

		template <typename IT> requires( IsInteger<IT> )
		__Cx__ Self&	EraseRange (IT first, IT count)		__NE___;

		template <typename IT> requires( IsInteger<IT> )
		NdCx__ bool		HasRange (IT first, IT count)		C_NE___;


		NdCx__ explicit	operator T ()						C_NE___	{ return _bits; }

		NdCx__ Self		operator ~  ()						C_NE___	{ return Self{ ~_bits }; }

		__Cx__ Self&	operator |= (Self rhs)				__NE___	{ _bits |= rhs._bits;  return *this; }
		__Cx__ Self&	operator &= (Self rhs)				__NE___	{ _bits &= rhs._bits;  return *this; }
		__Cx__ Self&	operator ^= (Self rhs)				__NE___	{ _bits ^= rhs._bits;  return *this; }

		NdCx__ Self		operator |  (Self rhs)				C_NE___	{ return Self{ _bits | rhs._bits }; }
		NdCx__ Self		operator &  (Self rhs)				C_NE___	{ return Self{ _bits & rhs._bits }; }
		NdCx__ Self		operator ^  (Self rhs)				C_NE___	{ return Self{ _bits ^ rhs._bits }; }

		NdCx__ bool		operator == (Self rhs)				C_NE___	{ return _bits == rhs._bits; }
		NdCx__ bool		operator != (Self rhs)				C_NE___	{ return _bits != rhs._bits; }

		NdCx__ bool		None ()								C_NE___	{ return ! _bits; }
		NdCx__ bool		Any ()								C_NE___	{ return !! _bits; }
		NdCx__ bool		All ()								C_NE___	{ return _bits == UMax; }

		NdCx__ T		Get ()								C_NE___	{ return _bits; }
		NdCx__ T &		Ref ()								__NE___	{ return _bits; }

		// change first 1 bit to 0
		NdCx__ T		ExtractBit ()						__NE___;
		NdCx__ int		ExtractBitIndex ()					__NE___	{ return IntLog2( ExtractBit() ); }

		// returns first 1 bit
		NdCx__ T		GetFirstBit ()						C_NE___;
		NdCx__ int		GetFirstBitIndex ()					C_NE___	{ return IntLog2( GetFirstBit() ); }

		// change first 0 bit to 1
		NdCx__ T		SetFirstZeroBit ()					__NE___;
		NdCx__ int		SetFirstZeroBitIndex ()				__NE___	{ return IntLog2( SetFirstZeroBit() ); }

		// returns first 0 bit
		NdCx__ T		GetFirstZeroBit ()					C_NE___;
		NdCx__ int		GetFirstZeroBitIndex ()				C_NE___	{ return IntLog2( GetFirstZeroBit() ); }

		ND_ usize		BitCount ()							C_NE___	{ return Base::BitCount( _bits ); }
		ND_ usize		ZeroBitCount ()						C_NE___	{ return Base::BitCount( ~_bits ); }

		ND_ HashVal		CalcHash ()							C_NE___;

		NdCx__ static usize  Count ()						__NE___	{ return _BitCount; }
	};




/*
=================================================
	ExtractBit
----
	extract lowest non-zero bit
=================================================
*/
	template <typename T>
	__Cx__ T  Bitfield<T>::ExtractBit () __NE___
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
	__Cx__ T  Bitfield<T>::SetFirstZeroBit () __NE___
	{
		T	inv		= ~_bits;
		T	result	= inv & ~(inv - T{1});
		_bits |= result;
		return result;
	}

	template <typename T>
	__Cx__ T  Bitfield<T>::GetFirstZeroBit () C_NE___
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
	template <usize Bit, usize Cnt>
	__Cx__ Bitfield<T>&  Bitfield<T>::SetRange () __NE___
	{
		StaticAssert( Bit < _BitCount );
		StaticAssert( Bit+Cnt <= _BitCount );

		_bits |= ToBitMask<T>( Cnt ) << Bit;
		return *this;
	}

	template <typename T>
	template <usize Bit, usize Cnt>
	__Cx__ Bitfield<T>&  Bitfield<T>::EraseRange () __NE___
	{
		StaticAssert( Bit < _BitCount );
		StaticAssert( Bit+Cnt <= _BitCount );

		_bits &= ~(ToBitMask<T>( Cnt ) << Bit);
		return *this;
	}

	template <typename T>
	template <usize Bit, usize Cnt>
	__Cx__ bool  Bitfield<T>::HasRange () C_NE___
	{
		StaticAssert( Bit < _BitCount );
		StaticAssert( Bit+Cnt <= _BitCount );

		const T	mask = ToBitMask<T>( Cnt ) << Bit;
		return (_bits & mask) == mask;
	}

	template <typename T>
	template <typename IT> requires( IsInteger<IT> )
	__Cx__ Bitfield<T>&  Bitfield<T>::SetRange (IT first, IT count) __NE___
	{
		ASSERT( first < _BitCount );
		ASSERT( first+count <= _BitCount );

		_bits |= ToBitMask<T>( count ) << first;
		return *this;
	}

	template <typename T>
	template <typename IT> requires( IsInteger<IT> )
	__Cx__ Bitfield<T>&  Bitfield<T>::EraseRange (IT first, IT count) __NE___
	{
		ASSERT( first < _BitCount );
		ASSERT( first+count <= _BitCount );

		_bits &= ~(ToBitMask<T>( count ) << first);
		return *this;
	}

	template <typename T>
	template <typename IT> requires( IsInteger<IT> )
	__Cx__ bool  Bitfield<T>::HasRange (IT first, IT count) C_NE___
	{
		ASSERT( first < _BitCount );
		ASSERT( first+count <= _BitCount );

		const T	mask = ToBitMask<T>( count ) << first;
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
