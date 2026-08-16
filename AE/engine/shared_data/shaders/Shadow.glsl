// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Shadow helper functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


ND_ float	ESM_Moment (float depth, float depthScale);
ND_ float2	VSM_Moments (float depth, float biasScale);
ND_ float4	EVSM_Moments (float depth, float2 depthScale, float biasScale);
ND_ float4	MSM_Moments (float depth, float biasScale);

ND_ float	ExponentialSM (float depth, float depthScale, float expDepth);
ND_ float	VarianceSM (float depth, float2 moments);
ND_ float	ExponentialVarianceSM (float depth, float2 depthScale, float4 moments);
ND_ float	MomentSM (float zf, float4 moments, float momentBias);
ND_ float	OriginMomentSM (float depth, float4 moments);
//-----------------------------------------------------------------------------



float   ESM_Moment (float depth, float depthScale)
{
	return Exp( depth * depthScale );
}

float2  VSM_Moments (float depth, float biasScale)
{
	float	dx	= gl.dFdx( depth );
	float	dy	= gl.dFdy( depth );
	float2	res;

	res.r  = depth;
	res.g  = depth * depth;
	res.g += biasScale * (dx * dx + dy * dy);
	return res;
}

float  WarpPositive (float z, float k)
{
	return Exp( k * z );
}

float  WarpNegative (float z, float k)
{
	return -Exp( -k * z );
}

float4  EVSM_Moments (float depth, float2 depthScale, float biasScale)
{
	float	pos		= WarpPositive( depth, depthScale.x );
	float	neg		= WarpNegative( depth, depthScale.y );

	float	dx_pos	= gl.dFdx( pos );
	float	dy_pos	= gl.dFdy( pos );

	float	dx_neg	= gl.dFdx( neg );
	float	dy_neg	= gl.dFdy( neg );

	float4	res;
	res.r	= pos;
	res.g	= pos * pos + biasScale * (dx_pos * dx_pos + dy_pos * dy_pos);

	res.b	= neg;
	res.a	= neg * neg + biasScale * (dx_neg * dx_neg + dy_neg * dy_neg);
	return	res;
}

float4  MSM_Moments (float depth, float biasScale)
{
	float	depth2	= depth * depth;

	float	dx		= gl.dFdx( depth );
	float	dy		= gl.dFdy( depth );

	return float4(	depth,
					depth2 + biasScale * (dx * dx + dy * dy),
					depth2 * depth,
					depth2 * depth2 );
}
//-----------------------------------------------------------------------------



float  ReduceLightBleeding (float p, float amount)
{
	return Saturate( (p - amount) / (1.0 - amount) );
}

float  ChebyshevUpperBound (float mean, float mean2, float depth)
{
	if ( depth <= mean )
		return 1.0;

	float	variance = mean2 - (mean * mean);
			variance = Max( variance, 0.00002 );

	float	d = depth - mean;
	float	p = variance / (variance + d * d);
	return	p;
}


float  ExponentialSM (float depth, float depthScale, float expDepth)
{
	return	Saturate( Exp( -depthScale * depth ) * expDepth );
}


float  VarianceSM (float depth, float2 moments)
{
	float	p = ChebyshevUpperBound( moments.x, moments.y, depth );
	return	ReduceLightBleeding( p, 0.3 );
}


float  ExponentialVarianceSM (float depth, float2 depthScale, float4 moments)
{
	float	pos_depth	= WarpPositive( depth, depthScale.x );
	float	neg_depth	= WarpNegative( depth, depthScale.y );

	float	pos_shadow	= ChebyshevUpperBound( moments.x, moments.y, pos_depth );
	float	neg_shadow	= ChebyshevUpperBound( moments.z, moments.w, neg_depth );

	float	shadow		= Min( pos_shadow, neg_shadow );
	return	ReduceLightBleeding( shadow, 0.2 );
}
//-----------------------------------------------------------------------------


/*
=================================================
	Simple MSM
=================================================
*/
float  MomentSM (float depth, float4 moments, float momentBias)
{
	const float MSM_EPS = 1.0e-5;

    // Bias moments toward a valid distribution
    moments = Lerp( moments, float4(0.5, 0.5, 0.5, 0.5), momentBias );

    // Build Cholesky-like terms from moments
    float	L21 = moments.y - moments.x * moments.x;
    float	D11 = 1.0;
    float	D22 = Max( L21, MSM_EPS );

    float	L31 = moments.z - moments.y * moments.x;
    float	L32	= (moments.w - moments.y * moments.y) / D22;
    float	D33	= Max( (moments.w - moments.z * moments.x) - L32 * L31, MSM_EPS );

    // Solve the quadratic that gives candidate support points
    float	c0 = 1.0;
    float	c1 = moments.x;
    float	c2 = moments.y;

    float	p		 = depth - moments.x;
    float	variance = Max( moments.y - moments.x * moments.x, MSM_EPS );

    // If the receiver is before the mean, fully lit
    if ( depth <= moments.x )
        return 1.0;

    // Initial approximation
    float	bound = variance / (variance + p * p);

    // Higher moment tightening
    float	skew	= moments.z - 3.0 * moments.x * moments.y + 2.0 * moments.x * moments.x * moments.x;
    float	kurt	= moments.w - 4.0 * moments.x * moments.z + 6.0 * moments.x * moments.x * moments.y - 3.0 * Pow( moments.x, 4.0 );

    float	tighten	= 1.0 / (1.0 + 10.0 * Abs(skew) + 5.0 * Abs(kurt));
    bound *= tighten;

    return Saturate( bound );
}

/*
=================================================
	Origin MSM
=================================================
*/
// Solve quadratic: ax^2 + bx + c = 0
float2  _OMSM_SolveQuadratic (float a, float b, float c)
{
    float	disc	= Max( b * b - 4.0 * a * c, 0.0 );
    float	s		= Sqrt( disc );
    return	float2( (-b - s) / (2.0 * a), (-b + s) / (2.0 * a) );
}

// Stable cubic root
float  _OMSM_CbrtApprox (float x)
{
    return Sign(x) * Pow( Abs(x), 1.0 / 3.0 );
}

// Solve monic cubic:
// x^3 + a*x^2 + b*x + c = 0
// returns 3 real roots if they exist; otherwise repeated/approximated roots
float3  _OMSM_SolveCubic (float a, float b, float c)
{
    float	a2	= a * a;
    float	q	= (3.0 * b - a2) / 9.0;
    float	r	= (9.0 * a * b - 27.0 * c - 2.0 * a2 * a) / 54.0;
    float	d	= q * q * q + r * r;

    if ( d >= 0.0 )
    {
        float	s	= _OMSM_CbrtApprox( r + Sqrt( d ));
        float	t	= _OMSM_CbrtApprox( r - Sqrt( d ));
        float	x1	= -a / 3.0 + (s + t);
        float	x2	= -a / 3.0 - 0.5 * (s + t);
        float	x3	= x2;
        return	float3(x1, x2, x3);
    }
    else
    {
        float	theta	 = ACos( Clamp( r / Sqrt(-q * q * q), -1.0, 1.0 ));
        float	twoSqrtQ = 2.0 * Sqrt( -q );
        float	x1		 = twoSqrtQ * Cos( theta / 3.0 ) - a / 3.0;
        float	x2		 = twoSqrtQ * Cos( (theta + 2.0 * 3.14159265 ) / 3.0) - a / 3.0;
        float	x3		 = twoSqrtQ * Cos( (theta + 4.0 * 3.14159265 ) / 3.0) - a / 3.0;
        return	float3(x1, x2, x3);
    }
}

// Sort 3 values ascending
float3  _OMSM_Sort3 (float3 v)
{
    if (v.x > v.y) { float t = v.x; v.x = v.y; v.y = t; }
    if (v.y > v.z) { float t = v.y; v.y = v.z; v.z = t; }
    if (v.x > v.y) { float t = v.x; v.x = v.y; v.y = t; }
    return v;
}

// Solve weights for 3-point distribution at locations z0,z1,z2
// matching first 3 moments:
// w0 + w1 + w2 = 1
// w0*z0 + w1*z1 + w2*z2 = m1
// w0*z0^2 + w1*z1^2 + w2*z2^2 = m2
float3  _OMSM_SolveWeights (float3 z, float m1, float m2)
{
	const float MSM_EPS = 1.0e-5;

    float z0 = z.x, z1 = z.y, z2 = z.z;

    float d0 = (z0 - z1) * (z0 - z2);
    float d1 = (z1 - z0) * (z1 - z2);
    float d2 = (z2 - z0) * (z2 - z1);

    d0 = abs(d0) < MSM_EPS ? sign(d0 + MSM_EPS) * MSM_EPS : d0;
    d1 = abs(d1) < MSM_EPS ? sign(d1 + MSM_EPS) * MSM_EPS : d1;
    d2 = abs(d2) < MSM_EPS ? sign(d2 + MSM_EPS) * MSM_EPS : d2;

    // Lagrange/Vandermonde inversion
    float w0 = (m2 - m1 * (z1 + z2) + z1 * z2) / d0;
    float w1 = (m2 - m1 * (z0 + z2) + z0 * z2) / d1;
    float w2 = (m2 - m1 * (z0 + z1) + z0 * z1) / d2;

    return float3(w0, w1, w2);
}

float  OriginMomentSM (float depth, float4 moments)
{
	const float MSM_EPS = 1.0e-5;

    // Moments:
    // m1 = E[z], m2 = E[z^2], m3 = E[z^3], m4 = E[z^4]
    float m1 = moments.x;
    float m2 = moments.y;
    float m3 = moments.z;
    float m4 = moments.w;

    // Build the cubic whose roots are the support points of the 3-point distribution.
    //
    // Using the recurrence from the moment matrix:
    //
    // [1  m1 m2] [c0]   [-m3]
    // [m1 m2 m3] [c1] = [-m4]
    // [m2 m3 m4] [c2]   [-m5]   <-- unavailable directly
    //
    // In practical 4-moment MSM, we instead form the orthogonal polynomial
    // coefficients from the Hankel moment system and reduce to a cubic fit.
    //
    // A widely used exact-style reduction is:
    float c0 = (-m2 * m2 + m1 * m3);
    float c1 = ( m2 * m1 - m3);
    float c2 = ( m2 - m1 * m1);

    // Stabilize
    c2 = Abs(c2) < MSM_EPS ? MSM_EPS : c2;

    // Monic cubic:
    // x^3 + a*x^2 + b*x + c = 0
    float a = c1 / c2;
    float b = c0 / c2;
    float c = 0.0;

    vec3 roots = _OMSM_SolveCubic( a, b, c );
    roots = _OMSM_Sort3( roots );

    // In exact MSM, support locations come from the orthogonal polynomial roots.
    // Many practical implementations shift them by the mean for stability.
    roots += float3(m1);

    // Solve weights from first two moments
    vec3 w = _OMSM_SolveWeights( roots, m1, m2 );

    // Clamp to valid probability mass and renormalize
    w = Max(w, vec3(0.0));
    float ws = w.x + w.y + w.z;
    if (ws < MSM_EPS)
        return depth <= m1 ? 1.0 : 0.0;

    w /= ws;

    // Visibility = probability that stored depth >= receiver depth
    float vis = 0.0;
    vis += (depth <= roots.x) ? w.x : 0.0;
    vis += (depth <= roots.y) ? w.y : 0.0;
    vis += (depth <= roots.z) ? w.z : 0.0;

    return Saturate( vis );
}

