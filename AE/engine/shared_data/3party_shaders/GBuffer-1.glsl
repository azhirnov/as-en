

// TODO
//	https://jcgt.org/published/0003/02/01/paper.pdf

#ifdef AE_ENABLE_UNKNOWN_LICENSE

/*
=================================================
	Stalker version
----
	Best accuracy, but requires signed format for negative Z.
	Has low accuracy when Z near to 1.
	Compatible with RG8 format.
=================================================
*/
float2  Stalker_EncodeNormal (const float3 norm)
{
	float2	res = ToUNorm( norm.xy );
	res.x *= Sign( norm.z );
	return res;
}

float3  Stalker_DecodeNormal (const float2 packed)
{
	float3	res;
	res.xy = ToSNorm( Abs( packed ));
	res.z  = Sign( packed.x ) * Sqrt( Abs( 1.0 - res.x*res.x - res.y*res.y ));
	return res;
}

/*
=================================================
	Octahedron-normal vectors
----
	from https://knarkowicz.wordpress.com/2014/04/16/octahedron-normal-vector-encoding/
=================================================
*/
float2  Octahedron_EncodeNormal (float3 n)
{
	n	/= (Abs(n.x) + Abs(n.y) + Abs(n.z));
	n.xy = n.z >= 0.0 ? n.xy :
			(1.0 - Abs(n.yx)) * Sign( n.xy );
	return n.xy;
}

float3  Octahedron_DecodeNormal (const float2 f)
{
	float3	n	= float3( f.x, f.y, 1.0 - Abs(f.x) - Abs(f.y) );
	float	t	= Saturate( -n.z );
			n.xy -= t * Sign( n.xy );
	return Normalize( n );
}

/*
=================================================
	Signed Octahedron
----
	from https://johnwhite3d.blogspot.com/2017/10/signed-octahedron-normal-encoding.html
	Best accuracy on whole range.
=================================================
*/
float3  SigOctahedron_EncodeNormal (float3 n)
{
	n /= (Abs(n.x) + Abs(n.y) + Abs(n.z));
	float3	res;
	res.y = ToUNorm( n.y );
	res.x = n.x * 0.5 + res.y;
	res.y = n.x * -0.5 + res.y;
	res.z = Saturate( n.z * float_max );
	return res;
}

float3  SigOctahedron_DecodeNormal (float3 n)
{
	float3	res;
	res.x = (n.x - n.y);
	res.y = (n.x + n.y) - 1.0;
	res.z = ToSNorm( n.z );
	res.z = res.z * (1.0 - Abs(res.x) - Abs(res.y));
	return Normalize( res );
}

/*
=================================================
	Stereographic Projection
----
	from https://aras-p.info/texts/CompactNormalStorage.html
	Requires signed format for negative Z.
=================================================
*/
float2  Stereo_EncodeNormal (const float3 n)
{
	float	scale	= 1.7777;
	float2	enc		= n.xy / (n.z+1);
	enc /= scale;
	return enc;
}

float3  Stereo_DecodeNormal (const float2 enc)
{
	float	scale	= 1.7777;
	float3	nn		= float3( enc * scale, 1.0 );
	float	g		= 2.0 / Dot( nn, nn );
	return float3( g * nn.xy, g - 1.0 );
}

/*
=================================================
	CryTeck version (Spheremap Transform)
----
	Large error on z = 1, but less than 'Spheremap' version.
=================================================
*/
float2  CryTeck_EncodeNormal (const float3 norm)
{
	return Normalize( norm.xy ) * Sqrt( ToUNorm( norm.z ));
}

float3  CryTeck_DecodeNormal (const float2 packed)
{
	float3	res;
	res.z  = ToSNorm( LengthSq( packed ));
	res.xy = Normalize( packed ) * Sqrt( 1.0 - res.z * res.z );
	return res;
}

/*
=================================================
	Spheremap Transform
----
	from https://aras-p.info/texts/CompactNormalStorage.html
	Large error on z = -1.
=================================================
*/
float2  Spheremap_EncodeNormal (const float3 n)
{
	float f = InvSqrt( 8.0 * n.z + 8.0 );
	return n.xy * f + 0.5;
}

float3  Spheremap_DecodeNormal (const float2 enc)
{
	float2	fenc = enc * 4.0 - 2.0;
	float	f	= Dot( fenc, fenc );
	float	g	= Sqrt( 1.0 - f / 4.0 );
	return float3( fenc * g, 1.0 - f / 2.0 );
}

/*
=================================================
	Spherical Coordinates
----
	from https://aras-p.info/texts/CompactNormalStorage.html
=================================================
*/
float2  Spherical_EncodeNormal (const float3 n)
{
	return float2( ATan( n.y, n.x ) * float_InvPi, n.z );
}

float3  Spherical_DecodeNormal (const float2 enc)
{
	float2	scth  = SinCos( enc.x * float_Pi );
	float2	scphi = float2( Sqrt( 1.0 - enc.y*enc.y ), enc.y );
	return float3( scth.y*scphi.x, scth.x*scphi.x, scphi.y );
}

/*
=================================================
	Diamond encoding
----
	from https://www.jeremyong.com/graphics/2023/01/09/tangent-spaces-and-diamond-encoding/
=================================================
*/
float  encode_diamond (float2 p)
{
	// Project to the unit diamond, then to the x-axis.
	float x = p.x / (Abs(p.x) + Abs(p.y));

	// Contract the x coordinate by a factor of 4 to represent all 4 quadrants in the unit range and remap
	float py_sign = Sign( p.y );
	return -py_sign * 0.25f * x + 0.5f + py_sign * 0.25f;
}

float2  decode_diamond (float p)
{
	float2 v;

	// Remap p to the appropriate segment on the diamond
	float p_sign = Sign( p - 0.5f );
	v.x = -p_sign * 4.f * p + 1.f + p_sign * 2.f;
	v.y =  p_sign * (1.f - Abs(v.x));

	// Normalization extends the point on the diamond back to the unit circle
	return Normalize( v );
}

// Given a normal and tangent vector, encode the tangent as a single float that can be subsequently quantized.
float  Diamond_EncodeTangent (float3 normal, float3 tangent)
{
	// First, find a canonical direction in the tangent plane
	float3 t1;
	if ( Abs(normal.y) > Abs(normal.z) )
	{
		// Pick a canonical direction orthogonal to n with z = 0
		t1 = float3(normal.y, -normal.x, 0.f);
	}
	else
	{
		// Pick a canonical direction orthogonal to n with y = 0
		t1 = float3(normal.z, 0.f, -normal.x);
	}
	t1 = Normalize( t1 );

	// Construct t2 such that t1 and t2 span the plane
	float3 t2 = Cross( t1, normal );

	// Decompose the tangent into two coordinates in the canonical basis
	float2 packed_tangent = float2( Dot( tangent, t1 ), Dot( tangent, t2 ));

	// Apply our diamond encoding to our two coordinates
	return encode_diamond( packed_tangent );
}

float3  Diamond_DecodeTangent (float3 normal, float diamond_tangent)
{
	// As in the encode step, find our canonical tangent basis span(t1, t2)
	float3 t1;
	if ( Abs(normal.y) > Abs(normal.z) )
	{
		t1 = float3(normal.y, -normal.x, 0.f);
	}
	else
	{
		t1 = float3(normal.z, 0.f, -normal.x);
	}
	t1 = Normalize( t1 );

	float3 t2 = Cross( t1, normal );

	// Recover the coordinates used with t1 and t2
	float2 packed_tangent = decode_diamond( diamond_tangent );

	return packed_tangent.x * t1 + packed_tangent.y * t2;
}


#endif // AE_ENABLE_UNKNOWN_LICENSE
