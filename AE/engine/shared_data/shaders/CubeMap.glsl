// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Cube map and spherical cube utilities.

	Also see [SphericalCubeMath.h](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/geometry_tools/SphericalCube/SphericalCubeMath.h)

	1. Forward projection converts snorm coordinate (UV) to the snorm coord with distortion correction.
	2. Forward projection converts snorm coordinate (UV) to the 3D coordinate of the cube face / sphere with applied distortion correction.
	3. Inverse projection invert distortion for UV coordinate.
	4. Inverse projection converts 3D coordinate to the snorm UV and cube face index.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"
#include "FastMath.glsl"

#define ECubeFace		int
#define ECubeFace_XPos	0	// right
#define ECubeFace_XNeg	1	// left
#define ECubeFace_YPos	2	// down
#define ECubeFace_YNeg	3	// up
#define ECubeFace_ZPos	4	// front - inside, back  - outside
#define ECubeFace_ZNeg	5	// back  - inside, front - outside


ND_ ECubeFace  CM_ToOppositeFace (ECubeFace face);

ND_ float3  CM_RotateVec (const float3 snormCoord, const ECubeFace face);
ND_ float4  CM_InverseRotation (float3 dir);						// returns {transformed 'dir', face}
ND_ float3  CM_InverseRotation (const ECubeFace face, float3 dir);	// returns transformed 'dir' for specified 'face'

ND_ float3  CM_CubeSC_Forward (const float2 snormCoord, const ECubeFace face);

ND_ float3  CM_IdentitySC_Forward (const float2 snormCoord, const ECubeFace face);
ND_ float3  CM_IdentitySC_Inverse (const float3 dir);

ND_ float2  CM_TangentialSC_Forward (const float2 snormCoord);
ND_ float3  CM_TangentialSC_Forward (const float2 snormCoord, const ECubeFace face);
ND_ float2  CM_TangentialSC_Inverse (const float2 snormCoord);
ND_ float3  CM_TangentialSC_Inverse (const float3 dir);
ND_ float2  CM_TangentialSC_FastInverse (const float2 snormCoord);

ND_ float2  CM_EverittSC_Forward (const float2 snormCoord);
ND_ float3  CM_EverittSC_Forward (const float2 snormCoord, const ECubeFace face);
ND_ float2  CM_EverittSC_Inverse (const float2 snormCoord);
ND_ float3  CM_EverittSC_Inverse (const float3 dir);

ND_ float2  CM_5thPolySC_Forward (const float2 snormCoord);
ND_ float3  CM_5thPolySC_Forward (const float2 snormCoord, const ECubeFace face);
ND_ float2  CM_5thPolySC_Inverse (const float2 snormCoord);
ND_ float3  CM_5thPolySC_Inverse (const float3 dir);

ND_ float2  CM_COBE_SC_Forward (const float2 snormCoord);
ND_ float3  CM_COBE_SC_Forward (const float2 snormCoord, const ECubeFace face);
ND_ float2  CM_COBE_SC_Inverse (const float2 snormCoord);
ND_ float3  CM_COBE_SC_Inverse (const float3 dir);

ND_ float2  CM_ArvoSC_Forward (const float2 snormCoord);
ND_ float3  CM_ArvoSC_Forward (const float2 snormCoord, const ECubeFace face);
ND_ float2  CM_ArvoSC_Inverse (const float2 snormCoord);
ND_ float3  CM_ArvoSC_Inverse (const float3 dir);
//-----------------------------------------------------------------------------



/*
=================================================
	CM_ToOppositeFace
----
	XPos to XNeg, etc
=================================================
*/
ECubeFace  CM_ToOppositeFace (ECubeFace face)
{
	return (face & ~1) + ((face + 1) & 1);
}

/*
=================================================
	CM_RotateVec / CM_InverseRotation
=================================================
*/
float3  CM_RotateVec (const float3 c, const ECubeFace face)
{
	return	float3( c.z, -c.y, -c.x) * float(face == ECubeFace_XPos) +
			float3(-c.z, -c.y,  c.x) * float(face == ECubeFace_XNeg) +
			float3( c.x,  c.z,  c.y) * float(face == ECubeFace_YPos) +
			float3( c.x, -c.z, -c.y) * float(face == ECubeFace_YNeg) +
			float3( c.x, -c.y,  c.z) * float(face == ECubeFace_ZPos) +
			float3(-c.x, -c.y, -c.z) * float(face == ECubeFace_ZNeg);
}

float4  CM_InverseRotation (float3 c)
{
	c.y = -c.y;

	// front (xy space)
	if ( All3( Abs(c.x) <= c.z,  c.z > 0.f,  Abs(c.y) <= c.z ))
		return float4( c.x, c.y, c.z, ECubeFace_ZPos );

	// right (zy space)
	if ( All3( Abs(c.z) <= c.x,  c.x > 0.f,  Abs(c.y) <= c.x ))
		return float4( -c.z, c.y, c.x, ECubeFace_XPos );

	// back (xy space)
	if ( All3( Abs(c.x) <= -c.z,  c.z < 0.f,  Abs(c.y) <= -c.z ))
		return float4( -c.x, c.y, -c.z, ECubeFace_ZNeg );

	// left (zy space)
	if ( All3( Abs(c.z) <= -c.x,  c.x < 0.f,  Abs(c.y) <= -c.x ))
		return float4( c.z, c.y, -c.x, ECubeFace_XNeg );

	// up (xz space)
	if ( c.y > 0.f )
		return float4( c.x, -c.z, c.y, ECubeFace_YNeg );

	// down (xz space)
	return float4( c.x, c.z, -c.y, ECubeFace_YPos );
}

float3  CM_InverseRotation (const ECubeFace face, float3 c)
{
	c.y = -c.y;
	return	float3( -c.z,  c.y,  c.x ) * float(face == ECubeFace_XPos) +
			float3(  c.z,  c.y, -c.x ) * float(face == ECubeFace_XNeg) +
			float3(  c.x, -c.z,  c.y ) * float(face == ECubeFace_YNeg) +
			float3(  c.x,  c.z, -c.y ) * float(face == ECubeFace_YPos) +
			float3(  c.x,  c.y,  c.z ) * float(face == ECubeFace_ZPos) +
			float3( -c.x,  c.y, -c.z ) * float(face == ECubeFace_ZNeg);
}

/*
=================================================
	CM_CubeSC_Forward
=================================================
*/
float3  CM_CubeSC_Forward (const float2 snormCoord, const ECubeFace face)
{
	return CM_RotateVec( float3(snormCoord, 1.f), face );
}

/*
=================================================
	CM_IdentitySC_Forward / CM_IdentitySC_Inverse
----
	identity spherical cube projection
=================================================
*/
float3  CM_IdentitySC_Forward (const float2 snormCoord, const ECubeFace face)
{
	return Normalize( CM_CubeSC_Forward( snormCoord, face ));
}

float3  CM_IdentitySC_Inverse (const float3 dir)
{
	float4	coord_face = CM_InverseRotation( dir );
	coord_face.xy /= coord_face.z;
	return coord_face.xyw;
}

/*
=================================================
	CM_TangentialSC_Forward / CM_TangentialSC_Inverse
----
	tangential spherical cube projection
=================================================
*/
float2  CM_TangentialSC_Forward (const float2 snormCoord)
{
	const float	warp_theta		= 0.868734829276f;
	const float	tan_warp_theta	= 1.182286685546f; //tan( warp_theta );
	return Tan( warp_theta * snormCoord ) / tan_warp_theta;
}

float3  CM_TangentialSC_Forward (const float2 snormCoord, const ECubeFace face)
{
	float2	coord = CM_TangentialSC_Forward( snormCoord );
	return Normalize( CM_RotateVec( float3(coord.x, coord.y, 1.f), face ));
}

float2  CM_TangentialSC_Inverse (const float2 snormCoord)
{
	const float	warp_theta		= 0.868734829276f;
	const float	tan_warp_theta	= 1.182286685546f; //tan( warp_theta );
	return ATan( snormCoord * tan_warp_theta ) / warp_theta;
}

float2  CM_TangentialSC_FastInverse (const float2 snormCoord)
{
	const float	warp_theta		= 0.868734829276f;
	const float	tan_warp_theta	= 1.182286685546f; //tan( warp_theta );
	return FastATan( snormCoord * tan_warp_theta ) / warp_theta;
}

float3  CM_TangentialSC_Inverse (const float3 dir)
{
	float4	coord_face = CM_InverseRotation( dir );
	return float3( CM_TangentialSC_Inverse( coord_face.xy / coord_face.z ), coord_face.w );
}

/*
=================================================
	CM_EverittSC_Forward / CM_EverittSC_Inverse
=================================================
*/
float2  CM_EverittSC_Forward (const float2 snormCoord)
{
	const float  e = 1.4511;
	return Sign( snormCoord ) * (e - Sqrt( e*e - 4.0 * (e - 1.0) * Abs(snormCoord) )) / (2.0 * (e - 1.0));
}

float3  CM_EverittSC_Forward (const float2 snormCoord, const ECubeFace face)
{
	float2	coord = CM_EverittSC_Forward( snormCoord );
	return Normalize( CM_RotateVec( float3(coord.x, coord.y, 1.f), face ));
}

float2  CM_EverittSC_Inverse (const float2 snormCoord)
{
	const float  e = 1.4511;
	return snormCoord * (e + (1.0 - e) * Abs(snormCoord));
}

float3  CM_EverittSC_Inverse (const float3 dir)
{
	float4	coord_face = CM_InverseRotation( dir );
	return float3( CM_EverittSC_Inverse( coord_face.xy / coord_face.z ), coord_face.w );
}

/*
=================================================
	CM_5thPolySC_Forward / CM_5thPolySC_Inverse
=================================================
*/
float2  CM_5thPolySC_Forward (const float2 snormCoord)
{
	float2	sq = snormCoord * snormCoord;
	return (0.745558715593 + (0.130546850193 + 0.123894434214 * sq) * sq) * snormCoord;
}

float3  CM_5thPolySC_Forward (const float2 snormCoord, const ECubeFace face)
{
	float2	coord = CM_5thPolySC_Forward( snormCoord );
	return Normalize( CM_RotateVec( float3(coord.x, coord.y, 1.f), face ));
}

float2  CM_5thPolySC_Inverse (const float2 snormCoord)
{
	float2	sq = snormCoord * snormCoord;
	return (1.34318229552 + (-0.486514066449 + 0.143331770927 * sq) * sq) * snormCoord;
}

float3  CM_5thPolySC_Inverse (const float3 dir)
{
	float4	coord_face = CM_InverseRotation( dir );
	return float3( CM_5thPolySC_Inverse( coord_face.xy / coord_face.z ), coord_face.w );
}

/*
=================================================
	CM_COBE_SC_Forward / CM_COBE_SC_Inverse
=================================================
*/
float2  CM_COBE_SC_Forward (const float2 snormCoord)
{
	float2	sq1	= snormCoord * snormCoord;
	float2	sq2	= sq1.yx;
	float2	sum	= ((-0.0941180085824 + 0.0409125981187 * sq2 - 0.0623272690881 * sq1) * sq1 + (0.0275922480902 + 0.0342217026979 * sq2) * sq2);
	return (0.723951234952 + 0.276048765048 * sq1 + (1.0 - sq1) * sum) * snormCoord;
}

float3  CM_COBE_SC_Forward (const float2 snormCoord, const ECubeFace face)
{
	float2	coord = CM_COBE_SC_Forward( snormCoord );
	return Normalize( CM_RotateVec( float3(coord.x, coord.y, 1.f), face ));
}

float2  CM_COBE_SC_Inverse (const float2 snormCoord)
{
	float2	sq1	= snormCoord * snormCoord;
	float2	sq2	= sq1.yx;
	float2	sum	= ((-0.212853382041 + 0.0941259684877 * sq2 + 0.0693532685333 * sq1) * sq1 + (-0.117847692949 + 0.0107989197181 * sq2) * sq2);
	return (1.37738198385 - 0.377381983848 * sq1 + (1.0 - sq1) * sum) * snormCoord;
}

float3  CM_COBE_SC_Inverse (const float3 dir)
{
	float4	coord_face = CM_InverseRotation( dir );
	return float3( CM_COBE_SC_Inverse( coord_face.xy / coord_face.z ), coord_face.w );
}

/*
=================================================
	CM_ArvoSC_Forward / CM_ArvoSC_Inverse
=================================================
*/
float2  CM_ArvoSC_Forward (const float2 snormCoord)
{
	float	tan_a_term	= Tan( snormCoord.x * 0.523598775598 );
	float	cos_a_term	= Cos( snormCoord.x * 1.0471975512 );
	return float2( 1.41421356237 * tan_a_term / Sqrt( 1.0 - tan_a_term * tan_a_term ),
				   snormCoord.y / Sqrt( 1.0 + (1.0 - snormCoord.y * snormCoord.y) * cos_a_term) );
}

float3  CM_ArvoSC_Forward (const float2 snormCoord, const ECubeFace face)
{
	float2	coord = CM_ArvoSC_Forward( snormCoord );
	return Normalize( CM_RotateVec( float3(coord.x, coord.y, 1.f), face ));
}

float2  CM_ArvoSC_Inverse (const float2 snormCoord)
{
	float	ss2 = Sqrt( snormCoord.x * snormCoord.x + 2.0 );
	return float2( ATan( snormCoord.x / ss2 ) * 1.9098593171,
				   snormCoord.y * ss2 / Sqrt( Dot( snormCoord, snormCoord ) + 1.0 ));
}

float3  CM_ArvoSC_Inverse (const float3 dir)
{
	float4	coord_face = CM_InverseRotation( dir );
	return float3( CM_ArvoSC_Inverse( coord_face.xy / coord_face.z ), coord_face.w );
}

