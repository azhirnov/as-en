// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Default math types and functions
*/

#ifdef __cplusplus
# pragma once
#endif

#define and             &&
#define or              ||

#define Any             any
#define All             all
#define Abs             abs
#define ACos            acos
#define ASin            asin
#define ASinH           asinh
#define ACosH           acosh
#define ATan            atan            // result in range [-Pi...+Pi]
#define BitScanReverse  findMSB
#define BitScanForward  findLSB
#define ATanH           atanh
#define Clamp           clamp
#define Ceil            ceil
#define Cos             cos
#define CosH            cosh
#define Cross           cross
#define Distance        distance
#define Dot             dot
#define Exp             exp
#define Exp2            exp2
#define Fract           fract           // x - Floor( x )
#define Floor           floor
#define FusedMulAdd     fma             // (a * b) + c
#define IsNaN           isnan
#define IsInfinity      isinf
#define IsFinite( x )   (! IsInfinity( x ) && ! IsNaN( x ))
#define InvSqrt         inversesqrt
#define IntLog2         BitScanReverse
#define Length          length
#define Lerp            mix
#define Ln              log
#define Log2            log2
#define Log( x, base )  (Ln(x) / Ln(base))
#define Log10( x )      (Ln(x) * 0.4342944819032518)
#define Min             min
#define Max             max
#define Mod             mod
#define MatInverse      inverse
#define MatTranspose    transpose
#define Normalize       normalize
#define Pow             pow
#define Round           round
#define Reflect         reflect
#define Refract         refract
#define Step            step
#define SmoothStep      smoothstep
#define Saturate( x )   clamp( x, 0.0f, 1.0f )
#define Sqrt            sqrt
#define Sin             sin
#define SinH            sinh
#define SignOrZero      sign            // -1, 0, +1, nan = 0
#define Tan             tan
#define TanH            tanh
#define Trunc           trunc
#define ToUNorm( x )    ((x) * 0.5f + 0.5f)
#define ToSNorm( x )    ((x) * 2.0f - 1.0f)
#define BitCount        bitCount
#define ToDeg           degrees
#define ToRad           radians
//-----------------------------------------------------------------------------


// to mark 'out' and 'inout' argument in function call
// in function argument list use defined by GLSL qualificators: in, out, inout
#define OUT
#define INOUT

//-----------------------------------------------------------------------------

#define Less            lessThan            // <
#define Greater         greaterThan         // >
#define LessEqual       lessThanEqual       // <=
#define GreaterEqual    greaterThanEqual    // >=
#define Not             not
//#define not               !

ND_ bool   Equals (const float  lhs, const float  rhs)      { return lhs == rhs; }
ND_ bool2  Equals (const float2 lhs, const float2 rhs)      { return equal( lhs, rhs ); }
ND_ bool3  Equals (const float3 lhs, const float3 rhs)      { return equal( lhs, rhs ); }
ND_ bool4  Equals (const float4 lhs, const float4 rhs)      { return equal( lhs, rhs ); }

ND_ bool   Equals (const int  lhs, const int  rhs)          { return lhs == rhs; }
ND_ bool2  Equals (const int2 lhs, const int2 rhs)          { return equal( lhs, rhs ); }
ND_ bool3  Equals (const int3 lhs, const int3 rhs)          { return equal( lhs, rhs ); }
ND_ bool4  Equals (const int4 lhs, const int4 rhs)          { return equal( lhs, rhs ); }

ND_ bool   Equals (const uint  lhs, const uint  rhs)        { return lhs == rhs; }
ND_ bool2  Equals (const uint2 lhs, const uint2 rhs)        { return equal( lhs, rhs ); }
ND_ bool3  Equals (const uint3 lhs, const uint3 rhs)        { return equal( lhs, rhs ); }
ND_ bool4  Equals (const uint4 lhs, const uint4 rhs)        { return equal( lhs, rhs ); }

ND_ bool   Equals (const double  lhs, const double  rhs)    { return lhs == rhs; }
ND_ bool2  Equals (const double2 lhs, const double2 rhs)    { return equal( lhs, rhs ); }
ND_ bool3  Equals (const double3 lhs, const double3 rhs)    { return equal( lhs, rhs ); }
ND_ bool4  Equals (const double4 lhs, const double4 rhs)    { return equal( lhs, rhs ); }

#define AllLess( a, b )         All( Less( (a), (b) ))
#define AllLessEqual( a, b )    All( LessEqual( (a), (b) ))

#define AllGreater( a, b )      All( Greater( (a), (b) ))
#define AllGreaterEqual( a, b ) All( GreaterEqual( (a), (b) ))

#define AnyLess( a, b )         Any( Less( (a), (b) ))
#define AnyLessEqual( a, b )    Any( LessEqual( (a), (b) ))

#define AnyGreater( a, b )      Any( Greater( (a), (b) ))
#define AnyGreaterEqual( a, b ) Any( GreaterEqual( (a), (b) ))

#define AllEqual( a, b )        All( Equals( (a), (b) ))
#define AnyEqual( a, b )        Any( Equals( (a), (b) ))

#define AllNotEqual( a, b )     All( Not( Equals( (a), (b) )))
#define AnyNotEqual( a, b )     Any( Not( Equals( (a), (b) )))

#define NotAllEqual( a, b )     !All( Equals( (a), (b) ))
#define NotAnyEqual( a, b )     !Any( Equals( (a), (b) ))

//-----------------------------------------------------------------------------


#define Min3( a, b, c )         Min( Min( (a), (b) ), (c) )
#define Min4( a, b, c, d )      Min( Min( (a), (b) ), Min( (c), (d) ))
#define Max3( a, b, c )         Max( Max( (a), (b) ), (c) )
#define Max4( a, b, c, d )      Max( Max( (a), (b) ), Max( (c), (d) ))


//-----------------------------------------------------------------------------
// Constants

ND_ float  Epsilon ()               { return 1.e-5f; }
ND_ float  Pi ()                    { return 3.14159265358979323846f; }
ND_ float  Pi2 ()                   { return Pi() * 2.0f; }

ND_ float  ReciprocalPi ()          { return 0.31830988618379067153f; }
ND_ float  SqrtOf2 ()               { return 1.41421356237309504880f; }


//-----------------------------------------------------------------------------
// Square

ND_ float   Square (const float x)      { return x * x; }
ND_ float2  Square (const float2 x)     { return x * x; }
ND_ float3  Square (const float3 x)     { return x * x; }
ND_ float4  Square (const float4 x)     { return x * x; }

ND_ int     Square (const int x)        { return x * x; }
ND_ int2    Square (const int2 x)       { return x * x; }
ND_ int3    Square (const int3 x)       { return x * x; }
ND_ int4    Square (const int4 x)       { return x * x; }

ND_ uint    Square (const uint x)       { return x * x; }
ND_ uint2   Square (const uint2 x)      { return x * x; }
ND_ uint3   Square (const uint3 x)      { return x * x; }
ND_ uint4   Square (const uint4 x)      { return x * x; }


//-----------------------------------------------------------------------------
// Same as 'condition ? ifTrue : ifFalse'

ND_ float2  Select (const bool2 condition, const float2 ifTrue, const float2 ifFalse)       { return Lerp( ifFalse, ifTrue, float2(condition) ); }
ND_ float3  Select (const bool3 condition, const float3 ifTrue, const float3 ifFalse)       { return Lerp( ifFalse, ifTrue, float3(condition) ); }
ND_ float4  Select (const bool4 condition, const float4 ifTrue, const float4 ifFalse)       { return Lerp( ifFalse, ifTrue, float4(condition) ); }

ND_ int2    Select (const bool2 condition, const int2 ifTrue, const int2 ifFalse)           { return (ifFalse * int2(Not(condition))) + (ifTrue * int2(condition)); }
ND_ int3    Select (const bool3 condition, const int3 ifTrue, const int3 ifFalse)           { return (ifFalse * int3(Not(condition))) + (ifTrue * int3(condition)); }
ND_ int4    Select (const bool4 condition, const int4 ifTrue, const int4 ifFalse)           { return (ifFalse * int4(Not(condition))) + (ifTrue * int4(condition)); }


//-----------------------------------------------------------------------------
// square length and distance

ND_ float  LengthSq (const float2 x)        { return Dot( x, x ); }
ND_ float  LengthSq (const float3 x)        { return Dot( x, x ); }

ND_ float  DistanceSq (const float2 x, const float2 y)  { float2 r = x - y;  return Dot( r, r ); }
ND_ float  DistanceSq (const float3 x, const float3 y)  { float3 r = x - y;  return Dot( r, r ); }


//-----------------------------------------------------------------------------
// Other functions

// -1 or +1, nan = +1
ND_ float   Sign (const float  x)       { return x < 0.0f ? -1.0f : 1.0f; }
ND_ float2  Sign (const float2 v)       { return Select( Less( v, float2(0.f) ), float2(-1.0f), float2(1.0f) ); }
ND_ float3  Sign (const float3 v)       { return Select( Less( v, float3(0.f) ), float3(-1.0f), float3(1.0f) ); }
ND_ float4  Sign (const float4 v)       { return Select( Less( v, float4(0.f) ), float4(-1.0f), float4(1.0f) ); }

ND_ int   Sign (const int  x)           { return  x < 0 ? -1 : 1; }
ND_ int2  Sign (const int2 v)           { return Select( Less( v, int2(0) ), int2(-1), int2(1) ); }
ND_ int3  Sign (const int3 v)           { return Select( Less( v, int3(0) ), int3(-1), int3(1) ); }
ND_ int4  Sign (const int4 v)           { return Select( Less( v, int4(0) ), int4(-1), int4(1) ); }

ND_ float2  SinCos (const float x)      { return float2(sin(x), cos(x)); }


//-----------------------------------------------------------------------------
// clamp / wrap

ND_ float  ClampOut (const float x, const float minVal, const float maxVal)
{
    float mid = (minVal + maxVal) * 0.5f;
    return x < mid ? (x < minVal ? x : minVal) : (x > maxVal ? x : maxVal);
}

ND_ int  ClampOut (const int x, const int minVal, const int maxVal)
{
    int mid = (minVal+1)/2 + (maxVal+1)/2;
    return x < mid ? (x < minVal ? x : minVal) : (x > maxVal ? x : maxVal);
}

ND_ float2  ClampOut (const float2 v, const float minVal, const float maxVal) {
    return float2( ClampOut( v.x, minVal, maxVal ),
                   ClampOut( v.y, minVal, maxVal ));
}

ND_ float3  ClampOut (const float3 v, const float minVal, const float maxVal) {
    return float3( ClampOut( v.x, minVal, maxVal ),
                   ClampOut( v.y, minVal, maxVal ),
                   ClampOut( v.z, minVal, maxVal ));
}

ND_ float4  ClampOut (const float4 v, const float minVal, const float maxVal) {
    return float4( ClampOut( v.x, minVal, maxVal ),
                   ClampOut( v.y, minVal, maxVal ),
                   ClampOut( v.z, minVal, maxVal ),
                   ClampOut( v.w, minVal, maxVal ));
}

ND_ int2  ClampOut (const int2 v, const int minVal, const int maxVal) {
    return int2( ClampOut( v.x, minVal, maxVal ),
                 ClampOut( v.y, minVal, maxVal ));
}

ND_ int3  ClampOut (const int3 v, const int minVal, const int maxVal) {
    return int3( ClampOut( v.x, minVal, maxVal ),
                 ClampOut( v.y, minVal, maxVal ),
                 ClampOut( v.z, minVal, maxVal ));
}

ND_ int4  ClampOut (const int4 v, const int minVal, const int maxVal) {
    return int4( ClampOut( v.x, minVal, maxVal ),
                 ClampOut( v.y, minVal, maxVal ),
                 ClampOut( v.z, minVal, maxVal ),
                 ClampOut( v.w, minVal, maxVal ));
}


ND_ float  Wrap (const float x, const float minVal, const float maxVal)
{
    if ( maxVal < minVal ) return minVal;
    float size = maxVal - minVal;
    float res = minVal + Mod( x - minVal, size );
    if ( res < minVal ) return res + size;
    return res;
}

ND_ int  Wrap (const int x, const int minVal, const int maxVal)
{
    if ( maxVal < minVal ) return minVal;
    int size = maxVal+1 - minVal;
    int res = minVal + ((x - minVal) % size);
    if ( res < minVal ) return res + size;
    return res;
}

ND_ float2  Wrap (const float2 v, const float minVal, const float maxVal) {
    return float2( Wrap( v.x, minVal, maxVal ),
                   Wrap( v.y, minVal, maxVal ));
}

ND_ float3  Wrap (const float3 v, const float minVal, const float maxVal) {
    return float3( Wrap( v.x, minVal, maxVal ),
                   Wrap( v.y, minVal, maxVal ),
                   Wrap( v.z, minVal, maxVal ));
}

ND_ float4  Wrap (const float4 v, const float minVal, const float maxVal) {
    return float4( Wrap( v.x, minVal, maxVal ),
                   Wrap( v.y, minVal, maxVal ),
                   Wrap( v.z, minVal, maxVal ),
                   Wrap( v.w, minVal, maxVal ));
}

ND_ float2 Wrap (const float2 v, const float2 minVal, const float2 maxVal) {
    return float2( Wrap( v.x, minVal.x, maxVal.y ),
                   Wrap( v.y, minVal.x, maxVal.y ));
}

ND_ float3 Wrap (const float3 v, const float3 minVal, const float3 maxVal) {
    return float3( Wrap( v.x, minVal.x, maxVal.x ),
                   Wrap( v.y, minVal.y, maxVal.y ),
                   Wrap( v.z, minVal.z, maxVal.z ));
}

ND_ float4 Wrap (const float4 v, const float4 minVal, const float4 maxVal) {
    return float4( Wrap( v.x, minVal.x, maxVal.x ),
                   Wrap( v.y, minVal.y, maxVal.y ),
                   Wrap( v.z, minVal.z, maxVal.z ),
                   Wrap( v.w, minVal.w, maxVal.w ));
}

ND_ int2 Wrap (const int2 v, const float minVal, const float maxVal) {
    return int2( Wrap( v.x, minVal, maxVal ),
                 Wrap( v.y, minVal, maxVal ));
}

ND_ int3  Wrap (const int3 v, const int minVal, const int maxVal) {
    return int3( Wrap( v.x, minVal, maxVal ),
                 Wrap( v.y, minVal, maxVal ),
                 Wrap( v.z, minVal, maxVal ));
}

ND_ int4  Wrap (const int4 v, const int minVal, const int maxVal) {
    return int4( Wrap( v.x, minVal, maxVal ),
                 Wrap( v.y, minVal, maxVal ),
                 Wrap( v.z, minVal, maxVal ),
                 Wrap( v.w, minVal, maxVal ));
}


//-----------------------------------------------------------------------------
// bit operations

ND_ int  BitRotateLeft (const int x, uint shift)
{
    const uint mask = 31u;
    shift = shift & mask;
    return (x << shift) | (x >> ( ~(shift-1u) & mask ));
}

ND_ uint  BitRotateLeft (const uint x, uint shift)
{
    const uint mask = 31u;
    shift = shift & mask;
    return (x << shift) | (x >> ( ~(shift-1u) & mask ));
}


ND_ int  BitRotateRight (const int x, uint shift)
{
    const uint mask = 31u;
    shift = shift & mask;
    return (x >> shift) | (x << ( ~(shift-1u) & mask ));
}

ND_ uint  BitRotateRight (const uint x, uint shift)
{
    const uint mask = 31u;
    shift = shift & mask;
    return (x >> shift) | (x << ( ~(shift-1u) & mask ));
}


//-----------------------------------------------------------------------------
// interpolation

ND_ float   BaryLerp (const float  v0, const float  v1, const float  v2, const float3 barycentrics)  { return v0 * barycentrics.x + v1 * barycentrics.y + v2 * barycentrics.z; }
ND_ float2  BaryLerp (const float2 v0, const float2 v1, const float2 v2, const float3 barycentrics)  { return v0 * barycentrics.x + v1 * barycentrics.y + v2 * barycentrics.z; }
ND_ float3  BaryLerp (const float3 v0, const float3 v1, const float3 v2, const float3 barycentrics)  { return v0 * barycentrics.x + v1 * barycentrics.y + v2 * barycentrics.z; }
ND_ float4  BaryLerp (const float4 v0, const float4 v1, const float4 v2, const float3 barycentrics)  { return v0 * barycentrics.x + v1 * barycentrics.y + v2 * barycentrics.z; }

ND_ float   BaryLerp (const float  v0, const float  v1, const float  v2, const float2 barycentrics)  { return v0 + FusedMulAdd( barycentrics.x,    (v1 - v0), barycentrics.y * (v2 - v0)); }
ND_ float2  BaryLerp (const float2 v0, const float2 v1, const float2 v2, const float2 barycentrics)  { return v0 + FusedMulAdd( barycentrics.xx,   (v1 - v0), barycentrics.y * (v2 - v0)); }
ND_ float3  BaryLerp (const float3 v0, const float3 v1, const float3 v2, const float2 barycentrics)  { return v0 + FusedMulAdd( barycentrics.xxx,  (v1 - v0), barycentrics.y * (v2 - v0)); }
ND_ float4  BaryLerp (const float4 v0, const float4 v1, const float4 v2, const float2 barycentrics)  { return v0 + FusedMulAdd( barycentrics.xxxx, (v1 - v0), barycentrics.y * (v2 - v0)); }

ND_ float   BiLerp (const float  x1y1, const float  x2y1, const float  x1y2, const float  x2y2, const float2 factor)  { return Lerp( Lerp( x1y1, x2y1, factor.x ), Lerp( x1y2, x2y2, factor.x ), factor.y ); }
ND_ float2  BiLerp (const float2 x1y1, const float2 x2y1, const float2 x1y2, const float2 x2y2, const float2 factor)  { return Lerp( Lerp( x1y1, x2y1, factor.x ), Lerp( x1y2, x2y2, factor.x ), factor.y ); }
ND_ float3  BiLerp (const float3 x1y1, const float3 x2y1, const float3 x1y2, const float3 x2y2, const float2 factor)  { return Lerp( Lerp( x1y1, x2y1, factor.x ), Lerp( x1y2, x2y2, factor.x ), factor.y ); }
ND_ float4  BiLerp (const float4 x1y1, const float4 x2y1, const float4 x1y2, const float4 x2y2, const float2 factor)  { return Lerp( Lerp( x1y1, x2y1, factor.x ), Lerp( x1y2, x2y2, factor.x ), factor.y ); }


// map 'v' in 'src' interval to 'dst' interval.
// only for scalar range.
ND_ float   Remap (const float2 src, const float2 dst, const float  v)  { return (v - src.x) / (src.y - src.x) * (dst.y - dst.x) + dst.x; }
ND_ float2  Remap (const float2 src, const float2 dst, const float2 v)  { return (v - src.x) / (src.y - src.x) * (dst.y - dst.x) + dst.x; }
ND_ float3  Remap (const float2 src, const float2 dst, const float3 v)  { return (v - src.x) / (src.y - src.x) * (dst.y - dst.x) + dst.x; }
ND_ float4  Remap (const float2 src, const float2 dst, const float4 v)  { return (v - src.x) / (src.y - src.x) * (dst.y - dst.x) + dst.x; }

// map 'v' in 'src' interval to 'dst' interval.
ND_ float2  Remap (const float2 src0, const float2 src1, const float2 dst0, const float2 dst1, const float2 v)  { return (v - src0) / (src1 - src0) * (dst1 - dst0) + dst0; }
ND_ float3  Remap (const float3 src0, const float3 src1, const float3 dst0, const float3 dst1, const float3 v)  { return (v - src0) / (src1 - src0) * (dst1 - dst0) + dst0; }
ND_ float4  Remap (const float4 src0, const float4 src1, const float4 dst0, const float4 dst1, const float4 v)  { return (v - src0) / (src1 - src0) * (dst1 - dst0) + dst0; }


// map 'v' in 'src' interval to 'dst' interval and clamp.
// only for scalar range.
ND_ float   RemapClamped (const float2 src, const float2 dst, const float  v)  { return Clamp( Remap( src, dst, v ), dst.x, dst.y ); }
ND_ float2  RemapClamped (const float2 src, const float2 dst, const float2 v)  { return Clamp( Remap( src, dst, v ), dst.x, dst.y ); }
ND_ float3  RemapClamped (const float2 src, const float2 dst, const float3 v)  { return Clamp( Remap( src, dst, v ), dst.x, dst.y ); }
ND_ float4  RemapClamped (const float2 src, const float2 dst, const float4 v)  { return Clamp( Remap( src, dst, v ), dst.x, dst.y ); }

// map 'v' in 'src' interval to 'dst' interval and clamp.
ND_ float2  RemapClamped (const float2 src0, const float2 src1, const float2 dst0, const float2 dst1, const float2 v)  { return Clamp( Remap( src0, src1, dst0, dst1, v ), dst0, dst1 ); }
ND_ float3  RemapClamped (const float3 src0, const float3 src1, const float3 dst0, const float3 dst1, const float3 v)  { return Clamp( Remap( src0, src1, dst0, dst1, v ), dst0, dst1 ); }
ND_ float4  RemapClamped (const float4 src0, const float4 src1, const float4 dst0, const float4 dst1, const float4 v)  { return Clamp( Remap( src0, src1, dst0, dst1, v ), dst0, dst1 ); }
//-----------------------------------------------------------------------------


ND_ float2  SampleArray_Helper (const int len, const float f)
{
    float a = Clamp( f * (len - 1), 0.0, float(len-1) );
    float b = Floor( a );
    return float2( b, a - b );
}
#define SampleArray( _array_, _factor_ )                                            \
    Lerp(   _array_[int(SampleArray_Helper( _array_.length(), _factor_ ).x)],       \
            _array_[int(SampleArray_Helper( _array_.length(), _factor_ ).x) + 1],   \
            SampleArray_Helper( _array_.length(), _factor_ ).y )
//-----------------------------------------------------------------------------


#define InterpolateQuad( _arr_, _field_, _factor2_ )                \
    (Lerp( Lerp( _arr_[0] _field_, _arr_[1] _field_, _factor2_.x ), \
           Lerp( _arr_[3] _field_, _arr_[2] _field_, _factor2_.x ), \
           _factor2_.y ))

#define InterpolateTriangle( _arr_, _field_, _factor3_ )            \
    ( _factor3_.x * _arr_[0] _field_ +                              \
      _factor3_.y * _arr_[1] _field_ +                              \
      _factor3_.z * _arr_[2] _field_ )
//-----------------------------------------------------------------------------


ND_ bool   IsZero (const float  x)      { return Abs(x) <= Epsilon(); }
ND_ bool2  IsZero (const float2 v)      { return LessEqual( Abs(v), float2(Epsilon()) ); }
ND_ bool3  IsZero (const float3 v)      { return LessEqual( Abs(v), float3(Epsilon()) ); }
ND_ bool4  IsZero (const float4 v)      { return LessEqual( Abs(v), float4(Epsilon()) ); }

ND_ bool   IsNotZero (const float  x)   { return Abs(x) > Epsilon(); }
ND_ bool2  IsNotZero (const float2 v)   { return Greater( Abs(v), float2(Epsilon()) ); }
ND_ bool3  IsNotZero (const float3 v)   { return Greater( Abs(v), float3(Epsilon()) ); }
ND_ bool4  IsNotZero (const float4 v)   { return Greater( Abs(v), float4(Epsilon()) ); }
//-----------------------------------------------------------------------------
