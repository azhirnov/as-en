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
	struct EnumSet
	{
	// types
	private:
		static constexpr uint	_BitCount	= uint(E::_Count);
		static constexpr uint	_ElemSize	= _BitCount <=  8 ?	 8 :
											  _BitCount <= 16 ?	16 :
											  _BitCount <= 32 ?	32 :
											  _BitCount <= 64 ?	64 :
											  AlignUp( _BitCount, 32 ) < AlignUp( _BitCount, 64 ) ? 32 : 64;
		static constexpr uint	_ArraySize	= (_BitCount + _ElemSize - 1) / _ElemSize;

		using Self		= EnumSet<E>;
		using Elem_t	= BitSizeToUInt< _ElemSize >;
		using BitArr_t	= StaticArray< Elem_t, _ArraySize >;
		using Index_t	= ByteSizeToUInt< Max( sizeof(E), sizeof(usize) )>;

		StaticAssert( sizeof(E) <= sizeof(Index_t) );
		StaticAssert( _BitCount <= _ElemSize * _ArraySize );
		StaticAssert( _BitCount > 0 );
		StaticAssert( _BitCount <= CT_SizeOfInBits<BitArr_t> );

		StaticAssert( _BitCount <= 8*1024 );	// 1 Kb


		struct ConstIterator
		{
		// variables
		private:
			E				_idx;
			Self const&		_ref;

		// methods
		public:
			constexpr ConstIterator (E idx, Self const& ref)				__NE___ : _idx{idx}, _ref{ref} {}

			constexpr ConstIterator&	operator ++ ()						__NE___	{ _idx = _ref.Next( _idx );  return *this; }
			constexpr ConstIterator		operator ++ (int)					__NE___	{ ConstIterator res{ _idx, _ref };  this->operator++();  return res; }

			ND_ constexpr bool			operator == (ConstIterator rhs)		C_NE___	{ return _idx == rhs._idx; }
			ND_ constexpr bool			operator != (ConstIterator rhs)		C_NE___	{ return _idx != rhs._idx; }

			constexpr E					operator * ()						C_NE___	{ return _idx; }
		};

	public:
		using iterator			= ConstIterator;
		using const_iterator	= ConstIterator;


	// variables
	private:
		BitArr_t	_bits = {};

		//   [0]     [1]     [2]
		// [|||||] [|||||] [||....]
		//                    ^-- _LastElemMask

		static constexpr Elem_t	_LastElemMask = ~((~Elem_t{0}) << (_BitCount % _ElemSize));


	// methods
	public:
		constexpr EnumSet ()												__NE___ {}
		constexpr EnumSet (const Self &)									__NE___ = default;
		constexpr EnumSet (Base::_hidden_::DefaultType)						__NE___ {}
		constexpr EnumSet (std::initializer_list<E> list)					__NE___	{ for (auto arg : list) { insert( arg ); }}

			constexpr Self&		operator = (const Self &)					__NE___ = default;
			constexpr Self&		operator = (Base::_hidden_::DefaultType)	__NE___	{ clear();  return *this; }

			constexpr Self&		set (E value, bool bit)						__NE___;
			constexpr Self&		insert (E value)							__NE___;
			constexpr Self&		InsertRange (E first, E last)				__NE___;

			constexpr Self&		erase (E value)								__NE___;
			constexpr Self&		EraseRange (E first, E last)				__NE___;

			constexpr Self&		clear ()									__NE___	{ _bits.fill( 0 );		return *this; }
			constexpr Self&		SetAll ()									__NE___	{ _bits.fill( UMax );	return *this; }

		ND_ constexpr bool		contains (E value)							C_NE___;

		ND_ constexpr bool		All ()										C_NE___	{ return BitCount() == _BitCount; }
		ND_ constexpr bool		Any ()										C_NE___;

		ND_ constexpr bool		AnyBits (const Self &rhs)					C_NE___;
		ND_ constexpr bool		AllBits (const Self &rhs)					C_NE___;

		ND_ constexpr bool		None ()										C_NE___	{ return not Any(); }
		ND_ constexpr usize		size ()										C_NE___	{ return _BitCount; }

		ND_ constexpr bool		AnyInRange (E first, E last)				C_NE___;
		ND_ constexpr bool		AllInRange (E first, E last)				C_NE___;

			constexpr Self&		operator |= (E rhs)							__NE___	{ return insert( rhs ); }
			constexpr Self&		operator &= (E rhs)							__NE___	{ return erase( rhs ); }

			constexpr Self&		operator |= (const Self &rhs)				__NE___;
			constexpr Self&		operator &= (const Self &rhs)				__NE___;

		ND_ constexpr Self		operator ~  ()								C_NE___;

		ND_ constexpr Self		operator |  (const Self &rhs)				C_NE___	{ return Self{*this} |= rhs; }
		ND_ constexpr Self		operator &  (const Self &rhs)				C_NE___	{ return Self{*this} &= rhs; }

		ND_ constexpr bool		operator == (const Self &rhs)				C_NE___;
		ND_ constexpr bool		operator != (const Self &rhs)				C_NE___	{ return not (*this == rhs); }
		ND_ constexpr bool		operator >  (const Self &rhs)				C_NE___;
		ND_ constexpr bool		operator <  (const Self &rhs)				C_NE___;
		ND_ constexpr bool		operator >= (const Self &rhs)				C_NE___	{ return not (*this < rhs); }
		ND_ constexpr bool		operator <= (const Self &rhs)				C_NE___	{ return not (*this > rhs); }

		ND_ constexpr BitArr_t const&  AsArray ()							C_NE___	{ return _bits; }

		ND_ constexpr Elem_t const&  AsBits ()								C_NE___	{ if constexpr( _ArraySize == 1 ) return _bits[0]; }

		ND_ constexpr usize		BitCount ()									C_NE___;
		ND_ constexpr usize		ZeroCount ()								C_NE___	{ return size() - BitCount(); }

		// returns 'E::_Count' if empty
		ND_ constexpr E			ExtractFirst ()								__NE___;
		ND_ constexpr E			First ()									C_NE___;	// first non-zero bit
		ND_ constexpr E			Last ()										C_NE___;	// last non-zero bit
		ND_ constexpr E			Next (E value)								C_NE___;

		ND_ constexpr iterator	begin ()									C_NE___	{ return iterator{ First(), *this }; }
		ND_ constexpr iterator	end ()										C_NE___	{ return iterator{ E::_Count, *this }; }

		ND_ HashVal				CalcHash ()									C_NE___;
	};


/*
=================================================
	insert
=================================================
*/
	template <typename E>
	constexpr EnumSet<E>&  EnumSet<E>::insert (E value) __NE___
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
	constexpr EnumSet<E>&  EnumSet<E>::set (E value, bool bit) __NE___
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
	constexpr EnumSet<E>&  EnumSet<E>::erase (E value) __NE___
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
	constexpr bool  EnumSet<E>::contains (E value) C_NE___
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
	constexpr EnumSet<E>&  EnumSet<E>::InsertRange (E first, E last) __NE___
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
	constexpr EnumSet<E>&  EnumSet<E>::EraseRange (E first, E last) __NE___
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
	constexpr EnumSet<E>&  EnumSet<E>::operator |= (const Self &rhs) __NE___
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
	constexpr EnumSet<E>&  EnumSet<E>::operator &= (const Self &rhs) __NE___
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
	constexpr EnumSet<E>  EnumSet<E>::operator ~ () C_NE___
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
	AllBits
----
	All non-zero bits in 'rhs' are equal to non-zero bits in 'this.
=================================================
*/
	template <typename E>
	constexpr bool  EnumSet<E>::AllBits (const Self &rhs) C_NE___
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
	constexpr bool  EnumSet<E>::Any () C_NE___
	{
		Elem_t	accum = 0;
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			accum |= _bits[i];
		}

		accum |= _bits.back() & _LastElemMask;
		return accum != 0;
	}

/*
=================================================
	AnyBits
----
	At least one non-zero bit in 'rhs' equal to non-zero bit in 'this'.
=================================================
*/
	template <typename E>
	constexpr bool  EnumSet<E>::AnyBits (const Self &rhs) C_NE___
	{
		bool	equal = true;
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			equal |= ((_bits[i] & rhs._bits[i]) != 0);
		}

		equal |= (((_bits.back() & rhs._bits.back()) & _LastElemMask) != 0);
		return equal;
	}

/*
=================================================
	AnyInRange
----
	At least one non-zero bit in 'this' exists in range [first, last].
=================================================
*/
	template <typename E>
	constexpr bool  EnumSet<E>::AnyInRange (E first, E last) C_NE___
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
----
	All bits in range [first, last] are non-zero.
=================================================
*/
	template <typename E>
	constexpr bool  EnumSet<E>::AllInRange (E first, E last) C_NE___
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
	constexpr bool  EnumSet<E>::operator == (const Self &rhs) C_NE___
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
	constexpr bool  EnumSet<E>::operator > (const Self &rhs) C_NE___
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
	constexpr bool  EnumSet<E>::operator < (const Self &rhs) C_NE___
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
	constexpr usize  EnumSet<E>::BitCount () C_NE___
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
----
	Extract first non-zero bit and reset it to zero.
=================================================
*/
	template <typename E>
	constexpr E  EnumSet<E>::ExtractFirst () __NE___
	{
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			if ( _bits[i] != 0 )
				return E( ExtractBitIndex( INOUT _bits[i] ) + i * _ElemSize );
		}

		if_likely( (_bits.back() & _LastElemMask) != 0 )
			return E( ExtractBitIndex( INOUT _bits.back() ) + (_ArraySize - 1) * _ElemSize );

		return E::_Count;
	}

/*
=================================================
	First
----
	Return first non-zero bit.
=================================================
*/
	template <typename E>
	constexpr E  EnumSet<E>::First () C_NE___
	{
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			if ( _bits[i] != 0 )
				return E( BitScanForward( _bits[i] ) + i * _ElemSize );
		}

		if_likely( Elem_t bits = (_bits.back() & _LastElemMask);  bits != 0 )
			return E( BitScanForward( bits ) + (_ArraySize - 1) * _ElemSize );

		return E::_Count;
	}

/*
=================================================
	Last
----
	Return last non-zero bit.
=================================================
*/
	template <typename E>
	constexpr E  EnumSet<E>::Last () C_NE___
	{
		if_likely( Elem_t bits = (_bits.back() & _LastElemMask);  bits != 0 )
			return E( BitScanReverse( bits ) + (_ArraySize - 1) * _ElemSize );

		for (int i = _ArraySize - 1; i >= 0; --i)
		{
			if ( _bits[i] != 0 )
				return E( BitScanReverse( _bits[i] ) + i * _ElemSize );
		}

		return E::_Count;
	}

/*
=================================================
	Next
----
	Return next non-zero bit after 'value'.
=================================================
*/
	template <typename E>
	constexpr E  EnumSet<E>::Next (E value) C_NE___
	{
		if constexpr( _ArraySize == 1 )
		{
			const Elem_t	bits = (_bits[0] & _LastElemMask) & ~ToBitMask<Elem_t>( Elem_t(value) + 1 );
			return bits != 0 ? E(BitScanForward( bits )) : E::_Count;
		}
		else
		{
			const uint		first	= uint(Elem_t(value) / _ElemSize);
			const Elem_t	mask	= ~ToBitMask<Elem_t>( Elem_t(value) + 1 );

			for (uint i = first; i < _ArraySize - 1; ++i)
			{
				const Elem_t	bits = _bits[i] & (i == first ? mask : ~Elem_t{0});
				if ( bits != 0 )
					return E( BitScanForward( bits ) + i * _ElemSize );
			}

			const Elem_t	last_bits = (_bits.back() & _LastElemMask) & (first == _ArraySize-1 ? mask : ~Elem_t{0});
			if_likely( last_bits != 0 )
				return E( BitScanForward( last_bits ) + (_ArraySize - 1) * _ElemSize );

			return E::_Count;
		}
	}

/*
=================================================
	CalcHash
=================================================
*/
	template <typename E>
	HashVal  EnumSet<E>::CalcHash () C_NE___
	{
		HashVal	h;
		for (uint i = 0; i < _ArraySize - 1; ++i)
		{
			h << HashOf( _bits[i] );
		}
		h << HashOf( _bits.back() & _LastElemMask );

		return h;
	}
//-----------------------------------------------------------------------------

	template <typename E> struct TMemCopyAvailable< EnumSet<E> >		{ static constexpr bool  value = true; };
	template <typename E> struct TZeroMemAvailable< EnumSet<E> >		{ static constexpr bool  value = true; };
	template <typename E> struct TTriviallySerializable< EnumSet<E> >{ static constexpr bool  value = true; };
	template <typename E> struct TTriviallyDestructible< EnumSet<E> >{ static constexpr bool  value = true; };

} // AE::Base


template <typename E>
struct std::hash< AE::Base::EnumSet<E> > :
	AE::Base::DefaultHasher_CalcHash< AE::Base::EnumSet<E> >
{};
