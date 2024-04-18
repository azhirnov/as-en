// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Transformation.h"
#include "base/Math/Rectangle.h"

namespace AE::Math
{

	//
	// Camera
	//

	template <typename T>
	struct TCamera
	{
	// types
		using Transform_t	= TTransformation<T>;
		using Mat4_t		= typename Transform_t::Mat4_t;
		using Vec3_t		= typename Transform_t::Vec3_t;
		using Quat_t		= typename Transform_t::Quat_t;
		using Rect_t		= Rectangle<T>;
		using Vec2_t		= Vec< T, 2 >;
		using Rad_t			= TRadian<T>;
		using Self			= TCamera<T>;
		using Value_t		= T;


	// variables
		Transform_t		transform;
		Mat4_t			projection;


	// methods
		TCamera ()																			__NE___	{}

		ND_ Quat_t const&	Orientation ()													C_NE___	{ return transform.orientation; }

		ND_ Mat4_t  ToModelViewProjMatrix ()												C_NE___	{ return projection * ToModelViewMatrix(); }
		ND_ Mat4_t	ToViewProjMatrix ()														C_NE___	{ return projection * ToViewMatrix(); }
		ND_ Mat4_t	ToViewMatrix ()															C_NE___	{ return transform.ToRotationMatrix(); }
		ND_ Mat4_t  ToModelMatrix ()														C_NE___	{ return Mat4_t::Translated( transform.position ); }
		ND_ Mat4_t  ToModelViewMatrix ()													C_NE___;


		// for transformation
		Self&  Move (const Vec3_t &delta)													__NE___	{ transform.Move( delta );  return *this; }
		Self&  SetPosition (const Vec3_t &value)											__NE___	{ transform.position = value;  return *this; }
		Self&  Rotate (const Quat_t &delta)													__NE___	{ transform.Rotate( delta );  return *this; }
		Self&  Rotate (Rad_t angle, const Vec3_t &normal)									__NE___;
		Self&  SetOrientation (const Quat_t &value)											__NE___	{ transform.orientation = value;  return *this;}


		// for projection
		Self&  SetOrtho (const Rect_t &viewport, const Vec2_t &range)						__NE___;

		Self&  SetPerspective (Rad_t fovY, Value_t aspect,
							   const Vec2_t &range, Bool reverseZ = False{})				__NE___;
		Self&  SetPerspective (Rad_t fovY, const Vec2_t &viewport,
							   const Vec2_t &range, Bool reverseZ = False{})				__NE___;

		Self&  SetFrustum (const Rect_t &viewport, const Vec2_t &range,
						   Bool reverseZ = False{})											__NE___;


		ND_ static Vec3_t  UpDir ()															__NE___	{ return Vec3_t( T(0), T(1), T(0) ); }
		ND_ static Vec3_t  RightDir ()														__NE___	{ return Vec3_t( T(1), T(0), T(0) ); }
		ND_ static Vec3_t  ForwardDir ()													__NE___	{ return Vec3_t( T(0), T(0), T(1) ); }
	};



/*
=================================================
	ToModelViewMatrix
=================================================
*/
	template <typename T>
	typename TCamera<T>::Mat4_t  TCamera<T>::ToModelViewMatrix () C_NE___
	{
		ASSERT( BitEqual( transform.scale, T(1) ));
		Mat4_t	orient_mat		{ transform.orientation };
		Mat4_t	translate_mat	= Mat4_t::Translated( transform.position );
		return orient_mat * translate_mat;
	}

/*
=================================================
	Rotate
=================================================
*/
	template <typename T>
	TCamera<T>&  TCamera<T>::Rotate (Rad_t angle, const Vec3_t &normal) __NE___
	{
		transform.Rotate( Quat_t::Identity().Rotate( angle, normal ));
		return *this;
	}

/*
=================================================
	SetOrtho
=================================================
*/
	template <typename T>
	TCamera<T>&  TCamera<T>::SetOrtho (const Rect_t &viewport, const Vec2_t &range) __NE___
	{
		projection = Mat4_t::Ortho( viewport, range );
		return *this;
	}

/*
=================================================
	SetPerspective
=================================================
*/
	template <typename T>
	TCamera<T>&  TCamera<T>::SetPerspective (Rad_t fovY, Value_t aspect, const Vec2_t &range, Bool reverseZ) __NE___
	{
		if ( IsFinite( range.y ))
			projection = Mat4_t::Perspective( fovY, aspect, range );
		else
			projection = Mat4_t::InfinitePerspective( fovY, aspect, range.x );
		if ( reverseZ )
			projection = Mat4_t::ReverseZTransform() * projection;
		return *this;
	}

	template <typename T>
	TCamera<T>&  TCamera<T>::SetPerspective (Rad_t fovY, const Vec2_t &viewport, const Vec2_t &range, Bool reverseZ) __NE___
	{
		projection = Mat4_t::Perspective( fovY, viewport, range );
		if ( reverseZ )
			projection = Mat4_t::ReverseZTransform() * projection;
		return *this;
	}

/*
=================================================
	SetFrustum
=================================================
*/
	template <typename T>
	TCamera<T>&  TCamera<T>::SetFrustum (const Rect_t &viewport, const Vec2_t &range, Bool reverseZ) __NE___
	{
		if ( IsFinite( range.y ))
			projection = Mat4_t::Frustum( viewport, range );
		else
			projection = Mat4_t::InfiniteFrustum( viewport, range.x );
		if ( reverseZ )
			projection = Mat4_t::ReverseZTransform() * projection;
		return *this;
	}


} // AE::Math
