#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


#ifdef AE_LICENSE_UNLICENSE
/*
=================================================
	WeylHash
----
	from https://www.shadertoy.com/view/MsV3z3
	LICENSE: http://unlicense.org/
=================================================
*/
ND_ float  WeylHash12 (const int2 c)
{
	const int x = 0x3504f333 * c.x * c.x + c.y;
	const int y = 0xf1bbcdcb * c.y * c.y + c.x;

	return float(x*y) * float(2.0 / 8589934592.0) + float(0.5);
}

ND_ float  WeylHash12 (const float2 c)
{
	// a pair of Weyl values with low star discrepancy
	const float2 W = float2( 0.5545497, 0.308517 );

	const float2 v = c * Fract( c * W );
	return Fract( v.x * v.y );
}
#endif
//-----------------------------------------------------------------------------


#ifdef AE_LICENSE_CC_BY_NC_SA_3
/*
=================================================
	ModHash
----
	from https://www.shadertoy.com/view/Xts3R7
	license CC BY-NC-SA 3.0
=================================================
*/
ND_ float  ModHash12 (float2 uv)
{

	uv = Abs( Mod( float(10.0) * Fract( (uv + float(1.1312)) * float(31.0) ), uv + float(2.0) ));
	uv = Abs( Mod( uv.x * Fract( (uv + float(1.721711)) * float(17.0) ), uv ));
	return Fract( float(10.0) * (float(7.0) * uv.y + float(31.0) * uv.x) );
}
#endif
//-----------------------------------------------------------------------------



#ifdef AE_LICENSE_UNLICENSE
/*
=================================================
	IWeylHash
----
	from https://www.shadertoy.com/view/4dlcR4
	LICENSE: http://unlicense.org/
=================================================
*/
uint3 _IWeylConst ()
{
	return uint3(
		0x3504f333u,	// W0 = 3*2309*128413
		0xf1bbcdcbu,	// W1 = 7*349*1660097
		741103597u		// M = 13*83*686843
	);
}

ND_ uint  IWeylHash12 (const uint2 p)
{
	uint	x = p.x;
	uint	y = p.y;

	x *= _IWeylConst().x;	// x' = Fx(x)
	y *= _IWeylConst().y;	// y' = Fy(y)
	x ^= y;					// combine
	x *= _IWeylConst().z;	// MLCG constant
	return x;
}

ND_ uint  IWeylHash12v2 (const uint2 p)
{
	uint	x = p.x;
	uint	y = p.y;

	x *= _IWeylConst().x;	// x' = Fx(x)
	y *= _IWeylConst().y;	// y' = Fy(y)
	x += _IWeylConst().y;	// some odd constant
	y += _IWeylConst().x;	// some odd constant
	x ^= y;					// combine
	x *= _IWeylConst().z;	// MLCG constant
	return x;
}

#endif // AE_LICENSE_UNLICENSE
//-----------------------------------------------------------------------------



#ifdef AE_LICENSE_CC_BY_NC_SA_3
/*
=================================================
	Hash_Uniform
	Hash_Triangular
	Hash_Gaussianish
	Hash_MoarGaussianish
----
	result in range [0, 1]
	'seed' in range [0, 1]
----
	from https://www.shadertoy.com/view/4ssXRX
	license CC BY-NC-SA 3.0
=================================================
*/

//note: uniformly distributed, normalized rand, [0;1]
# define nrand( n ) Fract( Sin( Dot( (n).xy, float2(12.9898, 78.233) )) * float(43758.5453) )

ND_ float  Hash_Uniform (const float2 n, const float seed)
{
	const float t = Fract( seed );
	const float nrnd0 = nrand( n + float(0.07)*t );
	return nrnd0;
}

ND_ float  Hash_Triangular (const float2 n, const float seed)
{
	const float t = Fract( seed );
	const float nrnd0 = nrand( n + float(0.07)*t );
	const float nrnd1 = nrand( n + float(0.11)*t );
	return (nrnd0+nrnd1) / float(2.0);
}

ND_ float  Hash_Gaussianish (const float2 n, const float seed)
{
	const float t = Fract( seed );
	const float nrnd0 = nrand( n + float(0.07)*t );
	const float nrnd1 = nrand( n + float(0.11)*t );
	const float nrnd2 = nrand( n + float(0.13)*t );
	const float nrnd3 = nrand( n + float(0.17)*t );
	return (nrnd0+nrnd1+nrnd2+nrnd3) / float(4.0);
}

ND_ float  Hash_MoarGaussianish (const float2 n, const float seed)
{
	const float t = Fract( seed );
	const float nrnd0 = nrand( n + float(0.07)*t );
	const float nrnd1 = nrand( n + float(0.11)*t );
	const float nrnd2 = nrand( n + float(0.13)*t );
	const float nrnd3 = nrand( n + float(0.17)*t );

	const float nrnd4 = nrand( n + float(0.19)*t );
	const float nrnd5 = nrand( n + float(0.23)*t );
	const float nrnd6 = nrand( n + float(0.29)*t );
	const float nrnd7 = nrand( n + float(0.31)*t );

	return (nrnd0 + nrnd1 + nrnd2 + nrnd3 + nrnd4 + nrnd5 + nrnd6 + nrnd7) / float(8.0);
}

# undef nrand
#endif // AE_LICENSE_CC_BY_NC_SA_3
//-----------------------------------------------------------------------------
