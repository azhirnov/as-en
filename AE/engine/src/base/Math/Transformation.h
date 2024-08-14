// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Quat.h"
#include "base/Math/Matrix.h"

namespace AE::Math
{

	//
	// Transformation
	//

	template <typename T>
	struct TTransformation
	{
		StaticAssert( IsFloatPoint<T> );

	// types
	public:
		using Value_t	= T;
		using Vec3_t	= Vec< T, 3 >;
		using Quat_t	= TQuat< T >;
		using Mat4_t	= Matrix< T, 4, 4 >;
		using Self		= TTransformation< T >;


	// variables
	public:
		Quat_t		orientation		{Quat_t::Identity()};
		Vec3_t		position		{ T{0} };
		Value_t		scale			{ T{1} };


	// methods
	public:
		TTransformation ()																__NE___	= default;
		TTransformation (const Self &)													__NE___	= default;
		TTransformation (Self &&)														__NE___	= default;

		TTransformation (const Vec3_t &pos, const Quat_t &orient, const T &scale = T{1})__NE___ : orientation{orient}, position{pos}, scale{scale} {}

		template <typename B>
		explicit TTransformation (const TTransformation<B> &other)						__NE___ : orientation{other.orientation}, position{other.position}, scale{T(other.scale)} {}

		explicit TTransformation (const Mat4_t &mat)									__NE___;

			constexpr Self&  operator =  (const Self &)									__NE___	= default;
			constexpr Self&  operator =  (Self &&)										__NE___	= default;

			Self&	operator += (const Self &rhs)										__NE___;
		ND_ Self	operator +  (const Self &rhs)										C_NE___;

			Self&	operator -= (const Self &rhs)										__NE___	{ return Self{*this} += rhs.Inversed(); }
		ND_ Self	operator -  (const Self &rhs)										C_NE___	{ return *this + rhs.Inversed(); }

		ND_ bool	operator == (const Self &rhs)										C_NE___;
		ND_ bool	operator != (const Self &rhs)										C_NE___	{ return not (*this == rhs); }

			Self&	Move (const Vec3_t &delta)											__NE___	{ position += orientation * (delta * scale);  return *this; }
			Self&	Rotate (const Quat_t &delta)										__NE___	{ orientation *= delta;  return *this; }
			Self&	Scale (const T value)												__NE___	{ scale *= value;  return *this; }

			Self&	Inverse ()															__NE___;
		ND_ Self	Inversed ()															C_NE___	{ return Self{*this}.Inverse(); }

		ND_ Mat4_t	ToMatrix ()															C_NE___;															// position transform
		ND_ Mat4_t	ToRotationMatrix ()													C_NE___	{ return Mat4_t{ orientation }; }							// normals transform
		ND_ Mat4_t	ToRotationScaleMatrix ()											C_NE___	{ return Mat4_t{ orientation } * Mat4_t::Scaled( scale ); }	// view matrix
		ND_ Mat4_t	ToModelMatrix ()													C_NE___	{ return Mat4_t::Translated( position ); }

		ND_ bool	IsIdentity ()														C_NE___	{ return Equal( *this, Self{} ); }


		// local space to global
		ND_ Vec3_t	ToGlobalVector (const Vec3_t &local)								C_NE___	{ return orientation * (local * scale); }
		ND_ Vec3_t	ToGlobalPosition (const Vec3_t &local)								C_NE___	{ return ToGlobalVector( local ) + position; }

		// global space to local
		ND_ Vec3_t	ToLocalVector (const Vec3_t &global)								C_NE___	{ return (orientation.Inversed() * global) / scale; }
		ND_ Vec3_t	ToLocalPosition (const Vec3_t &global)								C_NE___	{ return ToLocalVector( global - position ); }
	};


/*
=================================================
	constructor (mat4x4)
=================================================
*/
	template <typename T>
	TTransformation<T>::TTransformation (const Mat4_t &mat) __NE___
	{
		Vec3_t		scale3;
		Vec3_t		skew;
		Vec<T,4>	perspective;
		bool		ok = glm::decompose( mat._value, OUT scale3, OUT orientation._Base(), OUT position, OUT skew, OUT perspective );

		ASSERT( ok );  Unused( ok );
		ASSERT( Equal( scale3.x, scale3.y ) and Equal( scale3.x, scale3.z ));
		scale = scale3.x;
	}

/*
=================================================
	operator + / operator +=
----
	same as 'lhs.ToMatrix() * rhs.ToMatrix()'
=================================================
*/
	template <typename T>
	TTransformation<T>&  TTransformation<T>::operator += (const Self &rhs) __NE___
	{
		position	+= orientation * (rhs.position * scale);
		orientation	*= rhs.orientation;
		scale		*= rhs.scale;
		return *this;
	}

	template <typename T>
	TTransformation<T>  TTransformation<T>::operator + (const Self &rhs) C_NE___
	{
		return	TTransformation<T>{
					this->position + this->orientation * (rhs.position * this->scale),
					this->orientation * rhs.orientation,
					this->scale * rhs.scale };
	}

/*
=================================================
	operator ==
=================================================
*/
	template <typename T>
	bool TTransformation<T>::operator == (const Self &rhs) C_NE___
	{
		return	All( orientation == rhs.orientation )	&
				All( position	 == rhs.position )		&
				(scale			 == rhs.scale);
	}

/*
=================================================
	Equal
=================================================
*/
	template <typename T>
	ND_ bool  Equal (const TTransformation<T> &lhs, const TTransformation<T> &rhs, const T err = Epsilon<T>()) __NE___
	{
		return	All( Math::Equal( lhs.orientation, rhs.orientation, err ))	&
				All( Math::Equal( lhs.position, rhs.position, err ))		&
				Math::Equal( lhs.scale, rhs.scale, err );
	}

	template <typename T>
	ND_ bool  Equal (const TTransformation<T> &lhs, const TTransformation<T> &rhs, const Percent err) __NE___
	{
		return	All( Math::Equal( lhs.orientation, rhs.orientation, err ))	&
				All( Math::Equal( lhs.position, rhs.position, err ))		&
				Math::Equal( lhs.scale, rhs.scale, err );
	}

/*
=================================================
	BitEqual
=================================================
*/
	template <typename T>
	ND_ bool  BitEqual (const TTransformation<T> &lhs, const TTransformation<T> &rhs, const EnabledBitCount bitCount) __NE___
	{
		return	All( Math::BitEqual( lhs.orientation, rhs.orientation, bitCount ))	&
				All( Math::BitEqual( lhs.position, rhs.position, bitCount ))		&
				Math::BitEqual( lhs.scale, rhs.scale, bitCount );
	}

	template <typename T>
	ND_ bool  BitEqual (const TTransformation<T> &lhs, const TTransformation<T> &rhs) __NE___
	{
		return	All( Math::BitEqual( lhs.orientation, rhs.orientation ))	&
				All( Math::BitEqual( lhs.position, rhs.position ))			&
				Math::BitEqual( lhs.scale, rhs.scale );
	}

/*
=================================================
	Inverse
=================================================
*/
	template <typename T>
	TTransformation<T>&  TTransformation<T>::Inverse () __NE___
	{
		ASSERT( Math::IsNotZero( scale ));

		orientation.Inverse();
		scale		= T{1} / scale;
		position	= orientation * (-position * scale);
		return *this;
	}

/*
=================================================
	ToMatrix
=================================================
*/
	template <typename T>
	typename TTransformation<T>::Mat4_t  TTransformation<T>::ToMatrix () C_NE___
	{
		Mat4_t	result{ orientation };

		result[3] = Vec<T,4>{ position, T{1} };

		return result * Mat4_t::Scaled( scale );
	}


} // AE::Math


namespace AE::Base
{
	template <typename T>	struct TMemCopyAvailable< TTransformation<T> >	: CT_Bool< IsMemCopyAvailable<T> >{};
	template <typename T>	struct TZeroMemAvailable< TTransformation<T> >	: CT_Bool< IsZeroMemAvailable<T> >{};

	// 'IsTriviallySerializable< TTransformation<> > = false' - because SIMD and packed types has different alignment

} // AE::Base
