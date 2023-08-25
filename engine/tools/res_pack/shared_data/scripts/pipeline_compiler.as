//EE09734C
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

struct RGBA8u;
struct ECullMode;
struct EMipmapFilter;
struct ELogicOp;
struct EShaderProprocessor;
struct ShaderStructTypeUsage;
struct EBlendOp;
struct ETessPatch;
struct RGBA32f;
struct RenderState_ColorBuffer_ColorMask;
struct bool2;
struct HSVColor;
struct EQueueMask;
struct MissIndex;
struct bool3;
struct bool4;
struct EImage;
struct RGBA32i;
struct ECompilationTarget;
struct EVertexType;
struct RenderState_MultisampleState;
struct EAddressMode;
struct RayIndex;
struct ShaderStructType;
struct ERTInstanceOpt;
struct EBlendFactor;
struct Shader;
struct EStencilOp;
struct GlobalConfig;
struct ESamplerUsage;
struct ubyte4;
struct ESamplerYcbcrRange;
struct ubyte2;
struct RenderState_InputAssemblyState;
struct EGraphicsDeviceID;
struct CompatibleRenderPass;
struct ubyte3;
struct ESubgroupOperation;
struct EFormatFeature;
struct RenderState_RasterizationState;
struct ComputePipelineSpec;
struct EPrimitive;
struct EFilter;
struct RenderState_StencilBufferState;
struct RayTracingShaderBinding;
struct RenderTechnique;
struct RayTracingPipeline;
struct EPipelineDynamicState;
struct MultiSamples;
struct ESamplerChromaLocation;
struct DescriptorSetLayout;
struct EVertexInputRate;
struct EDescSetUsage;
struct EImageAspect;
struct EShaderIO;
struct float3;
struct float2;
struct EPipelineOpt;
struct InstanceIndex;
struct float4;
struct EAccessType;
struct EIndex;
struct ComputePipeline;
struct Align;
struct ESubgroupTypes;
struct ESamplerYcbcrModelConversion;
struct short3;
struct GraphicsPass;
struct short2;
struct sbyte3;
struct EAttachment;
struct sbyte4;
struct ushort2;
struct ushort4;
struct sbyte2;
struct ushort3;
struct EPixelFormat;
struct short4;
struct RenderState_ColorBuffer;
struct RenderPass;
struct int2;
struct int3;
struct ArraySize;
struct int4;
struct ImageLayer;
struct PipelineLayout;
struct GraphicsPipelineSpec;
struct TilePipeline;
struct EPolygonMode;
struct EAttachmentLoadOp;
struct TilePipelineSpec;
struct EValueType;
struct ETessSpacing;
struct ESurfaceFormat;
struct EShaderVersion;
struct EReductionMode;
struct MipmapLevel;
struct RenderState_ColorBuffersState;
struct RayTracingPipelineSpec;
struct AttachmentSpec;
struct RenderState;
struct DepthStencil;
struct EShaderOpt;
struct CallableIndex;
struct GraphicsPipeline;
struct EFeature;
struct MeshPipelineSpec;
struct Sampler;
struct EAttachmentStoreOp;
struct EImageType;
struct EVendorID;
struct EBorderColor;
struct VertexDivisor;
struct MeshPipeline;
struct NamedRenderState;
struct EResourceState;
struct ECompareOp;
struct FeatureSet;
struct ComputePass;
struct RenderState_StencilFaceState;
struct uint4;
struct uint2;
struct ShaderIO;
struct EMutableRenderState;
struct uint3;
struct EShader;
struct RenderState_DepthBufferState;
struct EShaderStages;
struct EStructLayout;
struct RGBA32u;
struct Attachment;
struct VertexBufferInput;

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
    float4 (const RGBA32f &);
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
RGBA32f  WavelengthToRGB (float);
RGBA32f  Lerp (const RGBA32f &, const RGBA32f &, float);
RGBA32f  AdjustContrast (const RGBA32f &, float);
float  Luminance (const RGBA32f &);
RGBA32f  Rainbow (float);
RGBA32f  RainbowWrap (float);
struct RGBA32f
{
    RGBA32f ();
    RGBA32f (const RGBA32f&);
    RGBA32f&  operator = (const RGBA32f&);
    RGBA32f (float);
    RGBA32f (float, float, float, float);
    float r;
    float g;
    float b;
    float a;
    RGBA32f (const HSVColor &, float);
    RGBA32f (const RGBA8u &);
    RGBA32f (const RGBA32u &);
};

struct RGBA32i
{
    RGBA32i ();
    RGBA32i (const RGBA32i&);
    RGBA32i&  operator = (const RGBA32i&);
    RGBA32i (int);
    RGBA32i (int, int, int, int);
    int r;
    int g;
    int b;
    int a;
    RGBA32i (const RGBA8u &);
    RGBA32i (const RGBA32u &);
};

struct RGBA32u
{
    RGBA32u ();
    RGBA32u (const RGBA32u&);
    RGBA32u&  operator = (const RGBA32u&);
    RGBA32u (uint);
    RGBA32u (uint, uint, uint, uint);
    uint r;
    uint g;
    uint b;
    uint a;
    RGBA32u (const RGBA8u &);
    RGBA32u (const RGBA32i &);
};

RGBA8u  AdjustContrast (const RGBA8u &, float);
float  Luminance (const RGBA8u &);
RGBA8u  AdjustSaturation (const RGBA8u &, float);
RGBA8u  Lerp (const RGBA8u &, const RGBA8u &, float);
struct RGBA8u
{
    RGBA8u ();
    RGBA8u (const RGBA8u&);
    RGBA8u&  operator = (const RGBA8u&);
    RGBA8u (uint8);
    RGBA8u (uint8, uint8, uint8, uint8);
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 a;
    RGBA8u (const RGBA32i &);
    RGBA8u (const RGBA32u &);
    RGBA8u (const RGBA32f &);
};

struct HSVColor
{
    HSVColor ();
    HSVColor (const HSVColor&);
    HSVColor&  operator = (const HSVColor&);
    float h;
    float s;
    float v;
    HSVColor (float);
    HSVColor (float, float);
    HSVColor (float, float, float);
    HSVColor (const RGBA32f &);
};

struct DepthStencil
{
    DepthStencil ();
    DepthStencil (const DepthStencil&);
    DepthStencil&  operator = (const DepthStencil&);
    float depth;
    uint stencil;
    DepthStencil (float, int);
};

RGBA8u  asARGB (uint);
RGBA8u  asRGBA (uint);
string  ToString (int);
string  ToString (uint);
string  ToString (int64);
string  ToString (uint64);
string  ToString (float);
string  ToString (double);
string  ToString (float, uint);
string  ToString (double, uint);
string  ToString (const bool2 &);
string  ToString (const bool3 &);
string  ToString (const bool4 &);
string  ToString (const sbyte2 &);
string  ToString (const sbyte3 &);
string  ToString (const sbyte4 &);
string  ToString (const ubyte2 &);
string  ToString (const ubyte3 &);
string  ToString (const ubyte4 &);
string  ToString (const short2 &);
string  ToString (const short3 &);
string  ToString (const short4 &);
string  ToString (const int2 &);
string  ToString (const int3 &);
string  ToString (const int4 &);
string  ToString (const uint2 &);
string  ToString (const uint3 &);
string  ToString (const uint4 &);
string  ToString (const float2 &);
string  ToString (const float3 &);
string  ToString (const float4 &);
string  ToString (const RGBA32f &);
string  ToString (const RGBA32i &);
string  ToString (const RGBA32u &);
string  ToString (const RGBA8u &);
void  LogError (const string &);
void  LogInfo (const string &);
void  LogDebug (const string &);
void  LogFatal (const string &);
void  Assert (bool);
void  Assert (bool, const string &);
struct EImage
{
    EImage () {}
    EImage (uint) {}
    operator uint () const;
    static constexpr uint 1D = 0;
    static constexpr uint 2D = 1;
    static constexpr uint 3D = 2;
    static constexpr uint 1DArray = 3;
    static constexpr uint 2DArray = 4;
    static constexpr uint Cube = 5;
    static constexpr uint CubeArray = 6;
};

struct EIndex
{
    EIndex () {}
    EIndex (uint) {}
    operator uint () const;
    static constexpr uint UShort = 0;
    static constexpr uint UInt = 1;
};

struct EPixelFormat
{
    EPixelFormat () {}
    EPixelFormat (uint) {}
    operator uint () const;
    static constexpr uint RGBA16_SNorm = 0;
    static constexpr uint RGBA8_SNorm = 1;
    static constexpr uint RGB16_SNorm = 2;
    static constexpr uint RGB8_SNorm = 3;
    static constexpr uint RG16_SNorm = 4;
    static constexpr uint RG8_SNorm = 5;
    static constexpr uint R16_SNorm = 6;
    static constexpr uint R8_SNorm = 7;
    static constexpr uint RGBA16_UNorm = 8;
    static constexpr uint RGBA8_UNorm = 9;
    static constexpr uint RGB16_UNorm = 10;
    static constexpr uint RGB8_UNorm = 11;
    static constexpr uint RG16_UNorm = 12;
    static constexpr uint RG8_UNorm = 13;
    static constexpr uint R16_UNorm = 14;
    static constexpr uint R8_UNorm = 15;
    static constexpr uint RGB10_A2_UNorm = 16;
    static constexpr uint RGBA4_UNorm = 17;
    static constexpr uint RGB5_A1_UNorm = 18;
    static constexpr uint RGB_5_6_5_UNorm = 19;
    static constexpr uint BGR8_UNorm = 20;
    static constexpr uint BGRA8_UNorm = 21;
    static constexpr uint sRGB8 = 22;
    static constexpr uint sRGB8_A8 = 23;
    static constexpr uint sBGR8 = 24;
    static constexpr uint sBGR8_A8 = 25;
    static constexpr uint R8I = 26;
    static constexpr uint RG8I = 27;
    static constexpr uint RGB8I = 28;
    static constexpr uint RGBA8I = 29;
    static constexpr uint R16I = 30;
    static constexpr uint RG16I = 31;
    static constexpr uint RGB16I = 32;
    static constexpr uint RGBA16I = 33;
    static constexpr uint R32I = 34;
    static constexpr uint RG32I = 35;
    static constexpr uint RGB32I = 36;
    static constexpr uint RGBA32I = 37;
    static constexpr uint R64I = 38;
    static constexpr uint R8U = 39;
    static constexpr uint RG8U = 40;
    static constexpr uint RGB8U = 41;
    static constexpr uint RGBA8U = 42;
    static constexpr uint R16U = 43;
    static constexpr uint RG16U = 44;
    static constexpr uint RGB16U = 45;
    static constexpr uint RGBA16U = 46;
    static constexpr uint R32U = 47;
    static constexpr uint RG32U = 48;
    static constexpr uint RGB32U = 49;
    static constexpr uint RGBA32U = 50;
    static constexpr uint RGB10_A2U = 51;
    static constexpr uint R64U = 52;
    static constexpr uint R16F = 53;
    static constexpr uint RG16F = 54;
    static constexpr uint RGB16F = 55;
    static constexpr uint RGBA16F = 56;
    static constexpr uint R32F = 57;
    static constexpr uint RG32F = 58;
    static constexpr uint RGB32F = 59;
    static constexpr uint RGBA32F = 60;
    static constexpr uint RGB_11_11_10F = 61;
    static constexpr uint RGB_9F_E5 = 62;
    static constexpr uint Depth16 = 63;
    static constexpr uint Depth24 = 64;
    static constexpr uint Depth32F = 65;
    static constexpr uint Depth16_Stencil8 = 66;
    static constexpr uint Depth24_Stencil8 = 67;
    static constexpr uint Depth32F_Stencil8 = 68;
    static constexpr uint BC1_RGB8_UNorm = 69;
    static constexpr uint BC1_sRGB8 = 70;
    static constexpr uint BC1_RGB8_A1_UNorm = 71;
    static constexpr uint BC1_sRGB8_A1 = 72;
    static constexpr uint BC2_RGBA8_UNorm = 73;
    static constexpr uint BC2_sRGB8 = 74;
    static constexpr uint BC3_RGBA8_UNorm = 75;
    static constexpr uint BC3_sRGB8 = 76;
    static constexpr uint BC4_R8_SNorm = 77;
    static constexpr uint BC4_R8_UNorm = 78;
    static constexpr uint BC5_RG8_SNorm = 79;
    static constexpr uint BC5_RG8_UNorm = 80;
    static constexpr uint BC6H_RGB16F = 81;
    static constexpr uint BC6H_RGB16UF = 82;
    static constexpr uint BC7_RGBA8_UNorm = 83;
    static constexpr uint BC7_sRGB8_A8 = 84;
    static constexpr uint ETC2_RGB8_UNorm = 85;
    static constexpr uint ECT2_sRGB8 = 86;
    static constexpr uint ETC2_RGB8_A1_UNorm = 87;
    static constexpr uint ETC2_sRGB8_A1 = 88;
    static constexpr uint ETC2_RGBA8_UNorm = 89;
    static constexpr uint ETC2_sRGB8_A8 = 90;
    static constexpr uint EAC_R11_SNorm = 91;
    static constexpr uint EAC_R11_UNorm = 92;
    static constexpr uint EAC_RG11_SNorm = 93;
    static constexpr uint EAC_RG11_UNorm = 94;
    static constexpr uint ASTC_RGBA_4x4 = 95;
    static constexpr uint ASTC_RGBA_5x4 = 96;
    static constexpr uint ASTC_RGBA_5x5 = 97;
    static constexpr uint ASTC_RGBA_6x5 = 98;
    static constexpr uint ASTC_RGBA_6x6 = 99;
    static constexpr uint ASTC_RGBA_8x5 = 100;
    static constexpr uint ASTC_RGBA_8x6 = 101;
    static constexpr uint ASTC_RGBA_8x8 = 102;
    static constexpr uint ASTC_RGBA_10x5 = 103;
    static constexpr uint ASTC_RGBA_10x6 = 104;
    static constexpr uint ASTC_RGBA_10x8 = 105;
    static constexpr uint ASTC_RGBA_10x10 = 106;
    static constexpr uint ASTC_RGBA_12x10 = 107;
    static constexpr uint ASTC_RGBA_12x12 = 108;
    static constexpr uint ASTC_sRGB8_A8_4x4 = 109;
    static constexpr uint ASTC_sRGB8_A8_5x4 = 110;
    static constexpr uint ASTC_sRGB8_A8_5x5 = 111;
    static constexpr uint ASTC_sRGB8_A8_6x5 = 112;
    static constexpr uint ASTC_sRGB8_A8_6x6 = 113;
    static constexpr uint ASTC_sRGB8_A8_8x5 = 114;
    static constexpr uint ASTC_sRGB8_A8_8x6 = 115;
    static constexpr uint ASTC_sRGB8_A8_8x8 = 116;
    static constexpr uint ASTC_sRGB8_A8_10x5 = 117;
    static constexpr uint ASTC_sRGB8_A8_10x6 = 118;
    static constexpr uint ASTC_sRGB8_A8_10x8 = 119;
    static constexpr uint ASTC_sRGB8_A8_10x10 = 120;
    static constexpr uint ASTC_sRGB8_A8_12x10 = 121;
    static constexpr uint ASTC_sRGB8_A8_12x12 = 122;
    static constexpr uint ASTC_RGBA16F_4x4 = 123;
    static constexpr uint ASTC_RGBA16F_5x4 = 124;
    static constexpr uint ASTC_RGBA16F_5x5 = 125;
    static constexpr uint ASTC_RGBA16F_6x5 = 126;
    static constexpr uint ASTC_RGBA16F_6x6 = 127;
    static constexpr uint ASTC_RGBA16F_8x5 = 128;
    static constexpr uint ASTC_RGBA16F_8x6 = 129;
    static constexpr uint ASTC_RGBA16F_8x8 = 130;
    static constexpr uint ASTC_RGBA16F_10x5 = 131;
    static constexpr uint ASTC_RGBA16F_10x6 = 132;
    static constexpr uint ASTC_RGBA16F_10x8 = 133;
    static constexpr uint ASTC_RGBA16F_10x10 = 134;
    static constexpr uint ASTC_RGBA16F_12x10 = 135;
    static constexpr uint ASTC_RGBA16F_12x12 = 136;
    static constexpr uint G8B8G8R8_422_UNorm = 137;
    static constexpr uint B8G8R8G8_422_UNorm = 138;
    static constexpr uint G8_B8R8_420_UNorm = 139;
    static constexpr uint G8_B8R8_422_UNorm = 140;
    static constexpr uint G8_B8_R8_420_UNorm = 141;
    static constexpr uint G8_B8_R8_422_UNorm = 142;
    static constexpr uint G8_B8_R8_444_UNorm = 143;
    static constexpr uint B10x6G10x6R10x6G10x6_422_UNorm = 144;
    static constexpr uint G10x6B10x6G10x6R10x6_422_UNorm = 145;
    static constexpr uint G10x6_B10x6R10x6_420_UNorm = 146;
    static constexpr uint G10x6_B10x6R10x6_422_UNorm = 147;
    static constexpr uint G10x6_B10x6_R10x6_420_UNorm = 148;
    static constexpr uint G10x6_B10x6_R10x6_422_UNorm = 149;
    static constexpr uint G10x6_B10x6_R10x6_444_UNorm = 150;
    static constexpr uint R10x6G10x6B10x6A10x6_UNorm = 151;
    static constexpr uint R10x6G10x6_UNorm = 152;
    static constexpr uint R10x6_UNorm = 153;
    static constexpr uint B12x4G12x4R12x4G12x4_422_UNorm = 154;
    static constexpr uint G12x4B12x4G12x4R12x4_422_UNorm = 155;
    static constexpr uint G12x4_B12x4R12x4_420_UNorm = 156;
    static constexpr uint G12x4_B12x4R12x4_422_UNorm = 157;
    static constexpr uint G12x4_B12x4_R12x4_420_UNorm = 158;
    static constexpr uint G12x4_B12x4_R12x4_422_UNorm = 159;
    static constexpr uint G12x4_B12x4_R12x4_444_UNorm = 160;
    static constexpr uint R12x4G12x4B12x4A12x4_UNorm = 161;
    static constexpr uint R12x4G12x4_UNorm = 162;
    static constexpr uint R12x4_UNorm = 163;
    static constexpr uint B16G16R16G16_422_UNorm = 164;
    static constexpr uint G16B16G16R16_422_UNorm = 165;
    static constexpr uint G16_B16R16_420_UNorm = 166;
    static constexpr uint G16_B16R16_422_UNorm = 167;
    static constexpr uint G16_B16_R16_420_UNorm = 168;
    static constexpr uint G16_B16_R16_422_UNorm = 169;
    static constexpr uint G16_B16_R16_444_UNorm = 170;
    static constexpr uint SwapchainColor = 254;
};

struct EAttachmentLoadOp
{
    EAttachmentLoadOp () {}
    EAttachmentLoadOp (uint) {}
    operator uint () const;
    static constexpr uint Invalidate = 0;
    static constexpr uint Load = 1;
    static constexpr uint Clear = 2;
    static constexpr uint None = 3;
};

struct EAttachmentStoreOp
{
    EAttachmentStoreOp () {}
    EAttachmentStoreOp (uint) {}
    operator uint () const;
    static constexpr uint Invalidate = 0;
    static constexpr uint Store = 1;
    static constexpr uint None = 2;
};

struct ECompareOp
{
    ECompareOp () {}
    ECompareOp (uint) {}
    operator uint () const;
    static constexpr uint Never = 0;
    static constexpr uint Less = 1;
    static constexpr uint Equal = 2;
    static constexpr uint LEqual = 3;
    static constexpr uint Greater = 4;
    static constexpr uint NotEqual = 5;
    static constexpr uint GEqual = 6;
    static constexpr uint Always = 7;
};

struct EBlendFactor
{
    EBlendFactor () {}
    EBlendFactor (uint) {}
    operator uint () const;
    static constexpr uint Zero = 0;
    static constexpr uint One = 1;
    static constexpr uint SrcColor = 2;
    static constexpr uint OneMinusSrcColor = 3;
    static constexpr uint DstColor = 4;
    static constexpr uint OneMinusDstColor = 5;
    static constexpr uint SrcAlpha = 6;
    static constexpr uint OneMinusSrcAlpha = 7;
    static constexpr uint DstAlpha = 8;
    static constexpr uint OneMinusDstAlpha = 9;
    static constexpr uint ConstColor = 10;
    static constexpr uint OneMinusConstColor = 11;
    static constexpr uint ConstAlpha = 12;
    static constexpr uint OneMinusConstAlpha = 13;
    static constexpr uint SrcAlphaSaturate = 14;
    static constexpr uint Src1Color = 15;
    static constexpr uint OneMinusSrc1Color = 16;
    static constexpr uint Src1Alpha = 17;
    static constexpr uint OneMinusSrc1Alpha = 18;
};

struct EBlendOp
{
    EBlendOp () {}
    EBlendOp (uint) {}
    operator uint () const;
    static constexpr uint Add = 0;
    static constexpr uint Sub = 1;
    static constexpr uint RevSub = 2;
    static constexpr uint Min = 3;
    static constexpr uint Max = 4;
};

struct ELogicOp
{
    ELogicOp () {}
    ELogicOp (uint) {}
    operator uint () const;
    static constexpr uint None = 0;
    static constexpr uint Clear = 1;
    static constexpr uint Set = 2;
    static constexpr uint Copy = 3;
    static constexpr uint CopyInverted = 4;
    static constexpr uint NoOp = 5;
    static constexpr uint Invert = 6;
    static constexpr uint And = 7;
    static constexpr uint NotAnd = 8;
    static constexpr uint Or = 9;
    static constexpr uint NotOr = 10;
    static constexpr uint Xor = 11;
    static constexpr uint Equiv = 12;
    static constexpr uint AndReverse = 13;
    static constexpr uint AndInverted = 14;
    static constexpr uint OrReverse = 15;
    static constexpr uint OrInverted = 16;
};

struct EStencilOp
{
    EStencilOp () {}
    EStencilOp (uint) {}
    operator uint () const;
    static constexpr uint Keep = 0;
    static constexpr uint Zero = 1;
    static constexpr uint Replace = 2;
    static constexpr uint Incr = 3;
    static constexpr uint IncrWrap = 4;
    static constexpr uint Decr = 5;
    static constexpr uint DecrWrap = 6;
    static constexpr uint Invert = 7;
};

struct EPolygonMode
{
    EPolygonMode () {}
    EPolygonMode (uint) {}
    operator uint () const;
    static constexpr uint Point = 0;
    static constexpr uint Line = 1;
    static constexpr uint Fill = 2;
};

struct EPrimitive
{
    EPrimitive () {}
    EPrimitive (uint) {}
    operator uint () const;
    static constexpr uint Point = 0;
    static constexpr uint LineList = 1;
    static constexpr uint LineStrip = 2;
    static constexpr uint LineListAdjacency = 3;
    static constexpr uint LineStripAdjacency = 4;
    static constexpr uint TriangleList = 5;
    static constexpr uint TriangleStrip = 6;
    static constexpr uint TriangleFan = 7;
    static constexpr uint TriangleListAdjacency = 8;
    static constexpr uint TriangleStripAdjacency = 9;
    static constexpr uint Patch = 10;
};

struct ECullMode
{
    ECullMode () {}
    ECullMode (uint) {}
    operator uint () const;
    static constexpr uint None = 0;
    static constexpr uint Front = 1;
    static constexpr uint Back = 2;
    static constexpr uint FontAndBack = 3;
};

struct EPipelineDynamicState
{
    EPipelineDynamicState () {}
    EPipelineDynamicState (uint) {}
    operator uint () const;
    static constexpr uint None = 0;
    static constexpr uint StencilCompareMask = 1;
    static constexpr uint StencilWriteMask = 2;
    static constexpr uint StencilReference = 4;
    static constexpr uint DepthBias = 8;
    static constexpr uint BlendConstants = 16;
    static constexpr uint RTStackSize = 64;
    static constexpr uint FragmentShadingRate = 128;
};

struct EResourceState
{
    EResourceState () {}
    EResourceState (uint) {}
    operator uint () const;
    static constexpr uint Unknown = 0;
    static constexpr uint Preserve = 1;
    static constexpr uint ShaderStorage_Read = 271;
    static constexpr uint ShaderStorage_Write = 528;
    static constexpr uint ShaderStorage_RW = 785;
    static constexpr uint ShaderUniform = 274;
    static constexpr uint ShaderSample = 275;
    static constexpr uint CopySrc = 258;
    static constexpr uint CopyDst = 515;
    static constexpr uint ClearDst = 516;
    static constexpr uint BlitSrc = 261;
    static constexpr uint BlitDst = 518;
    static constexpr uint InputColorAttachment = 276;
    static constexpr uint InputColorAttachment_RW = 533;
    static constexpr uint ColorAttachment = 519;
    static constexpr uint ColorAttachment_Blend = 775;
    static constexpr uint DepthStencilAttachment_Read = 1288;
    static constexpr uint DepthStencilAttachment_Write = 2569;
    static constexpr uint DepthStencilAttachment_RW = 3849;
    static constexpr uint DepthTest_StencilRW = 3338;
    static constexpr uint DepthRW_StencilTest = 1803;
    static constexpr uint DepthStencilTest_ShaderSample = 1304;
    static constexpr uint DepthTest_DepthSample_StencilRW = 3353;
    static constexpr uint InputDepthStencilAttachment = 1302;
    static constexpr uint InputDepthStencilAttachment_RW = 3863;
    static constexpr uint Host_Read = 282;
    static constexpr uint Host_Write = 539;
    static constexpr uint PresentImage = 268;
    static constexpr uint IndirectBuffer = 285;
    static constexpr uint IndexBuffer = 286;
    static constexpr uint VertexBuffer = 287;
    static constexpr uint ShadingRateImage = 269;
    static constexpr uint CopyRTAS_Read = 288;
    static constexpr uint CopyRTAS_Write = 545;
    static constexpr uint BuildRTAS_Read = 290;
    static constexpr uint BuildRTAS_Write = 547;
    static constexpr uint BuildRTAS_RW = 803;
    static constexpr uint BuildRTAS_IndirectBuffer = 292;
    static constexpr uint ShaderRTAS = 284;
    static constexpr uint RTShaderBindingTable = 293;
    static constexpr uint DSTestBeforeFS = 4096;
    static constexpr uint DSTestAfterFS = 8192;
    static constexpr uint Invalidate = 16384;
    static constexpr uint General = 782;
    static constexpr uint MeshTaskShader = 32768;
    static constexpr uint VertexProcessingShaders = 65536;
    static constexpr uint TileShader = 131072;
    static constexpr uint FragmentShader = 262144;
    static constexpr uint PreRasterizationShaders = 98304;
    static constexpr uint PostRasterizationShaders = 393216;
    static constexpr uint ComputeShader = 524288;
    static constexpr uint RayTracingShaders = 1048576;
    static constexpr uint AllGraphicsShaders = 491520;
    static constexpr uint AllShaders = 2064384;
    static constexpr uint BuildRTAS_ScratchBuffer = 803;
    static constexpr uint InputDepthAttachment = 1302;
    static constexpr uint DepthStencilAttachment = 16137;
};

struct EImageAspect
{
    EImageAspect () {}
    EImageAspect (uint) {}
    operator uint () const;
    static constexpr uint Color = 1;
    static constexpr uint Depth = 2;
    static constexpr uint Stencil = 4;
    static constexpr uint Plane_0 = 16;
    static constexpr uint Plane_1 = 32;
    static constexpr uint Plane_2 = 64;
};

struct EShaderIO
{
    EShaderIO () {}
    EShaderIO (uint) {}
    operator uint () const;
    static constexpr uint Int = 1;
    static constexpr uint UInt = 2;
    static constexpr uint Float = 3;
    static constexpr uint UFloat = 4;
    static constexpr uint Half = 5;
    static constexpr uint UNorm = 6;
    static constexpr uint SNorm = 7;
    static constexpr uint sRGB = 8;
    static constexpr uint AnyColor = 9;
    static constexpr uint Depth = 10;
    static constexpr uint Stencil = 11;
    static constexpr uint DepthStencil = 12;
};

struct ESubgroupTypes
{
    ESubgroupTypes () {}
    ESubgroupTypes (uint) {}
    operator uint () const;
    static constexpr uint Float16 = 32;
    static constexpr uint Float32 = 1;
    static constexpr uint Int8 = 4;
    static constexpr uint Int16 = 8;
    static constexpr uint Int32 = 2;
    static constexpr uint Int64 = 16;
};

struct ESubgroupOperation
{
    ESubgroupOperation () {}
    ESubgroupOperation (uint) {}
    operator uint () const;
    static constexpr uint IndexAndSize = 0;
    static constexpr uint Elect = 1;
    static constexpr uint Barrier = 2;
    static constexpr uint Any = 3;
    static constexpr uint All = 4;
    static constexpr uint AllEqual = 5;
    static constexpr uint Add = 6;
    static constexpr uint Mul = 7;
    static constexpr uint Min = 8;
    static constexpr uint Max = 9;
    static constexpr uint And = 10;
    static constexpr uint Or = 11;
    static constexpr uint Xor = 12;
    static constexpr uint InclusiveMul = 13;
    static constexpr uint InclusiveAdd = 14;
    static constexpr uint InclusiveMin = 15;
    static constexpr uint InclusiveMax = 16;
    static constexpr uint InclusiveAnd = 17;
    static constexpr uint InclusiveOr = 18;
    static constexpr uint InclusiveXor = 19;
    static constexpr uint ExclusiveAdd = 20;
    static constexpr uint ExclusiveMul = 21;
    static constexpr uint ExclusiveMin = 22;
    static constexpr uint ExclusiveMax = 23;
    static constexpr uint ExclusiveAnd = 24;
    static constexpr uint ExclusiveOr = 25;
    static constexpr uint ExclusiveXor = 26;
    static constexpr uint Ballot = 27;
    static constexpr uint Broadcast = 28;
    static constexpr uint BroadcastFirst = 29;
    static constexpr uint InverseBallot = 30;
    static constexpr uint BallotBitExtract = 31;
    static constexpr uint BallotBitCount = 32;
    static constexpr uint BallotInclusiveBitCount = 33;
    static constexpr uint BallotExclusiveBitCount = 34;
    static constexpr uint BallotFindLSB = 35;
    static constexpr uint BallotFindMSB = 36;
    static constexpr uint Shuffle = 37;
    static constexpr uint ShuffleXor = 38;
    static constexpr uint ShuffleUp = 39;
    static constexpr uint ShuffleDown = 40;
    static constexpr uint ClusteredAdd = 41;
    static constexpr uint ClusteredMul = 42;
    static constexpr uint ClusteredMin = 43;
    static constexpr uint ClusteredMax = 44;
    static constexpr uint ClusteredAnd = 45;
    static constexpr uint ClusteredOr = 46;
    static constexpr uint ClusteredXor = 47;
    static constexpr uint QuadBroadcast = 48;
    static constexpr uint QuadSwapHorizontal = 49;
    static constexpr uint QuadSwapVertical = 50;
    static constexpr uint QuadSwapDiagonal = 51;
    static constexpr uint _Basic_Begin = 0;
    static constexpr uint _Basic_End = 2;
    static constexpr uint _Vote_Begin = 3;
    static constexpr uint _Vote_End = 5;
    static constexpr uint _Arithmetic_Begin = 6;
    static constexpr uint _Arithmetic_End = 26;
    static constexpr uint _Ballot_Begin = 27;
    static constexpr uint _Ballot_End = 36;
    static constexpr uint _Shuffle_Begin = 37;
    static constexpr uint _Shuffle_End = 38;
    static constexpr uint _ShuffleRelative_Begin = 39;
    static constexpr uint _ShuffleRelative_End = 40;
    static constexpr uint _Clustered_Begin = 41;
    static constexpr uint _Clustered_End = 47;
    static constexpr uint _Quad_Begin = 48;
    static constexpr uint _Quad_End = 51;
};

struct EFeature
{
    EFeature () {}
    EFeature (uint) {}
    operator uint () const;
    static constexpr uint Ignore = 0;
    static constexpr uint RequireTrue = 2;
    static constexpr uint RequireFalse = 1;
};

struct EShader
{
    EShader () {}
    EShader (uint) {}
    operator uint () const;
    static constexpr uint Vertex = 0;
    static constexpr uint TessControl = 1;
    static constexpr uint TessEvaluation = 2;
    static constexpr uint Geometry = 3;
    static constexpr uint Fragment = 4;
    static constexpr uint Compute = 5;
    static constexpr uint Tile = 6;
    static constexpr uint MeshTask = 7;
    static constexpr uint Mesh = 8;
    static constexpr uint RayGen = 9;
    static constexpr uint RayAnyHit = 10;
    static constexpr uint RayClosestHit = 11;
    static constexpr uint RayMiss = 12;
    static constexpr uint RayIntersection = 13;
    static constexpr uint RayCallable = 14;
};

struct EShaderStages
{
    EShaderStages () {}
    EShaderStages (uint) {}
    operator uint () const;
    static constexpr uint Vertex = 1;
    static constexpr uint TessControl = 2;
    static constexpr uint TessEvaluation = 4;
    static constexpr uint Geometry = 8;
    static constexpr uint Fragment = 16;
    static constexpr uint Compute = 32;
    static constexpr uint Tile = 64;
    static constexpr uint MeshTask = 128;
    static constexpr uint Mesh = 256;
    static constexpr uint RayGen = 512;
    static constexpr uint RayAnyHit = 1024;
    static constexpr uint RayClosestHit = 2048;
    static constexpr uint RayMiss = 4096;
    static constexpr uint RayIntersection = 8192;
    static constexpr uint RayCallable = 16384;
    static constexpr uint All = 32767;
    static constexpr uint AllGraphics = 415;
    static constexpr uint AllRayTracing = 32256;
};

struct EVendorID
{
    EVendorID () {}
    EVendorID (uint) {}
    operator uint () const;
    static constexpr uint AMD = 0;
    static constexpr uint NVidia = 1;
    static constexpr uint Intel = 2;
    static constexpr uint ARM = 3;
    static constexpr uint Qualcomm = 4;
    static constexpr uint ImgTech = 5;
    static constexpr uint Microsoft = 6;
    static constexpr uint Apple = 7;
    static constexpr uint Mesa = 8;
    static constexpr uint Broadcom = 9;
    static constexpr uint Samsung = 10;
    static constexpr uint VeriSilicon = 11;
};

struct EVertexType
{
    EVertexType () {}
    EVertexType (uint) {}
    operator uint () const;
    static constexpr uint Byte = 4;
    static constexpr uint Byte2 = 5;
    static constexpr uint Byte3 = 6;
    static constexpr uint Byte4 = 7;
    static constexpr uint Byte_Norm = 68;
    static constexpr uint Byte2_Norm = 69;
    static constexpr uint Byte3_Norm = 70;
    static constexpr uint Byte4_Norm = 71;
    static constexpr uint Byte_Scaled = 132;
    static constexpr uint Byte2_Scaled = 133;
    static constexpr uint Byte3_Scaled = 134;
    static constexpr uint Byte4_Scaled = 135;
    static constexpr uint UByte = 8;
    static constexpr uint UByte2 = 9;
    static constexpr uint UByte3 = 10;
    static constexpr uint UByte4 = 11;
    static constexpr uint UByte_Norm = 72;
    static constexpr uint UByte2_Norm = 73;
    static constexpr uint UByte3_Norm = 74;
    static constexpr uint UByte4_Norm = 75;
    static constexpr uint UByte_Scaled = 136;
    static constexpr uint UByte2_Scaled = 137;
    static constexpr uint UByte3_Scaled = 138;
    static constexpr uint UByte4_Scaled = 139;
    static constexpr uint Short = 12;
    static constexpr uint Short2 = 13;
    static constexpr uint Short3 = 14;
    static constexpr uint Short4 = 15;
    static constexpr uint Short_Norm = 76;
    static constexpr uint Short2_Norm = 77;
    static constexpr uint Short3_Norm = 78;
    static constexpr uint Short4_Norm = 79;
    static constexpr uint Short_Scaled = 140;
    static constexpr uint Short2_Scaled = 141;
    static constexpr uint Short3_Scaled = 142;
    static constexpr uint Short4_Scaled = 143;
    static constexpr uint UShort = 16;
    static constexpr uint UShort2 = 17;
    static constexpr uint UShort3 = 18;
    static constexpr uint UShort4 = 19;
    static constexpr uint UShort_Norm = 80;
    static constexpr uint UShort2_Norm = 81;
    static constexpr uint UShort3_Norm = 82;
    static constexpr uint UShort4_Norm = 83;
    static constexpr uint UShort_Scaled = 144;
    static constexpr uint UShort2_Scaled = 145;
    static constexpr uint UShort3_Scaled = 146;
    static constexpr uint UShort4_Scaled = 147;
    static constexpr uint Int = 20;
    static constexpr uint Int2 = 21;
    static constexpr uint Int3 = 22;
    static constexpr uint Int4 = 23;
    static constexpr uint UInt = 24;
    static constexpr uint UInt2 = 25;
    static constexpr uint UInt3 = 26;
    static constexpr uint UInt4 = 27;
    static constexpr uint Long = 28;
    static constexpr uint Long2 = 29;
    static constexpr uint Long3 = 30;
    static constexpr uint Long4 = 31;
    static constexpr uint ULong = 32;
    static constexpr uint ULong2 = 33;
    static constexpr uint ULong3 = 34;
    static constexpr uint ULong4 = 35;
    static constexpr uint Half = 36;
    static constexpr uint Half2 = 37;
    static constexpr uint Half3 = 38;
    static constexpr uint Half4 = 39;
    static constexpr uint Float = 40;
    static constexpr uint Float2 = 41;
    static constexpr uint Float3 = 42;
    static constexpr uint Float4 = 43;
    static constexpr uint Double = 44;
    static constexpr uint Double2 = 45;
    static constexpr uint Double3 = 46;
    static constexpr uint Double4 = 47;
    static constexpr uint UInt_2_10_10_10 = 51;
    static constexpr uint UInt_2_10_10_10_Norm = 115;
    static constexpr uint UInt_2_10_10_10_Scaled = 179;
};

struct EGraphicsDeviceID
{
    EGraphicsDeviceID () {}
    EGraphicsDeviceID (uint) {}
    operator uint () const;
    static constexpr uint Adreno_500 = 0;
    static constexpr uint Adreno_600 = 1;
    static constexpr uint Adreno_600_QC4 = 2;
    static constexpr uint Adreno_600_OC4 = 3;
    static constexpr uint Adreno_600_QC5 = 4;
    static constexpr uint Adreno_700_SC3 = 5;
    static constexpr uint Adreno_700_DC4_SC5 = 6;
    static constexpr uint AMD_GCN1 = 7;
    static constexpr uint AMD_GCN2 = 8;
    static constexpr uint AMD_GCN3 = 9;
    static constexpr uint AMD_GCN4 = 10;
    static constexpr uint AMD_GCN5 = 11;
    static constexpr uint AMD_GCN5_APU = 12;
    static constexpr uint AMD_RDNA1 = 13;
    static constexpr uint AMD_RDNA2 = 14;
    static constexpr uint AMD_RDNA2_APU = 15;
    static constexpr uint AMD_RDNA3 = 16;
    static constexpr uint AMD_RDNA3_APU = 17;
    static constexpr uint Apple_A8 = 18;
    static constexpr uint Apple_A9_A10 = 19;
    static constexpr uint Apple_A11 = 20;
    static constexpr uint Apple_A12 = 21;
    static constexpr uint Apple_A13 = 22;
    static constexpr uint Apple_A14_M1 = 23;
    static constexpr uint Apple_A15_M2 = 24;
    static constexpr uint Apple_A16 = 25;
    static constexpr uint Mali_Midgard_Gen2 = 26;
    static constexpr uint Mali_Midgard_Gen3 = 27;
    static constexpr uint Mali_Midgard_Gen4 = 28;
    static constexpr uint Mali_Bifrost_Gen1 = 29;
    static constexpr uint Mali_Bifrost_Gen2 = 30;
    static constexpr uint Mali_Bifrost_Gen3 = 31;
    static constexpr uint Mali_Valhall_Gen1 = 32;
    static constexpr uint Mali_Valhall_Gen2 = 33;
    static constexpr uint Mali_Valhall_Gen3 = 34;
    static constexpr uint Mali_Valhall_Gen4 = 35;
    static constexpr uint NV_Maxwell = 36;
    static constexpr uint NV_Maxwell_Tegra = 37;
    static constexpr uint NV_Pascal = 38;
    static constexpr uint NV_Pascal_MX = 39;
    static constexpr uint NV_Pascal_Tegra = 40;
    static constexpr uint NV_Volta = 41;
    static constexpr uint NV_Turing_16 = 42;
    static constexpr uint NV_Turing = 43;
    static constexpr uint NV_Turing_MX = 44;
    static constexpr uint NV_Ampere = 45;
    static constexpr uint NV_Ampere_Orin = 46;
    static constexpr uint NV_Ada = 47;
    static constexpr uint Intel_Gen9 = 48;
    static constexpr uint Intel_Gen11 = 49;
    static constexpr uint Intel_Gen12 = 50;
    static constexpr uint PowerVR_Series8XE = 51;
    static constexpr uint PowerVR_Series8XEP = 52;
    static constexpr uint PowerVR_Series8XT = 53;
    static constexpr uint PowerVR_Series9XE = 54;
    static constexpr uint VeriSilicon = 55;
    static constexpr uint V3D_4 = 56;
    static constexpr uint SwiftShader = 57;
};

struct EFilter
{
    EFilter () {}
    EFilter (uint) {}
    operator uint () const;
    static constexpr uint Nearest = 0;
    static constexpr uint Linear = 1;
};

struct EMipmapFilter
{
    EMipmapFilter () {}
    EMipmapFilter (uint) {}
    operator uint () const;
    static constexpr uint None = 0;
    static constexpr uint Nearest = 1;
    static constexpr uint Linear = 2;
};

struct EAddressMode
{
    EAddressMode () {}
    EAddressMode (uint) {}
    operator uint () const;
    static constexpr uint Repeat = 0;
    static constexpr uint MirrorRepeat = 1;
    static constexpr uint ClampToEdge = 2;
    static constexpr uint ClampToBorder = 3;
    static constexpr uint MirrorClampToEdge = 4;
    static constexpr uint Clamp = 2;
    static constexpr uint MirrorClamp = 4;
};

struct EBorderColor
{
    EBorderColor () {}
    EBorderColor (uint) {}
    operator uint () const;
    static constexpr uint FloatTransparentBlack = 0;
    static constexpr uint FloatOpaqueBlack = 1;
    static constexpr uint FloatOpaqueWhite = 2;
    static constexpr uint IntTransparentBlack = 3;
    static constexpr uint IntOpaqueBlack = 4;
    static constexpr uint IntOpaqueWhite = 5;
};

struct EReductionMode
{
    EReductionMode () {}
    EReductionMode (uint) {}
    operator uint () const;
    static constexpr uint Average = 0;
    static constexpr uint Min = 1;
    static constexpr uint Max = 2;
};

struct ESamplerUsage
{
    ESamplerUsage () {}
    ESamplerUsage (uint) {}
    operator uint () const;
    static constexpr uint Default = 0;
    static constexpr uint Subsampled = 1;
    static constexpr uint SubsampledCoarseReconstruction = 2;
};

struct EVertexInputRate
{
    EVertexInputRate () {}
    EVertexInputRate (uint) {}
    operator uint () const;
    static constexpr uint Vertex = 0;
    static constexpr uint Instance = 1;
};

struct EDescSetUsage
{
    EDescSetUsage () {}
    EDescSetUsage (uint) {}
    operator uint () const;
    static constexpr uint AllowPartialyUpdate = 1;
    static constexpr uint UpdateTemplate = 2;
    static constexpr uint ArgumentBuffer = 4;
    static constexpr uint MutableArgBuffer = 8;
};

struct EPipelineOpt
{
    EPipelineOpt () {}
    EPipelineOpt (uint) {}
    operator uint () const;
    static constexpr uint Optimize = 1;
    static constexpr uint CS_DispatchBase = 2;
    static constexpr uint RT_NoNullAnyHitShaders = 4;
    static constexpr uint RT_NoNullClosestHitShaders = 8;
    static constexpr uint RT_NoNullMissShaders = 16;
    static constexpr uint RT_NoNullIntersectionShaders = 32;
    static constexpr uint RT_SkipTriangles = 64;
    static constexpr uint RT_SkipAABBs = 128;
};

struct EQueueMask
{
    EQueueMask () {}
    EQueueMask (uint) {}
    operator uint () const;
    static constexpr uint Graphics = 1;
    static constexpr uint AsyncCompute = 2;
    static constexpr uint AsyncTransfer = 4;
    static constexpr uint VideoEncode = 8;
    static constexpr uint VideoDecode = 16;
};

struct ESamplerChromaLocation
{
    ESamplerChromaLocation () {}
    ESamplerChromaLocation (uint) {}
    operator uint () const;
    static constexpr uint CositedEven = 0;
    static constexpr uint Midpoint = 1;
};

struct ESamplerYcbcrModelConversion
{
    ESamplerYcbcrModelConversion () {}
    ESamplerYcbcrModelConversion (uint) {}
    operator uint () const;
    static constexpr uint RGB_Identity = 0;
    static constexpr uint Ycbcr_Identity = 1;
    static constexpr uint Ycbcr_709 = 2;
    static constexpr uint Ycbcr_601 = 3;
    static constexpr uint Ycbcr_2020 = 4;
};

struct ESamplerYcbcrRange
{
    ESamplerYcbcrRange () {}
    ESamplerYcbcrRange (uint) {}
    operator uint () const;
    static constexpr uint ITU_Full = 0;
    static constexpr uint ITU_Narrow = 1;
};

struct ESurfaceFormat
{
    ESurfaceFormat () {}
    ESurfaceFormat (uint) {}
    operator uint () const;
    static constexpr uint BGRA8_sRGB_nonlinear = 0;
    static constexpr uint RGBA8_sRGB_nonlinear = 1;
    static constexpr uint BGRA8_BT709_nonlinear = 2;
    static constexpr uint RGBA16F_Extended_sRGB_linear = 3;
    static constexpr uint RGBA16F_sRGB_nonlinear = 4;
    static constexpr uint RGBA16F_BT709_nonlinear = 5;
    static constexpr uint RGBA16F_HDR10_ST2084 = 6;
    static constexpr uint RGBA16F_BT2020_linear = 7;
    static constexpr uint RGB10A2_sRGB_nonlinear = 8;
    static constexpr uint RGB10A2_HDR10_ST2084 = 9;
};

struct ERTInstanceOpt
{
    ERTInstanceOpt () {}
    ERTInstanceOpt (uint) {}
    operator uint () const;
    static constexpr uint TriangleCullDisable = 1;
    static constexpr uint TriangleFrontCCW = 2;
    static constexpr uint ForceOpaque = 4;
    static constexpr uint ForceNonOpaque = 8;
    static constexpr uint TriangleCullBack = 0;
    static constexpr uint TriangleFrontCW = 0;
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

struct RenderState_ColorBuffer_ColorMask
{
    RenderState_ColorBuffer_ColorMask ();
    RenderState_ColorBuffer_ColorMask (const RenderState_ColorBuffer_ColorMask&);
    RenderState_ColorBuffer_ColorMask&  operator = (const RenderState_ColorBuffer_ColorMask&);
    void  setR (bool);
    void  setG (bool);
    void  setB (bool);
    void  setA (bool);
    void  setRGBA (bool);
};

struct RenderState_ColorBuffer
{
    RenderState_ColorBuffer ();
    RenderState_ColorBuffer (const RenderState_ColorBuffer&);
    RenderState_ColorBuffer&  operator = (const RenderState_ColorBuffer&);
    void  SrcBlendFactor (EBlendFactor);
    void  SrcBlendFactor (EBlendFactor, EBlendFactor);
    void  DstBlendFactor (EBlendFactor);
    void  DstBlendFactor (EBlendFactor, EBlendFactor);
    void  BlendOp (EBlendOp);
    void  BlendOp (EBlendOp, EBlendOp);
    bool blend;
    RenderState_ColorBuffer_ColorMask colorMask;
};

struct RenderState_ColorBuffersState
{
    RenderState_ColorBuffersState ();
    RenderState_ColorBuffersState (const RenderState_ColorBuffersState&);
    RenderState_ColorBuffersState&  operator = (const RenderState_ColorBuffersState&);
    void  SetColorBuffer (uint, const RenderState_ColorBuffer &);
    const RenderState_ColorBuffer &  GetColorBuffer (uint) const;
    ELogicOp logicOp;
    RGBA32f blendColor;
};

struct RenderState_StencilFaceState
{
    RenderState_StencilFaceState ();
    RenderState_StencilFaceState (const RenderState_StencilFaceState&);
    RenderState_StencilFaceState&  operator = (const RenderState_StencilFaceState&);
    EStencilOp failOp;
    EStencilOp depthFailOp;
    EStencilOp passOp;
    ECompareOp compareOp;
    uint8 reference;
    uint8 writeMask;
    uint8 compareMask;
};

struct RenderState_StencilBufferState
{
    RenderState_StencilBufferState ();
    RenderState_StencilBufferState (const RenderState_StencilBufferState&);
    RenderState_StencilBufferState&  operator = (const RenderState_StencilBufferState&);
    RenderState_StencilFaceState front;
    RenderState_StencilFaceState back;
    bool enabled;
};

struct RenderState_DepthBufferState
{
    RenderState_DepthBufferState ();
    RenderState_DepthBufferState (const RenderState_DepthBufferState&);
    RenderState_DepthBufferState&  operator = (const RenderState_DepthBufferState&);
    float minBounds;
    float maxBounds;
    bool bounds;
    ECompareOp compareOp;
    bool write;
    bool test;
};

struct RenderState_InputAssemblyState
{
    RenderState_InputAssemblyState ();
    RenderState_InputAssemblyState (const RenderState_InputAssemblyState&);
    RenderState_InputAssemblyState&  operator = (const RenderState_InputAssemblyState&);
    EPrimitive topology;
    bool primitiveRestart;
};

struct RenderState_RasterizationState
{
    RenderState_RasterizationState ();
    RenderState_RasterizationState (const RenderState_RasterizationState&);
    RenderState_RasterizationState&  operator = (const RenderState_RasterizationState&);
    float depthBiasConstFactor;
    float depthBiasClamp;
    float depthBiasSlopeFactor;
    bool depthBias;
    EPolygonMode polygonMode;
    bool depthClamp;
    bool rasterizerDiscard;
    bool frontFaceCCW;
    ECullMode cullMode;
};

struct RenderState_MultisampleState
{
    RenderState_MultisampleState ();
    RenderState_MultisampleState (const RenderState_MultisampleState&);
    RenderState_MultisampleState&  operator = (const RenderState_MultisampleState&);
    uint sampleMask;
    float minSampleShading;
    MultiSamples samples;
    bool sampleShading;
    bool alphaToCoverage;
    bool alphaToOne;
};

struct RenderState
{
    RenderState ();
    RenderState (const RenderState&);
    RenderState&  operator = (const RenderState&);
    RenderState_ColorBuffersState color;
    RenderState_DepthBufferState depth;
    RenderState_StencilBufferState stencil;
    RenderState_InputAssemblyState inputAssembly;
    RenderState_RasterizationState rasterization;
    RenderState_MultisampleState multisample;
};

struct EShaderVersion
{
    EShaderVersion () {}
    EShaderVersion (uint) {}
    operator uint () const;
    static constexpr uint SPIRV_1_0 = 268435472;
    static constexpr uint SPIRV_1_1 = 268435473;
    static constexpr uint SPIRV_1_2 = 268435474;
    static constexpr uint SPIRV_1_3 = 268435475;
    static constexpr uint SPIRV_1_4 = 268435476;
    static constexpr uint SPIRV_1_5 = 268435477;
    static constexpr uint SPIRV_1_6 = 268435478;
    static constexpr uint Metal_2_0 = 1073741856;
    static constexpr uint Metal_2_1 = 1073741857;
    static constexpr uint Metal_2_2 = 1073741858;
    static constexpr uint Metal_2_3 = 1073741859;
    static constexpr uint Metal_2_4 = 1073741860;
    static constexpr uint Metal_3_0 = 1073741872;
    static constexpr uint Metal_iOS_2_0 = 536870944;
    static constexpr uint Metal_iOS_2_1 = 536870945;
    static constexpr uint Metal_iOS_2_2 = 536870946;
    static constexpr uint Metal_iOS_2_3 = 536870947;
    static constexpr uint Metal_iOS_2_4 = 536870948;
    static constexpr uint Metal_iOS_3_0 = 536870960;
    static constexpr uint Metal_Mac_2_0 = 805306400;
    static constexpr uint Metal_Mac_2_1 = 805306401;
    static constexpr uint Metal_Mac_2_2 = 805306402;
    static constexpr uint Metal_Mac_2_3 = 805306403;
    static constexpr uint Metal_Mac_2_4 = 805306404;
    static constexpr uint Metal_Mac_3_0 = 805306416;
};

struct EShaderOpt
{
    EShaderOpt () {}
    EShaderOpt (uint) {}
    operator uint () const;
    static constexpr uint DebugInfo = 1;
    static constexpr uint Trace = 2;
    static constexpr uint FnProfiling = 4;
    static constexpr uint TimeHeatMap = 8;
    static constexpr uint Optimize = 16;
    static constexpr uint OptimizeSize = 32;
    static constexpr uint StrongOptimization = 64;
    static constexpr uint WarnAsError = 1024;
};

struct EAccessType
{
    EAccessType () {}
    EAccessType (uint) {}
    operator uint () const;
    static constexpr uint Coherent = 1;
    static constexpr uint Volatile = 2;
    static constexpr uint Restrict = 3;
    static constexpr uint DeviceCoherent = 5;
    static constexpr uint QueueFamilyCoherent = 6;
    static constexpr uint WorkgroupCoherent = 7;
    static constexpr uint SubgroupCoherent = 8;
    static constexpr uint NonPrivate = 9;
};

struct EImageType
{
    EImageType () {}
    EImageType (uint) {}
    operator uint () const;
    static constexpr uint 1D = 1;
    static constexpr uint 1DArray = 2;
    static constexpr uint 2D = 3;
    static constexpr uint 2DArray = 4;
    static constexpr uint 2DMS = 5;
    static constexpr uint 2DMSArray = 6;
    static constexpr uint Cube = 7;
    static constexpr uint CubeArray = 8;
    static constexpr uint 3D = 9;
    static constexpr uint Buffer = 10;
    static constexpr uint Float = 16;
    static constexpr uint Half = 32;
    static constexpr uint SNorm = 48;
    static constexpr uint UNorm = 64;
    static constexpr uint Int = 80;
    static constexpr uint UInt = 96;
    static constexpr uint sRGB = 112;
    static constexpr uint Depth = 128;
    static constexpr uint Stencil = 144;
    static constexpr uint DepthStencil = 160;
    static constexpr uint Shadow = 256;
    static constexpr uint FImage1D = 17;
    static constexpr uint FImage2D = 19;
    static constexpr uint FImage2D_sRGB = 115;
    static constexpr uint FImage3D = 25;
    static constexpr uint FImage1DArray = 18;
    static constexpr uint FImage2DArray = 20;
    static constexpr uint FImageCube = 23;
    static constexpr uint FImageCubeArray = 24;
    static constexpr uint FImage2DMS = 21;
    static constexpr uint FImage2DMSArray = 22;
    static constexpr uint FImageBuffer = 26;
    static constexpr uint HImage1D = 33;
    static constexpr uint HImage2D = 35;
    static constexpr uint HImage3D = 41;
    static constexpr uint HImage1DArray = 34;
    static constexpr uint HImage2DArray = 36;
    static constexpr uint HImageCube = 39;
    static constexpr uint HImageCubeArray = 40;
    static constexpr uint HImage2DMS = 37;
    static constexpr uint HImage2DMSArray = 38;
    static constexpr uint HImageBuffer = 42;
    static constexpr uint Image1DShadow = 385;
    static constexpr uint Image2DShadow = 387;
    static constexpr uint Image1DArrayShadow = 386;
    static constexpr uint Image2DArrayShadow = 388;
    static constexpr uint ImageCubeShadow = 391;
    static constexpr uint ImageCubeArrayShadow = 392;
    static constexpr uint IImage1D = 81;
    static constexpr uint IImage2D = 83;
    static constexpr uint IImage3D = 89;
    static constexpr uint IImage1DArray = 82;
    static constexpr uint IImage2DArray = 84;
    static constexpr uint IImageCube = 87;
    static constexpr uint IImageCubeArray = 88;
    static constexpr uint IImage2DMS = 85;
    static constexpr uint IImage2DMSArray = 86;
    static constexpr uint IImageBuffer = 90;
    static constexpr uint UImage1D = 97;
    static constexpr uint UImage2D = 99;
    static constexpr uint UImage3D = 105;
    static constexpr uint UImage1DArray = 98;
    static constexpr uint UImage2DArray = 100;
    static constexpr uint UImageCube = 103;
    static constexpr uint UImageCubeArray = 104;
    static constexpr uint UImage2DMS = 101;
    static constexpr uint UImage2DMSArray = 102;
    static constexpr uint UImageBuffer = 106;
};

struct ECompilationTarget
{
    ECompilationTarget () {}
    ECompilationTarget (uint) {}
    operator uint () const;
    static constexpr uint Vulkan = 1;
    static constexpr uint Metal_iOS = 2;
    static constexpr uint Metal_Mac = 3;
};

struct EStructLayout
{
    EStructLayout () {}
    EStructLayout (uint) {}
    operator uint () const;
    static constexpr uint Compatible_Std140 = 0;
    static constexpr uint Compatible_Std430 = 1;
    static constexpr uint Metal = 2;
    static constexpr uint Std140 = 3;
    static constexpr uint Std430 = 4;
    static constexpr uint InternalIO = 5;
};

struct EValueType
{
    EValueType () {}
    EValueType (uint) {}
    operator uint () const;
    static constexpr uint Bool8 = 1;
    static constexpr uint Bool32 = 2;
    static constexpr uint Int8 = 3;
    static constexpr uint Int16 = 4;
    static constexpr uint Int32 = 5;
    static constexpr uint Int64 = 6;
    static constexpr uint UInt8 = 7;
    static constexpr uint UInt16 = 8;
    static constexpr uint UInt32 = 9;
    static constexpr uint UInt64 = 10;
    static constexpr uint Float16 = 11;
    static constexpr uint Float32 = 12;
    static constexpr uint Float64 = 13;
    static constexpr uint Int8_Norm = 14;
    static constexpr uint Int16_Norm = 15;
    static constexpr uint UInt8_Norm = 16;
    static constexpr uint UInt16_Norm = 17;
};

struct EShaderProprocessor
{
    EShaderProprocessor () {}
    EShaderProprocessor (uint) {}
    operator uint () const;
    static constexpr uint None = 0;
    static constexpr uint AEStyle = 1;
};

struct GlobalConfig
{
    GlobalConfig ();
    GlobalConfig (const GlobalConfig&);
    GlobalConfig&  operator = (const GlobalConfig&);
    void  SetTarget (ECompilationTarget);
    void  SetShaderVersion (EShaderVersion);
    void  SetSpirvToMslVersion (EShaderVersion);
    void  SetShaderOptions (EShaderOpt);
    void  SetDefaultLayout (EStructLayout);
    void  SetPipelineOptions (EPipelineOpt);
    void  SetPreprocessor (EShaderProprocessor);
    void  SetDefaultFeatureSet (const string &);
};

struct NamedRenderState
{
    NamedRenderState ();
    NamedRenderState (const string &);
    RenderState_ColorBuffersState color;
    RenderState_DepthBufferState depth;
    RenderState_StencilBufferState stencil;
    RenderState_InputAssemblyState inputAssembly;
    RenderState_RasterizationState rasterization;
    RenderState_MultisampleState multisample;
};

struct EFormatFeature
{
    EFormatFeature () {}
    EFormatFeature (uint) {}
    operator uint () const;
    static constexpr uint StorageImageAtomic = 1;
    static constexpr uint StorageImage = 2;
    static constexpr uint AttachmentBlend = 3;
    static constexpr uint Attachment = 4;
    static constexpr uint LinearSampled = 5;
    static constexpr uint UniformTexelBuffer = 6;
    static constexpr uint StorageTexelBuffer = 7;
    static constexpr uint StorageTexelBufferAtomic = 8;
    static constexpr uint HWCompressedAttachment = 9;
    static constexpr uint LossyCompressedAttachment = 10;
};

struct FeatureSet
{
    FeatureSet ();
    FeatureSet (const string &);
    void  AddTexelFormats (EFormatFeature, const array<EPixelFormat> &);
    void  AddSurfaceFormats (const array<ESurfaceFormat> &);
    void  AddSubgroupOperation (ESubgroupOperation);
    void  AddSubgroupOperations (const array<ESubgroupOperation> &);
    void  AddSubgroupOperationRange (ESubgroupOperation, ESubgroupOperation);
    void  AddVertexFormats (const array<EVertexType> &);
    void  AddAccelStructVertexFormats (const array<EVertexType> &);
    void  IncludeVendor (EVendorID);
    void  ExcludeVendor (EVendorID);
    void  IncludeVendors (const array<EVendorID> &);
    void  ExcludeVendors (const array<EVendorID> &);
    void  IncludeDevice (EGraphicsDeviceID);
    void  ExcludeDevice (EGraphicsDeviceID);
    void  MergeMin (RC<FeatureSet>);
    void  MergeMax (RC<FeatureSet>);
    void  Copy (RC<FeatureSet>);
    void  framebufferColorSampleCounts (const array<uint> &);
    void  framebufferDepthSampleCounts (const array<uint> &);
    void  minSpirvVersion (uint);
    void  minMetalVersion (uint);
    void  supportedQueues (EQueueMask);
    void  requiredQueues (EQueueMask);
    void  AddShadingRate (const array<uint> &, const array<uint> &);
    void  fragmentShadingRateTexelSize (const array<uint> &, const array<uint> &, uint);
    void  alphaToOne (EFeature);
    void  depthBiasClamp (EFeature);
    void  depthBounds (EFeature);
    void  depthClamp (EFeature);
    void  dualSrcBlend (EFeature);
    void  fillModeNonSolid (EFeature);
    void  independentBlend (EFeature);
    void  logicOp (EFeature);
    void  sampleRateShading (EFeature);
    void  constantAlphaColorBlendFactors (EFeature);
    void  pointPolygons (EFeature);
    void  separateStencilMaskRef (EFeature);
    void  triangleFans (EFeature);
    void  subgroupTypes (uint);
    void  subgroupStages (uint);
    void  subgroupQuadStages (uint);
    void  requiredSubgroupSizeStages (uint);
    void  subgroup (EFeature);
    void  subgroupBroadcastDynamicId (EFeature);
    void  subgroupSizeControl (EFeature);
    void  shaderSubgroupUniformControlFlow (EFeature);
    void  minSubgroupSize (uint);
    void  maxSubgroupSize (uint);
    void  shaderInt8 (EFeature);
    void  shaderInt16 (EFeature);
    void  shaderInt64 (EFeature);
    void  shaderFloat16 (EFeature);
    void  shaderFloat64 (EFeature);
    void  storageBuffer16BitAccess (EFeature);
    void  uniformAndStorageBuffer16BitAccess (EFeature);
    void  storageInputOutput16 (EFeature);
    void  storageBuffer8BitAccess (EFeature);
    void  uniformAndStorageBuffer8BitAccess (EFeature);
    void  uniformBufferStandardLayout (EFeature);
    void  scalarBlockLayout (EFeature);
    void  bufferDeviceAddress (EFeature);
    void  storagePushConstant8 (EFeature);
    void  storagePushConstant16 (EFeature);
    void  fragmentStoresAndAtomics (EFeature);
    void  vertexPipelineStoresAndAtomics (EFeature);
    void  shaderImageInt64Atomics (EFeature);
    void  shaderBufferInt64Atomics (EFeature);
    void  shaderSharedInt64Atomics (EFeature);
    void  shaderBufferFloat32Atomics (EFeature);
    void  shaderBufferFloat32AtomicAdd (EFeature);
    void  shaderBufferFloat64Atomics (EFeature);
    void  shaderBufferFloat64AtomicAdd (EFeature);
    void  shaderSharedFloat32Atomics (EFeature);
    void  shaderSharedFloat32AtomicAdd (EFeature);
    void  shaderSharedFloat64Atomics (EFeature);
    void  shaderSharedFloat64AtomicAdd (EFeature);
    void  shaderImageFloat32Atomics (EFeature);
    void  shaderImageFloat32AtomicAdd (EFeature);
    void  shaderBufferFloat16Atomics (EFeature);
    void  shaderBufferFloat16AtomicAdd (EFeature);
    void  shaderBufferFloat16AtomicMinMax (EFeature);
    void  shaderBufferFloat32AtomicMinMax (EFeature);
    void  shaderBufferFloat64AtomicMinMax (EFeature);
    void  shaderSharedFloat16Atomics (EFeature);
    void  shaderSharedFloat16AtomicAdd (EFeature);
    void  shaderSharedFloat16AtomicMinMax (EFeature);
    void  shaderSharedFloat32AtomicMinMax (EFeature);
    void  shaderSharedFloat64AtomicMinMax (EFeature);
    void  shaderImageFloat32AtomicMinMax (EFeature);
    void  sparseImageFloat32AtomicMinMax (EFeature);
    void  shaderOutputViewportIndex (EFeature);
    void  shaderOutputLayer (EFeature);
    void  shaderSubgroupClock (EFeature);
    void  shaderDeviceClock (EFeature);
    void  cooperativeMatrixNV (EFeature);
    void  shaderClipDistance (EFeature);
    void  shaderCullDistance (EFeature);
    void  shaderResourceMinLod (EFeature);
    void  shaderDrawParameters (EFeature);
    void  runtimeDescriptorArray (EFeature);
    void  shaderSMBuiltinsNV (EFeature);
    void  shaderCoreBuiltinsARM (EFeature);
    void  shaderSampleRateInterpolationFunctions (EFeature);
    void  shaderSampledImageArrayDynamicIndexing (EFeature);
    void  shaderStorageBufferArrayDynamicIndexing (EFeature);
    void  shaderStorageImageArrayDynamicIndexing (EFeature);
    void  shaderUniformBufferArrayDynamicIndexing (EFeature);
    void  shaderInputAttachmentArrayDynamicIndexing (EFeature);
    void  shaderUniformTexelBufferArrayDynamicIndexing (EFeature);
    void  shaderStorageTexelBufferArrayDynamicIndexing (EFeature);
    void  shaderUniformBufferArrayNonUniformIndexing (EFeature);
    void  shaderSampledImageArrayNonUniformIndexing (EFeature);
    void  shaderStorageBufferArrayNonUniformIndexing (EFeature);
    void  shaderStorageImageArrayNonUniformIndexing (EFeature);
    void  shaderInputAttachmentArrayNonUniformIndexing (EFeature);
    void  shaderUniformTexelBufferArrayNonUniformIndexing (EFeature);
    void  shaderStorageTexelBufferArrayNonUniformIndexing (EFeature);
    void  shaderStorageImageMultisample (EFeature);
    void  shaderStorageImageReadWithoutFormat (EFeature);
    void  shaderStorageImageWriteWithoutFormat (EFeature);
    void  vulkanMemoryModel (EFeature);
    void  vulkanMemoryModelDeviceScope (EFeature);
    void  vulkanMemoryModelAvailabilityVisibilityChains (EFeature);
    void  shaderDemoteToHelperInvocation (EFeature);
    void  shaderTerminateInvocation (EFeature);
    void  shaderZeroInitializeWorkgroupMemory (EFeature);
    void  shaderIntegerDotProduct (EFeature);
    void  fragmentShaderSampleInterlock (EFeature);
    void  fragmentShaderPixelInterlock (EFeature);
    void  fragmentShaderShadingRateInterlock (EFeature);
    void  fragmentShaderBarycentric (EFeature);
    void  pipelineFragmentShadingRate (EFeature);
    void  primitiveFragmentShadingRate (EFeature);
    void  attachmentFragmentShadingRate (EFeature);
    void  primitiveFragmentShadingRateWithMultipleViewports (EFeature);
    void  layeredShadingRateAttachments (EFeature);
    void  fragmentShadingRateWithShaderDepthStencilWrites (EFeature);
    void  fragmentShadingRateWithSampleMask (EFeature);
    void  fragmentShadingRateWithShaderSampleMask (EFeature);
    void  fragmentShadingRateWithFragmentShaderInterlock (EFeature);
    void  fragmentShadingRateWithCustomSampleLocations (EFeature);
    void  accelerationStructureIndirectBuild (EFeature);
    void  rayQuery (EFeature);
    void  rayQueryStages (uint);
    void  rayTracingPipeline (EFeature);
    void  rayTraversalPrimitiveCulling (EFeature);
    void  minRayRecursionDepth (uint);
    void  drawIndirectFirstInstance (EFeature);
    void  drawIndirectCount (EFeature);
    void  multiview (EFeature);
    void  multiviewGeometryShader (EFeature);
    void  multiviewTessellationShader (EFeature);
    void  minMultiviewViewCount (uint);
    void  multiViewport (EFeature);
    void  minViewports (uint);
    void  sampleLocations (EFeature);
    void  variableSampleLocations (EFeature);
    void  tessellationIsolines (EFeature);
    void  tessellationPointMode (EFeature);
    void  minTexelBufferElements (uint);
    void  minUniformBufferSize (uint);
    void  minStorageBufferSize (uint);
    void  minDescriptorSets (uint);
    void  minTexelOffset (uint);
    void  minTexelGatherOffset (uint);
    void  minFragmentOutputAttachments (uint);
    void  minFragmentDualSrcAttachments (uint);
    void  minFragmentCombinedOutputResources (uint);
    void  minPushConstantsSize (uint);
    void  minTotalThreadgroupSize (uint);
    void  minTotalTileMemory (uint);
    void  minVertAmplification (uint);
    void  minComputeSharedMemorySize (uint);
    void  minComputeWorkGroupInvocations (uint);
    void  minComputeWorkGroupSizeX (uint);
    void  minComputeWorkGroupSizeY (uint);
    void  minComputeWorkGroupSizeZ (uint);
    void  taskShader (EFeature);
    void  meshShader (EFeature);
    void  minTaskWorkGroupSize (uint);
    void  minMeshWorkGroupSize (uint);
    void  minMeshOutputVertices (uint);
    void  minMeshOutputPrimitives (uint);
    void  maxMeshOutputPerVertexGranularity (uint);
    void  maxMeshOutputPerPrimitiveGranularity (uint);
    void  minTaskPayloadSize (uint);
    void  minTaskSharedMemorySize (uint);
    void  minTaskPayloadAndSharedMemorySize (uint);
    void  minMeshSharedMemorySize (uint);
    void  minMeshPayloadAndSharedMemorySize (uint);
    void  minMeshOutputMemorySize (uint);
    void  minMeshPayloadAndOutputMemorySize (uint);
    void  minMeshMultiviewViewCount (uint);
    void  minPreferredTaskWorkGroupInvocations (uint);
    void  minPreferredMeshWorkGroupInvocations (uint);
    void  minRasterOrderGroups (uint);
    void  geometryShader (EFeature);
    void  tessellationShader (EFeature);
    void  computeShader (EFeature);
    void  tileShader (EFeature);
    void  vertexDivisor (EFeature);
    void  minVertexAttribDivisor (uint);
    void  minVertexAttributes (uint);
    void  minVertexBuffers (uint);
    void  imageCubeArray (EFeature);
    void  textureCompressionASTC_LDR (EFeature);
    void  textureCompressionASTC_HDR (EFeature);
    void  textureCompressionBC (EFeature);
    void  textureCompressionETC2 (EFeature);
    void  imageViewMinLod (EFeature);
    void  multisampleArrayImage (EFeature);
    void  minImageArrayLayers (uint);
    void  samplerAnisotropy (EFeature);
    void  samplerMirrorClampToEdge (EFeature);
    void  samplerFilterMinmax (EFeature);
    void  filterMinmaxImageComponentMapping (EFeature);
    void  samplerMipLodBias (EFeature);
    void  samplerYcbcrConversion (EFeature);
    void  minSamplerAnisotropy (float);
    void  minSamplerLodBias (float);
    void  minFramebufferLayers (uint);
    void  variableMultisampleRate (EFeature);
    void  metalArgBufferTier (uint);
    void  perDescrSet_minInputAttachments (uint);
    void  perDescrSet_minSampledImages (uint);
    void  perDescrSet_minSamplers (uint);
    void  perDescrSet_minStorageBuffers (uint);
    void  perDescrSet_minStorageImages (uint);
    void  perDescrSet_minUniformBuffers (uint);
    void  perDescrSet_minAccelStructures (uint);
    void  perDescrSet_minTotalResources (uint);
    void  perStage_minInputAttachments (uint);
    void  perStage_minSampledImages (uint);
    void  perStage_minSamplers (uint);
    void  perStage_minStorageBuffers (uint);
    void  perStage_minStorageImages (uint);
    void  perStage_minUniformBuffers (uint);
    void  perStage_minAccelStructures (uint);
    void  perStage_minTotalResources (uint);
};

RC<FeatureSet>  FindFeatureSet (const string &);
struct Align
{
    Align ();
    Align (const Align&);
    Align&  operator = (const Align&);
    Align (uint);
};

struct ArraySize
{
    ArraySize ();
    ArraySize (const ArraySize&);
    ArraySize&  operator = (const ArraySize&);
    ArraySize (uint);
};

struct ETessPatch
{
    ETessPatch () {}
    ETessPatch (uint) {}
    operator uint () const;
    static constexpr uint Points = 1;
    static constexpr uint Isolines = 2;
    static constexpr uint Triangles = 3;
    static constexpr uint Quads = 4;
};

struct ETessSpacing
{
    ETessSpacing () {}
    ETessSpacing (uint) {}
    operator uint () const;
    static constexpr uint Equal = 1;
    static constexpr uint FractionalEven = 2;
    static constexpr uint FractionalOdd = 3;
};

struct Shader
{
    Shader ();
    string source;
    string file;
    EShaderVersion version;
    EShaderOpt options;
    EShader type;
    void  AddSpec (EValueType, const string &);
    void  Define (const string &);
    void  LoadSelf ();
    void  ComputeSpec1 ();
    void  ComputeSpec2 ();
    void  ComputeSpec3 ();
    void  ComputeSpecAndDefault (uint);
    void  ComputeSpecAndDefault (uint, uint);
    void  ComputeSpecAndDefault (uint, uint, uint);
    void  ComputeLocalSize (uint);
    void  ComputeLocalSize (uint, uint);
    void  ComputeLocalSize (uint, uint, uint);
    void  MeshSpec1 ();
    void  MeshSpec2 ();
    void  MeshSpec3 ();
    void  MeshSpecAndDefault (uint);
    void  MeshSpecAndDefault (uint, uint);
    void  MeshSpecAndDefault (uint, uint, uint);
    void  MeshLocalSize (uint);
    void  MeshLocalSize (uint, uint);
    void  MeshLocalSize (uint, uint, uint);
    void  MeshOutput (uint, uint, EPrimitive);
    void  TessPatchSize (uint);
    void  TessPatchMode (ETessPatch, ETessSpacing, bool);
};

struct ShaderStructTypeUsage
{
    ShaderStructTypeUsage () {}
    ShaderStructTypeUsage (uint) {}
    operator uint () const;
    static constexpr uint ShaderIO = 1;
    static constexpr uint VertexAttribs = 2;
    static constexpr uint VertexLayout = 4;
    static constexpr uint BufferLayout = 8;
    static constexpr uint BufferReference = 16;
};

struct ShaderStructType
{
    ShaderStructType ();
    ShaderStructType (const string &);
    void  AddFeatureSet (const string &);
    void  Set (EStructLayout, const string &);
    void  Set (const string &);
    string  FieldsToString () const;
    void  AddUsage (ShaderStructTypeUsage);
    uint  StaticSize () const;
    uint  ArrayStride () const;
};

struct VertexDivisor
{
    VertexDivisor ();
    VertexDivisor (const VertexDivisor&);
    VertexDivisor&  operator = (const VertexDivisor&);
    VertexDivisor (uint);
};

struct VertexBufferInput
{
    VertexBufferInput ();
    VertexBufferInput (const string &);
    void  Add (const string &, const string &, uint);
    void  Add (const string &, const RC<ShaderStructType> &, uint);
    void  Add (const string &, const string &);
    void  Add (const string &, const RC<ShaderStructType> &);
    void  Add (const string &, const string &, const Align &);
    void  Add (const string &, const RC<ShaderStructType> &, const Align &);
    void  Add (const string &, const string &, uint, const VertexDivisor &);
    void  Add (const string &, const RC<ShaderStructType> &, uint, const VertexDivisor &);
    void  Add (const string &, const string &, const VertexDivisor &);
    void  Add (const string &, const RC<ShaderStructType> &, const VertexDivisor &);
    void  Add (const string &, const string &, const Align &, const VertexDivisor &);
    void  Add (const string &, const RC<ShaderStructType> &, const Align &, const VertexDivisor &);
    void  AddFeatureSet (const string &);
};

void  SameAttribs (const string &, const string &);
struct DescriptorSetLayout
{
    DescriptorSetLayout ();
    DescriptorSetLayout (const string &);
    void  AddFeatureSet (const string &);
    void  Define (const string &);
    void  UniformBuffer (uint, const string &, const ArraySize &, const string &, EResourceState);
    void  UniformBuffer (uint, const string &, const ArraySize &, const string &);
    void  UniformBufferDynamic (uint, const string &, const ArraySize &, const string &, EResourceState);
    void  UniformBufferDynamic (uint, const string &, const ArraySize &, const string &);
    void  StorageBuffer (uint, const string &, const ArraySize &, const string &, EAccessType, EResourceState);
    void  StorageBuffer (uint, const string &, const ArraySize &, const string &);
    void  StorageBufferDynamic (uint, const string &, const ArraySize &, const string &, EAccessType, EResourceState);
    void  StorageBufferDynamic (uint, const string &, const ArraySize &, const string &);
    void  UniformTexelBuffer (uint, const string &, const ArraySize &, EImageType, EResourceState);
    void  UniformTexelBuffer (uint, const string &, const ArraySize &, EImageType);
    void  StorageTexelBuffer (uint, const string &, const ArraySize &, EImageType, EPixelFormat, EAccessType, EResourceState);
    void  StorageTexelBuffer (uint, const string &, const ArraySize &, EImageType, EResourceState);
    void  StorageTexelBuffer (uint, const string &, const ArraySize &, EImageType);
    void  StorageImage (uint, const string &, const ArraySize &, EImageType, EPixelFormat, EAccessType, EResourceState);
    void  StorageImage (uint, const string &, const ArraySize &, EImageType, EPixelFormat);
    void  StorageImage (uint, const string &, const ArraySize &, EImageType);
    void  SampledImage (uint, const string &, const ArraySize &, EImageType, EResourceState);
    void  SampledImage (uint, const string &, const ArraySize &, EImageType);
    void  CombinedImage (uint, const string &, const ArraySize &, EImageType);
    void  CombinedImage (uint, const string &, const ArraySize &, EImageType, EResourceState);
    void  CombinedImage (uint, const string &, EImageType, EResourceState, const string &);
    void  CombinedImage (uint, const string &, EImageType, const string &);
    void  SubpassInputFromRenderTech (const string &, const string &);
    void  SubpassInputFromRenderPass (const string &, const string &);
    void  SubpassInput (uint, const string &, EImageType, EResourceState);
    void  SubpassInput (uint, const string &, EImageType);
    void  Sampler (uint, const string &, const ArraySize &);
    void  ImtblSampler (uint, const string &, const string &);
    void  RayTracingScene (uint, const string &, const ArraySize &);
    void  SetUsage (uint);
};

struct PipelineLayout
{
    PipelineLayout ();
    PipelineLayout (const string &);
    void  AddFeatureSet (const string &);
    void  DSLayout (uint, const string &);
    void  DSLayout (const string &, uint, const string &);
    void  DSLayout (uint, const RC<DescriptorSetLayout> &);
    void  DSLayout (const string &, uint, const RC<DescriptorSetLayout> &);
    void  AddDebugDSLayout (EShaderOpt, uint);
    void  AddDebugDSLayout (uint, EShaderOpt, uint);
    void  PushConst (const string &, const string &, EShader);
    void  PushConst (const string &, const RC<ShaderStructType> &, EShader);
    void  Define (const string &);
};

struct EMutableRenderState
{
    EMutableRenderState () {}
    EMutableRenderState (uint) {}
    operator uint () const;
};

struct GraphicsPass
{
    GraphicsPass ();
    void  SetRenderPass (const string &, const string &);
    void  SetRenderState (const RenderState &);
    void  SetRenderState (const string &);
    void  SetMutableStates (EMutableRenderState);
    void  SetDSLayout (const string &);
};

struct ComputePass
{
    ComputePass ();
    void  SetDSLayout (const string &);
};

struct RenderTechnique
{
    RenderTechnique ();
    RenderTechnique (const string &);
    void  AddFeatureSet (const string &);
    RC<GraphicsPass>  AddGraphicsPass (const string &);
    RC<ComputePass>  AddComputePass (const string &);
    RC<GraphicsPass>  CopyGraphicsPass (const string &, const string &, const string &);
    RC<ComputePass>  CopyComputePass (const string &, const string &, const string &);
};

struct GraphicsPipelineSpec
{
    GraphicsPipelineSpec ();
    void  SetSpecValue (const string &, uint);
    void  SetSpecValue (const string &, int);
    void  SetSpecValue (const string &, float);
    void  SetDynamicState (uint);
    void  SetRenderState (const RenderState &);
    void  SetRenderState (const string &);
    void  SetViewportCount (uint);
    void  AddToRenderTech (const string &, const string &);
    void  SetOptions (EPipelineOpt);
    void  SetVertexInput (const string &);
    void  SetVertexInput (const RC<VertexBufferInput> &);
};

struct GraphicsPipeline
{
    GraphicsPipeline ();
    GraphicsPipeline (const string &);
    void  Define (const string &);
    void  SetVertexShader (const RC<Shader> &);
    void  SetTessControlShader (const RC<Shader> &);
    void  SetTessEvalShader (const RC<Shader> &);
    void  SetGeometryShader (const RC<Shader> &);
    void  SetFragmentShader (const RC<Shader> &);
    RC<GraphicsPipelineSpec>  AddSpecialization (const string &);
    void  TestRenderPass (const string &, const string &);
    void  AddFeatureSet (const string &);
    void  SetLayout (const string &);
    void  SetLayout (const RC<PipelineLayout> &);
    void  SetVertexInput (const string &);
    void  SetVertexInput (const RC<VertexBufferInput> &);
    void  SetShaderIO (EShader, EShader, const string &);
    void  SetFragmentOutputFromRenderTech (const string &, const string &);
    void  SetFragmentOutputFromRenderPass (const string &, const string &);
};

struct MeshPipelineSpec
{
    MeshPipelineSpec ();
    void  SetSpecValue (const string &, uint);
    void  SetSpecValue (const string &, int);
    void  SetSpecValue (const string &, float);
    void  SetDynamicState (uint);
    void  SetRenderState (const RenderState &);
    void  SetRenderState2 (const string &);
    void  SetViewportCount (uint);
    void  SetTaskLocalSize (uint);
    void  SetTaskLocalSize (uint, uint);
    void  SetTaskLocalSize (uint, uint, uint);
    void  SetMeshLocalSize (uint);
    void  SetMeshLocalSize (uint, uint);
    void  SetMeshLocalSize (uint, uint, uint);
    void  AddToRenderTech (const string &, const string &);
    void  SetOptions (EPipelineOpt);
};

struct MeshPipeline
{
    MeshPipeline ();
    MeshPipeline (const string &);
    void  Define (const string &);
    void  SetTaskShader (const RC<Shader> &);
    void  SetMeshShader (const RC<Shader> &);
    void  SetFragmentShader (const RC<Shader> &);
    RC<MeshPipelineSpec>  AddSpecialization (const string &);
    void  TestRenderPass (const string &, const string &);
    void  AddFeatureSet (const string &);
    void  SetLayout (const string &);
    void  SetLayout (const RC<PipelineLayout> &);
    void  SetShaderIO (EShader, EShader, const string &);
    void  SetFragmentOutputFromRenderTech (const string &, const string &);
    void  SetFragmentOutputFromRenderPass (const string &, const string &);
};

struct ComputePipelineSpec
{
    ComputePipelineSpec ();
    void  SetSpecValue (const string &, uint);
    void  SetSpecValue (const string &, int);
    void  SetSpecValue (const string &, float);
    void  SetDynamicState (uint);
    void  SetLocalSize (uint);
    void  SetLocalSize (uint, uint);
    void  SetLocalSize (uint, uint, uint);
    void  AddToRenderTech (const string &, const string &);
    void  SetOptions (EPipelineOpt);
};

struct ComputePipeline
{
    ComputePipeline ();
    ComputePipeline (const string &);
    void  Define (const string &);
    void  SetShader (const RC<Shader> &);
    RC<ComputePipelineSpec>  AddSpecialization (const string &);
    void  AddFeatureSet (const string &);
    void  SetLayout (const string &);
    void  SetLayout (const RC<PipelineLayout> &);
};

struct TilePipelineSpec
{
    TilePipelineSpec ();
    void  SetSpecValue (const string &, uint);
    void  SetSpecValue (const string &, int);
    void  SetSpecValue (const string &, float);
    void  SetDynamicState (uint);
    void  SetLocalSize (uint);
    void  SetLocalSize (uint, uint);
    void  AddToRenderTech (const string &, const string &);
    void  SetOptions (EPipelineOpt);
};

struct TilePipeline
{
    TilePipeline ();
    TilePipeline (const string &);
    void  Define (const string &);
    void  SetShader (const RC<Shader> &);
    RC<TilePipelineSpec>  AddSpecialization (const string &);
    void  AddFeatureSet (const string &);
    void  SetLayout (const string &);
    void  SetLayout (const RC<PipelineLayout> &);
};

struct RayTracingPipelineSpec
{
    RayTracingPipelineSpec ();
    void  SetSpecValue (const string &, uint);
    void  SetSpecValue (const string &, int);
    void  SetSpecValue (const string &, float);
    void  SetDynamicState (uint);
    void  AddToRenderTech (const string &, const string &);
    void  SetOptions (EPipelineOpt);
    void  MaxRecursionDepth (uint);
};

struct RayTracingPipeline
{
    RayTracingPipeline ();
    RayTracingPipeline (const string &);
    void  Define (const string &);
    RC<RayTracingPipelineSpec>  AddSpecialization (const string &);
    void  AddFeatureSet (const string &);
    void  SetLayout (const string &);
    void  SetLayout (const RC<PipelineLayout> &);
    void  AddGeneralShader (const string &, const RC<Shader> &);
    void  AddTriangleHitGroup (const string &, const RC<Shader> &, const RC<Shader> &);
    void  AddProceduralHitGroup (const string &, const RC<Shader> &, const RC<Shader> &, const RC<Shader> &);
};

const string InitialLayout;
const string Subpass_ExternalIn;
const string FinalLayout;
const string Subpass_ExternalOut;
const string Subpass_Main;
const string Attachment_Color;
const string Attachment_Depth;
const string Attachment_DepthStencil;
struct EAttachment
{
    EAttachment () {}
    EAttachment (uint) {}
    operator uint () const;
    static constexpr uint Invalidate = 0;
    static constexpr uint Color = 1;
    static constexpr uint ReadWrite = 3;
    static constexpr uint ColorResolve = 2;
    static constexpr uint Input = 4;
    static constexpr uint Depth = 5;
    static constexpr uint Preserve = 6;
    static constexpr uint DepthStencil = 5;
    static constexpr uint ShadingRate = 7;
};

struct ShaderIO
{
    ShaderIO ();
    ShaderIO (const ShaderIO&);
    ShaderIO&  operator = (const ShaderIO&);
    ShaderIO (const string &);
    ShaderIO (const string &, EShaderIO);
    ShaderIO (uint, const string &, EShaderIO);
    ShaderIO (uint, const string &);
};

struct Attachment
{
    Attachment ();
    EPixelFormat format;
    MultiSamples samples;
    void  Usage (const string &, EAttachment);
    void  Usage (const string &, EAttachment, const ShaderIO &);
    void  Usage (const string &, EAttachment, const ShaderIO &, const ShaderIO &);
    void  Usage (const string &, EAttachment, const uint2 &);
    void  Print () const;
};

struct AttachmentSpec
{
    AttachmentSpec ();
    EAttachmentLoadOp loadOp;
    EAttachmentStoreOp storeOp;
    void  Layout (const string &, EResourceState);
    void  Layout (const string &, uint);
    void  GenOptimalLayouts ();
    void  GenOptimalLayouts (EResourceState, EResourceState);
    void  GenOptimalLayouts (uint, uint);
    void  Print () const;
};

struct RenderPass
{
    RenderPass ();
    RC<AttachmentSpec>  AddAttachment (const string &);
    void  GenOptimalLayouts ();
    void  Print () const;
};

struct CompatibleRenderPass
{
    CompatibleRenderPass ();
    CompatibleRenderPass (const string &);
    RC<RenderPass>  AddSpecialization (const string &);
    RC<Attachment>  AddAttachment (const string &);
    void  AddSubpass (const string &);
    void  Print () const;
    void  AddFeatureSet (const string &);
};

struct Sampler
{
    Sampler ();
    Sampler (const string &);
    void  AddFeatureSet (const string &);
    void  Filter (EFilter, EFilter, EMipmapFilter);
    void  AddressMode (EAddressMode);
    void  AddressMode (EAddressMode, EAddressMode, EAddressMode);
    void  MipLodBias (float);
    void  LodRange (float, float);
    void  Anisotropy (float);
    void  CompareOp (ECompareOp);
    void  BorderColor (EBorderColor);
    void  NormCoordinates (bool);
    void  ReductionMode (EReductionMode);
    void  Usage (ESamplerUsage);
    void  Ycbcr_Format (EPixelFormat);
    void  Ycbcr_Model (ESamplerYcbcrModelConversion);
    void  Ycbcr_Range (ESamplerYcbcrRange);
    void  Ycbcr_Components (const string &);
    void  Ycbcr_XChromaOffset (ESamplerChromaLocation);
    void  Ycbcr_YChromaOffset (ESamplerChromaLocation);
    void  Ycbcr_ChromaFilter (EFilter);
    void  Ycbcr_ForceExplicitReconstruction (bool);
};

struct MissIndex
{
    MissIndex ();
    MissIndex (const MissIndex&);
    MissIndex&  operator = (const MissIndex&);
    MissIndex (uint);
};

struct InstanceIndex
{
    InstanceIndex ();
    InstanceIndex (const InstanceIndex&);
    InstanceIndex&  operator = (const InstanceIndex&);
    InstanceIndex (uint);
};

struct RayIndex
{
    RayIndex ();
    RayIndex (const RayIndex&);
    RayIndex&  operator = (const RayIndex&);
    RayIndex (uint);
};

struct CallableIndex
{
    CallableIndex ();
    CallableIndex (const CallableIndex&);
    CallableIndex&  operator = (const CallableIndex&);
    CallableIndex (uint);
};

struct RayTracingShaderBinding
{
    RayTracingShaderBinding ();
    RayTracingShaderBinding (const RC<RayTracingPipelineSpec> &, const string &);
    void  HitGroupStride (uint);
    void  BindRayGen (const string &);
    void  BindMiss (const string &, const MissIndex &);
    void  BindHitGroup (const string &, const InstanceIndex &, const RayIndex &);
    void  BindCallable (const string &, const CallableIndex &);
};

bool  IsVulkan ();
bool  IsMetal_iOS ();
bool  IsMetal_Mac ();
bool  IsMetal ();
RC<ShaderStructType>  GetShaderStructType (const string &);
#define SCRIPT

const string Sampler_NearestClamp;
const string Sampler_NearestRepeat;
const string Sampler_NearestMirrorRepeat;
const string Sampler_LinearClamp;
const string Sampler_LinearRepeat;
const string Sampler_LinearMirrorRepeat;
const string Sampler_LinearMipmapClamp;
const string Sampler_LinearMipmapRepeat;
const string Sampler_LinearMipmapMirrorRepeat;
const string Sampler_Anisotropy8Repeat;
const string Sampler_Anisotropy8MirrorRepeat;
const string Sampler_Anisotropy8Clamp;
const string Sampler_Anisotropy16Repeat;
const string Sampler_Anisotropy16MirrorRepeat;
const string Sampler_Anisotropy16Clamp;
template <>
struct RC<ShaderStructType> : ShaderStructType
{
    RC (const ShaderStructType &);
};

template <>
struct RC<Shader> : Shader
{
    RC (const Shader &);
};

template <>
struct RC<CompatibleRenderPass> : CompatibleRenderPass
{
    RC (const CompatibleRenderPass &);
};

template <>
struct RC<ComputePipelineSpec> : ComputePipelineSpec
{
    RC (const ComputePipelineSpec &);
};

template <>
struct RC<RayTracingShaderBinding> : RayTracingShaderBinding
{
    RC (const RayTracingShaderBinding &);
};

template <>
struct RC<RenderTechnique> : RenderTechnique
{
    RC (const RenderTechnique &);
};

template <>
struct RC<RayTracingPipeline> : RayTracingPipeline
{
    RC (const RayTracingPipeline &);
};

template <>
struct RC<DescriptorSetLayout> : DescriptorSetLayout
{
    RC (const DescriptorSetLayout &);
};

template <>
struct RC<ComputePipeline> : ComputePipeline
{
    RC (const ComputePipeline &);
};

template <>
struct RC<GraphicsPass> : GraphicsPass
{
    RC (const GraphicsPass &);
};

template <>
struct RC<RenderPass> : RenderPass
{
    RC (const RenderPass &);
};

template <>
struct RC<PipelineLayout> : PipelineLayout
{
    RC (const PipelineLayout &);
};

template <>
struct RC<GraphicsPipelineSpec> : GraphicsPipelineSpec
{
    RC (const GraphicsPipelineSpec &);
};

template <>
struct RC<TilePipeline> : TilePipeline
{
    RC (const TilePipeline &);
};

template <>
struct RC<TilePipelineSpec> : TilePipelineSpec
{
    RC (const TilePipelineSpec &);
};

template <>
struct RC<RayTracingPipelineSpec> : RayTracingPipelineSpec
{
    RC (const RayTracingPipelineSpec &);
};

template <>
struct RC<AttachmentSpec> : AttachmentSpec
{
    RC (const AttachmentSpec &);
};

template <>
struct RC<GraphicsPipeline> : GraphicsPipeline
{
    RC (const GraphicsPipeline &);
};

template <>
struct RC<MeshPipelineSpec> : MeshPipelineSpec
{
    RC (const MeshPipelineSpec &);
};

template <>
struct RC<Sampler> : Sampler
{
    RC (const Sampler &);
};

template <>
struct RC<MeshPipeline> : MeshPipeline
{
    RC (const MeshPipeline &);
};

template <>
struct RC<NamedRenderState> : NamedRenderState
{
    RC (const NamedRenderState &);
};

template <>
struct RC<FeatureSet> : FeatureSet
{
    RC (const FeatureSet &);
};

template <>
struct RC<ComputePass> : ComputePass
{
    RC (const ComputePass &);
};

template <>
struct RC<Attachment> : Attachment
{
    RC (const Attachment &);
};

template <>
struct RC<VertexBufferInput> : VertexBufferInput
{
    RC (const VertexBufferInput &);
};

