// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::Base::_hidden_
{
	template <uint Count_v, uint Step = 0>
	struct MSBMask
	{
	// types
	public:
		using Self		= MSBMask< Count_v, Step >;
		using Value_t	= Conditional< ((Count_v << Step) > 32), ulong, uint >;

		StaticAssert( Count_v > 0 );
		StaticAssert( Count_v <= 64 );
		StaticAssert( (Count_v << Step) <= 64 );


	// variables
	private:
		Value_t		_value	= 0;


	// methods
	public:
		__Cx__ MSBMask ()								__NE___	{}
		__Cx__ MSBMask (const MSBMask &)				__NE___	= default;
		__Cx__ explicit MSBMask (int x)					__NE___	: _value{Value_t(x)} {}
		__Cx__ explicit MSBMask (uint x)				__NE___	: _value{x} {}

		template <typename T=Value_t, ENABLEIF( sizeof(T)==sizeof(ulong) )>
		__Cx__ explicit MSBMask (ulong x)				__NE___ : _value{x} {}

		NdCe__ static uint  Count ()					__NE___	{ return Count_v; }

		template <uint Bit>
		NdCx__ bool  get ()								C_NE___	{ StaticAssert( Bit < Count() );	return Base::HasBit< Value_t{Bit}<<Step >( _value ); }
		NdCz__ bool  get (uint bit)						C_NE___	{ ASSERT( bit < Count() );			return Base::HasBit( _value, Value_t{bit}<<Step ); }

		template <uint Bit>
		__Cx__ void  set (bool value = true)			__NE___	{ StaticAssert( Bit < Count() );	_value = Base::SetBit( _value, value, Value_t{Bit}<<Step ); }
		__Cz__ void  set (uint bit, bool value = true)	__NE___	{ ASSERT( bit < Count() );			_value = Base::SetBit( _value, value, Value_t{bit}<<Step ); }

		NdCx__ bool  All ()								C_NE___	{ auto m = _Mask();  return (_value & m) == m; }
		NdCx__ bool  Any ()								C_NE___	{ auto m = _Mask();  return (_value & m) != 0; }
		NdCx__ bool  None ()							C_NE___	{ auto m = _Mask();  return (_value & m) == 0; }

		NdCx__ Self  operator | (const Self &rhs)		C_NE___	{ return Self{ _value | rhs._value }; }
		NdCx__ Self  operator & (const Self &rhs)		C_NE___	{ return Self{ _value & rhs._value }; }
		NdCx__ Self  operator ^ (const Self &rhs)		C_NE___	{ return Self{ _value ^ rhs._value }; }
		NdCx__ Self  operator ~ ()						C_NE___	{ return Self{ ~_value }; }

		NdCx__ explicit operator uint ()				C_NE___	{ return _value; }
		NdCx__ explicit operator ulong ()				C_NE___	{ return _value; }

		NdCx__ Value_t  ExtractBit ()					__NE___	{ return Base::ExtractBit( INOUT _value ); }
		NdCx__ int		ExtractBitIndex ()				__NE___	{ return Base::ExtractBitIndex( INOUT _value ); }


	private:
		template <Value_t I>
		NdCe__ static Value_t  _RecursiveMask ()		__NE___
		{
			if constexpr( I < Count() )
				return (Value_t{1} << (I << Step)) | _RecursiveMask<I+1>();
			else
				return 0u;
		}

		NdCe__ static Value_t  _Mask ()					__NE___ { return _RecursiveMask<0>(); }
	};

} // AE::Base::_hidden_
