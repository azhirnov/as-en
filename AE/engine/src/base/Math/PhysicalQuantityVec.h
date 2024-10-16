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


	template <int			 VecLength,
			  typename		 ValueType,
			  typename		 Dimension,
			  typename		 ValueScale,
			  glm::qualifier Q
			 >
	using TPhysicalQuantityVec			= PhysicalQuantityVec< PhysicalQuantity<ValueType, Dimension, ValueScale>, VecLength, Q >;


	template <typename Quantity, int VecLength>
	using PhysicalQuantitySIMDVec		= PhysicalQuantityVec< Quantity, VecLength, GLMSimdQualifier >;

	template <typename Quantity, int VecLength>
	using PhysicalQuantityPackedVec		= PhysicalQuantityVec< Quantity, VecLength, GLMPackedQualifier >;

	template <typename Quantity>
	using PhysicalQuantitySIMDVec2		= PhysicalQuantitySIMDVec< Quantity, 2 >;

	template <typename Quantity>
	using PhysicalQuantitySIMDVec3		= PhysicalQuantitySIMDVec< Quantity, 3 >;

	template <typename Quantity>
	using PhysicalQuantityPackedVec2	= PhysicalQuantityPackedVec< Quantity, 2 >;

	template <typename Quantity>
	using PhysicalQuantityPackedVec3	= PhysicalQuantityPackedVec< Quantity, 3 >;



	//
	// Physical Quantity Vector
	//

	template <typename Quantity, glm::qualifier Q>
	struct PhysicalQuantityVec <Quantity, 2, Q> : TVec< Quantity, 2, Q >
	{
		StaticAssert( IsPhysicalQuantity< Quantity >);

	// types
		using Value_t		= typename Quantity::Value_t;
		using Scale_t		= typename Quantity::Scale_t;
		using Dimension_t	= typename Quantity::Dimension_t;
		using Quantity_t	= Quantity;
		using Self			= PhysicalQuantityVec< Quantity, 2, Q >;
		using ValVec_t		= TVec< Value_t,  2, Q >;
		using QVec_t		= TVec< Quantity, 2, Q >;

		StaticAssert( sizeof(QVec_t) == sizeof(ValVec_t) );

	// methods
		PhysicalQuantityVec ()								__NE___ {}

		template <typename S>
		PhysicalQuantityVec (const TPhysicalQuantityVec<2, Value_t, Dimension_t, S, Q> &other) __NE___ : QVec_t{other.x, other.y} {}

		PhysicalQuantityVec (Value_t X, Value_t Y)			__NE___ : QVec_t{Quantity{X}, Quantity{Y}} {}
		PhysicalQuantityVec (Quantity X, Quantity Y)		__NE___ : QVec_t{X,Y} {}

		explicit PhysicalQuantityVec (Value_t s)			__NE___ : QVec_t{Quantity{s}} {}
		explicit PhysicalQuantityVec (Quantity s)			__NE___ : QVec_t{s} {}

		explicit PhysicalQuantityVec (const ValVec_t &v)	__NE___ : QVec_t{v} {}

		ND_ ValVec_t const&		GetNonScaled ()				C_NE___	{ return *reinterpret_cast<ValVec_t const *>(this); }
		ND_ ValVec_t &			GetNonScaledRef ()			__NE___	{ return *reinterpret_cast<ValVec_t *>(this); }
		ND_ ValVec_t			GetScaled ()				C_NE___	{ return GetNonScaled() * ValVec_t{Scale_t::Value}; }
	};



	template <typename Quantity, glm::qualifier Q>
	struct PhysicalQuantityVec <Quantity, 3, Q> : TVec< Quantity, 3, Q >
	{
		StaticAssert( IsPhysicalQuantity< Quantity >);

	// types
		using Value_t		= typename Quantity::Value_t;
		using Scale_t		= typename Quantity::Scale_t;
		using Dimension_t	= typename Quantity::Dimension_t;
		using Quantity_t	= Quantity;
		using Self			= PhysicalQuantityVec< Quantity, 3, Q >;
		using ValVec_t		= TVec< Value_t,  3, Q >;
		using QVec_t		= TVec< Quantity, 3, Q >;

		StaticAssert( sizeof(QVec_t) == sizeof(ValVec_t) );

	// methods
		PhysicalQuantityVec ()										__NE___	{}

		template <typename S>
		PhysicalQuantityVec (const TPhysicalQuantityVec<3, Value_t, Dimension_t, S, Q> &other) __NE___ : QVec_t{other.x, other.y, other.z} {}

		PhysicalQuantityVec (Value_t X, Value_t Y, Value_t Z)		__NE___	: QVec_t{Quantity{X}, Quantity{Y}, Quantity{Z}} {}
		PhysicalQuantityVec (Quantity X, Quantity Y, Quantity Z)	__NE___	: QVec_t{X,Y,Z} {}

		explicit PhysicalQuantityVec (Value_t s)					__NE___	: QVec_t{Quantity{s}} {}
		explicit PhysicalQuantityVec (Quantity s)					__NE___	: QVec_t{s} {}

		explicit PhysicalQuantityVec (const ValVec_t &v)			__NE___	: QVec_t{v} {}

		ND_ ValVec_t const&		GetNonScaled ()						C_NE___	{ return *reinterpret_cast<ValVec_t const *>(this); }
		ND_ ValVec_t &			GetNonScaledRef ()					__NE___	{ return *reinterpret_cast<ValVec_t *>(this); }
		ND_ ValVec_t			GetScaled ()						C_NE___	{ return GetNonScaled() * ValVec_t{Scale_t::Value}; }
	};



	template <typename Quantity, glm::qualifier Q>
	struct PhysicalQuantityVec <Quantity, 4, Q> : TVec< Quantity, 4, Q >
	{
		StaticAssert( IsPhysicalQuantity< Quantity >);

	// types
		using Value_t		= typename Quantity::Value_t;
		using Scale_t		= typename Quantity::Scale_t;
		using Dimension_t	= typename Quantity::Dimension_t;
		using Quantity_t	= Quantity;
		using Self			= PhysicalQuantityVec< Quantity, 4, Q >;
		using ValVec_t		= TVec< Value_t,  4, Q >;
		using QVec_t		= TVec< Quantity, 4, Q >;

		StaticAssert( sizeof(QVec_t) == sizeof(ValVec_t) );

	// methods
		PhysicalQuantityVec ()													__NE___	{}

		template <typename S>
		PhysicalQuantityVec (const TPhysicalQuantityVec<4, Value_t, Dimension_t, S, Q> &other) __NE___ : QVec_t{other.x, other.y, other.z, other.w} {}

		PhysicalQuantityVec (Value_t X, Value_t Y, Value_t Z, Value_t W)		__NE___	: QVec_t{Quantity{X}, Quantity{Y}, Quantity{Z}, Quantity{W}} {}
		PhysicalQuantityVec (Quantity X, Quantity Y, Quantity Z, Quantity W)	__NE___ : QVec_t{X,Y,Z,W} {}

		explicit PhysicalQuantityVec (Value_t s)								__NE___	: QVec_t{Quantity{s}} {}
		explicit PhysicalQuantityVec (Quantity s)								__NE___	: QVec_t{s} {}

		explicit PhysicalQuantityVec (const ValVec_t &v)						__NE___	: QVec_t{v.x, v.y, v.z, v.w} {}

		ND_ ValVec_t const&		GetNonScaled ()									C_NE___	{ return *reinterpret_cast<ValVec_t const *>(this); }
		ND_ ValVec_t &			GetNonScaledRef ()								__NE___	{ return *reinterpret_cast<ValVec_t *>(this); }
		ND_ ValVec_t			GetScaled ()									C_NE___	{ return GetNonScaled() * ValVec_t{Scale_t::Value}; }
	};



/*
=================================================
	PhysicalQuantity_FromVec
=================================================
*/
	namespace _hidden_
	{
		template <typename		 VecType,
				  typename		 Dimension,
				  typename		 ValueScale,
				  glm::qualifier Q
				 >
		struct _PhysicalQuantity_FromVec;

		template <typename		 T,
				  int			 VecLength,
				  typename		 Dimension,
				  typename		 ValueScale,
				  glm::qualifier Q
				 >
		struct _PhysicalQuantity_FromVec< TVec<T,VecLength,Q>, Dimension, ValueScale, Q >
		{
			using type	= TPhysicalQuantityVec< VecLength, T, Dimension, ValueScale, Q >;
		};
	}
	template <typename		 VecType,
			  typename		 Dimension,
			  typename		 ValueScale	= PhysicalQuantity_Scale::Integer< typename VecType::value_type, 1 >,
			  glm::qualifier Q			= GLMSimdQualifier
			 >
	using PhysicalQuantity_FromVec = typename Math::_hidden_::_PhysicalQuantity_FromVec< VecType, Dimension, ValueScale, Q >::type;

/*
=================================================
	PhysicalQuantityVec_ToScale
=================================================
*/
namespace _hidden_
{
	template <typename NewScale, int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q>
	ND_ TVec< ValueType, VecLength, Q >
		PhysicalQuantityVec_ToScale (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q > &value) __NE___
	{
		const auto	scale = Scale::Value / NewScale::Value;
		return value.GetNonScaled() * TVec< ValueType, VecLength, Q >{ scale };
	}
}
/*
=================================================
	operator + (vec, vec)
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension,
			  typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  operator + (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, LhsScale, Q >&  lhs,
						  const TPhysicalQuantityVec< VecLength, ValueType, Dimension, RhsScale, Q >&  rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Add< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;

		return PhysicalQuantityVec< Type, VecLength, Q >{ Scale::Get( lhs.GetNonScaled(), rhs.GetNonScaled() )};
	}

/*
=================================================
	operator + (vec, scalar)
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension,
			  typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  operator + (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, LhsScale, Q >&	lhs,
						  const PhysicalQuantity< ValueType, Dimension, RhsScale >						rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Add< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;

		return PhysicalQuantityVec< Type, VecLength, Q >{ Scale::Get( lhs.GetNonScaled(), rhs.GetNonScaled() )};
	}

/*
=================================================
	operator + (scalar, vec)
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension,
			  typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  operator + (const PhysicalQuantity< ValueType, Dimension, RhsScale >						lhs,
						  const TPhysicalQuantityVec< VecLength, ValueType, Dimension, LhsScale, Q >&	rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Add< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;

		return PhysicalQuantityVec< Type, VecLength, Q >{ Scale::Get( lhs.GetNonScaled(), rhs.GetNonScaled() )};
	}

/*
=================================================
	operator - (vec, vec)
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension,
			  typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  operator - (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, LhsScale, Q >&  lhs,
						  const TPhysicalQuantityVec< VecLength, ValueType, Dimension, RhsScale, Q >&  rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Sub< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;

		return PhysicalQuantityVec< Type, VecLength, Q >{ Scale::Get( lhs.GetNonScaled(), rhs.GetNonScaled() )};
	}

/*
=================================================
	operator - (vec, scalar)
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension,
			  typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  operator - (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, LhsScale, Q >&	lhs,
						  const PhysicalQuantity< ValueType, Dimension, RhsScale >						rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Sub< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;

		return PhysicalQuantityVec< Type, VecLength, Q >{ Scale::Get( lhs.GetNonScaled(), rhs.GetNonScaled() )};
	}

/*
=================================================
	operator - (scalar, vec)
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension,
			  typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  operator - (const PhysicalQuantity< ValueType, Dimension, RhsScale >						lhs,
						  const TPhysicalQuantityVec< VecLength, ValueType, Dimension, LhsScale, Q >&	rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Sub< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;

		return PhysicalQuantityVec< Type, VecLength, Q >{ Scale::Get( lhs.GetNonScaled(), rhs.GetNonScaled() )};
	}

/*
=================================================
	operator * (vec, vec)
=================================================
*/
	template <int VecLength, typename ValueType,
			  typename LhsDim, typename LhsScale, typename RhsDim, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  operator * (const TPhysicalQuantityVec< VecLength, ValueType, LhsDim, LhsScale, Q >&  lhs,
						  const TPhysicalQuantityVec< VecLength, ValueType, RhsDim, RhsScale, Q >&  rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Mul< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, typename LhsDim::template Mul<RhsDim>, Scale >;

		return PhysicalQuantityVec< Type, VecLength, Q >{ Scale::Get( lhs.GetNonScaled(), rhs.GetNonScaled() )};
	}

	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ auto  operator * (const TVec< ValueType, VecLength, Q >									 &  lhs,
						  const TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >&  rhs) __NE___
	{
		return TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >{ lhs * rhs.GetNonScaled() };
	}

	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ auto  operator * (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >&  lhs,
						  const TVec< ValueType, VecLength, Q >									 &  rhs) __NE___
	{
		return TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >{ lhs.GetNonScaled() * rhs };
	}

/*
=================================================
	operator * (vec, scalar)
=================================================
*/
	template <int VecLength, typename ValueType,
			  typename LhsDim, typename LhsScale, typename RhsDim, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  operator * (const TPhysicalQuantityVec< VecLength, ValueType, LhsDim, LhsScale, Q >&  lhs,
						  const PhysicalQuantity< ValueType, RhsDim, RhsScale >                     rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Mul< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, typename LhsDim::template Mul<RhsDim>, Scale >;

		return PhysicalQuantityVec< Type, VecLength, Q >{ Scale::Get( lhs.GetNonScaled(), rhs.GetNonScaled() )};
	}

	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ auto  operator * (const TVec< ValueType, VecLength, Q >					&	lhs,
						  const PhysicalQuantity< ValueType, Dimension, Scale >		rhs) __NE___
	{
		return TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >{ lhs * rhs.GetNonScaled() };
	}

/*
=================================================
	operator * (scalar, vec)
=================================================
*/
	template <int VecLength, typename ValueType,
			  typename LhsDim, typename LhsScale, typename RhsDim, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  operator * (const PhysicalQuantity< ValueType, LhsDim, RhsScale >						lhs,
						  const TPhysicalQuantityVec< VecLength, ValueType, RhsDim, LhsScale, Q >&	rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Mul< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, typename LhsDim::template Mul<RhsDim>, Scale >;

		return PhysicalQuantityVec< Type, VecLength, Q >{ Scale::Get( lhs.GetNonScaled(), rhs.GetNonScaled() )};
	}

	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ auto  operator * (const PhysicalQuantity< ValueType, Dimension, Scale >		lhs,
						  const TVec< ValueType, VecLength, Q >					&	rhs) __NE___
	{
		return TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >{ lhs.GetNonScaled() * rhs };
	}

/*
=================================================
	operator / (vec, vec)
=================================================
*/
	template <int VecLength, typename ValueType,
			  typename LhsDim, typename LhsScale, typename RhsDim, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  operator / (const TPhysicalQuantityVec< VecLength, ValueType, LhsDim, LhsScale, Q >&  lhs,
						  const TPhysicalQuantityVec< VecLength, ValueType, RhsDim, RhsScale, Q >&  rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Div< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, typename LhsDim::template Div<RhsDim>, Scale >;

		return PhysicalQuantityVec< Type, VecLength, Q >{ Scale::Get( lhs.GetNonScaled(), rhs.GetNonScaled() )};
	}

	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ auto  operator / (const TVec< ValueType, VecLength, Q >									 &  lhs,
						  const TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >&  rhs) __NE___
	{
		return TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >{ lhs / rhs.GetNonScaled() };
	}

	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ auto  operator / (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >&  lhs,
						  const TVec< ValueType, VecLength, Q >									 &  rhs) __NE___
	{
		return TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >{ lhs.GetNonScaled() / rhs };
	}

/*
=================================================
	operator / (vec, scalar)
=================================================
*/
	template <int VecLength, typename ValueType,
			  typename LhsDim, typename LhsScale, typename RhsDim, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  operator / (const TPhysicalQuantityVec< VecLength, ValueType, LhsDim, LhsScale, Q >&	lhs,
						  const PhysicalQuantity< ValueType, RhsDim, RhsScale >						rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Div< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, typename LhsDim::template Div<RhsDim>, Scale >;

		return PhysicalQuantityVec< Type, VecLength, Q >{ Scale::Get( lhs.GetNonScaled(), rhs.GetNonScaled() )};
	}

	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ auto  operator / (const TVec< ValueType, VecLength, Q >					&	lhs,
						  const PhysicalQuantity< ValueType, Dimension, Scale >		rhs) __NE___
	{
		return TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >{ lhs / rhs.GetNonScaled() };
	}

/*
=================================================
	operator / (scalar, vec)
=================================================
*/
	template <int VecLength, typename ValueType,
			  typename LhsDim, typename LhsScale, typename RhsDim, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  operator / (const PhysicalQuantity< ValueType, LhsDim, RhsScale >						lhs,
						  const TPhysicalQuantityVec< VecLength, ValueType, RhsDim, LhsScale, Q >&	rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Div< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, typename LhsDim::template Div<RhsDim>, Scale >;

		return PhysicalQuantityVec< Type, VecLength, Q >{ Scale::Get( lhs.GetNonScaled(), rhs.GetNonScaled() )};
	}

	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			  >
	ND_ auto  operator / (const PhysicalQuantity< ValueType, Dimension, Scale >		lhs,
						  const TVec< ValueType, VecLength, Q >					&	rhs) __NE___
	{
		return TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >{ lhs.GetNonScaled() / rhs };
	}

/*
=================================================
	Normalize
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ auto  Normalize (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >  &value) __NE___
	{
		return TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >{ Normalize( value.GetNonScaled() )};
	}

/*
=================================================
	Length
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ auto  Length (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >  &value) __NE___
	{
		return PhysicalQuantity< ValueType, Dimension, Scale >{ Length( value.GetNonScaled() )};
	}

/*
=================================================
	LengthSq
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ auto  LengthSq (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >  &value) __NE___
	{
		using DstScale = PhysicalQuantity_Scale::template Pow< Scale, 2 >;
		return PhysicalQuantity< ValueType, Dimension, DstScale >{ LengthSq( value.GetNonScaled() )};
	}

/*
=================================================
	Dot
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  Dot (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, LhsScale, Q >&  lhs,
				   const TPhysicalQuantityVec< VecLength, ValueType, Dimension, RhsScale, Q >&  rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Mul< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;

		return Type{ Dot( lhs.GetNonScaled(), rhs.GetNonScaled() )};
	}

/*
=================================================
	Cross
=================================================
*/
	template <typename ValueType, typename Dimension, typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  Cross (const TPhysicalQuantityVec< 3, ValueType, Dimension, LhsScale, Q >&  lhs,
					 const TPhysicalQuantityVec< 3, ValueType, Dimension, RhsScale, Q >&  rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Mul< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;

		return PhysicalQuantityVec< Type, 3, Q >{ Cross( lhs.GetNonScaled(), rhs.GetNonScaled() )};
	}

/*
=================================================
	Distance
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  Distance (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, LhsScale, Q >&  lhs,
						const TPhysicalQuantityVec< VecLength, ValueType, Dimension, RhsScale, Q >&  rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Add< LhsScale, RhsScale >;
		using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;

		return Type{ Distance(	Math::_hidden_::PhysicalQuantityVec_ToScale< Scale >( lhs ),
								Math::_hidden_::PhysicalQuantityVec_ToScale< Scale >( rhs ) )};
	}

/*
=================================================
	DistanceSq
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename LhsScale, typename RhsScale, glm::qualifier Q
			 >
	ND_ auto  DistanceSq (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, LhsScale, Q > &  lhs,
						   const TPhysicalQuantityVec< VecLength, ValueType, Dimension, RhsScale, Q >&  rhs) __NE___
	{
		using Scale = PhysicalQuantity_Scale::template Add< LhsScale, RhsScale >;
		using Type	= PhysicalQuantity< ValueType, Dimension, PhysicalQuantity_Scale::template Pow< Scale, 2 > >;

		return Type{ DistanceSq( Math::_hidden_::PhysicalQuantityVec_ToScale< Scale >( lhs ),
								  Math::_hidden_::PhysicalQuantityVec_ToScale< Scale >( rhs ) )};
	}

/*
=================================================
	Min
=================================================
*/
	template <int VecLength, typename ValueType, typename Dimension, typename Scale, glm::qualifier Q
			 >
	ND_ auto  Min (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >&  lhs,
				   const TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >&  rhs) __NE___
	{
		using Type = PhysicalQuantity< ValueType, Dimension, Scale >;

		PhysicalQuantityVec< Type, VecLength, Q >	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{ Min( lhs[i].GetNonScaled(), rhs[i].GetNonScaled() )};	// TODO: SIMD ?
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
	ND_ auto  Max (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >&  lhs,
				   const TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q >&  rhs) __NE___
	{
		using Type = PhysicalQuantity< ValueType, Dimension, Scale >;

		PhysicalQuantityVec< Type, VecLength, Q >	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{ Max( lhs[i].GetNonScaled(), rhs[i].GetNonScaled() )};	// TODO: SIMD ?
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
	ND_ auto  Floor (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q > &value) __NE___
	{
		using Type = PhysicalQuantity< ValueType, Dimension, Scale >;

		PhysicalQuantityVec< Type, VecLength, Q >	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{ Floor( value[i].GetNonScaled() )};	// TODO: SIMD ?
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
	ND_ auto  Ceil (const TPhysicalQuantityVec< VecLength, ValueType, Dimension, Scale, Q > &value) __NE___
	{
		using Type = PhysicalQuantity< ValueType, Dimension, Scale >;

		PhysicalQuantityVec< Type, VecLength, Q >	ret;
		for (int i = 0; i < VecLength; ++i) {
			ret[i] = Type{ Ceil( value[i].GetNonScaled() )};	// TODO: SIMD ?
		}
		return ret;
	}

} // AE::Math


namespace AE::Base
{
	template <typename Qt, int I, glm::qualifier Ql>
	struct TMemCopyAvailable< PhysicalQuantityVec<Qt,I,Ql> >	: CT_Bool< IsMemCopyAvailable<Qt> >{};

	template <typename Qt, int I, glm::qualifier Ql>
	struct TZeroMemAvailable< PhysicalQuantityVec<Qt,I,Ql> >	: CT_Bool< IsZeroMemAvailable<Qt> >{};

	// 'IsTriviallySerializable< PhysicalQuantityVec<> > = false' - because SIMD and packed types has different alignment

} // AE::Base
