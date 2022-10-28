// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Camera.h"
#include "base/Math/Frustum.h"
#include "base/Math/Radians.h"

namespace AE::Math
{

	//
	// Camera for first person shooter
	//
	
	template <typename T>
	struct FPSCameraTempl final
	{
	// types
	private:
		using Camera_t	= CameraTempl<T>;
		using Frustum_t	= FrustumTempl<T>;
		using Self		= FPSCameraTempl<T>;
		using Quat_t	= typename Camera_t::Quat_t;
		using Vec2_t	= typename Camera_t::Vec2_t;
		using Vec3_t	= typename Camera_t::Vec3_t;
		using Mat4_t	= typename Camera_t::Mat4_t;
		using Rad_t		= RadiansTempl<T>;


	// variables
	private:
		Camera_t	_camera;
		Frustum_t	_frustum;


	// methods
	public:
		FPSCameraTempl () {}

		ND_ Camera_t const&		GetCamera ()	const	{ return _camera; }
		ND_ Frustum_t const&	GetFrustum ()	const	{ return _frustum; }

		void SetPerspective (Rad fovY, T aspect, T zNear, T zFar);

		Self&  Rotate (Rad_t horizontal, Rad_t vertical);
		Self&  Rotate (const Vec2_t &v)		{ return Rotate( Rad_t{v.x}, Rad_t{v.y} ); }

		Self&  Move (const Vec3_t &delta);
		Self&  Move2 (const Vec3_t &delta);					// free FPS camera

		ND_ Vec3_t  Transform (const Vec3_t &delta) const;
		ND_ Vec3_t  Transform2 (const Vec3_t &delta) const;	// free FPS camera

		Self&  SetPosition (const Vec3_t &pos);
		Self&  SetRotation (const Quat_t &q);
	};
	
	
/*
=================================================
	SetPerspective
=================================================
*/
	template <typename T>
	void  FPSCameraTempl<T>::SetPerspective (Rad fovY, T aspect, T zNear, T zFar)
	{
		_camera.SetPerspective( fovY, aspect, Vec2_t{zNear, zFar} );
		_frustum.Setup( _camera );
	}

/*
=================================================
	Rotate
=================================================
*/
	template <typename T>
	FPSCameraTempl<T>&  FPSCameraTempl<T>::Rotate (Rad_t horizontal, Rad_t vertical)
	{
		horizontal = -horizontal;

		Quat_t&	q		= _camera.transform.orientation;
		bool	has_ver	= not Equals( vertical, Rad_t(0) );
		bool	has_hor	= not Equals( horizontal, Rad_t(0) );

		if ( not (has_hor or has_ver) )
			return *this;
		
		//_LimitRotation( INOUT horizontal, INOUT vertical );	// TODO

		if ( has_ver )
			q = Quat_t{ Cos(vertical * T(0.5)), Sin(vertical * T(0.5)), T(0), T(0) } * q;

		if ( has_hor )
			q = q * Quat_t{ Cos(horizontal * T(0.5)), T(0), Sin(horizontal * T(0.5)), T(0) };

		q.Normalize();

		_frustum.Setup( _camera );
		return *this;
	}
	
/*
=================================================
	Move
----
	x - forward/backward
	y - side
	z - up/down
=================================================
*/
	template <typename T>
	FPSCameraTempl<T>&  FPSCameraTempl<T>::Move (const Vec3_t &delta)
	{
		_camera.transform.position += Transform( delta );
		return *this;
	}
	
	template <typename T>
	typename FPSCameraTempl<T>::Vec3_t
		FPSCameraTempl<T>::Transform (const Vec3_t &delta) const
	{
		const Mat4_t	view_mat	= _camera.ToViewMatrix();
		const Vec3_t	up_dir		{ T(0), T(1), T(0) };
		const Vec3_t	axis_x		{ view_mat[0][0], view_mat[1][0], view_mat[2][0] };
		const Vec3_t	forwards	= Normalize( Cross( up_dir, axis_x ));
		Vec3_t			pos;

		pos  = forwards * delta.x;
		pos += axis_x   * delta.y;
		pos += up_dir   * delta.z;
		return pos;
	}

/*
=================================================
	Move2
----
	x - forward/backward
	y - side
	z - up/down
=================================================
*/
	template <typename T>
	FPSCameraTempl<T>&  FPSCameraTempl<T>::Move2 (const Vec3_t &delta)
	{
		_camera.transform.position += Transform2( delta );
		return *this;
	}
	
	template <typename T>
	typename FPSCameraTempl<T>::Vec3_t
		FPSCameraTempl<T>::Transform2 (const Vec3_t &delta) const
	{
		const Mat4_t	view_mat	= _camera.ToViewMatrix();
		const Vec3_t	up_dir		{ T(0), T(1), T(0) };
		const Vec3_t	axis_x		{ view_mat[0][0], view_mat[1][0], view_mat[2][0] };
		const Vec3_t	axis_z		{ view_mat[0][2], view_mat[1][2], view_mat[2][2] };
		Vec3_t			pos;
		
		pos  = axis_z * -delta.x;
		pos += axis_x *  delta.y;
		pos += up_dir *  delta.z;
		return pos;
	}

/*
=================================================
	SetPosition
=================================================
*/
	template <typename T>
	FPSCameraTempl<T>&  FPSCameraTempl<T>::SetPosition (const Vec3_t &pos)
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
	FPSCameraTempl<T>&  FPSCameraTempl<T>::SetRotation (const Quat_t &q)
	{
		_camera.transform.orientation = q;
		return *this;
	}


} // AE::Math
