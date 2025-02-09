// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Byte.h"

namespace AE::Base
{
	class BitMutableView;


	//
	// Bit Array View
	//

	class BitView
	{
	// types
	public:
		using Self		= BitView;
		using Value_t	= usize;


	// variables
	private:
		Value_t const*		_bits		= null;
		usize				_bitCount	= 0;


	// methods
	public:
		__Cx__ BitView ()										__NE___	{}
		__Cx__ BitView (const Self &)							__NE___	= default;
		__Cx__ BitView (const BitMutableView &)					__NE___;
		__Cz__ BitView (const Value_t* bits, usize bitCount)	__NE___;

		__Cx__ Self&	operator =  (const Self &)				__NE___	= default;
		__Cx__ Self&	operator =  (const BitMutableView &)	__NE___;

		NdCx__ bool		operator == (const Self &)				C_NE___;

		NdCx__ bool		operator [] (usize i)					C_NE___;

		NdCx__ auto*	data ()									C_NE___	{ return _bits; }

		NdCx__ bool		empty ()								C_NE___	{ return _bitCount == 0; }
		NdCx__ bool		BitCount ()								C_NE___	{ return _bitCount; }
		NdCx__ Bytes	DataSize ()								C_NE___	{ return Bytes{ (_bitCount+7) >> 3 }; }		// TODO: align up
	};



	//
	// Bit Mutable Array View
	//

	class BitMutableView
	{
	// types
	public:
		using Self		= BitMutableView;
		using Value_t	= usize;

	private:
		struct _BitSetter
		{
			friend class BitMutableView;
		private:
			BitMutableView &	_self;
			usize				_index;

			__Cx__ _BitSetter ()								= delete;
			__Cx__ _BitSetter (const _BitSetter &)				= delete;
			__Cx__ _BitSetter (_BitSetter &&)					__NE___ = default;
			__Cx__ _BitSetter (BitMutableView &ref, usize idx)	__NE___ : _self{ref}, _index{idx} {}
		public:
			__Cz__ void  operator = (bool value)				__NE___	{ _self.set( _index, value ); }
		};


	// variables
	private:
		Value_t *		_bits	= null;
		usize			_bitCount	= 0;


	// methods
	public:
		__Cx__ BitMutableView ()								__NE___	{}
		__Cx__ BitMutableView (const Self &)					__NE___	= default;
		__Cz__ BitMutableView (Value_t* bits, usize bitCount)	__NE___;

		__Cx__ Self&	operator =  (const Self &)				__NE___	= default;

		NdCx__ bool		operator == (const Self &rhs)			C_NE___	{ return BitView{*this} == BitView{rhs}; }
		NdCx__ bool		operator == (const BitView &rhs)		C_NE___	{ return BitView{*this} == rhs; }

		NdCx__ bool		operator [] (usize i)					C_NE___;
		NdCx__ auto		operator [] (usize i)					__NE___	{ return _BitSetter{ *this, i }; }

		__Cz__ void		set (usize idx, bool value)				__NE___;

		NdCx__ auto*	data ()									__NE___	{ return _bits; }
		NdCx__ auto*	data ()									C_NE___	{ return _bits; }

		NdCx__ bool		empty ()								C_NE___	{ return _bitCount == 0; }
		NdCx__ bool		BitCount ()								C_NE___	{ return _bitCount; }
		NdCx__ Bytes	DataSize ()								C_NE___	{ return Bytes{ (_bitCount+7) >> 3 }; }		// TODO: align up
	};
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	__CxIn BitView::BitView (const BitMutableView &other) __NE___ :
		_bits{ other.data() }, _bitCount{ other.BitCount() }
	{}

	__CzIn BitView::BitView (const Value_t* bits, usize bitCount) __NE___ :
		_bits{ bits }, _bitCount{ bitCount }
	{
		ASSERT( (bits != null) == (bitCount > 0) );
	}

	__CxIn BitView&  BitView::operator = (const BitMutableView &rhs) __NE___
	{
		_bits		= rhs.data();
		_bitCount	= rhs.BitCount();
		return *this;
	}

/*
=================================================
	operator ==
=================================================
*/
	__CxIn bool  BitView::operator == (const Self &rhs) C_NE___
	{
		if ( BitCount() != rhs.BitCount() )
			return false;

		if_unlikely( empty() )
			return true;

		const usize		us_mask		= CT_SizeOfInBits<usize> - 1;
		const usize		count		= (_bitCount>>3) & ~us_mask;	// bytes

		if ( std::memcmp( _bits, rhs._bits, count ) != 0 )
			return false;

		const usize		tail_mask	= (usize{1} << (_bitCount - (count<<3))) - 1;
		const usize		i			= _bitCount / CT_SizeOfInBits<usize>;

		return (_bits[i] & tail_mask) == (rhs._bits[i] & tail_mask);
	}

/*
=================================================
	operator []
=================================================
*/
	__CxIn bool  BitView::operator [] (const usize bitIdx) C_NE___
	{
		ASSERT( bitIdx < _bitCount );

		const usize	i = bitIdx / CT_SizeOfInBits<usize>;
		const usize	j = bitIdx & (CT_SizeOfInBits<usize> - 1);

		return (_bits[i] & j) != 0;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	__CzIn BitMutableView::BitMutableView (Value_t* bits, usize bitCount) __NE___ :
		_bits{ bits }, _bitCount{ bitCount }
	{
		ASSERT( (bits != null) == (bitCount > 0) );
	}

/*
=================================================
	operator []
=================================================
*/
	__CxIn bool  BitMutableView::operator [] (const usize bitIdx) C_NE___
	{
		ASSERT( bitIdx < _bitCount );

		const usize	i = bitIdx / CT_SizeOfInBits<usize>;
		const usize	j = bitIdx & (CT_SizeOfInBits<usize> - 1);

		return (_bits[i] & j) != 0;
	}

/*
=================================================
	set
=================================================
*/
	__CzIn void  BitMutableView::set (const usize bitIdx, const bool newValue) __NE___
	{
		ASSERT( bitIdx < _bitCount );

		const usize	i		= bitIdx / CT_SizeOfInBits<usize>;
		const usize	mask	= bitIdx & (CT_SizeOfInBits<usize> - 1);

		_bits[i] = (_bits[i] & ~mask) | (newValue ? mask : 0);
	}


} // AE::Base
