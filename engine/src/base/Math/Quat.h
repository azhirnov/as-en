// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Radians.h"

#ifdef AE_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning (disable: 4201)
#endif

namespace AE::Math
{

	template <typename T>
	struct Quat
	{
		STATIC_ASSERT( IsScalar<T> );
		STATIC_ASSERT( IsFloatPoint<T> );

	// types
	public:
		using Value_t		= T;
		using Self			= Quat< T >;
		using _GLM_Quat_t	= glm::qua< T, GLMQuialifier >;
		using Vec3_t		= Vec< T, 3 >;
		using Vec4_t		= Vec< T, 4 >;
		using Rad_t			= RadiansTempl< T >;
		using Rad3_t		= RadianVec< T, 3 >;


	// variables
	public:
		union {
			_GLM_Quat_t		_value;
			struct {
				T			x, y, z, w;
			};
		};


	// methods
	public:
		Quat ()												__NE___	: _value{} {}
		Quat (const Self &other)							__NE___	: _value{other._value} {}
		
		explicit Quat (const _GLM_Quat_t &val)				__NE___	: _value{val} {}

		template <typename B>
		explicit Quat (const Quat<B> &other)				__NE___	: _value{other._value} {}

		Quat (Self &&other)									__NE___	: _value{other._value} {} 

		Quat (T w, T x, T y, T z)							__NE___	: _value{ w, x, y, z } {}

		explicit Quat (const Rad3_t &eulerAngles)			__NE___	: _value{ Vec3_t{ T(eulerAngles.x), T(eulerAngles.y), T(eulerAngles.z) }} {}
		
		template <glm::qualifier Q>	explicit Quat (const TMatrix<T,3,3,Q> &m) __NE___;
		template <glm::qualifier Q>	explicit Quat (const TMatrix<T,4,4,Q> &m) __NE___;

			Self&	Inverse ()								__NE___	{ _value = glm::inverse( _value );  return *this; }
		ND_ Self	Inversed ()								C_NE___	{ return Self{ glm::inverse( _value )}; }
		
			Self&	Normalize ()							__NE___	{ _value = glm::normalize( _value );  return *this; }
		ND_ Self	Normalized ()							C_NE___	{ return Self{ glm::normalize( _value )}; }

			Self&	operator = (const Self &rhs)			__NE___	{ _value = rhs._value;  return *this; }

		ND_ Value_t& operator [] (usize index)				__NE___	{ return _value[index]; }
		ND_ Value_t	 operator [] (usize index)				C_NE___	{ return _value[index]; }
		
			Self&	operator += (const Self &rhs)			__NE___	{ _value += rhs._value;  return *this; }
			Self&	operator -= (const Self &rhs)			__NE___	{ _value -= rhs._value;  return *this; }
			Self&	operator *= (const Self &rhs)			__NE___	{ _value *= rhs._value;  return *this; }

			Self&	operator *= (T rhs)						__NE___	{ _value *= rhs;  return *this; }
			Self&	operator /= (T rhs)						__NE___	{ _value /= rhs;  return *this; }

		ND_ Self	operator + ()							C_NE___	{ return *this; }
		ND_ Self	operator - ()							C_NE___	{ return Self{ -_value }; }

		ND_ Self	operator + (const Self &rhs)			C_NE___	{ return Self{ _value + rhs._value }; }
		ND_ Self	operator - (const Self &rhs)			C_NE___	{ return Self{ _value - rhs._value }; }
		ND_ Self	operator * (const Self &rhs)			C_NE___	{ return Self{ _value * rhs._value }; }

		ND_ Vec3_t	operator * (const Vec3_t &rhs)			C_NE___	{ return _value * rhs; }
		ND_ Vec4_t	operator * (const Vec4_t &rhs)			C_NE___	{ return _value * rhs; }
		
		ND_ Self	operator * (T rhs)						C_NE___	{ return Self{ _value * rhs }; }
		ND_ Self	operator / (T rhs)						C_NE___	{ return Self{ _value / rhs }; }
		
		ND_ bool4	operator == (const Self &rhs)			C_NE___;
		ND_ bool4	operator != (const Self &rhs)			C_NE___	{ return not (*this == rhs); }

		ND_ friend Self	operator * (T lhs, const Self &rhs)	__NE___	{ return Self{ lhs * rhs._value }; }

		ND_ Self&	Rotate (Rad_t angle, const Vec3_t &axis)__NE___	{ _value = glm::rotate( _value, T(angle), axis );  return *this; }

		ND_ Self	Conjugate ()							C_NE___	{ return Self{ glm::conjugate( _value )}; }
		ND_ T		Length ()								C_NE___	{ return glm::length( _value ); }

		ND_ Vec3_t	ToDirection ()							C_NE___;

		ND_ Self	Slerp (const Self &q, T a)				C_NE___	{ return Self{ glm::mix( _value, q._value, a )}; }

		ND_  static Self  Identity ()						__NE___	{ return Self{ _GLM_Quat_t( T{1}, T{0}, T{0}, T{0} )}; }

		ND_ static Self  RotateX (Rad_t angle)				__NE___	{ return Self{ glm::rotate( Identity()._value, T(angle), Vec3_t{T{1}, T{0}, T{0}} )}; }
		ND_ static Self  RotateY (Rad_t angle)				__NE___	{ return Self{ glm::rotate( Identity()._value, T(angle), Vec3_t{T{0}, T{1}, T{0}} )}; }
		ND_ static Self  RotateZ (Rad_t angle)				__NE___	{ return Self{ glm::rotate( Identity()._value, T(angle), Vec3_t{T{0}, T{0}, T{1}} )}; }
		ND_ static Self  Rotate (const Rad3_t &angle)		__NE___	{ return RotateX( angle.x ) * RotateY( angle.y ) * RotateZ( angle.z ); }

		ND_ static Self  FromDirection (const Vec3_t &dir, const Vec3_t &up) __NE___;
	};

	using QuatF = Quat< float >;

	
/*
=================================================
	Dot
=================================================
*/
	template <typename T>
	ND_ inline T  Dot (const Quat<T> &lhs, const Quat<T> &rhs) __NE___
	{
		return glm::dot( lhs._value, rhs._value );
	}

/*
=================================================
	Cross
=================================================
*/
	template <typename T>
	ND_ inline Quat<T>  Cross (const Quat<T> &lhs, const Quat<T> &rhs) __NE___
	{
		return Quat<T>{ glm::cross( lhs._value, rhs._value )};
	}
	
/*
=================================================
	operator ==
=================================================
*/
	template <typename T>
	inline bool4  Quat<T>::operator == (const Self &rhs) C_NE___
	{
		return bool4{ x == rhs.x, y == rhs.y, z == rhs.z, w == rhs.w };
	}

/*
=================================================
	Equals
=================================================
*/
	template <typename T>
	ND_ forceinline bool4  Equals (const Quat<T> &lhs, const Quat<T> &rhs, const T &err = Epsilon<T>()) __NE___
	{
		return bool4{
				Math::Equals( lhs.x, rhs.x, err ),
				Math::Equals( lhs.y, rhs.y, err ),
				Math::Equals( lhs.z, rhs.z, err ),
				Math::Equals( lhs.w, rhs.w, err )};
	}
	
/*
=================================================
	ToDirection
=================================================
*/
	template <typename T>
	inline Vec<T,3>  Quat<T>::ToDirection () C_NE___
	{
		return Vec3_t(	T{2} * x * z + T{2} * y * w,
						T{2} * z * y - T{2} * x * w,
						T{1} - T{2} * x * x - T{2} * y * y );
	}

/*
=================================================
	FromDirection
=================================================
*/
	template <typename T>
	inline Quat<T>  Quat<T>::FromDirection (const Vec3_t &dir, const Vec3_t &up) __NE___
	{
		Vec3_t	hor = Math::Normalize( Cross( up, dir ));
		Vec3_t	ver = Math::Normalize( Cross( dir, hor ));

		return Self{glm::quat_cast( glm::tmat3x3<T>{ hor, ver, dir })};
	}


} // AE::Math

#ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
#endif
