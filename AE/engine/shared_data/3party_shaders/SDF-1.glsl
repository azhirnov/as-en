/*
	Default signed distance fields.

	from:
		https://iquilezles.org/articles/distfunctions/
		https://iquilezles.org/articles/distfunctions2d/
*/

#ifdef AE_ENABLE_UNKNOWN_LICENSE

//-----------------------------------------------------------------------------
// 2D Shapes

float  SDF2_Line (const float2 position, const float2 point0, const float2 point1)
{
	const float2  pa = position - point0;
	const float2  ba = point1 - point0;
	const float   h  = Saturate( Dot( pa, ba ) / Dot( ba, ba ));
	return Length( pa - ba * h );
}


float  SDF2_Rect (const float2 position, const float2 hsize)
{
	const float2  d = Abs( position ) - hsize;
	return Length( Max( d, float2(0.0f) )) + Min( Max( d.x, d.y ), 0.0f );
}


float  SDF2_Circle (const float2 position, const float radius)
{
	return Length( position ) - radius;
}


float  SDF2_Pentagon (const float2 position, const float radius)
{
	const float3  k = float3( 0.809016994f, 0.587785252f, 0.726542528f );
		  float2  p = position;
	p.x = Abs(p.x);
	p -= 2.0f * Min( Dot( float2(-k.x, k.y ), p ), 0.0f ) * float2(-k.x, k.y);
	p -= 2.0f * Min( Dot( float2( k.x, k.y ), p ), 0.0f ) * float2( k.x, k.y);
	p -= float2( Clamp( p.x, -radius * k.z, radius * k.z ), radius );
	return Length( p ) * SignOrZero( p.y );
}


float  SDF2_Hexagon (const float2 position, const float radius)
{
	const float3	k = float3( -0.866025404, 0.5, 0.577350269 );
		  float2	p = Abs( position );
	p -= 2.0 * Min(Dot( k.xy, p ), 0.0f ) * k.xy;
	p -= float2( Clamp( p.x, -k.z * radius, k.z * radius ), radius );
	return Length( p ) * SignOrZero( p.y );
}


float  SDF2_Octagon (const float2 position, const float radius)
{
	const float3	k = float3( -0.9238795325f, 0.3826834323f, 0.4142135623f );
		  float2	p = Abs( position );
	p -= 2.0 * Min( Dot( float2( k.x, k.y), p ), 0.0f ) * float2( k.x,k.y);
	p -= 2.0 * Min( Dot( float2(-k.x, k.y), p ), 0.0f ) * float2(-k.x,k.y);
	p -= float2( Clamp( p.x, -k.z * radius, k.z * radius ), radius );
	return Length(p) * SignOrZero(p.y);
}


float  SDF2_RoundedRect (const float2 position, const float2 hsize, float4 radius)
{
			radius.xy	= (position.x > 0.0) ? radius.xy : radius.zw;
			radius.x	= (position.y > 0.0) ? radius.x  : radius.y;
	float2	q			= Abs( position ) - hsize + radius.x;
	return Min( Max( q.x, q.y ), 0.0f ) + Length( Max( q, 0.0f )) - radius.x;
}


float  SDF2_OrientedRect (const float2 position, const float2 a, const float2 b, const float angle)
{
	float	l = Length( b - a );
	float2	d = (b - a) / l;
	float2	q = (position - (a + b) * 0.5f);
			q = float2x2( d.x, -d.y, d.y, d.x ) * q;
			q = Abs(q) - float2( l, angle ) * 0.5f;
	return Length( Max( q, 0.0f )) + Min( Max( q.x, q.y ), 0.0f );
}


float  SDF2_EquilateralTriangle (float2 p, const float size)
{
	const float k = Sqrt(3.0);	// TODO
	p.x = Abs( p.x ) - size;
	p.y = p.y + size / k;
	if ( p.x + k * p.y > 0.0 )	p = float2( p.x - k * p.y, -k * p.x - p.y ) / 2.0;
	p.x -= Clamp( p.x, -2.0 * size, 0.0 );
	return -Length(p) * SignOrZero(p.y);
}


float  SDF2_Triangle (const float2 position, const float2 p0, const float2 p1, const float2 p2)
{
	float2	e0	= p1 - p0;
	float2	e1	= p2 - p1;
	float2	e2	= p0 - p2;
	float2	v0	= position - p0;
	float2	v1	= position - p1;
	float2	v2	= position - p2;
	float2	pq0	= v0 - e0 * Saturate( Dot( v0, e0 ) / Dot( e0, e0 ));
	float2	pq1	= v1 - e1 * Saturate( Dot( v1, e1 ) / Dot( e1, e1 ));
	float2	pq2	= v2 - e2 * Saturate( Dot( v2, e2 ) / Dot( e2, e2 ));
	float	s	= SignOrZero( e0.x * e2.y - e0.y * e2.x );
	float2	d	= Min(	Min( float2( Dot( pq0, pq0 ), s * (v0.x * e0.y - v0.y * e0.x)),
						float2( Dot( pq1, pq1 ), s * (v1.x * e1.y - v1.y * e1.x) )),
						float2( Dot( pq2, pq2 ), s * (v2.x * e2.y - v2.y * e2.x) ));
	return -Sqrt(d.x) * SignOrZero(d.y);
}


float  SDF2_Trapezeoid (float2 p, const float r1, const float r2, const float height)
{
	float2	k1	= float2( r2, height );
	float2	k2	= float2( r2 - r1, 2.0*height );
			p.x	= Abs( p.x );
	float2	ca	= float2( p.x - Min( p.x, (p.y < 0.0) ? r1 : r2 ), Abs(p.y) - height );
	float2	cb	= p - k1 + k2 * Saturate( Dot( k1-p, k2 ) / LengthSq(k2) );
	float	s	= (cb.x < 0.0 and ca.y < 0.0) ? -1.0 : 1.0;
	return s * Sqrt( Min( LengthSq(ca), LengthSq(cb) ));
}


float  SDF2_UnevenCapsule (float2 p, const float r1, const float r2, const float h)
{
			p.x	= Abs(p.x);
			p.y += h * 0.5;
	float	b	= (r1 - r2) / h;
	float	a	= Sqrt( 1.0 - b*b );
	float	k	= Dot( p, float2(-b,a) );
	if ( k < 0.0 ) return Length( p ) - r1;
	if ( k > a*h ) return Length( p - float2(0.0,h) ) - r2;
	return Dot( p, float2(a,b) ) - r1;
}


float  SDF2_Hexagram (float2 p, const float r)
{
	const float4 k = float4(-0.5,0.8660254038,0.5773502692,1.7320508076);
	p = Abs(p);
	p -= 2.0 * Min( Dot( k.xy, p ), 0.0 ) * k.xy;
	p -= 2.0 * Min( Dot( k.yx, p ), 0.0 ) * k.yx;
	p -= float2(Clamp( p.x, r * k.z, r * k.w ), r );
	return Length(p) * Sign(p.y);
}


float  SDF2_Star5 (float2 p, const float r, const float rf)
{
	const float2 k1 = float2(0.809016994375, -0.587785252292);
	const float2 k2 = float2(-k1.x,k1.y);
	p.x	= Abs(p.x);
	p	-= 2.0 * Max( Dot( k1, p ), 0.0 ) * k1;
	p	-= 2.0 * Max( Dot( k2, p ), 0.0 ) * k2;
	p.x	= Abs(p.x);
	p.y	-= r;
	float2	ba = rf * float2(-k1.y, k1.x) - float2(0.0, 1.0);
	float	h  = Clamp( Dot(p,ba) / LengthSq(ba), 0.0, r );
	return Length( p - ba * h ) * Sign( p.y * ba.x - p.x * ba.y );
}


//-----------------------------------------------------------------------------
// 3D Shapes

float  SDF_Sphere (const float3 position, const float radius)
{
	return Length( position ) - radius;
}


float  SDF_Ellipsoid (const float3 position, const float3 radius)
{
	const float  k0 = Length( position / radius );
	const float  k1 = Length( position / (radius * radius) );
	return k0 * (k0 - 1.0f) / k1;
}


float  SDF_Box (const float3 position, const float3 halfSize)
{
	const float3  d = Abs( position ) - halfSize;
	return Min( Max( d.x, Max( d.y, d.z )), 0.0f ) + Length( Max( d, 0.0001f ));
}


float  SDF_RoundedBox (const float3 position, const float3 halfSize, const float radius)
{
	return SDF_Box( position, halfSize ) - radius;
}


float  SDF_BoxFrame (const float3 position, const float3 halfSize, const float width)
{
	const float3  p = Abs( position ) - halfSize;
	const float3  q = Abs( p + width ) - width;
	return	Min(
				Min(
					Length( Max( float3( p.x, q.y, q.z ), 0.0f )) + Min( Max( p.x, Max( q.y, q.z )), 0.0f ),
					Length( Max( float3( q.x, p.y, q.z ), 0.0f )) + Min( Max( q.x, Max( p.y, q.z )), 0.0f )),
				Length( Max( float3( q.x, q.y, p.z ), 0.0f )) + Min( Max( q.x, Max( q.y, p.z )), 0.0f ));
}


float SDF_Torus (const float3 position, const float2 outerAndInnerRadius)
{
	const float2  q = float2( Length( position.xz ) - outerAndInnerRadius.x, position.y );
	return Length( q ) - outerAndInnerRadius.y;
}


float  SDF_Cylinder (const float3 position, const float2 radiusHeight)
{
	const float2  d = Abs( float2( Length( position.xz ), position.y )) - radiusHeight;
	return Min( Max( d.x, d.y ), 0.0f ) + Length( Max( d, 0.0f ));
}


float  SDF_InfiniteCone (const float3 position, const float2 direction)
{
	// 'direction' must be normalized
	const float  q = Length( position.xy );
	return Dot( direction, float2( q, position.z ));
}


float  SDF_Cone (const float3 position, const float2 angleSinCos, const float height)
{
	float2	q = height * float2( angleSinCos.x / angleSinCos.y, -1.0 );
	float2	w = float2( Length( position.xz ), position.y );
	float2	a = w - q * Saturate( Dot( w, q ) / Dot( q, q ));
	float2	b = w - q * float2( Saturate( w.x / q.x ), 1.0 );
	float	k = Sign( q.y );
	float	d = Min( Dot( a, a ), Dot( b, b ));
	float	s = Max( k*(w.x * q.y - w.y * q.x), k * (w.y - q.y) );
	return Sqrt(d) * Sign(s);
}


float  SDF_Plane (const float3 position, const float3 norm, const float dist)
{
	// 'norm' must be normalized
	return Dot( position, norm ) + dist;
}


float  SDF_Plane (const float3 center, const float3 planePos, const float3 pos)
{
	const float3  v = center - planePos;
	const float   d = Length( v );
	return Dot( v / d, pos ) - d;
}


float  SDF_HexagonalPrism (const float3 position, const float2 h)
{
	const float3  q = Abs( position );
	return Max( q.z - h.y, Max( q.x * 0.866025f + q.y * 0.5f, q.y ) - h.x );
}


float  SDF_TriangularPrism (const float3 position, const float2 h)
{
	const float3  q = Abs( position );
	return Max( q.z - h.y, Max( q.x * 0.866025f + position.y * 0.5f, -position.y ) - h.x * 0.5f );
}


float  SDF_Capsule (const float3 position, const float3 a, const float3 b, const float r)
{
	const float3  pa = position - a;
	const float3  ba = b - a;
	const float   h  = Saturate( Dot( pa, ba ) / Dot( ba, ba ));
	return Length( pa - ba * h ) - r;
}


float  SDF_CappedCone (const float3 position, const float height, const float r1, const float r2)
{
	const float2  q  = float2( Length( position.xz ), position.y );
	const float2  k1 = float2( r2, height );
	const float2  k2 = float2( r2 - r1, 2.0f * height );
	const float2  ca = float2( q.x - Min( q.x, (q.y < 0.0f) ? r1 : r2 ), Abs( q.y ) - height );
	const float2  cb = q - k1 + k2 * Saturate( Dot( k1 - q, k2 ) / Dot( k2, k2 ));
	const float   s  = (cb.x < 0.0f and ca.y < 0.0f) ? -1.0f : 1.0f;
	return s * Sqrt( Min( Dot( ca, ca ), Dot( cb, cb )));
}


float  SDF_Octahedron (const float3 position, const float size)
{
	const float3  p = Abs( position );
	const float   m = p.x + p.y + p.z - size;
	float3		  q;

	if ( 3.0 * p.x < m )	q = p.xyz;			else
	if ( 3.0 * p.y < m )	q = p.yzx;			else
	if ( 3.0 * p.z < m )	q = p.zxy;			else
							return m * 0.57735027f;

	const float  k = Clamp( 0.5f * (q.z - q.y + size), 0.0f, size );
	return Length( float3( q.x, q.y - size + k, q.z - k ));
}


float  SDF_Pyramid (const float3 position, const float height)
{
	const float	m2 = height * height + 0.25f;
	float3		p  = position;

	p.xz = Abs( p.xz );
	p.xz = (p.z > p.x) ? p.zx : p.xz;
	p.xz -= 0.5f;

	const float3  q = float3( p.z, height * p.y - 0.5f * p.x, height * p.x + 0.5f * p.y);

	const float   s = max(-q.x,0.0);
	const float   t = clamp( (q.y-0.5*p.z)/(m2+0.25), 0.0, 1.0 );

	const float   a = m2*(q.x+s)*(q.x+s) + q.y*q.y;
	const float   b = m2*(q.x+0.5*t)*(q.x+0.5*t) + (q.y-m2*t)*(q.y-m2*t);

	const float   d2 = Min( q.y, -q.x * m2 - q.y * 0.5f ) > 0.0f ? 0.0f : Min( a, b );

	return Sqrt( (d2 + q. z * q.z) / m2 ) * SignOrZero( Max( q.z, -p.y ));
}


//-----------------------------------------------------------------------------
// Unions

float  SDF_OpUnite (const float d1, const float d2)
{
	return Min( d1, d2 );
}


float  SDF_OpSub (const float d1, const float d2)
{
	return Max( d1, -d2 );
}


float  SDF_OpIntersect (const float d1, const float d2)
{
	return Max( d1, d2 );
}


// TODO: https://iquilezles.org/articles/smin/
float  SDF_OpUnite (const float d1, const float d2, const float smoothFactor)
{
	const float  h = Saturate( 0.5f + 0.5f * (d2-d1) / smoothFactor );
	return Lerp( d2, d1, h ) - smoothFactor * h * (1.0f - h);
}


float  SDF_OpSub (const float d1, const float d2, float smoothFactor)
{
	const float  h = Saturate( 0.5f - 0.5f * (d2+d1) / smoothFactor );
	return Lerp( d2, -d1, h ) + smoothFactor * h * (1.0f - h);
}


float  SDF_OpIntersect (const float d1, const float d2, float smoothFactor)
{
	const float  h = Saturate( 0.5f - 0.5f * (d2-d1) / smoothFactor );
	return Lerp( d2, d1, h ) + smoothFactor * h * (1.0f - h);
}


float  SDF_OpRoundedShape (const float dist, const float radius)
{
	return dist - radius;
}


float  SDF_OpAnnularShape (const float dist, const float radius)
{
	return Abs( dist ) - radius;
}


float  SDF_OpExtrusion (const float posZ, const float distXY, const float height)
{
	// distXY - distance in 2D space
	// posZ - Z coordinate

	const float2  w = float2( distXY, Abs(posZ) - height );
	return Min( Max( w.x, w.y ), 0.0f ) + Length( Max( w, 0.0f ));
}


#define SDF_OpRevolution( _position_, _sdf2_, _offset_ ) \
	_sdf2_(float2( Length(_position_.xz) - _offset_, _position_.y ))


float  SDF_Length2 (float3 p)
{
	p = p*p;
	return Sqrt( p.x + p.y + p.z );
}

float  SDF_Length6 (float3 p)
{
	p = p*p*p;
	p = p*p;
	return Pow( p.x + p.y + p.z, 1.0f/6.0f );
}

float  SDF_Length8 (float3 p)
{
	p = p*p;
	p = p*p;
	p = p*p;
	return Pow( p.x + p.y + p.z, 1.0f/8.0f );
}


//-----------------------------------------------------------------------------
// Transformation

float2  SDF_Move (const float2 position, const float2 delta)
{
	return position - delta;
}

float2  SDF_Move (const float2 position, const float delta)
{
	return position - delta;
}

float2  SDF_Move (const float2 position, const float dx, const float dy)
{
	return position - float2(dx, dy);
}

float3  SDF_Move (const float3 position, const float delta)
{
	return position - delta;
}

float3  SDF_Move (const float3 position, const float dx, const float dy, const float dz)
{
	return position - float3(dx, dy, dz);
}

float3  SDF_Move (const float3 position, const float3 delta)
{
	return position - delta;
}

float3  SDF_Rotate (const float3 position, const Quat q)
{
	return QMul( QInverse( q ), position );
}

float3  SDF_Transform (const float3 position, const Quat q, const float3 delta)
{
	return SDF_Rotate( SDF_Move( position, delta ), q );
}

float2  SDF_Rotate2D (const float2 p, const float angle)
{
	const float2  sc = SinCos( angle );
	return float2( p.x * sc.y - p.y * sc.x,
				   p.x * sc.x + p.y * sc.y );
}

#define SDF_Scale( _pos_, _scale_, _sdf_ )\
	(_sdf_( (_pos_)/(_scale_) ) * _scale_)

#define SDF_OpSymX( _pos_, _sdf_ )\
	(_sdf_(float3( Abs((_pos_).x), (_pos_).yz )))

#define SDF_OpSymXZ( _pos_, _sdf_ )\
	(_sdf_(float3( Abs((_pos_).x), (_pos_).y, Abs((_pos_).z) )))

#define SDF_InfRepetition( _pos_, _step_, _sdf_ )\
	(_sdf_( Mod( (_pos_) + 0.5f * (_step_), (_step_) ) - 0.5f * (_step_) ))

#define SDF_Repetition( _pos_, _step_, _count_, _sdf_ )\
	(_sdf_( (_pos_) - (_step_) * Clamp( Round( (_pos_)/(_step_) ), -(_count_), (_count_) )))


//-----------------------------------------------------------------------------
// Utils

// calculate normal for SDF scene
// https://iquilezles.org/articles/normalsSDF/

// Forward and central differences
#define GEN_SDF_NORMAL_6sp_FN( _fnName_, _sdf_ )				\
	ND_ float3  _fnName_ (const float3 pos, const float eps)	\
	{															\
		const float2  h		= float2( eps, 0.0f );				\
		const float3  norm	= float3(							\
			_sdf_( pos + h.xyy ) - _sdf_( pos - h.xyy ),		\
			_sdf_( pos + h.yxy ) - _sdf_( pos - h.yxy ),		\
			_sdf_( pos + h.yyx ) - _sdf_( pos - h.yyx ) );		\
		return Normalize( norm );								\
	}															\
																\
	ND_ float3  _fnName_ (const float3 pos) {					\
		return _fnName_( pos, 0.0001f );						\
	}


// Tetrahedron
#define GEN_SDF_NORMAL_4sp_FN( _fnName_, _sdf_ )				\
	ND_ float3  _fnName_ (const float3 pos, const float eps)	\
	{															\
		const float2  k		= float2(1.f, -1.f);				\
		const float3  norm	= 									\
			k.xyy * _sdf_( pos + k.xyy * eps ) +				\
			k.yyx * _sdf_( pos + k.yyx * eps ) +				\
			k.yxy * _sdf_( pos + k.yxy * eps ) +				\
			k.xxx * _sdf_( pos + k.xxx * eps );					\
		return Normalize( norm );								\
	}															\
																\
	ND_ float3  _fnName_ (const float3 pos) {					\
		return _fnName_( pos, 0.0001f );						\
	}


// Calc normal in 3x3x3 cube
#define GEN_SDF_NORMAL_27sp_FN( _fnName_, _sdf_ )				\
	ND_ float3  _fnName_ (const float3 pos, const float eps)	\
	{															\
		float3	norm = float3(0.0);								\
		for (int z = -1; z <= 1; ++z)							\
		for (int y = -1; y <= 1; ++y)							\
		for (int x = -1; x <= 1; ++x)							\
		{														\
			float3	dir = float3(x, y, z);						\
			norm += dir * _sdf_( pos + dir * eps );				\
		}														\
		return Normalize( norm );								\
	}															\
																\
	ND_ float3  _fnName_ (const float3 pos) {					\
		return _fnName_( pos, 0.0001f );						\
	}


#endif // AE_ENABLE_UNKNOWN_LICENSE
