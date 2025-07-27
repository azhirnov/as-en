// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Dynamic/DynamicScalar.h"

namespace AE::ResEditor
{

	//
	// Dynamic Vector
	//

	template <typename T, int I>
	class TDynamicVec final : public EnableRC< TDynamicVec< T, I >>
	{
	// types
	public:
		using Self			= TDynamicVec< T, I >;
		using Value_t		= Vec< T, I >;
		using GetValueFn_t	= Value_t (*) (EnableRCBase*);
		using EOperator		= EDynamicVarOperator;


	// variables
	private:
		mutable RWSpinLock	_guard;
		Value_t				_vec;
		Value_t				_opValue;
		EOperator			_op			= Default;
		RC<Self>			_opDynamic;
		const RC<>			_base;
		const GetValueFn_t	_getValue	= null;


	// methods
	public:
		TDynamicVec ()										__NE___	{}
		explicit TDynamicVec (const Value_t &v)				__NE___	: _vec{v} {}
		TDynamicVec (RC<> base, GetValueFn_t getValue)		__NE___	: _base{RVRef(base)}, _getValue{getValue} {}
		
			void		SetOp (RC<Self>, EOperator)			__NE___;
			void		SetOp (const Value_t &, EOperator)	__NE___;
			void		Set (const Value_t &v)				__NE___;
		ND_ Value_t		Get ()								C_NE___;

		ND_ bool		IsChanged (INOUT Value_t &oldVal)	C_NE___;

		ND_ RC<Self>	Clone ()							__NE___;

		ND_ RC<TDynamicScalar<T>>	GetDynamicX ()			__NE___;
		ND_ RC<TDynamicScalar<T>>	GetDynamicY ()			__NE___;
		ND_ RC<TDynamicScalar<T>>	GetDynamicZ ()			__NE___;
		ND_ RC<TDynamicScalar<T>>	GetDynamicW ()			__NE___;
		
		ND_ RC<TDynamicScalar<T>>	GetDynamicArea ()		__NE___;
		ND_ RC<TDynamicScalar<T>>	GetDynamicVolume ()		__NE___;

		ND_ RC<DynamicDim>			ToDim ()				__NE___;

	private:
		ND_ static Value_t	_Get (EnableRCBase*)			__NE___;

		ND_ static T		_GetX (EnableRCBase*)			__NE___;
		ND_ static T		_GetY (EnableRCBase*)			__NE___;
		ND_ static T		_GetZ (EnableRCBase*)			__NE___;
		ND_ static T		_GetW (EnableRCBase*)			__NE___;
		ND_ static uint3	_GetDim (EnableRCBase*)			__NE___;
		ND_ static uint		_GetArea (EnableRCBase*)		__NE___;
		ND_ static uint		_GetVolume (EnableRCBase*)		__NE___;
	};


	using DynamicFloat2	= TDynamicVec< float, 2 >;
	using DynamicFloat3	= TDynamicVec< float, 3 >;
	using DynamicFloat4	= TDynamicVec< float, 4 >;

	using DynamicInt2	= TDynamicVec< int, 2 >;
	using DynamicInt3	= TDynamicVec< int, 3 >;
	using DynamicInt4	= TDynamicVec< int, 4 >;

	using DynamicUInt2	= TDynamicVec< uint, 2 >;
	using DynamicUInt3	= TDynamicVec< uint, 3 >;
	using DynamicUInt4	= TDynamicVec< uint, 4 >;


	using AnyDynVecOrScalar_t	= Union< RC<DynamicInt>, RC<DynamicInt2>, RC<DynamicInt3>, RC<DynamicInt4>,
										 RC<DynamicUInt>, RC<DynamicUInt2>, RC<DynamicUInt3>, RC<DynamicUInt4>,
										 RC<DynamicFloat>, RC<DynamicFloat2>, RC<DynamicFloat3>, RC<DynamicFloat4> >;


/*
=================================================
	SetOp
=================================================
*/
	template <typename T, int I>
	void  TDynamicVec<T,I>::SetOp (RC<Self> dyn, EOperator op) __NE___
	{
		EXLOCK( _guard );
		_opDynamic	= RVRef(dyn);
		_op			= op;
	}

	template <typename T, int I>
	void  TDynamicVec<T,I>::SetOp (const Value_t &val, EOperator op) __NE___
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
	template <typename T, int I>
	void  TDynamicVec<T,I>::Set (const Value_t &v) __NE___
	{
		EXLOCK( _guard );
		CHECK_ERRV( _getValue == null );
		_vec = v;
	}

/*
=================================================
	Get
=================================================
*/
	template <typename T, int I>
	typename TDynamicVec<T,I>::Value_t  TDynamicVec<T,I>::Get () C_NE___
	{
		SHAREDLOCK( _guard );

		Value_t	result	= _vec;
		Value_t	r_value	= _opValue;

		if_unlikely( _getValue != null )
			result = _getValue( _base.get() );
		
		if ( _opDynamic )
			r_value = _opDynamic->Get();

		switch_enum( _op )
		{
			case_likely EOperator::Unknown :	break;
			case EOperator::Mul :				result *= r_value;								break;
			case EOperator::Div :				result /= r_value;								break;
			case EOperator::DivNear :			result = (result + r_value / T(2)) / r_value;	break;
			case EOperator::DivCeil :			result = (result + r_value - T(1)) / r_value;	break;
			case EOperator::Add :				result += r_value;								break;
			case EOperator::Sub :				result -= r_value;								break;
			case EOperator::Min :				result = Min( result, r_value );				break;
			case EOperator::Max :				result = Max( result, r_value );				break;

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
					result = r_value * Pow( Value_t{T(2)}, result );
				else
					result = r_value << result;
				break;

			case EOperator::Pow :
				if constexpr( IsFloatPoint<T> )
					result = Pow( result, r_value );
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
	template <typename T, int I>
	bool  TDynamicVec<T,I>::IsChanged (INOUT Value_t &oldVal) C_NE___
	{
		const Value_t	new_val = Get();

		if ( Any( new_val != oldVal ))
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
	template <typename T, int I>
	typename TDynamicVec<T,I>::Value_t  TDynamicVec<T,I>::_Get (EnableRCBase* base) __NE___
	{
		return Cast<TDynamicVec<T,I>>(base)->Get();
	}

	template <typename T, int I>
	RC<TDynamicVec<T,I>>  TDynamicVec<T,I>::Clone () __NE___
	{
		return MakeRCNe<Self>( RC<>{this->GetRC()}, &_Get );
	}

/*
=================================================
	GetDynamicX
=================================================
*/
	template <typename T, int I>
	T  TDynamicVec<T,I>::_GetX (EnableRCBase* base) __NE___
	{
		return Cast<Self>(base)->Get().x;
	}

	template <typename T, int I>
	RC<TDynamicScalar<T>>  TDynamicVec<T,I>::GetDynamicX () __NE___
	{
		return MakeRC<TDynamicScalar<T>>( RC<>{this->GetRC()}, &_GetX );
	}

/*
=================================================
	GetDynamicY
=================================================
*/
	template <typename T, int I>
	T  TDynamicVec<T,I>::_GetY (EnableRCBase* base) __NE___
	{
		return Cast<Self>(base)->Get().y;
	}

	template <typename T, int I>
	RC<TDynamicScalar<T>>  TDynamicVec<T,I>::GetDynamicY () __NE___
	{
		StaticAssert( I >= 2 );
		return MakeRC<TDynamicScalar<T>>( RC<>{this->GetRC()}, &_GetY );
	}

/*
=================================================
	GetDynamicZ
=================================================
*/
	template <typename T, int I>
	T  TDynamicVec<T,I>::_GetZ (EnableRCBase* base) __NE___
	{
		return Cast<Self>(base)->Get().z;
	}

	template <typename T, int I>
	RC<TDynamicScalar<T>>  TDynamicVec<T,I>::GetDynamicZ () __NE___
	{
		StaticAssert( I >= 3 );
		return MakeRC<TDynamicScalar<T>>( RC<>{this->GetRC()}, &_GetZ );
	}

/*
=================================================
	GetDynamicW
=================================================
*/
	template <typename T, int I>
	T  TDynamicVec<T,I>::_GetW (EnableRCBase* base) __NE___
	{
		return Cast<Self>(base)->Get().w;
	}

	template <typename T, int I>
	RC<TDynamicScalar<T>>  TDynamicVec<T,I>::GetDynamicW () __NE___
	{
		StaticAssert( I >= 4 );
		return MakeRC<TDynamicScalar<T>>( RC<>{this->GetRC()}, &_GetW );
	}
	
/*
=================================================
	GetDynamicArea
=================================================
*/
	template <typename T, int I>
	uint  TDynamicVec<T,I>::_GetArea (EnableRCBase* base) __NE___
	{
		uint2	v {Cast<TDynamicVec<T,I>>(base)->Get()};
		return v.x * v.y;
	}

	template <typename T, int I>
	RC<TDynamicScalar<T>>  TDynamicVec<T,I>::GetDynamicArea () __NE___
	{
		StaticAssert( I >= 2 );
		return MakeRC<TDynamicScalar<T>>( RC<>{this->GetRC()}, &_GetArea );
	}
	
/*
=================================================
	GetDynamicVolume
=================================================
*/
	template <typename T, int I>
	uint  TDynamicVec<T,I>::_GetVolume (EnableRCBase* base) __NE___
	{
		uint3	v {Cast<TDynamicVec<T,I>>(base)->Get()};
		return v.x * v.y * v.z;
	}

	template <typename T, int I>
	RC<TDynamicScalar<T>>  TDynamicVec<T,I>::GetDynamicVolume () __NE___
	{
		StaticAssert( I >= 3 );
		return MakeRC<TDynamicScalar<T>>( RC<>{this->GetRC()}, &_GetVolume );
	}
//------------------------------------------------------------------------------



/*
=================================================
	ToX1
=================================================
*/
	template <typename T>
	Vec<T,2>  TDynamicScalar<T>::_GetX1 (EnableRCBase* base) __NE___
	{
		T	x = Cast<TDynamicScalar<T>>(base)->Get();
		return typename TDynamicVec<T,2>::Value_t{ x, T{1} };
	}

	template <typename T>
	RC<TDynamicVec<T,2>>  TDynamicScalar<T>::ToX1 () __NE___
	{
		return MakeRC<TDynamicVec<T,2>>( RC<>{this->GetRC()}, &_GetX1 );
	}
	
/*
=================================================
	ToXX
=================================================
*/
	template <typename T>
	Vec<T,2>  TDynamicScalar<T>::_GetXX (EnableRCBase* base) __NE___
	{
		T	x = Cast<TDynamicScalar<T>>(base)->Get();
		return typename TDynamicVec<T,2>::Value_t{ x };
	}

	template <typename T>
	RC<TDynamicVec<T,2>>  TDynamicScalar<T>::ToXX () __NE___
	{
		return MakeRC<TDynamicVec<T,2>>( RC<>{this->GetRC()}, &_GetXX );
	}

/*
=================================================
	ToX11
=================================================
*/
	template <typename T>
	Vec<T,3>  TDynamicScalar<T>::_GetX11 (EnableRCBase* base) __NE___
	{
		T	x = Cast<TDynamicScalar<T>>(base)->Get();
		return typename TDynamicVec<T,3>::Value_t{ x, T{1}, T{1} };
	}

	template <typename T>
	RC<TDynamicVec<T,3>>  TDynamicScalar<T>::ToX11 () __NE___
	{
		return MakeRC<TDynamicVec<T,3>>( RC<>{this->GetRC()}, &_GetX11 );
	}
	
/*
=================================================
	ToXXX
=================================================
*/
	template <typename T>
	Vec<T,3>  TDynamicScalar<T>::_GetXXX (EnableRCBase* base) __NE___
	{
		T	x = Cast<TDynamicScalar<T>>(base)->Get();
		return typename TDynamicVec<T,3>::Value_t{ x };
	}

	template <typename T>
	RC<TDynamicVec<T,3>>  TDynamicScalar<T>::ToXXX () __NE___
	{
		return MakeRC<TDynamicVec<T,3>>( RC<>{this->GetRC()}, &_GetXXX );
	}


} // AE::ResEditor
