// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/BitMath.h"
#include "base/Math/Vec.h"

namespace AE::Base
{

	//
	// Enum Bit Set
	//

	template <typename E>
	struct EnumBitSet
	{
	// types
	private:
		static constexpr uint	_BitCount	= uint(E::_Count);
		static constexpr uint	_ElemSize	= _BitCount <= 32 ?	32 :
											  _BitCount <= 64 ?	64 :
																(_BitCount % 32) < (_BitCount % 64) ? 32 : 64;
		static constexpr uint	_ArraySize	= (_BitCount + _ElemSize - 1) / _ElemSize;

		using Self		= EnumBitSet<E>;
		using Elem_t	= Conditional< (_ElemSize > 32), ulong, uint >;
		using BitArr_t	= StaticArray< Elem_t, _ArraySize >;
		using Index_t	= ByteSizeToUInt< Max( sizeof(E), sizeof(usize) )>;

		STATIC_ASSERT( sizeof(E) <= sizeof(Index_t) );
		STATIC_ASSERT( _BitCount <= _ElemSize * _ArraySize );
		STATIC_ASSERT( _BitCount > 0 );
		STATIC_ASSERT( _BitCount <= sizeof(BitArr_t)*8 );


	// variables
	private:
		BitArr_t	_bits = {};

		//   [0]     [1]     [2]
		// [|||||] [|||||] [||....]
		//                    ^-- _LastElemMask


	// methods
	public:
		static constexpr Elem_t	_LastElemMask = ~((~Elem_t{0}) << (_BitCount % _ElemSize));

		constexpr EnumBitSet ()											__NE___ {}
		constexpr EnumBitSet (const Self &)								__NE___ = default;
		constexpr EnumBitSet (Base::_hidden_::DefaultType)				__NE___ {}
		constexpr EnumBitSet (std::initializer_list<E> list)			__NE___	{ for (auto arg : list) { insert( arg ); }}

			constexpr Self&  operator = (const Self &)					__NE___ = default;
			constexpr Self&  operator = (Base::_hidden_::DefaultType)	__NE___	{ clear();  return *this; }
			
			constexpr Self&  set (E value, bool bit)					__NE___;
			constexpr Self&  insert (E value)							__NE___;
			constexpr Self&  InsertRange (E first, E last)				__NE___;

			constexpr Self&  erase (E value)							__NE___;
			constexpr Self&  EraseRange (E first, E last)				__NE___;

			constexpr Self&  clear ()									__NE___	{ _bits.fill(0);  return *this; }
			constexpr Self&  SetAll ()									__NE___	{ _bits.fill(UMax);  return *this; }

		ND_ constexpr bool  contains (E value)							C_NE___;

		ND_ constexpr bool  All ()										C_NE___	{ return BitCount() == _BitCount; }
		ND_ constexpr bool  Any ()										C_NE___;
		
		ND_ constexpr bool  Any (const Self &rhs)						C_NE___;
		ND_ constexpr bool  All (const Self &rhs)						C_NE___;

		ND_ constexpr bool  None ()										C_NE___	{ return not Any(); }
		ND_ constexpr usize size ()										C_NE___	{ return _BitCount; }

		ND_ constexpr bool  AnyInRange (E first, E last)				C_NE___;
		ND_ constexpr bool  AllInRange (E first, E last)				C_NE___;
		
			constexpr Self&  operator |= (E rhs)						__NE___	{ return insert( rhs ); }
			constexpr Self&  operator &= (E rhs)						__NE___	{ return erase( rhs ); }

			constexpr Self&  operator |= (const Self &rhs)				__NE___;
			constexpr Self&  operator &= (const Self &rhs)				__NE___;

		ND_ constexpr Self  operator ~  ()								C_NE___;

		ND_ constexpr Self  operator |  (const Self &rhs)				C_NE___	{ return Self{*this} |= rhs; }
		ND_ constexpr Self  operator &  (const Self &rhs)				C_NE___	{ return Self{*this} &= rhs; }

		ND_ constexpr bool  operator == (const Self &rhs)				C_NE___;
		ND_ constexpr bool  operator != (const Self &rhs)				C_NE___	{ return not (*this == rhs); }
		ND_ constexpr bool  operator >  (const Self &rhs)				C_NE___;
		ND_ constexpr bool  operator <  (const Self &rhs)				C_NE___;
		ND_ constexpr bool  operator >= (const Self &rhs)				C_NE___	{ return not (*this < rhs); }
		ND_ constexpr bool  operator <= (const Self &rhs)				C_NE___	{ return not (*this > rhs); }

		ND_ constexpr BitArr_t const&  ToArray ()						C_NE___	{ return _bits; }

		ND_ constexpr usize		BitCount ()								C_NE___;
		ND_ constexpr usize		ZeroCount ()							C_NE___	{ return size() - BitCount(); }

		ND_ constexpr E			ExtractFirst ()							__NE___;
		ND_ constexpr E			First ()								C_NE___;	// first non-zero bit
		ND_ constexpr E			Last ()									C_NE___;	// last non-zero bit

		ND_ HashVal  CalcHash ()										C_NE___;
	};


/*
=================================================
	insert
=================================================
*/
	template <typename E>
	constexpr EnumBitSet<E>&  EnumBitSet<E>::insert (E value) __NE___
	{
		ASSERT( Index_t(value) < size() );
		_bits[ Index_t(value) / _ElemSize ] |= (Elem_t{1} << (Index_t(value) % _ElemSize));
		return *this;
	}
	
/*
=================================================
	set
=================================================
*/
	template <typename E>
	constexpr EnumBitSet<E>&  EnumBitSet<E>::set (E value, bool bit) __NE___
	{
		ASSERT( Index_t(value) < size() );
		_bits[ Index_t(value) / _ElemSize ] |= (Elem_t{bit} << (Index_t(value) % _ElemSize));
		return *this;
	}

/*
=================================================
	erase
=================================================
*/
	template <typename E>
	constexpr EnumBitSet<E>&  EnumBitSet<E>::erase (E value) __NE___
	{
		ASSERT( Index_t(value) < size() );
		_bits[ Index_t(value) / _ElemSize ] &= ~(Elem_t{1} << (Index_t(value) % _ElemSize));
		return *this;
	}
	
/*
=================================================
	contains
=================================================
*/
	template <typename E>
	constexpr bool  EnumBitSet<E>::contains (E value) C_NE___
	{
		ASSERT( Index_t(value) < size() );
		auto	bit = (_bits[ Index_t(value) / _ElemSize ] & (Elem_t{1} << (Index_t(value) % _ElemSize)));
		return bit != 0;
	}
		
/*
=================================================
	InsertRange
=================================================
*/
	template <typename E>
	constexpr EnumBitSet<E>&  EnumBitSet<E>::InsertRange (E first, E last) __NE___
	{
		ASSERT( first <= last );
		ASSERT( Index_t(last) < size() );

		const Index_t	first_idx	= Index_t(first) / _ElemSize;
		const Index_t	last_idx	= Index_t(last)  / _ElemSize;
			
		for_likely (Index_t i = first_idx; i <= last_idx; ++i)
		{
			const Index_t	min_val	= Max( Index_t(first), i *_ElemSize );
			const Index_t	count	= Index_t(last) - min_val + 1;

			_bits[i] |= ToBitMask<Elem_t>( min_val - (i *_ElemSize), count );
		}
		return *this;
	}
	
/*
=================================================
	EraseRange
=================================================
*/
	template <typename E>
	constexpr EnumBitSet<E>&  EnumBitSet<E>::EraseRange (E first, E last) __NE___
	{
		ASSERT( first <= last );
		ASSERT( Index_t(last) < size() );

		const Index_t	first_idx	= Index_t(first) / _ElemSize;
		const Index_t	last_idx	= Index_t(last)  / _ElemSize;
			
		for_likely (Index_t i = first_idx; i <= last_idx; ++i)
		{
			const Index_t	min_val	= Max( Index_t(first), i * _ElemSize );
			const Index_t	count	= Index_t(last) - min_val + 1;

			_bits[i] &= ~ToBitMask<Elem_t>( min_val - (i * _ElemSize), count );
		}
		return *this;
	}

/*
=================================================
	operator |=
=================================================
*/
	template <typename E>
	constexpr EnumBitSet<E>&  EnumBitSet<E>::operator |= (const Self &rhs) __NE___
	{
		for (uint i = 0; i < _ArraySize; ++i) {
			_bits[i] |= rhs._bits[i];
		}
		return *this;
	}
		
/*
=================================================
	operator &=
=================================================
*/
	template <typename E>
	constexpr EnumBitSet<E>&  EnumBitSet<E>::operator &= (const Self &rhs) __NE___
	{
		for (uint i = 0; i < _ArraySize; ++i) {
			_bits[i] &= rhs._bits[i];
		}
		return *this;
	}
	
/*
=================================================
	operator ~
=================================================
*/
	template <typename E>
	constexpr EnumBitSet<E>  EnumBitSet<E>::operator ~ () C_NE___
	{
		Self	res;
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			res._bits[i] = ~_bits[i]; 
		}
		res._bits.back() = (~_bits.back()) & _LastElemMask;
		return res;
	}

/*
=================================================
	All
=================================================
*/
	template <typename E>
	constexpr bool  EnumBitSet<E>::All (const Self &rhs) C_NE___
	{
		bool	equal = true;
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			equal &= ((_bits[i] & rhs._bits[i]) == rhs._bits[i]);
		}

		equal &= (((_bits.back() & rhs._bits.back()) & _LastElemMask) == rhs._bits.back());
		return equal;
	}

/*
=================================================
	Any
=================================================
*/
	template <typename E>
	constexpr bool  EnumBitSet<E>::Any () C_NE___
	{
		Elem_t	accum = 0;
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			accum |= _bits[i];
		}
		accum |= _bits.back() & _LastElemMask;

		return accum != 0;
	}
	
	template <typename E>
	constexpr bool  EnumBitSet<E>::Any (const Self &rhs) C_NE___
	{
		bool	equal = true;
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			equal &= ((_bits[i] & rhs._bits[i]) != 0);
		}

		equal &= (((_bits.back() & rhs._bits.back()) & _LastElemMask) != 0);
		return equal;
	}

/*
=================================================
	AnyInRange
=================================================
*/
	template <typename E>
	constexpr bool  EnumBitSet<E>::AnyInRange (E first, E last) C_NE___
	{
		ASSERT( first <= last );
		ASSERT( Index_t(last) < size() );

		const Index_t	first_idx	= Index_t(first) / _ElemSize;
		const Index_t	last_idx	= Index_t(last)  / _ElemSize;
			
		Elem_t	accum = 0;
		for_likely (Index_t i = first_idx; i <= last_idx; ++i)
		{
			const Index_t	min_val	= Max( Index_t(first), i *_ElemSize );
			const Index_t	count	= Index_t(last) - min_val + 1;

			accum |= (_bits[i] & ToBitMask<Elem_t>( min_val - (i *_ElemSize), count ));
		}
		return accum != 0;
	}
		
/*
=================================================
	AllInRange
=================================================
*/
	template <typename E>
	constexpr bool  EnumBitSet<E>::AllInRange (E first, E last) C_NE___
	{
		ASSERT( first <= last );
		ASSERT( Index_t(last) < size() );

		const Index_t	first_idx	= Index_t(first) / _ElemSize;
		const Index_t	last_idx	= Index_t(last)  / _ElemSize;
			
		usize	accum = 0;
		for_likely (Index_t i = first_idx; i <= last_idx; ++i)
		{
			const Index_t	min_val	= Max( Index_t(first), i *_ElemSize );
			const Index_t	count	= Index_t(last) - min_val + 1;

			accum += Math::BitCount( _bits[i] & ToBitMask<Elem_t>( min_val - (i *_ElemSize), count ));
		}
		return accum == (Index_t(last) - Index_t(first) + 1);
	}
	
/*
=================================================
	operator ==
=================================================
*/
	template <typename E>
	constexpr bool  EnumBitSet<E>::operator == (const Self &rhs) C_NE___
	{
		uint	accum = 0;
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			accum += uint(_bits[i] == rhs._bits[i]);
		}
		accum += uint((_bits.back() & _LastElemMask) == (rhs._bits.back() & _LastElemMask));

		return accum == _ArraySize;
	}
	
/*
=================================================
	operator >
=================================================
*/
	template <typename E>
	constexpr bool  EnumBitSet<E>::operator > (const Self &rhs) C_NE___
	{
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			if_unlikely( _bits[i] != rhs._bits[i] )
				return _bits[i] > rhs._bits[i];
		}
		return (_bits.back() & _LastElemMask) > (rhs._bits.back() & _LastElemMask);
	}
	
/*
=================================================
	operator <
=================================================
*/
	template <typename E>
	constexpr bool  EnumBitSet<E>::operator < (const Self &rhs) C_NE___
	{
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			if_unlikely( _bits[i] != rhs._bits[i] )
				return _bits[i] < rhs._bits[i];
		}
		return (_bits.back() & _LastElemMask) < (rhs._bits.back() & _LastElemMask);
	}
	
/*
=================================================
	BitCount
=================================================
*/
	template <typename E>
	constexpr usize  EnumBitSet<E>::BitCount () C_NE___
	{
		usize	cnt = 0;
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			cnt += Math::BitCount( _bits[i] );
		}
		cnt += Math::BitCount( _bits.back() & _LastElemMask );
		ASSERT( cnt <= size() );
		return cnt;
	}

/*
=================================================
	ExtractFirst
=================================================
*/
	template <typename E>
	constexpr E  EnumBitSet<E>::ExtractFirst () __NE___
	{
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			if ( _bits[i] != Default )
				return E( ExtractBitLog2( INOUT _bits[i] ) + i * _ElemSize );
		}

		if ( (_bits.back() & _LastElemMask) != Default )
			return E( ExtractBitLog2( INOUT _bits.back() ) + (_ArraySize - 1) * _ElemSize );

		return E(size());
	}
	
/*
=================================================
	First
=================================================
*/
	template <typename E>
	constexpr E  EnumBitSet<E>::First () C_NE___
	{
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			if ( _bits[i] != Default )
				return E( BitScanReverse( INOUT _bits[i] ) + i * _ElemSize );
		}

		if ( (_bits.back() & _LastElemMask) != Default )
			return E( BitScanReverse( INOUT _bits.back() ) + (_ArraySize - 1) * _ElemSize );

		return E(size());
	}
	
/*
=================================================
	Last
=================================================
*/
	template <typename E>
	constexpr E  EnumBitSet<E>::Last () C_NE___
	{
		if ( (_bits.back() & _LastElemMask) != Default )
			return E( BitScanReverse( INOUT _bits.back() ) + (_ArraySize - 1) * _ElemSize );

		for (int i = _ArraySize - 1; i >= 0; --i)
		{
			if ( _bits[i] != Default )
				return E( BitScanReverse( INOUT _bits[i] ) + i * _ElemSize );
		}

		return E(size());
	}

/*
=================================================
	CalcHash
=================================================
*/
	template <typename E>
	HashVal  EnumBitSet<E>::CalcHash () C_NE___
	{
		HashVal	h;
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			h << HashOf( _bits[i] );
		}
		h << HashOf( _bits.back() & _LastElemMask );

		return h;
	}

} // AE::Base


namespace std
{
	template <typename E>
	struct hash< AE::Base::EnumBitSet<E> > :
		AE::Base::DefaultHasher_CalcHash< AE::Base::EnumBitSet<E> >
	{};

} // std
