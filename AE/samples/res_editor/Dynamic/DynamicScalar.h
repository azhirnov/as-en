// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Common.h"

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
		PowOf2,			// const << 2
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
		using Scalar_t		= T;
		using GetValueFn_t	= Scalar_t (*) (EnableRCBase*);
		using EOperator		= EDynamicVarOperator;


	// variables
	private:
		mutable RWSpinLock	_guard;
		Scalar_t			_value;
		Scalar_t			_opValue;
		EOperator			_op			= Default;
		const RC<>			_base;
		const GetValueFn_t	_getValue	= null;


	// methods
	public:
		TDynamicScalar ()									__NE___	: _value{T(0)} {}
		explicit TDynamicScalar (Scalar_t val)				__NE___	: _value{val} {}
		TDynamicScalar (RC<> base, GetValueFn_t getValue)	__NE___	: _base{RVRef(base)}, _getValue{getValue} {}

			void		SetOp (Scalar_t, EOperator)			__NE___;
			void		Set (Scalar_t val)					__NE___;
		ND_ Scalar_t	Get ()								C_NE___;

		ND_ bool		IsChanged (INOUT Scalar_t &oldVal)	C_NE___;

		ND_ RC<Self>	Clone ()							__NE___;

		ND_ RC<TDynamicVec<T,2>>	ToX1 ()					__NE___;
		ND_ RC<TDynamicVec<T,3>>	ToX11 ()				__NE___;

		ND_ RC<DynamicDim>			ToDim2 ()				__NE___;
		ND_ RC<DynamicDim>			ToDim3 ()				__NE___;

	private:
		ND_ static T		_Get (EnableRCBase*)			__NE___;
		ND_ static Vec<T,2>	_GetX1 (EnableRCBase*)			__NE___;
		ND_ static Vec<T,3>	_GetX11 (EnableRCBase*)			__NE___;
		ND_ static uint3	_GetDim2 (EnableRCBase*)		__NE___;
		ND_ static uint3	_GetDim3 (EnableRCBase*)		__NE___;
	};


	using DynamicFloat	= TDynamicScalar< float >;
	using DynamicInt	= TDynamicScalar< int >;
	using DynamicUInt	= TDynamicScalar< uint >;
	using DynamicULong	= TDynamicScalar< ulong >;


/*
=================================================
	SetOp
=================================================
*/
	template <typename T>
	void  TDynamicScalar<T>::SetOp (Scalar_t val, EOperator op) __NE___
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
	void  TDynamicScalar<T>::Set (Scalar_t val) __NE___
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

		T	result = _value;

		if_unlikely( _getValue != null )
			result = _getValue( _base.get() );

		switch_enum( _op )
		{
			case_likely EOperator::Unknown :	break;
			case EOperator::Mul :				result *= _opValue;							break;
			case EOperator::Div :				result /= _opValue;							break;
			case EOperator::DivNear :			result = (result + _opValue/2) / _opValue;	break;
			case EOperator::DivCeil :			result = (result + _opValue-1) / _opValue;	break;
			case EOperator::Add :				result += _opValue;							break;
			case EOperator::Sub :				result -= _opValue;							break;

			case EOperator::PowOf2 :
				if constexpr( IsFloatPoint<T> )
					result = _opValue * Pow( T(2), result );
				else
					result = _opValue << result;
				break;

			case EOperator::Pow :
				if constexpr( IsFloatPoint<T> )
					result = Pow( result, _opValue );
				else
					result = IPow( result, _opValue );
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
	bool  TDynamicScalar<T>::IsChanged (INOUT Scalar_t &oldVal) C_NE___
	{
		const Scalar_t	new_val = Get();

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
