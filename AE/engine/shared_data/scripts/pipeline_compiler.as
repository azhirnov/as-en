//47029888
#include <vector>
#include <string>

using int8		= std::int8_t;
using uint8		= std::uint8_t;
using int16		= std::int16_t;
using uint16	= std::uint16_t;
using int		= std::int32_t;
using uint		= std::uint32_t;
using int32		= std::int32_t;
using uint32	= std::uint32_t;
using int64		= std::int64_t;
using uint64	= std::uint64_t;
using string	= std::string;

template <typename T>
struct RC;

template <typename T>
using array = std::vector<T>;

struct EAttachmentStoreOp;
struct ubyte4;
struct ESamplerYcbcrRange;
struct CompatibleRenderPass;
struct GlobalConfig;
struct ComputePipelineSpec;
struct EPrimitive;
struct ESubgroupOperation;
struct RenderState_InputAssemblyState;
struct ubyte2;
struct ubyte3;
struct EGraphicsDeviceID;
struct EFormatFeature;
struct RenderState_RasterizationState;
struct EImage;
struct bool3;
struct bool2;
struct EVertexType;
struct RGBA32i;
struct bool4;
struct EBlendOp;
struct ELogicOp;
struct ECullMode;
struct ETessPatch;
struct ShaderStructTypeUsage;
struct HSVColor;
struct RGBA32f;
struct RenderState_ColorBuffer_ColorMask;
struct EQueueMask;
struct EShaderPreprocessor;
struct EBlendFactor;
struct Shader;
struct ShaderStructType;
struct ERTInstanceOpt;
struct EStencilOp;
struct EAddressMode;
struct ECompilationTarget;
struct RayIndex;
struct RenderState_MultisampleState;
struct ComputePipeline;
struct EIndex;
struct EAccessType;
struct float4;
struct EAttachment;
struct ESamplerYcbcrModelConversion;
struct sbyte3;
struct sbyte4;
struct short3;
struct GraphicsPass;
struct short2;
struct ushort4;
struct ESubgroupTypes;
struct ESamplerOpt;
struct Align;
struct ESamplerChromaLocation;
struct DescriptorSetLayout;
struct RenderState_StencilBufferState;
struct EPipelineDynamicState;
struct EFilter;
struct MultiSamples;
struct RenderTechnique;
struct RayTracingShaderBinding;
struct RayTracingPipeline;
struct EShaderIO;
struct EImageAspect;
struct EPipelineOpt;
struct InstanceIndex;
struct float3;
struct float2;
struct EDescSetUsage;
struct EVertexInputRate;
struct EReductionMode;
struct ESurfaceFormat;
struct MipmapLevel;
struct EShaderVersion;
struct AttachmentSpec;
struct RenderState;
struct DepthStencil;
struct RenderState_ColorBuffersState;
struct EPixelFormatExternal;
struct RayTracingPipelineSpec;
struct int4;
struct ImageLayer;
struct int2;
struct int3;
struct ArraySize;
struct PipelineLayout;
struct GraphicsPipelineSpec;
struct EPixelFormat;
struct short4;
struct ushort2;
struct sbyte2;
struct ushort3;
struct RenderState_ColorBuffer;
struct RenderPass;
struct EValueType;
struct ETessSpacing;
struct TilePipelineSpec;
struct TilePipeline;
struct EPolygonMode;
struct EAttachmentLoadOp;
struct uint3;
struct uint2;
struct EMutableRenderState;
struct ShaderIO;
struct RenderState_DepthBufferState;
struct EShaderStages;
struct EShader;
struct EGPUVendor;
struct ECompareOp;
struct RenderState_StencilFaceState;
struct uint4;
struct EMipmapFilter;
struct RGBA8u;
struct VertexBufferInput;
struct EStructLayout;
struct RGBA32u;
struct Attachment;
struct Sampler;
struct CallableIndex;
struct GraphicsPipeline;
struct MeshPipelineSpec;
struct EShaderOpt;
struct EFeature;
struct MeshPipeline;
struct NamedRenderState;
struct EResourceState;
struct ComputePass;
struct FeatureSet;
struct EBorderColor;
struct VertexDivisor;
struct EImageType;

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
int  ExtractBitIndex (int & x);
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
uint  ExtractBitIndex (uint & x);
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
float  RemapClamp (float srcMin, float srcMax, float dstMin, float dstMax, float x);
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
string  FindAndReplace (const string &, const string &, const string &);
bool  StartsWith (const string &, const string &);
bool  StartsWithIC (const string &, const string &);
bool  EndsWith (const string &, const string &);
bool  EndsWithIC (const string &, const string &);
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
	static constexpr uint8 G8_B8R8_444_UNorm = 141;
	static constexpr uint8 G8_B8_R8_420_UNorm = 142;
	static constexpr uint8 G8_B8_R8_422_UNorm = 143;
	static constexpr uint8 G8_B8_R8_444_UNorm = 144;
	static constexpr uint8 B10x6G10x6R10x6G10x6_422_UNorm = 145;
	static constexpr uint8 G10x6B10x6G10x6R10x6_422_UNorm = 146;
	static constexpr uint8 G10x6_B10x6R10x6_420_UNorm = 147;
	static constexpr uint8 G10x6_B10x6R10x6_422_UNorm = 148;
	static constexpr uint8 G10x6_B10x6R10x6_444_UNorm = 149;
	static constexpr uint8 G10x6_B10x6_R10x6_420_UNorm = 150;
	static constexpr uint8 G10x6_B10x6_R10x6_422_UNorm = 151;
	static constexpr uint8 G10x6_B10x6_R10x6_444_UNorm = 152;
	static constexpr uint8 R10x6G10x6B10x6A10x6_UNorm = 153;
	static constexpr uint8 R10x6G10x6_UNorm = 154;
	static constexpr uint8 R10x6_UNorm = 155;
	static constexpr uint8 B12x4G12x4R12x4G12x4_422_UNorm = 156;
	static constexpr uint8 G12x4B12x4G12x4R12x4_422_UNorm = 157;
	static constexpr uint8 G12x4_B12x4R12x4_420_UNorm = 158;
	static constexpr uint8 G12x4_B12x4R12x4_422_UNorm = 159;
	static constexpr uint8 G12x4_B12x4R12x4_444_UNorm = 160;
	static constexpr uint8 G12x4_B12x4_R12x4_420_UNorm = 161;
	static constexpr uint8 G12x4_B12x4_R12x4_422_UNorm = 162;
	static constexpr uint8 G12x4_B12x4_R12x4_444_UNorm = 163;
	static constexpr uint8 R12x4G12x4B12x4A12x4_UNorm = 164;
	static constexpr uint8 R12x4G12x4_UNorm = 165;
	static constexpr uint8 R12x4_UNorm = 166;
	static constexpr uint8 B16G16R16G16_422_UNorm = 167;
	static constexpr uint8 G16B16G16R16_422_UNorm = 168;
	static constexpr uint8 G16_B16R16_420_UNorm = 169;
	static constexpr uint8 G16_B16R16_422_UNorm = 170;
	static constexpr uint8 G16_B16R16_444_UNorm = 171;
	static constexpr uint8 G16_B16_R16_420_UNorm = 172;
	static constexpr uint8 G16_B16_R16_422_UNorm = 173;
	static constexpr uint8 G16_B16_R16_444_UNorm = 174;
	static constexpr uint8 SwapchainColor = 254;
};

struct EPixelFormatExternal
{
	EPixelFormatExternal () {}
	EPixelFormatExternal (uint8) {}
	operator uint8 () const;
	static constexpr uint8 Android_Depth16 = 0;
	static constexpr uint8 Android_DepthJPEG = 1;
	static constexpr uint8 Android_DepthPointCloud = 2;
	static constexpr uint8 Android_JPEG = 3;
	static constexpr uint8 Android_Raw16 = 5;
	static constexpr uint8 Android_Raw12 = 6;
	static constexpr uint8 Android_Raw10 = 7;
	static constexpr uint8 Android_NV16 = 9;
	static constexpr uint8 Android_NV21 = 10;
	static constexpr uint8 Android_YCBCR_P010 = 11;
	static constexpr uint8 Android_YUV_420 = 12;
	static constexpr uint8 Android_YUV_422 = 13;
	static constexpr uint8 Android_YUV_444 = 14;
	static constexpr uint8 Android_YUY2 = 15;
	static constexpr uint8 Android_YV12 = 16;
	static constexpr uint8 Android_Y8 = 17;
	static constexpr uint8 Android_HEIC = 18;
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
	static constexpr uint8 DepthStencil = 6;
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
	static constexpr uint16 GraphicsStages = 31;
	static constexpr uint16 MeshStages = 400;
	static constexpr uint16 VertexProcessingStages = 271;
	static constexpr uint16 PreRasterizationStages = 399;
	static constexpr uint16 PostRasterizationStages = 80;
	static constexpr uint16 AllRayTracing = 32256;
};

struct EGPUVendor
{
	EGPUVendor () {}
	EGPUVendor (uint32) {}
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
	static constexpr uint32 Huawei = 12;
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
	static constexpr uint32 Adreno_700_QC5X = 7;
	static constexpr uint32 AMD_GCN1 = 8;
	static constexpr uint32 AMD_GCN2 = 9;
	static constexpr uint32 AMD_GCN3 = 10;
	static constexpr uint32 AMD_GCN4 = 11;
	static constexpr uint32 AMD_GCN5 = 12;
	static constexpr uint32 AMD_GCN5_APU = 13;
	static constexpr uint32 AMD_RDNA1 = 14;
	static constexpr uint32 AMD_RDNA2 = 15;
	static constexpr uint32 AMD_RDNA2_APU = 16;
	static constexpr uint32 AMD_RDNA3 = 17;
	static constexpr uint32 AMD_RDNA3_APU = 18;
	static constexpr uint32 Apple_A8 = 19;
	static constexpr uint32 Apple_A9_A10 = 20;
	static constexpr uint32 Apple_A11 = 21;
	static constexpr uint32 Apple_A12 = 22;
	static constexpr uint32 Apple_A13 = 23;
	static constexpr uint32 Apple_A14_M1 = 24;
	static constexpr uint32 Apple_A15_M2 = 25;
	static constexpr uint32 Apple_A16 = 26;
	static constexpr uint32 Apple_A17_M3 = 27;
	static constexpr uint32 Mali_Midgard_Gen2 = 28;
	static constexpr uint32 Mali_Midgard_Gen3 = 29;
	static constexpr uint32 Mali_Midgard_Gen4 = 30;
	static constexpr uint32 Mali_Bifrost_Gen1 = 31;
	static constexpr uint32 Mali_Bifrost_Gen2 = 32;
	static constexpr uint32 Mali_Bifrost_Gen3 = 33;
	static constexpr uint32 Mali_Valhall_Gen1 = 34;
	static constexpr uint32 Mali_Valhall_Gen2 = 35;
	static constexpr uint32 Mali_Valhall_Gen3 = 36;
	static constexpr uint32 Mali_Valhall_Gen4 = 37;
	static constexpr uint32 Mali_Valhall_Gen5 = 38;
	static constexpr uint32 NV_Maxwell = 39;
	static constexpr uint32 NV_Maxwell_Tegra = 40;
	static constexpr uint32 NV_Pascal = 41;
	static constexpr uint32 NV_Pascal_MX = 42;
	static constexpr uint32 NV_Pascal_Tegra = 43;
	static constexpr uint32 NV_Volta = 44;
	static constexpr uint32 NV_Turing_16 = 45;
	static constexpr uint32 NV_Turing = 46;
	static constexpr uint32 NV_Turing_MX = 47;
	static constexpr uint32 NV_Ampere = 48;
	static constexpr uint32 NV_Ampere_Orin = 49;
	static constexpr uint32 NV_Ada = 50;
	static constexpr uint32 Intel_Gen9_HD500 = 51;
	static constexpr uint32 Intel_Gen9_HD600 = 52;
	static constexpr uint32 Intel_Gen9_UHD600 = 53;
	static constexpr uint32 Intel_Gen11 = 54;
	static constexpr uint32 Intel_Gen12 = 55;
	static constexpr uint32 Intel_Gen12_7 = 56;
	static constexpr uint32 PowerVR_Series8XE = 57;
	static constexpr uint32 PowerVR_Series8XEP = 58;
	static constexpr uint32 PowerVR_Series8XT = 59;
	static constexpr uint32 PowerVR_Series9XE = 60;
	static constexpr uint32 PowerVR_BSeries = 61;
	static constexpr uint32 VeriSilicon = 62;
	static constexpr uint32 SwiftShader = 63;
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

struct ESamplerOpt
{
	ESamplerOpt () {}
	ESamplerOpt (uint8) {}
	operator uint8 () const;
	static constexpr uint8 ArgumentBuffer = 1;
	static constexpr uint8 UnnormalizedCoordinates = 4;
	static constexpr uint8 NonSeamlessCubeMap = 2;
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
	static constexpr uint8 MaybeUnsupported = 16;
};

struct EPipelineOpt
{
	EPipelineOpt () {}
	EPipelineOpt (uint16) {}
	operator uint16 () const;
	static constexpr uint16 Optimize = 1;
	static constexpr uint16 CS_DispatchBase = 2;
	static constexpr uint16 RT_NoNullAnyHitShaders = 4;
	static constexpr uint16 RT_NoNullClosestHitShaders = 8;
	static constexpr uint16 RT_NoNullMissShaders = 16;
	static constexpr uint16 RT_NoNullIntersectionShaders = 32;
	static constexpr uint16 RT_SkipTriangles = 64;
	static constexpr uint16 RT_SkipAABBs = 128;
	static constexpr uint16 DontCompile = 256;
	static constexpr uint16 CaptureStatistics = 512;
	static constexpr uint16 CaptureInternalRepresentation = 1024;
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
	static constexpr uint8 All = 31;
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
	void  SrcBlendFactor (EBlendFactor colorAndAlpha);
	void  SrcBlendFactor (EBlendFactor color, EBlendFactor alpha);
	void  DstBlendFactor (EBlendFactor colorAndAlpha);
	void  DstBlendFactor (EBlendFactor color, EBlendFactor alpha);
	void  BlendOp (EBlendOp colorAndAlpha);
	void  BlendOp (EBlendOp color, EBlendOp alpha);
	bool blend;
	RenderState_ColorBuffer_ColorMask colorMask;
};

struct RenderState_ColorBuffersState
{
	RenderState_ColorBuffersState ();
	RenderState_ColorBuffersState (const RenderState_ColorBuffersState&);
	RenderState_ColorBuffersState&  operator = (const RenderState_ColorBuffersState&);
	void  SetColorBuffer (uint index, const RenderState_ColorBuffer & cb);
	const RenderState_ColorBuffer &  GetColorBuffer (uint index) const;
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
	void  FailOp (EStencilOp);
	void  DepthFailOp (EStencilOp);
	void  PassOp (EStencilOp);
	void  CompareOp (ECompareOp);
	void  Reference (uint8);
	void  WriteMask (uint8);
	void  CompareMask (uint8);
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
	EShaderVersion (uint32) {}
	operator uint32 () const;

	// Vulkan 1.0
	static constexpr uint32 SPIRV_1_0 = 268435472;
	static constexpr uint32 SPIRV_1_1 = 268435473;
	static constexpr uint32 SPIRV_1_2 = 268435474;

	// Vulkan 1.1
	static constexpr uint32 SPIRV_1_3 = 268435475;

	// Vulkan 1.1 extension
	static constexpr uint32 SPIRV_1_4 = 268435476;

	// Vulkan 1.2
	static constexpr uint32 SPIRV_1_5 = 268435477;

	// Vulkan 1.3
	static constexpr uint32 SPIRV_1_6 = 268435478;

	// Metal API
	static constexpr uint32 Metal_2_0 = 1073741856;
	static constexpr uint32 Metal_2_1 = 1073741857;
	static constexpr uint32 Metal_2_2 = 1073741858;

	// Added ray tracing.
	static constexpr uint32 Metal_2_3 = 1073741859;
	static constexpr uint32 Metal_2_4 = 1073741860;

	// Added mesh shading.
	static constexpr uint32 Metal_3_0 = 1073741872;
	static constexpr uint32 Metal_3_1 = 1073741873;

	// Compile for iOS.
	static constexpr uint32 Metal_iOS_2_0 = 536870944;
	static constexpr uint32 Metal_iOS_2_1 = 536870945;
	static constexpr uint32 Metal_iOS_2_2 = 536870946;
	static constexpr uint32 Metal_iOS_2_3 = 536870947;
	static constexpr uint32 Metal_iOS_2_4 = 536870948;
	static constexpr uint32 Metal_iOS_3_0 = 536870960;
	static constexpr uint32 Metal_iOS_3_1 = 536870961;

	// Compile for MacOS.
	static constexpr uint32 Metal_Mac_2_0 = 805306400;
	static constexpr uint32 Metal_Mac_2_1 = 805306401;
	static constexpr uint32 Metal_Mac_2_2 = 805306402;
	static constexpr uint32 Metal_Mac_2_3 = 805306403;
	static constexpr uint32 Metal_Mac_2_4 = 805306404;
	static constexpr uint32 Metal_Mac_3_0 = 805306416;
	static constexpr uint32 Metal_Mac_3_1 = 805306417;
};

struct EShaderOpt
{
	EShaderOpt () {}
	EShaderOpt (uint32) {}
	operator uint32 () const;

	// Add debug information. Used in RenderDoc shader debugger.
	static constexpr uint32 DebugInfo = 1;

	// Insert shader trace recording. Shader will be very slow.
	static constexpr uint32 Trace = 2;

	// Insert shader function profiling. Shader will be very slow.
	static constexpr uint32 FnProfiling = 4;

	// Insert whole shader time measurement. Shader will be a bit slow.
	static constexpr uint32 TimeHeatMap = 8;

	// Enable optimizations. Take a lot of CPU time at shader compilation.
	static constexpr uint32 Optimize = 16;

	// Enable bytecode size optimizations. Take a lot of CPU time at shader compilation.
	static constexpr uint32 OptimizeSize = 32;

	// Enable strong optimizations. Take a lot of CPU time at shader compilation.
	static constexpr uint32 StrongOptimization = 64;
	static constexpr uint32 WarnAsError = 1024;
};

struct EAccessType
{
	EAccessType () {}
	EAccessType (uint32) {}
	operator uint32 () const;
	static constexpr uint32 Coherent = 1;
	static constexpr uint32 Volatile = 2;
	static constexpr uint32 Restrict = 3;

	// Require 'vulkanMemoryModel' feature.
	static constexpr uint32 DeviceCoherent = 5;
	static constexpr uint32 QueueFamilyCoherent = 6;
	static constexpr uint32 WorkgroupCoherent = 7;
	static constexpr uint32 SubgroupCoherent = 8;
	static constexpr uint32 NonPrivate = 9;
};

struct EImageType
{
	EImageType () {}
	EImageType (uint16) {}
	operator uint16 () const;
	static constexpr uint16 1D = 1;
	static constexpr uint16 1DArray = 2;
	static constexpr uint16 2D = 3;
	static constexpr uint16 2DArray = 4;
	static constexpr uint16 2DMS = 5;
	static constexpr uint16 2DMSArray = 6;
	static constexpr uint16 Cube = 7;
	static constexpr uint16 CubeArray = 8;
	static constexpr uint16 3D = 9;
	static constexpr uint16 Buffer = 10;
	static constexpr uint16 Float = 16;
	static constexpr uint16 Half = 32;
	static constexpr uint16 SNorm = 48;
	static constexpr uint16 UNorm = 64;
	static constexpr uint16 Int = 80;
	static constexpr uint16 UInt = 96;
	static constexpr uint16 sRGB = 112;
	static constexpr uint16 Depth = 128;
	static constexpr uint16 Stencil = 144;
	static constexpr uint16 DepthStencil = 160;
	static constexpr uint16 Shadow = 256;
	static constexpr uint16 FImage1D = 17;
	static constexpr uint16 FImage2D = 19;
	static constexpr uint16 FImage2D_sRGB = 115;
	static constexpr uint16 FImage3D = 25;
	static constexpr uint16 FImage1DArray = 18;
	static constexpr uint16 FImage2DArray = 20;
	static constexpr uint16 FImageCube = 23;
	static constexpr uint16 FImageCubeArray = 24;
	static constexpr uint16 FImage2DMS = 21;
	static constexpr uint16 FImage2DMSArray = 22;
	static constexpr uint16 FImageBuffer = 26;
	static constexpr uint16 HImage1D = 33;
	static constexpr uint16 HImage2D = 35;
	static constexpr uint16 HImage3D = 41;
	static constexpr uint16 HImage1DArray = 34;
	static constexpr uint16 HImage2DArray = 36;
	static constexpr uint16 HImageCube = 39;
	static constexpr uint16 HImageCubeArray = 40;
	static constexpr uint16 HImage2DMS = 37;
	static constexpr uint16 HImage2DMSArray = 38;
	static constexpr uint16 HImageBuffer = 42;
	static constexpr uint16 Image1DShadow = 385;
	static constexpr uint16 Image2DShadow = 387;
	static constexpr uint16 Image1DArrayShadow = 386;
	static constexpr uint16 Image2DArrayShadow = 388;
	static constexpr uint16 ImageCubeShadow = 391;
	static constexpr uint16 ImageCubeArrayShadow = 392;
	static constexpr uint16 IImage1D = 81;
	static constexpr uint16 IImage2D = 83;
	static constexpr uint16 IImage3D = 89;
	static constexpr uint16 IImage1DArray = 82;
	static constexpr uint16 IImage2DArray = 84;
	static constexpr uint16 IImageCube = 87;
	static constexpr uint16 IImageCubeArray = 88;
	static constexpr uint16 IImage2DMS = 85;
	static constexpr uint16 IImage2DMSArray = 86;
	static constexpr uint16 IImageBuffer = 90;
	static constexpr uint16 UImage1D = 97;
	static constexpr uint16 UImage2D = 99;
	static constexpr uint16 UImage3D = 105;
	static constexpr uint16 UImage1DArray = 98;
	static constexpr uint16 UImage2DArray = 100;
	static constexpr uint16 UImageCube = 103;
	static constexpr uint16 UImageCubeArray = 104;
	static constexpr uint16 UImage2DMS = 101;
	static constexpr uint16 UImage2DMSArray = 102;
	static constexpr uint16 UImageBuffer = 106;
};

struct ECompilationTarget
{
	ECompilationTarget () {}
	ECompilationTarget (uint32) {}
	operator uint32 () const;
	static constexpr uint32 Vulkan = 1;
	static constexpr uint32 Metal_iOS = 2;
	static constexpr uint32 Metal_Mac = 3;
};

struct EStructLayout
{
	EStructLayout () {}
	EStructLayout (uint8) {}
	operator uint8 () const;

	// Apply GLSL std140 rules but structure must be compatible with Metal otherwise will throw exception.
	static constexpr uint8 Compatible_Std140 = 0;

	// Apply GLSL std430 rules but structure must be compatible with Metal otherwise will throw exception.
	static constexpr uint8 Compatible_Std430 = 1;

	// Apply MSL rules.
	static constexpr uint8 Metal = 2;

	// Apply GLSL std140 rules.
	static constexpr uint8 Std140 = 3;

	// Apply GLSL std430 rules.
	static constexpr uint8 Std430 = 4;

	// Platform depended layout.
	static constexpr uint8 InternalIO = 5;
};

struct EValueType
{
	EValueType () {}
	EValueType (uint8) {}
	operator uint8 () const;
	static constexpr uint8 Bool8 = 1;
	static constexpr uint8 Bool32 = 2;
	static constexpr uint8 Int8 = 3;
	static constexpr uint8 Int16 = 4;
	static constexpr uint8 Int32 = 5;
	static constexpr uint8 Int64 = 6;
	static constexpr uint8 UInt8 = 7;
	static constexpr uint8 UInt16 = 8;
	static constexpr uint8 UInt32 = 9;
	static constexpr uint8 UInt64 = 10;
	static constexpr uint8 Float16 = 11;
	static constexpr uint8 Float32 = 12;
	static constexpr uint8 Float64 = 13;
	static constexpr uint8 Int8_Norm = 14;
	static constexpr uint8 Int16_Norm = 15;
	static constexpr uint8 UInt8_Norm = 16;
	static constexpr uint8 UInt16_Norm = 17;
	static constexpr uint8 DeviceAddress = 18;
};

struct EShaderPreprocessor
{
	EShaderPreprocessor () {}
	EShaderPreprocessor (uint32) {}
	operator uint32 () const;
	static constexpr uint32 None = 0;

	// Use <aestyle.glsl.h> for auto-complete in IDE.
	static constexpr uint32 AEStyle = 1;
};

struct GlobalConfig
{
	GlobalConfig ();
	GlobalConfig (const GlobalConfig&);
	GlobalConfig&  operator = (const GlobalConfig&);

	// Set target GAPI and platform.
	void  SetTarget (ECompilationTarget);

	// Set default shader version.
	void  SetShaderVersion (EShaderVersion);

	// Set MSL version which is used when cross compiling from SPIRV.
	void  SetSpirvToMslVersion (EShaderVersion);

	// Set default shader options.
	void  SetShaderOptions (EShaderOpt);

	// Set default layout (align rules) for shader structure.
	void  SetDefaultLayout (EStructLayout);

	// Set default pipeline options.
	void  SetPipelineOptions (EPipelineOpt);

	// Set shader preprocessor.
	// Can transform any source to GLSL/MSL code.
	void  SetPreprocessor (EShaderPreprocessor);

	// Set FeatureSet which will be added to all resources.
	void  SetDefaultFeatureSet (const string & fsName);
};

struct NamedRenderState
{
	NamedRenderState ();

	// Create render state.
	// Name is used only in script.
	NamedRenderState (const string & name);
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
	EFormatFeature (uint32) {}
	operator uint32 () const;
	static constexpr uint32 StorageImageAtomic = 1;
	static constexpr uint32 StorageImage = 2;
	static constexpr uint32 AttachmentBlend = 3;
	static constexpr uint32 Attachment = 4;
	static constexpr uint32 LinearSampled = 5;
	static constexpr uint32 UniformTexelBuffer = 6;
	static constexpr uint32 StorageTexelBuffer = 7;
	static constexpr uint32 StorageTexelBufferAtomic = 8;
	static constexpr uint32 HWCompressedAttachment = 9;
	static constexpr uint32 LossyCompressedAttachment = 10;
};

struct FeatureSet
{
	FeatureSet ();
	FeatureSet (const string & name);
	void  AddTexelFormats (EFormatFeature, const array<EPixelFormat> &);
	void  AddSurfaceFormats (const array<ESurfaceFormat> &);
	void  AddSubgroupOperation (ESubgroupOperation);
	void  AddSubgroupOperations (const array<ESubgroupOperation> &);
	void  AddSubgroupOperationRange (ESubgroupOperation, ESubgroupOperation);
	void  AddVertexFormats (const array<EVertexType> &);
	void  AddAccelStructVertexFormats (const array<EVertexType> &);
	void  IncludeVendor (EGPUVendor);
	void  ExcludeVendor (EGPUVendor);
	void  IncludeVendors (const array<EGPUVendor> &);
	void  ExcludeVendors (const array<EGPUVendor> &);
	void  IncludeDevice (EGraphicsDeviceID);
	void  ExcludeDevice (EGraphicsDeviceID);
	void  MergeMin (RC<FeatureSet>);
	void  MergeMax (RC<FeatureSet>);
	void  Copy (RC<FeatureSet>);
	void  framebufferColorSampleCounts (const array<uint> &);
	void  framebufferDepthSampleCounts (const array<uint> &);
	void  maxSpirvVersion (uint);
	void  maxMetalVersion (uint);
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
	void  triangleFans (EFeature);
	void  subgroupTypes (uint);
	void  subgroupStages (uint);
	void  subgroupQuadStages (uint);
	void  requiredSubgroupSizeStages (uint);
	void  minSubgroupSize (uint);
	void  maxSubgroupSize (uint);
	void  subgroup (EFeature);
	void  subgroupBroadcastDynamicId (EFeature);
	void  subgroupSizeControl (EFeature);
	void  shaderSubgroupUniformControlFlow (EFeature);
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
	void  cooperativeMatrix (EFeature);
	void  cooperativeMatrixStages (uint);
	void  shaderClipDistance (EFeature);
	void  shaderCullDistance (EFeature);
	void  shaderResourceMinLod (EFeature);
	void  shaderDrawParameters (EFeature);
	void  runtimeDescriptorArray (EFeature);
	void  shaderSMBuiltinsNV (EFeature);
	void  shaderCoreBuiltinsARM (EFeature);
	void  shaderSampleRateInterpolationFunctions (EFeature);
	void  shaderStencilExport (EFeature);
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
	void  maxRayRecursionDepth (uint);
	void  drawIndirectFirstInstance (EFeature);
	void  drawIndirectCount (EFeature);
	void  multiview (EFeature);
	void  multiviewGeometryShader (EFeature);
	void  multiviewTessellationShader (EFeature);
	void  maxMultiviewViewCount (uint);
	void  multiViewport (EFeature);
	void  maxViewports (uint);
	void  sampleLocations (EFeature);
	void  variableSampleLocations (EFeature);
	void  tessellationIsolines (EFeature);
	void  tessellationPointMode (EFeature);
	void  maxTexelBufferElements (uint);
	void  maxUniformBufferSize (uint);
	void  maxStorageBufferSize (uint);
	void  perDescrSet_maxUniformBuffersDynamic (uint);
	void  perDescrSet_maxStorageBuffersDynamic (uint);
	void  maxDescriptorSets (uint);
	void  maxTexelOffset (uint);
	void  maxTexelGatherOffset (uint);
	void  maxFragmentOutputAttachments (uint);
	void  maxFragmentDualSrcAttachments (uint);
	void  maxFragmentCombinedOutputResources (uint);
	void  maxPushConstantsSize (uint);
	void  maxTotalThreadgroupSize (uint);
	void  maxTotalTileMemory (uint);
	void  maxVertAmplification (uint);
	void  maxComputeSharedMemorySize (uint);
	void  maxComputeWorkGroupInvocations (uint);
	void  maxComputeWorkGroupSizeX (uint);
	void  maxComputeWorkGroupSizeY (uint);
	void  maxComputeWorkGroupSizeZ (uint);
	void  taskShader (EFeature);
	void  meshShader (EFeature);
	void  maxTaskWorkGroupSize (uint);
	void  maxMeshWorkGroupSize (uint);
	void  maxMeshOutputVertices (uint);
	void  maxMeshOutputPrimitives (uint);
	void  maxMeshOutputPerVertexGranularity (uint);
	void  maxMeshOutputPerPrimitiveGranularity (uint);
	void  maxTaskPayloadSize (uint);
	void  maxTaskSharedMemorySize (uint);
	void  maxTaskPayloadAndSharedMemorySize (uint);
	void  maxMeshSharedMemorySize (uint);
	void  maxMeshPayloadAndSharedMemorySize (uint);
	void  maxMeshOutputMemorySize (uint);
	void  maxMeshPayloadAndOutputMemorySize (uint);
	void  maxMeshMultiviewViewCount (uint);
	void  maxPreferredTaskWorkGroupInvocations (uint);
	void  maxPreferredMeshWorkGroupInvocations (uint);
	void  maxRasterOrderGroups (uint);
	void  geometryShader (EFeature);
	void  tessellationShader (EFeature);
	void  computeShader (EFeature);
	void  tileShader (EFeature);
	void  vertexDivisor (EFeature);
	void  maxVertexAttribDivisor (uint);
	void  maxVertexAttributes (uint);
	void  maxVertexBuffers (uint);
	void  imageCubeArray (EFeature);
	void  textureCompressionASTC_LDR (EFeature);
	void  textureCompressionASTC_HDR (EFeature);
	void  textureCompressionBC (EFeature);
	void  textureCompressionETC2 (EFeature);
	void  imageViewMinLod (EFeature);
	void  multisampleArrayImage (EFeature);
	void  imageViewFormatList (EFeature);
	void  imageViewExtendedUsage (EFeature);
	void  maxImageArrayLayers (uint);
	void  samplerAnisotropy (EFeature);
	void  samplerMirrorClampToEdge (EFeature);
	void  samplerFilterMinmax (EFeature);
	void  filterMinmaxImageComponentMapping (EFeature);
	void  samplerMipLodBias (EFeature);
	void  samplerYcbcrConversion (EFeature);
	void  ycbcr2Plane444 (EFeature);
	void  nonSeamlessCubeMap (EFeature);
	void  maxSamplerAnisotropy (float);
	void  maxSamplerLodBias (float);
	void  maxFramebufferLayers (uint);
	void  variableMultisampleRate (EFeature);
	void  externalFormatAndroid (EFeature);
	void  metalArgBufferTier (uint);
	void  perDescrSet_maxInputAttachments (uint);
	void  perDescrSet_maxSampledImages (uint);
	void  perDescrSet_maxSamplers (uint);
	void  perDescrSet_maxStorageBuffers (uint);
	void  perDescrSet_maxStorageImages (uint);
	void  perDescrSet_maxUniformBuffers (uint);
	void  perDescrSet_maxAccelStructures (uint);
	void  perDescrSet_maxTotalResources (uint);
	void  perStage_maxInputAttachments (uint);
	void  perStage_maxSampledImages (uint);
	void  perStage_maxSamplers (uint);
	void  perStage_maxStorageBuffers (uint);
	void  perStage_maxStorageImages (uint);
	void  perStage_maxUniformBuffers (uint);
	void  perStage_maxAccelStructures (uint);
	void  perStage_maxTotalResources (uint);
};

RC<FeatureSet>  FindFeatureSet (const string & name);
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
	ETessPatch (uint32) {}
	operator uint32 () const;
	static constexpr uint32 Points = 1;
	static constexpr uint32 Isolines = 2;
	static constexpr uint32 Triangles = 3;
	static constexpr uint32 Quads = 4;
};

struct ETessSpacing
{
	ETessSpacing () {}
	ETessSpacing (uint32) {}
	operator uint32 () const;
	static constexpr uint32 Equal = 1;
	static constexpr uint32 FractionalEven = 2;
	static constexpr uint32 FractionalOdd = 3;
};

struct Shader
{
	Shader ();

	// Set shader source in script.
	string source;

	// Load shader source from file.
	string file;

	// Set custom shader version.
	// By default used value from 'GlobalConfig::SetShaderVersion()'.
	EShaderVersion version;

	// Set shader options.
	// By default used value from 'GlobalConfig::SetShaderOptions()'.
	EShaderOpt options;

	// Set shader type.
	// Optional for compute/graphics/mesh pipelines, required for ray tracing pipeline.
	EShader type;

	// Add specialization constant.
	void  AddSpec (EValueType valueType, const string & name);

	// Add macros which will be used in shader.
	// Format: MACROS = value; DEF
	void  Define (const string &);

	// Load shader source from current file.
	void  LoadSelf ();

	// Add specialization constant for dynamic workgroup size.
	// Only for compute shader. Must be explicitly specialized by 'ComputePipelineSpec::SetLocalSize()'.
	void  ComputeSpec1 ();
	void  ComputeSpec2 ();
	void  ComputeSpec3 ();

	// Add specialization constant for dynamic workgroup size.
	// Only for compute shader. Can be explicitly specialized by 'ComputePipelineSpec::SetLocalSize()', otherwise default value will be used
	void  ComputeSpecAndDefault (uint x);
	void  ComputeSpecAndDefault (uint x, uint y);
	void  ComputeSpecAndDefault (uint x, uint y, uint z);

	// Set constant workgroup size. Only for compute shader.
	void  ComputeLocalSize (uint x);
	void  ComputeLocalSize (uint x, uint y);
	void  ComputeLocalSize (uint x, uint y, uint z);

	// Add specialization constant for dynamic workgroup size.
	// Only for mesh/task shader. Must be explicitly specialized by 'MeshPipelineSpec::SetLocalSize()'.
	void  MeshSpec1 ();
	void  MeshSpec2 ();
	void  MeshSpec3 ();

	// Add specialization constant for dynamic workgroup size.
	// Only for mesh/task shader. Can be explicitly specialized by 'MeshPipelineSpec::SetLocalSize()', otherwise default value will be used
	void  MeshSpecAndDefault (uint x);
	void  MeshSpecAndDefault (uint x, uint y);
	void  MeshSpecAndDefault (uint x, uint y, uint z);

	// Set constant workgroup size. Only for mesh/task shader.
	void  MeshLocalSize (uint x);
	void  MeshLocalSize (uint x, uint y);
	void  MeshLocalSize (uint x, uint y, uint z);

	// Set size and topology for mesh shader output.
	void  MeshOutput (uint maxVertices, uint maxPrimitives, EPrimitive primitive);

	// Set number of vertices in tessellation patch.
	// Only for graphics pipeline with tessellation shader.
	void  TessPatchSize (uint vertexCount);

	// Set tessellation mode.
	// Only for graphics pipeline with tessellation shader.
	void  TessPatchMode (ETessPatch mode, ETessSpacing spacing, bool ccw);
};

struct ShaderStructTypeUsage
{
	ShaderStructTypeUsage () {}
	ShaderStructTypeUsage (uint32) {}
	operator uint32 () const;

	// Used as interface between graphics pipeline stages. Reflection to C++ is not supported.
	static constexpr uint32 ShaderIO = 1;

	// Used as vertex buffer layout. Enables reflection to C++.
	static constexpr uint32 VertexLayout = 4;

	// Used as vertex attributes in shader.
	// Reflection to C++ is not enabled, use 'VertexLayout' to enable it.
	static constexpr uint32 VertexAttribs = 2;

	// Used as uniform/storage buffer. Layout must be same in GLSL/MSL and C++. Enables reflection to C++.
	static constexpr uint32 BufferLayout = 8;

	// Used as buffer reference in shader. Layout must be same between shaders in single platform.
	// Reflection to C++ is not enabled, use 'BufferLayout' to enable it.
	static constexpr uint32 BufferReference = 16;
};

struct ShaderStructType
{
	ShaderStructType ();

	// Create structure type.
	// Name is used as typename for uniform/storage/vertex buffer or as shader in/out block.
	ShaderStructType (const string & name);

	// Add FeatureSet to the structure.
	// If used float64/int64 types FeatureSet must support this features.
	void  AddFeatureSet (const string & fsName);

	// Set source with structure fields.
	// Layout - offset and align rules.
	void  Set (EStructLayout layout, const string & fields);
	void  Set (const string & fields);

	// Manually specify how structure will be used.
	void  AddUsage (ShaderStructTypeUsage);

	// Returns size of the static data.
	uint  StaticSize () const;

	// Returns array element size for dynamic arrays.
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

	// Create vertex buffer.
	// Name is used as typename for vertex data.
	VertexBufferInput (const string & name);

	// Add vertex attribute.
	void  Add (const string & bufferName, const string & vertexTypeName, uint stride);
	void  Add (const string & bufferName, const RC<ShaderStructType> & vertexType, uint stride);
	void  Add (const string & bufferName, const string & vertexTypeName);
	void  Add (const string & bufferName, const RC<ShaderStructType> & vertexType);
	void  Add (const string & bufferName, const string & vertexTypeName, const Align & align);
	void  Add (const string & bufferName, const RC<ShaderStructType> & vertexType, const Align & align);

	// Add vertex attribute with divisor.
	void  Add (const string & bufferName, const string & vertexTypeName, uint stride, const VertexDivisor & divisor);
	void  Add (const string & bufferName, const RC<ShaderStructType> & vertexType, uint stride, const VertexDivisor & divisor);
	void  Add (const string & bufferName, const string & vertexTypeName, const VertexDivisor & divisor);
	void  Add (const string & bufferName, const RC<ShaderStructType> & vertexType, const VertexDivisor & divisor);
	void  Add (const string & bufferName, const string & vertexTypeName, const Align & align, const VertexDivisor & divisor);
	void  Add (const string & bufferName, const RC<ShaderStructType> & vertexType, const Align & align, const VertexDivisor & divisor);

	// Add FeatureSet to the vertex buffer.
	void  AddFeatureSet (const string & fsName);
};


// Check if two vertex buffers have the same attributes.
void  SameAttribs (const string & lhs, const string & rhs);
struct DescriptorSetLayout
{
	DescriptorSetLayout ();

	// Create descriptor set layout.
	// Name may be used in C++ code to create descriptor set.
	DescriptorSetLayout (const string & name);

	// Add FeatureSet to the descriptor set and all dependent resources.
	void  AddFeatureSet (const string & fsName);

	// Add macros which will be used in shader.
	// Format: MACROS = value; DEF
	void  Define (const string &);

	// Set descriptor set usage (EDescSetUsage).
	void  SetUsage (EDescSetUsage);
	void  SetUsage (uint);

	// Add input attachment from render technique graphics pass.
	void  SubpassInputFromRenderTech (const string & rtech, const string & gpass);

	// Add input attachment from render pass subpass.
	void  SubpassInputFromRenderPass (const string & compatRP, const string & subpass);

	// Add uniform buffer.
	void  UniformBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName);
	void  UniformBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName);
	void  UniformBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EResourceState state);
	void  UniformBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EResourceState state);
	void  UniformBuffer (EShaderStages shaderStages, const string & uniform, const string & typeName);
	void  UniformBuffer (uint shaderStages, const string & uniform, const string & typeName);
	void  UniformBuffer (EShaderStages shaderStages, const string & uniform, const string & typeName, EResourceState state);
	void  UniformBuffer (uint shaderStages, const string & uniform, const string & typeName, EResourceState state);

	// Add dynamic uniform buffer.
	void  UniformBufferDynamic (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName);
	void  UniformBufferDynamic (uint shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName);
	void  UniformBufferDynamic (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EResourceState state);
	void  UniformBufferDynamic (uint shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EResourceState state);
	void  UniformBufferDynamic (EShaderStages shaderStages, const string & uniform, const string & typeName);
	void  UniformBufferDynamic (uint shaderStages, const string & uniform, const string & typeName);
	void  UniformBufferDynamic (EShaderStages shaderStages, const string & uniform, const string & typeName, EResourceState state);
	void  UniformBufferDynamic (uint shaderStages, const string & uniform, const string & typeName, EResourceState state);

	// Add storage buffer.
	void  StorageBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName);
	void  StorageBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName);
	void  StorageBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EResourceState state);
	void  StorageBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EResourceState state);
	void  StorageBuffer (EShaderStages shaderStages, const string & uniform, const string & typeName);
	void  StorageBuffer (uint shaderStages, const string & uniform, const string & typeName);
	void  StorageBuffer (EShaderStages shaderStages, const string & uniform, const string & typeName, EResourceState state);
	void  StorageBuffer (uint shaderStages, const string & uniform, const string & typeName, EResourceState state);
	void  StorageBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EAccessType access);
	void  StorageBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EAccessType access);
	void  StorageBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EAccessType access, EResourceState state);
	void  StorageBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EAccessType access, EResourceState state);
	void  StorageBuffer (EShaderStages shaderStages, const string & uniform, const string & typeName, EAccessType access);
	void  StorageBuffer (uint shaderStages, const string & uniform, const string & typeName, EAccessType access);
	void  StorageBuffer (EShaderStages shaderStages, const string & uniform, const string & typeName, EAccessType access, EResourceState state);
	void  StorageBuffer (uint shaderStages, const string & uniform, const string & typeName, EAccessType access, EResourceState state);

	// Add dynamic storage buffer.
	void  StorageBufferDynamic (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName);
	void  StorageBufferDynamic (uint shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName);
	void  StorageBufferDynamic (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EResourceState state);
	void  StorageBufferDynamic (uint shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EResourceState state);
	void  StorageBufferDynamic (EShaderStages shaderStages, const string & uniform, const string & typeName);
	void  StorageBufferDynamic (uint shaderStages, const string & uniform, const string & typeName);
	void  StorageBufferDynamic (EShaderStages shaderStages, const string & uniform, const string & typeName, EResourceState state);
	void  StorageBufferDynamic (uint shaderStages, const string & uniform, const string & typeName, EResourceState state);
	void  StorageBufferDynamic (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EAccessType access);
	void  StorageBufferDynamic (uint shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EAccessType access);
	void  StorageBufferDynamic (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EAccessType access, EResourceState state);
	void  StorageBufferDynamic (uint shaderStages, const string & uniform, const ArraySize & arraySize, const string & typeName, EAccessType access, EResourceState state);
	void  StorageBufferDynamic (EShaderStages shaderStages, const string & uniform, const string & typeName, EAccessType access);
	void  StorageBufferDynamic (uint shaderStages, const string & uniform, const string & typeName, EAccessType access);
	void  StorageBufferDynamic (EShaderStages shaderStages, const string & uniform, const string & typeName, EAccessType access, EResourceState state);
	void  StorageBufferDynamic (uint shaderStages, const string & uniform, const string & typeName, EAccessType access, EResourceState state);

	// Add uniform (sampled) texel buffer.
	// 'imageType' must be 'Buffer | Int/Uint/Float'
	void  UniformTexelBuffer (EShaderStages shaderStages, const string & uniform, EImageType imageType);
	void  UniformTexelBuffer (uint shaderStages, const string & uniform, EImageType imageType);
	void  UniformTexelBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType);
	void  UniformTexelBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType);
	void  UniformTexelBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state);
	void  UniformTexelBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state);

	// Add storage texel buffer.
	// 'imageType' must be 'Buffer | Int/Uint/Float'.
	// 'format' must be included in 'storageImageFormats' in at least one of feature set.
	void  StorageTexelBuffer (EShaderStages shaderStages, const string & uniform, EPixelFormat format);
	void  StorageTexelBuffer (uint shaderStages, const string & uniform, EPixelFormat format);
	void  StorageTexelBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EPixelFormat format);
	void  StorageTexelBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, EPixelFormat format);
	void  StorageTexelBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EPixelFormat format, EResourceState state);
	void  StorageTexelBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, EPixelFormat format, EResourceState state);
	void  StorageTexelBuffer (EShaderStages shaderStages, const string & uniform, EPixelFormat format, EAccessType access);
	void  StorageTexelBuffer (uint shaderStages, const string & uniform, EPixelFormat format, EAccessType access);
	void  StorageTexelBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EPixelFormat format, EAccessType access);
	void  StorageTexelBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, EPixelFormat format, EAccessType access);
	void  StorageTexelBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EPixelFormat format, EAccessType access, EResourceState state);
	void  StorageTexelBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, EPixelFormat format, EAccessType access, EResourceState state);

	// Add storage texel buffer.
	// 'imageType' must be 'Buffer | Int/Uint/Float'.
	// Requires 'shaderStorageImageReadWithoutFormat' or 'shaderStorageImageWriteWithoutFormat' feature.
	void  StorageTexelBuffer (EShaderStages shaderStages, const string & uniform, EImageType imageType);
	void  StorageTexelBuffer (uint shaderStages, const string & uniform, EImageType imageType);
	void  StorageTexelBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType);
	void  StorageTexelBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType);
	void  StorageTexelBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state);
	void  StorageTexelBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state);
	void  StorageTexelBuffer (EShaderStages shaderStages, const string & uniform, EImageType imageType, EAccessType access);
	void  StorageTexelBuffer (uint shaderStages, const string & uniform, EImageType imageType, EAccessType access);
	void  StorageTexelBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access);
	void  StorageTexelBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access);
	void  StorageTexelBuffer (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access, EResourceState state);
	void  StorageTexelBuffer (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access, EResourceState state);

	// Add storage image.
	// 'format' must be included in 'storageImageFormats' in at least one of feature set.
	void  StorageImage (EShaderStages shaderStages, const string & uniform, EImageType imageType, EPixelFormat format);
	void  StorageImage (uint shaderStages, const string & uniform, EImageType imageType, EPixelFormat format);
	void  StorageImage (EShaderStages shaderStages, const string & uniform, EImageType imageType, EPixelFormat format, EAccessType access);
	void  StorageImage (uint shaderStages, const string & uniform, EImageType imageType, EPixelFormat format, EAccessType access);
	void  StorageImage (EShaderStages shaderStages, const string & uniform, EImageType imageType, EPixelFormat format, EResourceState state);
	void  StorageImage (uint shaderStages, const string & uniform, EImageType imageType, EPixelFormat format, EResourceState state);
	void  StorageImage (EShaderStages shaderStages, const string & uniform, EImageType imageType, EPixelFormat format, EAccessType access, EResourceState state);
	void  StorageImage (uint shaderStages, const string & uniform, EImageType imageType, EPixelFormat format, EAccessType access, EResourceState state);
	void  StorageImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EPixelFormat format);
	void  StorageImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EPixelFormat format);
	void  StorageImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EPixelFormat format, EAccessType access);
	void  StorageImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EPixelFormat format, EAccessType access);
	void  StorageImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EPixelFormat format, EResourceState state);
	void  StorageImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EPixelFormat format, EResourceState state);
	void  StorageImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EPixelFormat format, EAccessType access, EResourceState state);
	void  StorageImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EPixelFormat format, EAccessType access, EResourceState state);

	// Add storage image.
	// Requires 'shaderStorageImageReadWithoutFormat' or 'shaderStorageImageWriteWithoutFormat' feature.
	void  StorageImage (EShaderStages shaderStages, const string & uniform, EImageType imageType);
	void  StorageImage (uint shaderStages, const string & uniform, EImageType imageType);
	void  StorageImage (EShaderStages shaderStages, const string & uniform, EImageType imageType, EAccessType access);
	void  StorageImage (uint shaderStages, const string & uniform, EImageType imageType, EAccessType access);
	void  StorageImage (EShaderStages shaderStages, const string & uniform, EImageType imageType, EResourceState state);
	void  StorageImage (uint shaderStages, const string & uniform, EImageType imageType, EResourceState state);
	void  StorageImage (EShaderStages shaderStages, const string & uniform, EImageType imageType, EAccessType access, EResourceState state);
	void  StorageImage (uint shaderStages, const string & uniform, EImageType imageType, EAccessType access, EResourceState state);
	void  StorageImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType);
	void  StorageImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType);
	void  StorageImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access);
	void  StorageImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access);
	void  StorageImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state);
	void  StorageImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state);
	void  StorageImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access, EResourceState state);
	void  StorageImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access, EResourceState state);

	// Add sampled image (without sampler).
	void  SampledImage (EShaderStages shaderStages, const string & uniform, EImageType imageType);
	void  SampledImage (uint shaderStages, const string & uniform, EImageType imageType);
	void  SampledImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType);
	void  SampledImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType);
	void  SampledImage (EShaderStages shaderStages, const string & uniform, EImageType imageType, EResourceState state);
	void  SampledImage (uint shaderStages, const string & uniform, EImageType imageType, EResourceState state);
	void  SampledImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state);
	void  SampledImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state);

	// Add sampled image with sampler.
	void  CombinedImage (EShaderStages shaderStages, const string & uniform, EImageType imageType);
	void  CombinedImage (uint shaderStages, const string & uniform, EImageType imageType);
	void  CombinedImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType);
	void  CombinedImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType);
	void  CombinedImage (EShaderStages shaderStages, const string & uniform, EImageType imageType, EResourceState state);
	void  CombinedImage (uint shaderStages, const string & uniform, EImageType imageType, EResourceState state);
	void  CombinedImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state);
	void  CombinedImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state);

	// Add sampled image with immutable sampler.
	void  CombinedImage (EShaderStages shaderStages, const string & uniform, EImageType imageType, const string & samplerName);
	void  CombinedImage (uint shaderStages, const string & uniform, EImageType imageType, const string & samplerName);
	void  CombinedImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, const string & samplerName);
	void  CombinedImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, const string & samplerName);
	void  CombinedImage (EShaderStages shaderStages, const string & uniform, EImageType imageType, EResourceState state, const string & samplerName);
	void  CombinedImage (uint shaderStages, const string & uniform, EImageType imageType, EResourceState state, const string & samplerName);
	void  CombinedImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state, const string & samplerName);
	void  CombinedImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state, const string & samplerName);
	void  CombinedImage (EShaderStages shaderStages, const string & uniform, EImageType imageType, const array<string> & samplerName);
	void  CombinedImage (uint shaderStages, const string & uniform, EImageType imageType, const array<string> & samplerName);
	void  CombinedImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, const array<string> & samplerName);
	void  CombinedImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, const array<string> & samplerName);
	void  CombinedImage (EShaderStages shaderStages, const string & uniform, EImageType imageType, EResourceState state, const array<string> & samplerName);
	void  CombinedImage (uint shaderStages, const string & uniform, EImageType imageType, EResourceState state, const array<string> & samplerName);
	void  CombinedImage (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state, const array<string> & samplerName);
	void  CombinedImage (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state, const array<string> & samplerName);

	// Add input attachment.
	void  SubpassInput (EShaderStages shaderStages, const string & uniform);
	void  SubpassInput (uint shaderStages, const string & uniform);
	void  SubpassInput (EShaderStages shaderStages, const string & uniform, EImageType imageType);
	void  SubpassInput (uint shaderStages, const string & uniform, EImageType imageType);
	void  SubpassInput (EShaderStages shaderStages, const string & uniform, EResourceState state);
	void  SubpassInput (uint shaderStages, const string & uniform, EResourceState state);
	void  SubpassInput (EShaderStages shaderStages, const string & uniform, EImageType imageType, EResourceState state);
	void  SubpassInput (uint shaderStages, const string & uniform, EImageType imageType, EResourceState state);
	void  SubpassInput (EShaderStages shaderStages, const string & uniform, uint inputAttachmentIndex);
	void  SubpassInput (uint shaderStages, const string & uniform, uint inputAttachmentIndex);
	void  SubpassInput (EShaderStages shaderStages, const string & uniform, uint inputAttachmentIndex, EImageType imageType);
	void  SubpassInput (uint shaderStages, const string & uniform, uint inputAttachmentIndex, EImageType imageType);
	void  SubpassInput (EShaderStages shaderStages, const string & uniform, uint inputAttachmentIndex, EResourceState state);
	void  SubpassInput (uint shaderStages, const string & uniform, uint inputAttachmentIndex, EResourceState state);
	void  SubpassInput (EShaderStages shaderStages, const string & uniform, uint inputAttachmentIndex, EImageType imageType, EResourceState state);
	void  SubpassInput (uint shaderStages, const string & uniform, uint inputAttachmentIndex, EImageType imageType, EResourceState state);

	// Add separate sampler.
	void  Sampler (EShaderStages shaderStages, const string & uniform);
	void  Sampler (uint shaderStages, const string & uniform);
	void  Sampler (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize);
	void  Sampler (uint shaderStages, const string & uniform, const ArraySize & arraySize);

	// Add immutable sampler.
	void  ImtblSampler (EShaderStages shaderStages, const string & uniform, const string & samplerName);
	void  ImtblSampler (uint shaderStages, const string & uniform, const string & samplerName);
	void  ImtblSampler (EShaderStages shaderStages, const string & uniform, const array<string> & samplerNames);
	void  ImtblSampler (uint shaderStages, const string & uniform, const array<string> & samplerNames);

	// Add ray tracing scene (top level acceleration structure).
	void  RayTracingScene (EShaderStages shaderStages, const string & uniform);
	void  RayTracingScene (uint shaderStages, const string & uniform);
	void  RayTracingScene (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize);
	void  RayTracingScene (uint shaderStages, const string & uniform, const ArraySize & arraySize);
};

struct PipelineLayout
{
	PipelineLayout ();

	// Create pipeline template.
	// Name is used only in script.
	PipelineLayout (const string & name);

	// Add FeatureSet to the pipeline layout.
	void  AddFeatureSet (const string & fsName);

	// Add descriptor set layout.
	void  DSLayout (uint index, const string & dslName);
	void  DSLayout (const string & dsName, uint index, const string & dslName);
	void  DSLayout (uint index, const RC<DescriptorSetLayout> & dsl);
	void  DSLayout (const string & dsName, uint index, const RC<DescriptorSetLayout> & dsl);

	// Add descriptor set layout for shader debugging.
	void  AddDebugDSLayout (EShaderOpt dbgMode, uint shaderStages);
	void  AddDebugDSLayout (uint index, EShaderOpt dbgMode, uint shaderStages);

	// Add descriptor set layout for shader debugging, returns 'false' if failed.
	bool  TryAddDebugDSLayout (EShaderOpt dbgMode, uint shaderStages);
	bool  TryAddDebugDSLayout (uint index, EShaderOpt dbgMode, uint shaderStages);

	// Set push constant layout for specific shader stage.
	// Push constants are native in Vulkan and emulated in Metal.
	void  PushConst (const string & pcName, const string & structName, EShader stage);
	void  PushConst (const string & pcName, const RC<ShaderStructType> & type, EShader stage);

	// Add macros which will be used in shader.
	// Format: MACROS = value; DEF
	void  Define (const string &);
};

struct EMutableRenderState
{
	EMutableRenderState () {}
	EMutableRenderState (uint32) {}
	operator uint32 () const;
};

struct GraphicsPass
{
	GraphicsPass ();

	// Set RenderPass and Subpass which is used in current pass.
	// All pipelines must be compatible with RenderPass and use same Subpass.
	void  SetRenderPass (const string & rp, const string & subpass);

	// Set default render state.
	// Pipelines can override some parameters.
	void  SetRenderState (const RenderState & rs);
	void  SetRenderState (const string & rsName);

	// Set render state mutable state which can be overriden by pipelines.
	void  SetMutableStates (EMutableRenderState states);

	// Set per-pass descriptor set layout.
	// All pipelines must contains this DSLayout.
	void  SetDSLayout (const string & dsl);
};

struct ComputePass
{
	ComputePass ();

	// Set per-pass descriptor set layout.
	// All pipelines must contains this DSLayout.
	void  SetDSLayout (const string & dsl);
};

struct RenderTechnique
{
	RenderTechnique ();

	// Create render technique.
	// Name is used in C++ code to create render technique.
	RenderTechnique (const string & name);

	// Add FeatureSet to the render technique.
	void  AddFeatureSet (const string & fsName);

	// Create graphics pass.
	// Name is used in C++ to begin render pass.
	RC<GraphicsPass>  AddGraphicsPass (const string & passName);

	// Create compute pass.
	RC<ComputePass>  AddComputePass (const string & passName);

	// Copy graphics pass from another render technique.
	RC<GraphicsPass>  CopyGraphicsPass (const string & newName, const string & rtech, const string & gpass);

	// Copy compute pass from another render technique.
	RC<ComputePass>  CopyComputePass (const string & newName, const string & rtech, const string & cpass);
};

struct GraphicsPipelineSpec
{
	GraphicsPipelineSpec ();

	// Set specialization value.
	// Specialization constant must be previously defined in shader by 'Shader::AddSpec()'.
	void  SetSpecValue (const string & name, uint value);
	void  SetSpecValue (const string & name, int value);
	void  SetSpecValue (const string & name, float value);

	// Set dynamic states (EPipelineDynamicState).
	// Supported: StencilCompareMask, StencilWriteMask, StencilReference, DepthBias, BlendConstants', FragmentShadingRate.
	void  SetDynamicState (uint states);

	// Set render state.
	void  SetRenderState (const RenderState & rs);
	void  SetRenderState (const string & rsName);

	// Set number of viewports. Default is 1.
	// Requires 'multiViewport' feature.
	void  SetViewportCount (uint count);

	// Attach pipeline to the render technique.
	// When rtech is created it will create all attached pipelines.
	void  AddToRenderTech (const string & rtech, const string & gpass);

	// Set pipeline options (EPipelineOpt).
	// Supported: 'Optimize'.
	// By default used value from 'GlobalConfig::SetPipelineOptions()'.
	void  SetOptions (EPipelineOpt opts);

	// Set vertex attributes.
	void  SetVertexInput (const string & vbName);
	void  SetVertexInput (const RC<VertexBufferInput> & vb);
};

struct GraphicsPipeline
{
	GraphicsPipeline ();

	// Create pipeline template.
	// Name is used in C++ code to create pipeline.
	GraphicsPipeline (const string & name);

	// Add macros which will be used in shader.
	// Format: MACROS = value; DEF
	void  Define (const string &);

	// Set vertex shader.
	// Pipeline will inherit shader feature sets.
	void  SetVertexShader (const RC<Shader> &);

	// Set tessellation control shader. This shader is optional.
	// Pipeline will inherit shader feature sets.
	void  SetTessControlShader (const RC<Shader> &);

	// Set tessellation evaluation shader. This shader is optional.
	// Pipeline will inherit shader feature sets.
	void  SetTessEvalShader (const RC<Shader> &);

	// Set geometry shader. This shader is optional.
	// Pipeline will inherit shader feature sets.
	void  SetGeometryShader (const RC<Shader> &);

	// Set vertex shader.
	// Pipeline will inherit shader feature sets.
	void  SetFragmentShader (const RC<Shader> &);

	// Create specialization for pipeline template.
	// Name is used in C++ code to get pipeline from render technique.
	// Pipeline specialization use the same pipeline layout, same shader binaries, compatible render pass, difference only in some parameters.
	RC<GraphicsPipelineSpec>  AddSpecialization (const string & specName);

	// Check is fragment shader compatible with render pass.
	void  TestRenderPass (const string & compatRP, const string & subpass);

	// Add FeatureSet to the pipeline.
	void  AddFeatureSet (const string & fsName);

	// Set pipeline layout.
	// Pipeline will inherit layout feature sets.
	void  SetLayout (const string & plName);
	void  SetLayout (const RC<PipelineLayout> & pl);

	// Set vertex default attributes.
	void  SetVertexInput (const string & vbName);
	void  SetVertexInput (const RC<VertexBufferInput> & vb);

	// Set shader interface between stages.
	void  SetShaderIO (EShader output, EShader input, const string & typeName);

	// Add fragment shader output variables from render technique graphics pass.
	// Don't use it with explicit shader output.
	void  SetFragmentOutputFromRenderTech (const string & renTechName, const string & passName);

	// Add fragment shader output variables from render pass.
	// Don't use it with explicit shader output.
	void  SetFragmentOutputFromRenderPass (const string & compatRP, const string & subpass);
};

struct MeshPipelineSpec
{
	MeshPipelineSpec ();

	// Set specialization value.
	// Specialization constant must be previously defined in shader by 'Shader::AddSpec()'.
	void  SetSpecValue (const string & name, uint value);
	void  SetSpecValue (const string & name, int value);
	void  SetSpecValue (const string & name, float value);

	// Set dynamic states (EPipelineDynamicState).
	// Supported: StencilCompareMask, StencilWriteMask, StencilReference, DepthBias, BlendConstants', FragmentShadingRate.
	void  SetDynamicState (uint states);

	// Set render state.
	void  SetRenderState (const RenderState & rs);
	void  SetRenderState2 (const string & rsName);

	// Set number of viewports. Default is 1.
	// Requires 'multiViewport' feature.
	void  SetViewportCount (uint count);

	// Set task shader workgroup size. All threads in workgroup can use same (shared) memory and payload.
	// Shader must use 'SetMeshSpec1/2/3()' to define specialization constant.
	void  SetTaskLocalSize (uint x);
	void  SetTaskLocalSize (uint x, uint y);
	void  SetTaskLocalSize (uint x, uint y, uint z);

	// Set mesh shader workgroup size. All threads in workgroup can use same (shared) memory.
	// Shader must use 'SetMeshSpec1/2/3()' to define specialization constant.
	void  SetMeshLocalSize (uint x);
	void  SetMeshLocalSize (uint x, uint y);
	void  SetMeshLocalSize (uint x, uint y, uint z);

	// Attach pipeline to the render technique.
	// When rtech is created it will create all attached pipelines.
	void  AddToRenderTech (const string & rtech, const string & gpass);

	// Set pipeline options (EPipelineOpt).
	// Supported: 'Optimize'.
	// By default used value from 'GlobalConfig::SetPipelineOptions()'.
	void  SetOptions (EPipelineOpt opts);
};

struct MeshPipeline
{
	MeshPipeline ();

	// Create pipeline template.
	// Name is used in C++ code to create pipeline.
	MeshPipeline (const string & name);

	// Add macros which will be used in shader.
	// Format: MACROS = value; DEF
	void  Define (const string &);

	// Set task shader. This shader is optional.
	// Pipeline will inherit shader feature sets.
	void  SetTaskShader (const RC<Shader> &);

	// Set mesh shader.
	// Pipeline will inherit shader feature sets.
	void  SetMeshShader (const RC<Shader> &);

	// Set fragment shader.
	// Pipeline will inherit shader feature sets.
	void  SetFragmentShader (const RC<Shader> &);

	// Create specialization for pipeline template.
	// Name is used in C++ code to get pipeline from render technique.
	// Pipeline specialization use the same pipeline layout, same shader binaries, compatible render pass, difference only in some parameters.
	RC<MeshPipelineSpec>  AddSpecialization (const string & specName);

	// Check is fragment shader compatible with render pass.
	void  TestRenderPass (const string & compatRP, const string & subpass);

	// Add FeatureSet to the pipeline.
	void  AddFeatureSet (const string & fsName);

	// Set pipeline layout.
	// Pipeline will inherit layout feature sets.
	void  SetLayout (const string & plName);
	void  SetLayout (const RC<PipelineLayout> & pl);

	// Set shader interface between stages.
	void  SetShaderIO (EShader output, EShader input, const string & typeName);

	// Add fragment shader output variables from render technique graphics pass.
	// Don't use it with explicit shader output.
	void  SetFragmentOutputFromRenderTech (const string & renTechName, const string & passName);

	// Add fragment shader output variables from render pass.
	// Don't use it with explicit shader output.
	void  SetFragmentOutputFromRenderPass (const string & compatRP, const string & subpass);
};

struct ComputePipelineSpec
{
	ComputePipelineSpec ();

	// Set specialization value.
	// Specialization constant must be previously defined in shader by 'Shader::AddSpec()'.
	void  SetSpecValue (const string & name, uint value);
	void  SetSpecValue (const string & name, int value);
	void  SetSpecValue (const string & name, float value);

	// Set dynamic states (EPipelineDynamicState).
	// None of the states are supported for compute pipeline.
	void  SetDynamicState (uint states);

	// Set compute shader workgroup size. All threads in workgroup can use same (shared) memory.
	// Shader must use 'ComputeSpec1/2/3()' to define specialization constant.
	void  SetLocalSize (uint x);
	void  SetLocalSize (uint x, uint y);
	void  SetLocalSize (uint x, uint y, uint z);

	// Attach pipeline to the render technique.
	// When rtech is created it will create all attached pipelines.
	void  AddToRenderTech (const string & rtech, const string & gpass);

	// Set pipeline options (EPipelineOpt).
	// Supported: 'Optimize', 'CS_DispatchBase'.
	// By default used value from 'GlobalConfig::SetPipelineOptions()'.
	void  SetOptions (EPipelineOpt opts);
};

struct ComputePipeline
{
	ComputePipeline ();

	// Create pipeline template.
	// Name is used in C++ code to create pipeline.
	ComputePipeline (const string & name);

	// Add macros which will be used in shader.
	// Format: MACROS = value; DEF
	void  Define (const string &);

	// Set compute shader.
	// Pipeline will inherit shader feature sets.
	void  SetShader (const RC<Shader> &);

	// Create specialization for pipeline template.
	// Name is used in C++ code to get pipeline from render technique.
	// Pipeline specialization use the same pipeline layout, same shader binary, difference only in some parameters.
	RC<ComputePipelineSpec>  AddSpecialization (const string & specName);

	// Add FeatureSet to the pipeline.
	void  AddFeatureSet (const string & fsName);

	// Set pipeline layout.
	// Pipeline will inherit layout feature sets.
	void  SetLayout (const string & plName);
	void  SetLayout (const RC<PipelineLayout> & pl);
};

struct TilePipelineSpec
{
	TilePipelineSpec ();

	// Set specialization value.
	// Specialization constant must be previously defined in shader by 'Shader::AddSpec()'.
	void  SetSpecValue (const string & name, uint value);
	void  SetSpecValue (const string & name, int value);
	void  SetSpecValue (const string & name, float value);

	// Set dynamic states (EPipelineDynamicState).
	// None of the states are supported for tile pipeline.
	void  SetDynamicState (uint states);

	// Set tile shader tile size. All threads in tile can use same (tile) memory.
	void  SetLocalSize (uint x);
	void  SetLocalSize (uint x, uint y);

	// Attach pipeline to the render technique.
	// When rtech is created it will create all attached pipelines.
	void  AddToRenderTech (const string & rtech, const string & gpass);

	// Set pipeline options (EPipelineOpt).
	// Supported: 'Optimize'.
	// By default used value from 'GlobalConfig::SetPipelineOptions()'.
	void  SetOptions (EPipelineOpt opts);
};

struct TilePipeline
{
	TilePipeline ();

	// Create pipeline template.
	// Name is used in C++ code to create pipeline.
	TilePipeline (const string & name);

	// Add macros which will be used in shader.
	// Format: MACROS = value; DEF
	void  Define (const string &);

	// Set tile shader.
	// Pipeline will inherit shader feature sets.
	void  SetShader (const RC<Shader> &);

	// Create specialization for pipeline template.
	// Name is used in C++ code to get pipeline from render technique.
	// Pipeline specialization use the same pipeline layout, same shader binary, difference only in some parameters.
	RC<TilePipelineSpec>  AddSpecialization (const string & specName);

	// Add FeatureSet to the pipeline.
	void  AddFeatureSet (const string & fsName);

	// Set pipeline layout.
	// Pipeline will inherit layout feature sets.
	void  SetLayout (const string & plName);
	void  SetLayout (const RC<PipelineLayout> & pl);
};

struct RayTracingPipelineSpec
{
	RayTracingPipelineSpec ();

	// Set specialization value.
	// Specialization constant must be previously defined in shader by 'Shader::AddSpec()'.
	void  SetSpecValue (const string & name, uint value);
	void  SetSpecValue (const string & name, int value);
	void  SetSpecValue (const string & name, float value);

	// Set dynamic states (EPipelineDynamicState).
	// Supported: 'RTStackSize'.
	void  SetDynamicState (uint states);

	// Attach pipeline to the render technique.
	// When rtech is created it will create all attached pipelines.
	void  AddToRenderTech (const string & rtech, const string & cpass);

	// Set pipeline options (EPipelineOpt).
	// Supported: Optimize, RT_NoNullAnyHitShaders, RT_NoNullClosestHitShaders, RT_NoNullMissShaders, RT_NoNullIntersectionShaders, RT_SkipTriangles, RT_SkipAABBs.
	// By default used value from 'GlobalConfig::SetPipelineOptions()'.
	void  SetOptions (EPipelineOpt opts);

	// Set max recursion.
	// User must check recursion depth in shader and avoid exceeding this limit.
	void  MaxRecursionDepth (uint);
};

struct RayTracingPipeline
{
	RayTracingPipeline ();

	// Create pipeline template.
	// Name is used in C++ code to create pipeline.
	RayTracingPipeline (const string & name);

	// Add macros which will be used in shader.
	// Format: MACROS = value; DEF
	void  Define (const string &);

	// Create specialization for pipeline template.
	// Name is used in C++ code to get pipeline from render technique.
	// Pipeline specialization use the same pipeline layout, same shader binary, difference only in some parameters.
	RC<RayTracingPipelineSpec>  AddSpecialization (const string & specName);

	// Add FeatureSet to the pipeline.
	void  AddFeatureSet (const string & fsName);

	// Set pipeline layout.
	// Pipeline will inherit layout feature sets.
	void  SetLayout (const string & plName);
	void  SetLayout (const RC<PipelineLayout> & pl);

	// Create general shader group. Can be used as RayGen/Miss/Callable shaders.
	void  AddGeneralShader (const string & groupName, const RC<Shader> & shader);

	// Create triangle hit group. Can be used only as HitGroup.
	// 'closestHit' must be defined, 'anyHit' is optional.
	void  AddTriangleHitGroup (const string & groupName, const RC<Shader> & closestHit, const RC<Shader> & anyHit);

	// Create procedural hit group. Can be used only as HitGroup.
	// 'intersection' and 'closestHit' must be defined, 'anyHit' is optional.
	void  AddProceduralHitGroup (const string & groupName, const RC<Shader> & intersection, const RC<Shader> & closestHit, const RC<Shader> & anyHit);
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
	EAttachment (uint32) {}
	operator uint32 () const;

	// Discard previous content. Used as optimization for TBDR architectures.
	static constexpr uint32 Invalidate = 0;

	// Color attachment.
	static constexpr uint32 Color = 1;

	// Used as input attachment and color attachment.
	static constexpr uint32 ReadWrite = 3;

	// Resolve attachment - will get content from multisampled color attachment.
	static constexpr uint32 ColorResolve = 2;

	// Input attachment.
	static constexpr uint32 Input = 4;

	// Depth attachment.
	static constexpr uint32 Depth = 5;

	// Keep attachment content between passes.
	static constexpr uint32 Preserve = 6;

	// Depth and stencil attachment.
	static constexpr uint32 DepthStencil = 5;

	// Fragment shading rate attachment.
	static constexpr uint32 ShadingRate = 7;
};

struct EAttachmentLoadOp
{
	EAttachmentLoadOp () {}
	EAttachmentLoadOp (uint8) {}
	operator uint8 () const;

	// Previous content will not be preserved.
	// In TBDR is allow to avoid transfer from global memory to cache.
	static constexpr uint8 Invalidate = 0;

	// Preserve attachment content.
	// In TBDR contents in global memory will be copied to cache.
	static constexpr uint8 Load = 1;

	// Clear attachment before first pass.
	// In TBDR is allow to avoid transfer from global memory to cache.
	static constexpr uint8 Clear = 2;

	// Attachment is not used at all.
	// Can be used to keep one compatible render pass and avoid unnecessary synchronizations for unused attachment.
	static constexpr uint8 None = 3;
};

struct EAttachmentStoreOp
{
	EAttachmentStoreOp () {}
	EAttachmentStoreOp (uint8) {}
	operator uint8 () const;

	// Attachment content will not needed after rendering.
	// In TBDR it allow to avoid transfer from cache to global memory.
	static constexpr uint8 Invalidate = 0;

	// Attachment content will be written to global memory.
	static constexpr uint8 Store = 1;

	// Attachment is read-only. Content may not be written to memory, but if changed then content in memory will be undefined.
	// In TBDR it allow to avoid transfer from cache to global memory.
	static constexpr uint8 None = 2;
};

struct ShaderIO
{
	ShaderIO ();
	ShaderIO (const ShaderIO&);
	ShaderIO&  operator = (const ShaderIO&);
	ShaderIO (const string & shaderVariableName);
	ShaderIO (const string & shaderVariableName, EShaderIO type);
	ShaderIO (uint colorOrInputAttachmentIndex, const string & shaderVariableName, EShaderIO type);
	ShaderIO (uint colorOrInputAttachmentIndex, const string & shaderVariableName);
};

struct Attachment
{
	Attachment ();

	// Attachment format.
	EPixelFormat format;

	// Number of samples for multisampling.
	MultiSamples samples;

	// Attachment usage in subpass.
	void  Usage (const string & subpass, EAttachment usage);
	void  Usage (const string & subpass, EAttachment usage, const ShaderIO & inOrOut);
	void  Usage (const string & subpass, EAttachment usage, const ShaderIO & in, const ShaderIO & out);
	void  Usage (const string & subpass, EAttachment usage, const uint2 & shadingRateTexelSize);

	// For debugging: print information to the log.
	void  Print () const;
};

struct AttachmentSpec
{
	AttachmentSpec ();

	// Load operation.
	// Specify how image content will be loaded from global memory to tile memory.
	EAttachmentLoadOp loadOp;

	// Store operation.
	// Specify how image content will be stored from tile memory to global memory.
	EAttachmentStoreOp storeOp;

	// Set image layout in subpass.
	void  Layout (const string & subpass, EResourceState state);
	void  Layout (const string & subpass, uint state);

	// Generate optimal layouts for current attachment.
	void  GenOptimalLayouts ();
	void  GenOptimalLayouts (EResourceState initialState, EResourceState finalState);
	void  GenOptimalLayouts (uint initialState, uint finalState);

	// For debugging: print information to the log.
	void  Print () const;
};

struct RenderPass
{
	RenderPass ();

	// Create specialization for attachment to set layout per subpass and load/store operations.
	// Specialization contains params which can not break render pass compatibility.
	RC<AttachmentSpec>  AddAttachment (const string & name);

	// Generate optimal layouts for all attachments. Used instead of specialization.
	void  GenOptimalLayouts ();

	// For debugging: print information to the log.
	void  Print () const;
};

struct CompatibleRenderPass
{
	CompatibleRenderPass ();

	// Create compatible render pass.
	// Name may be used in C++ code to create graphics/mesh/tile pipeline.
	CompatibleRenderPass (const string & name);

	// Create render pass specialization.
	// Name is used in C++ code to begin render pass (in 'RenderPassDesc').
	RC<RenderPass>  AddSpecialization (const string & rpName);

	// Create render pass attachment.
	// Name is used in C++ code to bind image to attachment (in 'RenderPassDesc').
	RC<Attachment>  AddAttachment (const string & attachmentName);

	// Create render pass subpass.
	// Name may be used in C++ code to create graphics/mesh/tile pipeline.
	void  AddSubpass (const string & subpassName);

	// Add FeatureSet to the render pass.
	// Render pass can use only features that are enabled in at least one FeatureSet.
	void  AddFeatureSet (const string & fsName);

	// For debugging: print information to the log.
	void  Print () const;
};

struct Sampler
{
	Sampler ();

	// Create sampler.
	// Name is used as typename for immutable samplers or to get sampler in C++ code.
	Sampler (const string & name);

	// Add FeatureSet to the sampler.
	// Sampler must only use features which is enable in FS.
	void  AddFeatureSet (const string & fsName);

	// Set min/mag/mipmap filter.
	void  Filter (EFilter min, EFilter mag, EMipmapFilter mipmap);

	// Set UVW address mode.
	void  AddressMode (EAddressMode uvw);
	void  AddressMode (EAddressMode u, EAddressMode v, EAddressMode w);

	// Set mipmap level of detail bias.
	void  MipLodBias (float bias);

	// Set level of detail range.
	void  LodRange (float min, float max);

	// Set anisotropy level.
	void  Anisotropy (float level);
	bool  TrySetAnisotropy (float level);

	// Set compare operator.
	void  CompareOp (ECompareOp op);

	// Set border color.
	void  BorderColor (EBorderColor color);

	// Set reduction mode.
	// Requires 'samplerFilterMinmax' feature. Default value is 'Average'.
	void  ReductionMode (EReductionMode mode);

	// Set sampler options.
	void  Options (ESamplerOpt);
	void  Options (uint);

	// ----
	// Requires 'samplerYcbcrConversion' feature
	// ----

	// Set Ycbcr format. Requires multiplanar format.
	void  Ycbcr_Format (EPixelFormat);
	void  Ycbcr_Format (EPixelFormatExternal);

	// Set Ycbcr model conversion.
	void  Ycbcr_Model (ESamplerYcbcrModelConversion);

	// Set Ycbcr range.
	void  Ycbcr_Range (ESamplerYcbcrRange);

	// Set Ycbcr component swizzle.
	// Format: 'ARGB', 'R001'.
	void  Ycbcr_Components (const string &);

	// Set Ycbcr X chroma location.
	void  Ycbcr_XChromaOffset (ESamplerChromaLocation);

	// Set Ycbcr Y chroma location.
	void  Ycbcr_YChromaOffset (ESamplerChromaLocation);

	// Set Ycbcr Y chroma filter.
	void  Ycbcr_ChromaFilter (EFilter);

	// Set Ycbcr force explicit reconstruction.
	void  Ycbcr_ForceExplicitReconstruction (bool);
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

	// Create ray tracing shader binding for ray tracing pipeline.
	// Name is used in C++ code to get ray tracing shader binding.
	RayTracingShaderBinding (const RC<RayTracingPipelineSpec> & ppln, const string & sbtName);

	// Set number of ray types.
	// It will be used to calculate offsets in table:
	// 	'hitShaders [InstanceCount] [RayTypeCount]'Where 'ray type' is primary, shadow, reflection and other. All types are user-defined.
	void  MaxRayTypes (uint count);

	// Bind shader group from ray tracing pipeline as a ray generation shader.
	void  BindRayGen (const string & groupName);

	// Bind shader group from ray tracing pipeline as a miss shader.
	// 'missIndex' should be < MaxRayTypes.
	void  BindMiss (const string & groupName, const RayIndex & missIndex);

	// Bind shader group from ray tracing pipeline as a hit group.
	// 'rayIndex' must be < MaxRayTypes.
	void  BindHitGroup (const string & groupName, const InstanceIndex & instanceIndex, const RayIndex & rayIndex);

	// Bind shader group from ray tracing pipeline as callable shader.
	void  BindCallable (const string & groupName, const CallableIndex & callableIndex);
};

bool  IsShaderTraceSupported ();
bool  IsMetalCompilerSupported ();
bool  IsVulkan ();
bool  IsMetal_iOS ();
bool  IsMetal_Mac ();
bool  IsMetal ();
RC<ShaderStructType>  GetShaderStructType (const string & name);
bool  HasRenderTech (const string &);
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
const string Sampler_LinearMipmapMirrorClamp;
const string Sampler_Anisotropy8Repeat;
const string Sampler_Anisotropy8MirrorRepeat;
const string Sampler_Anisotropy8Clamp;
const string Sampler_Anisotropy16Repeat;
const string Sampler_Anisotropy16MirrorRepeat;
const string Sampler_Anisotropy16Clamp;
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
struct RC<Shader> : Shader
{
	RC (const Shader &);
};

template <>
struct RC<ShaderStructType> : ShaderStructType
{
	RC (const ShaderStructType &);
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
struct RC<DescriptorSetLayout> : DescriptorSetLayout
{
	RC (const DescriptorSetLayout &);
};

template <>
struct RC<RenderTechnique> : RenderTechnique
{
	RC (const RenderTechnique &);
};

template <>
struct RC<RayTracingShaderBinding> : RayTracingShaderBinding
{
	RC (const RayTracingShaderBinding &);
};

template <>
struct RC<RayTracingPipeline> : RayTracingPipeline
{
	RC (const RayTracingPipeline &);
};

template <>
struct RC<AttachmentSpec> : AttachmentSpec
{
	RC (const AttachmentSpec &);
};

template <>
struct RC<RayTracingPipelineSpec> : RayTracingPipelineSpec
{
	RC (const RayTracingPipelineSpec &);
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
struct RC<RenderPass> : RenderPass
{
	RC (const RenderPass &);
};

template <>
struct RC<TilePipelineSpec> : TilePipelineSpec
{
	RC (const TilePipelineSpec &);
};

template <>
struct RC<TilePipeline> : TilePipeline
{
	RC (const TilePipeline &);
};

template <>
struct RC<VertexBufferInput> : VertexBufferInput
{
	RC (const VertexBufferInput &);
};

template <>
struct RC<Attachment> : Attachment
{
	RC (const Attachment &);
};

template <>
struct RC<Sampler> : Sampler
{
	RC (const Sampler &);
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
struct RC<ComputePass> : ComputePass
{
	RC (const ComputePass &);
};

template <>
struct RC<FeatureSet> : FeatureSet
{
	RC (const FeatureSet &);
};

