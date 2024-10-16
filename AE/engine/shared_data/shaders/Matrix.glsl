// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Matrix functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

ND_ float2x2	f2x2_Identity ();
ND_ float3x3	f3x3_Identity ();
ND_ float3x4	f3x4_Identity ();
ND_ float4x3	f4x3_Identity ();
ND_ float4x4	f4x4_Identity ();

	void		SetTranslation (inout float4x3 m, const float3 pos);
	void		SetTranslation (inout float4x4 m, const float3 pos);

ND_ float4x3	f4x3_Translate (const float3 pos);
ND_ float4x4	f4x4_Translate (const float3 pos);

ND_ float2		GetTranslation2D (const float3x3 m);
ND_ float2		GetTranslation2D (const float3x2 m);
ND_ float3		GetTranslation3D (const float4x4 m);
ND_ float3		GetTranslation3D (const float4x3 m);


// Rotation is clockwise for -Y axis (default in Vulkan)
ND_ float2x2	f2x2_Rotate (const float angle);

ND_ float3x3	f3x3_RotateX (const float angle);
ND_ float3x3	f3x3_RotateY (const float angle);
ND_ float3x3	f3x3_RotateZ (const float angle);
ND_ float3x3	f3x3_Rotate  (const float angle, const float3 axis);

ND_ float4x4	f4x4_RotateX (const float angle);
ND_ float4x4	f4x4_RotateY (const float angle);
ND_ float4x4	f4x4_RotateZ (const float angle);
ND_ float4x4	f4x4_Rotate  (const float angle, const float3 axis);


// Projection
ND_ float4x4	f4x4_Ortho (const float4 viewport, const float2 range);
ND_ float4x4	f4x4_InfinitePerspective (const float fovY, const float aspect, const float zNear);
ND_ float4x4	f4x4_Perspective (float fovY, const float aspect, const float2 range);
ND_ float4x4	f4x4_Perspective (const float fovY, const float2 viewportSize, const float2 range);


// Scale
ND_ float2x2	f2x2_Scale (const float  value);
ND_ float2x2	f2x2_Scale (const float2 value);

ND_ float3x3	f3x3_Scale (const float  value);
ND_ float3x3	f3x3_Scale (const float3 value);

ND_ float4x4	f4x4_Scale (const float  value);
ND_ float4x4	f4x4_Scale (const float3 value);


ND_ float2		GetDirection2D (const float angle);
ND_ float2		GetDirection2D (const float3x3 m);

ND_ float3		GetAxisX (const float3x3 m);
ND_ float3		GetAxisX (const float4x4 m);

ND_ float3		GetAxisY (const float3x3 m);
ND_ float3		GetAxisY (const float4x4 m);

ND_ float3		GetAxisZ (const float3x3 m);
ND_ float3		GetAxisZ (const float4x4 m);

ND_ float3x3	LookAt (const float3 dir, const float3 up);
ND_ float2		Transform2D (const float4x4 mat, const float2 point);

ND_ float3		Project (const float4x4 mat, const float3 pos, const float4 viewport);
ND_ float3		UnProject (const float4x4 invMat, const float3 pos, const float4 viewport);

ND_ float3		ViewDir (const float4x4 invMat, const float2 screenPos, const float2 screenSize);
ND_ float3		ViewDir (const float4x4 invMat, const float2 unormPos);
//-----------------------------------------------------------------------------


// GLSL specs:
//	"If there is a single scalar parameter to a matrix constructor,
//	 it is used to initialize all the components on the matrix’s diagonal,
//	 with the remaining components initialized to 0.0."

float2x2  f2x2_Identity ()
{
	return float2x2( 1.f );
//	return float2x2( float2( 1.f, 0.f ),
//					 float2( 0.f, 1.f ));
}

float3x3  f3x3_Identity ()
{
	return float3x3( 1.f );
//	return float3x3( float3( 1.f, 0.f, 0.f ),
//					 float3( 0.f, 1.f, 0.f ),
//					 float3( 0.f, 0.f, 1.f ));
}

float3x4  f3x4_Identity ()
{
	return float3x4( 1.f );
//	return float3x4( float4( 1.f, 0.f, 0.f, 0.f ),
//					 float4( 0.f, 1.f, 0.f, 0.f ),
//					 float4( 0.f, 0.f, 1.f, 0.f ));
}

float4x3  f4x3_Identity ()
{
	return float4x3( 1.f );
//	return float4x3( float3( 1.f, 0.f, 0.f ),
//					 float3( 0.f, 1.f, 0.f ),
//					 float3( 0.f, 0.f, 1.f ),
//					 float3( 0.f, 0.f, 0.f ));
}

float4x4  f4x4_Identity ()
{
	return float4x4( 1.f );
//	return float4x4( float4( 1.f, 0.f, 0.f, 0.f ),
//					 float4( 0.f, 1.f, 0.f, 0.f ),
//					 float4( 0.f, 0.f, 1.f, 0.f ),
//					 float4( 0.f, 0.f, 0.f, 1.f ));
}
//-----------------------------------------------------------------------------



void  SetTranslation (inout float4x3 m, const float3 pos)
{
	m[3].xyz = pos;
}

void  SetTranslation (inout float4x4 m, const float3 pos)
{
	m[3].xyz = pos;
}

float4x3  f4x3_Translate (const float3 pos)
{
	float4x3	res = f4x3_Identity();
	SetTranslation( INOUT res, pos );
	return res;
}

float4x4  f4x4_Translate (const float3 pos)
{
	float4x4	res = f4x4_Identity();
	SetTranslation( INOUT res, pos );
	return res;
}
//-----------------------------------------------------------------------------


float2  GetTranslation2D (const float3x3 m)
{
	return m[2].xy;
}

float2  GetTranslation2D (const float3x2 m)
{
	return m[2];
}

float3  GetTranslation3D (const float4x4 m)
{
	return m[3].xyz;
}

float3  GetTranslation3D (const float4x3 m)
{
	return m[3];
}
//-----------------------------------------------------------------------------


float2x2  f2x2_Rotate (const float angle)
{
	const float	s = Sin( angle );
	const float	c = Cos( angle );
	return float2x2( float2(  c, s ),
					 float2( -s, c ));
}

float3x3  f3x3_RotateX (const float angle)
{
	const float	s = Sin( angle );
	const float	c = Cos( angle );
	return float3x3( float3( 1.f, 0.f, 0.f ),
					 float3( 0.f,  c,   s  ),
					 float3( 0.f, -s,   c  ));
}

float3x3  f3x3_RotateY (const float angle)
{
	const float	s = Sin( angle );
	const float	c = Cos( angle );
	return float3x3( float3(  c,  0.f, -s  ),
					 float3( 0.f, 1.f, 0.f ),
					 float3(  s,  0.f,  c  ));
}

float3x3  f3x3_RotateZ (const float angle)
{
	const float	s = Sin( angle );
	const float	c = Cos( angle );
	return float3x3( float3(  c,   s,  0.f ),
					 float3( -s,   c,  0.f ),
					 float3( 0.f, 0.f, 1.f ));
}

float3x3  f3x3_Rotate (const float angle, const float3 inAxis)
{
	const float		s		= Sin( angle );
	const float		c		= Cos( angle );
	const float3	axis	= Normalize( inAxis );
	const float3	temp	= (1.f - c) * axis;

	float3x3	result;
	result[0][0] = c + temp[0] * axis[0];
	result[0][1] = temp[0] * axis[1] + s * axis[2];
	result[0][2] = temp[0] * axis[2] - s * axis[1];
	result[1][0] = temp[1] * axis[0] - s * axis[2];
	result[1][1] = c + temp[1] * axis[1];
	result[1][2] = temp[1] * axis[2] + s * axis[0];
	result[2][0] = temp[2] * axis[0] + s * axis[1];
	result[2][1] = temp[2] * axis[1] - s * axis[0];
	result[2][2] = c + temp[2] * axis[2];
	return result;
}

float4x4  f4x4_RotateX (const float angle)						{ float4x4 m = float4x4(f3x3_RotateX( angle ));  m[3][3] = 1.f;  return m; }
float4x4  f4x4_RotateY (const float angle)						{ float4x4 m = float4x4(f3x3_RotateY( angle ));  m[3][3] = 1.f;  return m; }
float4x4  f4x4_RotateZ (const float angle)						{ float4x4 m = float4x4(f3x3_RotateZ( angle ));  m[3][3] = 1.f;  return m; }
float4x4  f4x4_Rotate  (const float angle, const float3 axis)	{ float4x4 m = float4x4(f3x3_Rotate( angle, axis ));  m[3][3] = 1.f;  return m; }
//-----------------------------------------------------------------------------


float2x2  f2x2_Scale (const float2 value)
{
	return	float2x2( value.x, 0.f,
					  0.f, value.y );
}

float3x3  f3x3_Scale (const float3 value)
{
	return	float3x3( value.x,	0.f,		0.f,
					  0.f,		value.y,	0.f,
					  0.f,		0.f,		value.z );
}

float2x2  f2x2_Scale (const float  value)	{ return f2x2_Scale( float2(value) ); }
float3x3  f3x3_Scale (const float  value)	{ return f3x3_Scale( float3(value) ); }

float4x4  f4x4_Scale (const float3 value)	{ float4x4 m = float4x4(f3x3_Scale( value ));  m[3][3] = 1.f;  return m; }
float4x4  f4x4_Scale (const float  value)	{ return f4x4_Scale( float3(value) ); }
//-----------------------------------------------------------------------------


float3x3  LookAt (const float3 dir, const float3 up)
{
	float3x3 m;
	m[2] = dir;
	m[0] = Normalize( Cross( up, m[2] ));
	m[1] = Cross( m[2], m[0] );
	return m;
}


float2  Transform2D (const float4x4 mat, const float2 point)
{
	return ( mat * float4(point, 0.0f, 1.0f) ).xy;
}
//-----------------------------------------------------------------------------


float3  Project (const float4x4 mat, const float3 pos, const float4 viewport)
{
	float4	temp	= mat * float4( pos, 1.0 );
	float2	size	= float2( viewport[2] - viewport[0], viewport[3] - viewport[0] );
	temp = ToUNorm( temp * (1.0 / temp.w) );
	temp.xy = temp.xy * size + viewport.xy;
	return temp.xyz;
}

float3  UnProject (const float4x4 invMat, const float3 pos, const float4 viewport)
{
	float4	temp	= float4( pos, 1.0 );
	float2	size	= float2( viewport[2] - viewport[0], viewport[3] - viewport[0] );
	temp.xy = (temp.xy - viewport.xy) / size;
	temp = invMat * ToSNorm( temp );
	temp *= (1.0 / temp.w);
	return temp.xyz;
}
//-----------------------------------------------------------------------------


float3  ViewDir (const float4x4 invMat, const float2 unormPos)
{
	const float4	world_pos = invMat * float4(ToSNorm(unormPos), -1.0f, 1.0f);
	return Normalize( world_pos.xyz / world_pos.w );
}

float3  ViewDir (const float4x4 invMat, const float2 screenPos, const float2 screenSize)
{
	return ViewDir( invMat, screenPos / screenSize );
}
//-----------------------------------------------------------------------------


float2  GetDirection2D (const float3x3 m)
{
	return (m * float3(0.0, 1.0, 0.0)).xy;
}

float2  GetDirection2D (const float angle)
{
	return float2( Cos(angle), -Sin(angle) );
}

float3  GetAxisX (const float3x3 m)		{ return float3( m[0][0], m[1][0], m[2][0] ); };
float3  GetAxisX (const float4x4 m)		{ return float3( m[0][0], m[1][0], m[2][0] ); };

float3  GetAxisY (const float3x3 m)		{ return float3( m[0][1], m[1][1], m[2][1] ); }
float3  GetAxisY (const float4x4 m)		{ return float3( m[0][1], m[1][1], m[2][1] ); }

float3  GetAxisZ (const float3x3 m)		{ return float3( m[0][2], m[1][2], m[2][2] ); }
float3  GetAxisZ (const float4x4 m)		{ return float3( m[0][2], m[1][2], m[2][2] ); }
//-----------------------------------------------------------------------------


#ifdef AE_LICENSE_MIT

// based on code from GLM (MIT license) https://github.com/g-truc/glm

float4x4  f4x4_InfinitePerspective (const float fovY, const float aspect, const float zNear)
{
	const float		range	= Tan( fovY * 0.5 ) * zNear;
	const float		left	= -range * aspect;
	const float		right	= range * aspect;
	const float		bottom	= -range;
	const float		top		= range;

	float4x4	result = float4x4( 0.f );
	result[0][0] = (2.f * zNear) / (right - left);
	result[1][1] = (2.f * zNear) / (top - bottom);
	result[2][2] = 1.f;
	result[2][3] = 1.f;
	result[3][2] = - zNear;
	return result;
}

float4x4  f4x4_Ortho (const float4 viewport, const float2 range)
{
	// viewport - {left, top, right, bottom}
	float4x4	result = float4x4( 1.f );
	result[0][0] = 2.f / (viewport.z - viewport.x);
	result[1][1] = 2.f / (viewport.y - viewport.w);
	result[2][2] = - 1.f;
	result[3][0] = - (viewport.z + viewport.x) / (viewport.z - viewport.x);
	result[3][1] = - (viewport.y + viewport.w) / (viewport.y - viewport.w);
	return result;
}

float4x4  f4x4_Perspective (float fovY, const float aspect, const float2 range)
{
	fovY = Tan( fovY * 0.5f );

	float4x4	result = float4x4( 0.f );
	result[0][0] = 1.f / (aspect * fovY);
	result[1][1] = 1.f / fovY;
	result[2][2] = range.y / (range.y - range.x);
	result[2][3] = 1.f;
	result[3][2] = -(range.y * range.x) / (range.y - range.x);
	return result;
}

float4x4  f4x4_Perspective (const float fovY, const float2 viewportSize, const float2 range)
{
	const float	h = Cos( 0.5f * fovY ) / Sin( 0.5f * fovY );
	const float	w = h * viewportSize.y / viewportSize.x;

	float4x4	result = float4x4( 0.f );
	result[0][0] = w;
	result[1][1] = h;
	result[2][2] = range.y / (range.y - range.x);
	result[2][3] = 1.f;
	result[3][2] = -(range.y * range.x) / (range.y - range.x);
	return result;
}

#endif // AE_LICENSE_MIT
//-----------------------------------------------------------------------------
