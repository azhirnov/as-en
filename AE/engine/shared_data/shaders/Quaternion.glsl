// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Quaternion functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


struct Quat
{
	float4	data;	// x, y, z, w
};


ND_ Quat	QIdentity ();
ND_ Quat	QCreate (const float4 v);
ND_ Quat	QCreate (const float3 axis, const float angle);
ND_ Quat	QCreate (float x, float y, float z, float w);
ND_ Quat	QCreateWXYZ (float w, float x, float y, float z);

ND_ Quat	QFromSpherical (const float2 phiTheta);
ND_ Quat	QFromEuler (float yaw, float pitch, float roll);

ND_ Quat	QFrom2Normals (const float3 from, const float3 to);

ND_ Quat	QNormalize (const Quat q);
ND_ Quat	QInverse (const Quat q);

ND_ Quat	QMul (const Quat left, const Quat right);
ND_ float3	QMul (const Quat left, const float3 right);

ND_ float	QDot (const Quat left, const Quat right);
ND_ Quat	QSlerp (const Quat qx, const Quat qy, const float factor);

ND_ float3	QDirection (const Quat q);
ND_ Quat	QLookAt (const float3 from, const float3 to);
ND_ Quat	QLookAt (const float3 dir);

ND_ Quat	QRotationX (float angleRad);
ND_ Quat	QRotationY (float angleRad);
ND_ Quat	QRotationZ (float angleRad);

ND_ Quat	QRotation (const float3 anglesRad);
ND_ Quat	QRotation (float angleRad, const float3 axis);
ND_ Quat	QRotation_ChordOver2R (const float x, const float3 axis);

ND_ Quat	QRotationX (const float sin, const float cos);
ND_ Quat	QRotationY (const float sin, const float cos);
ND_ Quat	QRotationZ (const float sin, const float cos);

ND_ Quat	QRotationX_ChordOver2R (const float x);
ND_ Quat	QRotationY_ChordOver2R (const float x);
ND_ Quat	QRotationZ_ChordOver2R (const float x);
//-----------------------------------------------------------------------------


/*
=================================================
	QIdentity
=================================================
*/
Quat  QIdentity ()
{
	Quat	ret;
	ret.data = float4( 0.0, 0.0, 0.0, 1.0 );
	return ret;
}

/*
=================================================
	QCreate
=================================================
*/
Quat  QCreate (const float4 v)
{
	Quat	ret;
	ret.data = v;
	return ret;
}

Quat  QCreate (const float3 axis, const float angle)
{
	Quat	ret;
	ret.data = float4( axis, angle );
	return ret;
}

Quat  QCreate (float x, float y, float z, float w)
{
	Quat	ret;
	ret.data = float4( x, y, z, w );
	return ret;
}

Quat  QCreateWXYZ (float w, float x, float y, float z)
{
	Quat	ret;
	ret.data = float4( x, y, z, w );
	return ret;
}

/*
=================================================
	QFromEuler / QFromSpherical
=================================================
*/
Quat  QFromEuler (float yaw, float pitch, float roll)
{
	float	cy = Cos( yaw   * 0.5 );
    float	sy = Sin( yaw   * 0.5 );
    float	cp = Cos( pitch * 0.5 );
    float	sp = Sin( pitch * 0.5 );
    float	cr = Cos( roll  * 0.5 );
    float	sr = Sin( roll  * 0.5 );

    Quat	q;
    q.data.w = (cr * cp * cy) + (sr * sp * sy);
    q.data.x = (sr * cp * cy) - (cr * sp * sy);
    q.data.y = (cr * sp * cy) + (sr * cp * sy);
    q.data.z = (cr * cp * sy) - (sr * sp * cy);
    return QNormalize( q );
}

Quat  QFromSpherical (const float2 phiTheta)
{
	return QFromEuler( phiTheta.y, phiTheta.x, 0.0 );
}

/*
=================================================
	QNormalize
=================================================
*/
Quat  QNormalize (const Quat q)
{
	Quat	ret = q;
	float	n	= Dot( q.data, q.data );

	if ( n < 1.0e-6 )
	{
		// repair
		ret.data.w = 1.0;
		n = Dot( q.data, q.data );
	}

	if ( Abs( n - 1.0 ) < 1.0e-5 )
		return ret;

	ret.data *= InvSqrt( n );
	return ret;
}

/*
=================================================
	QInverse
=================================================
*/
Quat  QInverse (const Quat q)
{
	Quat	ret;
	ret.data.xyz = -q.data.xyz;
	ret.data.w   = q.data.w;
	return ret;
}

/*
=================================================
	QMul
=================================================
*/
Quat  QMul (const Quat left, const Quat right)
{
	Quat	ret;

	ret.data.xyz	= left.data.w * right.data.xyz +
					  left.data.xyz * right.data.w +
					  Cross( left.data.xyz, right.data.xyz );

	float4	dt		= left.data.xyzw * right.data.xyzw;
	ret.data.w		= Dot( dt, float4( -1.0, -1.0, -1.0, 1.0 ));

	return ret;
}

/*
=================================================
	QMul
=================================================
*/
float3  QMul (const Quat left, const float3 right)
{
	float3	q	= left.data.xyz;
	float3	uv	= Cross( q, right );
	float3	uuv	= Cross( q, uv );

	return right + ((uv * left.data.w) + uuv) * 2.0f;
}

/*
=================================================
	QDot
=================================================
*/
float  QDot (const Quat left, const Quat right)
{
	return Dot( left.data, right.data );
}

/*
=================================================
	QSlerp
=================================================
*/
Quat  QSlerp (const Quat qx, const Quat qy, const float factor)
{
	Quat	ret;
	float4	qz			= qy.data;
	float	cos_theta	= Dot( qx.data, qy.data );

	if ( cos_theta < 0.0f )
	{
		qz			= -qy.data;
		cos_theta	= -cos_theta;
	}

	if ( cos_theta > 1.0f - float_epsilon )
	{
		ret.data = Lerp( qx.data, qy.data, factor );
	}
	else
	{
		float	angle = ACos( cos_theta );

		ret.data =	( Sin( (1.0f - factor) * angle ) * qx.data +
					  Sin( factor * angle ) * qz ) / Sin( angle );
	}
	return ret;
}

/*
=================================================
	QDirection
=================================================
*/
float3  QDirection (const Quat q)
{
	return float3(	2.0 * (q.data.x * q.data.z - q.data.w * q.data.y),
					2.0 * (q.data.y * q.data.z + q.data.w * q.data.x),
					1.0 - 2.0 * (q.data.x * q.data.x + q.data.y * q.data.y) );
}

/*
=================================================
	QRotationX
=================================================
*/
Quat  QRotationX (float a)
{
	a *= 0.5;
	return QRotationX( Sin(a), Cos(a) );
}

Quat  QRotationX (const float sin, const float cos)
{
	Quat	q;
	q.data = float4( sin, 0.0f, 0.0f, cos );
	return q;
}

Quat  QRotationX_ChordOver2R (const float x)
{
	return QRotationX( x, Sqrt(1.0f - x*x) );
}

/*
=================================================
	QRotationY
=================================================
*/
Quat  QRotationY (float a)
{
	a *= 0.5;
	return QRotationY( Sin(a), Cos(a) );
}

Quat  QRotationY (const float sin, const float cos)
{
	Quat	q;
	q.data = float4( 0.0, sin, 0.0f, cos );
	return q;
}

Quat  QRotationY_ChordOver2R (const float x)
{
	return QRotationY( x, Sqrt(1.0f - x*x) );
}

/*
=================================================
	QRotationZ
=================================================
*/
Quat  QRotationZ (float a)
{
	a *= 0.5f;
	return QRotationZ( Sin(a), Cos(a) );
}

Quat  QRotationZ (const float sin, const float cos)
{
	Quat	q;
	q.data = float4( 0.0f, 0.0f, sin, cos );
	return q;
}

Quat  QRotationZ_ChordOver2R (const float x)
{
	return QRotationZ( x, Sqrt(1.0f - x*x) );
}

/*
=================================================
	QRotation
=================================================
*/
Quat  QRotation (const float3 anglesRad)
{
	return QMul( QMul( QRotationX( anglesRad.x ), QRotationY( anglesRad.y )), QRotationZ( anglesRad.z ));
}

Quat  QRotation (float a, const float3 axis)
{
	a *= 0.5;
	return QCreate( axis * Sin(a), Cos(a) );
}

Quat  QRotation_ChordOver2R (const float x, const float3 axis)
{
	return QCreate( axis * x, Sqrt(1.0f - x*x) );
}

/*
=================================================
	QLookAt
=================================================
*/
Quat  QLookAt (const float3 from, const float3 to)
{
	return QLookAt( to - from );
}

Quat  QLookAt (const float3 dir)
{
	float3	fwd		= float3(0.0, 0.0, 1.0);
	float3	axis	= Cross( fwd, dir );
	float	angle	= Dot( fwd, dir );

	return QNormalize( QCreate( axis, angle + 1.0f ));
}

/*
=================================================
	QFrom2Normals
----
	from GLM (MIT license) https://github.com/g-truc/glm
=================================================
*/
#ifdef AE_LICENSE_MIT
	Quat  QFrom2Normals (const float3 u, const float3 v)
	{
		float	norm_u_norm_v	= Sqrt( Dot( u, u ) * Dot( v, v ));
		float	real_part		= norm_u_norm_v + Dot( u, v );
		float3	t;

		if ( real_part < 1.0e-6 * norm_u_norm_v )
		{
			real_part = 0.0f;
			t = Abs(u.x) > Abs(u.z) ?
					float3( -u.y, u.x, 0.f ) :
					float3( 0.f, -u.z, u.y );
		}
		else
		{
			t = Cross( u, v );
		}
		return QNormalize( QCreate( t.x, t.y, t.z, real_part ));
	}
#endif
