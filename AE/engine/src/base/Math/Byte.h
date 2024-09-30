// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/Cast.h"
#include "base/Math/BitMath.h"

namespace AE::Math
{

	//
	// Byte type
	//

	template <typename T>
	struct TByte
	{
		StaticAssert( IsInteger<T> and IsScalar<T>, "must be integer scalar" );

	// types
	public:
		using Value_t	= T;
		using Self		= TByte<T>;

		struct SizeAndAlign
		{
			Self	size;
			Self	align;

			__Cx__ SizeAndAlign ()													__NE___	{}
			__Cx__ SizeAndAlign (const SizeAndAlign &)								__NE___	= default;
			__Cx__ SizeAndAlign (SizeAndAlign &&)									__NE___	= default;

			template <typename B1, typename B2>
			__Cx__ explicit SizeAndAlign (B1 inSize, B2 inAlign)					__NE___	: size{inSize}, align{inAlign} {}

			template <typename B>
			__Cx__ explicit SizeAndAlign (const B &other)							__NE___	: size{other.size}, align{other.align} {}
			__Cx__ explicit SizeAndAlign (std::size_t size, std::align_val_t align)	__NE___ : size{size}, align{align} {}

			__Cx__ SizeAndAlign  operator * (usize count)							C_NE___	{ SizeAndAlign tmp{*this};  tmp.size *= count;  return tmp; }
		};


	// variables
	private:
		T	_value;


	// methods
	public:
		__Cx__ TByte ()												__NE___ : _value{0} {}
		__Cx__ TByte (UMax_t)										__NE___	: _value{UMax} {}
		__Cx__ TByte (Default_t)									__NE___	: _value{0} {}
		__Cx__ TByte (const Self &)									__NE___	= default;

		__Cx__ explicit TByte (T value)								__NE___ : _value{value} {}
		__Cx__ explicit TByte (std::align_val_t val)				__NE___ : _value{usize(val)} {}
		__Cx__ explicit TByte (const void* ptr)						__NE___	: _value{BitCast<usize>(ptr)} {}

		template <typename B>
		__Cx__ TByte (const TByte<B> &other)						__NE___	: _value{CheckCast<T>(other)} {}

		NdCx__ explicit operator sbyte ()							C_NE___	{ return static_cast<sbyte >(_value); }
		NdCx__ explicit operator sshort ()							C_NE___	{ return static_cast<sshort>(_value); }
		NdCx__ explicit operator int ()								C_NE___	{ return static_cast<int   >(_value); }
		NdCx__ explicit operator slong ()							C_NE___	{ return static_cast<slong >(_value); }

		NdCx__ explicit operator ubyte ()							C_NE___	{ return static_cast<ubyte >(_value); }
		NdCx__ explicit operator ushort ()							C_NE___	{ return static_cast<ushort>(_value); }
		NdCx__ explicit operator uint ()							C_NE___	{ return static_cast<uint  >(_value); }
		NdCx__ explicit operator ulong ()							C_NE___	{ return static_cast<ulong >(_value); }

	  #if defined(AE_PLATFORM_WINDOWS)		or \
		  defined(AE_PLATFORM_APPLE)		or \
		  defined(AE_PLATFORM_EMSCRIPTEN)	or \
		  (defined(AE_PLATFORM_ANDROID) and AE_PLATFORM_BITS == 32)
		NdCx__ explicit operator signed long ()						C_NE___ { return static_cast< signed long >(_value); }
		NdCx__ explicit operator unsigned long ()					C_NE___ { return static_cast< unsigned long >(_value); }
	  #endif

		template <typename R>
		NdCx__ R *			AsPtr ()								C_NE___	{ return BitCast<R *>( CheckCast<usize>( _value )); }
		NdCx__ void*		AsPtr ()								C_NE___	{ return BitCast<void *>( CheckCast<usize>( _value )); }

		template <typename R>
		NdCx__ explicit operator R * ()								C_NE___	{ return BitCast<R *>( CheckCast<usize>( _value )); }

		NdCx__ T			get ()									C_NE___	{ return _value; }
		NdCx__ T			Kb ()									C_NE___	{ return _value >> 10; }
		NdCx__ T			Mb ()									C_NE___	{ return _value >> 20; }
		NdCx__ T			Gb ()									C_NE___	{ return _value >> 30; }

		NdCx__ static Self	FromBits (T value)						__NE___	{ return Self( value >> 3 ); }
		NdCx__ static Self	FromKb (T value)						__NE___	{ return Self( value << 10 ); }
		NdCx__ static Self	FromMb (T value)						__NE___	{ return Self( value << 20 ); }
		NdCx__ static Self	FromGb (T value)						__NE___	{ return Self( value << 30 ); }


		template <typename B>	NdCv__ static Self			SizeOf ()					__NE___	{ StaticAssert( not IsVoid<B> );  return Self( sizeof(B) ); }
		template <typename B>	NdCv__ static Self			SizeOf (const B &)			__NE___	{ StaticAssert( not IsVoid<B> );  return Self( sizeof(B) ); }

		template <typename B>	NdCv__ static Self			AlignOf ()					__NE___	{ StaticAssert( not IsVoid<B> );  return Self( alignof(B) ); }
		template <typename B>	NdCv__ static Self			AlignOf (const B &)			__NE___	{ StaticAssert( not IsVoid<B> );  return Self( alignof(B) ); }

		template <typename B>	NdCv__ static SizeAndAlign	SizeAndAlignOf ()			__NE___	{ return SizeAndAlign{ SizeOf<B>(), AlignOf<B>() }; }
		template <typename B>	NdCv__ static SizeAndAlign	SizeAndAlignOf (const B &)	__NE___	{ return SizeAndAlign{ SizeOf<B>(), AlignOf<B>() }; }


		// move any pointer
		template <typename B>	ND_ friend B*	operator +  (B* lhs, const Self &rhs)	__NE___	{ return BitCast<B *>( usize(lhs) + usize(rhs._value) ); }
		template <typename B>	ND_ friend B*	operator -  (B* lhs, const Self &rhs)	__NE___	{ return BitCast<B *>( usize(lhs) - usize(rhs._value) ); }
		template <typename B>		friend B*&	operator += (B* &lhs, const Self &rhs)	__NE___	{ return (lhs = lhs + rhs); }
		template <typename B>		friend B*&	operator -= (B* &lhs, const Self &rhs)	__NE___	{ return (lhs = lhs + rhs); }


		__Cx__	Self&	operator = (UMax_t)							__NE___	{ _value = UMax;	return *this; }
		__Cx__	Self&	operator = (Default_t)						__NE___	{ _value = 0;		return *this; }
		__Cx__	Self&	operator = (const Self &rhs)				__NE___	= default;

		NdCx__	Self	operator ~ ()								C_NE___	{ return Self( ~_value ); }

				Self&	operator ++ ()								__NE___	{ ++_value; return *this; }
				Self	operator ++ (int)							__NE___	{ auto res = _value++; return Self{res}; }

				Self&	operator += (const Self &rhs)				__NE___	{ _value += rhs._value;  return *this; }
		NdCx__	Self	operator +  (const Self &rhs)				C_NE___	{ return Self( _value + rhs._value ); }

				Self&	operator -= (const Self &rhs)				__NE___	{ _value -= rhs._value;  return *this; }
		NdCx__	Self	operator -  (const Self &rhs)				C_NE___	{ return Self( _value - rhs._value ); }

				Self&	operator *= (const Self &rhs)				__NE___	{ _value *= rhs._value;  return *this; }
		NdCx__	Self	operator *  (const Self &rhs)				C_NE___	{ return Self( _value * rhs._value ); }

				Self&	operator /= (const Self &rhs)				__NE___	{ _value /= rhs._value;  return *this; }
		NdCx__	Self	operator /  (const Self &rhs)				C_NE___	{ return Self( _value / rhs._value ); }

				Self&	operator %= (const Self &rhs)				__NE___	{ _value %= rhs._value;  return *this; }
		NdCx__	Self	operator %  (const Self &rhs)				C_NE___	{ return Self( _value % rhs._value ); }


				Self&	operator += (const T rhs)					__NE___	{ _value += rhs;  return *this; }
		NdCx__	Self	operator +  (const T rhs)					C_NE___	{ return Self( _value + rhs ); }

				Self&	operator -= (const T rhs)					__NE___	{ _value -= rhs;  return *this; }
		NdCx__	Self	operator -  (const T rhs)					C_NE___	{ return Self( _value - rhs ); }

				Self&	operator *= (const T rhs)					__NE___	{ _value *= rhs;  return *this; }
		NdCx__	Self	operator *  (const T rhs)					C_NE___	{ return Self( _value * rhs ); }

				Self&	operator /= (const T rhs)					__NE___	{ _value /= rhs;  return *this; }
		NdCx__	Self	operator /  (const T rhs)					C_NE___	{ return Self( _value / rhs ); }

				Self&	operator %= (const T rhs)					__NE___	{ _value %= rhs;  return *this; }
		NdCx__	Self	operator %  (const T rhs)					C_NE___	{ return Self( _value % rhs ); }

				Self&	operator >>= (const T rhs)					__NE___	{ _value >>= rhs;  return *this; }
		NdCx__	Self	operator >>  (const T rhs)					C_NE___	{ return Self{ _value >> rhs }; }

				Self&	operator <<= (const T rhs)					__NE___	{ _value <<= rhs;  return *this; }
		NdCx__	Self	operator <<  (const T rhs)					C_NE___	{ return Self{ _value << rhs }; }

		NdCx__ bool  operator == (const Self &rhs)					C_NE___	{ return _value == rhs._value; }
		NdCx__ bool  operator != (const Self &rhs)					C_NE___	{ return _value != rhs._value; }
		NdCx__ bool  operator >  (const Self &rhs)					C_NE___	{ return _value >  rhs._value; }
		NdCx__ bool  operator <  (const Self &rhs)					C_NE___	{ return _value <  rhs._value; }
		NdCx__ bool  operator >= (const Self &rhs)					C_NE___	{ return _value >= rhs._value; }
		NdCx__ bool  operator <= (const Self &rhs)					C_NE___	{ return _value <= rhs._value; }

		NdCx__ bool  operator == (const T rhs)						C_NE___	{ return _value == rhs; }
		NdCx__ bool  operator != (const T rhs)						C_NE___	{ return _value != rhs; }
		NdCx__ bool  operator >  (const T rhs)						C_NE___	{ return _value >  rhs; }
		NdCx__ bool  operator <  (const T rhs)						C_NE___	{ return _value <  rhs; }
		NdCx__ bool  operator >= (const T rhs)						C_NE___	{ return _value >= rhs; }
		NdCx__ bool  operator <= (const T rhs)						C_NE___	{ return _value <= rhs; }

		NdCx__ friend Self  operator +  (T lhs, const Self &rhs)	__NE___ { return Self( lhs + rhs._value ); }
		NdCx__ friend Self  operator -  (T lhs, const Self &rhs)	__NE___ { return Self( lhs - rhs._value ); }
		NdCx__ friend Self  operator *  (T lhs, const Self &rhs)	__NE___ { return Self( lhs * rhs._value ); }
		NdCx__ friend Self  operator /  (T lhs, const Self &rhs)	__NE___ { return Self( lhs / rhs._value ); }
		NdCx__ friend Self  operator %  (T lhs, const Self &rhs)	__NE___ { return Self( lhs % rhs._value ); }

		NdCx__ friend bool  operator == (T lhs, Self rhs)			__NE___ { return lhs == rhs._value; }
		NdCx__ friend bool  operator != (T lhs, Self rhs)			__NE___ { return lhs != rhs._value; }
		NdCx__ friend bool  operator >  (T lhs, Self rhs)			__NE___ { return lhs >  rhs._value; }
		NdCx__ friend bool  operator <  (T lhs, Self rhs)			__NE___ { return lhs <  rhs._value; }
		NdCx__ friend bool  operator >= (T lhs, Self rhs)			__NE___ { return lhs >= rhs._value; }
		NdCx__ friend bool  operator <= (T lhs, Self rhs)			__NE___ { return lhs <= rhs._value; }

		NdCv__ static Self  Max ()									__NE___ { return Self{ MaxValue<T>() }; }
		NdCv__ static Self  Min ()									__NE___ { return Self{ MinValue<T>() }; }
	};


	using Bytes64u		= TByte< ulong >;
	using Bytes32u		= TByte< uint >;
	using Bytes16u		= TByte< ushort >;
	using Bytes8u		= TByte< ubyte >;
	using BytesUSize	= TByte< usize >;

	using Bytes64s		= TByte< slong >;
	using Bytes32s		= TByte< int >;
	using Bytes16s		= TByte< short >;
	using BytesSSize	= TByte< ssize >;

	using Bytes			= Bytes64u;
	using SizeAndAlign	= Bytes::SizeAndAlign;

	template <typename T>
	inline static constexpr Bytes  SizeOf = Bytes::SizeOf<T>();

	template <typename T>
	NdCx__ Bytes  Sizeof (const T &) __NE___ { return Bytes::SizeOf<T>(); }		// TODO: consteval

	template <typename T>
	inline static constexpr Bytes  AlignOf = Bytes::AlignOf<T>();

	template <typename T>
	NdCx__ Bytes  Alignof (const T &) __NE___ { return Bytes::AlignOf<T>(); }

	template <typename T>
	inline static constexpr SizeAndAlign  SizeAndAlignOf = Bytes::SizeAndAlignOf<T>();

	template <typename T>
	NdCx__ SizeAndAlign  SizeAndAlignof (const T &) __NE___ { return Bytes::SizeAndAlignOf<T>(); }


	NdCv__ Bytes  operator "" _b  (unsigned long long value)	__NE___	{ return Bytes{ CheckCast<Bytes::Value_t>(value) }; }
	NdCv__ Bytes  operator "" _Kb (unsigned long long value)	__NE___	{ return Bytes::FromKb( CheckCast<Bytes::Value_t>(value) ); }
	NdCv__ Bytes  operator "" _Mb (unsigned long long value)	__NE___	{ return Bytes::FromMb( CheckCast<Bytes::Value_t>(value) ); }
	NdCv__ Bytes  operator "" _Gb (unsigned long long value)	__NE___	{ return Bytes::FromGb( CheckCast<Bytes::Value_t>(value) ); }


	namespace _hidden_
	{
		template <typename T>
		struct _IsBytes				: CT_False {};

		template <typename T>
		struct _IsBytes< TByte<T> >	: CT_True {};
	}

	template <typename T>
	static constexpr bool  IsBytes = Math::_hidden_::_IsBytes<T>::value;

/*
=================================================
	OffsetOf
=================================================
*/
	template <typename A, typename B>
	ND_ Bytes  OffsetOf (A B::*member) __NE___
	{
		const union U {
			B		b;
			int		tmp;
			U () : tmp{0} {}
			~U () {}
		} u;
		return Bytes( usize(std::addressof(u.b.*member)) - usize(std::addressof(u.b)) );
	}

/*
=================================================
	FloorPOT / CeilPOT
=================================================
*/
	template <typename T>
	NdCx__ TByte<T>  FloorPOT (const TByte<T> x) __NE___
	{
		return TByte<T>{ FloorPOT( T{x} )};
	}

	template <typename T>
	NdCx__ TByte<T>  CeilPOT (const TByte<T> x) __NE___
	{
		return TByte<T>{ CeilPOT( T{x} )};
	}

/*
=================================================
	IsPowerOfTwo
=================================================
*/
	template <typename T>
	NdCx__ bool  IsPowerOfTwo (const TByte<T> x) __NE___
	{
		return IsPowerOfTwo( T{x} );
	}

} // AE::Math


namespace AE::Base
{
	template <typename T>	struct TMemCopyAvailable< TByte<T> >		: CT_Bool< IsMemCopyAvailable<T>		>{};
	template <typename T>	struct TZeroMemAvailable< TByte<T> >		: CT_Bool< IsZeroMemAvailable<T>		>{};
	template <typename T>	struct TTriviallySerializable< TByte<T> >	: CT_Bool< IsTriviallySerializable<T>	>{};

} // AE::Base


template <typename T>
struct std::hash< AE::Math::TByte<T> >
{
	ND_ size_t  operator () (const AE::Math::TByte<T> &value) C_NE___
	{
		return size_t(AE::Base::HashOf( T(value) ));
	}
};

template <typename T>
class std::numeric_limits< AE::Math::TByte<T> >
{
private:
	using Bytes = AE::Math::TByte<T>;
	using Base	= std::numeric_limits<T>;

public:
	static constexpr bool is_specialized = Base::is_specialized;

	NdCx__ static Bytes  min () __NE___ {
		return Bytes{Base::min()};
	}

	NdCx__ static Bytes  max () __NE___ {
		return Bytes{Base::max()};
	}
};
