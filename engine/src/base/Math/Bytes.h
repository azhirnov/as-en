// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/Cast.h"
#include "base/Math/BitMath.h"

namespace AE::Math
{

	//
	// Bytes template
	//

	template <typename T>
	struct TBytes
	{
		STATIC_ASSERT( IsInteger<T> and IsScalar<T>, "must be integer scalar" );
		
	// types
	public:
		using Value_t	= T;
		using Self		= TBytes<T>;

		struct SizeAndAlign
		{
			Self	size;
			Self	align;

			constexpr SizeAndAlign () {}
			constexpr SizeAndAlign (const SizeAndAlign &) = default;
			
			template <typename B>
			constexpr SizeAndAlign (B inSize, B inAlign) : size{inSize}, align{inAlign} {}

			template <typename B>
			constexpr explicit SizeAndAlign (const B &other) : size{other.size}, align{other.align} {}
		};


	// variables
	private:
		T	_value;


	// methods
	public:
		constexpr TBytes () : _value{0} {}

		explicit constexpr TBytes (T value) : _value{value} {}
		
		explicit constexpr TBytes (std::align_val_t value) : _value{usize(value)} {}

		template <typename B>
		constexpr TBytes (const TBytes<B> &other) : _value{CheckCast<T>(other)} {}

		explicit TBytes (const void* ptr) : _value{BitCast<usize>(ptr)} {}
		
		ND_ explicit constexpr operator sbyte ()	const	{ return static_cast<sbyte >(_value); }
		ND_ explicit constexpr operator sshort ()	const	{ return static_cast<sshort>(_value); }
		ND_ explicit constexpr operator int ()		const	{ return static_cast<int   >(_value); }
		ND_ explicit constexpr operator slong ()	const	{ return static_cast<slong >(_value); }

		ND_ explicit constexpr operator ubyte ()	const	{ return static_cast<ubyte >(_value); }
		ND_ explicit constexpr operator ushort ()	const	{ return static_cast<ushort>(_value); }
		ND_ explicit constexpr operator uint ()		const	{ return static_cast<uint  >(_value); }
		ND_ explicit constexpr operator ulong ()	const	{ return static_cast<ulong >(_value); }
		
	#ifdef AE_PLATFORM_APPLE
		ND_ explicit constexpr operator ssize ()	const	{ return ssize( _value ); }
		ND_ explicit constexpr operator usize ()	const	{ return usize( _value ); }
	#endif

		template <typename R>
		ND_ explicit constexpr operator R * ()		const	{ return BitCast<R *>( CheckCast<usize>( _value )); }
		
		ND_ constexpr T		Kb ()	const					{ return _value >> 10; }
		ND_ constexpr T		Mb ()	const					{ return _value >> 20; }
		ND_ constexpr T		Gb ()	const					{ return _value >> 30; }
		
		ND_ static constexpr Self	FromBits (T value)		{ return Self( value >> 3 ); }
		ND_ static constexpr Self	FromKb (T value)		{ return Self( value << 10 ); }
		ND_ static constexpr Self	FromMb (T value)		{ return Self( value << 20 ); }
		ND_ static constexpr Self	FromGb (T value)		{ return Self( value << 30 ); }
		

		template <typename B>	ND_ static constexpr Self	SizeOf ()			{ return Self( sizeof(B) ); }
		template <typename B>	ND_ static constexpr Self	SizeOf (const B &)	{ return Self( sizeof(B) ); }
		
		template <typename B>	ND_ static constexpr Self	AlignOf ()			{ return Self( alignof(B) ); }
		template <typename B>	ND_ static constexpr Self	AlignOf (const B &)	{ return Self( alignof(B) ); }
		
		template <typename B>	ND_ static constexpr SizeAndAlign	SizeAndAlignOf ()			{ return SizeAndAlign{ SizeOf<B>(), AlignOf<B>() }; }
		template <typename B>	ND_ static constexpr SizeAndAlign	SizeAndAlignOf (const B &)	{ return SizeAndAlign{ SizeOf<B>(), AlignOf<B>() }; }


		// move any pointer
		template <typename B>	ND_ friend B*  operator +  (B *lhs, const Self &rhs)	{ return BitCast<B *>( usize(lhs) + usize(rhs._value) ); }
		template <typename B>	ND_ friend B*  operator -  (B *lhs, const Self &rhs)	{ return BitCast<B *>( usize(lhs) - usize(rhs._value) ); }
		template <typename B>		friend B*& operator += (B* &lhs, const Self &rhs)	{ return (lhs = lhs + rhs); }
		template <typename B>		friend B*& operator -= (B* &lhs, const Self &rhs)	{ return (lhs = lhs + rhs); }


		ND_ constexpr Self	operator ~ () const					{ return Self( ~_value ); }
		
			Self&			operator ++ ()						{ ++_value; return *this; }
			Self			operator ++ (int)					{ auto res = _value++; return Self{res}; }

			Self&			operator += (const Self &rhs)		{ _value += rhs._value;  return *this; }
		ND_ constexpr Self	operator +  (const Self &rhs) const	{ return Self( _value + rhs._value ); }
		
			Self&			operator -= (const Self &rhs)		{ _value -= rhs._value;  return *this; }
		ND_ constexpr Self	operator -  (const Self &rhs) const	{ return Self( _value - rhs._value ); }

			Self&			operator *= (const Self &rhs)		{ _value *= rhs._value;  return *this; }
		ND_ constexpr Self	operator *  (const Self &rhs) const	{ return Self( _value * rhs._value ); }
		
			Self&			operator /= (const Self &rhs)		{ _value /= rhs._value;  return *this; }
		ND_ constexpr Self	operator /  (const Self &rhs) const	{ return Self( _value / rhs._value ); }
		
			Self&			operator %= (const Self &rhs)		{ _value %= rhs._value;  return *this; }
		ND_ constexpr Self	operator %  (const Self &rhs) const	{ return Self( _value % rhs._value ); }
		

			Self&			operator += (const T rhs)			{ _value += rhs;  return *this; }
		ND_ constexpr Self	operator +  (const T rhs) const		{ return Self( _value + rhs ); }
		
			Self&			operator -= (const T rhs)			{ _value -= rhs;  return *this; }
		ND_ constexpr Self	operator -  (const T rhs) const		{ return Self( _value - rhs ); }

			Self&			operator *= (const T rhs)			{ _value *= rhs;  return *this; }
		ND_ constexpr Self	operator *  (const T rhs) const		{ return Self( _value * rhs ); }
		
			Self&			operator /= (const T rhs)			{ _value /= rhs;  return *this; }
		ND_ constexpr Self	operator /  (const T rhs) const		{ return Self( _value / rhs ); }
		
			Self&			operator %= (const T rhs)			{ _value %= rhs;  return *this; }
		ND_ constexpr Self	operator %  (const T rhs) const		{ return Self( _value % rhs ); }

		ND_ constexpr bool	operator == (const Self &rhs) const	{ return _value == rhs._value; }
		ND_ constexpr bool	operator != (const Self &rhs) const	{ return _value != rhs._value; }
		ND_ constexpr bool	operator >  (const Self &rhs) const	{ return _value >  rhs._value; }
		ND_ constexpr bool	operator <  (const Self &rhs) const	{ return _value <  rhs._value; }
		ND_ constexpr bool	operator >= (const Self &rhs) const	{ return _value >= rhs._value; }
		ND_ constexpr bool	operator <= (const Self &rhs) const	{ return _value <= rhs._value; }
		
		ND_ constexpr bool	operator == (const T rhs) const		{ return _value == rhs; }
		ND_ constexpr bool	operator != (const T rhs) const		{ return _value != rhs; }
		ND_ constexpr bool	operator >  (const T rhs) const		{ return _value >  rhs; }
		ND_ constexpr bool	operator <  (const T rhs) const		{ return _value <  rhs; }
		ND_ constexpr bool	operator >= (const T rhs) const		{ return _value >= rhs; }
		ND_ constexpr bool	operator <= (const T rhs) const		{ return _value <= rhs; }

		ND_ friend constexpr Self  operator +  (T lhs, const Self &rhs)	{ return Self( lhs + rhs._value ); }
		ND_ friend constexpr Self  operator -  (T lhs, const Self &rhs)	{ return Self( lhs - rhs._value ); }
		ND_ friend constexpr Self  operator *  (T lhs, const Self &rhs)	{ return Self( lhs * rhs._value ); }
		ND_ friend constexpr Self  operator /  (T lhs, const Self &rhs)	{ return Self( lhs / rhs._value ); }
		ND_ friend constexpr Self  operator %  (T lhs, const Self &rhs)	{ return Self( lhs % rhs._value ); }

		ND_ friend constexpr bool  operator == (T lhs, Self rhs)		{ return lhs == rhs._value; }
		ND_ friend constexpr bool  operator != (T lhs, Self rhs)		{ return lhs != rhs._value; }
		ND_ friend constexpr bool  operator >  (T lhs, Self rhs)		{ return lhs >  rhs._value; }
		ND_ friend constexpr bool  operator <  (T lhs, Self rhs)		{ return lhs <  rhs._value; }
		ND_ friend constexpr bool  operator >= (T lhs, Self rhs)		{ return lhs >= rhs._value; }
		ND_ friend constexpr bool  operator <= (T lhs, Self rhs)		{ return lhs <= rhs._value; }

		ND_ static constexpr Self  Max ()								{ return Self{ MaxValue<T>() }; }
		ND_ static constexpr Self  Min ()								{ return Self{ MinValue<T>() }; }
	};
	

	using Bytes64u		= TBytes< ulong >;
	using Bytes32u		= TBytes< uint >;
	using Bytes16u		= TBytes< ushort >;
	using BytesUSize	= TBytes< usize >;

	using Bytes64s		= TBytes< slong >;
	using Bytes32s		= TBytes< int >;
	using Bytes16s		= TBytes< short >;
	using BytesSSize	= TBytes< ssize >;
	
	using Bytes			= Bytes64u;
	
	template <typename T>
	inline static constexpr Bytes	SizeOf = Bytes::SizeOf<T>();
	
	template <typename T>
	inline static constexpr Bytes	AlignOf = Bytes::AlignOf<T>();

	template <typename T>
	inline static constexpr Bytes::SizeAndAlign	SizeAndAlignOf = Bytes::SizeAndAlignOf<T>();
	

	ND_ constexpr Bytes  operator "" _b  (unsigned long long value)	{ return Bytes( CheckCast<Bytes::Value_t>(value) ); }
	ND_ constexpr Bytes  operator "" _Kb (unsigned long long value)	{ return Bytes::FromKb( CheckCast<Bytes::Value_t>(value) ); }
	ND_ constexpr Bytes  operator "" _Mb (unsigned long long value)	{ return Bytes::FromMb( CheckCast<Bytes::Value_t>(value) ); }
	ND_ constexpr Bytes  operator "" _Gb (unsigned long long value)	{ return Bytes::FromGb( CheckCast<Bytes::Value_t>(value) ); }

	
/*
=================================================
	OffsetOf
=================================================
*/
	template <typename A, typename B>
	ND_ constexpr forceinline Bytes  OffsetOf (A B::*member)
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
	ND_ forceinline TBytes<T>  FloorPOT (const TBytes<T> x)
	{
		int	i = IntLog2( T{x} );
		return TBytes<T>{ i >= 0 ? (T{1} << i) : T{0} };
	}

	template <typename T>
	ND_ forceinline TBytes<T>  CeilPOT (const TBytes<T> x)
	{
		int	i = IntLog2( T{x} );
		return TBytes<T>{ i >= 0 ? (T{1} << (i + int(not IsPowerOfTwo( T{x} )))) : T{0} };
	}

}	// AE::Math


namespace AE::Base
{
	template <typename T>	struct TMemCopyAvailable< TBytes<T> >		{ static constexpr bool  value = IsMemCopyAvailable<T>; };
	template <typename T>	struct TZeroMemAvailable< TBytes<T> >		{ static constexpr bool  value = IsZeroMemAvailable<T>; };
	template <typename T>	struct TTrivialySerializable< TBytes<T> >	{ static constexpr bool  value = IsTrivialySerializable<T>; };

}	// AE::Base


namespace std
{
	template <typename T>
	struct hash< AE::Math::TBytes<T> >
	{
		ND_ size_t  operator () (const AE::Math::TBytes<T> &value) const
		{
			return size_t(AE::Base::HashOf( T(value) ));
		}
	};

	template <typename T>
	class numeric_limits< AE::Math::TBytes<T> >
	{
	private:
		using Bytes = AE::Math::TBytes<T>;
		using Base	= std::numeric_limits<T>;

	public:
		ND_ static constexpr Bytes  min () noexcept {
			return Bytes{Base::min()};
		}

		ND_ static constexpr Bytes  max () noexcept {
			return Bytes{Base::max()};
		}
	};

}	// std
