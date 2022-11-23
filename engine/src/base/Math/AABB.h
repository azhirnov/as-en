// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Sphere.h"
#include "base/Containers/ArrayView.h"

namespace AE::Math
{

	//
	// Axis-Aligned Bounding Box
	//

	template <typename T>
	struct AxisAlignedBoundingBox
	{
	// types
		using Self		= AxisAlignedBoundingBox<T>;
		using Vec3_t	= Vec<T,3>;
		using Value_t	= T;


	// variables
		Vec3_t		min;
		Vec3_t		max;


	// methods
		AxisAlignedBoundingBox ()								__NE___	: min{T{0}}, max{T{0}} {}
		explicit AxisAlignedBoundingBox (const Vec3_t &point)	__NE___	: min{point}, max{point} {}


		ND_ Vec3_t	Center ()									C_NE___	{ return (max + min) * T{0.5}; }
		ND_ Vec3_t	Extent ()									C_NE___	{ return (max - min); }
		ND_ Vec3_t	HalfExtent ()								C_NE___	{ return Extent() * T{0.5}; }
		ND_ bool	Empty ()									C_NE___	{ return all( max == min ); }
		ND_ T		Distance (const Vec3_t &point)				C_NE___;

		ND_ bool  IsIntersects (const Self &other)				C_NE___;
		ND_ bool  IsIntersects (const BoundingSphere<T> &sphere)C_NE___;
		ND_ bool  IsIntersects (const Vec3_t &point)			C_NE___;


		ND_ BoundingSphere<T>  ToInnerSphere ()					C_NE___;
		ND_ BoundingSphere<T>  ToOuterSphere ()					C_NE___;


		Self&  Move (const Vec3_t &delta)						__NE___;
		Self&  Repair ()										__NE___;
		Self&  Add (const Vec3_t &point)						__NE___;
		Self&  Add (const Self &aabb)							__NE___;
		Self&  SetExtent (const Vec3_t &extent)					__NE___;
		Self&  SetCenter (const Vec3_t &center)					__NE___;
		Self&  Transform (const Transformation<T> &tr)			__NE___;

		ND_ static Self  FromPoints (ArrayView<Vec3_t> points)	__NE___;
	};


	using AABB = AxisAlignedBoundingBox<float>;
	
	
	template <typename T>
	bool  AxisAlignedBoundingBox<T>::IsIntersects (const Self &other) C_NE___
	{
		return not( max.x < other.min.x or max.y < other.min.y or max.z < other.min.z or
					min.x > other.max.x or min.y > other.max.y or min.z > other.max.z );
	}
	
	template <typename T>
	bool  AxisAlignedBoundingBox<T>::IsIntersects (const BoundingSphere<T> &sphere) C_NE___
	{
		T			dist = sphere.radius * sphere.radius;
		constexpr T	err  = Epsilon<T>();

		dist -= (sphere.center.x < min.x ? Square( sphere.center.x - min.x ) : Square( sphere.center.x - max.x ));
		dist -= (sphere.center.y < min.y ? Square( sphere.center.y - min.y ) : Square( sphere.center.y - max.y ));
		dist -= (sphere.center.z < min.z ? Square( sphere.center.z - min.z ) : Square( sphere.center.z - max.z ));

		return dist > -err;
	}
	
	template <typename T>
	bool  AxisAlignedBoundingBox<T>::IsIntersects (const Vec3_t &point) C_NE___
	{
		return all( point > min ) and all( point < max );
	}
	
	template <typename T>
	BoundingSphere<T>  AxisAlignedBoundingBox<T>::ToInnerSphere () C_NE___
	{
		const Vec3_t	side = Extent();
		const Value_t	r    = T{0.5} * Min( side.x, side.y, side.z );
		return BoundingSphere<T>{ Center(), r };
	}
	
	template <typename T>
	BoundingSphere<T>  AxisAlignedBoundingBox<T>::ToOuterSphere () C_NE___
	{
		constexpr T		sq3_div2 = T(0.86602540378443864676372317075294);
		const Vec3_t	side	 = Extent();
		const Value_t	r        = sq3_div2 * Max( side.x, side.y, side.z );
		return BoundingSphere<T>{ Center(), r };
	}
	
	template <typename T>
	AxisAlignedBoundingBox<T>&  AxisAlignedBoundingBox<T>::Move (const Vec3_t &delta) __NE___
	{
		min += delta;
		max += delta;
		return *this;
	}
		
	template <typename T>
	AxisAlignedBoundingBox<T>&  AxisAlignedBoundingBox<T>::Repair () __NE___
	{
		if ( min.x > max.x )	std::swap( min.x, max.x );
		if ( min.y > max.y )	std::swap( min.y, max.y );
		if ( min.z > max.z )	std::swap( min.z, max.z );
		return *this;
	}
	
	template <typename T>
	AxisAlignedBoundingBox<T>&  AxisAlignedBoundingBox<T>::Add (const Vec3_t &point) __NE___
	{
		this->min = Min( point, this->min );
		this->max = Max( point, this->max );
		return *this;
	}
	
	template <typename T>
	AxisAlignedBoundingBox<T>&  AxisAlignedBoundingBox<T>::Add (const Self &aabb) __NE___
	{
		return Add( aabb.min ).Add( aabb.max );
	}
	
	template <typename T>
	AxisAlignedBoundingBox<T>&  AxisAlignedBoundingBox<T>::SetExtent (const Vec3_t &extent) __NE___
	{
		Vec3_t	center		= Center();
		Vec3_t	halfextent	= extent * T{0.5};

		min = center - halfextent;
		max = center + halfextent;
		return *this;
	}
	
	template <typename T>
	AxisAlignedBoundingBox<T>&  AxisAlignedBoundingBox<T>::SetCenter (const Vec3_t &center) __NE___
	{
		Vec3_t	halfextent = HalfExtent();
		min = center - halfextent;
		max = center + halfextent;
		return *this;
	}
	
	template <typename T>
	AxisAlignedBoundingBox<T>&  AxisAlignedBoundingBox<T>::Transform (const Transformation<T> &tr) __NE___
	{
		Vec3_t	point = min;	min = max = tr.ToGlobalPosition( point );
		point.z = max.z;		Add( tr.ToGlobalPosition( point ));
		point.y = max.y;		Add( tr.ToGlobalPosition( point ));
		point.z = min.z;		Add( tr.ToGlobalPosition( point ));
		point.x = max.x;		Add( tr.ToGlobalPosition( point ));
		point.z = max.z;		Add( tr.ToGlobalPosition( point ));
		point.y = min.y;		Add( tr.ToGlobalPosition( point ));
		point.z = min.z;		Add( tr.ToGlobalPosition( point ));

		return *this;
	}
		
	template <typename T>
	T  AxisAlignedBoundingBox<T>::Distance (const Vec3_t &point) C_NE___
	{
		const Vec3_t	d = Abs( point ) - HalfExtent();
		return Min( Max( d.x, Max( d.y, d.z )), T{0} ) + Length( Max( d, T{0} ));
	}
	
	template <typename T>
	AxisAlignedBoundingBox<T>  AxisAlignedBoundingBox<T>::FromPoints (ArrayView<Vec3_t> points) __NE___
	{
		CHECK_ERR( points.size() );

		Self	result{ points[0] };
		for (usize i = 1; i < points.size(); ++i) {
			result.Add( points[i] );
		}
		return result;
	}
	
	template <typename T>
	AxisAlignedBoundingBox<T>  BoundingSphere<T>::ToAABB () C_NE___
	{
		AxisAlignedBoundingBox<T>	result;
		result.min = center - radius;
		result.max = center + radius;
		return result;
	}
	
	template <typename T>
	bool  BoundingSphere<T>::IsIntersects (const AxisAlignedBoundingBox<T> &aabb) C_NE___
	{
		return aabb.IsIntersects( *this );
	}

} // AE::Math
