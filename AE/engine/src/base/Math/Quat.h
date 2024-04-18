// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Radian.h"

namespace AE::Math
{

	//
	// Quaternion
	//

	template <typename T, glm::qualifier Q>
	struct TQuat final : private glm::qua< T, Q >
	{
		StaticAssert( IsScalar<T> and IsFloatPoint<T> );

	// types
	public:
		using Value_t		= T;
		using Self			= TQuat< T, Q >;
		using _GLM_Quat_t	= glm::qua< T, Q >;
		using Vec2_t		= TVec< T, 2, Q >;
		using Vec3_t		= TVec< T, 3, Q >;
		using Vec4_t		= TVec< T, 4, Q >;
		using Rad_t			= TRadian< T >;
		using Rad3_t		= RadianVec< T, 3 >;
		using Mat3_t		= TMatrix< T, 3, 3, Q >;
		using Mat4_t		= TMatrix< T, 4, 4, Q >;


	// variables
	public:
		using _GLM_Quat_t::x;
		using _GLM_Quat_t::y;
		using _GLM_Quat_t::z;
		using _GLM_Quat_t::w;


	// methods
	public:
		TQuat ()													__NE___	= default;
		TQuat (const Self &other)									__NE___	= default;
		TQuat (Self &&other)										__NE___	= default;

		explicit TQuat (const _GLM_Quat_t &val)						__NE___	: _GLM_Quat_t{val} {}
		explicit TQuat (Base::_hidden_::_Zero)						__NE___ : _GLM_Quat_t{ T{0}, T{0}, T{0}, T{0} } {}

		template <typename T2, glm::qualifier Q2>
		explicit TQuat (const TQuat<T2,Q2> &other)					__NE___	: _GLM_Quat_t{other._Base()} {}

		TQuat (T w, T x, T y, T z)									__NE___	: _GLM_Quat_t{w, x, y, z} {}

		explicit TQuat (const Rad3_t &eulerAngles)					__NE___	: _GLM_Quat_t{ Vec3_t{ T(eulerAngles.x), T(eulerAngles.y), T(eulerAngles.z) }} {}

		explicit TQuat (const Mat3_t &m)							__NE___;
		explicit TQuat (const Mat4_t &m)							__NE___;

			Self&	Inverse ()										__NE___	{ _Base() = glm::inverse( _Base() );  return *this; }
		ND_ Self	Inversed ()										C_NE___	{ return Self{ glm::inverse( _Base() )}; }

			Self&	Normalize ()									__NE___	{ _Base() = glm::normalize( _Base() );  return *this; }
		ND_ Self	Normalized ()									C_NE___	{ return Self{ glm::normalize( _Base() )}; }
		ND_ bool	IsNormalized ()									C_NE___	{ return Abs( LengthSq() - T{1} ) < T{1.0e-4}; }

			Self&	operator = (const Self &rhs)					__NE___	= default;

			using _GLM_Quat_t::operator [];

		ND_ Vec3_t	xyz ()											C_NE___	{ return Vec3_t{ x, y, z }; }

			Self&	operator += (const Self &rhs)					__NE___	{ _Base() += rhs._Base();  return *this; }
			Self&	operator -= (const Self &rhs)					__NE___	{ _Base() -= rhs._Base();  return *this; }
			Self&	operator *= (const Self &rhs)					__NE___	{ _Base() *= rhs._Base();  return *this; }

			Self&	operator *= (T rhs)								__NE___	{ _Base() *= rhs;  return *this; }
			Self&	operator /= (T rhs)								__NE___	{ _Base() /= rhs;  return *this; }

		ND_ Self	operator + ()									C_NE___	{ return *this; }
		ND_ Self	operator - ()									C_NE___	{ return Self{ -_Base() }; }

		ND_ Self	operator + (const Self &rhs)					C_NE___	{ return Self{ _Base() + rhs._Base() }; }
		ND_ Self	operator - (const Self &rhs)					C_NE___	{ return Self{ _Base() - rhs._Base() }; }
		ND_ Self	operator * (const Self &rhs)					C_NE___	{ return Self{ _Base() * rhs._Base() }; }

		ND_ Vec3_t	operator * (const Vec3_t &rhs)					C_NE___	{ return _Base() * rhs; }
		ND_ Vec4_t	operator * (const Vec4_t &rhs)					C_NE___	{ return _Base() * rhs; }

		ND_ Self	operator * (T rhs)								C_NE___	{ return Self{ _Base() * rhs }; }
		ND_ Self	operator / (T rhs)								C_NE___	{ return Self{ _Base() / rhs }; }

		ND_ bool4	operator == (const Self &rhs)					C_NE___;
		ND_ bool4	operator != (const Self &rhs)					C_NE___	{ return not (*this == rhs); }

		ND_ friend Self  operator * (T lhs, const Self &rhs)		__NE___	{ return Self{ lhs * rhs._Base() }; }

		ND_ Rad3_t	ToEuler ()										C_NE___	{ return Rad3_t{glm::eulerAngles( _Base() )}; }
		ND_ Rad_t	ToEulerX ()										C_NE___	{ return Rad_t{glm::pitch( _Base() )}; }
		ND_ Rad_t	ToEulerY ()										C_NE___	{ return Rad_t{glm::yaw( _Base() )}; }
		ND_ Rad_t	ToEulerZ ()										C_NE___	{ return Rad_t{glm::roll( _Base() )}; }

		ND_ Self	Conjugate ()									C_NE___	{ return Self{glm::conjugate( _Base() )}; }
		ND_ T		Length ()										C_NE___	{ return glm::length( _Base() ); }
		ND_ T		LengthSq ()										C_NE___	{ return glm::dot( _Base(), _Base() ); }

		ND_ Vec3_t  ToDirection ()									C_NE___	{ return AxisZ(); }

		ND_ Vec3_t  AxisX ()										C_NE___;	// right
		ND_ Vec3_t  AxisY ()										C_NE___;	// up
		ND_ Vec3_t  AxisZ ()										C_NE___;	// forward

		ND_ Self	MirrorX ()										C_NE___	{ return Self{-w, -x,  y,  z}; }
		ND_ Self	MirrorY ()										C_NE___	{ return Self{-w,  x, -y,  z}; }
		ND_ Self	MirrorZ ()										C_NE___	{ return Self{-w,  x,  y, -z}; }

		ND_ Vec3_t	Axis ()											C_NE___	{ return glm::axis( _Base() ); }
		ND_ Rad_t	Angle ()										C_NE___	{ return Rad_t{glm::angle( _Base() )}; }

		ND_ static Self  Identity ()								__NE___	{ return Self{glm::quat_identity<T,Q>()}; }

		ND_ static Self  Rotate (Rad_t angle, const Vec3_t &axis)	__NE___	{ return Self{glm::rotate( glm::quat_identity<T,Q>(), T(angle), axis )}; }
		ND_ static Self  RotateX (Rad_t angle)						__NE___	{ return Rotate( angle, Vec3_t{T{1}, T{0}, T{0}} ); }
		ND_ static Self  RotateY (Rad_t angle)						__NE___	{ return Rotate( angle, Vec3_t{T{0}, T{1}, T{0}} ); }
		ND_ static Self  RotateZ (Rad_t angle)						__NE___	{ return Rotate( angle, Vec3_t{T{0}, T{0}, T{1}} ); }
		ND_ static Self  Rotate (const Rad3_t &angle)				__NE___	{ return RotateX( angle.x ) * RotateY( angle.y ) * RotateZ( angle.z ); }
		ND_ static Self  Rotate2 (const Rad3_t &angle)				__NE___;

		ND_ static Self  LookAt (const Vec3_t &dir, const Vec3_t &up)__NE___;
		ND_ static Self  From2Normals (const Vec3_t &n1, const Vec3_t &n2)	__NE___	{ return Self{_GLM_Quat_t{ n1, n2 }}; }
		ND_ static Self  FromAngleAxis (Rad_t angle, const Vec3_t &axis)	__NE___	{ return glm::angleAxis( angle, axis ); }

		ND_ static T	CalcW (T x, T y, T z)						__NE___	{ return T{1} - Sqrt( x*x + y*y + z*z ); }

		ND_ _GLM_Quat_t&		_Base ()							__NE___	{ return *this; }
		ND_ _GLM_Quat_t const&	_Base ()							C_NE___	{ return *this; }
	};

	using Quat			= TQuat< float, GLMSimdQualifier >;
	using PackedQuat	= TQuat< float, GLMPackedQualifier >;


/*
=================================================
	_QuatInfo
=================================================
*/
namespace _hidden_
{
	template <typename T>
	struct _QuatInfo {
		using					type	= void;
		static constexpr bool	is_quat	= false;
		static constexpr bool	is_simd	= false;
	};

	template <typename T, glm::qualifier Q>
	struct _QuatInfo< TQuat<T,Q> >
	{
		using					type	= T;
		static constexpr bool	is_quat	= true;
		static constexpr bool	is_simd = false;
	};

	template <typename T>
	struct _QuatInfo< TQuat<T,GLMSimdQualifier> >
	{
		using					type	= T;
		static constexpr bool	is_quat	= true;
		static constexpr bool	is_simd = true;
	};

}
/*
=================================================
	IsQuat / IsSimdQuat
=================================================
*/
	template <typename T>
	static constexpr bool	IsQuat = Math::_hidden_::_QuatInfo<T>::is_quat;

	template <typename T>
	static constexpr bool	IsSimdQuat = Math::_hidden_::_QuatInfo<T>::is_simd;

/*
=================================================
	QuatToScalarType
=================================================
*/
	template <typename T>
	using QuatToScalarType = typename Math::_hidden_::_QuatInfo<T>::type;

/*
=================================================
	Dot
=================================================
*/
	template <typename T, glm::qualifier Q>
	ND_ T  Dot (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs) __NE___
	{
		return glm::dot( lhs, rhs );
	}

/*
=================================================
	Cross
=================================================
*/
	template <typename T, glm::qualifier Q>
	ND_ TQuat<T,Q>  Cross (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs) __NE___
	{
		return TQuat<T,Q>{ glm::cross( lhs, rhs )};
	}

/*
=================================================
	operator ==
=================================================
*/
	template <typename T, glm::qualifier Q>
	bool4  TQuat<T,Q>::operator == (const Self &rhs) C_NE___
	{
		return bool4{ x == rhs.x,
					  y == rhs.y,
					  z == rhs.z,
					  w == rhs.w };
	}

/*
=================================================
	Equal
=================================================
*/
	template <typename T, glm::qualifier Q>
	ND_ bool4  Equal (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs, const T err = Epsilon<T>()) __NE___
	{
		return bool4{
				Math::Equal( lhs.x, rhs.x, err ),
				Math::Equal( lhs.y, rhs.y, err ),
				Math::Equal( lhs.z, rhs.z, err ),
				Math::Equal( lhs.w, rhs.w, err )};
	}

	template <typename T, glm::qualifier Q>
	ND_ bool4  Equal (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs, const Percent err) __NE___
	{
		return bool4{
				Math::Equal( lhs.x, rhs.x, err ),
				Math::Equal( lhs.y, rhs.y, err ),
				Math::Equal( lhs.z, rhs.z, err ),
				Math::Equal( lhs.w, rhs.w, err )};
	}

/*
=================================================
	BitEqual
=================================================
*/
	template <typename T, glm::qualifier Q>
	ND_ bool4  BitEqual (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs, const EnabledBitCount bitCount) __NE___
	{
		return bool4{
				Math::BitEqual( lhs.x, rhs.x, bitCount ),
				Math::BitEqual( lhs.y, rhs.y, bitCount ),
				Math::BitEqual( lhs.z, rhs.z, bitCount ),
				Math::BitEqual( lhs.w, rhs.w, bitCount )};
	}

	template <typename T, glm::qualifier Q>
	ND_ bool4  BitEqual (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs) __NE___
	{
		return bool4{
				Math::BitEqual( lhs.x, rhs.x ),
				Math::BitEqual( lhs.y, rhs.y ),
				Math::BitEqual( lhs.z, rhs.z ),
				Math::BitEqual( lhs.w, rhs.w )};
	}

/*
=================================================
	IsInfinity / IsNaN / IsFinite
=================================================
*/
	template <typename T, glm::qualifier Q>
	ND_ bool4  IsInfinity (const TQuat<T,Q> &q) __NE___
	{
		return glm::isinf( q );
	}

	template <typename T, glm::qualifier Q>
	ND_ bool4  IsNaN (const TQuat<T,Q> &q) __NE___
	{
		return glm::isnan( q );
	}

	template <typename T, glm::qualifier Q>
	ND_ bool4  IsFinite (const TQuat<T,Q> &q) __NE___
	{
		return (q == q);
	}

/*
=================================================
	Lerp
=================================================
*/
	template <typename T, glm::qualifier Q>
	ND_ TQuat<T,Q>  Lerp (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs, const T factor) __NE___
	{
		return TQuat<T,Q>{glm::lerp( lhs, rhs, factor )};
	}

/*
=================================================
	SLerp
=================================================
*/
	template <typename T, glm::qualifier Q>
	ND_ TQuat<T,Q>  SLerp (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs, const T a) __NE___
	{
		return TQuat<T,Q>{glm::slerp( lhs, rhs, a )};
	}

	template <typename T, glm::qualifier Q>
	ND_ TQuat<T,Q>  SLerp (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs, const T a, const T k) __NE___
	{
		return TQuat<T,Q>{glm::slerp( lhs, rhs, a, k )};
	}

/*
=================================================
	Rotate2
=================================================
*/
	template <typename T, glm::qualifier Q>
	TQuat<T,Q>  TQuat<T,Q>::Rotate2 (const Rad3_t &angle) __NE___
	{
		const Vec2_t	scr  = SinCos( angle.x * T(0.5) );
		const Vec2_t	scp  = SinCos( angle.y * T(0.5) );
		const Vec2_t	scy  = SinCos( angle.z * T(0.5) );

		const T			cpcy = scp[1] * scy[1];
		const T			spcy = scp[0] * scy[1];
		const T			cpsy = scp[1] * scy[0];
		const T			spsy = scp[0] * scy[0];

		Self			result;

		result.w = scr[1] * cpcy + scr[0] * spsy;
		result.x = scr[0] * cpcy - scr[1] * spsy;
		result.y = scr[1] * spcy + scr[0] * cpsy;
		result.z = scr[1] * cpsy - scr[0] * spcy;

		return result.Normalize();
	}

/*
=================================================
	Axis*
=================================================
*/
	template <typename T, glm::qualifier Q>
	typename TQuat<T,Q>::Vec3_t  TQuat<T,Q>::AxisX () C_NE___
	{
		return Vec3_t{	T{1} - T{2} * (y * y + z * z),
						T{2} * (x * y - w * z),
						T{2} * (x * z + w * y) };
	}

	template <typename T, glm::qualifier Q>
	typename TQuat<T,Q>::Vec3_t  TQuat<T,Q>::AxisY () C_NE___
	{
		return Vec3_t{	T{2} * (x * y + w * z),
						T{1} - T{2} * (x * x + z * z),
						T{2} * (y * z - w * x) };
	}

	template <typename T, glm::qualifier Q>
	typename TQuat<T,Q>::Vec3_t  TQuat<T,Q>::AxisZ () C_NE___
	{
		return Vec3_t{	T{2} * (x * z - w * y),
						T{2} * (y * z + w * x),
						T{1} - T{2} * (x * x + y * y) };
	}


} // AE::Math
