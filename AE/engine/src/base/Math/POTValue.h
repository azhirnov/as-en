// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/Cast.h"
#include "base/CompileTime/Math.h"
#include "base/Math/Vec.h"

namespace AE::Base
{
	enum class PowerOfTwo : int {};


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
		using POT_t		= sbyte;
	private:
		static constexpr POT_t	_MaxPOT = MaxValue<POT_t>();


	// variables
	private:
		POT_t	_pot	= 0;


	// methods
	public:
		__Cx__ TPowerOf2Value ()									__NE___	{}
		__Cx__ TPowerOf2Value (const Self &other)					__NE___	= default;

		__Cx__ explicit TPowerOf2Value (UMax_t)						__NE___ : _pot{_MaxPOT} {}

		template <typename IT> requires( IsUnsignedInteger<IT> )
		__Cx__ explicit TPowerOf2Value (IT val)						__NE___	: _pot{POT_t(IntLog2( val ))}  { ASSERT( val == Cast<IT>() ); }

		template <typename IT, typename B=T> requires( IsBytes<B> )
		explicit TPowerOf2Value (TByte<IT> val)						__NE___ : TPowerOf2Value{IT{val}} {}

		__Cx__ TPowerOf2Value (PowerOfTwo pot)						__NE___	: _pot{POT_t(pot)} {}

		__Cx__ Self&	operator = (const Self &)					__NE___ = default;
		__Cx__ Self&	operator = (UMax_t)							__NE___ { _pot = _MaxPOT;  return *this; }

		NdCx__ operator T ()										C_NE___	{ return Cast<T>(); }

		template <typename IT>
		NdCx__ explicit operator IT ()								C_NE___	{ return Cast<IT>(); }

		NdCx__ Self		operator *  (Self rhs)						C_NE___	{ return Self{PowerOfTwo( _pot + rhs._pot )}; }
		NdCx__ Self		operator /  (Self rhs)						C_NE___	{ ASSERT( _pot >= rhs._pot );  return Self{PowerOfTwo( Max( _pot - rhs._pot, 0 ))}; }
		NdCx__ T		operator +  (Self rhs)						C_NE___	{ return T{*this} + T{rhs}; }

		NdCx__ bool		operator == (Self rhs)						C_NE___	{ return _pot == rhs._pot; }
		NdCx__ bool		operator != (Self rhs)						C_NE___	{ return _pot != rhs._pot; }
		NdCx__ bool		operator >  (Self rhs)						C_NE___	{ return _pot >  rhs._pot; }
		NdCx__ bool		operator <  (Self rhs)						C_NE___	{ return _pot <  rhs._pot; }
		NdCx__ bool		operator >= (Self rhs)						C_NE___	{ return _pot >= rhs._pot; }
		NdCx__ bool		operator <= (Self rhs)						C_NE___	{ return _pot <= rhs._pot; }

		NdCx__ Self		LShift (uint rhs)							C_NE___	{ return Self{PowerOfTwo( _pot + rhs )}; }
		NdCx__ Self		RShift (uint rhs)							C_NE___	{ return Self{PowerOfTwo( Max( int(_pot) - int(rhs), 0 ))}; }

		NdCx__ Self		Next ()										C_NE___	{ return LShift( 1 ); }
		NdCx__ Self		Prev ()										C_NE___	{ return RShift( 1 ); }

		NdCx__ int		GetPOT ()									C_NE___	{ return _pot; }
		NdCx__ bool		IsInvalid ()								C_NE___	{ return _pot == -1; }

		template <typename IT> requires( IsUnsignedInteger<IT> )
		NdCx__ IT		BitMask ()									C_NE___	{ return Base::ToBitMask<IT>( _pot ); }

		template <typename IT> requires( IsUnsignedInteger<IT> )
		NdCx__ IT		InvBitMask ()								C_NE___	{ return ~BitMask(); }

		NdCx__ float	AsFloat ()									C_NE___;

		template <typename IT>
		NdCx__ IT		Cast ()										C_NE___;

		template <typename IT> requires( IsUnsignedInteger<IT> )
		NdCx__ static Self  FromCeil (IT value)						__NE___;

		NdCx__ static Self  Invalid ()								__NE___	{ return Self{PowerOfTwo(-1)}; }
	};


	template <typename T, int I>
	struct TPowerOf2ValueVec;


	//
	// Power of 2 value Vector
	//
	template <typename T>
	struct TPowerOf2ValueVec< T, 2 >
	{
	// types
		using Value_t	= TPowerOf2Value<T>;
		using Self		= TPowerOf2ValueVec< T, 2 >;

	// variables
		Value_t		x, y;

	// methods
		__Cx__ TPowerOf2ValueVec ()							__NE___ {}
		__Cx__ explicit TPowerOf2ValueVec (UMax_t)			__NE___ : x{UMax}, y{UMax} {}
		__Cx__ TPowerOf2ValueVec (Value_t X, Value_t Y)		__NE___ : x{X}, y{Y} {}

		template <typename IT> requires( IsUnsignedInteger<IT> )
		__Cx__ explicit TPowerOf2ValueVec (IT X, IT Y)		__NE___ : x{X}, y{Y} {}

		NdCx__ Value_t const&	operator [] (usize idx)		C_NE___	{ ASSERT( idx < 2 );  return (&x)[idx]; }
		NdCx__ Value_t &		operator [] (usize idx)		__NE___	{ ASSERT( idx < 2 );  return (&x)[idx]; }

	// constants
		NdCx__ static Self		c_1_1 ()					__NE___	{ return Self{ 1u, 1u }; }
		NdCx__ static Self		c_1_2 ()					__NE___	{ return Self{ 1u, 2u }; }
		NdCx__ static Self		c_2_1 ()					__NE___	{ return Self{ 2u, 1u }; }
		NdCx__ static Self		c_2_2 ()					__NE___	{ return Self{ 2u, 2u }; }
	};


	//
	// Power of 2 value Vector
	//
	template <typename T>
	struct TPowerOf2ValueVec< T, 3 >
	{
	// types
		using Value_t	= TPowerOf2Value<T>;
		using Self		= TPowerOf2ValueVec< T, 3 >;

	// variables
		Value_t		x, y, z;

	// methods
		__Cx__ TPowerOf2ValueVec ()									__NE___ {}
		__Cx__ explicit TPowerOf2ValueVec (UMax_t)					__NE___ : x{UMax}, y{UMax}, z{UMax} {}
		__Cx__ TPowerOf2ValueVec (Value_t X, Value_t Y, Value_t Z)	__NE___ : x{X}, y{Y}, z{Z} {}

		template <typename IT> requires( IsUnsignedInteger<IT> )
		__Cx__ explicit TPowerOf2ValueVec (IT X, IT Y, IT Z)		__NE___ : x{X}, y{Y}, z{Z} {}

		NdCx__ Value_t const&	operator [] (usize idx)				C_NE___	{ ASSERT( idx < 3 );  return (&x)[idx]; }
		NdCx__ Value_t &		operator [] (usize idx)				__NE___	{ ASSERT( idx < 3 );  return (&x)[idx]; }
	};


	//
	// Power of 2 value Vector
	//
	template <typename T>
	struct TPowerOf2ValueVec< T, 4 >
	{
	// types
		using Value_t	= TPowerOf2Value<T>;
		using Self		= TPowerOf2ValueVec< T, 4 >;

	// variables
		Value_t		x, y, z, w;

	// methods
		__Cx__ TPowerOf2ValueVec ()												__NE___ {}
		__Cx__ explicit TPowerOf2ValueVec (UMax_t)								__NE___ : x{UMax}, y{UMax}, z{UMax}, w{UMax} {}
		__Cx__ TPowerOf2ValueVec (Value_t X, Value_t Y, Value_t Z, Value_t W)	__NE___ : x{X}, y{Y}, z{Z}, w{W} {}

		template <typename IT> requires( IsUnsignedInteger<IT> )
		__Cx__ explicit TPowerOf2ValueVec (IT X, IT Y, IT Z, IT W)				__NE___ : x{X}, y{Y}, z{Z}, w{W} {}

		NdCx__ Value_t const&	operator [] (usize idx)							C_NE___	{ ASSERT( idx < 4 );  return (&x)[idx]; }
		NdCx__ Value_t &		operator [] (usize idx)							__NE___	{ ASSERT( idx < 4 );  return (&x)[idx]; }
	};



	using POTValue	= TPowerOf2Value< ulong >;
	using POTBytes	= TPowerOf2Value< Bytes >;

	using POTVec2	= TPowerOf2ValueVec< ulong, 2 >;
	using POTVec3	= TPowerOf2ValueVec< ulong, 3 >;
	using POTVec4	= TPowerOf2ValueVec< ulong, 4 >;


	StaticAssert( sizeof(POTValue) == 1 );
	StaticAssert( sizeof(POTVec2) == 2 );
	StaticAssert( sizeof(POTVec3) == 3 );
	StaticAssert( sizeof(POTVec4) == 4 );


	template <typename T>
	inline static constexpr POTBytes	POTSizeOf		{PowerOfTwo( CT_IntLog2<sizeof(T)> )};

	template <typename T>
	inline static constexpr POTBytes	POTAlignOf		{PowerOfTwo( CT_IntLog2<alignof(T)> )};

	template <auto X>
	inline static constexpr POTValue	POTValue_From	{PowerOfTwo( CT_IntLog2<X> )};

	template <auto X>
	inline static constexpr POTBytes	POTBytes_From	{PowerOfTwo( CT_IntLog2<X> )};


	NdCxIn POTValue operator ""_pot (unsigned long long value) __NE___	{ return POTValue{ PowerOfTwo{ CheckCast{ value }}}; }

	namespace _hidden_
	{
		template <typename T>
		struct _IsPowerOf2Value : CT_False {};

		template <typename T>
		struct _IsPowerOf2Value< TPowerOf2Value<T> > : CT_True {};
	}

	template <typename T>
	static constexpr bool  IsPowerOf2Value = Base::_hidden_::_IsPowerOf2Value<T>::value;


/*
=================================================
	AsFloat
=================================================
*/
	template <typename T>
	__Cx__ float  TPowerOf2Value<T>::AsFloat () C_NE___
	{
		ASSERT( _pot >= 0 );
		ASSERT( _pot <= int(Float32Bits::_NaNExp/2) );

		if ( _pot < 0 )
			return 0.f;

		if ( _pot > int(Float32Bits::_NaNExp/2) )
			return NaN<float>();

		uint exp = Base::Min( Float32Bits::_NaNExp/2 + _pot, Float32Bits::_NaNExp );
		Float32Bits b;
		b.m=0; b.e=exp; b.s=0;
		return BitCast<float>(b);
	}

/*
=================================================
	operator *
=================================================
*/
	template <typename T, typename IT>
	__Cx__ IT  operator * (const TPowerOf2Value<T> lhs, const IT rhs) __NE___
	{
		StaticAssert( IsUnsigned<IT> );
		ASSERT( lhs.GetPOT() >= 0 );
		ASSERT( lhs.GetPOT() < int(CT_SizeOfInBits<IT>) );

		if ( lhs.GetPOT() < 0 )
			return Zero;

		if ( lhs.GetPOT() >= int(CT_SizeOfInBits<IT>) )
			return MaxValue<IT>();

		if constexpr( IsBytes<T> ){
			if constexpr( IsBytes<IT> )
				return rhs << lhs.GetPOT();		// returns bytes
		}else{
			if constexpr( IsInteger<IT> )
				return rhs << lhs.GetPOT();		// return integer
		}
	}

	template <typename T, typename IT>
	__Cx__ IT  operator * (const IT lhs, const TPowerOf2Value<T> rhs) __NE___
	{
		return rhs * lhs;
	}

/*
=================================================
	operator /
=================================================
*/
	template <typename T, typename IT>
	__Cx__ IT  operator / (const IT lhs, const TPowerOf2Value<T> rhs) __NE___
	{
		StaticAssert( IsUnsigned<IT> );
		ASSERT( rhs.GetPOT() >= 0 );
		ASSERT( rhs.GetPOT() < int(CT_SizeOfInBits<IT>) );

		if ( rhs.GetPOT() < 0 )
			return MaxValue<IT>();

		if ( rhs.GetPOT() >= int(CT_SizeOfInBits<IT>) )
			return Zero;

		if constexpr( IsBytes<T> ){
			if constexpr( IsBytes<IT> )
				return lhs >> rhs.GetPOT();		// returns bytes
		}else{
			if constexpr( IsInteger<IT> )
				return lhs >> rhs.GetPOT();		// return integer
		}
	}

/*
=================================================
	operator %
=================================================
*/
	template <typename T, typename IT>
	__Cx__ IT  operator % (const IT lhs, const TPowerOf2Value<T> rhs) __NE___
	{
		StaticAssert( IsUnsigned<IT> );
		ASSERT( rhs.GetPOT() >= 0 );
        ASSERT( rhs.GetPOT() < int(CT_SizeOfInBits<IT>) );

		if constexpr( IsBytes<T> ){
			if constexpr( IsBytes<IT> ){
				using Int_t = typename Conditional< IsBytes<T>, T, TypeToValue_t<T> >::Value_t;
				return lhs & rhs.template BitMask< Int_t >();	// returns bytes
			}
		}else{
			if constexpr( IsInteger<IT> )
				return lhs & rhs.template BitMask<IT>();		// return integer
		}
	}

/*
=================================================
	Cast
=================================================
*/
	template <typename T>
	template <typename IT>
	__Cx__ IT  TPowerOf2Value<T>::Cast () C_NE___
	{
		if constexpr( IsSignedInteger<IT> )
		{
			//ASSERT( _pot < POT_t(CT_SizeOfInBits<IT>-1) );
			return	_pot < 0 ?
						Zero :
					(_pot < POT_t(CT_SizeOfInBits<IT>-1) ?
						static_cast<IT>( IT{1} << _pot ) :
						MaxValue<IT>());
		}
		else
		if constexpr( IsUnsignedInteger<IT> )
		{
			//ASSERT( _pot < POT_t(CT_SizeOfInBits<IT>) );
			return	_pot < 0 ?
						Zero :
					(_pot < POT_t(CT_SizeOfInBits<IT>) ?
						static_cast<IT>( IT{1} << _pot ) :
						MaxValue<IT>());
		}
		else
		if constexpr( IsBytes<IT> )
		{
			return IT{ Cast<typename IT::Value_t>() };
		}
		else
		if constexpr( IsIntegerVec<IT> )
		{
			return IT{ Cast< VecToScalarType<IT> >() };
		}
		else
		if constexpr( IsSpecializationOf< IT, TPowerOf2Value >)
		{
			return IT{PowerOfTwo( _pot )};
		}
	}

/*
=================================================
	FromCeil
=================================================
*/
	template <typename T>
	template <typename IT> requires( IsUnsignedInteger<IT> )
	__Cx__ TPowerOf2Value<T>  TPowerOf2Value<T>::FromCeil (IT value) __NE___
	{
		int pot = Max( IntLog2( value ) + int(not Base::IsPowerOfTwo( value )), 0 );
		return Self{PowerOfTwo(pot)};
	}

/*
=================================================
	Min / Max / Clamp
=================================================
*/
	template <typename T>
	NdCx__ TPowerOf2Value<T>  Min (const TPowerOf2Value<T> x, const TPowerOf2Value<T> y) __NE___
	{
		return TPowerOf2Value<T>{ PowerOfTwo( Base::Min( x.GetPOT(), y.GetPOT() ))};
	}

	template <typename T>
	NdCx__ TPowerOf2Value<T>  Max (const TPowerOf2Value<T> x, const TPowerOf2Value<T> y) __NE___
	{
		return TPowerOf2Value<T>{ PowerOfTwo( Base::Max( x.GetPOT(), y.GetPOT() ))};
	}

	template <typename T>
	NdCx__ TPowerOf2Value<T>  Clamp (const TPowerOf2Value<T> x, const TPowerOf2Value<T> minValue, const TPowerOf2Value<T> maxValue) __NE___
	{
		return TPowerOf2Value<T>{ PowerOfTwo( Base::Clamp( x.GetPOT(), minValue.GetPOT(), maxValue.GetPOT() ))};
	}

/*
=================================================
	AlignDown
=================================================
*/
	template <typename T, typename T1>
	NdCx__ auto  AlignDown (const T &value, const TPowerOf2Value<T1> alignPOT) __NE___
	{
		StaticAssert( IsUnsigned<T> );
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
	NdCx__ auto  AlignUp (const T &value, const TPowerOf2Value<T1> alignPOT) __NE___
	{
		StaticAssert( IsUnsigned<T> );
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
	NdCx__ bool  IsMultipleOf (const T &value, const TPowerOf2Value<T1> alignPOT) __NE___
	{
		StaticAssert( IsUnsigned<T> or IsPointer<T> );
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

/*
=================================================
	operator *
=================================================
*/
	template <typename T, int I, glm::qualifier Q, typename P>
	ND_ TVec<T,I,Q>  operator * (const TVec<T,I,Q> &lhs, TPowerOf2Value<P> rhs) __NE___
	{
		StaticAssert( IsUnsignedInteger<T> );

		TVec<T,I,Q>	res;
		for (int i = 0; i < I; ++i)
			res[i] = lhs[i] * rhs;
		return res;
	}

	template <typename T, int I, glm::qualifier Q, typename P>
	ND_ TVec<T,I,Q>  operator * (const TVec<T,I,Q> &lhs, const TVec<TPowerOf2Value<P>,I,Q> &rhs) __NE___
	{
		StaticAssert( IsUnsignedInteger<T> );

		TVec<T,I,Q>	res;
		for (int i = 0; i < I; ++i)
			res[i] = lhs[i] * rhs[i];
		return res;
	}

/*
=================================================
	operator /
=================================================
*/
	template <typename T, int I, glm::qualifier Q, typename P>
	ND_ TVec<T,I,Q>  operator / (const TVec<T,I,Q> &lhs, TPowerOf2Value<P> rhs) __NE___
	{
		StaticAssert( IsUnsignedInteger<T> );

		TVec<T,I,Q>	res;
		for (int i = 0; i < I; ++i)
			res[i] = lhs[i] / rhs;
		return res;
	}

	template <typename T, int I, glm::qualifier Q, typename P>
	ND_ TVec<T,I,Q>  operator / (const TVec<T,I,Q> &lhs, const TVec<TPowerOf2Value<P>,I,Q> &rhs) __NE___
	{
		StaticAssert( IsUnsignedInteger<T> );

		TVec<T,I,Q>	res;
		for (int i = 0; i < I; ++i)
			res[i] = lhs[i] / rhs[i];
		return res;
	}

/*
=================================================
	DivCeil
=================================================
*/
	template <typename T, typename P> requires(IsUnsignedInteger<T>)
	NdCx__ T  DivCeil (const T &x, const TPowerOf2Value<P> &divider) __NE___
	{
		return (x + (T(1) << divider.GetPOT()) - T(1)) >> divider.GetPOT();
	}

	template <typename T, int I, glm::qualifier Q, typename P> requires(IsUnsignedInteger<T>)
	NdCx__ TVec<T,I,Q>  DivCeil (const TVec<T,I,Q> &x, const TPowerOf2Value<P> &divider) __NE___
	{
		TVec<T,I,Q>	res;
		for (int i = 0; i < I; ++i)
			res[i] = DivCeil( x[i], divider );
		return res;
	}

	template <typename T, int I, glm::qualifier Q, typename P> requires(IsUnsignedInteger<T>)
	NdCx__ TVec<T,I,Q>  DivCeil (const TVec<T,I,Q> &x, const TVec<TPowerOf2Value<P>,I,Q> &divider) __NE___
	{
		TVec<T,I,Q>	res;
		for (int i = 0; i < I; ++i)
			res[i] = DivCeil( x[i], divider[i] );
		return res;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	operator ==
=================================================
*/
	template <typename T, int I>
	ND_ Vec<bool,I>  operator == (const TPowerOf2ValueVec<T,I> &lhs, const TPowerOf2ValueVec<T,I> &rhs) __NE___
	{
		Vec<bool,I>		res;
		for (int i = 0; i < I; ++i) {
			res[i] = lhs[i] == rhs[i];
		}
		return res;
	}

/*
=================================================
	operator *
=================================================
*/
	template <typename T0, typename T1, int I, glm::qualifier Q>
	ND_ TVec<T0,I,Q>  operator * (const TVec<T0,I,Q> &lhs, const TPowerOf2ValueVec<T1,I> &rhs) __NE___
	{
		TVec<T0,I,Q>	res;
		for (int i = 0; i < I; ++i) {
			res[i] = lhs[i] * rhs[i];
		}
		return res;
	}

/*
=================================================
	operator /
=================================================
*/
	template <typename T0, typename T1, int I, glm::qualifier Q>
	ND_ TVec<T0,I,Q>  operator / (const TVec<T0,I,Q> &lhs, const TPowerOf2ValueVec<T1,I> &rhs) __NE___
	{
		TVec<T0,I,Q>	res;
		for (int i = 0; i < I; ++i) {
			res[i] = lhs[i] / rhs[i];
		}
		return res;
	}

/*
=================================================
	operator %
=================================================
*/
	template <typename T0, typename T1, int I, glm::qualifier Q>
	ND_ TVec<T0,I,Q>  operator % (const TVec<T0,I,Q> &lhs, const TPowerOf2ValueVec<T1,I> &rhs) __NE___
	{
		TVec<T0,I,Q>	res;
		for (int i = 0; i < I; ++i) {
			res[i] = lhs[i] % rhs[i];
		}
		return res;
	}

/*
=================================================
	IsMultipleOf
=================================================
*/
	template <typename T0, typename T1, int I, glm::qualifier Q>
	ND_ TVec<bool,I,Q>  IsMultipleOf (const TVec<T0,I,Q> &value, const TPowerOf2ValueVec<T1,I> &align) __NE___
	{
		return value % align == T0{0};
	}
//-----------------------------------------------------------------------------



	template <typename T>	struct TMemCopyAvailable< TPowerOf2Value<T> >		: CT_Bool< IsMemCopyAvailable<T>		>{};
	template <typename T>	struct TZeroMemAvailable< TPowerOf2Value<T> >		: CT_Bool< IsZeroMemAvailable<T>		>{};
	template <typename T>	struct TTriviallySerializable< TPowerOf2Value<T> >	: CT_Bool< IsTriviallySerializable<T>	>{};
	template <typename T>	struct TUnwrap< TPowerOf2Value<T> >					: TUnwrap<T> {};

} // AE::Base
//-----------------------------------------------------------------------------


template <typename T>
struct std::hash< AE::Base::TPowerOf2Value<T> >
{
	ND_ size_t  operator () (const AE::Base::TPowerOf2Value<T> &value) C_NE___
	{
		return size_t(AE::Base::HashOf( value.GetPOT() ));
	}
};

template <typename T>
struct std::numeric_limits< AE::Base::TPowerOf2Value<T> >
{
	using P  = AE::Base::TPowerOf2Value<T>;
	using P2 = AE::Base::PowerOfTwo;

	static constexpr bool	is_specialized = true;

	static constexpr auto	min ()	{ return P{ P2( -1 )}; }
	static constexpr auto	max ()	{ return P{ P2( AE::Base::MaxValue< typename P::POT_t >() )}; }
};
