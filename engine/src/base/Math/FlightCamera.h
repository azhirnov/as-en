// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Camera.h"
#include "base/Math/Frustum.h"
#include "base/Math/Radians.h"

namespace AE::Math
{

	//
	// Camera for flight simulator
	//
	
	template <typename T>
	struct FlightCameraTempl final
	{
	// types
	private:
		using Camera_t	= CameraTempl<T>;
		using Frustum_t	= FrustumTempl<T>;
		using Self		= FlightCameraTempl<T>;
		using Quat_t	= typename Camera_t::Quat_t;
		using Vec2_t	= typename Camera_t::Vec2_t;
		using Vec3_t	= typename Camera_t::Vec3_t;
		using Mat4_t	= typename Camera_t::Mat4_t;
		using Rad_t		= RadiansTempl<T>;
		using Rad3_t	= RadianVec<T, 3>;


	// variables
	private:
		Camera_t	_camera;
		Frustum_t	_frustum;


	// methods
	public:
		FlightCameraTempl ()										__NE___	{}

		ND_ Camera_t const&		GetCamera ()						C_NE___	{ return _camera; }
		ND_ Frustum_t const&	GetFrustum ()						C_NE___	{ return _frustum; }

		void  SetPerspective (Rad fovY, T aspect, T zNear, T zFar)	__NE___;
		
		Self&  Rotate (Rad_t yaw, Rad_t pitch, Rad_t roll)			__NE___;
		Self&  Rotate (const Rad3_t &v)								__NE___	{ return Rotate( v.x, v.y, v.z ); }

		Self&  Move (const Vec3_t &delta)							__NE___;

		ND_ Vec3_t  Transform (const Vec3_t &delta)					C_NE___;

		Self&  SetPosition (const Vec3_t &pos)						__NE___;
		Self&  SetRotation (const Quat_t &q)						__NE___;
	};
	
	
/*
=================================================
	SetPerspective
=================================================
*/
	template <typename T>
	void  FlightCameraTempl<T>::SetPerspective (Rad fovY, T aspect, T zNear, T zFar) __NE___
	{
		_camera.SetPerspective( fovY, aspect, Vec2_t{zNear, zFar} );
		_frustum.Setup( _camera );
	}

/*
=================================================
	SetPosition
=================================================
*/
	template <typename T>
	FlightCameraTempl<T>&  FlightCameraTempl<T>::SetPosition (const Vec3_t &pos) __NE___
	{
		_camera.transform.position = pos;
		return *this;
	}
	
/*
=================================================
	SetRotation
=================================================
*/
	template <typename T>
	FlightCameraTempl<T>&  FlightCameraTempl<T>::SetRotation (const Quat_t &q) __NE___
	{
		_camera.transform.orientation = q;
		return *this;
	}
	
/*
=================================================
	Rotate
=================================================
*/
	template <typename T>
	FlightCameraTempl<T>&  FlightCameraTempl<T>::Rotate (Rad_t yaw, Rad_t pitch, Rad_t roll) __NE___
	{
		Quat_t&	q = _camera.transform.orientation;
		
		q = Quat_t::RotateX( pitch ) * q;
		q = q * Quat_t::RotateY( -yaw );
		q = Quat_t::RotateZ( roll ) * q;

		q.Normalize();

		_frustum.Setup( _camera );
		return *this;
	}
	
/*
=================================================
	Move / Transform
----
	x - forward/backward
	y - side
	z - up/down
=================================================
*/
	template <typename T>
	FlightCameraTempl<T>&  FlightCameraTempl<T>::Move (const Vec3_t &delta) __NE___
	{
		_camera.transform.position += Transform( delta );
		return *this;
	}

	template <typename T>
	typename FlightCameraTempl<T>::Vec3_t
		FlightCameraTempl<T>::Transform (const Vec3_t &delta) C_NE___
	{
		const Mat4_t	view_mat	= _camera.ToViewMatrix();	// TODO: optimize?
		const Vec3_t	up_dir		= _camera.UpDir();
		const Vec3_t	axis_x		{ view_mat[0][0], view_mat[1][0], view_mat[2][0] };
		const Vec3_t	axis_z		{ view_mat[0][2], view_mat[1][2], view_mat[2][2] };
		Vec3_t			pos;

		pos  = axis_x *  delta.y;
		pos += up_dir *  delta.z;
		pos += axis_z * -delta.x;
		return pos;
	}


} // AE::Math
