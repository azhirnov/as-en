// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/Cast.h"
#include "base/CompileTime/Math.h"
#include "base/Math/Vec.h"

namespace AE::Math
{
	enum class PowerOfTwo : uint {};


	//
	// Power of 2 value
	//

	template <typename T>
	struct TPowerOf2Value
	{
		StaticAssert( IsInteger<T> or IsBytes<T> );

	// types
	public:
		using Self		= TPowerOf2Value<T>;
	private:
		using Int_t		= typename Conditional< IsBytes<T>, T, TypeToValue_t<T> >::Value_t;
		using POT_t		= ubyte;


	// variables
	private:
		POT_t	_pot	= 0;


	// methods
	public:
		constexpr TPowerOf2Value ()									__NE___	{}
		constexpr TPowerOf2Value (const Self &other)				__NE___	= default;

		explicit constexpr TPowerOf2Value (UMax_t)					__NE___ : _pot{ CT_SizeOfInBits<T>-1 } {}

		template <typename IT>
		explicit TPowerOf2Value (IT val)							__NE___	: _pot{POT_t( Math::Max( IntLog2( static_cast<Int_t>(val) ), 0 ))} { ASSERT( val == Cast<IT>() ); }
		explicit constexpr TPowerOf2Value (PowerOfTwo pot)			__NE___	: _pot{POT_t(pot)} {}

		constexpr Self&  operator = (const Self &)					__NE___ = default;
		constexpr Self&  operator = (UMax_t)						__NE___ { _pot = CT_SizeOfInBits<T>-1;  return *this; }

		ND_ constexpr operator T ()									C_NE___	{ return Cast<T>(); }

		template <typename IT>
		ND_ explicit constexpr operator IT ()						C_NE___	{ return Cast<IT>(); }

		ND_ constexpr Self		operator *  (Self rhs)				C_NE___	{ return Self{ _pot + rhs._pot }; }
		ND_ constexpr Self		operator /  (Self rhs)				C_NE___	{ ASSERT( _pot >= rhs._pot );  return Self{ _pot - rhs._pot }; }
		ND_ constexpr T			operator +  (Self rhs)				C_NE___	{ return T{*this} + T{rhs}; }
		ND_ constexpr Self		operator << (uint rhs)				C_NE___	{ return Self{PowerOfTwo( _pot + rhs )}; }

		ND_ constexpr bool		operator == (Self rhs)				C_NE___	{ return _pot == rhs._pot; }
		ND_ constexpr bool		operator != (Self rhs)				C_NE___	{ return _pot != rhs._pot; }
		ND_ constexpr bool		operator >  (Self rhs)				C_NE___	{ return _pot >  rhs._pot; }
		ND_ constexpr bool		operator <  (Self rhs)				C_NE___	{ return _pot <  rhs._pot; }
		ND_ constexpr bool		operator >= (Self rhs)				C_NE___	{ return _pot >= rhs._pot; }
		ND_ constexpr bool		operator <= (Self rhs)				C_NE___	{ return _pot <= rhs._pot; }

		ND_ constexpr Self		Max (Self rhs)						C_NE___	{ return Self{ Math::Max( _pot, rhs._pot )}; }
		ND_ constexpr Self		Min (Self rhs)						C_NE___	{ return Self{ Math::Min( _pot, rhs._pot )}; }

		ND_ constexpr uint		GetPOT ()							C_NE___	{ return _pot; }

		template <typename IT>
		ND_ constexpr IT		BitMask ()							C_NE___	{ return ToBitMask<IT>( _pot ); }

		template <typename IT>
		ND_ constexpr IT		InvBitMask ()						C_NE___	{ return ~BitMask(); }

		template <typename IT>
		ND_ friend constexpr IT  operator * (const Self lhs, const IT rhs) __NE___
		{
			if constexpr( IsInteger<IT> and not IsBytes<T> )
				return rhs << lhs._pot;		// return integer
			else
			if constexpr( IsBytes<IT> and IsBytes<T> )
				return rhs << lhs._pot;		// returns bytes
		}

		template <typename IT>
		ND_ friend constexpr IT  operator * (const IT lhs, const Self rhs) __NE___
		{
			if constexpr( IsInteger<IT> and not IsBytes<T> )
				return lhs << rhs._pot;		// return integer
			else
			if constexpr( IsBytes<IT> )
				return lhs << rhs._pot;		// returns bytes
		}

		template <typename IT>
		ND_ friend constexpr IT  operator / (const IT lhs, const Self rhs) __NE___
		{
			if constexpr( IsInteger<IT> and not IsBytes<T> )
				return lhs >> rhs._pot;		// return integer
			else
			if constexpr( IsBytes<IT> and IsBytes<T> )
				return lhs >> rhs._pot;		// returns bytes
		}

		template <typename IT>
		ND_ constexpr IT  Cast ()										C_NE___
		{
			if constexpr( IsSignedInteger<IT> )
			{
				ASSERT( _pot < CT_SizeOfInBits<IT> - 1 );
				return static_cast<IT>( IT{1} << _pot );
			}
			else
			if constexpr( IsUnsignedInteger<IT> )
			{
				ASSERT( _pot < CT_SizeOfInBits<IT> );
				return static_cast<IT>( IT{1} << _pot );
			}
			else
			if constexpr( IsBytes<IT> )
			{
				return IT{ Cast<typename IT::Value_t>() };
			}
		}
	};


	using POTValue		= TPowerOf2Value< ulong >;
	using POTBytes		= TPowerOf2Value< Bytes >;


	template <typename T>
	inline static constexpr POTBytes	POTSizeOf		{PowerOfTwo( CT_IntLog2<sizeof(T)> )};

	template <typename T>
	inline static constexpr POTBytes	POTAlignOf		{PowerOfTwo( CT_IntLog2<alignof(T)> )};

	template <auto X>
	inline static constexpr POTValue	POTValue_From	{PowerOfTwo( CT_IntLog2<X> )};

	template <auto X>
	inline static constexpr POTBytes	POTBytes_From	{PowerOfTwo( CT_IntLog2<X> )};


	ND_ inline constexpr POTValue operator "" _pot (unsigned long long value) __NE___	{ return POTValue{ CheckCast<PowerOfTwo>(value) }; }

	namespace _hidden_
	{
		template <typename T>
		struct _IsPowerOf2Value : CT_False {};

		template <typename T>
		struct _IsPowerOf2Value< TPowerOf2Value<T> > : CT_True {};
	}

	template <typename T>
	static constexpr bool  IsPowerOf2Value = Math::_hidden_::_IsPowerOf2Value<T>::value;


/*
=================================================
	Min / Max / Clamp
=================================================
*/
	template <typename T>
	ND_ constexpr TPowerOf2Value<T>  Min (const TPowerOf2Value<T> x, const TPowerOf2Value<T> y) __NE___
	{
		return TPowerOf2Value<T>{ PowerOfTwo( Math::Min( x.GetPOT(), y.GetPOT() ))};
	}

	template <typename T>
	ND_ constexpr TPowerOf2Value<T>  Max (const TPowerOf2Value<T> x, const TPowerOf2Value<T> y) __NE___
	{
		return TPowerOf2Value<T>{ PowerOfTwo( Math::Max( x.GetPOT(), y.GetPOT() ))};
	}

	template <typename T>
	ND_ constexpr TPowerOf2Value<T>  Clamp (const TPowerOf2Value<T> x, const TPowerOf2Value<T> minValue, const TPowerOf2Value<T> maxValue) __NE___
	{
		return TPowerOf2Value<T>{ PowerOfTwo( Math::Clamp( x.GetPOT(), minValue.GetPOT(), maxValue.GetPOT() ))};
	}

/*
=================================================
	AlignDown
=================================================
*/
	template <typename T, typename T1>
	ND_ constexpr auto  AlignDown (const T &value, const TPowerOf2Value<T1> alignPOT) __NE___
	{
		StaticAssert( not IsPowerOf2Value<T> );

		const auto	pot = alignPOT.GetPOT();

		if constexpr( IsPointer<T> )
			return BitCast<T>( (BitCast<usize>(value) >> pot) << pot );
		else
		if constexpr( IsBytes<T> )
			return T{ (value.get() >> pot) << pot };
		else
		if constexpr( IsInteger<T> )
			return (value >> pot) << pot;
		else
		if constexpr( IsIntegerVec<T> )
			return (value >> pot) << pot;
	}

/*
=================================================
	AlignUp
=================================================
*/
	template <typename T, typename T1>
	ND_ constexpr auto  AlignUp (const T &value, const TPowerOf2Value<T1> alignPOT) __NE___
	{
		StaticAssert( not IsPowerOf2Value<T> );

		if constexpr( IsPointer<T> )
		{
			const usize	mask = alignPOT.template BitMask<usize>();
			return BitCast<T>( (BitCast<usize>(value) + mask) & ~mask );
		}else
		if constexpr( IsBytes<T> )
		{
			const auto	mask = alignPOT.template BitMask< typename T::Value_t >();
			return T{ (value.get() + mask) & ~mask };
		}else
		if constexpr( IsInteger<T> )
		{
			const auto	mask = alignPOT.template BitMask<T>();
			return (value + mask) & ~mask;
		}else
		if constexpr( IsIntegerVec<T> )
		{
			const auto	mask = alignPOT.template BitMask<T>();
			return (value + mask) & ~mask;
		}
	}

/*
=================================================
	IsMultipleOf
=================================================
*/
	template <typename T, typename T1>
	ND_ constexpr bool  IsMultipleOf (const T &value, const TPowerOf2Value<T1> alignPOT) __NE___
	{
		StaticAssert( not IsPowerOf2Value<T> );

		if constexpr( IsPointer<T> )
			return (BitCast<usize>(value) & alignPOT.template BitMask<usize>()) == 0;
		else
		if constexpr( IsBytes<T> )
			return (value.get() & alignPOT.template BitMask< typename T::Value_t >()) == 0;
		else
		if constexpr( IsInteger<T> )
			return (value & alignPOT.template BitMask<T>()) == 0;
	}

} // AE::Math

namespace AE::Base
{
	template <typename T>	struct TMemCopyAvailable< TPowerOf2Value<T> >		: CT_Bool< IsMemCopyAvailable<T>		>{};
	template <typename T>	struct TZeroMemAvailable< TPowerOf2Value<T> >		: CT_Bool< IsZeroMemAvailable<T>		>{};
	template <typename T>	struct TTriviallySerializable< TPowerOf2Value<T> >	: CT_Bool< IsTriviallySerializable<T>	>{};

} // AE::Base
