//F47019D6
#include <vector>
#include <string>

using int8      = std::int8_t;
using uint8     = std::uint8_t;
using int16     = std::int16_t;
using uint16    = std::uint16_t;
using int       = std::int32_t;
using uint      = std::uint32_t;
using int64     = std::int64_t;
using uint64    = std::uint64_t;
using string    = std::string;

template <typename T>
struct RC;

template <typename T>
using array = std::vector<T>;

struct short4;
struct Archive;
struct sbyte2;
struct ushort2;
struct ushort3;
struct bool4;
struct bool2;
struct EPathParamsFlags;
struct bool3;
struct int4;
struct int3;
struct int2;
struct ubyte4;
struct ubyte3;
struct EFileType;
struct ubyte2;
struct InputActions;
struct PipelineCompiler;
struct float2;
struct float3;
struct uint4;
struct float4;
struct uint3;
struct uint2;
struct EReflectionFlags;
struct AssetPacker;
struct sbyte4;
struct short2;
struct ushort4;
struct sbyte3;
struct short3;

int  Abs (int);
int  MirroredWrap (int, int, int);
int  Square (int);
int  Min (int, int);
int  Max (int, int);
int  Clamp (int, int, int);
int  Wrap (int, int, int);
int  CalcAverage (int, int);
int  IntLog2 (int);
int  CeilIntLog2 (int);
int  BitScanReverse (int);
int  BitScanForward (int);
uint  BitCount (int);
bool  IsPowerOfTwo (int);
bool  IsSingleBitSet (int);
bool  AllBits (int, int);
bool  AnyBits (int, int);
int  ExtractBit (int &);
int  ExtractBitLog2 (int &);
int  BitRotateLeft (int, uint);
int  BitRotateRight (int, uint);
int  FloorPOT (int);
int  CeilPOT (int);
int  AlignDown (int, int);
int  AlignUp (int, int);
bool  IsAligned (int, int);
uint  Square (uint);
uint  Min (uint, uint);
uint  Max (uint, uint);
uint  Clamp (uint, uint, uint);
uint  Wrap (uint, uint, uint);
uint  CalcAverage (uint, uint);
int  IntLog2 (uint);
int  CeilIntLog2 (uint);
int  BitScanReverse (uint);
int  BitScanForward (uint);
uint  BitCount (uint);
bool  IsPowerOfTwo (uint);
bool  IsSingleBitSet (uint);
bool  AllBits (uint, uint);
bool  AnyBits (uint, uint);
uint  ExtractBit (uint &);
uint  ExtractBitLog2 (uint &);
uint  BitRotateLeft (uint, uint);
uint  BitRotateRight (uint, uint);
uint  FloorPOT (uint);
uint  CeilPOT (uint);
uint  AlignDown (uint, uint);
uint  AlignUp (uint, uint);
bool  IsAligned (uint, uint);
float  Abs (float);
float  MirroredWrap (float, float, float);
float  Square (float);
float  Min (float, float);
float  Max (float, float);
float  Clamp (float, float, float);
float  Wrap (float, float, float);
float  CalcAverage (float, float);
float  Floor (float);
float  Ceil (float);
float  Trunc (float);
float  Fract (float);
float  Round (float);
float  Mod (float, float);
float  Sqrt (float);
float  Pow (float, float);
float  Ln (float);
float  Log (float, float);
float  Log2 (float);
float  Log10 (float);
float  Exp (float);
float  Exp2 (float);
float  Exp10 (float);
float  ExpMinus1 (float);
float  Sin (float);
float  SinH (float);
float  ASin (float);
float  Cos (float);
float  CosH (float);
float  ACos (float);
float  Tan (float);
float  TanH (float);
float  ATan (float, float);
float  ToRad (float);
float  ToDeg (float);
float  Lerp (float, float, float);
float  ToSNorm (float);
float  ToUNorm (float);
float  Remap (float, float, float, float, float);
float  RemapClamped (float, float, float, float, float);
int  RoundToInt (float);
uint  RoundToUint (float);
float  IsInfinity (float);
float  IsNaN (float);
float  IsFinite (float);
struct bool2
{
    bool2 ();
    bool2 (const bool2&);
    bool2&  operator = (const bool2&);
    bool x;
    bool y;
    bool2 (bool);
    bool2 (const bool3 &);
    bool2 (const bool4 &);
    bool2 (bool, bool);
    bool2 (const short2 &);
    bool2 (const short3 &);
    bool2 (const short4 &);
    bool2 (const ushort2 &);
    bool2 (const ushort3 &);
    bool2 (const ushort4 &);
    bool2 (const int2 &);
    bool2 (const int3 &);
    bool2 (const int4 &);
    bool2 (const uint2 &);
    bool2 (const uint3 &);
    bool2 (const uint4 &);
    bool2 (const float2 &);
    bool2 (const float3 &);
    bool2 (const float4 &);
};

struct bool3
{
    bool3 ();
    bool3 (const bool3&);
    bool3&  operator = (const bool3&);
    bool x;
    bool y;
    bool z;
    bool3 (bool);
    bool3 (const bool2 &);
    bool3 (const bool4 &);
    bool3 (const bool2 &, bool);
    bool3 (bool, bool, bool);
    bool3 (const short2 &);
    bool3 (const short3 &);
    bool3 (const short4 &);
    bool3 (const ushort2 &);
    bool3 (const ushort3 &);
    bool3 (const ushort4 &);
    bool3 (const int2 &);
    bool3 (const int3 &);
    bool3 (const int4 &);
    bool3 (const uint2 &);
    bool3 (const uint3 &);
    bool3 (const uint4 &);
    bool3 (const float2 &);
    bool3 (const float3 &);
    bool3 (const float4 &);
};

struct bool4
{
    bool4 ();
    bool4 (const bool4&);
    bool4&  operator = (const bool4&);
    bool x;
    bool y;
    bool z;
    bool w;
    bool4 (bool);
    bool4 (const bool2 &);
    bool4 (const bool3 &);
    bool4 (const bool2 &, const bool2 &);
    bool4 (const bool3 &, bool);
    bool4 (bool, bool, bool, bool);
    bool4 (const short2 &);
    bool4 (const short3 &);
    bool4 (const short4 &);
    bool4 (const ushort2 &);
    bool4 (const ushort3 &);
    bool4 (const ushort4 &);
    bool4 (const int2 &);
    bool4 (const int3 &);
    bool4 (const int4 &);
    bool4 (const uint2 &);
    bool4 (const uint3 &);
    bool4 (const uint4 &);
    bool4 (const float2 &);
    bool4 (const float3 &);
    bool4 (const float4 &);
};

struct sbyte2
{
    sbyte2 ();
    sbyte2 (const sbyte2&);
    sbyte2&  operator = (const sbyte2&);
    int8 x;
    int8 y;
    sbyte2 (int8);
    sbyte2 (const sbyte3 &);
    sbyte2 (const sbyte4 &);
    sbyte2 (int8, int8);
    sbyte2 (const short2 &);
    sbyte2 (const short3 &);
    sbyte2 (const short4 &);
    sbyte2 (const ushort2 &);
    sbyte2 (const ushort3 &);
    sbyte2 (const ushort4 &);
    sbyte2 (const int2 &);
    sbyte2 (const int3 &);
    sbyte2 (const int4 &);
    sbyte2 (const uint2 &);
    sbyte2 (const uint3 &);
    sbyte2 (const uint4 &);
    sbyte2 (const float2 &);
    sbyte2 (const float3 &);
    sbyte2 (const float4 &);
};

struct sbyte3
{
    sbyte3 ();
    sbyte3 (const sbyte3&);
    sbyte3&  operator = (const sbyte3&);
    int8 x;
    int8 y;
    int8 z;
    sbyte3 (int8);
    sbyte3 (const sbyte2 &);
    sbyte3 (const sbyte4 &);
    sbyte3 (const sbyte2 &, int8);
    sbyte3 (int8, int8, int8);
    sbyte3 (const short2 &);
    sbyte3 (const short3 &);
    sbyte3 (const short4 &);
    sbyte3 (const ushort2 &);
    sbyte3 (const ushort3 &);
    sbyte3 (const ushort4 &);
    sbyte3 (const int2 &);
    sbyte3 (const int3 &);
    sbyte3 (const int4 &);
    sbyte3 (const uint2 &);
    sbyte3 (const uint3 &);
    sbyte3 (const uint4 &);
    sbyte3 (const float2 &);
    sbyte3 (const float3 &);
    sbyte3 (const float4 &);
};

struct sbyte4
{
    sbyte4 ();
    sbyte4 (const sbyte4&);
    sbyte4&  operator = (const sbyte4&);
    int8 x;
    int8 y;
    int8 z;
    int8 w;
    sbyte4 (int8);
    sbyte4 (const sbyte2 &);
    sbyte4 (const sbyte3 &);
    sbyte4 (const sbyte2 &, const sbyte2 &);
    sbyte4 (const sbyte3 &, int8);
    sbyte4 (int8, int8, int8, int8);
    sbyte4 (const short2 &);
    sbyte4 (const short3 &);
    sbyte4 (const short4 &);
    sbyte4 (const ushort2 &);
    sbyte4 (const ushort3 &);
    sbyte4 (const ushort4 &);
    sbyte4 (const int2 &);
    sbyte4 (const int3 &);
    sbyte4 (const int4 &);
    sbyte4 (const uint2 &);
    sbyte4 (const uint3 &);
    sbyte4 (const uint4 &);
    sbyte4 (const float2 &);
    sbyte4 (const float3 &);
    sbyte4 (const float4 &);
};

struct ubyte2
{
    ubyte2 ();
    ubyte2 (const ubyte2&);
    ubyte2&  operator = (const ubyte2&);
    uint8 x;
    uint8 y;
    ubyte2 (uint8);
    ubyte2 (const ubyte3 &);
    ubyte2 (const ubyte4 &);
    ubyte2 (uint8, uint8);
    ubyte2 (const short2 &);
    ubyte2 (const short3 &);
    ubyte2 (const short4 &);
    ubyte2 (const ushort2 &);
    ubyte2 (const ushort3 &);
    ubyte2 (const ushort4 &);
    ubyte2 (const int2 &);
    ubyte2 (const int3 &);
    ubyte2 (const int4 &);
    ubyte2 (const uint2 &);
    ubyte2 (const uint3 &);
    ubyte2 (const uint4 &);
    ubyte2 (const float2 &);
    ubyte2 (const float3 &);
    ubyte2 (const float4 &);
};

struct ubyte3
{
    ubyte3 ();
    ubyte3 (const ubyte3&);
    ubyte3&  operator = (const ubyte3&);
    uint8 x;
    uint8 y;
    uint8 z;
    ubyte3 (uint8);
    ubyte3 (const ubyte2 &);
    ubyte3 (const ubyte4 &);
    ubyte3 (const ubyte2 &, uint8);
    ubyte3 (uint8, uint8, uint8);
    ubyte3 (const short2 &);
    ubyte3 (const short3 &);
    ubyte3 (const short4 &);
    ubyte3 (const ushort2 &);
    ubyte3 (const ushort3 &);
    ubyte3 (const ushort4 &);
    ubyte3 (const int2 &);
    ubyte3 (const int3 &);
    ubyte3 (const int4 &);
    ubyte3 (const uint2 &);
    ubyte3 (const uint3 &);
    ubyte3 (const uint4 &);
    ubyte3 (const float2 &);
    ubyte3 (const float3 &);
    ubyte3 (const float4 &);
};

struct ubyte4
{
    ubyte4 ();
    ubyte4 (const ubyte4&);
    ubyte4&  operator = (const ubyte4&);
    uint8 x;
    uint8 y;
    uint8 z;
    uint8 w;
    ubyte4 (uint8);
    ubyte4 (const ubyte2 &);
    ubyte4 (const ubyte3 &);
    ubyte4 (const ubyte2 &, const ubyte2 &);
    ubyte4 (const ubyte3 &, uint8);
    ubyte4 (uint8, uint8, uint8, uint8);
    ubyte4 (const short2 &);
    ubyte4 (const short3 &);
    ubyte4 (const short4 &);
    ubyte4 (const ushort2 &);
    ubyte4 (const ushort3 &);
    ubyte4 (const ushort4 &);
    ubyte4 (const int2 &);
    ubyte4 (const int3 &);
    ubyte4 (const int4 &);
    ubyte4 (const uint2 &);
    ubyte4 (const uint3 &);
    ubyte4 (const uint4 &);
    ubyte4 (const float2 &);
    ubyte4 (const float3 &);
    ubyte4 (const float4 &);
};

struct short2
{
    short2 ();
    short2 (const short2&);
    short2&  operator = (const short2&);
    int16 x;
    int16 y;
    short2 (int16);
    short2 (const short3 &);
    short2 (const short4 &);
    short2 (int16, int16);
    short2 (const ushort2 &);
    short2 (const ushort3 &);
    short2 (const ushort4 &);
    short2 (const int2 &);
    short2 (const int3 &);
    short2 (const int4 &);
    short2 (const uint2 &);
    short2 (const uint3 &);
    short2 (const uint4 &);
    short2 (const float2 &);
    short2 (const float3 &);
    short2 (const float4 &);
};

struct short3
{
    short3 ();
    short3 (const short3&);
    short3&  operator = (const short3&);
    int16 x;
    int16 y;
    int16 z;
    short3 (int16);
    short3 (const short2 &);
    short3 (const short4 &);
    short3 (const short2 &, int16);
    short3 (int16, int16, int16);
    short3 (const ushort2 &);
    short3 (const ushort3 &);
    short3 (const ushort4 &);
    short3 (const int2 &);
    short3 (const int3 &);
    short3 (const int4 &);
    short3 (const uint2 &);
    short3 (const uint3 &);
    short3 (const uint4 &);
    short3 (const float2 &);
    short3 (const float3 &);
    short3 (const float4 &);
};

struct short4
{
    short4 ();
    short4 (const short4&);
    short4&  operator = (const short4&);
    int16 x;
    int16 y;
    int16 z;
    int16 w;
    short4 (int16);
    short4 (const short2 &);
    short4 (const short3 &);
    short4 (const short2 &, const short2 &);
    short4 (const short3 &, int16);
    short4 (int16, int16, int16, int16);
    short4 (const ushort2 &);
    short4 (const ushort3 &);
    short4 (const ushort4 &);
    short4 (const int2 &);
    short4 (const int3 &);
    short4 (const int4 &);
    short4 (const uint2 &);
    short4 (const uint3 &);
    short4 (const uint4 &);
    short4 (const float2 &);
    short4 (const float3 &);
    short4 (const float4 &);
};

struct ushort2
{
    ushort2 ();
    ushort2 (const ushort2&);
    ushort2&  operator = (const ushort2&);
    uint16 x;
    uint16 y;
    ushort2 (uint16);
    ushort2 (const ushort3 &);
    ushort2 (const ushort4 &);
    ushort2 (uint16, uint16);
    ushort2 (const short2 &);
    ushort2 (const short3 &);
    ushort2 (const short4 &);
    ushort2 (const int2 &);
    ushort2 (const int3 &);
    ushort2 (const int4 &);
    ushort2 (const uint2 &);
    ushort2 (const uint3 &);
    ushort2 (const uint4 &);
    ushort2 (const float2 &);
    ushort2 (const float3 &);
    ushort2 (const float4 &);
};

struct ushort3
{
    ushort3 ();
    ushort3 (const ushort3&);
    ushort3&  operator = (const ushort3&);
    uint16 x;
    uint16 y;
    uint16 z;
    ushort3 (uint16);
    ushort3 (const ushort2 &);
    ushort3 (const ushort4 &);
    ushort3 (const ushort2 &, uint16);
    ushort3 (uint16, uint16, uint16);
    ushort3 (const short2 &);
    ushort3 (const short3 &);
    ushort3 (const short4 &);
    ushort3 (const int2 &);
    ushort3 (const int3 &);
    ushort3 (const int4 &);
    ushort3 (const uint2 &);
    ushort3 (const uint3 &);
    ushort3 (const uint4 &);
    ushort3 (const float2 &);
    ushort3 (const float3 &);
    ushort3 (const float4 &);
};

struct ushort4
{
    ushort4 ();
    ushort4 (const ushort4&);
    ushort4&  operator = (const ushort4&);
    uint16 x;
    uint16 y;
    uint16 z;
    uint16 w;
    ushort4 (uint16);
    ushort4 (const ushort2 &);
    ushort4 (const ushort3 &);
    ushort4 (const ushort2 &, const ushort2 &);
    ushort4 (const ushort3 &, uint16);
    ushort4 (uint16, uint16, uint16, uint16);
    ushort4 (const short2 &);
    ushort4 (const short3 &);
    ushort4 (const short4 &);
    ushort4 (const int2 &);
    ushort4 (const int3 &);
    ushort4 (const int4 &);
    ushort4 (const uint2 &);
    ushort4 (const uint3 &);
    ushort4 (const uint4 &);
    ushort4 (const float2 &);
    ushort4 (const float3 &);
    ushort4 (const float4 &);
};

struct int2
{
    int2 ();
    int2 (const int2&);
    int2&  operator = (const int2&);
    int x;
    int y;
    int2 (int);
    int2 (const int3 &);
    int2 (const int4 &);
    int2 (int, int);
    int2 (const short2 &);
    int2 (const short3 &);
    int2 (const short4 &);
    int2 (const ushort2 &);
    int2 (const ushort3 &);
    int2 (const ushort4 &);
    int2 (const uint2 &);
    int2 (const uint3 &);
    int2 (const uint4 &);
    int2 (const float2 &);
    int2 (const float3 &);
    int2 (const float4 &);
};

struct int3
{
    int3 ();
    int3 (const int3&);
    int3&  operator = (const int3&);
    int x;
    int y;
    int z;
    int3 (int);
    int3 (const int2 &);
    int3 (const int4 &);
    int3 (const int2 &, int);
    int3 (int, int, int);
    int3 (const short2 &);
    int3 (const short3 &);
    int3 (const short4 &);
    int3 (const ushort2 &);
    int3 (const ushort3 &);
    int3 (const ushort4 &);
    int3 (const uint2 &);
    int3 (const uint3 &);
    int3 (const uint4 &);
    int3 (const float2 &);
    int3 (const float3 &);
    int3 (const float4 &);
};

struct int4
{
    int4 ();
    int4 (const int4&);
    int4&  operator = (const int4&);
    int x;
    int y;
    int z;
    int w;
    int4 (int);
    int4 (const int2 &);
    int4 (const int3 &);
    int4 (const int2 &, const int2 &);
    int4 (const int3 &, int);
    int4 (int, int, int, int);
    int4 (const short2 &);
    int4 (const short3 &);
    int4 (const short4 &);
    int4 (const ushort2 &);
    int4 (const ushort3 &);
    int4 (const ushort4 &);
    int4 (const uint2 &);
    int4 (const uint3 &);
    int4 (const uint4 &);
    int4 (const float2 &);
    int4 (const float3 &);
    int4 (const float4 &);
};

struct uint2
{
    uint2 ();
    uint2 (const uint2&);
    uint2&  operator = (const uint2&);
    uint x;
    uint y;
    uint2 (uint);
    uint2 (const uint3 &);
    uint2 (const uint4 &);
    uint2 (uint, uint);
    uint2 (const short2 &);
    uint2 (const short3 &);
    uint2 (const short4 &);
    uint2 (const ushort2 &);
    uint2 (const ushort3 &);
    uint2 (const ushort4 &);
    uint2 (const int2 &);
    uint2 (const int3 &);
    uint2 (const int4 &);
    uint2 (const float2 &);
    uint2 (const float3 &);
    uint2 (const float4 &);
};

struct uint3
{
    uint3 ();
    uint3 (const uint3&);
    uint3&  operator = (const uint3&);
    uint x;
    uint y;
    uint z;
    uint3 (uint);
    uint3 (const uint2 &);
    uint3 (const uint4 &);
    uint3 (const uint2 &, uint);
    uint3 (uint, uint, uint);
    uint3 (const short2 &);
    uint3 (const short3 &);
    uint3 (const short4 &);
    uint3 (const ushort2 &);
    uint3 (const ushort3 &);
    uint3 (const ushort4 &);
    uint3 (const int2 &);
    uint3 (const int3 &);
    uint3 (const int4 &);
    uint3 (const float2 &);
    uint3 (const float3 &);
    uint3 (const float4 &);
};

struct uint4
{
    uint4 ();
    uint4 (const uint4&);
    uint4&  operator = (const uint4&);
    uint x;
    uint y;
    uint z;
    uint w;
    uint4 (uint);
    uint4 (const uint2 &);
    uint4 (const uint3 &);
    uint4 (const uint2 &, const uint2 &);
    uint4 (const uint3 &, uint);
    uint4 (uint, uint, uint, uint);
    uint4 (const short2 &);
    uint4 (const short3 &);
    uint4 (const short4 &);
    uint4 (const ushort2 &);
    uint4 (const ushort3 &);
    uint4 (const ushort4 &);
    uint4 (const int2 &);
    uint4 (const int3 &);
    uint4 (const int4 &);
    uint4 (const float2 &);
    uint4 (const float3 &);
    uint4 (const float4 &);
};

struct float2
{
    float2 ();
    float2 (const float2&);
    float2&  operator = (const float2&);
    float x;
    float y;
    float2 (float);
    float2 (const float3 &);
    float2 (const float4 &);
    float2 (float, float);
    float2 (const short2 &);
    float2 (const short3 &);
    float2 (const short4 &);
    float2 (const ushort2 &);
    float2 (const ushort3 &);
    float2 (const ushort4 &);
    float2 (const int2 &);
    float2 (const int3 &);
    float2 (const int4 &);
    float2 (const uint2 &);
    float2 (const uint3 &);
    float2 (const uint4 &);
};

struct float3
{
    float3 ();
    float3 (const float3&);
    float3&  operator = (const float3&);
    float x;
    float y;
    float z;
    float3 (float);
    float3 (const float2 &);
    float3 (const float4 &);
    float3 (const float2 &, float);
    float3 (float, float, float);
    float3 (const short2 &);
    float3 (const short3 &);
    float3 (const short4 &);
    float3 (const ushort2 &);
    float3 (const ushort3 &);
    float3 (const ushort4 &);
    float3 (const int2 &);
    float3 (const int3 &);
    float3 (const int4 &);
    float3 (const uint2 &);
    float3 (const uint3 &);
    float3 (const uint4 &);
};

struct float4
{
    float4 ();
    float4 (const float4&);
    float4&  operator = (const float4&);
    float x;
    float y;
    float z;
    float w;
    float4 (float);
    float4 (const float2 &);
    float4 (const float3 &);
    float4 (const float2 &, const float2 &);
    float4 (const float3 &, float);
    float4 (float, float, float, float);
    float4 (const short2 &);
    float4 (const short3 &);
    float4 (const short4 &);
    float4 (const ushort2 &);
    float4 (const ushort3 &);
    float4 (const ushort4 &);
    float4 (const int2 &);
    float4 (const int3 &);
    float4 (const int4 &);
    float4 (const uint2 &);
    float4 (const uint3 &);
    float4 (const uint4 &);
};

bool  All (const bool2 &);
bool  Any (const bool2 &);
bool  All (const bool3 &);
bool  Any (const bool3 &);
bool  All (const bool4 &);
bool  Any (const bool4 &);
int2  Abs (const int2 &);
int2  MirroredWrap (const int2 &, const int2 &, const int2 &);
int2  MirroredWrap (const int2 &, int, int);
int  Sum (const int2 &);
int  Area (const int2 &);
int2  Min (const int2 &, const int2 &);
int2  Max (const int2 &, const int2 &);
int2  Clamp (const int2 &, const int2 &, const int2 &);
int2  Clamp (const int2 &, int, int);
int2  Wrap (const int2 &, const int2 &, const int2 &);
int2  Wrap (const int2 &, int, int);
int  ToLinear (const int2 &, const int2 &);
int3  Abs (const int3 &);
int3  MirroredWrap (const int3 &, const int3 &, const int3 &);
int3  MirroredWrap (const int3 &, int, int);
int  Sum (const int3 &);
int  Area (const int3 &);
int3  Min (const int3 &, const int3 &);
int3  Max (const int3 &, const int3 &);
int3  Clamp (const int3 &, const int3 &, const int3 &);
int3  Clamp (const int3 &, int, int);
int3  Wrap (const int3 &, const int3 &, const int3 &);
int3  Wrap (const int3 &, int, int);
int  ToLinear (const int3 &, const int3 &);
int4  Abs (const int4 &);
int4  MirroredWrap (const int4 &, const int4 &, const int4 &);
int4  MirroredWrap (const int4 &, int, int);
int  Sum (const int4 &);
int  Area (const int4 &);
int4  Min (const int4 &, const int4 &);
int4  Max (const int4 &, const int4 &);
int4  Clamp (const int4 &, const int4 &, const int4 &);
int4  Clamp (const int4 &, int, int);
int4  Wrap (const int4 &, const int4 &, const int4 &);
int4  Wrap (const int4 &, int, int);
int  ToLinear (const int4 &, const int4 &);
uint  Sum (const uint2 &);
uint  Area (const uint2 &);
uint2  Min (const uint2 &, const uint2 &);
uint2  Max (const uint2 &, const uint2 &);
uint2  Clamp (const uint2 &, const uint2 &, const uint2 &);
uint2  Clamp (const uint2 &, uint, uint);
uint2  Wrap (const uint2 &, const uint2 &, const uint2 &);
uint2  Wrap (const uint2 &, uint, uint);
uint  ToLinear (const uint2 &, const uint2 &);
uint  Sum (const uint3 &);
uint  Area (const uint3 &);
uint3  Min (const uint3 &, const uint3 &);
uint3  Max (const uint3 &, const uint3 &);
uint3  Clamp (const uint3 &, const uint3 &, const uint3 &);
uint3  Clamp (const uint3 &, uint, uint);
uint3  Wrap (const uint3 &, const uint3 &, const uint3 &);
uint3  Wrap (const uint3 &, uint, uint);
uint  ToLinear (const uint3 &, const uint3 &);
uint  Sum (const uint4 &);
uint  Area (const uint4 &);
uint4  Min (const uint4 &, const uint4 &);
uint4  Max (const uint4 &, const uint4 &);
uint4  Clamp (const uint4 &, const uint4 &, const uint4 &);
uint4  Clamp (const uint4 &, uint, uint);
uint4  Wrap (const uint4 &, const uint4 &, const uint4 &);
uint4  Wrap (const uint4 &, uint, uint);
uint  ToLinear (const uint4 &, const uint4 &);
float2  Abs (const float2 &);
float2  MirroredWrap (const float2 &, const float2 &, const float2 &);
float2  MirroredWrap (const float2 &, float, float);
float  Sum (const float2 &);
float  Area (const float2 &);
float2  Min (const float2 &, const float2 &);
float2  Max (const float2 &, const float2 &);
float2  Clamp (const float2 &, const float2 &, const float2 &);
float2  Clamp (const float2 &, float, float);
float2  Wrap (const float2 &, const float2 &, const float2 &);
float2  Wrap (const float2 &, float, float);
float  ToLinear (const float2 &, const float2 &);
float2  ToSNorm (const float2 &);
float2  ToUNorm (const float2 &);
float  Dot (const float2 &, const float2 &);
float2  Lerp (const float2 &, const float2 &, float);
float2  Normalize (const float2 &);
float2  Floor (const float2 &);
float2  Ceil (const float2 &);
float2  Round (const float2 &);
float2  Fract (const float2 &);
float2  Sqrt (const float2 &);
float2  Ln (const float2 &);
float2  Log2 (const float2 &);
float2  Log (const float2 &, const float2 &);
float2  Log (const float2 &, float);
float2  Pow (const float2 &, const float2 &);
float2  Pow (const float2 &, float);
float2  Exp (const float2 &);
float2  Exp2 (const float2 &);
float2  Exp10 (const float2 &);
float  Length (const float2 &);
float  LengthSqr (const float2 &);
float  Distance (const float2 &, const float2 &);
float  DistanceSqr (const float2 &, const float2 &);
float3  Abs (const float3 &);
float3  MirroredWrap (const float3 &, const float3 &, const float3 &);
float3  MirroredWrap (const float3 &, float, float);
float  Sum (const float3 &);
float  Area (const float3 &);
float3  Min (const float3 &, const float3 &);
float3  Max (const float3 &, const float3 &);
float3  Clamp (const float3 &, const float3 &, const float3 &);
float3  Clamp (const float3 &, float, float);
float3  Wrap (const float3 &, const float3 &, const float3 &);
float3  Wrap (const float3 &, float, float);
float  ToLinear (const float3 &, const float3 &);
float3  Cross (const float3 &, const float3 &);
float3  ToSNorm (const float3 &);
float3  ToUNorm (const float3 &);
float  Dot (const float3 &, const float3 &);
float3  Lerp (const float3 &, const float3 &, float);
float3  Normalize (const float3 &);
float3  Floor (const float3 &);
float3  Ceil (const float3 &);
float3  Round (const float3 &);
float3  Fract (const float3 &);
float3  Sqrt (const float3 &);
float3  Ln (const float3 &);
float3  Log2 (const float3 &);
float3  Log (const float3 &, const float3 &);
float3  Log (const float3 &, float);
float3  Pow (const float3 &, const float3 &);
float3  Pow (const float3 &, float);
float3  Exp (const float3 &);
float3  Exp2 (const float3 &);
float3  Exp10 (const float3 &);
float  Length (const float3 &);
float  LengthSqr (const float3 &);
float  Distance (const float3 &, const float3 &);
float  DistanceSqr (const float3 &, const float3 &);
float4  Abs (const float4 &);
float4  MirroredWrap (const float4 &, const float4 &, const float4 &);
float4  MirroredWrap (const float4 &, float, float);
float  Sum (const float4 &);
float  Area (const float4 &);
float4  Min (const float4 &, const float4 &);
float4  Max (const float4 &, const float4 &);
float4  Clamp (const float4 &, const float4 &, const float4 &);
float4  Clamp (const float4 &, float, float);
float4  Wrap (const float4 &, const float4 &, const float4 &);
float4  Wrap (const float4 &, float, float);
float  ToLinear (const float4 &, const float4 &);
float4  ToSNorm (const float4 &);
float4  ToUNorm (const float4 &);
float  Dot (const float4 &, const float4 &);
float4  Lerp (const float4 &, const float4 &, float);
float4  Normalize (const float4 &);
float4  Floor (const float4 &);
float4  Ceil (const float4 &);
float4  Round (const float4 &);
float4  Fract (const float4 &);
float4  Sqrt (const float4 &);
float4  Ln (const float4 &);
float4  Log2 (const float4 &);
float4  Log (const float4 &, const float4 &);
float4  Log (const float4 &, float);
float4  Pow (const float4 &, const float4 &);
float4  Pow (const float4 &, float);
float4  Exp (const float4 &);
float4  Exp2 (const float4 &);
float4  Exp10 (const float4 &);
float  Length (const float4 &);
float  LengthSqr (const float4 &);
float  Distance (const float4 &, const float4 &);
float  DistanceSqr (const float4 &, const float4 &);
string  GetSharedFeatureSetPath ();
string  GetSharedShadersPath ();
string  GetCanvasVerticesPath ();
string  GetOutputDir ();
void  DeleteFolder (const string &);
struct EPathParamsFlags
{
    EPathParamsFlags () {}
    EPathParamsFlags (uint) {}
    operator uint () const;
    static constexpr uint Unknown = 0;
    static constexpr uint Recursive = 1;
};

struct EFileType
{
    EFileType () {}
    EFileType (uint) {}
    operator uint () const;
    static constexpr uint Raw = 1;
    static constexpr uint Brotli = 2;
    static constexpr uint InMemory = 4;
    static constexpr uint BrotliInMemory = 6;
};

struct EReflectionFlags
{
    EReflectionFlags () {}
    EReflectionFlags (uint) {}
    operator uint () const;
    static constexpr uint RenderTechniques = 1;
    static constexpr uint RTechPass_Pipelines = 2;
    static constexpr uint RTech_ShaderBindingTable = 4;
    static constexpr uint All = 7;
};

struct PipelineCompiler
{
    PipelineCompiler ();
    void  AddPipelineFolder (const string &, uint, EPathParamsFlags);
    void  AddPipelineFolder (const string &, EPathParamsFlags);
    void  AddPipelineFolder (const string &);
    void  AddPipeline (const string &, uint, EPathParamsFlags);
    void  AddPipeline (const string &, EPathParamsFlags);
    void  AddPipeline (const string &);
    void  AddShaderFolder (const string &);
    void  IncludeDir (const string &);
    void  SetOutputCPPFile (const string &, const string &, uint);
    void  SetOutputCPPFile (const string &, const string &, EReflectionFlags);
    void  Compile (const string &);
    void  CompileWithNameMapping (const string &);
};

struct InputActions
{
    InputActions ();
    void  Add (const string &);
    void  SetOutputCPPFile (const string &);
    void  Convert (const string &);
};

struct AssetPacker
{
    AssetPacker ();
    void  Add (const string &);
    void  AddFolder (const string &);
    void  SetTempFile (const string &);
    void  ToArchive (const string &);
};

struct Archive
{
    Archive ();
    void  SetTempFile (const string &);
    void  SetDefaultFileType (EFileType);
    void  Add (const string &, const string &, EFileType);
    void  Add (const string &, EFileType);
    void  Add (const string &, const string &);
    void  Add (const string &);
    void  AddArchive (const string &);
    void  Store (const string &);
};

template <>
struct RC<Archive> : Archive
{
    RC (const Archive &);
};

template <>
struct RC<InputActions> : InputActions
{
    RC (const InputActions &);
};

template <>
struct RC<PipelineCompiler> : PipelineCompiler
{
    RC (const PipelineCompiler &);
};

template <>
struct RC<AssetPacker> : AssetPacker
{
    RC (const AssetPacker &);
};

