// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Common.h"

namespace AE::ResEditor
{

	enum class EDynamicVarRounding : ubyte
	{
		Floor,
		Round,
		Ceil,
	};

	enum class EDynamicVarOperator : ubyte
	{
		Unknown,
		Mul,			// x * const
		Div,			// x / const
		DivNear,		// (x + const/2) / const
		DivCeil,		// (x + const - 1) / const
		Add,			// x + const
		Sub,			// x - const
		Pow,			// pow( x, const )
		PowOf2,			// const << x
		FloorPOT,
		CeilPOT,
		NearPOT,
		Min,			// min( x, const )
		Max,			// max( x, const )
	};

	template <typename T, int I>
	class TDynamicVec;
	class DynamicDim;



	//
	// Dynamic Scalar
	//

	template <typename T>
	class TDynamicScalar final : public EnableRC< TDynamicScalar<T> >
	{
	// types
	public:
		using Self			= TDynamicScalar< T >;
		using Value_t		= T;
		using GetValueFn_t	= Value_t (*) (EnableRCBase*);
		using EOperator		= EDynamicVarOperator;


	// variables
	protected:
		mutable RWSpinLock	_guard;
		Value_t				_value;
		Value_t				_opValue;
		EOperator			_op			= Default;
		RC<Self>			_opDynamic;
		const RC<>			_base;
		const GetValueFn_t	_getValue	= null;


	// methods
	public:
		TDynamicScalar ()									__NE___	: _value{T(0)} {}
		explicit TDynamicScalar (Value_t val)				__NE___	: _value{val} {}
		explicit TDynamicScalar (RC<Self> base)				__NE___	: _base{RVRef(base)}, _getValue{&TDynamicScalar<T>::_Get} {}
		TDynamicScalar (RC<> base, GetValueFn_t getValue)	__NE___	: _base{RVRef(base)}, _getValue{getValue} {}

			void		SetOp (RC<Self>, EOperator)			__NE___;
			void		SetOp (Value_t, EOperator)			__NE___;
			void		Set (Value_t val)					__NE___;
		ND_ Value_t		Get ()								C_NE___;

		ND_ bool		IsChanged (INOUT Value_t &oldVal)	C_NE___;

		ND_ RC<Self>	Clone ()							__NE___;

		ND_ RC<TDynamicVec<T,2>>	ToX1 ()					__NE___;
		ND_ RC<TDynamicVec<T,2>>	ToXX ()					__NE___;

		ND_ RC<TDynamicVec<T,3>>	ToX11 ()				__NE___;
		ND_ RC<TDynamicVec<T,3>>	ToXXX ()				__NE___;

		ND_ RC<DynamicDim>			ToDim2 ()				__NE___;
		ND_ RC<DynamicDim>			ToDim3 ()				__NE___;

	protected:
		ND_ static T		_Get (EnableRCBase*)			__NE___;
		ND_ static Vec<T,2>	_GetX1 (EnableRCBase*)			__NE___;
		ND_ static Vec<T,2>	_GetXX (EnableRCBase*)			__NE___;
		ND_ static Vec<T,3>	_GetX11 (EnableRCBase*)			__NE___;
		ND_ static Vec<T,3>	_GetXXX (EnableRCBase*)			__NE___;
		ND_ static uint3	_GetDim2 (EnableRCBase*)		__NE___;
		ND_ static uint3	_GetDim3 (EnableRCBase*)		__NE___;
	};


	using DynamicFloat	= TDynamicScalar< float >;
	using DynamicInt	= TDynamicScalar< int >;
	using DynamicUInt	= TDynamicScalar< uint >;
	using DynamicULong	= TDynamicScalar< ulong >;
//-----------------------------------------------------------------------------



/*
=================================================
	SetOp
=================================================
*/
	template <typename T>
	void  TDynamicScalar<T>::SetOp (RC<Self> dyn, EOperator op) __NE___
	{
		EXLOCK( _guard );
		_opDynamic	= RVRef(dyn);
		_op			= op;
	}

	template <typename T>
	void  TDynamicScalar<T>::SetOp (Value_t val, EOperator op) __NE___
	{
		EXLOCK( _guard );
		_opValue	= val;
		_op			= op;
	}

/*
=================================================
	Set
=================================================
*/
	template <typename T>
	void  TDynamicScalar<T>::Set (Value_t val) __NE___
	{
		EXLOCK( _guard );
		CHECK_ERRV( _getValue == null );
		_value = val;
	}

/*
=================================================
	Get
=================================================
*/
	template <typename T>
	T  TDynamicScalar<T>::Get () C_NE___
	{
		SHAREDLOCK( _guard );

		T	result	= _value;
		T	r_value	= _opValue;

		if_unlikely( _getValue != null )
			result = _getValue( _base.get() );

		if ( _opDynamic )
			r_value = _opDynamic->Get();

		switch_enum( _op )
		{
			case_likely EOperator::Unknown :	break;
			case EOperator::Mul :				result *= r_value;							break;
			case EOperator::Div :				result /= r_value;							break;
			case EOperator::DivNear :			result = (result + r_value/2) / r_value;	break;
			case EOperator::DivCeil :			result = (result + r_value-1) / r_value;	break;
			case EOperator::Add :				result += r_value;							break;
			case EOperator::Sub :				result -= r_value;							break;
			case EOperator::Min :				result = Min( result, r_value );			break;
			case EOperator::Max :				result = Max( result, r_value );			break;

			case EOperator::FloorPOT :
				if constexpr( IsInteger<T> )
					result = FloorPOT( result );
				break;

			case EOperator::CeilPOT :
				if constexpr( IsInteger<T> )
					result = CeilPOT( result );
				break;
				
			case EOperator::NearPOT :
				if constexpr( IsInteger<T> )
					result = NearPOT( result );
				break;

			case EOperator::PowOf2 :
				if constexpr( IsFloatPoint<T> )
					result = r_value * Pow( T(2), result );
				else
					result = r_value << result;
				break;

			case EOperator::Pow :
				if constexpr( IsFloatPoint<T> )
					result = Pow( result, r_value );
				else
					result = IPow( result, r_value );
				break;
		}
		switch_end

		return result;
	}

/*
=================================================
	IsChanged
=================================================
*/
	template <typename T>
	bool  TDynamicScalar<T>::IsChanged (INOUT Value_t &oldVal) C_NE___
	{
		const Value_t	new_val = Get();

		if ( new_val != oldVal )
		{
			oldVal = new_val;
			return true;
		}
		return false;
	}

/*
=================================================
	Clone
=================================================
*/
	template <typename T>
	T  TDynamicScalar<T>::_Get (EnableRCBase* base) __NE___
	{
		return Cast<TDynamicScalar<T>>(base)->Get();
	}

	template <typename T>
	RC<TDynamicScalar<T>>  TDynamicScalar<T>::Clone () __NE___
	{
		return MakeRC<Self>( RC<>{this->GetRC()}, &_Get );
	}


} // AE::ResEditor
