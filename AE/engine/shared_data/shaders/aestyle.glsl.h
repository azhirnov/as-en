// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    used only for auto-complete in shader code
*/

#pragma once

#ifndef __cplusplus
# error Only for C++ code!
#endif

#include <type_traits>

#define isinf   _IsInf
#define isnan   _IsNaN

#define highp
#define mediump
#define lowp

#define out
#define inout
#define in
#define layout(...)

#define INOUT
#define OUT


// Math types
#if 1
#include "aestyle_shared.h"

template <typename T>           ND_ T               abs (const T);
template <typename T>           ND_ T               acos (const T);
template <typename T>           ND_ T               acosh (const T);
                                ND_ bool            all (const bool2);
                                ND_ bool            all (const bool3);
                                ND_ bool            all (const bool4);
                                ND_ bool            any (const bool2);
                                ND_ bool            any (const bool3);
                                ND_ bool            any (const bool4);
template <typename T>           ND_ T               asin (const T);
template <typename T>           ND_ T               asinh (const T);
template <typename T>           ND_ T               atan (const T y, const T x);
template <typename T>           ND_ T               atan (const T y_over_x);
template <typename T>           ND_ T               atan (const T);
template <typename T>           ND_ T               bitCount (const T);
template <typename T>           ND_ T               bitfieldExtract (const T value, int offset, int bits);
template <typename T>           ND_ T               bitfieldInsert (const T base, const T insert, int offset, int bits);
template <typename T>           ND_ T               bitfieldReverse (const T);
template <typename T>           ND_ T               ceil (const T);
template <typename T>           ND_ _Scalar<T>      clamp (const T x, const T minVal, const T maxVal);
template <typename T, int I>    ND_ _Vec<T,I>       clamp (const _Vec<T,I> x, const T minVal, const T maxVal);
template <typename T, int I>    ND_ _Vec<T,I>       clamp (const _Vec<T,I> x, const _Vec<T,I> minVal, const _Vec<T,I> maxVal);
template <typename T>           ND_ T               cos (const T);
template <typename T>           ND_ T               cosh (const T);
                                ND_ float3          cross (const float3 x, const float3 y);
                                ND_ double3         cross (const double3 x, const double3 y);
template <typename T>           ND_ T               degrees (const T);
                                ND_ float           distance (const float2, const float2);
                                ND_ float           distance (const float3, const float3);
                                ND_ float           distance (const float4, const float4);
                                ND_ double          distance (const double2, const double2);
                                ND_ double          distance (const double3, const double3);
                                ND_ double          distance (const double4, const double4);
                                ND_ float           dot (const float2, const float2);
                                ND_ float           dot (const float3, const float3);
                                ND_ float           dot (const float4, const float4);
                                ND_ double          dot (const double2, const double2);
                                ND_ double          dot (const double3, const double3);
                                ND_ double          dot (const double4, const double4);
template <typename T, int I>    ND_ _Vec<bool,I>    equal (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T>           ND_ T               exp (const T);
template <typename T>           ND_ T               exp2 (const T);
template <typename T>           ND_ T               faceforward (const T n, const T i, const T Nref);
template <typename T>           ND_ T               findLSB (const T);
template <typename T>           ND_ T               findMSB (const T);
                                ND_ int             floatBitsToInt (const float);           // intBitsToFloat
template <int I>                ND_ _Vec<int,I>     floatBitsToInt (const _Vec<float,I>);   // intBitsToFloat
                                ND_ uint            floatBitsToUint (const float);          // uintBitsToFloat
template <int I>                ND_ _Vec<uint,I>    floatBitsToUint (const _Vec<float,I>);  // uintBitsToFloat
template <typename T>           ND_ T               floor (const T);
template <typename T>           ND_ T               fma (const T a, const T b, const T c);
template <typename T>           ND_ T               fract (const T);
template <typename T>           ND_ T               frexp (const T x, OUT T &exp);
template <typename T, int I>    ND_ _Vec<bool,I>    greaterThan (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T, int I>    ND_ _Vec<bool,I>    greaterThanEqual (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T>           ND_ void            umulExtended (const T x, const T y, OUT T &msb, OUT T &lsb);
template <typename T>           ND_ void            imulExtended (const T x, const T y, OUT T &msb, OUT T &lsb);
                                ND_ float           intBitsToFloat (const int);             // floatBitsToInt
template <int I>                ND_ _Vec<float,I>   intBitsToFloat (const _Vec<int,I>);     // floatBitsToInt
                                ND_ float           uintBitsToFloat (const uint);           // floatBitsToUint
template <int I>                ND_ _Vec<float,I>   uintBitsToFloat (const _Vec<uint,I>);   // floatBitsToUint
template <typename T>           ND_ T               inversesqrt (const T);
template <typename T>           ND_ bool            isinf (const T);
template <typename T, int I>    ND_ _Vec<bool,I>    isinf (const _Vec<T,I>);
template <typename T>           ND_ bool            isinf (const T);
template <typename T, int I>    ND_ _Vec<bool,I>    isinf (const _Vec<T,I>);
template <typename T>           ND_ bool            isnan (const T);
template <typename T, int I>    ND_ _Vec<bool,I>    isnan (const _Vec<T,I>);
template <typename T, typename IT>  ND_ T           ldexp (const T x, const IT exp);
template <int I>                ND_ float           length (const _Vec<float,I>);
template <int I>                ND_ double          length (const _Vec<double,I>);
template <typename T, int I>    ND_ _Vec<bool,I>    lessThan (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T, int I>    ND_ _Vec<bool,I>    lessThanEqual (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T>           ND_ T               log (const T);
template <typename T>           ND_ T               log2 (const T);
template <typename T, int I>    ND_ _Vec<T,I>       max (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T, int I>    ND_ _Vec<T,I>       max (const _Vec<T,I> x, const T y);
template <typename T>           ND_ T               max (const T x, const T y);
template <typename T, int I>    ND_ _Vec<T,I>       min (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T, int I>    ND_ _Vec<T,I>       min (const _Vec<T,I> x, const T y);
template <typename T>           ND_ T               min (const T x, const T y);
template <typename T, int I>    ND_ _Vec<T,I>       mix (const _Vec<T,I> x, const _Vec<T,I> y, const T a);
template <typename T>           ND_ T               mix (const T x, const T y, const T a);
template <typename T, int I>    ND_ _Vec<T,I>       mod (const _Vec<T,I> x, const _Vec<T,I> y);
template <typename T, int I>    ND_ _Vec<T,I>       mod (const _Vec<T,I> x, const T y);
template <typename T>           ND_ T               mod (const T x, const T y);
template <typename T>           ND_ T               modf (const T x, OUT T &i);
template <typename T, int I>    ND_ _Vec<T,I>       normalize (const _Vec<T,I>);
                            //  ND_ _Vec<bool,I>    not (const _Vec<bool,I>);
template <typename T, int I>    ND_ _Vec<bool,I>    notEqual (const _Vec<T,I> x, const _Vec<T,I> y);
                                ND_ double          packDouble2x32 (const uint2);
                                ND_ uint2           unpackDouble2x32 (double);
                                ND_ uint            packHalf2x16 (const float2);
                                ND_ float2          unpackHalf2x16 (const uint);
                                ND_ uint            packUnorm2x16 (const float2);
                                ND_ uint            packSnorm2x16 (const float2);
                                ND_ uint            packUnorm4x8 (const float4);
                                ND_ uint            packSnorm4x8 (const float4);
                                ND_ float2          unpackUnorm2x16 (const uint);
                                ND_ float2          unpackSnorm2x16 (const uint);
                                ND_ float4          unpackUnorm4x8 (const uint);
                                ND_ float4          unpackSnorm4x8 (const uint);
template <typename T>           ND_ T               pow (const T x, const T y);
template <typename T>           ND_ T               radians (const T);
template <typename T>           ND_ T               reflect (const T i, const T n);
template <typename T>           ND_ T               refract (const T i, const T n, float eta);
template <typename T>           ND_ T               round (const T);
template <typename T>           ND_ T               roundEven (const T);
template <typename T>           ND_ T               sign (const T);
template <typename T>           ND_ T               sin (const T);
template <typename T>           ND_ T               sinh (const T);
template <typename T>           ND_ T               smoothstep (const T edge0, const T edge1, const T x);
template <typename T, int I>    ND_ _Vec<T,I>       smoothstep (const T edge0, const T edge1, const _Vec<T,I> x);
template <typename T, int I>    ND_ _Vec<T,I>       smoothstep (const _Vec<T,I> edge0, const _Vec<T,I> edge1, const _Vec<T,I> x);
template <typename T>           ND_ T               sqrt (const T);
template <typename T>           ND_ T               step (const T edge, const T x);
template <typename T, int I>    ND_ _Vec<T,I>       step (const T edge, const _Vec<T,I> x);
template <typename T, int I>    ND_ _Vec<T,I>       step (const _Vec<T,I> edge, const _Vec<T,I> x);
template <typename T>           ND_ T               tan (const T);
template <typename T>           ND_ T               tanh (const T);
template <typename T>           ND_ T               trunc (const T);
template <typename T>           ND_ T               uaddCarry (const T x, const T y, OUT T &carry);
template <typename T>           ND_ T               usubBorrow (const T x, const T y, OUT T barrow);

template <typename T, int C, int R> ND_ T               determinant (const _Matrix<T,C,R>);
template <typename T, int C, int R> ND_ _Matrix<T,C,R>  inverse (const _Matrix<T,C,R>);
template <typename T, int C, int R> ND_ _Matrix<T,C,R>  matrixCompMult (const _Matrix<T,C,R> x, const _Matrix<T,C,R> y);
template <typename T, int C, int R> ND_ _Matrix<T,C,R>  outerProduct (const _Vec<T,R> c, const _Vec<T,C> r);
template <typename T, int C, int R> ND_ _Matrix<T,R,C>  transpose (const _Matrix<T,C,R>);
#endif



struct gl
{
private:
    template <typename T> struct _SubpassBase       {};
    template <typename T> struct _TextureBase       {};
    template <typename T> struct _CombinedTexBase   {};
    template <typename T> struct _ImageBase         {};

    enum class _Uns_ {};

    static constexpr uint   _MaxClipDistance                = 8;
    static constexpr uint   _MaxCullDistance                = 8;
    static constexpr uint   _MaxSampleMask                  = 1;
    static constexpr uint   _MaxMeshVertices                = 1000;
    static constexpr uint   _MaxMeshPrimitives              = 1000;
    static constexpr uint   _MaxPrimitivePointIndices       = 1000;
    static constexpr uint   _MaxPrimitiveLineIndices        = 1000;
    static constexpr uint   _MaxPrimitiveTriangleIndices    = 1000;


public:
    enum class DeviceAddress : uint64_t {};

    template <typename T> struct SubpassInput           : _SubpassBase<T> { SubpassInput ();        };
    template <typename T> struct SubpassInputMS         : _SubpassBase<T> { SubpassInputMS ();      };

    template <typename T> struct Texture1D              : _TextureBase<T> { Texture1D ();           using Size = int;   using Offset = int;     };
    template <typename T> struct Texture2D              : _TextureBase<T> { Texture2D ();           using Size = int2;  using Offset = int2;    };
    template <typename T> struct Texture3D              : _TextureBase<T> { Texture3D ();           using Size = int3;  using Offset = int3;    };
    template <typename T> struct TextureCube            : _TextureBase<T> { TextureCube ();         using Size = int2;  using Offset = _Uns_;   };
    template <typename T> struct Texture1DArray         : _TextureBase<T> { Texture1DArray ();      using Size = int2;  using Offset = int;     };
    template <typename T> struct Texture2DArray         : _TextureBase<T> { Texture2DArray ();      using Size = int3;  using Offset = int2;    };
    template <typename T> struct TextureBuffer          : _TextureBase<T> { TextureBuffer ();       using Size = int;   using Offset = int;     };
    template <typename T> struct Texture2DMS            : _TextureBase<T> { Texture2DMS ();         using Size = int2;  using Offset = int2;    };
    template <typename T> struct Texture2DMSArray       : _TextureBase<T> { Texture2DMSArray ();    using Size = int3;  using Offset = int3;    };
    template <typename T> struct TextureCubeArray       : _TextureBase<T> { TextureCubeArray ();    using Size = int3;  using Offset = _Uns_;   };

    struct Sampler {};
    struct SamplerShadow {};

    template <typename T> struct CombinedTex1D          : _CombinedTexBase<T> { CombinedTex1D ();           CombinedTex1D (Texture1D<T>, Sampler);                  using Size = int;   using Offset = int;     using UNorm = float;    using Grad = float;     };
    template <typename T> struct CombinedTex2D          : _CombinedTexBase<T> { CombinedTex2D ();           CombinedTex2D (Texture2D<T>, Sampler);                  using Size = int2;  using Offset = int2;    using UNorm = float2;   using Grad = float2;    };
    template <typename T> struct CombinedTex3D          : _CombinedTexBase<T> { CombinedTex3D ();           CombinedTex3D (Texture3D<T>, Sampler);                  using Size = int3;  using Offset = int3;    using UNorm = float3;   using Grad = float3;    };
    template <typename T> struct CombinedTexCube        : _CombinedTexBase<T> { CombinedTexCube ();         CombinedTexCube (TextureCube<T>, Sampler);              using Size = int2;  using Offset = _Uns_;   using UNorm = float3;   using Grad = float3;    };
    template <typename T> struct CombinedTex1DArray     : _CombinedTexBase<T> { CombinedTex1DArray ();      CombinedTex1DArray (Texture1DArray<T>, Sampler);        using Size = int2;  using Offset = int;     using UNorm = float2;   using Grad = float;     };
    template <typename T> struct CombinedTex2DArray     : _CombinedTexBase<T> { CombinedTex2DArray ();      CombinedTex2DArray (Texture2DArray<T>, Sampler);        using Size = int3;  using Offset = int2;    using UNorm = float3;   using Grad = float2;    };
    template <typename T> struct CombinedTexBuffer      : _CombinedTexBase<T> { CombinedTexBuffer ();       CombinedTexBuffer (TextureBuffer<T>, Sampler);          using Size = int;   using Offset = int;     using UNorm = float;    using Grad = _Uns_;     };
    template <typename T> struct CombinedTex2DMS        : _CombinedTexBase<T> { CombinedTex2DMS ();         CombinedTex2DMS (Texture2DMS<T>, Sampler);              using Size = int2;  using Offset = int2;    using UNorm = _Uns_;    using Grad = _Uns_;     };
    template <typename T> struct CombinedTex2DMSArray   : _CombinedTexBase<T> { CombinedTex2DMSArray ();    CombinedTex2DMSArray (Texture2DMSArray<T>, Sampler);    using Size = int3;  using Offset = int3;    using UNorm = _Uns_;    using Grad = _Uns_;     };
    template <typename T> struct CombinedTexCubeArray   : _CombinedTexBase<T> { CombinedTexCubeArray ();    CombinedTexCubeArray (TextureCubeArray<T>, Sampler);    using Size = int3;  using Offset = _Uns_;   using UNorm = float4;   using Grad = float3;    };

    template <typename T> struct Image1D                : _ImageBase<T> { Image1D ();           using Coord = int;  };
    template <typename T> struct Image2D                : _ImageBase<T> { Image2D ();           using Coord = int2; };
    template <typename T> struct Image3D                : _ImageBase<T> { Image3D ();           using Coord = int3; };
    template <typename T> struct ImageCube              : _ImageBase<T> { ImageCube ();         using Coord = int3; };
    template <typename T> struct ImageBuffer            : _ImageBase<T> { ImageBuffer ();       using Coord = int;  };
    template <typename T> struct Image1DArray           : _ImageBase<T> { Image1DArray ();      using Coord = int2; };
    template <typename T> struct Image2DArray           : _ImageBase<T> { Image2DArray ();      using Coord = int3; };
    template <typename T> struct ImageCubeArray         : _ImageBase<T> { ImageCubeArray ();    using Coord = int3; };
    template <typename T> struct Image2DMS              : _ImageBase<T> { Image2DMS ();         using Coord = int2; };
    template <typename T> struct Image2DMSArray         : _ImageBase<T> { Image2DMSArray ();    using Coord = int3; };


    // Use it if array index is vary within a wave.
    // PrimitiveID, InstanceID, DrawID, ...

    ND_ int   Nonuniform (int);         // GL_EXT_nonuniform_qualifier
    ND_ uint  Nonuniform (uint);        // GL_EXT_nonuniform_qualifier

    template <typename T>
    ND_ T   SubgroupUniform (const T &);// GL_EXT_subgroupuniform_qualifier


  #ifdef AE_MEM_SCOPE
    enum class Scope
    {
        Device,             // gl_ScopeDevice
        Workgroup,          // gl_ScopeWorkgroup
        Subgroup,           // gl_ScopeSubgroup
        Invocation,         // gl_ScopeInvocation
        QueueFamily,        // gl_ScopeQueueFamily
    };

    enum class Semantics
    {
        Relaxed,            // gl_SemanticsRelaxed
        Acquire,            // gl_SemanticsAcquire
        Release,            // gl_SemanticsRelease
        AcquireRelease,     // gl_SemanticsAcquireRelease
        MakeAvailable,      // gl_SemanticsMakeAvailable
        MakeVisible,        // gl_SemanticsMakeVisible
        Volatile,           // gl_SemanticsVolatile
    };

    enum class StorageSemantics
    {
        None,               // gl_StorageSemanticsNone
        Buffer,             // gl_StorageSemanticsBuffer
        Shared,             // gl_StorageSemanticsShared
        Image,              // gl_StorageSemanticsImage
        Output,             // gl_StorageSemanticsOutput
    };
  #endif

  #ifdef AE_HAS_ATOMICS
    uint   AtomicAdd (INOUT uint  &mem, uint  data);
    sint   AtomicAdd (INOUT sint  &mem, sint  data);
    ulong  AtomicAdd (INOUT ulong &mem, ulong data);
    slong  AtomicAdd (INOUT slong &mem, slong data);

    #ifdef AE_MEM_SCOPE
    uint   AtomicAdd (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
    sint   AtomicAdd (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
    ulong  AtomicAdd (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
    slong  AtomicAdd (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
    #endif

    float  AtomicAdd (INOUT float  &mem, float  data);
    double AtomicAdd (INOUT double &mem, double data);

    #ifdef AE_MEM_SCOPE
    float  AtomicAdd (INOUT float  &mem, float  data, Scope scope, StorageSemantics storage, Semantics sem);
    double AtomicAdd (INOUT double &mem, double data, Scope scope, StorageSemantics storage, Semantics sem);
    #endif

    uint   AtomicMin (INOUT uint  &mem, uint  data);
    sint   AtomicMin (INOUT sint  &mem, sint  data);
    ulong  AtomicMin (INOUT ulong &mem, ulong data);
    slong  AtomicMin (INOUT slong &mem, slong data);

    #ifdef AE_MEM_SCOPE
    uint   AtomicMin (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
    sint   AtomicMin (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
    ulong  AtomicMin (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
    slong  AtomicMin (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
    #endif

    uint   AtomicMax (INOUT uint  &mem, uint  data);
    sint   AtomicMax (INOUT sint  &mem, sint  data);
    ulong  AtomicMax (INOUT ulong &mem, ulong data);
    slong  AtomicMax (INOUT slong &mem, slong data);

    #ifdef AE_MEM_SCOPE
    uint   AtomicMax (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
    sint   AtomicMax (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
    ulong  AtomicMax (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
    slong  AtomicMax (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
    #endif

    uint   AtomicAnd (INOUT uint  &mem, uint  data);
    sint   AtomicAnd (INOUT sint  &mem, sint  data);
    ulong  AtomicAnd (INOUT ulong &mem, ulong data);
    slong  AtomicAnd (INOUT slong &mem, slong data);

    #ifdef AE_MEM_SCOPE
    uint   AtomicAnd (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
    sint   AtomicAnd (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
    ulong  AtomicAnd (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
    slong  AtomicAnd (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
    #endif

    uint   AtomicOr (INOUT uint  &mem, uint  data);
    sint   AtomicOr (INOUT sint  &mem, sint  data);
    ulong  AtomicOr (INOUT ulong &mem, ulong data);
    slong  AtomicOr (INOUT slong &mem, slong data);

    #ifdef AE_MEM_SCOPE
    uint   AtomicOr (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
    sint   AtomicOr (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
    ulong  AtomicOr (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
    slong  AtomicOr (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
    #endif

    uint   AtomicXor (INOUT uint  &mem, uint  data);
    sint   AtomicXor (INOUT sint  &mem, sint  data);
    ulong  AtomicXor (INOUT ulong &mem, ulong data);
    slong  AtomicXor (INOUT slong &mem, slong data);

    #ifdef AE_MEM_SCOPE
    uint   AtomicXor (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
    sint   AtomicXor (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
    ulong  AtomicXor (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
    slong  AtomicXor (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
    #endif

    uint   AtomicExchange (INOUT uint  &mem, uint  data);
    sint   AtomicExchange (INOUT sint  &mem, sint  data);
    ulong  AtomicExchange (INOUT ulong &mem, ulong data);
    slong  AtomicExchange (INOUT slong &mem, slong data);

    #ifdef AE_MEM_SCOPE
    uint   AtomicExchange (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
    sint   AtomicExchange (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
    ulong  AtomicExchange (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
    slong  AtomicExchange (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
    #endif

    float  AtomicExchange (INOUT float  &mem, float  data);
    double AtomicExchange (INOUT double &mem, double data);

    #ifdef AE_MEM_SCOPE
    float  AtomicExchange (INOUT float  &mem, float  data, Scope scope, StorageSemantics storage, Semantics sem);
    double AtomicExchange (INOUT double &mem, double data, Scope scope, StorageSemantics storage, Semantics sem);
    #endif

    uint   AtomicCompSwap (INOUT uint  &mem, uint  compare, uint  data);
    sint   AtomicCompSwap (INOUT sint  &mem, sint  compare, sint  data);
    ulong  AtomicCompSwap (INOUT ulong &mem, ulong compare, ulong data);
    slong  AtomicCompSwap (INOUT slong &mem, slong compare, slong data);

    #ifdef AE_MEM_SCOPE
    uint   AtomicCompSwap (INOUT uint  &mem, uint  compare, uint  data, Scope scope, StorageSemantics storageEqual, Semantics semEqual, StorageSemantics storageUnequal, Semantics semUnequal);
    sint   AtomicCompSwap (INOUT sint  &mem, sint  compare, sint  data, Scope scope, StorageSemantics storageEqual, Semantics semEqual, StorageSemantics storageUnequal, Semantics semUnequal);
    ulong  AtomicCompSwap (INOUT ulong &mem, ulong compare, ulong data, Scope scope, StorageSemantics storageEqual, Semantics semEqual, StorageSemantics storageUnequal, Semantics semUnequal);
    slong  AtomicCompSwap (INOUT slong &mem, slong compare, slong data, Scope scope, StorageSemantics storageEqual, Semantics semEqual, StorageSemantics storageUnequal, Semantics semUnequal);
    #endif

    ND_ uint   AtomicLoad (uint  &mem);
    ND_ sint   AtomicLoad (sint  &mem);
    ND_ ulong  AtomicLoad (ulong &mem);
    ND_ slong  AtomicLoad (slong &mem);

    #ifdef AE_MEM_SCOPE
    ND_ uint   AtomicLoad (uint  &mem, Scope scope, StorageSemantics storage, Semantics sem);
    ND_ sint   AtomicLoad (sint  &mem, Scope scope, StorageSemantics storage, Semantics sem);
    ND_ ulong  AtomicLoad (ulong &mem, Scope scope, StorageSemantics storage, Semantics sem);
    ND_ slong  AtomicLoad (slong &mem, Scope scope, StorageSemantics storage, Semantics sem);
    #endif

    ND_ float  AtomicLoad (float  &mem);
    ND_ double AtomicLoad (double &mem);

    #ifdef AE_MEM_SCOPE
    ND_ float  AtomicLoad (float  &mem, Scope scope, StorageSemantics storage, Semantics sem);
    ND_ double AtomicLoad (double &mem, Scope scope, StorageSemantics storage, Semantics sem);
    #endif

    void  AtomicStore (INOUT uint  &mem, uint  data);
    void  AtomicStore (INOUT sint  &mem, sint  data);
    void  AtomicStore (INOUT ulong &mem, ulong data);
    void  AtomicStore (INOUT slong &mem, slong data);

    #ifdef AE_MEM_SCOPE
    void  AtomicStore (INOUT uint  &mem, uint  data, Scope scope, StorageSemantics storage, Semantics sem);
    void  AtomicStore (INOUT sint  &mem, sint  data, Scope scope, StorageSemantics storage, Semantics sem);
    void  AtomicStore (INOUT ulong &mem, ulong data, Scope scope, StorageSemantics storage, Semantics sem);
    void  AtomicStore (INOUT slong &mem, slong data, Scope scope, StorageSemantics storage, Semantics sem);
    #endif

    void  AtomicStore (INOUT float  &mem, float  data);
    void  AtomicStore (INOUT double &mem, double data);

    #ifdef AE_MEM_SCOPE
    void  AtomicStore (INOUT float  &mem, float  data, Scope scope, StorageSemantics storage, Semantics sem);
    void  AtomicStore (INOUT double &mem, double data, Scope scope, StorageSemantics storage, Semantics sem);
    #endif
  #endif // AE_HAS_ATOMICS


    const struct {
        #define _GEN_IMAGE_ATOMIC( _type_, ... ) \
            _type_   AtomicAdd (__VA_ARGS__, _type_ data) const; \
            _type_   AtomicAdd (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
            \
            _type_   AtomicMin (__VA_ARGS__, _type_ data) const; \
            _type_   AtomicMin (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
            \
            _type_   AtomicMax (__VA_ARGS__, _type_ data) const; \
            _type_   AtomicMax (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
            \
            _type_   AtomicAnd (__VA_ARGS__, _type_ data) const; \
            _type_   AtomicAnd (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
            \
            _type_   AtomicOr (__VA_ARGS__, _type_ data) const; \
            _type_   AtomicOr (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
            \
            _type_   AtomicXor (__VA_ARGS__, _type_ data) const; \
            _type_   AtomicXor (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
            \
            _type_   AtomicExchange (__VA_ARGS__, _type_ data) const; \
            _type_   AtomicExchange (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
            \
            _type_   AtomicCompSwap (__VA_ARGS__, _type_ compare, _type_ data) const; \
            _type_   AtomicCompSwap (__VA_ARGS__, _type_ compare, _type_ data, Scope scope, StorageSemantics storageEqual, Semantics semEqual, StorageSemantics storageUnequal, Semantics semUnequal) const; \
            \
            ND_ _type_  AtomicLoad (__VA_ARGS__, Scope scope, StorageSemantics storage, Semantics sem) const; \
            \
            void  AtomicStore (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \

        // GL_EXT_shader_atomic_float
        #define _GEN_IMAGE_ATOMIC_F1( _type_, ... ) \
            _type_   AtomicAdd (__VA_ARGS__, _type_ data) const; \
            _type_   AtomicAdd (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
            \
            _type_   AtomicExchange (__VA_ARGS__, _type_ data) const; \
            _type_   AtomicExchange (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
            \
            ND_ _type_  AtomicLoad (__VA_ARGS__, Scope scope, StorageSemantics storage, Semantics sem) const; \
            \
            void  AtomicStore (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \

        // GL_EXT_shader_atomic_float2
        #define _GEN_IMAGE_ATOMIC_F2( _type_, ... ) \
            _type_   AtomicMin (__VA_ARGS__, _type_ data) const; \
            _type_   AtomicMin (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \
            \
            _type_   AtomicMax (__VA_ARGS__, _type_ data) const; \
            _type_   AtomicMax (__VA_ARGS__, _type_ data, Scope scope, StorageSemantics storage, Semantics sem) const; \

        #define _GEN_IMAGE_ATOMIC_Fx( _type_, ... ) \
            _GEN_IMAGE_ATOMIC_F1( _type_, __VA_ARGS__ )\
            _GEN_IMAGE_ATOMIC_F2( _type_, __VA_ARGS__ )\

        #define _GEN_IMAGE( _type_, ... ) \
            ND_ _type_  Load (__VA_ARGS__) const; \
            \
            void  Store (__VA_ARGS__, _type_ data) const; \

        #define GEN_IMAGE_ATOMIC( _type_ ) \
            _GEN_IMAGE_ATOMIC( _type_, Image1D<_type_>        img, typename Image1D<_type_>::Coord        p )\
            _GEN_IMAGE_ATOMIC( _type_, Image2D<_type_>        img, typename Image2D<_type_>::Coord        p )\
            _GEN_IMAGE_ATOMIC( _type_, Image3D<_type_>        img, typename Image3D<_type_>::Coord        p )\
            _GEN_IMAGE_ATOMIC( _type_, ImageCube<_type_>      img, typename ImageCube<_type_>::Coord      p )\
            _GEN_IMAGE_ATOMIC( _type_, ImageBuffer<_type_>    img, typename ImageBuffer<_type_>::Coord    p )\
            _GEN_IMAGE_ATOMIC( _type_, Image1DArray<_type_>   img, typename Image1DArray<_type_>::Coord   p )\
            _GEN_IMAGE_ATOMIC( _type_, Image2DArray<_type_>   img, typename Image2DArray<_type_>::Coord   p )\
            _GEN_IMAGE_ATOMIC( _type_, ImageCubeArray<_type_> img, typename ImageCubeArray<_type_>::Coord p )\
            _GEN_IMAGE_ATOMIC( _type_, Image2DMS<_type_>      img, typename Image2DMS<_type_>::Coord      p, int sample )\
            _GEN_IMAGE_ATOMIC( _type_, Image2DMSArray<_type_> img, typename Image2DMSArray<_type_>::Coord p, int sample )\

        #define GEN_IMAGE_ATOMICF( _type_ ) \
            _GEN_IMAGE_ATOMIC_Fx( _type_, Image1D<_type_>        img, typename Image1D<_type_>::Coord        p )\
            _GEN_IMAGE_ATOMIC_Fx( _type_, Image2D<_type_>        img, typename Image2D<_type_>::Coord        p )\
            _GEN_IMAGE_ATOMIC_Fx( _type_, Image3D<_type_>        img, typename Image3D<_type_>::Coord        p )\
            _GEN_IMAGE_ATOMIC_Fx( _type_, ImageCube<_type_>      img, typename ImageCube<_type_>::Coord      p )\
            _GEN_IMAGE_ATOMIC_Fx( _type_, ImageBuffer<_type_>    img, typename ImageBuffer<_type_>::Coord    p )\
            _GEN_IMAGE_ATOMIC_Fx( _type_, Image1DArray<_type_>   img, typename Image1DArray<_type_>::Coord   p )\
            _GEN_IMAGE_ATOMIC_Fx( _type_, Image2DArray<_type_>   img, typename Image2DArray<_type_>::Coord   p )\
            _GEN_IMAGE_ATOMIC_Fx( _type_, ImageCubeArray<_type_> img, typename ImageCubeArray<_type_>::Coord p )\
            _GEN_IMAGE_ATOMIC_Fx( _type_, Image2DMS<_type_>      img, typename Image2DMS<_type_>::Coord      p, int sample )\
            _GEN_IMAGE_ATOMIC_Fx( _type_, Image2DMSArray<_type_> img, typename Image2DMSArray<_type_>::Coord p, int sample )\

        #define GEN_IMAGE( _type4_, _type_ ) \
            _GEN_IMAGE( _type4_, Image1D<_type_>        img, typename Image1D<_type_>::Coord        p )\
            _GEN_IMAGE( _type4_, Image2D<_type_>        img, typename Image2D<_type_>::Coord        p )\
            _GEN_IMAGE( _type4_, Image3D<_type_>        img, typename Image3D<_type_>::Coord        p )\
            _GEN_IMAGE( _type4_, ImageCube<_type_>      img, typename ImageCube<_type_>::Coord      p )\
            _GEN_IMAGE( _type4_, ImageBuffer<_type_>    img, typename ImageBuffer<_type_>::Coord    p )\
            _GEN_IMAGE( _type4_, Image1DArray<_type_>   img, typename Image1DArray<_type_>::Coord   p )\
            _GEN_IMAGE( _type4_, Image2DArray<_type_>   img, typename Image2DArray<_type_>::Coord   p )\
            _GEN_IMAGE( _type4_, ImageCubeArray<_type_> img, typename ImageCubeArray<_type_>::Coord p )\
            _GEN_IMAGE( _type4_, Image2DMS<_type_>      img, typename Image2DMS<_type_>::Coord      p, int sample )\
            _GEN_IMAGE( _type4_, Image2DMSArray<_type_> img, typename Image2DMSArray<_type_>::Coord p, int sample )\

      #ifdef AE_HAS_ATOMICS
        GEN_IMAGE_ATOMIC(  uint )
        GEN_IMAGE_ATOMIC(  sint )
        GEN_IMAGE_ATOMICF( float )
      #endif
        GEN_IMAGE( uint4,  uint )
        GEN_IMAGE( int4,   sint )
        GEN_IMAGE( float4, float )

        #undef _GEN_IMAGE_ATOMIC
        #undef _GEN_IMAGE_ATOMIC_F1
        #undef _GEN_IMAGE_ATOMIC_F2
        #undef _GEN_IMAGE_ATOMIC_Fx
        #undef _GEN_IMAGE
        #undef GEN_IMAGE_ATOMIC
        #undef GEN_IMAGE_ATOMICF
        #undef GEN_IMAGE

        template <typename T>   ND_ int  GetSamples (Image2DMS<T>      img) const;
        template <typename T>   ND_ int  GetSamples (Image2DMSArray<T> img) const;

        template <typename T>   ND_ typename Image1D<T>::Coord          GetSize (Image1D<T>        img) const;
        template <typename T>   ND_ typename Image2D<T>::Coord          GetSize (Image2D<T>        img) const;
        template <typename T>   ND_ typename Image3D<T>::Coord          GetSize (Image3D<T>        img) const;
        template <typename T>   ND_ typename ImageCube<T>::Coord        GetSize (ImageCube<T>      img) const;
        template <typename T>   ND_ typename ImageBuffer<T>::Coord      GetSize (ImageBuffer<T>    img) const;
        template <typename T>   ND_ typename Image1DArray<T>::Coord     GetSize (Image1DArray<T>   img) const;
        template <typename T>   ND_ typename Image2DArray<T>::Coord     GetSize (Image2DArray<T>   img) const;
        template <typename T>   ND_ typename ImageCubeArray<T>::Coord   GetSize (ImageCubeArray<T> img) const;
        template <typename T>   ND_ typename Image2DMS<T>::Coord        GetSize (Image2DMS<T>      img) const;
        template <typename T>   ND_ typename Image2DMSArray<T>::Coord   GetSize (Image2DMSArray<T> img) const;

    } image {};

    const struct {
        // 1D, 2D, 3D 1DArr, 2DArr, Cube, CubeArr
        #define _GEN_TEXTURE1( _type_, _texType_ ) \
            ND_ _type_  Fetch (_texType_ tex, typename _texType_::Size p, int lod) const; \
            \
            ND_ typename _texType_::Size    GetSize (_texType_ tex, int lod) const; \
            ND_ int                         QueryLevels (_texType_ tex) const; \

        // 1D, 2D, 3D 1DArr, 2DArr
        #define _GEN_TEXTURE2( _type_, _texType_ ) \
            ND_ _type_  FetchOffset (_texType_ tex, typename _texType_::Size p, int lod, typename _texType_::Offset offset) const; \

        // 1D, 2D, 3D 1DArr, 2DArr, Cube, CubeArr
        #define _GEN_TEXTURE3( _type_, _texType_ ) \
            ND_ _type_  Sample (_texType_ tex, typename _texType_::UNorm p) const; \
            ND_ _type_  Sample (_texType_ tex, typename _texType_::UNorm p, float bias) const; \
            \
            ND_ _type_  SampleLod (_texType_ tex, typename _texType_::UNorm p, float lod) const; \
            \
            ND_ _type_  SampleGrad (_texType_ tex, typename _texType_::UNorm p, typename _texType_::Grad dPdx, typename _texType_::Grad dPdy) const; \

        // 1D, 2D, 3D 1DArr, 2DArr
        #define _GEN_TEXTURE4( _type_, _texType_ ) \
            ND_ _type_  SampleOffset (_texType_ tex, typename _texType_::UNorm p, typename _texType_::Offset offset) const; \
            ND_ _type_  SampleOffset (_texType_ tex, typename _texType_::UNorm p, typename _texType_::Offset offset, float bias) const; \
            \
            ND_ _type_  SampleLodOffset (_texType_ tex, typename _texType_::UNorm p, float lod, typename _texType_::Offset offset) const; \
            \
            ND_ _type_  SampleGradOffset (_texType_ tex, typename _texType_::UNorm p, typename _texType_::Grad dPdx, typename _texType_::Grad dPdy, typename _texType_::Offset offset) const; \

        // 2D, 2DArr, Cube, CubeArr
        #define _GEN_TEXTURE5( _type_, _texType_ ) \
            ND_ _type_  Gather (_texType_ tex, typename _texType_::UNorm p) const; \
            ND_ _type_  Gather (_texType_ tex, typename _texType_::UNorm p, int comp) const; \

        // 2D, 2DArr
        #define _GEN_TEXTURE6( _type_, _texType_ ) \
            ND_ _type_  GatherOffset (_texType_ tex, typename _texType_::UNorm p, int2 offset) const; \
            ND_ _type_  GatherOffset (_texType_ tex, typename _texType_::UNorm p, int2 offset, int comp) const; \
            \
            ND_ _type_  GatherOffsets (_texType_ tex, typename _texType_::UNorm p, const int2 (&offsets)[4]) const; \
            ND_ _type_  GatherOffsets (_texType_ tex, typename _texType_::UNorm p, const int2 (&offsets)[4], int comp) const; \

        // 1D, 2D, 3D
        #define _GEN_TEXTURE7( _type_, _texType_ ) \
            ND_ float2  QueryLod (_texType_ tex, typename _texType_::UNorm p) const; \
            \
            ND_ _type_  SampleProj (_texType_ tex, float4 p) const; \
            ND_ _type_  SampleProj (_texType_ tex, float4 p, float bias) const; \
            ND_ _type_  SampleProjGrad (_texType_ tex, float4 p, typename _texType_::Grad dPdx, typename _texType_::Grad dPdy) const; \
            ND_ _type_  SampleProjGradOffset (_texType_ tex, float4 p, typename _texType_::Grad dPdx, typename _texType_::Grad dPdy, typename _texType_::Offset offset) const; \
            ND_ _type_  SampleProjLod (_texType_ tex, float4 p, float lod) const; \
            ND_ _type_  SampleProjLodOffset (_texType_ tex, float4 p, float lod, typename _texType_::Offset offset) const; \
            ND_ _type_  SampleProjOffset (_texType_ tex, float4 p, typename _texType_::Offset offset) const; \
            ND_ _type_  SampleProjOffset (_texType_ tex, float4 p, typename _texType_::Offset offset, float bias) const; \

        #define GEN_TEXTURE( _type4_, _type_ ) \
            _GEN_TEXTURE1( _type4_, CombinedTex1D<_type_>        )\
            _GEN_TEXTURE1( _type4_, CombinedTex2D<_type_>        )\
            _GEN_TEXTURE1( _type4_, CombinedTex3D<_type_>        )\
            _GEN_TEXTURE1( _type4_, CombinedTexCube<_type_>      )\
            _GEN_TEXTURE1( _type4_, CombinedTex1DArray<_type_>   )\
            _GEN_TEXTURE1( _type4_, CombinedTex2DArray<_type_>   )\
            _GEN_TEXTURE1( _type4_, CombinedTexCubeArray<_type_> )\
            \
            _GEN_TEXTURE2( _type4_, CombinedTex1D<_type_>        )\
            _GEN_TEXTURE2( _type4_, CombinedTex2D<_type_>        )\
            _GEN_TEXTURE2( _type4_, CombinedTex3D<_type_>        )\
            _GEN_TEXTURE2( _type4_, CombinedTex1DArray<_type_>   )\
            _GEN_TEXTURE2( _type4_, CombinedTex2DArray<_type_>   )\
            \
            _GEN_TEXTURE3( _type4_, CombinedTex1D<_type_>        )\
            _GEN_TEXTURE3( _type4_, CombinedTex2D<_type_>        )\
            _GEN_TEXTURE3( _type4_, CombinedTex3D<_type_>        )\
            _GEN_TEXTURE3( _type4_, CombinedTexCube<_type_>      )\
            _GEN_TEXTURE3( _type4_, CombinedTex1DArray<_type_>   )\
            _GEN_TEXTURE3( _type4_, CombinedTex2DArray<_type_>   )\
            _GEN_TEXTURE3( _type4_, CombinedTexCubeArray<_type_> )\
            \
            _GEN_TEXTURE4( _type4_, CombinedTex1D<_type_>        )\
            _GEN_TEXTURE4( _type4_, CombinedTex2D<_type_>        )\
            _GEN_TEXTURE4( _type4_, CombinedTex3D<_type_>        )\
            _GEN_TEXTURE4( _type4_, CombinedTex1DArray<_type_>   )\
            _GEN_TEXTURE4( _type4_, CombinedTex2DArray<_type_>   )\
            \
            _GEN_TEXTURE5( _type4_, CombinedTex2D<_type_>        )\
            _GEN_TEXTURE5( _type4_, CombinedTex2DArray<_type_>   )\
            _GEN_TEXTURE5( _type4_, CombinedTexCube<_type_>      )\
            _GEN_TEXTURE5( _type4_, CombinedTexCubeArray<_type_> )\
            \
            _GEN_TEXTURE6( _type4_, CombinedTex2D<_type_>        )\
            _GEN_TEXTURE6( _type4_, CombinedTex2DArray<_type_>   )\
            \
            _GEN_TEXTURE7( _type4_, CombinedTex1D<_type_>        )\
            _GEN_TEXTURE7( _type4_, CombinedTex2D<_type_>        )\
            _GEN_TEXTURE7( _type4_, CombinedTex3D<_type_>        )\
            \
            _GEN_TEXTURE1( _type4_, Texture1D<_type_>        )\
            _GEN_TEXTURE1( _type4_, Texture2D<_type_>        )\
            _GEN_TEXTURE1( _type4_, Texture3D<_type_>        )\
            _GEN_TEXTURE1( _type4_, TextureCube<_type_>      )\
            _GEN_TEXTURE1( _type4_, Texture1DArray<_type_>   )\
            _GEN_TEXTURE1( _type4_, Texture2DArray<_type_>   )\
            _GEN_TEXTURE1( _type4_, TextureCubeArray<_type_> )\
            \
            _GEN_TEXTURE2( _type4_, Texture1D<_type_>        )\
            _GEN_TEXTURE2( _type4_, Texture2D<_type_>        )\
            _GEN_TEXTURE2( _type4_, Texture3D<_type_>        )\
            _GEN_TEXTURE2( _type4_, Texture1DArray<_type_>   )\
            _GEN_TEXTURE2( _type4_, Texture2DArray<_type_>   )\

        GEN_TEXTURE( uint4,  uint );
        GEN_TEXTURE( int4,   sint );
        GEN_TEXTURE( float4, float );

        #undef GEN_TEXTURE
        #undef _GEN_TEXTURE7
        #undef _GEN_TEXTURE6
        #undef _GEN_TEXTURE5
        #undef _GEN_TEXTURE4
        #undef _GEN_TEXTURE3
        #undef _GEN_TEXTURE2
        #undef _GEN_TEXTURE1

        template <typename T>   ND_ int  GetSamples (CombinedTex2DMS<T>      tex) const;
        template <typename T>   ND_ int  GetSamples (CombinedTex2DMSArray<T> tex) const;

        template <typename T>   ND_ int  GetSamples (Texture2DMS<T>      tex) const;
        template <typename T>   ND_ int  GetSamples (Texture2DMSArray<T> tex) const;

        template <typename T>   ND_ typename CombinedTexBuffer<T>::Size     GetSize (CombinedTexBuffer<T>    tex) const;
        template <typename T>   ND_ typename CombinedTex2DMS<T>::Size       GetSize (CombinedTex2DMS<T>      tex) const;
        template <typename T>   ND_ typename CombinedTex2DMSArray<T>::Size  GetSize (CombinedTex2DMSArray<T> tex) const;

        template <typename T>   ND_ typename TextureBuffer<T>::Size     GetSize (TextureBuffer<T>    tex) const;
        template <typename T>   ND_ typename Texture2DMS<T>::Size       GetSize (Texture2DMS<T>      tex) const;
        template <typename T>   ND_ typename Texture2DMSArray<T>::Size  GetSize (Texture2DMSArray<T> tex) const;

        template <typename T>   ND_ _Vec<T,4>  Fetch (CombinedTexBuffer<T>    tex, typename CombinedTexBuffer<T>::Offset    p) const;
        template <typename T>   ND_ _Vec<T,4>  Fetch (CombinedTex2DMS<T>      tex, typename CombinedTex2DMS<T>::Offset      p, int samples) const;
        template <typename T>   ND_ _Vec<T,4>  Fetch (CombinedTex2DMSArray<T> tex, typename CombinedTex2DMSArray<T>::Offset p, int samples) const;

        template <typename T>   ND_ _Vec<T,4>  Fetch (TextureBuffer<T>    tex, typename TextureBuffer<T>::Offset    p) const;
        template <typename T>   ND_ _Vec<T,4>  Fetch (Texture2DMS<T>      tex, typename Texture2DMS<T>::Offset      p, int samples) const;
        template <typename T>   ND_ _Vec<T,4>  Fetch (Texture2DMSArray<T> tex, typename Texture2DMSArray<T>::Offset p, int samples) const;

        template <typename T>   ND_ float2  QueryLod (TextureCube<T>      tex, float3 p) const;
        template <typename T>   ND_ float2  QueryLod (TextureCubeArray<T> tex, float3 p) const;
        template <typename T>   ND_ float2  QueryLod (Texture1DArray<T>   tex, float  p) const;
        template <typename T>   ND_ float2  QueryLod (Texture2DArray<T>   tex, float2 p) const;

        template <typename T>   ND_ float2  QueryLod (CombinedTexCube<T>      tex, float3 p) const;
        template <typename T>   ND_ float2  QueryLod (CombinedTexCubeArray<T> tex, float3 p) const;
        template <typename T>   ND_ float2  QueryLod (CombinedTex1DArray<T>   tex, float  p) const;
        template <typename T>   ND_ float2  QueryLod (CombinedTex2DArray<T>   tex, float2 p) const;

    } texture {};

    const struct {
        template <typename T>   _Vec<T,4>  Load (SubpassInput<T>   sp) const;
        template <typename T>   _Vec<T,4>  Load (SubpassInputMS<T> sp, int sample) const;
    } subpass {};

    const struct {
        // sync
        void    ExecutionBarrier () const;
        const struct {
            void  All ()        const;  // all memory accesses, scope: shader invocation
            void  Buffer ()     const;
            void  Image ()      const;

          #if defined(SH_COMPUTE) or defined(SH_MESH_TASK) or defined(SH_MESH)
            void  Shared ()     const;  // for shared variables
          #endif
        } memoryBarrier {};

        ND_ bool    Elect () const;
        ND_ bool    All (bool) const;
        ND_ bool    Any (bool) const;

        template <typename T>   ND_ bool    AllEqual (const T) const;
        template <typename T>   ND_ T       Broadcast (const T value, uint id) const;
        template <typename T>   ND_ T       BroadcastFirst (const T) const;

        ND_ uint4   Ballot (bool) const;
        ND_ bool    InverseBallot (const uint4) const;
        ND_ bool    BallotBitExtract (const uint4 value, uint index) const;
        ND_ uint    BallotBitCount (const uint4) const;
        ND_ uint    BallotInclusiveBitCount (const uint4) const;
        ND_ uint    BallotExclusiveBitCount (const uint4) const;
        ND_ uint    BallotFindLSB (const uint4) const;
        ND_ uint    BallotFindMSB (const uint4) const;

        template <typename T>   ND_ T       Shuffle (const T value, uint id) const;
        template <typename T>   ND_ T       ShuffleXor (const T value, uint mask) const;
        template <typename T>   ND_ T       ShuffleUp (const T value, uint delta) const;
        template <typename T>   ND_ T       ShuffleDown (const T value, uint delta) const;

        template <typename T>   ND_ T       Add (const T) const;
        template <typename T>   ND_ T       Mul (const T) const;
        template <typename T>   ND_ T       Min (const T) const;
        template <typename T>   ND_ T       Max (const T) const;
        template <typename T>   ND_ T       And (const T) const;
        template <typename T>   ND_ T       Or  (const T) const;
        template <typename T>   ND_ T       Xor (const T) const;

        template <typename T>   ND_ T       InclusiveAdd (const T) const;
        template <typename T>   ND_ T       InclusiveMul (const T) const;
        template <typename T>   ND_ T       InclusiveMin (const T) const;
        template <typename T>   ND_ T       InclusiveMax (const T) const;
        template <typename T>   ND_ T       InclusiveAnd (const T) const;
        template <typename T>   ND_ T       InclusiveOr  (const T) const;
        template <typename T>   ND_ T       InclusiveXor (const T) const;

        template <typename T>   ND_ T       ExclusiveAdd (const T) const;
        template <typename T>   ND_ T       ExclusiveMul (const T) const;
        template <typename T>   ND_ T       ExclusiveMin (const T) const;
        template <typename T>   ND_ T       ExclusiveMax (const T) const;
        template <typename T>   ND_ T       ExclusiveAnd (const T) const;
        template <typename T>   ND_ T       ExclusiveOr  (const T) const;
        template <typename T>   ND_ T       ExclusiveXor (const T) const;

        template <typename T>   ND_ T       ClusteredAdd (const T value, uint clasterSize) const;
        template <typename T>   ND_ T       ClusteredMul (const T value, uint clasterSize) const;
        template <typename T>   ND_ T       ClusteredMin (const T value, uint clasterSize) const;
        template <typename T>   ND_ T       ClusteredMax (const T value, uint clasterSize) const;
        template <typename T>   ND_ T       ClusteredAnd (const T value, uint clasterSize) const;
        template <typename T>   ND_ T       ClusteredOr  (const T value, uint clasterSize) const;
        template <typename T>   ND_ T       ClusteredXor (const T value, uint clasterSize) const;

        // quad ids:
        //   0  1
        //   2  3
        template <typename T>   ND_ T       QuadBroadcast (const T value, uint id) const;
        template <typename T>   ND_ T       QuadSwapHorizontal (const T) const;
        template <typename T>   ND_ T       QuadSwapVertical (const T) const;
        template <typename T>   ND_ T       QuadSwapDiagonal (const T) const;


        // in
        const   uint    Size;
        const   uint    Index;          // QuadIndex = Index & 3
        const   uint4   EqMask;
        const   uint4   GeMask;
        const   uint4   GtMask;
        const   uint4   LeMask;
        const   uint4   LtMask;
      #ifdef SH_COMPUTE
        const   uint    GroupCount;     // in workgroup
        const   uint    GroupIndex;     // in workgroup
      #endif

    } subgroup {};


  #ifdef SH_VERT
    // in
    const   int     InstanceIndex       = {};
    const   int     VertexIndex         = {};
    const   int     PrimitiveID         = {};
    const   int     BaseInstance        = {};
    const   int     BaseVertex          = {};
    const   int     DrawIndex           = {};

    // out
            float4  Position;
            float   ClipDistance [_MaxClipDistance];
            float   CullDistance [_MaxCullDistance];
            int     Layer;
            float   PointSize;
  #endif


  #ifdef SH_TESS_CTRL
    // sync
    void  PatchBarrier ();

    // in
    const   int     InvocationID        = {};
    const   int     PatchVerticesIn     = {};

    // out
            float   TessLevelInner [2];
            float   TessLevelOuter [4];
  #endif


  #ifdef SH_TESS_EVAL
    // in
    const   int     PrimitiveID         = {};
    const   int     PatchVerticesIn     = {};
    const   float3  TessCoord           = {};
    const   float   TessLevelInner [2]  = {};
    const   float   TessLevelOuter [4]  = {};

    // out
            float4  Position;
            float   ClipDistance [_MaxClipDistance];
            float   CullDistance [_MaxCullDistance];
            int     Layer;
            float   PointSize;
  #endif


  #ifdef SH_GEOM
    void  EmitStreamVertex (int stream);
    void  EmitVertex ();
    void  EndPrimitive ();
    void  EndStreamPrimitive (int stream);

    // in
    const   int     PrimitiveIDIn       = {};
    const   int     InvocationID        = {};

    // out
            int     PrimitiveID;
            int     ViewportIndex;
            float4  Position;
            float   ClipDistance [_MaxClipDistance];
            float   CullDistance [_MaxCullDistance];
            int     Layer;
            float   PointSize;
            int     PrimitiveID;
  #endif


    // GL_EXT_fragment_shading_rate
  #if defined(SH_VERT) or defined(SH_GEOM) or defined(SH_FRAG) or defined(SH_MESH)
    enum class ShadingRateFlag : uint
    {
        Y1,
        Y2,
        Y4,

        X1,
        X2,
        X4,
    };
  #endif
  #ifdef SH_FRAG
    // in
    const   ShadingRateFlag     ShadingRate     = ShadingRateFlag(0);
  #endif
  #if defined(SH_VERT) or defined(SH_GEOM)
    // out
            ShadingRateFlag     PrimitiveShadingRate;
  #endif


    // GLSL_EXT_mesh_shader
  #ifdef SH_MESH_TASK
    void  EmitMeshTasks (uint groupCountX, uint groupCountY, uint groupCountZ);

    // in
    const   int     DrawID      = {};   // GL_ARB_shader_draw_parameters 
  #endif


    // GLSL_EXT_mesh_shader
  #ifdef SH_MESH
    void  SetMeshOutputs (uint vertexCount, uint primitiveCount);

    // in
    const   int     DrawID      = {};   // GL_ARB_shader_draw_parameters
    const   int     ViewIndex   = {};   // GL_EXT_multiview

    // out
            uint    PrimitivePointIndices    [_MaxPrimitivePointIndices];
            uint2   PrimitiveLineIndices     [_MaxPrimitiveLineIndices];
            uint3   PrimitiveTriangleIndices [_MaxPrimitiveTriangleIndices];

    struct MeshPerVertex
    {
        float4      Position;
        float       PointSize;
        float       ClipDistance [_MaxClipDistance];
        float       CullDistance [_MaxCullDistance];
    };
    MeshPerVertex   MeshVertices [_MaxMeshVertices];

    struct MeshPerPrimitive
    {
        int             PrimitiveID;
        int             Layer;
        int             ViewportIndex;
        bool            CullPrimitive;              // GL_EXT_multiview
        ShadingRateFlag PrimitiveShadingRate;       // GL_EXT_fragment_shading_rate
    };
    MeshPerPrimitive MeshPrimitives [_MaxMeshPrimitives];
  #endif


  #ifdef SH_FRAG
    // derivatives
    template <typename T>   T  dFdx (const T &);
    template <typename T>   T  dFdy (const T &);
    template <typename T>   T  dFdxCoarse (const T &);
    template <typename T>   T  dFdyCoarse (const T &);
    template <typename T>   T  dFdxFine (const T &);
    template <typename T>   T  dFdyFine (const T &);
    template <typename T>   T  fwidth (const T &);
    template <typename T>   T  fwidthCoarse (const T &);
    template <typename T>   T  fwidthFine (const T &);

    ND_ float   InterpolateAtCentroid (float);
    ND_ float2  InterpolateAtCentroid (float2);
    ND_ float3  InterpolateAtCentroid (float3);
    ND_ float4  InterpolateAtCentroid (float4);

    ND_ float   InterpolateAtOffset (float);
    ND_ float2  InterpolateAtOffset (float2);
    ND_ float3  InterpolateAtOffset (float3);
    ND_ float4  InterpolateAtOffset (float4);

    ND_ float   InterpolateAtSample (float);
    ND_ float2  InterpolateAtSample (float2);
    ND_ float3  InterpolateAtSample (float3);
    ND_ float4  InterpolateAtSample (float4);

    const   _Uns_   Discard                         = {};

    // in
    const   float4  Position                        = {};
    const   float   ClipDistance [_MaxClipDistance] = {};
    const   float   CullDistance [_MaxCullDistance] = {};
    const   float4  FragCoord                       = {};
    const   bool    FrontFacing                     = {};
    const   bool    HelperInvocation                = {};
    const   int     Layer                           = {};
    const   float2  PointCoord                      = {};
    const   int     SampleID                        = {};
    const   int     SampleMaskIn [_MaxSampleMask]   = {};
    const   float2  SamplePosition                  = {};
    const   int     ViewportIndex                   = {};

    // out
            float   FragDepth;
            int     SampleMask [_MaxSampleMask];
  #endif


  #ifdef SH_TILE
    // TODO
  #endif


  #if defined(SH_COMPUTE) or defined(SH_MESH_TASK) or defined(SH_MESH)
    //template <typename T> using WGShared = T; // TODO

    // sync
    void  WorkgroupBarrier ();

    #ifdef AE_MEM_SCOPE
    void  ExecutionBarrier (gl::Scope execution, gl::Scope memory, gl::StorageSemantics storage, gl::Semantics sem);
    void  MemoryBarrier (gl::Scope execution, gl::Scope memory, gl::StorageSemantics storage, gl::Semantics sem);
    #endif

    const struct {
        void  All ()        const;  // all memory accesses, scope: shader invocation
        void  Buffer ()     const;
        void  Image ()      const;
        void  Shared ()     const;  // for shared variables
        void  Workgroup ()  const;  // all memory accesses, scope: workgroup
        void  Subgroup ()   const;  // all memory accesses, scope: subgroup
    } memoryBarrier;

    // in
    const   uint3   GlobalInvocationID      = {};
    const   uint3   LocalInvocationID       = {};
    const   uint    LocalInvocationIndex    = {};
    const   uint3   NumWorkGroups           = {};
    const   uint3   WorkGroupID             = {};
    const   uint3   WorkGroupSize           = {};

  #else

    // sync 
    #ifdef AE_MEM_SCOPE
    void  MemoryBarrier (gl::Scope execution, gl::Scope memory, gl::StorageSemantics storage, gl::Semantics sem);
    #endif

    const struct {
        void  All ()        const;  // all memory accesses, scope: shader invocation
        void  Buffer ()     const;
        void  Image ()      const;
    } memoryBarrier;

  #endif


  #if defined(SH_RAY_GEN)   or defined(SH_RAY_AHIT) or defined(SH_RAY_CHIT) or \
      defined(SH_RAY_MISS)  or defined(SH_RAY_INT)  or defined(SH_RAY_CALL)

    #ifndef AE_RAY_QUERY
    #  define AE_RAY_QUERY
    #endif

    template <typename T>   using RayPayload        = T;
    template <typename T>   using RayPayloadIn      = T;
    template <typename T>   using HitAttribute      = T;
    template <typename T>   using CallableData      = T;
    template <typename T>   using CallableDataIn    = T;
    template <typename T>   using ShaderRecord      = T;

    enum class TriangleHitKind
    {
        FrontFacing,
        BackFacing,
    };

    // in
    const   uint3       LaunchID;
    const   uint3       LaunchSize;
  #endif

    // shared RayTracing/RayQuery types
  #ifdef AE_RAY_QUERY
    struct AccelerationStructure
    {
        AccelerationStructure ();
        explicit AccelerationStructure (DeviceAddress);
    };

    enum class RayFlags
    {
        None,
        Opaque,
        NoOpaque,
        TerminateOnFirstHit,
        SkipClosestHitShader,
        CullBackFacingTriangles,
        CullFrontFacingTriangles,
        CullOpaque,
        CullNoOpaque,

        // GL_EXT_ray_flags_primitive_culling
        SkipTriangles,
        SkipAABB,
    };
  #endif

  #if defined(SH_RAY_GEN) or defined(SH_RAY_CHIT) or defined(SH_RAY_MISS)
    void    TraceRay (AccelerationStructure &tlas,
                      RayFlags rayFlags, uint cullMask,
                      uint sbtRecordOffset, uint sbtRecordStride, uint missIndex,
                      const float3 origin, float tMin,
                      const float3 direction, float tMax,
                      int payload);
  #endif
  #if defined(SH_RAY_GEN) or defined(SH_RAY_CHIT) or defined(SH_RAY_MISS) or defined(SH_RAY_CALL)
    void    ExecuteCallable (uint sbtRecordIndex, int callable);
  #endif
  #ifdef SH_RAY_INT
    ND_ bool  ReportIntersection (float hitT, TriangleHitKind hitKind);
  #endif

  #if defined(SH_RAY_AHIT) or defined(SH_RAY_CHIT) or defined(SH_RAY_INT)
    //   Ray hit info
    const   float               HitT;
    const   TriangleHitKind     HitKind;
  #endif

  #if defined(SH_RAY_AHIT) or defined(SH_RAY_CHIT) or defined(SH_RAY_INT)
    //   Geometry instance ids
    const   int         PrimitiveID;
    const   int         InstanceID;
    const   int         InstanceCustomIndex;
    const   int         GeometryIndex;
    //   World space parameters
    const   float3      WorldRayOrigin;
    const   float3      WorldRayDirection;
    const   float3      ObjectRayOrigin;
    const   float3      ObjectRayDirection;
    //   Ray parameters
    const   float       RayTmin;
    const   float       RayTmax;
    const   RayFlags    IncomingRayFlags;
    //   Transform matrices
    const   float4x3    ObjectToWorld;
    const   float3x4    ObjectToWorld3x4;
    const   float4x3    WorldToObject;
    const   float3x4    WorldToObject3x4;
  #endif

  #ifdef SH_RAY_MISS
    //   World space parameters
    const   float3      WorldRayOrigin;
    const   float3      WorldRayDirection;
    //   Ray parameters
    const   float       RayTmin;
    const   float       RayTmax;
    const   RayFlags    IncomingRayFlags;
  #endif

  #ifdef SH_RAY_AHIT
    const   _Uns_       IgnoreIntersection;
    const   _Uns_       TerminateRay;
  #endif

  #if defined(SH_RAY_MISS) or defined(SH_RAY_AHIT) or defined(SH_RAY_CHIT) or defined(SH_RAY_INT)
    // in
    const   uint        CullMask;   // GL_EXT_ray_cull_mask
  #endif

  #if defined(SH_RAY_AHIT) or defined(SH_RAY_CHIT)
    // in
    const   float3      HitTriangleVertexPositions [3]; // GL_EXT_ray_tracing_position_fetch
  #endif

  #ifdef AE_RAY_QUERY
    struct RayQuery     { RayQuery(); };

    enum class RayQueryCommittedIntersection
    {
        None,
        Triangle,
        Generated,
    };

    enum class RayQueryCandidateIntersection
    {
        Triangle,
        AABB,
    };

    const struct {
            void        Initialize (RayQuery &rq, AccelerationStructure &tlas,
                                    RayFlags rayFlags, uint cullMask, const float3 origin,
                                    float tMin, const float3 direction, float tMax) const;

        ND_ bool        Proceed (RayQuery &rq) const;
            void        Terminate (RayQuery &rq) const;
            void        GenerateIntersection (RayQuery &rq, float tHit) const;
            void        ConfirmIntersection (RayQuery &rq) const;
        ND_ uint        GetIntersectionType (RayQuery &rq, bool committed) const;   // RayQueryCommittedIntersection or RayQueryCandidateIntersection
        ND_ float       GetRayTMin (RayQuery &rq) const;
        ND_ RayFlags    GetRayFlags (RayQuery &rq) const;
        ND_ float3      GetWorldRayOrigin (RayQuery &rq) const;
        ND_ float3      GetWorldRayDirection (RayQuery &rq) const;
        ND_ float       GetIntersectionT (RayQuery &rq, bool committed) const;
        ND_ int         GetIntersectionInstanceCustomIndex (RayQuery &rq, bool committed) const;
        ND_ int         GetIntersectionInstanceId (RayQuery &rq, bool committed) const;
        ND_ uint        GetIntersectionInstanceSBTOffset (RayQuery &rq, bool committed) const;
        ND_ int         GetIntersectionGeometryIndex (RayQuery &rq, bool committed) const;
        ND_ int         GetIntersectionPrimitiveIndex (RayQuery &rq, bool committed) const;
        ND_ float2      GetIntersectionBarycentrics (RayQuery &rq, bool committed) const;
        ND_ bool        GetIntersectionFrontFace (RayQuery &rq, bool committed) const;
        ND_ bool        GetIntersectionCandidateAABBOpaque (RayQuery &rq, bool committed) const;
        ND_ float3      GetIntersectionObjectRayDirection (RayQuery &rq, bool committed) const;
        ND_ float3      GetIntersectionObjectRayOrigin (RayQuery &rq, bool committed) const;
        ND_ float4x3    GetIntersectionObjectToWorld (RayQuery &rq, bool committed) const;
        ND_ float4x3    GetIntersectionWorldToObject (RayQuery &rq, bool committed) const;
            void        GetIntersectionTriangleVertexPositions (RayQuery &rq, bool committed, OUT float3 (&positions)[3]) const;    // GL_EXT_ray_tracing_position_fetch

    } rayQuery {};
  #endif


    // build ray tracing acceleration structure
  #ifdef AE_RTAS_BUILD
    enum class GeometryInstanceFlags : uint
    {
        TriangleCullDisable,
        TriangleFrontCCW,
        ForceOpaque,
        ForceNonOpaque,
    };
  #endif


    // GL_NV_cooperative_matrix, GL_NV_integer_cooperative_matrix
  #if defined(SH_COMPUTE) or defined(AE_MEM_SCOPE)
    template <uint CompBits, Scope ScopeType, uint Rows, uint Columns>
    struct FCoopMatNV
    {
        using type  = ;
        using Self  = FCoopMatNV< CompBits, ScopeType, Rows, Columns >;

        ND_ constexpr uint  length()                        { return Rows * Columns; }
        ND_ type&           operator [] (uint i);
        ND_ Self            operator + (Self rhs)   const;
        ND_ Self            operator - (Self rhs)   const;
        ND_ Self            operator * (Self rhs)   const;
        ND_ Self            operator / (Self rhs)   const;
    };

    template <uint CompBits, Scope ScopeType, uint Rows, uint Columns>
    struct ICoopMatNV
    {
        using type  = ;
        using Self  = ICoopMatNV< CompBits, ScopeType, Rows, Columns >;

        ND_ constexpr uint  length()                        { return Rows * Columns; }
        ND_ type&           operator [] (uint i);
        ND_ Self            operator + (Self rhs)   const;
        ND_ Self            operator - (Self rhs)   const;
        ND_ Self            operator * (Self rhs)   const;
        ND_ Self            operator / (Self rhs)   const;
    };

    template <uint CompBits, Scope ScopeType, uint Rows, uint Columns>
    struct UCoopMatNV
    {
        using type  = ;
        using Self  = UCoopMatNV< CompBits, ScopeType, Rows, Columns >;

        ND_ constexpr uint  length()                        { return Rows * Columns; }
        ND_ type&           operator [] (uint i);
        ND_ Self            operator + (Self rhs)   const;
        ND_ Self            operator - (Self rhs)   const;
        ND_ Self            operator * (Self rhs)   const;
        ND_ Self            operator / (Self rhs)   const;
    };

    template <uint B, Scope S, uint R, uint C, typename T>  void  CoopMatLoadNV (OUT FCoopMatNV<B,S,R,C> &m, T* buf, uint element, uint stride, bool colMajor);
    template <uint B, Scope S, uint R, uint C, typename T>  void  CoopMatLoadNV (OUT ICoopMatNV<B,S,R,C> &m, T* buf, uint element, uint stride, bool colMajor);
    template <uint B, Scope S, uint R, uint C, typename T>  void  CoopMatLoadNV (OUT UCoopMatNV<B,S,R,C> &m, T* buf, uint element, uint stride, bool colMajor);

    template <uint B, Scope S, uint R, uint C, typename T>  void  CoopMatStoreNV (FCoopMatNV<B,S,R,C> m, OUT T* buf, uint element, uint stride, bool colMajor);
    template <uint B, Scope S, uint R, uint C, typename T>  void  CoopMatStoreNV (ICoopMatNV<B,S,R,C> m, OUT T* buf, uint element, uint stride, bool colMajor);
    template <uint B, Scope S, uint R, uint C, typename T>  void  CoopMatStoreNV (UCoopMatNV<B,S,R,C> m, OUT T* buf, uint element, uint stride, bool colMajor);

    template <uint B, Scope S, uint R, uint C, typename T>  ND_ FCoopMatNV<B,S,R,C>  CoopMatMulAddNV (FCoopMatNV<B,S,R,C> a, FCoopMatNV<B,S,R,C> b, FCoopMatNV<B,S,R,C> c);
    template <uint B, Scope S, uint R, uint C, typename T>  ND_ ICoopMatNV<B,S,R,C>  CoopMatMulAddNV (ICoopMatNV<B,S,R,C> a, ICoopMatNV<B,S,R,C> b, ICoopMatNV<B,S,R,C> c);
    template <uint B, Scope S, uint R, uint C, typename T>  ND_ UCoopMatNV<B,S,R,C>  CoopMatMulAddNV (UCoopMatNV<B,S,R,C> a, UCoopMatNV<B,S,R,C> b, UCoopMatNV<B,S,R,C> c);

  #endif // SH_COMPUTE or AE_MEM_SCOPE


    // GL_ARB_fragment_shader_interlock
  #ifdef SH_FRAG
    void  BeginInvocationInterlock ();
    void  EndInvocationInterlock ();
  #endif

    // GL_EXT_fragment_shader_barycentric
  #ifdef SH_FRAG
    // in
    const   float3  BaryCoord ();
    const   float3  BaryCoordNoPersp ();

    // PerVertex<>
  #endif

} gl;


// GL_EXT_fragment_shading_rate
#if defined(SH_VERT) or defined(SH_GEOM) or defined(SH_FRAG) or defined(SH_MESH)
    ND_ gl::ShadingRateFlag  operator | (gl::ShadingRateFlag lhs, gl::ShadingRateFlag rhs);
    ND_ gl::ShadingRateFlag  operator & (gl::ShadingRateFlag lhs, gl::ShadingRateFlag rhs);
#endif

#ifdef AE_RTAS_BUILD
    ND_ gl::GeometryInstanceFlags  operator | (gl::GeometryInstanceFlags lhs, gl::GeometryInstanceFlags rhs);
#endif

#ifdef AE_RAY_QUERY
    ND_ gl::RayFlags  operator | (gl::RayFlags lhs, gl::RayFlags rhs);
#endif

#ifdef AE_MEM_SCOPE
    ND_ gl::Scope               operator | (gl::Scope lhs, gl::Scope rhs);
    ND_ gl::Semantics           operator | (gl::Semantics lhs, gl::Semantics rhs);
    ND_ gl::StorageSemantics    operator | (gl::StorageSemantics lhs, gl::StorageSemantics rhs);
#endif


#ifdef __INTELLISENSE__
#   define SH_VERT
#   define SH_TESS_CTRL
#   define SH_TESS_EVAL
#   define SH_GEOM
#   define SH_FRAG
#   define SH_COMPUTE
#   define SH_TILE
#   define SH_MESH_TASK
#   define SH_MESH
#   define SH_RAY_GEN
#   define SH_RAY_AHIT
#   define SH_RAY_CHIT
#   define SH_RAY_MISS
#   define SH_RAY_INT
#   define SH_RAY_CALL
#endif
