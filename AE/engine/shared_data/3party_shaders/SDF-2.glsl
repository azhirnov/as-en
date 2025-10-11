

/*
=================================================
	SDF_MinCubic, SDF_OpBlend
----
	from https://github.com/electricsquare/raymarching-workshop
=================================================
*/
#ifdef AE_LICENSE_MIT

	ND_ float  SDF_MinCubic (const float a, float b, float k)
	{
		// polynomial smooth min (k = 0.1);
		float	h = Max( k - Abs(a-b), 0.0 );
		return Min(a, b) - h * h * h / (6.0 * k * k);
	}

	float2  SDF_OpBlend (const float2 dm1, const float2 dm2)
	{
		float	k = 2.0;
		float	d = SDF_MinCubic( dm1.x, dm2.x, k );
		float	m = Lerp( dm1.y, dm2.y, Clamp( dm1.x - d, 0.f, 1.f ));
		return float2( d, m );
	}

#endif // AE_LICENSE_MIT

/*
=================================================
	SDF_MinCubic, SDF_OpBlend
----
	from https://www.shadertoy.com/view/XdB3Ww
	MIT License - https://opensource.org/license/mit
	Copyright (c) 2014, Per Bloksgaard - https://perbloksgaard.dk
=================================================
*/
#ifdef AE_LICENSE_MIT

	// Find roots using Cardano's method. http://en.wikipedia.org/wiki/Cubic_function#Cardano.27s_method
	float2  SolveCubic2 (const float3 a)
	{
		float	p	= a.y - a.x * a.x / 3.0;
		float	p3	= p*p*p;
		float	q	= a.x * (2.0 * a.x * a.x - 9.0 * a.y) / 27.0 + a.z;
		float	d	= q*q + 4.0 * p3 / 27.0;
		if ( d > 0.0 )
		{
			float2	x = (float2(1.0,-1.0) * Sqrt(d) - q) * 0.5;
			x = Sign(x) * Pow( Abs(x), float2(1.0/3.0) );
  			return float2( (x.x + x.y) - a.x / 3.0 );
  		}
 		float	v = ACos( -Sqrt( -27.0 / p3 ) * q * 0.5 ) / 3.0;
 		float	m = Cos( v );
 		float	n = Sin( v ) * 1.732050808;
		return float2( m+m, -n-m ) * Sqrt( -p / 3.0 ) - a.x / 3.0;
	}

	// How to solve the equation below can be seen on this image.
	// https://perbloksgaard.dk/research/DistanceToQuadraticBezier.jpg
	float  SDF2_QuadraticBezier (const float2 p, const float2 a, float2 b, const float2 c)
	{
		b += Lerp( float2(1.0e-4), float2(0.0), Abs(Sign( b * 2.0 - a - c )) );
		float2	A = b - a;
		float2	B = c - b - A;
		float2	C = p - a;
		float2	D = A * 2.0;
		float2	T = Saturate( SolveCubic2( float3(-3.0 * Dot(A,B), Dot(C,B) - 2.0 * LengthSq(A), Dot(C,A) ) / -LengthSq(B) ));
		return Sqrt( Min( LengthSq( C - (D + B * T.x) * T.x ), LengthSq( C - (D + B * T.y) * T.y )));
	}

#endif // AE_LICENSE_MIT
