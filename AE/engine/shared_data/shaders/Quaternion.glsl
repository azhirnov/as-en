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

ND_ Quat	QNormalize (const Quat q);
ND_ Quat	QInverse (const Quat q);

ND_ Quat	QMul (const Quat left, const Quat right);
ND_ float3	QMul (const Quat left, const float3 right);

ND_ float	QDot (const Quat left, const Quat right);
ND_ Quat	QSlerp (const Quat qx, const Quat qy, const float factor);

ND_ float3	QDirection (const Quat q);
ND_ Quat	QLookAt (const float3 from, const float3 to);
ND_ Quat	QLookAt (const float3 dir);

ND_ Quat	QRotationX (const float angleRad);
ND_ Quat	QRotationY (const float angleRad);
ND_ Quat	QRotationZ (const float angleRad);
ND_ Quat	QRotation (const float3 anglesRad);

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

/*
=================================================
	QNormalize
=================================================
*/
Quat  QNormalize (const Quat q)
{
	Quat	ret = q;
	float	n	= Dot( q.data, q.data );

	if ( n == 1.0 )
		return ret;

	ret.data /= Sqrt( n );
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

	return right + ((uv * left.data.w) + uuv) * 2.0;
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

	if ( cos_theta < 0.0 )
	{
		qz			= -qy.data;
		cos_theta	= -cos_theta;
	}

	if ( cos_theta > 1.0 - float_epsilon )
	{
		ret.data = Lerp( qx.data, qy.data, factor );
	}
	else
	{
		float	angle = ACos( cos_theta );

		ret.data =	( Sin( (1.0 - factor) * angle ) * qx.data +
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
	return float3( 2.0 * q.data.x * q.data.z + 2.0 * q.data.y * q.data.w,
				   2.0 * q.data.z * q.data.y - 2.0 * q.data.x * q.data.w,
				   1.0 - 2.0 - q.data.x * q.data.x - 2.0 * q.data.y * q.data.y );
}

/*
=================================================
	QRotationX
=================================================
*/
Quat  QRotationX (const float angleRad)
{
	Quat	q;
	float	a = angleRad * 0.5;

	q.data = float4( Sin(a), 0.0, 0.0, Cos(a) );
	return q;
}

/*
=================================================
	QRotationY
=================================================
*/
Quat  QRotationY (const float angleRad)
{
	Quat	q;
	float	a = angleRad * 0.5;

	q.data = float4( 0.0, Sin(a), 0.0, Cos(a) );
	return q;
}

/*
=================================================
	QRotationZ
=================================================
*/
Quat  QRotationZ (const float angleRad)
{
	Quat	q;
	float	a = angleRad * 0.5;

	q.data = float4( 0.0, 0.0, Sin(a), Cos(a) );
	return q;
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

	return QNormalize( QCreate( axis, angle + 1.0 ));
}
