// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Transformation.h"

namespace AE::Math
{
	template <typename T> struct AxisAlignedBoundingBox;



	//
	// Sphere
	//

	template <typename T>
	struct BoundingSphere
	{
	// types
		using Self		= BoundingSphere<T>;
		using Vec3_t	= Vec<T,3>;
		using Value_t	= T;


	// variables
		Vec3_t		center	{T{0}};
		Value_t		radius	= T{0};


	// methods
		BoundingSphere () {}
		BoundingSphere (const Vec3_t &center, T radius) : center{center}, radius{abs(radius)} {}


		Self&  Move (const Vec3_t &delta)		{ center += delta;  return *this; }
		Self&  Scale (T scale)					{ radius *= Abs(scale);  return *this; }
		

		Self&  Transform (const Transformation<T> &tr)
		{
			center  = tr.ToGlobalPosition( center );
			radius *= tr.scale;
			return *this;
		}

		ND_ bool  IsIntersects (const BoundingSphere<T> &other) const
		{
			const T		dist = radius + other.radius;
			constexpr T	err  = Epsilon<T>();

			return	Distance2( center, other.center ) < (dist * dist + err);
		}

		ND_ bool  IsIntersects (const AxisAlignedBoundingBox<T> &aabb) const;

		ND_ AxisAlignedBoundingBox<T>  ToAABB () const;
	};


	using Sphere = BoundingSphere<float>;

}	// AE::Math
