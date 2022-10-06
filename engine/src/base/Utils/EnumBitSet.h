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
		using Index_t	= ToUnsignedInteger<E>;
		using Elem_t	= Conditional< (_ElemSize > 32), ulong, uint >;
		using BitArr_t	= StaticArray< Elem_t, _ArraySize >;


		STATIC_ASSERT( sizeof(E) == sizeof(Index_t) );
		STATIC_ASSERT( _BitCount <= _ElemSize * _ArraySize );
		STATIC_ASSERT( _BitCount > 0 );
		STATIC_ASSERT( _BitCount <= sizeof(BitArr_t)*8 );


	// variables
	private:
		BitArr_t	_bits = {};


	// methods
	public:
		static constexpr Elem_t	_LastElemMask = ~((~Elem_t{0}) << (_BitCount % _ElemSize));

		constexpr EnumBitSet () {}
		constexpr EnumBitSet (const Self &) = default;

			constexpr Self&  operator = (const Self &) = default;
			
			constexpr Self&  set (E value, bool bit);
			constexpr Self&  insert (E value);
			constexpr Self&  InsertRange (E first, E last);

			constexpr Self&  erase (E value);
			constexpr Self&  EraseRange (E first, E last);

			constexpr Self&  clear ()				{ _bits.fill(0);  return *this; }
			constexpr Self&  SetAll ()				{ _bits.fill(UMax);  return *this; }

		ND_ constexpr bool  contains (E value) const;

		ND_ constexpr bool  All () const;
		ND_ constexpr bool  Any () const;
		
		ND_ constexpr bool  Any (const Self &rhs) const;
		ND_ constexpr bool  All (const Self &rhs) const;

		ND_ constexpr bool  None ()		const	{ return not Any(); }
		ND_ constexpr usize size ()		const	{ return _BitCount; }

		ND_ constexpr bool  AnyInRange (E first, E last) const;
		ND_ constexpr bool  AllInRange (E first, E last) const;

			constexpr Self&  operator |= (const Self &rhs);
			constexpr Self&  operator &= (const Self &rhs);

		ND_ constexpr Self  operator ~  () const;

		ND_ constexpr Self  operator |  (const Self &rhs)	const	{ return Self{*this} |= rhs; }
		ND_ constexpr Self  operator &  (const Self &rhs)	const	{ return Self{*this} &= rhs; }

		ND_ constexpr bool  operator == (const Self &rhs)	const;
		ND_ constexpr bool  operator != (const Self &rhs)	const	{ return not (*this == rhs); }
		ND_ constexpr bool  operator >  (const Self &rhs)	const;
		ND_ constexpr bool  operator <  (const Self &rhs)	const;
		ND_ constexpr bool  operator >= (const Self &rhs)	const	{ return not (*this < rhs); }
		ND_ constexpr bool  operator <= (const Self &rhs)	const	{ return not (*this > rhs); }

		ND_ constexpr BitArr_t const&  ToArray () const	{ return _bits; }

		ND_ HashVal CalcHash ()	const;
	};
	
	
/*
=================================================
	insert
=================================================
*/
	template <typename E>
	constexpr EnumBitSet<E>&  EnumBitSet<E>::insert (E value)
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
	constexpr EnumBitSet<E>&  EnumBitSet<E>::set (E value, bool bit)
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
	constexpr EnumBitSet<E>&  EnumBitSet<E>::erase (E value)
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
	constexpr bool  EnumBitSet<E>::contains (E value) const
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
	constexpr EnumBitSet<E>&  EnumBitSet<E>::InsertRange (E first, E last)
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
	constexpr EnumBitSet<E>&  EnumBitSet<E>::EraseRange (E first, E last)
	{
		ASSERT( first <= last );
		ASSERT( Index_t(last) < size() );

		const Index_t	first_idx	= Index_t(first) / _ElemSize;
		const Index_t	last_idx	= Index_t(last)  / _ElemSize;
			
		for_likely (Index_t i = first_idx; i <= last_idx; ++i)
		{
			const Index_t	min_val	= Max( Index_t(first), i *_ElemSize );
			const Index_t	count	= Index_t(last) - min_val + 1;

			_bits[i] &= ~ToBitMask<Elem_t>( min_val - (i *_ElemSize), count );
		}
		return *this;
	}

/*
=================================================
	operator |=
=================================================
*/
	template <typename E>
	constexpr EnumBitSet<E>&  EnumBitSet<E>::operator |= (const Self &rhs)
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
	constexpr EnumBitSet<E>&  EnumBitSet<E>::operator &= (const Self &rhs)
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
	constexpr EnumBitSet<E>  EnumBitSet<E>::operator ~ () const
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
	constexpr bool  EnumBitSet<E>::All () const
	{
		usize	accum = 0;
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			accum += BitCount( _bits[i] );
		}
		accum += BitCount( _bits.back() & _LastElemMask );

		return accum == _BitCount;
	}
	
	template <typename E>
	constexpr bool  EnumBitSet<E>::All (const Self &rhs) const
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
	constexpr bool  EnumBitSet<E>::Any () const
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
	constexpr bool  EnumBitSet<E>::Any (const Self &rhs) const
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
	constexpr bool  EnumBitSet<E>::AnyInRange (E first, E last) const
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
	constexpr bool  EnumBitSet<E>::AllInRange (E first, E last) const
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

			accum += BitCount( _bits[i] & ToBitMask<Elem_t>( min_val - (i *_ElemSize), count ));
		}
		return accum == (Index_t(last) - Index_t(first) + 1);
	}
	
/*
=================================================
	operator ==
=================================================
*/
	template <typename E>
	constexpr bool  EnumBitSet<E>::operator == (const Self &rhs) const
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
	constexpr bool  EnumBitSet<E>::operator > (const Self &rhs) const
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
	constexpr bool  EnumBitSet<E>::operator < (const Self &rhs) const
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
	CalcHash
=================================================
*/
	template <typename E>
	HashVal  EnumBitSet<E>::CalcHash () const
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
