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
#define Fract           fract
#define Floor           floor
#define FusedMulAdd     fma     // (a * b) + c
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
#define SignOrZero      sign
#define Tan             tan
#define TanH            tanh
#define Trunc           trunc
#define ToUNorm( x )    ((x) * 0.5f + 0.5f)
#define ToSNorm( x )    ((x) * 2.0f - 1.0f)
#define BitCount        bitCount


// to mark 'out' and 'inout' argument in function call
// in function argument list use defined by GLSL qualificators: in, out, inout
#define OUT
#define INOUT

#define Less            lessThan            // <
#define Greater         greaterThan         // >
#define LessEqual       lessThanEqual       // <=
#define GreaterEqual    greaterThanEqual    // >=
#define Not             not
#define not             !

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

#define Min3( a, b, c )         Min( Min( (a), (b) ), (c) )
#define Min4( a, b, c, d )      Min( Min( (a), (b) ), Min( (c), (d) ))
#define Max3( a, b, c )         Max( Max( (a), (b) ), (c) )
#define Max4( a, b, c, d )      Max( Max( (a), (b) ), Max( (c), (d) ))


//-----------------------------------------------------------------------------
// Constants

ND_ float  Epsilon ()               { return 0.00001f; }
ND_ float  Pi ()                    { return 3.14159265358979323846f; }
ND_ float  Pi2 ()                   { return Pi() * 2.0f; }

ND_ float  ReciprocalPi ()          { return 0.31830988618379067153f; }
ND_ float  SqrtOf2 ()               { return 1.41421356237309504880f; }

//-----------------------------------------------------------------------------

ND_ float  Sign (const float x)     { return  x < 0.0f ? -1.0f : 1.0f; }
ND_ int    Sign (const int x)       { return  x < 0 ? -1 : 1; }

ND_ float2  SinCos (const float x)  { return float2(sin(x), cos(x)); }


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
// square length and distance

ND_ float  Length2 (const float2 x)     { return Dot( x, x ); }
ND_ float  Length2 (const float3 x)     { return Dot( x, x ); }

ND_ float  Distance2 (const float2 x, const float2 y)  { float2 r = x - y;  return Dot( r, r ); }
ND_ float  Distance2 (const float3 x, const float3 y)  { float3 r = x - y;  return Dot( r, r ); }


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

ND_ float   BaryLerp (const float  a, const float  b, const float  c, const float3 barycentrics)  { return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z; }
ND_ float2  BaryLerp (const float2 a, const float2 b, const float2 c, const float3 barycentrics)  { return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z; }
ND_ float3  BaryLerp (const float3 a, const float3 b, const float3 c, const float3 barycentrics)  { return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z; }
ND_ float4  BaryLerp (const float4 a, const float4 b, const float4 c, const float3 barycentrics)  { return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z; }


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

// map pixels to unorm coords with correct aspect ratio.
ND_ float2  MapPixCoordsToUNorm (const float2 posPx, const float2 sizePx)   { return (posPx+0.5f) / Max( sizePx.x, sizePx.y ); }
ND_ float3  MapPixCoordsToUNorm (const float3 posPx, const float3 sizePx)   { return (posPx+0.5f) / Max( sizePx.x, sizePx.y ); }


// map pixels to snorm coords with correct aspect ratio.
ND_ float2  MapPixCoordsToSNorm (const float2 posPx, const float2 sizePx)
{
    float2  hsize = sizePx * 0.5f;
    return (Floor(posPx) - hsize) / Max( hsize.x, hsize.y );
}

ND_ float3  MapPixCoordsToSNorm (const float3 posPx, const float3 sizePx)
{
    float3  hsize = sizePx * 0.5f;
    return (Floor(posPx) - hsize) / Max( hsize.x, hsize.y );
}


// map pixels to unorm coords with correct aspect ratio.
ND_ float2  MapPixCoordsToUNorm2 (const float2 srcPosPx, const float2 srcSizePx, const float2 dstSizePx)
{
    const float2    snorm       = ToSNorm( srcPosPx / srcSizePx );
    const float     src_aspect  = srcSizePx.x / srcSizePx.y;
    const float     dst_aspect  = dstSizePx.x / dstSizePx.y;
    const float     scale1      = Max( src_aspect, dst_aspect ) / dst_aspect;
    const float     scale2      = Min( src_aspect, dst_aspect ) / dst_aspect;
    const float2    scale       = src_aspect >= dst_aspect ? float2(scale1, 1.0f) : float2(1.0f, 1.0f/scale2);
    return ToUNorm( snorm * scale );
}


//-----------------------------------------------------------------------------

ND_ bool  IsInsideRect (const int2 pos, const int2 minBound, const int2 maxBound)
{
    return All(bool4( GreaterEqual( pos, minBound ), Less( pos, maxBound )));
}

ND_ bool  IsInsideRect (const float2 pos, const float2 minBound, const float2 maxBound)
{
    return All(bool4( GreaterEqual( pos, minBound ), Less( pos, maxBound )));
}

ND_ bool  IsOutsideRect (const float2 pos, const float2 minBound, const float2 maxBound)
{
    return Any(bool4( Less( pos, minBound ), Greater( pos, maxBound )));
}

ND_ bool  IsInsideCircle (const float2 pos, const float2 center, const float radius)
{
    return Distance( pos, center ) < radius;
}

//-----------------------------------------------------------------------------

ND_ float2 SampleArray_Helper (int len, float f)
{
    float a = Clamp( f * (len - 1), 0.0, float(len-1) );
    float b = Floor( a );
    return float2( b, a - b );
}

#define SampleArray( _array_, _factor_ ) \
    Lerp(   _array_[int(SampleArray_Helper( _array_.length(), _factor_ ).x)], \
            _array_[int(SampleArray_Helper( _array_.length(), _factor_ ).x) + 1], \
            SampleArray_Helper( _array_.length(), _factor_ ).y )

//-----------------------------------------------------------------------------

