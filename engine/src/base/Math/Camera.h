// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Transformation.h"
#include "base/Math/Radians.h"
#include "base/Math/Rectangle.h"

namespace AE::Math
{

	//
	// Camera
	//

	template <typename T>
	struct CameraTempl
	{
	// types
		using Transform_t	= Transformation<T>;
		using Mat4_t		= typename Transform_t::Mat4_t;
		using Vec3_t		= typename Transform_t::Vec3_t;
		using Quat_t		= typename Transform_t::Quat_t;
		using Rect_t		= Rectangle<T>;
		using Vec2_t		= Vec< T, 2 >;
		using Radians_t		= RadiansTempl<T>;
		using Self			= CameraTempl<T>;
		using Value_t		= T;


	// variables
		Transform_t		transform;
		Mat4_t			projection;


	// methods
		CameraTempl () {}
		
		ND_ Mat4_t  ToModelViewProjMatrix ()	const	{ return projection * ToModelViewMatrix(); }
		ND_ Mat4_t	ToViewProjMatrix ()			const	{ return projection * transform.ToRotationMatrix(); }
		ND_ Mat4_t	ToViewMatrix ()				const	{ return transform.ToRotationMatrix(); }
		
		ND_ Mat4_t  ToModelViewMatrix ()		const
		{
			Mat4_t	orient_mat		{ transform.orientation };
			Mat4_t	translate_mat	= Mat4_t::Translate( transform.position );
			Mat4_t	scale_mat		= Mat4_t::Scale( Vec3_t{ transform.scale });
			return orient_mat * scale_mat * translate_mat;
		}


		// for transformation
		Self&	Move (const Vec3_t &delta)
		{
			transform.Move( delta );
			return *this;
		}

		Self&  SetPosition (const Vec3_t &value)
		{
			transform.position = value;
			return *this;
		}

		Self&	Rotate (const Quat_t &delta)
		{
			transform.Rotate( delta );
			return *this;
		}

		Self&  Rotate (Radians_t angle, const Vec3_t &normal)
		{
			transform.Rotate( Quat_t::Identity().Rotate( angle, normal ));
			return *this;
		}

		Self&  SetOrientation (const Quat_t &value)
		{
			transform.orientation = value;
			return *this;
		}


		// for projection
		Self&  SetOrtho (const Rect_t &viewport, const Vec2_t &range)
		{
			projection = Mat4_t::Ortho( viewport, range );
			return *this;
		}

		Self&  SetPerspective (Radians_t fovY, Value_t aspect, Value_t zNear)
		{
			projection = Mat4_t::InfinitePerspective( fovY, aspect, zNear );
			return *this;
		}

		Self&  SetPerspective (Radians_t fovY, Value_t aspect, const Vec2_t &range)
		{
			projection = Mat4_t::Perspective( fovY, aspect, range );
			return *this;
		}

		Self&  SetPerspective (Radians_t fovY, const Vec2_t &viewport, const Vec2_t &range)
		{
			projection = Mat4_t::Perspective( fovY, viewport, range );
			return *this;
		}

		Self&  SetFrustum (const Rect_t &viewport, const Vec2_t &range)
		{
			projection = Mat4_t::Frustum( viewport, range );
			return *this;
		}
	};


} // AE::Math
