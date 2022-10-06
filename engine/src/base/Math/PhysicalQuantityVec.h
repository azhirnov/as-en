// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/PhysicalQuantity.h"

namespace AE::Math
{

	//
	// Physical Quantity Vector
	//

	template <typename		 Quantity,
			  int			 VecLength,
			  glm::qualifier Q
			 >
	struct PhysicalQuantityVec;
	
	template <int VecLength,
			  typename ValueType,
			  typename Dimension,
			  typename ValueScale,
			  glm::qualifier Q
			 >
	using TPhysicalQuantityVec = PhysicalQuantityVec< PhysicalQuantity<ValueType, Dimension, ValueScale>, VecLength, Q >;
	

	template <typename Quantity, int VecLength>
	using PhysicalQuantitySSEVec = PhysicalQuantityVec< Quantity, VecLength, GLMQuialifier >;
	
	template <typename Quantity, int VecLength>
	using PhysicalQuantityPackedVec = PhysicalQuantityVec< Quantity, VecLength, glm::qualifier::packed_highp >;



	template <typename Quantity, glm::qualifier Q>
	struct PhysicalQuantityVec <Quantity, 2, Q> : TVec<Quantity, 2, Q>
	{
	// types
		using Value_t		= typename Quantity::Value_t;
		using Scale_t		= typename Quantity::Scale_t;
		using Dimension_t	= typename Quantity::Dimension_t;
		using Self			= PhysicalQuantityVec< Quantity, 2, Q >;
		using ValVec_t		= TVec<Value_t, 2, Q>;
		using QVec_t		= TVec<Quantity, 2, Q>;
		
		STATIC_ASSERT( sizeof(QVec_t) == sizeof(ValVec_t) );

	// methods
		GLM_CONSTEXPR PhysicalQuantityVec () {}
		
		template <typename S>
		GLM_CONSTEXPR PhysicalQuantityVec (const TPhysicalQuantityVec<2, Value_t, Dimension_t, S, Q> &other) : QVec_t{other.x, other.y} {}

		GLM_CONSTEXPR PhysicalQuantityVec (Value_t X, Value_t Y) : QVec_t{X,Y} {}
		GLM_CONSTEXPR PhysicalQuantityVec (Quantity X, Quantity Y) : QVec_t{X,Y} {}

		GLM_CONSTEXPR explicit PhysicalQuantityVec (Value_t V) : QVec_t{V,V} {}
		GLM_CONSTEXPR explicit PhysicalQuantityVec (Quantity V) : QVec_t{V,V} {}
		
		GLM_CONSTEXPR explicit PhysicalQuantityVec (const ValVec_t &V) : QVec_t{V.x,V.y} {}

		ND_ GLM_CONSTEXPR ValVec_t const&	GetNonScaled ()		const	{ return *reinterpret_cast<ValVec_t const *>(this); }
		ND_ GLM_CONSTEXPR ValVec_t &		GetNonScaledRef ()			{ return *reinterpret_cast<ValVec_t *>(this); }
		ND_ GLM_CONSTEXPR ValVec_t			GetScaled ()		const	{ return { this->x.GetScaled(), this->y.GetScaled() }; }
	};
	


	template <typename Quantity, glm::qualifier Q>
	struct PhysicalQuantityVec <Quantity, 3, Q> : TVec<Quantity, 3, Q>
	{
	// types
		using Value_t		= typename Quantity::Value_t;
		using Scale_t		= typename Quantity::Scale_t;
		using Dimension_t	= typename Quantity::Dimension_t;
		using Self			= PhysicalQuantityVec< Quantity, 3, Q >;
		using ValVec_t		= TVec<Value_t, 3, Q>;
		using QVec_t		= TVec<Quantity, 3, Q>;
		
		STATIC_ASSERT( sizeof(QVec_t) == sizeof(ValVec_t) );

	// methods
		GLM_CONSTEXPR PhysicalQuantityVec () {}
		
		template <typename S>
		GLM_CONSTEXPR PhysicalQuantityVec (const TPhysicalQuantityVec<3, Value_t, Dimension_t, S, Q> &other) : QVec_t{other.x, other.y, other.z} {}

		GLM_CONSTEXPR PhysicalQuantityVec (Value_t X, Value_t Y, Value_t Z) : QVec_t{X,Y,Z} {}
		GLM_CONSTEXPR PhysicalQuantityVec (Quantity X, Quantity Y, Quantity Z) : QVec_t{X,Y,Z} {}
		
		GLM_CONSTEXPR explicit PhysicalQuantityVec (Value_t V) : QVec_t{V,V,V} {}
		GLM_CONSTEXPR explicit PhysicalQuantityVec (Quantity V) : QVec_t{V,V,V} {}
		
		GLM_CONSTEXPR explicit PhysicalQuantityVec (const ValVec_t &V) : QVec_t{V.x,V.y,V.z} {}
		
		ND_ GLM_CONSTEXPR ValVec_t const&	GetNonScaled ()		const	{ return *reinterpret_cast<ValVec_t const *>(this); }
		ND_ GLM_CONSTEXPR ValVec_t &		GetNonScaledRef ()			{ return *reinterpret_cast<ValVec_t *>(this); }
		ND_ GLM_CONSTEXPR ValVec_t			GetScaled ()		const	{ return { this->x.GetScaled(), this->y.GetScaled(), this->z.GetScaled() }; }
	};
	


	template <typename Quantity, glm::qualifier Q>
	struct PhysicalQuantityVec <Quantity, 4, Q> : TVec<Quantity, 4, Q>
	{
	// types
		using Value_t		= typename Quantity::Value_t;
		using Scale_t		= typename Quantity::Scale_t;
		using Dimension_t	= typename Quantity::Dimension_t;
		using Self			= PhysicalQuantityVec< Quantity, 4, Q >;
		using ValVec_t		= TVec<Value_t, 4, Q>;
		using QVec_t		= TVec<Quantity, 4, Q>;
		
		STATIC_ASSERT( sizeof(QVec_t) == sizeof(ValVec_t) );

	// methods
		GLM_CONSTEXPR PhysicalQuantityVec () {}
		
		template <typename S>
		GLM_CONSTEXPR PhysicalQuantityVec (const TPhysicalQuantityVec<4, Value_t, Dimension_t, S, Q> &other) : QVec_t{other.x, other.y, other.z, other.w} {}

		GLM_CONSTEXPR PhysicalQuantityVec (Value_t X, Value_t Y, Value_t Z, Value_t W) : QVec_t{X,Y,Z,W} {}
		GLM_CONSTEXPR PhysicalQuantityVec (Quantity X, Quantity Y, Quantity Z, Quantity W) : QVec_t{X,Y,Z,W} {}
		
		GLM_CONSTEXPR explicit PhysicalQuantityVec (Value_t V) : QVec_t{V,V,V,V} {}
		GLM_CONSTEXPR explicit PhysicalQuantityVec (Quantity V) : QVec_t{V,V,V,V} {}
		
		GLM_CONSTEXPR explicit PhysicalQuantityVec (const ValVec_t &V) : QVec_t{V.x,V.y,V.z,V.w} {}
		
		ND_ GLM_CONSTEXPR ValVec_t const&	GetNonScaled ()		const	{ return *reinterpret_cast<ValVec_t const *>(this); }
		ND_ GLM_CONSTEXPR ValVec_t &		GetNonScaledRef ()			{ return *reinterpret_cast<ValVec_t *>(this); }
		ND_ GLM_CONSTEXPR ValVec_t			GetScaled ()		const	{ return { this->x.GetScaled(), this->y.GetScaled(), this->z.GetScaled(), this->w.GetScaled() }; }
	};


namespace _math_hidden_
{
/*
=================================================
	PhysicalQuantityVec_GetNonScaled
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q>
	ND_ GLM_CONSTEXPR TVec<ValueType, VecLength, Q>
		PhysicalQuantityVec_GetNonScaled (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &value)
	{
		TVec<ValueType, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = value[i].GetNonScaled();
		}
		return ret;
	}
	
/*
=================================================
	PhysicalQuantityVec_GetScaled
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q>
	ND_ GLM_CONSTEXPR TVec<ValueType, VecLength, Q>
		PhysicalQuantityVec_GetScaled (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &value)
	{
		TVec<ValueType, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = value[i].GetScaled();
		}
		return ret;
	}
	
/*
=================================================
	PhysicalQuantityVec_ToScale
=================================================
*/
	template <typename NewScale, int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q>
	ND_ GLM_CONSTEXPR TVec<ValueType, VecLength, Q>
		PhysicalQuantityVec_ToScale (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &value)
	{
		const auto	scale = Scale::Value / NewScale::Value;

		TVec<ValueType, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = value[i].GetNonScaled() * scale;
		}
		return ret;
	}

}	// _math_hidden_
	

/*
=================================================
	operator + (vec, vec)
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension,
			  typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator + (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, LhsScale, Q> &lhs,
										const TPhysicalQuantityVec<VecLength, ValueType, Dimension, RhsScale, Q> &rhs)
	{
		using Scale = ValueScaleTempl::template Add< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;
		
		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{Scale::Get( lhs[i].GetNonScaled(), rhs[i].GetNonScaled() )};
		}
		return ret;
	}

/*
=================================================
	operator + (vec, scalar)
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension,
			  typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator + (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, LhsScale, Q> &lhs,
										PhysicalQuantity<ValueType, Dimension, RhsScale> rhs)
	{
		using Scale = ValueScaleTempl::template Add< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;
		
		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{Scale::Get( lhs[i].GetNonScaled(), rhs.GetNonScaled() )};
		}
		return ret;
	}

/*
=================================================
	operator + (scalar, vec)
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension,
			  typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator + (PhysicalQuantity<ValueType, Dimension, RhsScale> lhs,
										const TPhysicalQuantityVec<VecLength, ValueType, Dimension, LhsScale, Q> &rhs)
	{
		using Scale = ValueScaleTempl::template Add< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;
		
		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{Scale::Get( lhs.GetNonScaled(), rhs[i].GetNonScaled() )};
		}
		return ret;
	}
	
/*
=================================================
	operator - (vec, vec)
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension,
			  typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator - (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, LhsScale, Q> &lhs,
										const TPhysicalQuantityVec<VecLength, ValueType, Dimension, RhsScale, Q> &rhs)
	{
		using Scale = ValueScaleTempl::template Sub< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;
		
		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{Scale::Get( lhs[i].GetNonScaled(), rhs[i].GetNonScaled() )};
		}
		return ret;
	}

/*
=================================================
	operator - (vec, scalar)
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension,
			  typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator - (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, LhsScale, Q> &lhs,
										PhysicalQuantity<ValueType, Dimension, RhsScale> rhs)
	{
		using Scale = ValueScaleTempl::template Sub< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;
		
		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{Scale::Get( lhs[i].GetNonScaled(), rhs.GetNonScaled() )};
		}
		return ret;
	}

/*
=================================================
	operator - (scalar, vec)
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension,
			  typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator - (PhysicalQuantity<ValueType, Dimension, RhsScale> lhs,
										const TPhysicalQuantityVec<VecLength, ValueType, Dimension, LhsScale, Q> &rhs)
	{
		using Scale = ValueScaleTempl::template Sub< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;
		
		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{Scale::Get( lhs.GetNonScaled(), rhs[i].GetNonScaled() )};
		}
		return ret;
	}
	
/*
=================================================
	operator * (vec, vec)
=================================================
*/
	template <int VecLength, typename ValueType,
			  typename LhsDim, typename LhsScale, typename RhsDim, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator * (const TPhysicalQuantityVec<VecLength, ValueType, LhsDim, LhsScale, Q> &lhs,
										const TPhysicalQuantityVec<VecLength, ValueType, RhsDim, RhsScale, Q> &rhs)
	{
		using Scale = ValueScaleTempl::template Mul< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, typename LhsDim::template Mul<RhsDim>, Scale >;
		
		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{Scale::Get( lhs[i].GetNonScaled(), rhs[i].GetNonScaled() )};
		}
		return ret;
	}
	
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator * (const TVec<ValueType, VecLength, Q> &lhs,
										const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &rhs)
	{
		TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = lhs[i] * rhs[i];
		}
		return ret;
	}
	
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator * (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &lhs,
										const TVec<ValueType, VecLength, Q> &rhs)
	{
		TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = lhs[i] * rhs[i];	// TODO: optimize
		}
		return ret;
	}

/*
=================================================
	operator * (vec, scalar)
=================================================
*/
	template <int VecLength, typename ValueType,
			  typename LhsDim, typename LhsScale, typename RhsDim, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator * (const TPhysicalQuantityVec<VecLength, ValueType, LhsDim, LhsScale, Q> &lhs,
										PhysicalQuantity<ValueType, RhsDim, RhsScale> rhs)
	{
		using Scale = ValueScaleTempl::template Mul< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, typename LhsDim::template Mul<RhsDim>, Scale >;
		
		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{Scale::Get( lhs[i].GetNonScaled(), rhs.GetNonScaled() )};
		}
		return ret;
	}
	
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator * (const TVec<ValueType, VecLength, Q> &lhs,
										PhysicalQuantity<ValueType, Dimension, Scale> rhs)
	{
		TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q>	ret{ rhs };
		for (int i = 0; i < VecLength; ++i) {
			ret[i] *= lhs[i];
		}
		return ret;
	}

/*
=================================================
	operator * (scalar, vec)
=================================================
*/
	template <int VecLength, typename ValueType,
			  typename LhsDim, typename LhsScale, typename RhsDim, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator * (PhysicalQuantity<ValueType, LhsDim, RhsScale> lhs,
										const TPhysicalQuantityVec<VecLength, ValueType, RhsDim, LhsScale, Q> &rhs)
	{
		using Scale = ValueScaleTempl::template Mul< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, typename LhsDim::template Mul<RhsDim>, Scale >;
		
		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{Scale::Get( lhs.GetNonScaled(), rhs[i].GetNonScaled() )};
		}
		return ret;
	}
	
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator * (PhysicalQuantity<ValueType, Dimension, Scale> lhs,
										const TVec<ValueType, VecLength, Q> &rhs)
	{
		TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q>	ret{ lhs };
		for (int i = 0; i < VecLength; ++i) {
			ret[i] *= rhs[i];
		}
		return ret;
	}

/*
=================================================
	operator / (vec, vec)
=================================================
*/
	template <int VecLength, typename ValueType,
			  typename LhsDim, typename LhsScale, typename RhsDim, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator / (const TPhysicalQuantityVec<VecLength, ValueType, LhsDim, LhsScale, Q> &lhs,
										const TPhysicalQuantityVec<VecLength, ValueType, RhsDim, RhsScale, Q> &rhs)
	{
		using Scale = ValueScaleTempl::template Div< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, typename LhsDim::template Div<RhsDim>, Scale >;
		
		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{Scale::Get( lhs[i].GetNonScaled(), rhs[i].GetNonScaled() )};
		}
		return ret;
	}
	
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator / (const TVec<ValueType, VecLength, Q> &lhs,
										const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &rhs)
	{
		TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q>	ret{ lhs };
		for (int i = 0; i < VecLength; ++i) {
			ret[i] /= rhs[i];
		}
		return ret;
	}
	
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator / (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &lhs,
										const TVec<ValueType, VecLength, Q> &rhs)
	{
		TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q>	ret{ lhs };
		for (int i = 0; i < VecLength; ++i) {
			ret[i] /= rhs[i];
		}
		return ret;
	}

/*
=================================================
	operator / (vec, scalar)
=================================================
*/
	template <int VecLength, typename ValueType,
			  typename LhsDim, typename LhsScale, typename RhsDim, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator / (const TPhysicalQuantityVec<VecLength, ValueType, LhsDim, LhsScale, Q> &lhs,
										PhysicalQuantity<ValueType, RhsDim, RhsScale> rhs)
	{
		using Scale = ValueScaleTempl::template Div< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, typename LhsDim::template Div<RhsDim>, Scale >;
		
		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{Scale::Get( lhs[i].GetNonScaled(), rhs.GetNonScaled() )};
		}
		return ret;
	}
	
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator / (const TVec<ValueType, VecLength, Q> &lhs,
										PhysicalQuantity<ValueType, Dimension, Scale> rhs)
	{
		TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q>	ret{ lhs };
		for (int i = 0; i < VecLength; ++i) {
			ret[i] /= rhs[i];
		}
		return ret;
	}

/*
=================================================
	operator / (scalar, vec)
=================================================
*/
	template <int VecLength, typename ValueType,
			  typename LhsDim, typename LhsScale, typename RhsDim, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  operator / (PhysicalQuantity<ValueType, LhsDim, RhsScale> lhs,
										const TPhysicalQuantityVec<VecLength, ValueType, RhsDim, LhsScale, Q> &rhs)
	{
		using Scale = ValueScaleTempl::template Div< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, typename LhsDim::template Div<RhsDim>, Scale >;
		
		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{Scale::Get( lhs.GetNonScaled(), rhs[i].GetNonScaled() )};
		}
		return ret;
	}
	
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			  >
	ND_ GLM_CONSTEXPR auto  operator / (PhysicalQuantity<ValueType, Dimension, Scale> lhs,
										const TVec<ValueType, VecLength, Q> &rhs)
	{
		TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q>	ret{ lhs };
		for (int i = 0; i < VecLength; ++i) {
			ret[i] /= rhs[i];
		}
		return ret;
	}

/*
=================================================
	Normalize
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  Normalize (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &value)
	{
		using namespace _math_hidden_;
		return TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q>{ Normalize( PhysicalQuantityVec_GetNonScaled( value ))};
	}

/*
=================================================
	Length
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  Length (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &value)
	{
		using namespace _math_hidden_;
		return PhysicalQuantity<ValueType, Dimension, Scale>{ Length( PhysicalQuantityVec_GetNonScaled( value ))};
	}
	
/*
=================================================
	LengthSqr
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  LengthSqr (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &value)
	{
		using namespace _math_hidden_;
		using DstScale = ValueScaleTempl::template Pow< Scale, 2 >;
		return PhysicalQuantity<ValueType, Dimension, DstScale>{ LengthSqr( PhysicalQuantityVec_GetNonScaled( value ))};
	}

/*
=================================================
	Dot
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  Dot (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, LhsScale, Q> &lhs,
								 const TPhysicalQuantityVec<VecLength, ValueType, Dimension, RhsScale, Q> &rhs)
	{
		using namespace _math_hidden_;
		using Scale = ValueScaleTempl::template Mul< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;

		return Type{ Dot( PhysicalQuantityVec_GetNonScaled( lhs ), PhysicalQuantityVec_GetNonScaled( rhs ))};
	}

/*
=================================================
	Cross
=================================================
*/
	template <typename ValueType, typename Dimension, typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  Cross (const TPhysicalQuantityVec<3, ValueType, Dimension, LhsScale, Q> &lhs,
								   const TPhysicalQuantityVec<3, ValueType, Dimension, RhsScale, Q> &rhs)
	{
		using namespace _math_hidden_;
		using Scale = ValueScaleTempl::template Mul< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;

		return PhysicalQuantityVec<Type,3,Q>{ Cross( PhysicalQuantityVec_GetNonScaled( lhs ), PhysicalQuantityVec_GetNonScaled( rhs ))};
	}
	
/*
=================================================
	Distance
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  Distance (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, LhsScale, Q> &lhs,
									  const TPhysicalQuantityVec<VecLength, ValueType, Dimension, RhsScale, Q> &rhs)
	{
		using namespace _math_hidden_;
		using Scale = ValueScaleTempl::template Add< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;

		return Type{ Distance(	PhysicalQuantityVec_ToScale< Scale >( lhs ),
								PhysicalQuantityVec_ToScale< Scale >( rhs ) )};
	}
	
/*
=================================================
	DistanceSqr
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  DistanceSqr (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, LhsScale, Q> &lhs,
										 const TPhysicalQuantityVec<VecLength, ValueType, Dimension, RhsScale, Q> &rhs)
	{
		using namespace _math_hidden_;
		using Scale = ValueScaleTempl::template Add< LhsScale, RhsScale >;
		using Type	= PhysicalQuantity< ValueType, Dimension, ValueScaleTempl::template Pow< Scale, 2 > >;
		
		return Type{ DistanceSqr( PhysicalQuantityVec_ToScale< Scale >( lhs ),
								  PhysicalQuantityVec_ToScale< Scale >( rhs ) )};
	}
	
/*
=================================================
	Min
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  Min (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &lhs,
								 const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &rhs)
	{
		using Type = PhysicalQuantity< ValueType, Dimension, Scale >;

		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{ Min( lhs[i].GetNonScaled(), rhs[i].GetNonScaled() )};
		}
		return ret;
	}
	
/*
=================================================
	Max
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  Max (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &lhs,
								 const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &rhs)
	{
		using Type = PhysicalQuantity< ValueType, Dimension, Scale >;

		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{ Max( lhs[i].GetNonScaled(), rhs[i].GetNonScaled() )};
		}
		return ret;
	}
	
/*
=================================================
	Floor
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  Floor (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &value)
	{
		using Type = PhysicalQuantity< ValueType, Dimension, Scale >;

		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{ Floor( value[i].GetNonScaled() )};
		}
		return ret;
	}
	
/*
=================================================
	Ceil
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ GLM_CONSTEXPR auto  Ceil (const TPhysicalQuantityVec<VecLength, ValueType, Dimension, Scale, Q> &value)
	{
		using Type = PhysicalQuantity< ValueType, Dimension, Scale >;

		PhysicalQuantityVec<Type, VecLength, Q>	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{ Ceil( value[i].GetNonScaled() )};
		}
		return ret;
	}

}	// AE::Math


namespace AE::Base
{
	template <typename Qt, int I, glm::qualifier Ql>
	struct TMemCopyAvailable< PhysicalQuantityVec<Qt,I,Ql> > { static constexpr bool  value = IsMemCopyAvailable<Qt>; };
	
	template <typename Qt, int I, glm::qualifier Ql>
	struct TZeroMemAvailable< PhysicalQuantityVec<Qt,I,Ql> > { static constexpr bool  value = IsZeroMemAvailable<Qt>; };
	
	template <typename Qt, int I, glm::qualifier Ql>
	struct TTrivialySerializable< PhysicalQuantityVec<Qt,I,Ql> > { static constexpr bool  value = IsTrivialySerializable<Qt>; };

}	// AE::Base
