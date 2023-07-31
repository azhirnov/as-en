/*
    Default signed distance fields.

    from:
        https://iquilezles.org/articles/distfunctions/
        https://iquilezles.org/articles/distfunctions2d/
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Quaternion.glsl"


// 2D Shapes
ND_ float  SDF_Line (const float2 position, const float2 point0, const float2 point1);
ND_ float  SDF_Rect (const float2 position, const float2 hsize);
ND_ float  SDF_Circle (const float2 position, const float radius);
ND_ float  SDF_Pentagon (const float2 position, const float radius);


// 3D Shapes
ND_ float  SDF_Sphere (const float3 position, const float radius);
ND_ float  SDF_Ellipsoid (const float3 position, const float3 radius);
ND_ float  SDF_Box (const float3 position, const float3 halfSize);
ND_ float  SDF_BoxFrame (const float3 position, const float3 halfSize, const float width);
ND_ float  SDF_Torus (const float3 position, const float2 outerAndInnerRadius);
ND_ float  SDF_Cylinder (const float3 position, const float2 radiusHeight);
ND_ float  SDF_Cone (const float3 position, const float2 direction);
ND_ float  SDF_Plane (const float3 position, const float3 norm, const float dist);
ND_ float  SDF_Plane (const float3 center, const float3 planePos, const float3 pos);
ND_ float  SDF_HexagonalPrism (const float3 position, const float2 h);
ND_ float  SDF_TriangularPrism (const float3 position, const float2 h);
ND_ float  SDF_Capsule (const float3 position, const float3 a, const float3 b, const float r);
ND_ float  SDF_CappedCone (const float3 position, const float height, const float r1, const float r2);
ND_ float  SDF_Octahedron (const float3 position, const float size);
ND_ float  SDF_Pyramid (const float3 position, const float height);


// Unions
ND_ float  SDF_OpUnite (const float d1, const float d2);
ND_ float  SDF_OpUnite (const float d1, const float d2, const float smoothFactor);
ND_ float  SDF_OpSub (const float d1, const float d2);
ND_ float  SDF_OpSub (const float d1, const float d2, float smoothFactor);
ND_ float  SDF_OpIntersect (const float d1, const float d2);
ND_ float  SDF_OpIntersect (const float d1, const float d2, float smoothFactor);
ND_ float  SDF_OpRoundedShape (const float dist, const float radius);
ND_ float  SDF_OpAnnularShape (const float dist, const float radius);
ND_ float  SDF_OpExtrusion (const float posZ, const float dist, const float height);
#if 0 // macros
ND_ float  SDF_OpRevolution (const float3 position, float (*sdf)(float2), float offset);
#endif

ND_ float  SDF_Length2 (float3 position);
ND_ float  SDF_Length6 (float3 position);
ND_ float  SDF_Length8 (float3 position);


// Transformation
ND_ float2  SDF_Move (const float2 position, const float  delta);
ND_ float2  SDF_Move (const float2 position, const float2 delta);
ND_ float3  SDF_Move (const float3 position, const float  delta);
ND_ float3  SDF_Move (const float3 position, const float3 delta);
ND_ float3  SDF_Rotate (const float3 position, const quat q);
ND_ float2  SDF_Rotate2D (const float2 position, const float angle);
ND_ float3  SDF_Transform (const float3 position, const quat q, const float3 delta);

#if 0 // macros
ND_ float  SDF_Scale (const float3 position, float scale, float (*sdf)(float3));
ND_ float  SDF_OpSymX (const float3 position, float (*sdf)(float3));
ND_ float  SDF_OpSymXZ (const float3 position, float (*sdf)(float3));
ND_ float  SDF_InfRepetition (const float3 position, const float3 center, float (*sdf)(float3));
ND_ float  SDF_Repetition (const float3 position, const float c, const float3 l, float (*sdf)(float3));
#endif



//-----------------------------------------------------------------------------
// 2D Shapes

float  SDF_Line (const float2 position, const float2 point0, const float2 point1)
{
    const float2  pa = position - point0;
    const float2  ba = point1 - point0;
    const float   h  = Saturate( Dot( pa, ba ) / Dot( ba, ba ));
    return Length( pa - ba * h );
}


float  SDF_Rect (const float2 position, const float2 hsize)
{
    const float2  d = Abs( position ) - hsize;
    return Length(Max( d, float2(0.0f) )) + Min(Max( d.x, d.y ), 0.0f );
}


float  SDF_Circle (const float2 position, const float radius)
{
    return Length( position ) - radius;
}


float  SDF_Pentagon (const float2 position, const float radius)
{
    const float3  k = float3( 0.809016994f, 0.587785252f, 0.726542528f );
          float2  p = position;
    p.x = Abs(p.x);
    p -= 2.0f * Min(Dot( float2(-k.x,k.y), p ), 0.0f) * float2(-k.x,k.y);
    p -= 2.0f * Min(Dot( float2( k.x,k.y), p ), 0.0f) * float2( k.x,k.y);
    p -= float2( Clamp( p.x, -radius * k.z, radius * k.z ), radius );
    return Length(p) * SignOrZero(p.y);
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
    return Min( Max( d.x, Max( d.y, d.z )), 0.0f ) + Length( Max( d, 0.0f ));
}


float  SDF_BoxFrame (const float3 position, const float3 halfSize, const float width)
{
    const float3  p = Abs( position ) - halfSize;
    const float3  q = Abs( p + width ) - width;
    return  Min(
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


float  SDF_Cone (const float3 position, const float2 direction)
{
    // 'direction' must be normalized
    const float  q = Length( position.xy );
    return Dot( direction, float2( q, position.z ));
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
    const float3  ba = position - a;
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
    float3        q;

    if ( 3.0 * p.x < m )    q = p.xyz;          else
    if ( 3.0 * p.y < m )    q = p.yzx;          else
    if ( 3.0 * p.z < m )    q = p.zxy;          else
                            return m * 0.57735027f;

    const float  k = Clamp( 0.5f * (q.z - q.y + size), 0.0f, size ); 
    return Length( float3( q.x, q.y - size + k, q.z - k )); 
}


float  SDF_Pyramid (const float3 position, const float height)
{
    const float  m2 = height * height + 0.25f;
    float3       p  = position;

    p.xz = Abs( p.xz );
    p.xz = (p.z > p.x) ? p.zx : p.xz;
    p.xz -= 0.5f;

    const float3  q = float3( p.z, height * p.y - 0.5f * p.x, height * p.x + 0.5f * p.y);

    const float   s = max(-q.x,0.0);
    const float   t = clamp( (q.y-0.5*p.z)/(m2+0.25), 0.0, 1.0 );

    const float   a = m2*(q.x+s)*(q.x+s) + q.y*q.y;
    const float   b = m2*(q.x+0.5*t)*(q.x+0.5*t) + (q.y-m2*t)*(q.y-m2*t);

    const float   d2 = Min( q.y, -q.x * m2 - q.y * 0.5f ) > 0.0f ? 0.0f : Min( a, b );

    return Sqrt( (d2 + q. z * q.z) / m2 ) * Sign( Max( q.z, -p.y ));
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


float  SDF_OpExtrusion (const float posZ, const float dist, const float height)
{
    const float2  w = float2( dist, Abs(posZ) - height );
    return Min( Max( w.x, w.y ), 0.0f ) + Length( Max( w, 0.0f ));
}


#define SDF_OpRevolution( _position_, _sdf2_, _offset_ ) \
    _sdf2_(float2( Length(_position_.xz) - _offset_, _position_.y ))


float  SDF_Length2 (float3 p)
{
    p = p*p;
    return Sqrt( p.x + p.y + p.z);
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

float3  SDF_Move (const float3 position, const float delta)
{
    return position - delta;
}

float3  SDF_Move (const float3 position, const float3 delta)
{
    return position - delta;
}


float3  SDF_Rotate (const float3 position, const quat q)
{
    return QMul( QInverse( q ), position );
}


float3  SDF_Transform (const float3 position, const quat q, const float3 delta)
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
    _sdf_(float3( Abs((_pos_).x), (_pos_).yz ))

#define SDF_OpSymXZ( _pos_, _sdf_ )\
    _sdf_(float3( Abs((_pos_).x), (_pos_).y, Abs((_pos_).z) ))

#define SDF_InfRepetition( _pos_, _center_, _sdf_ )\
    _sdf_( Mod( (_pos_) + 0.5f * (_center_), (_center_) ) - 0.5f * (_center_) )

#define SDF_Repetition( _pos_, _c_, _l_, _sdf_ )\
    _sdf_( (_pos_) - (_c_) * Clamp( Round( (_pos_)/(_c_) ), -(_l_), (_l_) ))


//-----------------------------------------------------------------------------
// Utils

// calculate normal for SDF scene
// https://iquilezles.org/articles/normalsSDF/

#define GEN_SDF_NORMAL_FN( _fnName_, _sdf_, _field_ )                           \
    ND_ float3  _fnName_ (const float3 pos)                                     \
    {                                                                           \
        const float2  eps  = float2( 0.001f, 0.0f );                            \
        const float3  norm = float3(                                            \
            _sdf_( pos + eps.xyy ) _field_ - _sdf_( pos - eps.xyy ) _field_,    \
            _sdf_( pos + eps.yxy ) _field_ - _sdf_( pos - eps.yxy ) _field_,    \
            _sdf_( pos + eps.yyx ) _field_ - _sdf_( pos - eps.yyx ) _field_ );  \
        return Normalize( norm );                                               \
    }

#define GEN_SDF_NORMAL_FN2( _fnName_, _sdf_, _field_ )  \
    ND_ float3  _fnName_ (const float3 pos)             \
    {                                                   \
        const float   h    = 0.001f;                    \
        const float2  k    = float2(1.f, -1.f);         \
        const float3  norm = float3(                    \
            k.xyy * _sdf_( pos + k.xyy * h ) _field_ +  \
            k.yyx * _sdf_( pos + k.yyx * h ) _field_ +  \
            k.yxy * _sdf_( pos + k.yxy * h ) _field_ +  \
            k.xxx * _sdf_( pos + k.xxx * h ) _field_ ); \
        return Normalize( norm );                       \
    }

