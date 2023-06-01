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
	struct Transformation
	{
	// types
	public:
		using Value_t	= T;
		using Vec3_t	= Vec< T, 3 >;
		using Quat_t	= Quat< T >;
		using Mat4_t	= Matrix< T, 4, 4 >;
		using Self		= Transformation< T >;


	// variables
	public:
		Quat_t		orientation		{Quat_t::Identity()};
		Vec3_t		position		{ T{0} };
		Value_t		scale			{ T{1} };


	// methods
	public:
		constexpr Transformation ()							__NE___	= default;
		constexpr Transformation (const Self &)				__NE___	= default;
		constexpr Transformation (Self &&)					__NE___	= default;

		constexpr Transformation (const Vec3_t &pos, const Quat_t &orient, const T &scale = T{1}) __NE___ :
			orientation{orient}, position{pos}, scale{scale} {}

		explicit Transformation (const Mat4_t &mat)			__NE___;
		
			constexpr Self&	operator =  (const Self &)		__NE___	= default;
			constexpr Self&	operator =  (Self &&)			__NE___	= default;

			Self&	operator += (const Self &rhs)			__NE___;
		ND_ Self	operator +  (const Self &rhs)			C_NE___	{ return Self{*this} += rhs; }
		
			Self&	operator -= (const Self &rhs)			__NE___	{ return Self{*this} += rhs.Inversed(); }
		ND_ Self	operator -  (const Self &rhs)			C_NE___	{ return Self{*this} -= rhs; }

		ND_ bool	operator == (const Self &rhs)			C_NE___;
		ND_ bool	operator != (const Self &rhs)			C_NE___	{ return not (*this == rhs); }

			Self&	Move (const Vec3_t &delta)				__NE___;
			Self&	Rotate (const Quat_t &delta)			__NE___;
			Self&	Scale (float scale)						__NE___;

			Self&	Inverse ()								__NE___;
		ND_ Self	Inversed ()								C_NE___	{ return Self{*this}.Inverse(); }

		ND_ Mat4_t	ToMatrix ()								C_NE___;
		ND_ Mat4_t	ToRotationMatrix ()						C_NE___;

		ND_ bool	IsIdentity ()							C_NE___;


		// local space to global
		ND_ Vec3_t	ToGlobalVector (const Vec3_t &local)	C_NE___;
		ND_ Vec3_t	ToGlobalPosition (const Vec3_t &local)	C_NE___	{ return ToGlobalVector( local ) + position; }

		// global space to local
		ND_ Vec3_t	ToLocalVector (const Vec3_t &global)	C_NE___;
		ND_ Vec3_t	ToLocalPosition (const Vec3_t &global)	C_NE___	{ return ToLocalVector( global - position ); }
	};

	
	using Transform = Transformation<float>;

	
/*
=================================================
	constructor (mat4x4)
=================================================
*/
	template <typename T>
	inline Transformation<T>::Transformation (const Mat4_t &mat) __NE___
	{
		Vec3_t		scale3;
		Vec3_t		skew;
		Vec<T,4>	perspective;
		Unused( glm::decompose( mat._value, OUT scale3, OUT orientation._value, OUT position, OUT skew, OUT perspective ));

		ASSERT( Equals( scale3.x, scale3.y ) and Equals( scale3.x, scale3.z ));
		scale = scale3.x;
	}
	
/*
=================================================
	operator +=
=================================================
*/
	template <typename T>
	inline Transformation<T>&  Transformation<T>::operator += (const Self &rhs) __NE___
	{
		position	+= orientation * (rhs.position * scale);
		orientation	*= rhs.orientation;
		scale		*= rhs.scale;
		return *this;
	}
	
/*
=================================================
	operator ==
=================================================
*/
	template <typename T>
	inline bool Transformation<T>::operator == (const Self &rhs) C_NE___
	{
		return	All( orientation == rhs.orientation )	&
				All( position	 == rhs.position )		&
				(scale			 == rhs.scale);
	}
	
/*
=================================================
	Equals
=================================================
*/
	template <typename T>
	ND_ inline bool  Equals (const Transformation<T> &lhs, const Transformation<T> &rhs, const T &err = Epsilon<T>()) __NE___
	{
		return	All( Math::Equals( lhs.orientation, rhs.orientation, err ))	&
				All( Math::Equals( lhs.position, rhs.position, err ))		&
				Math::Equals( lhs.scale, rhs.scale, err );
	}

/*
=================================================
	Move
=================================================
*/
	template <typename T>
	inline Transformation<T>&  Transformation<T>::Move (const Vec3_t &delta) __NE___
	{
		position += orientation * (delta * scale);
		return *this;
	}
	
/*
=================================================
	Rotate
=================================================
*/
	template <typename T>
	inline Transformation<T>&  Transformation<T>::Rotate (const Quat_t &delta) __NE___
	{
		orientation *= delta;
		return *this;
	}
	
/*
=================================================
	Scale
=================================================
*/
	template <typename T>
	inline Transformation<T>&  Transformation<T>::Scale (float value) __NE___
	{
		this->scale *= value;
		return *this;
	}
	
/*
=================================================
	Inverse
=================================================
*/
	template <typename T>
	inline Transformation<T>&  Transformation<T>::Inverse () __NE___
	{
		ASSERT( Math::IsNotZero( scale ));

		orientation.Inverse();
		scale		= T{1} / scale;
		position	= orientation * (-position * scale);
		return *this;
	}
	
/*
=================================================
	ToGlobalVector
=================================================
*/
	template <typename T>
	inline typename Transformation<T>::Vec3_t
		Transformation<T>::ToGlobalVector (const Vec3_t &local) C_NE___
	{
		return orientation * (local * scale);
	}
	
/*
=================================================
	ToLocalVector
=================================================
*/
	template <typename T>
	inline typename Transformation<T>::Vec3_t
		Transformation<T>::ToLocalVector (const Vec3_t &global) C_NE___
	{
		return (orientation.Inversed() * global) / scale;
	}
	
/*
=================================================
	ToMatrix
=================================================
*/
	template <typename T>
	inline typename Transformation<T>::Mat4_t  Transformation<T>::ToMatrix () C_NE___
	{
		Mat4_t	result{ orientation };
		
		result[3] = Vec<T,4>{ position, T{1} };

		return result * Mat4_t::Scale( scale );
	}
	
/*
=================================================
	ToRotationMatrix
=================================================
*/
	template <typename T>
	inline typename Transformation<T>::Mat4_t  Transformation<T>::ToRotationMatrix () C_NE___
	{
		return Mat4_t{ orientation } * Mat4_t::Scale( scale );
	}
	
/*
=================================================
	IsIdentity
=================================================
*/
	template <typename T>
	bool  Transformation<T>::IsIdentity () C_NE___
	{
		return Equals( *this, Self{} );
	}


} // AE::Math


namespace AE::Base
{
	template <typename T>	struct TMemCopyAvailable< Transformation<T> >		{ static constexpr bool  value = IsMemCopyAvailable<T>; };
	template <typename T>	struct TZeroMemAvailable< Transformation<T> >		{ static constexpr bool  value = IsZeroMemAvailable<T>; };
	template <typename T>	struct TTriviallySerializable< Transformation<T> >	{ static constexpr bool  value = IsTriviallySerializable<T>; };
	
} // AE::Base
