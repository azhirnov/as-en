//CB390BE2
#include <vector>
#include <string>

using int8      = std::int8_t;
using uint8     = std::uint8_t;
using int16     = std::int16_t;
using uint16    = std::uint16_t;
using int       = std::int32_t;
using uint      = std::uint32_t;
using int32     = std::int32_t;
using uint32    = std::uint32_t;
using int64     = std::int64_t;
using uint64    = std::uint64_t;
using string    = std::string;

template <typename T>
struct RC;

template <typename T>
using array = std::vector<T>;

struct EResourceState;
struct EPipelineOpt;
struct RasterFont;
struct float2;
struct ESurfaceFormat;
struct MipmapLevel;
struct ESamplerYcbcrRange;
struct ESamplerUsage;
struct EReductionMode;
struct ubyte4;
struct ESubgroupOperation;
struct ubyte2;
struct EGraphicsDeviceID;
struct ubyte3;
struct Material;
struct Mesh;
struct EPrimitive;
struct DepthStencil;
struct short4;
struct EPixelFormat;
struct EBlendOp;
struct ushort3;
struct ushort2;
struct sbyte2;
struct HSVColor;
struct ELogicOp;
struct int4;
struct EImage;
struct RGBA32f;
struct int2;
struct EQueueMask;
struct bool3;
struct RGBA32i;
struct bool4;
struct ImageLayer;
struct EVertexType;
struct bool2;
struct int3;
struct ECullMode;
struct ECubeFace;
struct EAddressMode;
struct EPolygonMode;
struct ERasterFontMode;
struct EAttachmentLoadOp;
struct EBlendFactor;
struct EStencilOp;
struct ERTInstanceOpt;
struct Model;
struct float4;
struct ECompareOp;
struct uint4;
struct RectU;
struct uint2;
struct EIndex;
struct uint3;
struct RectI;
struct RectF;
struct ESubgroupTypes;
struct EShader;
struct EShaderStages;
struct RGBA32u;
struct short3;
struct ushort4;
struct short2;
struct sbyte3;
struct sbyte4;
struct ESamplerYcbcrModelConversion;
struct RGBA8u;
struct Texture;
struct ImageAtlas;
struct EFilter;
struct EMipmapFilter;
struct EPipelineDynamicState;
struct MultiSamples;
struct EFeature;
struct ESamplerChromaLocation;
struct EBorderColor;
struct EDescSetUsage;
struct EAttachmentStoreOp;
struct EVertexInputRate;
struct EImageAspect;
struct EVendorID;
struct EShaderIO;
struct float3;

using sbyte = int8;
using ubyte = uint8;
using sshort = int16;
using ushort = uint16;
using sint = int;
using slong = int64;
using ulong = uint64;
int  Abs (int x);
int  MirroredWrap (int x, int min, int max);
int  Square (int x);
int  Min (int x, int y);
int  Max (int x, int y);
int  Clamp (int x, int min, int max);
int  Wrap (int x, int min, int max);
int  Average (int x, int y);
int  IntLog2 (int x);
int  CeilIntLog2 (int x);
int  BitScanReverse (int x);
int  BitScanForward (int x);
uint  BitCount (int x);
bool  IsPowerOfTwo (int x);
bool  IsSingleBitSet (int x);
bool  AllBits (int x, int y);
bool  AnyBits (int x, int y);
int  ExtractBit (int & x);
int  ExtractBitLog2 (int & x);
int  BitRotateLeft (int x, uint shift);
int  BitRotateRight (int x, uint shift);
int  FloorPOT (int x);
int  CeilPOT (int x);
int  AlignDown (int x, int align);
int  AlignUp (int x, int align);
bool  IsMultipleOf (int x, int align);
uint  Square (uint x);
uint  Min (uint x, uint y);
uint  Max (uint x, uint y);
uint  Clamp (uint x, uint min, uint max);
uint  Wrap (uint x, uint min, uint max);
uint  Average (uint x, uint y);
int  IntLog2 (uint x);
int  CeilIntLog2 (uint x);
int  BitScanReverse (uint x);
int  BitScanForward (uint x);
uint  BitCount (uint x);
bool  IsPowerOfTwo (uint x);
bool  IsSingleBitSet (uint x);
bool  AllBits (uint x, uint y);
bool  AnyBits (uint x, uint y);
uint  ExtractBit (uint & x);
uint  ExtractBitLog2 (uint & x);
uint  BitRotateLeft (uint x, uint shift);
uint  BitRotateRight (uint x, uint shift);
uint  FloorPOT (uint x);
uint  CeilPOT (uint x);
uint  AlignDown (uint x, uint align);
uint  AlignUp (uint x, uint align);
bool  IsMultipleOf (uint x, uint align);
float  Abs (float x);
float  MirroredWrap (float x, float min, float max);
float  Square (float x);
float  Min (float x, float y);
float  Max (float x, float y);
float  Clamp (float x, float min, float max);
float  Wrap (float x, float min, float max);
float  Average (float x, float y);
float  Floor (float x);
float  Ceil (float x);
float  Trunc (float x);
float  Fract (float x);
float  Round (float x);
float  Mod (float x, float y);
float  Sqrt (float x);
float  Pow (float x, float pow);
float  Ln (float x);
float  Log (float x, float base);
float  Log2 (float x);
float  Log10 (float x);
float  Exp (float x);
float  Exp2 (float x);
float  Exp10 (float x);
float  ExpMinus1 (float x);
float  Sin (float x);
float  SinH (float x);
float  ASin (float x);
float  Cos (float x);
float  CosH (float x);
float  ACos (float x);
float  Tan (float x);
float  TanH (float x);
float  ATan (float y, float x);
float  ToRad (float x);
float  ToDeg (float x);
float  Lerp (float x, float y, float factor);
float  ToSNorm (float x);
float  ToUNorm (float x);
float  Remap (float srcMin, float srcMax, float dstMin, float dstMax, float x);
float  RemapClamped (float srcMin, float srcMax, float dstMin, float dstMax, float x);
int  RoundToInt (float x);
uint  RoundToUint (float x);
float  IsInfinity (float x);
float  IsNaN (float x);
float  IsFinite (float x);
float  Pi ();
struct bool2
{
    bool2 ();
    bool2 (const bool2&);
    bool2&  operator = (const bool2&);
    bool x;
    bool y;
    bool2 (bool v);
    bool2 (const bool3 & v3);
    bool2 (const bool4 & v4);
    bool2 (bool x, bool y);
    bool2 (const short2 & v2);
    bool2 (const short3 & v3);
    bool2 (const short4 & v4);
    bool2 (const ushort2 & v2);
    bool2 (const ushort3 & v3);
    bool2 (const ushort4 & v4);
    bool2 (const int2 & v2);
    bool2 (const int3 & v3);
    bool2 (const int4 & v4);
    bool2 (const uint2 & v2);
    bool2 (const uint3 & v3);
    bool2 (const uint4 & v4);
    bool2 (const float2 & v2);
    bool2 (const float3 & v3);
    bool2 (const float4 & v4);
};

struct bool3
{
    bool3 ();
    bool3 (const bool3&);
    bool3&  operator = (const bool3&);
    bool x;
    bool y;
    bool z;
    bool3 (bool v);
    bool3 (const bool2 & v2);
    bool3 (const bool4 & v4);
    bool3 (const bool2 & xy, bool z);
    bool3 (bool x, bool y, bool z);
    bool3 (const short2 & v2);
    bool3 (const short3 & v3);
    bool3 (const short4 & v4);
    bool3 (const ushort2 & v2);
    bool3 (const ushort3 & v3);
    bool3 (const ushort4 & v4);
    bool3 (const int2 & v2);
    bool3 (const int3 & v3);
    bool3 (const int4 & v4);
    bool3 (const uint2 & v2);
    bool3 (const uint3 & v3);
    bool3 (const uint4 & v4);
    bool3 (const float2 & v2);
    bool3 (const float3 & v3);
    bool3 (const float4 & v4);
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
    bool4 (bool v);
    bool4 (const bool2 & v2);
    bool4 (const bool3 & v3);
    bool4 (const bool2 & xy, const bool2 & zw);
    bool4 (const bool3 & xyz, bool w);
    bool4 (bool x, bool y, bool z, bool w);
    bool4 (bool x, const bool3 & yzw);
    bool4 (const short2 & v2);
    bool4 (const short3 & v3);
    bool4 (const short4 & v4);
    bool4 (const ushort2 & v2);
    bool4 (const ushort3 & v3);
    bool4 (const ushort4 & v4);
    bool4 (const int2 & v2);
    bool4 (const int3 & v3);
    bool4 (const int4 & v4);
    bool4 (const uint2 & v2);
    bool4 (const uint3 & v3);
    bool4 (const uint4 & v4);
    bool4 (const float2 & v2);
    bool4 (const float3 & v3);
    bool4 (const float4 & v4);
};

struct sbyte2
{
    sbyte2 ();
    sbyte2 (const sbyte2&);
    sbyte2&  operator = (const sbyte2&);
    int8 x;
    int8 y;
    sbyte2 (int8 v);
    sbyte2 (const sbyte3 & v3);
    sbyte2 (const sbyte4 & v4);
    sbyte2 (int8 x, int8 y);
    sbyte2 (const short2 & v2);
    sbyte2 (const short3 & v3);
    sbyte2 (const short4 & v4);
    sbyte2 (const ushort2 & v2);
    sbyte2 (const ushort3 & v3);
    sbyte2 (const ushort4 & v4);
    sbyte2 (const int2 & v2);
    sbyte2 (const int3 & v3);
    sbyte2 (const int4 & v4);
    sbyte2 (const uint2 & v2);
    sbyte2 (const uint3 & v3);
    sbyte2 (const uint4 & v4);
    sbyte2 (const float2 & v2);
    sbyte2 (const float3 & v3);
    sbyte2 (const float4 & v4);
};

struct sbyte3
{
    sbyte3 ();
    sbyte3 (const sbyte3&);
    sbyte3&  operator = (const sbyte3&);
    int8 x;
    int8 y;
    int8 z;
    sbyte3 (int8 v);
    sbyte3 (const sbyte2 & v2);
    sbyte3 (const sbyte4 & v4);
    sbyte3 (const sbyte2 & xy, int8 z);
    sbyte3 (int8 x, int8 y, int8 z);
    sbyte3 (const short2 & v2);
    sbyte3 (const short3 & v3);
    sbyte3 (const short4 & v4);
    sbyte3 (const ushort2 & v2);
    sbyte3 (const ushort3 & v3);
    sbyte3 (const ushort4 & v4);
    sbyte3 (const int2 & v2);
    sbyte3 (const int3 & v3);
    sbyte3 (const int4 & v4);
    sbyte3 (const uint2 & v2);
    sbyte3 (const uint3 & v3);
    sbyte3 (const uint4 & v4);
    sbyte3 (const float2 & v2);
    sbyte3 (const float3 & v3);
    sbyte3 (const float4 & v4);
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
    sbyte4 (int8 v);
    sbyte4 (const sbyte2 & v2);
    sbyte4 (const sbyte3 & v3);
    sbyte4 (const sbyte2 & xy, const sbyte2 & zw);
    sbyte4 (const sbyte3 & xyz, int8 w);
    sbyte4 (int8 x, int8 y, int8 z, int8 w);
    sbyte4 (int8 x, const sbyte3 & yzw);
    sbyte4 (const short2 & v2);
    sbyte4 (const short3 & v3);
    sbyte4 (const short4 & v4);
    sbyte4 (const ushort2 & v2);
    sbyte4 (const ushort3 & v3);
    sbyte4 (const ushort4 & v4);
    sbyte4 (const int2 & v2);
    sbyte4 (const int3 & v3);
    sbyte4 (const int4 & v4);
    sbyte4 (const uint2 & v2);
    sbyte4 (const uint3 & v3);
    sbyte4 (const uint4 & v4);
    sbyte4 (const float2 & v2);
    sbyte4 (const float3 & v3);
    sbyte4 (const float4 & v4);
};

struct ubyte2
{
    ubyte2 ();
    ubyte2 (const ubyte2&);
    ubyte2&  operator = (const ubyte2&);
    uint8 x;
    uint8 y;
    ubyte2 (uint8 v);
    ubyte2 (const ubyte3 & v3);
    ubyte2 (const ubyte4 & v4);
    ubyte2 (uint8 x, uint8 y);
    ubyte2 (const short2 & v2);
    ubyte2 (const short3 & v3);
    ubyte2 (const short4 & v4);
    ubyte2 (const ushort2 & v2);
    ubyte2 (const ushort3 & v3);
    ubyte2 (const ushort4 & v4);
    ubyte2 (const int2 & v2);
    ubyte2 (const int3 & v3);
    ubyte2 (const int4 & v4);
    ubyte2 (const uint2 & v2);
    ubyte2 (const uint3 & v3);
    ubyte2 (const uint4 & v4);
    ubyte2 (const float2 & v2);
    ubyte2 (const float3 & v3);
    ubyte2 (const float4 & v4);
};

struct ubyte3
{
    ubyte3 ();
    ubyte3 (const ubyte3&);
    ubyte3&  operator = (const ubyte3&);
    uint8 x;
    uint8 y;
    uint8 z;
    ubyte3 (uint8 v);
    ubyte3 (const ubyte2 & v2);
    ubyte3 (const ubyte4 & v4);
    ubyte3 (const ubyte2 & xy, uint8 z);
    ubyte3 (uint8 x, uint8 y, uint8 z);
    ubyte3 (const short2 & v2);
    ubyte3 (const short3 & v3);
    ubyte3 (const short4 & v4);
    ubyte3 (const ushort2 & v2);
    ubyte3 (const ushort3 & v3);
    ubyte3 (const ushort4 & v4);
    ubyte3 (const int2 & v2);
    ubyte3 (const int3 & v3);
    ubyte3 (const int4 & v4);
    ubyte3 (const uint2 & v2);
    ubyte3 (const uint3 & v3);
    ubyte3 (const uint4 & v4);
    ubyte3 (const float2 & v2);
    ubyte3 (const float3 & v3);
    ubyte3 (const float4 & v4);
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
    ubyte4 (uint8 v);
    ubyte4 (const ubyte2 & v2);
    ubyte4 (const ubyte3 & v3);
    ubyte4 (const ubyte2 & xy, const ubyte2 & zw);
    ubyte4 (const ubyte3 & xyz, uint8 w);
    ubyte4 (uint8 x, uint8 y, uint8 z, uint8 w);
    ubyte4 (uint8 x, const ubyte3 & yzw);
    ubyte4 (const short2 & v2);
    ubyte4 (const short3 & v3);
    ubyte4 (const short4 & v4);
    ubyte4 (const ushort2 & v2);
    ubyte4 (const ushort3 & v3);
    ubyte4 (const ushort4 & v4);
    ubyte4 (const int2 & v2);
    ubyte4 (const int3 & v3);
    ubyte4 (const int4 & v4);
    ubyte4 (const uint2 & v2);
    ubyte4 (const uint3 & v3);
    ubyte4 (const uint4 & v4);
    ubyte4 (const float2 & v2);
    ubyte4 (const float3 & v3);
    ubyte4 (const float4 & v4);
};

struct short2
{
    short2 ();
    short2 (const short2&);
    short2&  operator = (const short2&);
    int16 x;
    int16 y;
    short2 (int16 v);
    short2 (const short3 & v3);
    short2 (const short4 & v4);
    short2 (int16 x, int16 y);
    short2 (const ushort2 & v2);
    short2 (const ushort3 & v3);
    short2 (const ushort4 & v4);
    short2 (const int2 & v2);
    short2 (const int3 & v3);
    short2 (const int4 & v4);
    short2 (const uint2 & v2);
    short2 (const uint3 & v3);
    short2 (const uint4 & v4);
    short2 (const float2 & v2);
    short2 (const float3 & v3);
    short2 (const float4 & v4);
};

struct short3
{
    short3 ();
    short3 (const short3&);
    short3&  operator = (const short3&);
    int16 x;
    int16 y;
    int16 z;
    short3 (int16 v);
    short3 (const short2 & v2);
    short3 (const short4 & v4);
    short3 (const short2 & xy, int16 z);
    short3 (int16 x, int16 y, int16 z);
    short3 (const ushort2 & v2);
    short3 (const ushort3 & v3);
    short3 (const ushort4 & v4);
    short3 (const int2 & v2);
    short3 (const int3 & v3);
    short3 (const int4 & v4);
    short3 (const uint2 & v2);
    short3 (const uint3 & v3);
    short3 (const uint4 & v4);
    short3 (const float2 & v2);
    short3 (const float3 & v3);
    short3 (const float4 & v4);
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
    short4 (int16 v);
    short4 (const short2 & v2);
    short4 (const short3 & v3);
    short4 (const short2 & xy, const short2 & zw);
    short4 (const short3 & xyz, int16 w);
    short4 (int16 x, int16 y, int16 z, int16 w);
    short4 (int16 x, const short3 & yzw);
    short4 (const ushort2 & v2);
    short4 (const ushort3 & v3);
    short4 (const ushort4 & v4);
    short4 (const int2 & v2);
    short4 (const int3 & v3);
    short4 (const int4 & v4);
    short4 (const uint2 & v2);
    short4 (const uint3 & v3);
    short4 (const uint4 & v4);
    short4 (const float2 & v2);
    short4 (const float3 & v3);
    short4 (const float4 & v4);
};

struct ushort2
{
    ushort2 ();
    ushort2 (const ushort2&);
    ushort2&  operator = (const ushort2&);
    uint16 x;
    uint16 y;
    ushort2 (uint16 v);
    ushort2 (const ushort3 & v3);
    ushort2 (const ushort4 & v4);
    ushort2 (uint16 x, uint16 y);
    ushort2 (const short2 & v2);
    ushort2 (const short3 & v3);
    ushort2 (const short4 & v4);
    ushort2 (const int2 & v2);
    ushort2 (const int3 & v3);
    ushort2 (const int4 & v4);
    ushort2 (const uint2 & v2);
    ushort2 (const uint3 & v3);
    ushort2 (const uint4 & v4);
    ushort2 (const float2 & v2);
    ushort2 (const float3 & v3);
    ushort2 (const float4 & v4);
};

struct ushort3
{
    ushort3 ();
    ushort3 (const ushort3&);
    ushort3&  operator = (const ushort3&);
    uint16 x;
    uint16 y;
    uint16 z;
    ushort3 (uint16 v);
    ushort3 (const ushort2 & v2);
    ushort3 (const ushort4 & v4);
    ushort3 (const ushort2 & xy, uint16 z);
    ushort3 (uint16 x, uint16 y, uint16 z);
    ushort3 (const short2 & v2);
    ushort3 (const short3 & v3);
    ushort3 (const short4 & v4);
    ushort3 (const int2 & v2);
    ushort3 (const int3 & v3);
    ushort3 (const int4 & v4);
    ushort3 (const uint2 & v2);
    ushort3 (const uint3 & v3);
    ushort3 (const uint4 & v4);
    ushort3 (const float2 & v2);
    ushort3 (const float3 & v3);
    ushort3 (const float4 & v4);
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
    ushort4 (uint16 v);
    ushort4 (const ushort2 & v2);
    ushort4 (const ushort3 & v3);
    ushort4 (const ushort2 & xy, const ushort2 & zw);
    ushort4 (const ushort3 & xyz, uint16 w);
    ushort4 (uint16 x, uint16 y, uint16 z, uint16 w);
    ushort4 (uint16 x, const ushort3 & yzw);
    ushort4 (const short2 & v2);
    ushort4 (const short3 & v3);
    ushort4 (const short4 & v4);
    ushort4 (const int2 & v2);
    ushort4 (const int3 & v3);
    ushort4 (const int4 & v4);
    ushort4 (const uint2 & v2);
    ushort4 (const uint3 & v3);
    ushort4 (const uint4 & v4);
    ushort4 (const float2 & v2);
    ushort4 (const float3 & v3);
    ushort4 (const float4 & v4);
};

struct int2
{
    int2 ();
    int2 (const int2&);
    int2&  operator = (const int2&);
    int x;
    int y;
    int2 (int v);
    int2 (const int3 & v3);
    int2 (const int4 & v4);
    int2 (int x, int y);
    int2 (const short2 & v2);
    int2 (const short3 & v3);
    int2 (const short4 & v4);
    int2 (const ushort2 & v2);
    int2 (const ushort3 & v3);
    int2 (const ushort4 & v4);
    int2 (const uint2 & v2);
    int2 (const uint3 & v3);
    int2 (const uint4 & v4);
    int2 (const float2 & v2);
    int2 (const float3 & v3);
    int2 (const float4 & v4);
};

struct int3
{
    int3 ();
    int3 (const int3&);
    int3&  operator = (const int3&);
    int x;
    int y;
    int z;
    int3 (int v);
    int3 (const int2 & v2);
    int3 (const int4 & v4);
    int3 (const int2 & xy, int z);
    int3 (int x, int y, int z);
    int3 (const short2 & v2);
    int3 (const short3 & v3);
    int3 (const short4 & v4);
    int3 (const ushort2 & v2);
    int3 (const ushort3 & v3);
    int3 (const ushort4 & v4);
    int3 (const uint2 & v2);
    int3 (const uint3 & v3);
    int3 (const uint4 & v4);
    int3 (const float2 & v2);
    int3 (const float3 & v3);
    int3 (const float4 & v4);
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
    int4 (int v);
    int4 (const int2 & v2);
    int4 (const int3 & v3);
    int4 (const int2 & xy, const int2 & zw);
    int4 (const int3 & xyz, int w);
    int4 (int x, int y, int z, int w);
    int4 (int x, const int3 & yzw);
    int4 (const short2 & v2);
    int4 (const short3 & v3);
    int4 (const short4 & v4);
    int4 (const ushort2 & v2);
    int4 (const ushort3 & v3);
    int4 (const ushort4 & v4);
    int4 (const uint2 & v2);
    int4 (const uint3 & v3);
    int4 (const uint4 & v4);
    int4 (const float2 & v2);
    int4 (const float3 & v3);
    int4 (const float4 & v4);
};

struct uint2
{
    uint2 ();
    uint2 (const uint2&);
    uint2&  operator = (const uint2&);
    uint x;
    uint y;
    uint2 (uint v);
    uint2 (const uint3 & v3);
    uint2 (const uint4 & v4);
    uint2 (uint x, uint y);
    uint2 (const short2 & v2);
    uint2 (const short3 & v3);
    uint2 (const short4 & v4);
    uint2 (const ushort2 & v2);
    uint2 (const ushort3 & v3);
    uint2 (const ushort4 & v4);
    uint2 (const int2 & v2);
    uint2 (const int3 & v3);
    uint2 (const int4 & v4);
    uint2 (const float2 & v2);
    uint2 (const float3 & v3);
    uint2 (const float4 & v4);
};

struct uint3
{
    uint3 ();
    uint3 (const uint3&);
    uint3&  operator = (const uint3&);
    uint x;
    uint y;
    uint z;
    uint3 (uint v);
    uint3 (const uint2 & v2);
    uint3 (const uint4 & v4);
    uint3 (const uint2 & xy, uint z);
    uint3 (uint x, uint y, uint z);
    uint3 (const short2 & v2);
    uint3 (const short3 & v3);
    uint3 (const short4 & v4);
    uint3 (const ushort2 & v2);
    uint3 (const ushort3 & v3);
    uint3 (const ushort4 & v4);
    uint3 (const int2 & v2);
    uint3 (const int3 & v3);
    uint3 (const int4 & v4);
    uint3 (const float2 & v2);
    uint3 (const float3 & v3);
    uint3 (const float4 & v4);
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
    uint4 (uint v);
    uint4 (const uint2 & v2);
    uint4 (const uint3 & v3);
    uint4 (const uint2 & xy, const uint2 & zw);
    uint4 (const uint3 & xyz, uint w);
    uint4 (uint x, uint y, uint z, uint w);
    uint4 (uint x, const uint3 & yzw);
    uint4 (const short2 & v2);
    uint4 (const short3 & v3);
    uint4 (const short4 & v4);
    uint4 (const ushort2 & v2);
    uint4 (const ushort3 & v3);
    uint4 (const ushort4 & v4);
    uint4 (const int2 & v2);
    uint4 (const int3 & v3);
    uint4 (const int4 & v4);
    uint4 (const float2 & v2);
    uint4 (const float3 & v3);
    uint4 (const float4 & v4);
};

struct float2
{
    float2 ();
    float2 (const float2&);
    float2&  operator = (const float2&);
    float x;
    float y;
    float2 (float v);
    float2 (const float3 & v3);
    float2 (const float4 & v4);
    float2 (float x, float y);
    float2 (const short2 & v2);
    float2 (const short3 & v3);
    float2 (const short4 & v4);
    float2 (const ushort2 & v2);
    float2 (const ushort3 & v3);
    float2 (const ushort4 & v4);
    float2 (const int2 & v2);
    float2 (const int3 & v3);
    float2 (const int4 & v4);
    float2 (const uint2 & v2);
    float2 (const uint3 & v3);
    float2 (const uint4 & v4);
};

struct float3
{
    float3 ();
    float3 (const float3&);
    float3&  operator = (const float3&);
    float x;
    float y;
    float z;
    float3 (float v);
    float3 (const float2 & v2);
    float3 (const float4 & v4);
    float3 (const float2 & xy, float z);
    float3 (float x, float y, float z);
    float3 (const short2 & v2);
    float3 (const short3 & v3);
    float3 (const short4 & v4);
    float3 (const ushort2 & v2);
    float3 (const ushort3 & v3);
    float3 (const ushort4 & v4);
    float3 (const int2 & v2);
    float3 (const int3 & v3);
    float3 (const int4 & v4);
    float3 (const uint2 & v2);
    float3 (const uint3 & v3);
    float3 (const uint4 & v4);
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
    float4 (float v);
    float4 (const float2 & v2);
    float4 (const float3 & v3);
    float4 (const float2 & xy, const float2 & zw);
    float4 (const float3 & xyz, float w);
    float4 (float x, float y, float z, float w);
    float4 (float x, const float3 & yzw);
    float4 (const short2 & v2);
    float4 (const short3 & v3);
    float4 (const short4 & v4);
    float4 (const ushort2 & v2);
    float4 (const ushort3 & v3);
    float4 (const ushort4 & v4);
    float4 (const int2 & v2);
    float4 (const int3 & v3);
    float4 (const int4 & v4);
    float4 (const uint2 & v2);
    float4 (const uint3 & v3);
    float4 (const uint4 & v4);
    float4 (const RGBA32f & x);
};

bool  All (const bool2 & x);
bool  Any (const bool2 & x);
bool  All (const bool3 & x);
bool  Any (const bool3 & x);
bool  All (const bool4 & x);
bool  Any (const bool4 & x);
int2  Abs (const int2 & x);
int2  MirroredWrap (const int2 & x, const int2 & min, const int2 & max);
int2  MirroredWrap (const int2 & x, int min, int max);
int  Sum (const int2 & x);
int  Area (const int2 & x);
int2  Min (const int2 & x, const int2 & y);
int2  Max (const int2 & x, const int2 & y);
int2  Clamp (const int2 & val, const int2 & min, const int2 & max);
int2  Clamp (const int2 & val, int min, int max);
int2  Wrap (const int2 & val, const int2 & min, const int2 & max);
int2  Wrap (const int2 & val, int min, int max);
int  VecToLinear (const int2 & pos, const int2 & dim);
int3  Abs (const int3 & x);
int3  MirroredWrap (const int3 & x, const int3 & min, const int3 & max);
int3  MirroredWrap (const int3 & x, int min, int max);
int  Sum (const int3 & x);
int  Area (const int3 & x);
int3  Min (const int3 & x, const int3 & y);
int3  Max (const int3 & x, const int3 & y);
int3  Clamp (const int3 & val, const int3 & min, const int3 & max);
int3  Clamp (const int3 & val, int min, int max);
int3  Wrap (const int3 & val, const int3 & min, const int3 & max);
int3  Wrap (const int3 & val, int min, int max);
int  VecToLinear (const int3 & pos, const int3 & dim);
int4  Abs (const int4 & x);
int4  MirroredWrap (const int4 & x, const int4 & min, const int4 & max);
int4  MirroredWrap (const int4 & x, int min, int max);
int  Sum (const int4 & x);
int  Area (const int4 & x);
int4  Min (const int4 & x, const int4 & y);
int4  Max (const int4 & x, const int4 & y);
int4  Clamp (const int4 & val, const int4 & min, const int4 & max);
int4  Clamp (const int4 & val, int min, int max);
int4  Wrap (const int4 & val, const int4 & min, const int4 & max);
int4  Wrap (const int4 & val, int min, int max);
int  VecToLinear (const int4 & pos, const int4 & dim);
uint  Sum (const uint2 & x);
uint  Area (const uint2 & x);
uint2  Min (const uint2 & x, const uint2 & y);
uint2  Max (const uint2 & x, const uint2 & y);
uint2  Clamp (const uint2 & val, const uint2 & min, const uint2 & max);
uint2  Clamp (const uint2 & val, uint min, uint max);
uint2  Wrap (const uint2 & val, const uint2 & min, const uint2 & max);
uint2  Wrap (const uint2 & val, uint min, uint max);
uint  VecToLinear (const uint2 & pos, const uint2 & dim);
uint  Sum (const uint3 & x);
uint  Area (const uint3 & x);
uint3  Min (const uint3 & x, const uint3 & y);
uint3  Max (const uint3 & x, const uint3 & y);
uint3  Clamp (const uint3 & val, const uint3 & min, const uint3 & max);
uint3  Clamp (const uint3 & val, uint min, uint max);
uint3  Wrap (const uint3 & val, const uint3 & min, const uint3 & max);
uint3  Wrap (const uint3 & val, uint min, uint max);
uint  VecToLinear (const uint3 & pos, const uint3 & dim);
uint  Sum (const uint4 & x);
uint  Area (const uint4 & x);
uint4  Min (const uint4 & x, const uint4 & y);
uint4  Max (const uint4 & x, const uint4 & y);
uint4  Clamp (const uint4 & val, const uint4 & min, const uint4 & max);
uint4  Clamp (const uint4 & val, uint min, uint max);
uint4  Wrap (const uint4 & val, const uint4 & min, const uint4 & max);
uint4  Wrap (const uint4 & val, uint min, uint max);
uint  VecToLinear (const uint4 & pos, const uint4 & dim);
float2  Abs (const float2 & x);
float2  MirroredWrap (const float2 & x, const float2 & min, const float2 & max);
float2  MirroredWrap (const float2 & x, float min, float max);
float  Sum (const float2 & x);
float  Area (const float2 & x);
float2  Min (const float2 & x, const float2 & y);
float2  Max (const float2 & x, const float2 & y);
float2  Clamp (const float2 & val, const float2 & min, const float2 & max);
float2  Clamp (const float2 & val, float min, float max);
float2  Wrap (const float2 & val, const float2 & min, const float2 & max);
float2  Wrap (const float2 & val, float min, float max);
float  VecToLinear (const float2 & pos, const float2 & dim);
float2  ToSNorm (const float2 & x);
float2  ToUNorm (const float2 & x);
float  Dot (const float2 & x, const float2 & y);
float2  Lerp (const float2 & x, const float2 & y, float factor);
float2  Normalize (const float2 & x);
float2  Floor (const float2 & x);
float2  Ceil (const float2 & x);
float2  Round (const float2 & x);
float2  Fract (const float2 & x);
float2  Sqrt (const float2 & x);
float2  Ln (const float2 & x);
float2  Log2 (const float2 & x);
float2  Log (const float2 & x, const float2 & base);
float2  Log (const float2 & x, float base);
float2  Pow (const float2 & x, const float2 & pow);
float2  Pow (const float2 & x, float pow);
float2  Exp (const float2 & x);
float2  Exp2 (const float2 & x);
float2  Exp10 (const float2 & x);
float  Length (const float2 & x);
float  LengthSq (const float2 & x);
float  Distance (const float2 & x, const float2 & y);
float  DistanceSq (const float2 & x, const float2 & y);
float3  Abs (const float3 & x);
float3  MirroredWrap (const float3 & x, const float3 & min, const float3 & max);
float3  MirroredWrap (const float3 & x, float min, float max);
float  Sum (const float3 & x);
float  Area (const float3 & x);
float3  Min (const float3 & x, const float3 & y);
float3  Max (const float3 & x, const float3 & y);
float3  Clamp (const float3 & val, const float3 & min, const float3 & max);
float3  Clamp (const float3 & val, float min, float max);
float3  Wrap (const float3 & val, const float3 & min, const float3 & max);
float3  Wrap (const float3 & val, float min, float max);
float  VecToLinear (const float3 & pos, const float3 & dim);
float3  Cross (const float3 & x, const float3 & y);
float3  ToSNorm (const float3 & x);
float3  ToUNorm (const float3 & x);
float  Dot (const float3 & x, const float3 & y);
float3  Lerp (const float3 & x, const float3 & y, float factor);
float3  Normalize (const float3 & x);
float3  Floor (const float3 & x);
float3  Ceil (const float3 & x);
float3  Round (const float3 & x);
float3  Fract (const float3 & x);
float3  Sqrt (const float3 & x);
float3  Ln (const float3 & x);
float3  Log2 (const float3 & x);
float3  Log (const float3 & x, const float3 & base);
float3  Log (const float3 & x, float base);
float3  Pow (const float3 & x, const float3 & pow);
float3  Pow (const float3 & x, float pow);
float3  Exp (const float3 & x);
float3  Exp2 (const float3 & x);
float3  Exp10 (const float3 & x);
float  Length (const float3 & x);
float  LengthSq (const float3 & x);
float  Distance (const float3 & x, const float3 & y);
float  DistanceSq (const float3 & x, const float3 & y);
float4  Abs (const float4 & x);
float4  MirroredWrap (const float4 & x, const float4 & min, const float4 & max);
float4  MirroredWrap (const float4 & x, float min, float max);
float  Sum (const float4 & x);
float  Area (const float4 & x);
float4  Min (const float4 & x, const float4 & y);
float4  Max (const float4 & x, const float4 & y);
float4  Clamp (const float4 & val, const float4 & min, const float4 & max);
float4  Clamp (const float4 & val, float min, float max);
float4  Wrap (const float4 & val, const float4 & min, const float4 & max);
float4  Wrap (const float4 & val, float min, float max);
float  VecToLinear (const float4 & pos, const float4 & dim);
float4  ToSNorm (const float4 & x);
float4  ToUNorm (const float4 & x);
float  Dot (const float4 & x, const float4 & y);
float4  Lerp (const float4 & x, const float4 & y, float factor);
float4  Normalize (const float4 & x);
float4  Floor (const float4 & x);
float4  Ceil (const float4 & x);
float4  Round (const float4 & x);
float4  Fract (const float4 & x);
float4  Sqrt (const float4 & x);
float4  Ln (const float4 & x);
float4  Log2 (const float4 & x);
float4  Log (const float4 & x, const float4 & base);
float4  Log (const float4 & x, float base);
float4  Pow (const float4 & x, const float4 & pow);
float4  Pow (const float4 & x, float pow);
float4  Exp (const float4 & x);
float4  Exp2 (const float4 & x);
float4  Exp10 (const float4 & x);
float  Length (const float4 & x);
float  LengthSq (const float4 & x);
float  Distance (const float4 & x, const float4 & y);
float  DistanceSq (const float4 & x, const float4 & y);
struct RectI
{
    RectI ();
    RectI (const RectI&);
    RectI&  operator = (const RectI&);
    int left;
    int top;
    int right;
    int bottom;
    RectI (const RectI & value);
    RectI (const int2 & value);
    RectI (const int2 & leftTop, const int2 & rightBottom);
    RectI (int sizeX, int sizeY);
    RectI (int left, int top, int right, int bottom);
};

struct RectU
{
    RectU ();
    RectU (const RectU&);
    RectU&  operator = (const RectU&);
    uint left;
    uint top;
    uint right;
    uint bottom;
    RectU (const RectU & value);
    RectU (const uint2 & value);
    RectU (const uint2 & leftTop, const uint2 & rightBottom);
    RectU (uint sizeX, uint sizeY);
    RectU (uint left, uint top, uint right, uint bottom);
};

struct RectF
{
    RectF ();
    RectF (const RectF&);
    RectF&  operator = (const RectF&);
    float left;
    float top;
    float right;
    float bottom;
    RectF (const RectF & value);
    RectF (const float2 & value);
    RectF (const float2 & leftTop, const float2 & rightBottom);
    RectF (float sizeX, float sizeY);
    RectF (float left, float top, float right, float bottom);
};

RGBA32f  Lerp (const RGBA32f & x, const RGBA32f & y, float factor);
RGBA32f  AdjustContrast (const RGBA32f & col, float factor);
float  Luminance (const RGBA32f & col);
RGBA32f  Rainbow (float factor);
RGBA32f  RainbowWrap (float factor);
struct RGBA32f
{
    RGBA32f ();
    RGBA32f (const RGBA32f&);
    RGBA32f&  operator = (const RGBA32f&);
    RGBA32f (float value);
    RGBA32f (float r, float g, float b, float a);
    float r;
    float g;
    float b;
    float a;
    RGBA32f (const HSVColor & hsv, float alpha);
    RGBA32f (const RGBA8u & c);
    RGBA32f (const RGBA32u & c);
};

struct RGBA32i
{
    RGBA32i ();
    RGBA32i (const RGBA32i&);
    RGBA32i&  operator = (const RGBA32i&);
    RGBA32i (int value);
    RGBA32i (int r, int g, int b, int a);
    int r;
    int g;
    int b;
    int a;
    RGBA32i (const RGBA8u & c);
    RGBA32i (const RGBA32u & c);
};

struct RGBA32u
{
    RGBA32u ();
    RGBA32u (const RGBA32u&);
    RGBA32u&  operator = (const RGBA32u&);
    RGBA32u (uint value);
    RGBA32u (uint r, uint g, uint b, uint a);
    uint r;
    uint g;
    uint b;
    uint a;
    RGBA32u (const RGBA8u & c);
    RGBA32u (const RGBA32i & c);
};

RGBA8u  AdjustContrast (const RGBA8u & col, float factor);
float  Luminance (const RGBA8u & col);
RGBA8u  AdjustSaturation (const RGBA8u & col, float factor);
RGBA8u  Lerp (const RGBA8u & x, const RGBA8u & y, float factor);
struct RGBA8u
{
    RGBA8u ();
    RGBA8u (const RGBA8u&);
    RGBA8u&  operator = (const RGBA8u&);
    RGBA8u (uint8 value);
    RGBA8u (uint8 r, uint8 g, uint8 b, uint8 a);
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 a;
    RGBA8u (const RGBA32i & c);
    RGBA8u (const RGBA32u & c);
    RGBA8u (const RGBA32f & c);
};

struct HSVColor
{
    HSVColor ();
    HSVColor (const HSVColor&);
    HSVColor&  operator = (const HSVColor&);
    float h;
    float s;
    float v;
    HSVColor (float h);
    HSVColor (float h, float s);
    HSVColor (float h, float s, float v);
    HSVColor (const RGBA32f & col);
};

struct DepthStencil
{
    DepthStencil ();
    DepthStencil (const DepthStencil&);
    DepthStencil&  operator = (const DepthStencil&);
    float depth;
    uint stencil;
    DepthStencil (float depth, int stencil);
};

RGBA8u  asARGB (uint);
RGBA8u  asRGBA (uint);
string  ToString (int value);
string  ToString (uint value);
string  ToString (int64 value);
string  ToString (uint64 value);
string  ToString (float value);
string  ToString (double value);
string  ToString (float value, uint fractParts);
string  ToString (double value, uint fractParts);
string  ToString (const bool2 & value);
string  ToString (const bool3 & value);
string  ToString (const bool4 & value);
string  ToString (const sbyte2 & value);
string  ToString (const sbyte3 & value);
string  ToString (const sbyte4 & value);
string  ToString (const ubyte2 & value);
string  ToString (const ubyte3 & value);
string  ToString (const ubyte4 & value);
string  ToString (const short2 & value);
string  ToString (const short3 & value);
string  ToString (const short4 & value);
string  ToString (const ushort2 & value);
string  ToString (const ushort3 & value);
string  ToString (const ushort4 & value);
string  ToString (const int2 & value);
string  ToString (const int3 & value);
string  ToString (const int4 & value);
string  ToString (const uint2 & value);
string  ToString (const uint3 & value);
string  ToString (const uint4 & value);
string  ToString (const float2 & value);
string  ToString (const float3 & value);
string  ToString (const float4 & value);
string  ToString (const RectU & value);
string  ToString (const RectI & value);
string  ToString (const RectF & value);
string  ToString (const RGBA32f & value);
string  ToString (const RGBA32i & value);
string  ToString (const RGBA32u & value);
string  ToString (const RGBA8u & value);
void  LogError (const string & msg);
void  LogInfo (const string & msg);
void  LogDebug (const string & msg);
void  LogFatal (const string & msg);
void  Assert (bool expr);
void  Assert (bool expr, const string & msg);
struct EImage
{
    EImage () {}
    EImage (uint8) {}
    operator uint8 () const;
    static constexpr uint8 1D = 0;
    static constexpr uint8 2D = 1;
    static constexpr uint8 3D = 2;
    static constexpr uint8 1DArray = 3;
    static constexpr uint8 2DArray = 4;
    static constexpr uint8 Cube = 5;
    static constexpr uint8 CubeArray = 6;
};

struct EIndex
{
    EIndex () {}
    EIndex (uint8) {}
    operator uint8 () const;
    static constexpr uint8 UShort = 0;
    static constexpr uint8 UInt = 1;
};

struct EPixelFormat
{
    EPixelFormat () {}
    EPixelFormat (uint8) {}
    operator uint8 () const;
    static constexpr uint8 RGBA16_SNorm = 0;
    static constexpr uint8 RGBA8_SNorm = 1;
    static constexpr uint8 RGB16_SNorm = 2;
    static constexpr uint8 RGB8_SNorm = 3;
    static constexpr uint8 RG16_SNorm = 4;
    static constexpr uint8 RG8_SNorm = 5;
    static constexpr uint8 R16_SNorm = 6;
    static constexpr uint8 R8_SNorm = 7;
    static constexpr uint8 RGBA16_UNorm = 8;
    static constexpr uint8 RGBA8_UNorm = 9;
    static constexpr uint8 RGB16_UNorm = 10;
    static constexpr uint8 RGB8_UNorm = 11;
    static constexpr uint8 RG16_UNorm = 12;
    static constexpr uint8 RG8_UNorm = 13;
    static constexpr uint8 R16_UNorm = 14;
    static constexpr uint8 R8_UNorm = 15;
    static constexpr uint8 RGB10_A2_UNorm = 16;
    static constexpr uint8 RGBA4_UNorm = 17;
    static constexpr uint8 RGB5_A1_UNorm = 18;
    static constexpr uint8 RGB_5_6_5_UNorm = 19;
    static constexpr uint8 BGR8_UNorm = 20;
    static constexpr uint8 BGRA8_UNorm = 21;
    static constexpr uint8 sRGB8 = 22;
    static constexpr uint8 sRGB8_A8 = 23;
    static constexpr uint8 sBGR8 = 24;
    static constexpr uint8 sBGR8_A8 = 25;
    static constexpr uint8 R8I = 26;
    static constexpr uint8 RG8I = 27;
    static constexpr uint8 RGB8I = 28;
    static constexpr uint8 RGBA8I = 29;
    static constexpr uint8 R16I = 30;
    static constexpr uint8 RG16I = 31;
    static constexpr uint8 RGB16I = 32;
    static constexpr uint8 RGBA16I = 33;
    static constexpr uint8 R32I = 34;
    static constexpr uint8 RG32I = 35;
    static constexpr uint8 RGB32I = 36;
    static constexpr uint8 RGBA32I = 37;
    static constexpr uint8 R64I = 38;
    static constexpr uint8 R8U = 39;
    static constexpr uint8 RG8U = 40;
    static constexpr uint8 RGB8U = 41;
    static constexpr uint8 RGBA8U = 42;
    static constexpr uint8 R16U = 43;
    static constexpr uint8 RG16U = 44;
    static constexpr uint8 RGB16U = 45;
    static constexpr uint8 RGBA16U = 46;
    static constexpr uint8 R32U = 47;
    static constexpr uint8 RG32U = 48;
    static constexpr uint8 RGB32U = 49;
    static constexpr uint8 RGBA32U = 50;
    static constexpr uint8 RGB10_A2U = 51;
    static constexpr uint8 R64U = 52;
    static constexpr uint8 R16F = 53;
    static constexpr uint8 RG16F = 54;
    static constexpr uint8 RGB16F = 55;
    static constexpr uint8 RGBA16F = 56;
    static constexpr uint8 R32F = 57;
    static constexpr uint8 RG32F = 58;
    static constexpr uint8 RGB32F = 59;
    static constexpr uint8 RGBA32F = 60;
    static constexpr uint8 RGB_11_11_10F = 61;
    static constexpr uint8 RGB9F_E5 = 62;
    static constexpr uint8 Depth16 = 63;
    static constexpr uint8 Depth24 = 64;
    static constexpr uint8 Depth32F = 65;
    static constexpr uint8 Depth16_Stencil8 = 66;
    static constexpr uint8 Depth24_Stencil8 = 67;
    static constexpr uint8 Depth32F_Stencil8 = 68;
    static constexpr uint8 BC1_RGB8_UNorm = 69;
    static constexpr uint8 BC1_sRGB8 = 70;
    static constexpr uint8 BC1_RGB8_A1_UNorm = 71;
    static constexpr uint8 BC1_sRGB8_A1 = 72;
    static constexpr uint8 BC2_RGBA8_UNorm = 73;
    static constexpr uint8 BC2_sRGB8 = 74;
    static constexpr uint8 BC3_RGBA8_UNorm = 75;
    static constexpr uint8 BC3_sRGB8 = 76;
    static constexpr uint8 BC4_R8_SNorm = 77;
    static constexpr uint8 BC4_R8_UNorm = 78;
    static constexpr uint8 BC5_RG8_SNorm = 79;
    static constexpr uint8 BC5_RG8_UNorm = 80;
    static constexpr uint8 BC6H_RGB16F = 81;
    static constexpr uint8 BC6H_RGB16UF = 82;
    static constexpr uint8 BC7_RGBA8_UNorm = 83;
    static constexpr uint8 BC7_sRGB8_A8 = 84;
    static constexpr uint8 ETC2_RGB8_UNorm = 85;
    static constexpr uint8 ECT2_sRGB8 = 86;
    static constexpr uint8 ETC2_RGB8_A1_UNorm = 87;
    static constexpr uint8 ETC2_sRGB8_A1 = 88;
    static constexpr uint8 ETC2_RGBA8_UNorm = 89;
    static constexpr uint8 ETC2_sRGB8_A8 = 90;
    static constexpr uint8 EAC_R11_SNorm = 91;
    static constexpr uint8 EAC_R11_UNorm = 92;
    static constexpr uint8 EAC_RG11_SNorm = 93;
    static constexpr uint8 EAC_RG11_UNorm = 94;
    static constexpr uint8 ASTC_RGBA_4x4 = 95;
    static constexpr uint8 ASTC_RGBA_5x4 = 96;
    static constexpr uint8 ASTC_RGBA_5x5 = 97;
    static constexpr uint8 ASTC_RGBA_6x5 = 98;
    static constexpr uint8 ASTC_RGBA_6x6 = 99;
    static constexpr uint8 ASTC_RGBA_8x5 = 100;
    static constexpr uint8 ASTC_RGBA_8x6 = 101;
    static constexpr uint8 ASTC_RGBA_8x8 = 102;
    static constexpr uint8 ASTC_RGBA_10x5 = 103;
    static constexpr uint8 ASTC_RGBA_10x6 = 104;
    static constexpr uint8 ASTC_RGBA_10x8 = 105;
    static constexpr uint8 ASTC_RGBA_10x10 = 106;
    static constexpr uint8 ASTC_RGBA_12x10 = 107;
    static constexpr uint8 ASTC_RGBA_12x12 = 108;
    static constexpr uint8 ASTC_sRGB8_A8_4x4 = 109;
    static constexpr uint8 ASTC_sRGB8_A8_5x4 = 110;
    static constexpr uint8 ASTC_sRGB8_A8_5x5 = 111;
    static constexpr uint8 ASTC_sRGB8_A8_6x5 = 112;
    static constexpr uint8 ASTC_sRGB8_A8_6x6 = 113;
    static constexpr uint8 ASTC_sRGB8_A8_8x5 = 114;
    static constexpr uint8 ASTC_sRGB8_A8_8x6 = 115;
    static constexpr uint8 ASTC_sRGB8_A8_8x8 = 116;
    static constexpr uint8 ASTC_sRGB8_A8_10x5 = 117;
    static constexpr uint8 ASTC_sRGB8_A8_10x6 = 118;
    static constexpr uint8 ASTC_sRGB8_A8_10x8 = 119;
    static constexpr uint8 ASTC_sRGB8_A8_10x10 = 120;
    static constexpr uint8 ASTC_sRGB8_A8_12x10 = 121;
    static constexpr uint8 ASTC_sRGB8_A8_12x12 = 122;
    static constexpr uint8 ASTC_RGBA16F_4x4 = 123;
    static constexpr uint8 ASTC_RGBA16F_5x4 = 124;
    static constexpr uint8 ASTC_RGBA16F_5x5 = 125;
    static constexpr uint8 ASTC_RGBA16F_6x5 = 126;
    static constexpr uint8 ASTC_RGBA16F_6x6 = 127;
    static constexpr uint8 ASTC_RGBA16F_8x5 = 128;
    static constexpr uint8 ASTC_RGBA16F_8x6 = 129;
    static constexpr uint8 ASTC_RGBA16F_8x8 = 130;
    static constexpr uint8 ASTC_RGBA16F_10x5 = 131;
    static constexpr uint8 ASTC_RGBA16F_10x6 = 132;
    static constexpr uint8 ASTC_RGBA16F_10x8 = 133;
    static constexpr uint8 ASTC_RGBA16F_10x10 = 134;
    static constexpr uint8 ASTC_RGBA16F_12x10 = 135;
    static constexpr uint8 ASTC_RGBA16F_12x12 = 136;
    static constexpr uint8 G8B8G8R8_422_UNorm = 137;
    static constexpr uint8 B8G8R8G8_422_UNorm = 138;
    static constexpr uint8 G8_B8R8_420_UNorm = 139;
    static constexpr uint8 G8_B8R8_422_UNorm = 140;
    static constexpr uint8 G8_B8_R8_420_UNorm = 141;
    static constexpr uint8 G8_B8_R8_422_UNorm = 142;
    static constexpr uint8 G8_B8_R8_444_UNorm = 143;
    static constexpr uint8 B10x6G10x6R10x6G10x6_422_UNorm = 144;
    static constexpr uint8 G10x6B10x6G10x6R10x6_422_UNorm = 145;
    static constexpr uint8 G10x6_B10x6R10x6_420_UNorm = 146;
    static constexpr uint8 G10x6_B10x6R10x6_422_UNorm = 147;
    static constexpr uint8 G10x6_B10x6_R10x6_420_UNorm = 148;
    static constexpr uint8 G10x6_B10x6_R10x6_422_UNorm = 149;
    static constexpr uint8 G10x6_B10x6_R10x6_444_UNorm = 150;
    static constexpr uint8 R10x6G10x6B10x6A10x6_UNorm = 151;
    static constexpr uint8 R10x6G10x6_UNorm = 152;
    static constexpr uint8 R10x6_UNorm = 153;
    static constexpr uint8 B12x4G12x4R12x4G12x4_422_UNorm = 154;
    static constexpr uint8 G12x4B12x4G12x4R12x4_422_UNorm = 155;
    static constexpr uint8 G12x4_B12x4R12x4_420_UNorm = 156;
    static constexpr uint8 G12x4_B12x4R12x4_422_UNorm = 157;
    static constexpr uint8 G12x4_B12x4_R12x4_420_UNorm = 158;
    static constexpr uint8 G12x4_B12x4_R12x4_422_UNorm = 159;
    static constexpr uint8 G12x4_B12x4_R12x4_444_UNorm = 160;
    static constexpr uint8 R12x4G12x4B12x4A12x4_UNorm = 161;
    static constexpr uint8 R12x4G12x4_UNorm = 162;
    static constexpr uint8 R12x4_UNorm = 163;
    static constexpr uint8 B16G16R16G16_422_UNorm = 164;
    static constexpr uint8 G16B16G16R16_422_UNorm = 165;
    static constexpr uint8 G16_B16R16_420_UNorm = 166;
    static constexpr uint8 G16_B16R16_422_UNorm = 167;
    static constexpr uint8 G16_B16_R16_420_UNorm = 168;
    static constexpr uint8 G16_B16_R16_422_UNorm = 169;
    static constexpr uint8 G16_B16_R16_444_UNorm = 170;
    static constexpr uint8 SwapchainColor = 254;
};

struct EAttachmentLoadOp
{
    EAttachmentLoadOp () {}
    EAttachmentLoadOp (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Invalidate = 0;
    static constexpr uint8 Load = 1;
    static constexpr uint8 Clear = 2;
    static constexpr uint8 None = 3;
};

struct EAttachmentStoreOp
{
    EAttachmentStoreOp () {}
    EAttachmentStoreOp (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Invalidate = 0;
    static constexpr uint8 Store = 1;
    static constexpr uint8 None = 2;
};

struct ECompareOp
{
    ECompareOp () {}
    ECompareOp (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Never = 0;
    static constexpr uint8 Less = 1;
    static constexpr uint8 Equal = 2;
    static constexpr uint8 LEqual = 3;
    static constexpr uint8 Greater = 4;
    static constexpr uint8 NotEqual = 5;
    static constexpr uint8 GEqual = 6;
    static constexpr uint8 Always = 7;
    static constexpr uint8 LessOrEqual = 3;
    static constexpr uint8 GreaterOrEqual = 6;
};

struct EBlendFactor
{
    EBlendFactor () {}
    EBlendFactor (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Zero = 0;
    static constexpr uint8 One = 1;
    static constexpr uint8 SrcColor = 2;
    static constexpr uint8 OneMinusSrcColor = 3;
    static constexpr uint8 DstColor = 4;
    static constexpr uint8 OneMinusDstColor = 5;
    static constexpr uint8 SrcAlpha = 6;
    static constexpr uint8 OneMinusSrcAlpha = 7;
    static constexpr uint8 DstAlpha = 8;
    static constexpr uint8 OneMinusDstAlpha = 9;
    static constexpr uint8 ConstColor = 10;
    static constexpr uint8 OneMinusConstColor = 11;
    static constexpr uint8 ConstAlpha = 12;
    static constexpr uint8 OneMinusConstAlpha = 13;
    static constexpr uint8 SrcAlphaSaturate = 14;
    static constexpr uint8 Src1Color = 15;
    static constexpr uint8 OneMinusSrc1Color = 16;
    static constexpr uint8 Src1Alpha = 17;
    static constexpr uint8 OneMinusSrc1Alpha = 18;
};

struct EBlendOp
{
    EBlendOp () {}
    EBlendOp (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Add = 0;
    static constexpr uint8 Sub = 1;
    static constexpr uint8 RevSub = 2;
    static constexpr uint8 Min = 3;
    static constexpr uint8 Max = 4;
};

struct ELogicOp
{
    ELogicOp () {}
    ELogicOp (uint8) {}
    operator uint8 () const;
    static constexpr uint8 None = 0;
    static constexpr uint8 Clear = 1;
    static constexpr uint8 Set = 2;
    static constexpr uint8 Copy = 3;
    static constexpr uint8 CopyInverted = 4;
    static constexpr uint8 NoOp = 5;
    static constexpr uint8 Invert = 6;
    static constexpr uint8 And = 7;
    static constexpr uint8 NotAnd = 8;
    static constexpr uint8 Or = 9;
    static constexpr uint8 NotOr = 10;
    static constexpr uint8 Xor = 11;
    static constexpr uint8 Equiv = 12;
    static constexpr uint8 AndReverse = 13;
    static constexpr uint8 AndInverted = 14;
    static constexpr uint8 OrReverse = 15;
    static constexpr uint8 OrInverted = 16;
};

struct EStencilOp
{
    EStencilOp () {}
    EStencilOp (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Keep = 0;
    static constexpr uint8 Zero = 1;
    static constexpr uint8 Replace = 2;
    static constexpr uint8 Incr = 3;
    static constexpr uint8 IncrWrap = 4;
    static constexpr uint8 Decr = 5;
    static constexpr uint8 DecrWrap = 6;
    static constexpr uint8 Invert = 7;
};

struct EPolygonMode
{
    EPolygonMode () {}
    EPolygonMode (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Point = 0;
    static constexpr uint8 Line = 1;
    static constexpr uint8 Fill = 2;
};

struct EPrimitive
{
    EPrimitive () {}
    EPrimitive (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Point = 0;
    static constexpr uint8 LineList = 1;
    static constexpr uint8 LineStrip = 2;
    static constexpr uint8 LineListAdjacency = 3;
    static constexpr uint8 LineStripAdjacency = 4;
    static constexpr uint8 TriangleList = 5;
    static constexpr uint8 TriangleStrip = 6;
    static constexpr uint8 TriangleFan = 7;
    static constexpr uint8 TriangleListAdjacency = 8;
    static constexpr uint8 TriangleStripAdjacency = 9;
    static constexpr uint8 Patch = 10;
};

struct ECullMode
{
    ECullMode () {}
    ECullMode (uint8) {}
    operator uint8 () const;
    static constexpr uint8 None = 0;
    static constexpr uint8 Front = 1;
    static constexpr uint8 Back = 2;
    static constexpr uint8 FontAndBack = 3;
};

struct EPipelineDynamicState
{
    EPipelineDynamicState () {}
    EPipelineDynamicState (uint16) {}
    operator uint16 () const;
    static constexpr uint16 None = 0;
    static constexpr uint16 StencilCompareMask = 1;
    static constexpr uint16 StencilWriteMask = 2;
    static constexpr uint16 StencilReference = 4;
    static constexpr uint16 DepthBias = 8;
    static constexpr uint16 BlendConstants = 16;
    static constexpr uint16 RTStackSize = 64;
    static constexpr uint16 FragmentShadingRate = 128;
};

struct EResourceState
{
    EResourceState () {}
    EResourceState (uint32) {}
    operator uint32 () const;
    static constexpr uint32 Unknown = 0;
    static constexpr uint32 Preserve = 1;
    static constexpr uint32 ShaderStorage_Read = 271;
    static constexpr uint32 ShaderStorage_Write = 528;
    static constexpr uint32 ShaderStorage_RW = 785;
    static constexpr uint32 ShaderUniform = 274;
    static constexpr uint32 ShaderSample = 275;
    static constexpr uint32 CopySrc = 258;
    static constexpr uint32 CopyDst = 515;
    static constexpr uint32 ClearDst = 516;
    static constexpr uint32 BlitSrc = 261;
    static constexpr uint32 BlitDst = 518;
    static constexpr uint32 InputColorAttachment = 276;
    static constexpr uint32 InputColorAttachment_RW = 533;
    static constexpr uint32 ColorAttachment = 519;
    static constexpr uint32 ColorAttachment_Blend = 775;
    static constexpr uint32 DepthStencilAttachment_Read = 1288;
    static constexpr uint32 DepthStencilAttachment_Write = 2569;
    static constexpr uint32 DepthStencilAttachment_RW = 3849;
    static constexpr uint32 DepthTest_StencilRW = 3338;
    static constexpr uint32 DepthRW_StencilTest = 1803;
    static constexpr uint32 DepthStencilTest_ShaderSample = 1304;
    static constexpr uint32 DepthTest_DepthSample_StencilRW = 3353;
    static constexpr uint32 InputDepthStencilAttachment = 1302;
    static constexpr uint32 InputDepthStencilAttachment_RW = 3863;
    static constexpr uint32 Host_Read = 282;
    static constexpr uint32 Host_Write = 539;
    static constexpr uint32 PresentImage = 268;
    static constexpr uint32 IndirectBuffer = 285;
    static constexpr uint32 IndexBuffer = 286;
    static constexpr uint32 VertexBuffer = 287;
    static constexpr uint32 ShadingRateImage = 269;
    static constexpr uint32 CopyRTAS_Read = 288;
    static constexpr uint32 CopyRTAS_Write = 545;
    static constexpr uint32 BuildRTAS_Read = 290;
    static constexpr uint32 BuildRTAS_Write = 547;
    static constexpr uint32 BuildRTAS_RW = 803;
    static constexpr uint32 BuildRTAS_IndirectBuffer = 292;
    static constexpr uint32 ShaderRTAS = 284;
    static constexpr uint32 RTShaderBindingTable = 293;
    static constexpr uint32 DSTestBeforeFS = 4096;
    static constexpr uint32 DSTestAfterFS = 8192;
    static constexpr uint32 Invalidate = 16384;
    static constexpr uint32 General = 782;
    static constexpr uint32 MeshTaskShader = 32768;
    static constexpr uint32 VertexProcessingShaders = 65536;
    static constexpr uint32 TileShader = 131072;
    static constexpr uint32 FragmentShader = 262144;
    static constexpr uint32 PreRasterizationShaders = 98304;
    static constexpr uint32 PostRasterizationShaders = 393216;
    static constexpr uint32 ComputeShader = 524288;
    static constexpr uint32 RayTracingShaders = 1048576;
    static constexpr uint32 AllGraphicsShaders = 491520;
    static constexpr uint32 AllShaders = 2064384;
    static constexpr uint32 BuildRTAS_ScratchBuffer = 803;
    static constexpr uint32 InputDepthAttachment = 1302;
    static constexpr uint32 DepthStencilAttachment = 16137;
};

struct EImageAspect
{
    EImageAspect () {}
    EImageAspect (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Color = 1;
    static constexpr uint8 Depth = 2;
    static constexpr uint8 Stencil = 4;
    static constexpr uint8 Plane_0 = 16;
    static constexpr uint8 Plane_1 = 32;
    static constexpr uint8 Plane_2 = 64;
};

struct EShaderIO
{
    EShaderIO () {}
    EShaderIO (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Int = 1;
    static constexpr uint8 UInt = 2;
    static constexpr uint8 Float = 3;
    static constexpr uint8 UFloat = 4;
    static constexpr uint8 Half = 5;
    static constexpr uint8 UNorm = 6;
    static constexpr uint8 SNorm = 7;
    static constexpr uint8 sRGB = 8;
    static constexpr uint8 AnyColor = 9;
    static constexpr uint8 Depth = 10;
    static constexpr uint8 Stencil = 11;
    static constexpr uint8 DepthStencil = 12;
};

struct ESubgroupTypes
{
    ESubgroupTypes () {}
    ESubgroupTypes (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Float16 = 32;
    static constexpr uint8 Float32 = 1;
    static constexpr uint8 Int8 = 4;
    static constexpr uint8 Int16 = 8;
    static constexpr uint8 Int32 = 2;
    static constexpr uint8 Int64 = 16;
};

struct ESubgroupOperation
{
    ESubgroupOperation () {}
    ESubgroupOperation (uint32) {}
    operator uint32 () const;
    static constexpr uint32 IndexAndSize = 0;
    static constexpr uint32 Elect = 1;
    static constexpr uint32 Barrier = 2;
    static constexpr uint32 Any = 3;
    static constexpr uint32 All = 4;
    static constexpr uint32 AllEqual = 5;
    static constexpr uint32 Add = 6;
    static constexpr uint32 Mul = 7;
    static constexpr uint32 Min = 8;
    static constexpr uint32 Max = 9;
    static constexpr uint32 And = 10;
    static constexpr uint32 Or = 11;
    static constexpr uint32 Xor = 12;
    static constexpr uint32 InclusiveMul = 13;
    static constexpr uint32 InclusiveAdd = 14;
    static constexpr uint32 InclusiveMin = 15;
    static constexpr uint32 InclusiveMax = 16;
    static constexpr uint32 InclusiveAnd = 17;
    static constexpr uint32 InclusiveOr = 18;
    static constexpr uint32 InclusiveXor = 19;
    static constexpr uint32 ExclusiveAdd = 20;
    static constexpr uint32 ExclusiveMul = 21;
    static constexpr uint32 ExclusiveMin = 22;
    static constexpr uint32 ExclusiveMax = 23;
    static constexpr uint32 ExclusiveAnd = 24;
    static constexpr uint32 ExclusiveOr = 25;
    static constexpr uint32 ExclusiveXor = 26;
    static constexpr uint32 Ballot = 27;
    static constexpr uint32 Broadcast = 28;
    static constexpr uint32 BroadcastFirst = 29;
    static constexpr uint32 InverseBallot = 30;
    static constexpr uint32 BallotBitExtract = 31;
    static constexpr uint32 BallotBitCount = 32;
    static constexpr uint32 BallotInclusiveBitCount = 33;
    static constexpr uint32 BallotExclusiveBitCount = 34;
    static constexpr uint32 BallotFindLSB = 35;
    static constexpr uint32 BallotFindMSB = 36;
    static constexpr uint32 Shuffle = 37;
    static constexpr uint32 ShuffleXor = 38;
    static constexpr uint32 ShuffleUp = 39;
    static constexpr uint32 ShuffleDown = 40;
    static constexpr uint32 ClusteredAdd = 41;
    static constexpr uint32 ClusteredMul = 42;
    static constexpr uint32 ClusteredMin = 43;
    static constexpr uint32 ClusteredMax = 44;
    static constexpr uint32 ClusteredAnd = 45;
    static constexpr uint32 ClusteredOr = 46;
    static constexpr uint32 ClusteredXor = 47;
    static constexpr uint32 QuadBroadcast = 48;
    static constexpr uint32 QuadSwapHorizontal = 49;
    static constexpr uint32 QuadSwapVertical = 50;
    static constexpr uint32 QuadSwapDiagonal = 51;
    static constexpr uint32 _Basic_Begin = 0;
    static constexpr uint32 _Basic_End = 2;
    static constexpr uint32 _Vote_Begin = 3;
    static constexpr uint32 _Vote_End = 5;
    static constexpr uint32 _Arithmetic_Begin = 6;
    static constexpr uint32 _Arithmetic_End = 26;
    static constexpr uint32 _Ballot_Begin = 27;
    static constexpr uint32 _Ballot_End = 36;
    static constexpr uint32 _Shuffle_Begin = 37;
    static constexpr uint32 _Shuffle_End = 38;
    static constexpr uint32 _ShuffleRelative_Begin = 39;
    static constexpr uint32 _ShuffleRelative_End = 40;
    static constexpr uint32 _Clustered_Begin = 41;
    static constexpr uint32 _Clustered_End = 47;
    static constexpr uint32 _Quad_Begin = 48;
    static constexpr uint32 _Quad_End = 51;
};

struct EFeature
{
    EFeature () {}
    EFeature (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Ignore = 0;
    static constexpr uint8 RequireTrue = 2;
    static constexpr uint8 RequireFalse = 1;
};

struct EShader
{
    EShader () {}
    EShader (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Vertex = 0;
    static constexpr uint8 TessControl = 1;
    static constexpr uint8 TessEvaluation = 2;
    static constexpr uint8 Geometry = 3;
    static constexpr uint8 Fragment = 4;
    static constexpr uint8 Compute = 5;
    static constexpr uint8 Tile = 6;
    static constexpr uint8 MeshTask = 7;
    static constexpr uint8 Mesh = 8;
    static constexpr uint8 RayGen = 9;
    static constexpr uint8 RayAnyHit = 10;
    static constexpr uint8 RayClosestHit = 11;
    static constexpr uint8 RayMiss = 12;
    static constexpr uint8 RayIntersection = 13;
    static constexpr uint8 RayCallable = 14;
};

struct EShaderStages
{
    EShaderStages () {}
    EShaderStages (uint16) {}
    operator uint16 () const;
    static constexpr uint16 Vertex = 1;
    static constexpr uint16 TessControl = 2;
    static constexpr uint16 TessEvaluation = 4;
    static constexpr uint16 Geometry = 8;
    static constexpr uint16 Fragment = 16;
    static constexpr uint16 Compute = 32;
    static constexpr uint16 Tile = 64;
    static constexpr uint16 MeshTask = 128;
    static constexpr uint16 Mesh = 256;
    static constexpr uint16 RayGen = 512;
    static constexpr uint16 RayAnyHit = 1024;
    static constexpr uint16 RayClosestHit = 2048;
    static constexpr uint16 RayMiss = 4096;
    static constexpr uint16 RayIntersection = 8192;
    static constexpr uint16 RayCallable = 16384;
    static constexpr uint16 All = 32767;
    static constexpr uint16 AllGraphics = 415;
    static constexpr uint16 AllRayTracing = 32256;
    static constexpr uint16 GraphicsStages = 31;
    static constexpr uint16 MeshStages = 400;
    static constexpr uint16 VertexProcessingStages = 271;
    static constexpr uint16 PreRasterizationStages = 399;
    static constexpr uint16 PostRasterizationStages = 80;
};

struct EVendorID
{
    EVendorID () {}
    EVendorID (uint32) {}
    operator uint32 () const;
    static constexpr uint32 AMD = 0;
    static constexpr uint32 NVidia = 1;
    static constexpr uint32 Intel = 2;
    static constexpr uint32 ARM = 3;
    static constexpr uint32 Qualcomm = 4;
    static constexpr uint32 ImgTech = 5;
    static constexpr uint32 Microsoft = 6;
    static constexpr uint32 Apple = 7;
    static constexpr uint32 Mesa = 8;
    static constexpr uint32 Broadcom = 9;
    static constexpr uint32 Samsung = 10;
    static constexpr uint32 VeriSilicon = 11;
};

struct EVertexType
{
    EVertexType () {}
    EVertexType (uint16) {}
    operator uint16 () const;
    static constexpr uint16 Byte = 4;
    static constexpr uint16 Byte2 = 5;
    static constexpr uint16 Byte3 = 6;
    static constexpr uint16 Byte4 = 7;
    static constexpr uint16 Byte_Norm = 68;
    static constexpr uint16 Byte2_Norm = 69;
    static constexpr uint16 Byte3_Norm = 70;
    static constexpr uint16 Byte4_Norm = 71;
    static constexpr uint16 Byte_Scaled = 132;
    static constexpr uint16 Byte2_Scaled = 133;
    static constexpr uint16 Byte3_Scaled = 134;
    static constexpr uint16 Byte4_Scaled = 135;
    static constexpr uint16 UByte = 8;
    static constexpr uint16 UByte2 = 9;
    static constexpr uint16 UByte3 = 10;
    static constexpr uint16 UByte4 = 11;
    static constexpr uint16 UByte_Norm = 72;
    static constexpr uint16 UByte2_Norm = 73;
    static constexpr uint16 UByte3_Norm = 74;
    static constexpr uint16 UByte4_Norm = 75;
    static constexpr uint16 UByte_Scaled = 136;
    static constexpr uint16 UByte2_Scaled = 137;
    static constexpr uint16 UByte3_Scaled = 138;
    static constexpr uint16 UByte4_Scaled = 139;
    static constexpr uint16 Short = 12;
    static constexpr uint16 Short2 = 13;
    static constexpr uint16 Short3 = 14;
    static constexpr uint16 Short4 = 15;
    static constexpr uint16 Short_Norm = 76;
    static constexpr uint16 Short2_Norm = 77;
    static constexpr uint16 Short3_Norm = 78;
    static constexpr uint16 Short4_Norm = 79;
    static constexpr uint16 Short_Scaled = 140;
    static constexpr uint16 Short2_Scaled = 141;
    static constexpr uint16 Short3_Scaled = 142;
    static constexpr uint16 Short4_Scaled = 143;
    static constexpr uint16 UShort = 16;
    static constexpr uint16 UShort2 = 17;
    static constexpr uint16 UShort3 = 18;
    static constexpr uint16 UShort4 = 19;
    static constexpr uint16 UShort_Norm = 80;
    static constexpr uint16 UShort2_Norm = 81;
    static constexpr uint16 UShort3_Norm = 82;
    static constexpr uint16 UShort4_Norm = 83;
    static constexpr uint16 UShort_Scaled = 144;
    static constexpr uint16 UShort2_Scaled = 145;
    static constexpr uint16 UShort3_Scaled = 146;
    static constexpr uint16 UShort4_Scaled = 147;
    static constexpr uint16 Int = 20;
    static constexpr uint16 Int2 = 21;
    static constexpr uint16 Int3 = 22;
    static constexpr uint16 Int4 = 23;
    static constexpr uint16 UInt = 24;
    static constexpr uint16 UInt2 = 25;
    static constexpr uint16 UInt3 = 26;
    static constexpr uint16 UInt4 = 27;
    static constexpr uint16 Long = 28;
    static constexpr uint16 Long2 = 29;
    static constexpr uint16 Long3 = 30;
    static constexpr uint16 Long4 = 31;
    static constexpr uint16 ULong = 32;
    static constexpr uint16 ULong2 = 33;
    static constexpr uint16 ULong3 = 34;
    static constexpr uint16 ULong4 = 35;
    static constexpr uint16 Half = 36;
    static constexpr uint16 Half2 = 37;
    static constexpr uint16 Half3 = 38;
    static constexpr uint16 Half4 = 39;
    static constexpr uint16 Float = 40;
    static constexpr uint16 Float2 = 41;
    static constexpr uint16 Float3 = 42;
    static constexpr uint16 Float4 = 43;
    static constexpr uint16 Double = 44;
    static constexpr uint16 Double2 = 45;
    static constexpr uint16 Double3 = 46;
    static constexpr uint16 Double4 = 47;
    static constexpr uint16 UInt_2_10_10_10 = 51;
    static constexpr uint16 UInt_2_10_10_10_Norm = 115;
    static constexpr uint16 UInt_2_10_10_10_Scaled = 179;
};

struct EGraphicsDeviceID
{
    EGraphicsDeviceID () {}
    EGraphicsDeviceID (uint32) {}
    operator uint32 () const;
    static constexpr uint32 Adreno_500 = 0;
    static constexpr uint32 Adreno_600 = 1;
    static constexpr uint32 Adreno_600_QC4 = 2;
    static constexpr uint32 Adreno_600_OC4 = 3;
    static constexpr uint32 Adreno_600_QC5 = 4;
    static constexpr uint32 Adreno_700_SC3 = 5;
    static constexpr uint32 Adreno_700_DC4_SC5 = 6;
    static constexpr uint32 AMD_GCN1 = 7;
    static constexpr uint32 AMD_GCN2 = 8;
    static constexpr uint32 AMD_GCN3 = 9;
    static constexpr uint32 AMD_GCN4 = 10;
    static constexpr uint32 AMD_GCN5 = 11;
    static constexpr uint32 AMD_GCN5_APU = 12;
    static constexpr uint32 AMD_RDNA1 = 13;
    static constexpr uint32 AMD_RDNA2 = 14;
    static constexpr uint32 AMD_RDNA2_APU = 15;
    static constexpr uint32 AMD_RDNA3 = 16;
    static constexpr uint32 AMD_RDNA3_APU = 17;
    static constexpr uint32 Apple_A8 = 18;
    static constexpr uint32 Apple_A9_A10 = 19;
    static constexpr uint32 Apple_A11 = 20;
    static constexpr uint32 Apple_A12 = 21;
    static constexpr uint32 Apple_A13 = 22;
    static constexpr uint32 Apple_A14_M1 = 23;
    static constexpr uint32 Apple_A15_M2 = 24;
    static constexpr uint32 Apple_A16 = 25;
    static constexpr uint32 Mali_Midgard_Gen2 = 26;
    static constexpr uint32 Mali_Midgard_Gen3 = 27;
    static constexpr uint32 Mali_Midgard_Gen4 = 28;
    static constexpr uint32 Mali_Bifrost_Gen1 = 29;
    static constexpr uint32 Mali_Bifrost_Gen2 = 30;
    static constexpr uint32 Mali_Bifrost_Gen3 = 31;
    static constexpr uint32 Mali_Valhall_Gen1 = 32;
    static constexpr uint32 Mali_Valhall_Gen2 = 33;
    static constexpr uint32 Mali_Valhall_Gen3 = 34;
    static constexpr uint32 Mali_Valhall_Gen4 = 35;
    static constexpr uint32 NV_Maxwell = 36;
    static constexpr uint32 NV_Maxwell_Tegra = 37;
    static constexpr uint32 NV_Pascal = 38;
    static constexpr uint32 NV_Pascal_MX = 39;
    static constexpr uint32 NV_Pascal_Tegra = 40;
    static constexpr uint32 NV_Volta = 41;
    static constexpr uint32 NV_Turing_16 = 42;
    static constexpr uint32 NV_Turing = 43;
    static constexpr uint32 NV_Turing_MX = 44;
    static constexpr uint32 NV_Ampere = 45;
    static constexpr uint32 NV_Ampere_Orin = 46;
    static constexpr uint32 NV_Ada = 47;
    static constexpr uint32 Intel_Gen9 = 48;
    static constexpr uint32 Intel_Gen11 = 49;
    static constexpr uint32 Intel_Gen12 = 50;
    static constexpr uint32 PowerVR_Series8XE = 51;
    static constexpr uint32 PowerVR_Series8XEP = 52;
    static constexpr uint32 PowerVR_Series8XT = 53;
    static constexpr uint32 PowerVR_Series9XE = 54;
    static constexpr uint32 VeriSilicon = 55;
    static constexpr uint32 V3D_4 = 56;
    static constexpr uint32 SwiftShader = 57;
};

struct EFilter
{
    EFilter () {}
    EFilter (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Nearest = 0;
    static constexpr uint8 Linear = 1;
};

struct EMipmapFilter
{
    EMipmapFilter () {}
    EMipmapFilter (uint8) {}
    operator uint8 () const;
    static constexpr uint8 None = 0;
    static constexpr uint8 Nearest = 1;
    static constexpr uint8 Linear = 2;
};

struct EAddressMode
{
    EAddressMode () {}
    EAddressMode (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Repeat = 0;
    static constexpr uint8 MirrorRepeat = 1;
    static constexpr uint8 ClampToEdge = 2;
    static constexpr uint8 ClampToBorder = 3;
    static constexpr uint8 MirrorClampToEdge = 4;
    static constexpr uint8 Clamp = 2;
    static constexpr uint8 MirrorClamp = 4;
};

struct EBorderColor
{
    EBorderColor () {}
    EBorderColor (uint8) {}
    operator uint8 () const;
    static constexpr uint8 FloatTransparentBlack = 0;
    static constexpr uint8 FloatOpaqueBlack = 1;
    static constexpr uint8 FloatOpaqueWhite = 2;
    static constexpr uint8 IntTransparentBlack = 3;
    static constexpr uint8 IntOpaqueBlack = 4;
    static constexpr uint8 IntOpaqueWhite = 5;
};

struct EReductionMode
{
    EReductionMode () {}
    EReductionMode (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Average = 0;
    static constexpr uint8 Min = 1;
    static constexpr uint8 Max = 2;
};

struct ESamplerUsage
{
    ESamplerUsage () {}
    ESamplerUsage (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Default = 0;
    static constexpr uint8 Subsampled = 1;
    static constexpr uint8 SubsampledCoarseReconstruction = 2;
};

struct EVertexInputRate
{
    EVertexInputRate () {}
    EVertexInputRate (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Vertex = 0;
    static constexpr uint8 Instance = 1;
};

struct EDescSetUsage
{
    EDescSetUsage () {}
    EDescSetUsage (uint8) {}
    operator uint8 () const;
    static constexpr uint8 AllowPartialyUpdate = 1;
    static constexpr uint8 UpdateTemplate = 2;
    static constexpr uint8 ArgumentBuffer = 4;
    static constexpr uint8 MutableArgBuffer = 8;
};

struct EPipelineOpt
{
    EPipelineOpt () {}
    EPipelineOpt (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Optimize = 1;
    static constexpr uint8 CS_DispatchBase = 2;
    static constexpr uint8 RT_NoNullAnyHitShaders = 4;
    static constexpr uint8 RT_NoNullClosestHitShaders = 8;
    static constexpr uint8 RT_NoNullMissShaders = 16;
    static constexpr uint8 RT_NoNullIntersectionShaders = 32;
    static constexpr uint8 RT_SkipTriangles = 64;
    static constexpr uint8 RT_SkipAABBs = 128;
};

struct EQueueMask
{
    EQueueMask () {}
    EQueueMask (uint8) {}
    operator uint8 () const;
    static constexpr uint8 Graphics = 1;
    static constexpr uint8 AsyncCompute = 2;
    static constexpr uint8 AsyncTransfer = 4;
    static constexpr uint8 VideoEncode = 8;
    static constexpr uint8 VideoDecode = 16;
};

struct ESamplerChromaLocation
{
    ESamplerChromaLocation () {}
    ESamplerChromaLocation (uint8) {}
    operator uint8 () const;
    static constexpr uint8 CositedEven = 0;
    static constexpr uint8 Midpoint = 1;
};

struct ESamplerYcbcrModelConversion
{
    ESamplerYcbcrModelConversion () {}
    ESamplerYcbcrModelConversion (uint8) {}
    operator uint8 () const;
    static constexpr uint8 RGB_Identity = 0;
    static constexpr uint8 Ycbcr_Identity = 1;
    static constexpr uint8 Ycbcr_709 = 2;
    static constexpr uint8 Ycbcr_601 = 3;
    static constexpr uint8 Ycbcr_2020 = 4;
};

struct ESamplerYcbcrRange
{
    ESamplerYcbcrRange () {}
    ESamplerYcbcrRange (uint8) {}
    operator uint8 () const;
    static constexpr uint8 ITU_Full = 0;
    static constexpr uint8 ITU_Narrow = 1;
};

struct ESurfaceFormat
{
    ESurfaceFormat () {}
    ESurfaceFormat (uint8) {}
    operator uint8 () const;
    static constexpr uint8 BGRA8_sRGB_nonlinear = 0;
    static constexpr uint8 RGBA8_sRGB_nonlinear = 1;
    static constexpr uint8 BGRA8_BT709_nonlinear = 2;
    static constexpr uint8 RGBA16F_Extended_sRGB_linear = 3;
    static constexpr uint8 RGBA16F_sRGB_nonlinear = 4;
    static constexpr uint8 RGBA16F_BT709_nonlinear = 5;
    static constexpr uint8 RGBA16F_HDR10_ST2084 = 6;
    static constexpr uint8 RGBA16F_BT2020_linear = 7;
    static constexpr uint8 RGB10A2_sRGB_nonlinear = 8;
    static constexpr uint8 RGB10A2_HDR10_ST2084 = 9;
};

struct ERTInstanceOpt
{
    ERTInstanceOpt () {}
    ERTInstanceOpt (uint8) {}
    operator uint8 () const;
    static constexpr uint8 TriangleCullDisable = 1;
    static constexpr uint8 TriangleFrontCCW = 2;
    static constexpr uint8 ForceOpaque = 4;
    static constexpr uint8 ForceNonOpaque = 8;
    static constexpr uint8 TriangleCullBack = 0;
    static constexpr uint8 TriangleFrontCW = 0;
};

struct MultiSamples
{
    MultiSamples ();
    MultiSamples (const MultiSamples&);
    MultiSamples&  operator = (const MultiSamples&);
    MultiSamples (uint);
};

struct ImageLayer
{
    ImageLayer ();
    ImageLayer (const ImageLayer&);
    ImageLayer&  operator = (const ImageLayer&);
    ImageLayer (uint);
};

struct MipmapLevel
{
    MipmapLevel ();
    MipmapLevel (const MipmapLevel&);
    MipmapLevel&  operator = (const MipmapLevel&);
    MipmapLevel (uint);
};

struct ECubeFace
{
    ECubeFace () {}
    ECubeFace (uint32) {}
    operator uint32 () const;
    static constexpr uint32 XPos = 0;
    static constexpr uint32 XNeg = 1;
    static constexpr uint32 YPos = 2;
    static constexpr uint32 YNeg = 3;
    static constexpr uint32 ZPos = 4;
    static constexpr uint32 ZNeg = 5;
};

struct Texture
{
    Texture ();
    void  Alloc (const uint2 & dimension, EPixelFormat format);
    void  Alloc (const uint2 & dimension, EPixelFormat format, const ImageLayer & layers);
    void  Alloc (const uint2 & dimension, EPixelFormat format, const MipmapLevel & mipmaps);
    void  Alloc (const uint2 & dimension, EPixelFormat format, const ImageLayer & layers, const MipmapLevel & mipmaps);
    void  Alloc (const uint3 & dimension, EPixelFormat format);
    void  Alloc (const uint3 & dimension, EPixelFormat format, const MipmapLevel & mipmaps);
    void  AllocCube (const uint2 & dimension, EPixelFormat format);
    void  AllocCube (const uint2 & dimension, EPixelFormat format, const MipmapLevel & mipmaps);
    void  Load (const string & imageFile);
    void  Load (const string & imageFile, bool flipY);
    void  Load (const string & imageFile, const RectU & region);
    void  AddLayer (const string & imageFile, uint layer);
    void  AddLayer (const string & imageFile, uint layer, bool flipY);
    void  AddLayer (const string & imageFile, const RectU & region, uint layer);
    void  LoadChannel (const string & imageFile, const string & srcSwizzle, const string & dstSwizzle);
    void  Store (const string & nameInArchive);
    void  Format (EPixelFormat newFormat);
};

struct ImageAtlas
{
    ImageAtlas ();
    void  Add (const string & imageNameInAtlas, const string & filename);
    void  Add (const string & imageNameInAtlas, const string & filename, const RectU & region);
    void  Store (const string & nameInArchive);
    void  Padding (uint paddingInPixels);
    void  Format (EPixelFormat newFormat);
};

struct ERasterFontMode
{
    ERasterFontMode () {}
    ERasterFontMode (uint32) {}
    operator uint32 () const;

    // Raster font, glyph dimension on screen should be multiple of glyph bitmap size.
    static constexpr uint32 Raster = 0;

    // SDF font, can be scaled, but have smooth angles.
    static constexpr uint32 SDF = 1;

    // Multichannel SDF font, can be scaled, have sharp angles, but a bit less performance.
    static constexpr uint32 MC_SDF = 2;
};

struct RasterFont
{
    RasterFont ();
    void  Load (const string & fontFile);
    void  AddCharset (uint firstCharIndexInUnicode, uint lastCharIndexInUnicode);
    void  AddCharset_Ascii ();
    void  AddCharset_Rus ();
    void  ClearCharset ();
    void  GlyphSize (uint heightInPixels);
    void  GlyphPadding (uint paddingInPixels);
    void  Store (const string & nameInArchive);
    void  Format (EPixelFormat newFormat);
    void  RasterMode (ERasterFontMode);
    void  SDFGlyphBorder (uint borderSizeInPixels);

    // Increase value for better anti-aliasing.
    void  SDFPixelRange (float range);
};

struct Mesh
{
    Mesh ();
};

struct Model
{
    Model ();
};

struct Material
{
    Material ();
};

template <>
struct RC<RasterFont> : RasterFont
{
    RC (const RasterFont &);
};

template <>
struct RC<Material> : Material
{
    RC (const Material &);
};

template <>
struct RC<Mesh> : Mesh
{
    RC (const Mesh &);
};

template <>
struct RC<Model> : Model
{
    RC (const Model &);
};

template <>
struct RC<Texture> : Texture
{
    RC (const Texture &);
};

template <>
struct RC<ImageAtlas> : ImageAtlas
{
    RC (const ImageAtlas &);
};

