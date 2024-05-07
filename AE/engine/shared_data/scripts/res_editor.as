//47243657
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

struct MipmapLevel;
struct BaseController;
struct ESurfaceFormat;
struct FlightCamera;
struct TopDownCamera;
struct EReductionMode;
struct ScaleBiasCamera;
struct EPixelFormatExternal;
struct DepthStencil;
struct Scene;
struct EPixelFormat;
struct short4;
struct ushort2;
struct sbyte2;
struct ushort3;
struct int4;
struct int2;
struct ImageLayer;
struct int3;
struct EPolygonMode;
struct float4x3;
struct float4x2;
struct float4x4;
struct DynamicUInt;
struct Model;
struct float3x4;
struct float3x2;
struct float3x3;
struct uint4;
struct EColorSpace;
struct RayTracingPass;
struct RectU;
struct EGPUVendor;
struct FPSCamera;
struct ECompareOp;
struct DynamicDim;
struct EShaderStages;
struct RTInstanceMask;
struct EShader;
struct DynamicFloat;
struct uint2;
struct uint3;
struct RectI;
struct RGBA32u;
struct UnifiedGeometry_DrawMeshTasks;
struct RectF;
struct SphericalCube;
struct EMipmapFilter;
struct RGBA8u;
struct UnifiedGeometry_DrawMeshTasksIndirectCount;
struct OrbitalCamera;
struct EFeature;
struct CallableIndex;
struct Buffer;
struct UnifiedGeometry_DrawIndirectCount;
struct EBorderColor;
struct EImageType;
struct UnifiedGeometry;
struct UnifiedGeometry_DrawMeshTasksIndirect;
struct EResourceState;
struct ComputePass;
struct SceneRayTracingPass;
struct Postprocess;
struct RTInstanceSBTOffset;
struct RemoteCamera;
struct RTInstanceCustomIndex;
struct ubyte4;
struct ESamplerYcbcrRange;
struct Random_Normal4;
struct ESubgroupOperation;
struct VideoImage;
struct FPVCamera;
struct ubyte2;
struct ubyte3;
struct EGraphicsDeviceID;
struct EPrimitive;
struct Random_Normal2;
struct Random_Normal1;
struct Random_Normal3;
struct UnifiedGeometry_Draw;
struct HSVColor;
struct EQueueMask;
struct RGBA32f;
struct EBlendOp;
struct ECullMode;
struct ELogicOp;
struct EVertexType;
struct RGBA32i;
struct bool4;
struct SceneGraphicsPass;
struct EImage;
struct DynamicULong;
struct bool3;
struct bool2;
struct RTScene;
struct EPassFlags;
struct RayIndex;
struct Collection;
struct EAddressMode;
struct EStencilOp;
struct DbgViewFlags;
struct EBlendFactor;
struct ERTInstanceOpt;
struct DynamicFloat4;
struct EPostprocess;
struct DynamicUInt2;
struct RTShader;
struct DynamicUInt3;
struct DynamicFloat2;
struct ImageLoadOpFlags;
struct float4;
struct DynamicFloat3;
struct DynamicUInt4;
struct RTGeometry;
struct UnifiedGeometry_DrawIndirect;
struct GeomSource;
struct EIndex;
struct DynamicInt4;
struct DynamicInt3;
struct DynamicInt2;
struct Image;
struct UnifiedGeometry_DrawIndexedIndirectCount;
struct ScriptFlags;
struct ESamplerOpt;
struct ESubgroupTypes;
struct sbyte3;
struct sbyte4;
struct UnifiedGeometry_DrawIndexed;
struct short3;
struct ushort4;
struct Random;
struct short2;
struct ESamplerYcbcrModelConversion;
struct IPass;
struct MultiSamples;
struct Random_Binomial2;
struct Random_Binomial3;
struct Random_Binomial1;
struct EPipelineDynamicState;
struct Random_Binomial4;
struct EFilter;
struct float2x4;
struct float2x2;
struct float2x3;
struct UnifiedGeometry_DrawIndexedIndirect;
struct ESamplerChromaLocation;
struct EDescSetUsage;
struct EVertexInputRate;
struct EPipelineOpt;
struct InstanceIndex;
struct ERenderLayer;
struct float3;
struct DynamicInt;
struct float2;
struct EShaderIO;
struct RTInstanceTransform;
struct EImageAspect;

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

string  FindAndReplace (const string &, const string &, const string &);
bool  StartsWith (const string &, const string &);
bool  StartsWithIC (const string &, const string &);
bool  EndsWith (const string &, const string &);
bool  EndsWithIC (const string &, const string &);
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

struct float2x2
{
	float2x2 ();
	float2x2 (const float2x2&);
	float2x2&  operator = (const float2x2&);
	float2x2 (const float2x2 & value);
	float2x2 (const float2 & col0, const float2 & col1);
	float2 col0;
	float2 col1;
};

struct float2x3
{
	float2x3 ();
	float2x3 (const float2x3&);
	float2x3&  operator = (const float2x3&);
	float2x3 (const float2x3 & value);
	float2x3 (const float3 & col0, const float3 & col1);
	float3 col0;
	float3 col1;
};

struct float2x4
{
	float2x4 ();
	float2x4 (const float2x4&);
	float2x4&  operator = (const float2x4&);
	float2x4 (const float2x4 & value);
	float2x4 (const float4 & col0, const float4 & col1);
	float4 col0;
	float4 col1;
};

struct float3x2
{
	float3x2 ();
	float3x2 (const float3x2&);
	float3x2&  operator = (const float3x2&);
	float3x2 (const float3x2 & value);
	float3x2 (const float2 & col0, const float2 & col1, const float2 & col2);
	float2 col0;
	float2 col1;
	float2 col2;
};

struct float3x3
{
	float3x3 ();
	float3x3 (const float3x3&);
	float3x3&  operator = (const float3x3&);
	float3x3 (const float3x3 & value);
	float3x3 (const float3 & col0, const float3 & col1, const float3 & col2);
	float3 col0;
	float3 col1;
	float3 col2;
	float3x3  RotateX (float angle) const;
	float3x3  RotateY (float angle) const;
	float3x3  RotateZ (float angle) const;
};

struct float3x4
{
	float3x4 ();
	float3x4 (const float3x4&);
	float3x4&  operator = (const float3x4&);
	float3x4 (const float3x4 & value);
	float3x4 (const float4 & col0, const float4 & col1, const float4 & col2);
	float4 col0;
	float4 col1;
	float4 col2;
	float3x4  RotateX (float angle) const;
	float3x4  RotateY (float angle) const;
	float3x4  RotateZ (float angle) const;
};

struct float4x2
{
	float4x2 ();
	float4x2 (const float4x2&);
	float4x2&  operator = (const float4x2&);
	float4x2 (const float4x2 & value);
	float4x2 (const float2 & col0, const float2 & col1, const float2 & col2, const float2 & col3);
	float2 col0;
	float2 col1;
	float2 col2;
	float2 col3;
};

struct float4x3
{
	float4x3 ();
	float4x3 (const float4x3&);
	float4x3&  operator = (const float4x3&);
	float4x3 (const float4x3 & value);
	float4x3 (const float3 & col0, const float3 & col1, const float3 & col2, const float3 & col3);
	float3 col0;
	float3 col1;
	float3 col2;
	float3 col3;
	float4x3  RotateX (float angle) const;
	float4x3  RotateY (float angle) const;
	float4x3  RotateZ (float angle) const;
};

struct float4x4
{
	float4x4 ();
	float4x4 (const float4x4&);
	float4x4&  operator = (const float4x4&);
	float4x4 (const float4x4 & value);
	float4x4 (const float4 & col0, const float4 & col1, const float4 & col2, const float4 & col3);
	float4 col0;
	float4 col1;
	float4 col2;
	float4 col3;
	float4x4  RotateX (float angle) const;
	float4x4  RotateY (float angle) const;
	float4x4  RotateZ (float angle) const;
	float4x4  Rotate (float angle, const float3 & axis) const;
	float4x4  Ortho (const RectF & viewport, const float2 & range) const;
	float4x4  InfinitePerspective (float fovY, float aspect, float zNear) const;
	float4x4  Perspective (float fovY, float aspect, const float2 & range) const;
	float4x4  Perspective (float fovY, const float2 & viewport, const float2 & range) const;
	float4x4  Frustum (const RectF & viewport, const float2 & range) const;
	float4x4  InfiniteFrustum (const RectF & viewport, float zNear) const;
	float4x4  Translate (const float3 & translation) const;
	float4x4  Scaled (const float3 & scale) const;
	float4x4  Scaled (float scale) const;
	float3  Project (const float3 & pos, const RectF & viewport) const;
	float3  UnProject (const float3 & pos, const RectF & viewport) const;
	float4x4  LookAt (const float3 & eye, const float3 & center, const float3 & up) const;
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
void  LogError (const string & msg);
void  LogInfo (const string & msg);
void  LogDebug (const string & msg);
void  LogFatal (const string & msg);
void  Assert (bool expr);
void  Assert (bool expr, const string & msg);
struct Random_Binomial1
{
	Random_Binomial1 (const Random_Binomial1&);
};

struct Random_Binomial2
{
	Random_Binomial2 (const Random_Binomial2&);
};

struct Random_Binomial3
{
	Random_Binomial3 (const Random_Binomial3&);
};

struct Random_Binomial4
{
	Random_Binomial4 (const Random_Binomial4&);
};

struct Random_Normal1
{
	Random_Normal1 (const Random_Normal1&);
};

struct Random_Normal2
{
	Random_Normal2 (const Random_Normal2&);
};

struct Random_Normal3
{
	Random_Normal3 (const Random_Normal3&);
};

struct Random_Normal4
{
	Random_Normal4 (const Random_Normal4&);
};

struct Random
{
	Random ();
	Random (const Random&);
	Random&  operator = (const Random&);
	float  Uniform (float min, float max);
	float2  Uniform (const float2 & min, const float2 & max);
	float3  Uniform (const float3 & min, const float3 & max);
	float4  Uniform (const float4 & min, const float4 & max);
	int  Uniform (int min, int max);
	int2  Uniform (const int2 & min, const int2 & max);
	int3  Uniform (const int3 & min, const int3 & max);
	int4  Uniform (const int4 & min, const int4 & max);
	uint  Uniform (uint min, uint max);
	uint2  Uniform (const uint2 & min, const uint2 & max);
	uint3  Uniform (const uint3 & min, const uint3 & max);
	uint4  Uniform (const uint4 & min, const uint4 & max);
	RGBA32f  UniformColor ();
	bool  Bernoulli (float p);
	bool2  Bernoulli2 (float p);
	bool3  Bernoulli3 (float p);
	bool4  Bernoulli4 (float p);
	Random_Binomial1  Binomial (int trials, float probability);
	Random_Binomial2  Binomial2 (int trials, float probability);
	Random_Binomial3  Binomial3 (int trials, float probability);
	Random_Binomial4  Binomial4 (int trials, float probability);
	Random_Normal1  Normal (float mean, float sigma);
	Random_Normal2  Normal2 (float mean, float sigma);
	Random_Normal3  Normal3 (float mean, float sigma);
	Random_Normal4  Normal4 (float mean, float sigma);
};

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

struct EColorSpace
{
	EColorSpace () {}
	EColorSpace (uint8) {}
	operator uint8 () const;
	static constexpr uint8 sRGB_nonlinear = 0;
	static constexpr uint8 BT709_nonlinear = 6;
	static constexpr uint8 Extended_sRGB_linear = 2;
	static constexpr uint8 HDR10_ST2084 = 8;
	static constexpr uint8 BT2020_linear = 7;
};

struct DbgViewFlags
{
	DbgViewFlags () {}
	DbgViewFlags (uint32) {}
	operator uint32 () const;
	static constexpr uint32 NoCopy = 1;
	static constexpr uint32 Copy = 0;
	static constexpr uint32 Histogram = 2;
	static constexpr uint32 LinearDepth = 3;
	static constexpr uint32 Stencil = 4;
};

struct ScriptFlags
{
	ScriptFlags () {}
	ScriptFlags (uint32) {}
	operator uint32 () const;
	static constexpr uint32 RunOnce = 1;
	static constexpr uint32 OnRequest = 2;
	static constexpr uint32 RunOnce_AfterLoading = 3;
};

struct DynamicUInt
{
	DynamicUInt ();
	DynamicUInt (uint);
	RC<DynamicUInt>  Mul (uint);
};

struct DynamicUInt2
{
	DynamicUInt2 ();
	DynamicUInt2 (const uint2 &);
};

struct DynamicUInt3
{
	DynamicUInt3 ();
	DynamicUInt3 (const uint3 &);
};

struct DynamicUInt4
{
	DynamicUInt4 ();
	DynamicUInt4 (const uint4 &);
};

struct DynamicInt
{
	DynamicInt ();
	DynamicInt (int);
};

struct DynamicInt2
{
	DynamicInt2 ();
	DynamicInt2 (const int2 &);
};

struct DynamicInt3
{
	DynamicInt3 ();
	DynamicInt3 (const int3 &);
};

struct DynamicInt4
{
	DynamicInt4 ();
	DynamicInt4 (const int4 &);
};

struct DynamicFloat
{
	DynamicFloat ();
	DynamicFloat (float);
};

struct DynamicFloat2
{
	DynamicFloat2 ();
	DynamicFloat2 (const float2 &);
};

struct DynamicFloat3
{
	DynamicFloat3 ();
	DynamicFloat3 (const float3 &);
};

struct DynamicFloat4
{
	DynamicFloat4 ();
	DynamicFloat4 (const float4 &);
};

struct DynamicULong
{
	DynamicULong ();
	DynamicULong (uint64);
};

struct DynamicDim
{
	RC<DynamicDim>  opMul (int) const;
	RC<DynamicDim>  opDiv (int) const;
	RC<DynamicDim>  Mul (int) const;
	RC<DynamicDim>  Div (int) const;
	RC<DynamicDim>  DivRound (int) const;
	RC<DynamicDim>  DivCeil (int) const;
	RC<DynamicDim>  opMul (const int2 &) const;
	RC<DynamicDim>  opDiv (const int2 &) const;
	RC<DynamicDim>  Mul (const int2 &) const;
	RC<DynamicDim>  Div (const int2 &) const;
	RC<DynamicDim>  DivRound (const int2 &) const;
	RC<DynamicDim>  DivCeil (const int2 &) const;
	RC<DynamicDim>  opMul (const int3 &) const;
	RC<DynamicDim>  opDiv (const int3 &) const;
	RC<DynamicDim>  Mul (const int3 &) const;
	RC<DynamicDim>  Div (const int3 &) const;
	RC<DynamicDim>  DivRound (const int3 &) const;
	RC<DynamicDim>  DivCeil (const int3 &) const;
	RC<DynamicUInt>  Area () const;
	RC<DynamicUInt>  Volume () const;
};

struct ImageLoadOpFlags
{
	ImageLoadOpFlags () {}
	ImageLoadOpFlags (uint32) {}
	operator uint32 () const;

	// Generate mipmaps after loading
	static constexpr uint32 GenMipmaps = 1;
};

struct Image
{

	// Create image from file.
	// File will be searched in VFS.
	Image (EImageType imageType, const string & filenameInVFS);
	Image (EImageType imageType, const string & filenameInVFS, ImageLoadOpFlags flags);

	// Create image with constant dimension
	Image (EPixelFormat format, const uint2 & dimension);
	Image (EPixelFormat format, const uint3 & dimension);
	Image (EPixelFormat format, const uint2 & dimension, const ImageLayer & layers);
	Image (EPixelFormat format, const uint2 & dimension, const MipmapLevel & mipmaps);
	Image (EPixelFormat format, const uint3 & dimension, const MipmapLevel & mipmaps);
	Image (EPixelFormat format, const uint2 & dimension, const ImageLayer & layers, const MipmapLevel & mipmaps);

	// Create image with dynamic dimension
	Image (EPixelFormat format, const RC<DynamicDim> & dynamicDimension);
	Image (EPixelFormat format, const RC<DynamicDim> & dynamicDimension, const ImageLayer & layers);
	Image (EPixelFormat format, const RC<DynamicDim> & dynamicDimension, const MipmapLevel & mipmaps);
	Image (EPixelFormat format, const RC<DynamicDim> & dynamicDimension, const ImageLayer & layers, const MipmapLevel & mipmaps);

	// Set resource name. It is used for debugging.
	void  Name (const string &);

	// Load specified array layer from file, can be used for 2DArray/CubeMap/CubeMapArray.
	// File will be searched in VFS.
	void  LoadLayer (const string & filenameInVFS, uint layer);
	void  LoadLayer (const string & filenameInVFS, uint layer, ImageLoadOpFlags flags);
	void  LoadLayer (const string & filenameInVFS, uint layer, uint flags);

	// Returns 'true' if used dynamic dimension, methods 'Dimension2()', 'Dimension2_Layers()', 'Dimension3()' can not be used for dynamic dimension, only 'Dimension()' is allowed
	bool  IsMutableDimension () const;

	// Returns constant dimension of the image
	uint2  Dimension2 () const;
	uint3  Dimension2_Layers () const;
	uint3  Dimension3 () const;
	uint  ArrayLayers () const;
	uint  MipmapCount () const;

	// Returns dynamic dimension of the image
	RC<DynamicDim>  Dimension () const;

	// Set image swizzle like a 'RGBA', 'R000', ...
	void  SetSwizzle (const string &);

	// Set image aspect, otherwise it will be auto-detected.
	// DepthStencil images can not be sampled, you must choose depth or stencil aspect.
	void  SetAspectMask (EImageAspect);

	// Returns image description
	EImageType  ImageType () const;
	bool  IsFloatFormat () const;
	bool  IsIntFormat () const;
	bool  IsUIntFormat () const;
	bool  Is1D () const;
	bool  Is2D () const;
	bool  Is3D () const;
	bool  IsCube () const;
	bool  Is1DArray () const;
	bool  Is2DArray () const;
	bool  IsCubeArray () const;

	// Create image as view for current image.Can be used to create CubeMap from 2DArray or set different swizzle.
	RC<Image>  CreateView (EImage viewType, EPixelFormat format, const MipmapLevel & baseMipmap, uint mipmapCount, const ImageLayer & baseLayer, uint layerCount);
	RC<Image>  CreateView (EImage viewType, const MipmapLevel & baseMipmap, uint mipmapCount, const ImageLayer & baseLayer, uint layerCount);
	RC<Image>  CreateView (EImage viewType, const MipmapLevel & baseMipmap, uint mipmapCount);
	RC<Image>  CreateView (EImage viewType, const ImageLayer & baseLayer, uint layerCount);
	RC<Image>  CreateView (EImage viewType);
	RC<Image>  CreateView (EImage viewType, EPixelFormat format);
	RC<Image>  CreateView ();
};

struct VideoImage
{
	VideoImage ();
	VideoImage (const string & videoFilePath);
	VideoImage (EPixelFormat format, const string & videoFilePath);

	// Set resource name. It is used for debugging.
	void  Name (const string &);

	// Returns dynamic dimension of the image
	RC<DynamicDim>  Dimension ();
	void  Sampler (const string &);

	// Set Ycbcr format. Requires multiplanar format.
	void  Ycbcr_Format (EPixelFormat);

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

struct Buffer
{
	Buffer ();
	Buffer (uint sizeInBytes);
	Buffer (const string & filenameInVFS);

	// Set resource name. It is used for debugging.
	void  Name (const string &);

	// Get buffer type name. Result is valid only after 'Layout*()' methods.
	// Can be used for debugging: 'LogInfo( buf.TypeName() );'
	string  TypeName () const;

	// Set explicit name of the 'ShaderStructType' which will be created for buffer data layout.
	// It is used when buffer is passed to the pipeline which is explicitly declared (in 'pipelines' folder)
	// so typename must match in 'Layout()' and in 'ds.StorageBuffer()' call in pipeline script.
	void  LayoutName (const string & typeName);

	// Allow to declare array of struct with constant or dynamic size.
	// Layout will be '{ <typeName>  elements [<count>]; }'.
	// 'typeName' must be previously declared or one of built-in type:
	// 	'DispatchIndirectCommand', 'DrawIndirectCommand', 'DrawIndexedIndirectCommand',
	// 	'DrawMeshTasksIndirectCommand', 'TraceRayIndirectCommand', 'ASBuildIndirectCommand'
	// 	'AccelStructInstance'.
	void  ArrayLayout (const string & typeName, uint count);
	void  ArrayLayout (const string & typeName, const RC<DynamicUInt> & count);

	// Allow to declare array of struct with constant or dynamic size.
	// Created a new structure with type 'typeName' and fields in 'source'.
	// See field declaration rules for 'ShaderStructType::Set()' method in [pipeline_compiler.as](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/scripts/pipeline_compiler.as).
	void  ArrayLayout (const string & typeName, const string & source, uint count);
	void  ArrayLayout (const string & typeName, const string & source, const RC<DynamicUInt> & count);
	void  ArrayLayout (const string & typeName, const string & arrayElementSource, const string & staticSource, uint count);
	void  ArrayLayout (const string & typeName, const string & arrayElementSource, const string & staticSource, const RC<DynamicUInt> & count);

	// Allow to declare single structure as a buffer layout.
	// 'typeName' must be previously declared or one of built-in type (see 'ArrayLayout').
	void  UseLayout (const string & typeName);

	// Created a new structure with type 'typeName' and fields in 'source'.
	// See field declaration rules for 'ShaderStructType::Set()' method in [pipeline_compiler.as](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/scripts/pipeline_compiler.as).
	void  UseLayout (const string & typeName, const string & source);

	// Returns buffer device address.
	// Requires 'GL_EXT_buffer_reference extension' in GLSL.
	// It passed as 'uint64' type so you should cast it to buffer reference type.
	uint64  DeviceAddress ();

	// Force enable buffer content history.
	// It store copy of the buffer content on last N frames.
	void  EnableHistory ();

	// Call this method if 'DeviceAddress()' of another buffer is used in current buffer to avoid missed synchronizations.
	void  AddReference (const RC<Buffer> &);

	// Dynamic array size, can be used for draw call.
	RC<DynamicUInt>  ArraySize () const;

	// Build buffer data layout with initial content.
	// Returns offset in bytes where data is begin.
	uint  Float (const string &, float);
	uint  Float (const string &, float, float);
	uint  Float (const string &, float, float, float);
	uint  Float (const string &, float, float, float, float);
	uint  Float (const string &, const float2 &);
	uint  Float (const string &, const float3 &);
	uint  Float (const string &, const float4 &);
	uint  Float (const string &, const float2x2 &);
	uint  Float (const string &, const float2x3 &);
	uint  Float (const string &, const float2x4 &);
	uint  Float (const string &, const float3x2 &);
	uint  Float (const string &, const float3x3 &);
	uint  Float (const string &, const float3x4 &);
	uint  Float (const string &, const float4x2 &);
	uint  Float (const string &, const float4x3 &);
	uint  Float (const string &, const float4x4 &);
	uint  Int (const string &, int);
	uint  Int (const string &, int, int);
	uint  Int (const string &, int, int, int);
	uint  Int (const string &, int, int, int, int);
	uint  Int (const string &, const int2 &);
	uint  Int (const string &, const int3 &);
	uint  Int (const string &, const int4 &);
	uint  Uint (const string &, uint);
	uint  Uint (const string &, uint, uint);
	uint  Uint (const string &, uint, uint, uint);
	uint  Uint (const string &, uint, uint, uint, uint);
	uint  Uint (const string &, const uint2 &);
	uint  Uint (const string &, const uint3 &);
	uint  Uint (const string &, const uint4 &);
	uint  ULong (const string &, uint64);
	uint  FloatArray (const string &, const array<float> &);
	uint  FloatArray (const string &, const array<float2> &);
	uint  FloatArray (const string &, const array<float3> &);
	uint  FloatArray (const string &, const array<float4> &);
	uint  FloatArray (const string &, const array<float2x2> &);
	uint  FloatArray (const string &, const array<float2x3> &);
	uint  FloatArray (const string &, const array<float2x4> &);
	uint  FloatArray (const string &, const array<float3x2> &);
	uint  FloatArray (const string &, const array<float3x3> &);
	uint  FloatArray (const string &, const array<float3x4> &);
	uint  FloatArray (const string &, const array<float4x2> &);
	uint  FloatArray (const string &, const array<float4x3> &);
	uint  FloatArray (const string &, const array<float4x4> &);
	uint  IntArray (const string &, const array<int> &);
	uint  IntArray (const string &, const array<int2> &);
	uint  IntArray (const string &, const array<int3> &);
	uint  IntArray (const string &, const array<int4> &);
	uint  UIntArray (const string &, const array<uint> &);
	uint  UIntArray (const string &, const array<uint2> &);
	uint  UIntArray (const string &, const array<uint3> &);
	uint  UIntArray (const string &, const array<uint4> &);
	uint  ULongArray (const string &, const array<uint64> &);
};

struct RTGeometry
{
	RTGeometry ();

	// Set resource name. It is used for debugging.
	void  Name (const string &);

	// Add triangle mesh.
	// Supported formats:
	// 	float2/float3  position []/[x];
	// 	Vertex{ float2/float3  pos; ... }  verts []/[x];
	// 	with static or dynamic array.
	void  AddTriangles (const RC<Buffer> & vertexBuffer);
	void  AddTriangles (const RC<Buffer> & vertexBuffer, uint maxVertex, uint maxPrimitives);
	void  AddTriangles (const RC<Buffer> & vertexBuffer, const string & vbField);
	void  AddTriangles (const RC<Buffer> & vertexBuffer, const string & vbField, uint maxVertex, uint maxPrimitives);

	// Add indexed triangle mesh.
	// Supported formats:
	// 	ushort/uint  indices []/[x];
	// 	with static or dynamic array.
	void  AddIndexedTriangles (const RC<Buffer> & vertexBuffer, const RC<Buffer> & indexBuffer);
	void  AddIndexedTriangles (const RC<Buffer> & vertexBuffer, uint maxVertex, uint maxPrimitives, const RC<Buffer> & indexBuffer, EIndex indexType);
	void  AddIndexedTriangles (const RC<Buffer> & vertexBuffer, const string & vbField, const RC<Buffer> & indexBuffer, const string & ibField);
	void  AddIndexedTriangles (const RC<Buffer> & vertexBuffer, const string & vbField, uint maxVertex, uint maxPrimitives, const RC<Buffer> & indexBuffer, const string & ibField);

	// Returns indirect buffer, only this buffer must be used for indirect build.
	RC<Buffer>  IndirectBuffer ();

	// Returns number of meshes.
	uint  GeometryCount ();
};

struct RTInstanceCustomIndex
{
	RTInstanceCustomIndex ();
	RTInstanceCustomIndex (const RTInstanceCustomIndex&);
	RTInstanceCustomIndex&  operator = (const RTInstanceCustomIndex&);

	// Pass custom index to the shader, used first 24 bits.
	// Use 'gl.rayQuery.GetIntersectionInstanceCustomIndex()' or 'gl.InstanceCustomIndex' to get it in shader.
	RTInstanceCustomIndex (uint);
};

struct RTInstanceMask
{
	RTInstanceMask ();
	RTInstanceMask (const RTInstanceMask&);
	RTInstanceMask&  operator = (const RTInstanceMask&);

	// Set instance cull mask, used only first 8 bits.
	// In trace ray call if (cullMask_argument & instance_cullMask) != 0 then instance is visible.
	RTInstanceMask (uint);
};

struct RTInstanceSBTOffset
{
	RTInstanceSBTOffset ();
	RTInstanceSBTOffset (const RTInstanceSBTOffset&);
	RTInstanceSBTOffset&  operator = (const RTInstanceSBTOffset&);

	// Set shader binding table offset, used first 24 bits.
	// By default SBTOffset is calculated as 'instanceIndex * MaxRayTypes()'.
	RTInstanceSBTOffset (uint);
};

struct RTInstanceTransform
{
	RTInstanceTransform ();
	RTInstanceTransform (const RTInstanceTransform&);
	RTInstanceTransform&  operator = (const RTInstanceTransform&);
	RTInstanceTransform (const float3 & position, const float3 & angleInRadians);
	RTInstanceTransform (const float3 & position, const float3 & angleInRadians, float scale);
};

struct RTScene
{
	RTScene ();

	// Set resource name. It is used for debugging.
	void  Name (const string &);

	// Add instance to the scene.
	void  AddInstance (const RC<RTGeometry> &);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceMask &);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceSBTOffset &);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceCustomIndex &);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceCustomIndex &, const RTInstanceMask &);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &);
	void  AddInstance (const RC<RTGeometry> &, const float3 &);
	void  AddInstance (const RC<RTGeometry> &, const float3 &, const RTInstanceMask &);
	void  AddInstance (const RC<RTGeometry> &, const float3 &, const RTInstanceSBTOffset &);
	void  AddInstance (const RC<RTGeometry> &, const float3 &, const RTInstanceCustomIndex &);
	void  AddInstance (const RC<RTGeometry> &, const float3 &, const RTInstanceCustomIndex &, const RTInstanceMask &);
	void  AddInstance (const RC<RTGeometry> &, const float3 &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &);
	void  AddInstance (const RC<RTGeometry> &, const float3 &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceTransform &);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceTransform &, const RTInstanceMask &);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceTransform &, const RTInstanceSBTOffset &);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceTransform &, const RTInstanceCustomIndex &);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceTransform &, const RTInstanceCustomIndex &, const RTInstanceMask &);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceTransform &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceTransform &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &);
	void  AddInstance (const RC<RTGeometry> &, ERTInstanceOpt);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceMask &, ERTInstanceOpt);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceSBTOffset &, ERTInstanceOpt);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceCustomIndex &, ERTInstanceOpt);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceCustomIndex &, const RTInstanceMask &, ERTInstanceOpt);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &, ERTInstanceOpt);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &, ERTInstanceOpt);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceTransform &, ERTInstanceOpt);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceTransform &, const RTInstanceMask &, ERTInstanceOpt);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceTransform &, const RTInstanceSBTOffset &, ERTInstanceOpt);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceTransform &, const RTInstanceCustomIndex &, ERTInstanceOpt);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceTransform &, const RTInstanceCustomIndex &, const RTInstanceMask &, ERTInstanceOpt);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceTransform &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &, ERTInstanceOpt);
	void  AddInstance (const RC<RTGeometry> &, const RTInstanceTransform &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &, ERTInstanceOpt);

	// Returns instance buffer, can be used to update instances in compute shader.
	RC<Buffer>  InstanceBuffer ();

	// Returns number of instances.
	uint  InstanceCount ();

	// Returns indirect buffer, only this buffer must be used for indirect build.
	RC<Buffer>  IndirectBuffer ();

	// Set number of ray types. It is used to calculate SBTOffset for instances.
	void  MaxRayTypes (uint);
};

struct BaseController
{
};

struct ScaleBiasCamera
{
	ScaleBiasCamera ();

	// Returns dynamic dimension which is attached to the camera.
	// If not specified then render target size will be used from first pass where camera attached.
	RC<DynamicDim>  Dimension ();

	// Set dynamic dimension for camera.
	// If camera is used in single pass use default value.
	// If camera is used in multiple passes with different resolution then set explicit dimension.
	// When dimension changed camera with perspective projection will be resized for new aspect ratio.
	void  Dimension (const RC<DynamicDim> &);
};

struct TopDownCamera
{
	TopDownCamera ();

	// Set scale for forward and backward movement.
	void  ForwardBackwardScale (float);
	void  ForwardBackwardScale (float forward, float backward);

	// Set scale for side (left/right) movement.
	void  SideMovementScale (float);

	// Set rotation scale for mouse/touches/arrows.
	void  RotationScale (float);

	// Set initial position
	void  Position (const float2 &);

	// Returns dynamic dimension which is attached to the camera.
	// If not specified then render target size will be used from first pass where camera attached.
	RC<DynamicDim>  Dimension ();

	// Set dynamic dimension for camera.
	// If camera is used in single pass use default value.
	// If camera is used in multiple passes with different resolution then set explicit dimension.
	// When dimension changed camera with perspective projection will be resized for new aspect ratio.
	void  Dimension (const RC<DynamicDim> &);
};

struct OrbitalCamera
{
	OrbitalCamera ();

	// Returns dynamic dimension which is attached to the camera.
	// If not specified then render target size will be used from first pass where camera attached.
	RC<DynamicDim>  Dimension ();

	// Set dynamic dimension for camera.
	// If camera is used in single pass use default value.
	// If camera is used in multiple passes with different resolution then set explicit dimension.
	// When dimension changed camera with perspective projection will be resized for new aspect ratio.
	void  Dimension (const RC<DynamicDim> &);

	// Set field or view on Y axis in radians. On X axis it will be calculate automaticaly by aspect ratio.
	void  FovY (float);

	// Set near and far clip planes.
	void  ClipPlanes (float near, float far);

	// Set near clip plane for infinity projection.
	void  ClipPlanes (float near);
	void  ReverseZ (bool);

	// Set rotation scale for mouse/touches/arrows.
	void  RotationScale (float xy);
	void  RotationScale (float x, float y);
	void  OffsetScale (float);

	// Set initial position.
	void  Position (const float3 &);
	void  Offset (float);
};

struct FlightCamera
{
	FlightCamera ();

	// Set rotation scale for mouse/touches/arrows.
	void  RotationScale (float);
	void  RotationScale (float yaw, float pitch, float roll);
	void  EngineThrust (float min, float max);

	// Set field or view on Y axis in radians. On X axis it will be calculate automaticaly by aspect ratio.
	void  FovY (float);

	// Set near and far clip planes.
	void  ClipPlanes (float near, float far);

	// Set near clip plane for infinity projection.
	void  ClipPlanes (float near);
	void  ReverseZ (bool);

	// Set initial position.
	void  Position (const float3 &);

	// Returns dynamic dimension which is attached to the camera.
	// If not specified then render target size will be used from first pass where camera attached.
	RC<DynamicDim>  Dimension ();

	// Set dynamic dimension for camera.
	// If camera is used in single pass use default value.
	// If camera is used in multiple passes with different resolution then set explicit dimension.
	// When dimension changed camera with perspective projection will be resized for new aspect ratio.
	void  Dimension (const RC<DynamicDim> &);
};

struct FPSCamera
{
	FPSCamera ();

	// Set scale for forward and backward movement.
	void  ForwardBackwardScale (float);
	void  ForwardBackwardScale (float forward, float backward);

	// Set scale for up and down movement.
	void  UpDownScale (float);
	void  UpDownScale (float up, float down);

	// Set scale for side (left/right) movement.
	void  SideMovementScale (float);

	// Set rotation scale for mouse/touches/arrows.
	void  RotationScale (float xy);
	void  RotationScale (float x, float y);

	// Set field or view on Y axis in radians. On X axis it will be calculate automaticaly by aspect ratio.
	void  FovY (float);

	// Set near and far clip planes.
	void  ClipPlanes (float near, float far);

	// Set near clip plane for infinity projection.
	void  ClipPlanes (float near);
	void  ReverseZ (bool);

	// Set initial position.
	void  Position (const float3 &);

	// Returns dynamic dimension which is attached to the camera.
	// If not specified then render target size will be used from first pass where camera attached.
	RC<DynamicDim>  Dimension ();

	// Set dynamic dimension for camera.
	// If camera is used in single pass use default value.
	// If camera is used in multiple passes with different resolution then set explicit dimension.
	// When dimension changed camera with perspective projection will be resized for new aspect ratio.
	void  Dimension (const RC<DynamicDim> &);
};

struct RemoteCamera
{
	RemoteCamera ();

	// Set scale for forward and backward movement.
	void  ForwardBackwardScale (float);
	void  ForwardBackwardScale (float forward, float backward);

	// Set scale for up and down movement.
	void  UpDownScale (float);
	void  UpDownScale (float up, float down);

	// Set scale for side (left/right) movement.
	void  SideMovementScale (float);

	// Set field or view on Y axis in radians. On X axis it will be calculate automaticaly by aspect ratio.
	void  FovY (float);

	// Set near and far clip planes.
	void  ClipPlanes (float near, float far);

	// Set near clip plane for infinity projection.
	void  ClipPlanes (float near);
	void  ReverseZ (bool);

	// Set initial position.
	void  Position (const float3 &);

	// Returns dynamic dimension which is attached to the camera.
	// If not specified then render target size will be used from first pass where camera attached.
	RC<DynamicDim>  Dimension ();

	// Set dynamic dimension for camera.
	// If camera is used in single pass use default value.
	// If camera is used in multiple passes with different resolution then set explicit dimension.
	// When dimension changed camera with perspective projection will be resized for new aspect ratio.
	void  Dimension (const RC<DynamicDim> &);
};

struct FPVCamera
{
	FPVCamera ();

	// Set scale for forward and backward movement.
	void  ForwardBackwardScale (float);
	void  ForwardBackwardScale (float forward, float backward);

	// Set scale for up and down movement.
	void  UpDownScale (float);
	void  UpDownScale (float up, float down);

	// Set scale for side (left/right) movement.
	void  SideMovementScale (float);

	// Set rotation scale for mouse/touches/arrows.
	void  RotationScale (float xy);
	void  RotationScale (float x, float y);

	// Set field or view on Y axis in radians. On X axis it will be calculate automaticaly by aspect ratio.
	void  FovY (float);

	// Set near and far clip planes.
	void  ClipPlanes (float near, float far);

	// Set near clip plane for infinity projection.
	void  ClipPlanes (float near);
	void  ReverseZ (bool);

	// Set initial position.
	void  Position (const float3 &);

	// Returns dynamic dimension which is attached to the camera.
	// If not specified then render target size will be used from first pass where camera attached.
	RC<DynamicDim>  Dimension ();

	// Set dynamic dimension for camera.
	// If camera is used in single pass use default value.
	// If camera is used in multiple passes with different resolution then set explicit dimension.
	// When dimension changed camera with perspective projection will be resized for new aspect ratio.
	void  Dimension (const RC<DynamicDim> &);
};

struct IPass
{
};

struct GeomSource
{
};

struct SphericalCube
{
	SphericalCube ();

	// Add resource to all shaders in the current pass.
	// In - resource is used for read access.
	// Out - resource is used for write access.
	void  ArgIn (const string & uniformName, const RC<RTScene> & resource);
	void  ArgIn (const string & uniformName, const RC<Buffer> & resource);
	void  ArgOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgInOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource);
	void  ArgOut (const string & uniformName, const RC<Image> & resource);
	void  ArgInOut (const string & uniformName, const RC<Image> & resource);
	void  ArgIn (const string & uniformName, const array<Image@> & resources);
	void  ArgOut (const string & uniformName, const array<Image@> & resources);
	void  ArgInOut (const string & uniformName, const array<Image@> & resources);
	void  ArgTex (const string & uniformName, const RC<Image> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource, const string & samplerName);
	void  ArgTex (const string & uniformName, const array<Image@> & resources);
	void  ArgIn (const string & uniformName, const array<Image@> & resources, const string & samplerName);
	void  ArgIn (const string & uniformName, const RC<VideoImage> & resource, const string & samplerName);

	// Set detail level of the sphere.
	// Vertex count: (lod+2)^2, index count: 6*(lod+1)^2.
	void  DetailLevel (uint maxLOD);
	void  DetailLevel (uint minLOD, uint maxLOD);
};

struct UnifiedGeometry_Draw
{
	UnifiedGeometry_Draw ();
	UnifiedGeometry_Draw (const UnifiedGeometry_Draw&);
	UnifiedGeometry_Draw&  operator = (const UnifiedGeometry_Draw&);
	void  VertexCount (const RC<DynamicUInt> &);
	void  InstanceCount (const RC<DynamicUInt> &);

	// Pattern to choose pipeline if supported multiple pipelines.
	void  PipelineHint (const string &);
	uint vertexCount;
	uint instanceCount;
	uint firstVertex;
	uint firstInstance;
};

struct UnifiedGeometry_DrawIndexed
{
	UnifiedGeometry_DrawIndexed ();
	UnifiedGeometry_DrawIndexed (const UnifiedGeometry_DrawIndexed&);
	UnifiedGeometry_DrawIndexed&  operator = (const UnifiedGeometry_DrawIndexed&);
	void  IndexCount (const RC<DynamicUInt> &);
	void  InstanceCount (const RC<DynamicUInt> &);

	// Set buffer which contains array of 'ushort/uint' (2/4 bytes) indices, array size must be at least 'indexCount'.
	void  IndexBuffer (EIndex type, const RC<Buffer> & buffer);
	void  IndexBuffer (EIndex type, const RC<Buffer> & buffer, uint64 offset);
	void  IndexBuffer (const RC<Buffer> & buffer, const string & field);

	// Pattern to choose pipeline if supported multiple pipelines.
	void  PipelineHint (const string &);
	uint indexCount;
	uint instanceCount;
	uint firstIndex;
	int vertexOffset;
	uint firstInstance;
};

struct UnifiedGeometry_DrawIndirect
{
	UnifiedGeometry_DrawIndirect ();
	UnifiedGeometry_DrawIndirect (const UnifiedGeometry_DrawIndirect&);
	UnifiedGeometry_DrawIndirect&  operator = (const UnifiedGeometry_DrawIndirect&);
	void  DrawCount (const RC<DynamicUInt> &);

	// Set buffer which contains array of 'DrawIndirectCommand' (16 bytes) structs, array size must be at least 'drawCount'.
	void  IndirectBuffer (const RC<Buffer> & buffer);
	void  IndirectBuffer (const RC<Buffer> & buffer, uint64 offset);
	void  IndirectBuffer (const RC<Buffer> & buffer, const string & field);

	// Pattern to choose pipeline if supported multiple pipelines.
	void  PipelineHint (const string &);

	// Stride must be at least 16 bytes and multiple of 4.
	uint stride;
	uint drawCount;
};

struct UnifiedGeometry_DrawIndexedIndirect
{
	UnifiedGeometry_DrawIndexedIndirect ();
	UnifiedGeometry_DrawIndexedIndirect (const UnifiedGeometry_DrawIndexedIndirect&);
	UnifiedGeometry_DrawIndexedIndirect&  operator = (const UnifiedGeometry_DrawIndexedIndirect&);
	void  DrawCount (const RC<DynamicUInt> &);

	// Set buffer which contains array of 'ushort/uint' (2/4 bytes) indices, array size must be at least 'indexCount'.
	void  IndexBuffer (EIndex type, const RC<Buffer> & buffer);
	void  IndexBuffer (EIndex type, const RC<Buffer> & buffer, uint64 offset);
	void  IndexBuffer (const RC<Buffer> & buffer, const string & field);

	// Set buffer which contains array of 'DrawIndexedIndirectCommand' (20 bytes) structs, array size must be at least 'drawCount'.
	void  IndirectBuffer (const RC<Buffer> & buffer);
	void  IndirectBuffer (const RC<Buffer> & buffer, uint64 offset);
	void  IndirectBuffer (const RC<Buffer> & buffer, const string & field);

	// Pattern to choose pipeline if supported multiple pipelines.
	void  PipelineHint (const string &);

	// Stride must be at least 20 bytes and multiple of 4.
	uint stride;
	uint drawCount;
};

struct UnifiedGeometry_DrawMeshTasks
{
	UnifiedGeometry_DrawMeshTasks ();
	UnifiedGeometry_DrawMeshTasks (const UnifiedGeometry_DrawMeshTasks&);
	UnifiedGeometry_DrawMeshTasks&  operator = (const UnifiedGeometry_DrawMeshTasks&);
	void  TaskCount (const RC<DynamicUInt3> &);
	uint3 taskCount;
};

struct UnifiedGeometry_DrawMeshTasksIndirect
{
	UnifiedGeometry_DrawMeshTasksIndirect ();
	UnifiedGeometry_DrawMeshTasksIndirect (const UnifiedGeometry_DrawMeshTasksIndirect&);
	UnifiedGeometry_DrawMeshTasksIndirect&  operator = (const UnifiedGeometry_DrawMeshTasksIndirect&);
	void  DrawCount (const RC<DynamicUInt> &);

	// Set buffer which contains array of 'DrawMeshTasksIndirectCommand' (12 bytes) structs, array size must be at least 'drawCount'.
	void  IndirectBuffer (const RC<Buffer> & buffer);
	void  IndirectBuffer (const RC<Buffer> & buffer, uint64 offset);
	void  IndirectBuffer (const RC<Buffer> & buffer, const string & field);

	// Pattern to choose pipeline if supported multiple pipelines.
	void  PipelineHint (const string &);

	// Stride must be at least 12 bytes and multiple of 4.
	uint stride;
	uint drawCount;
};

struct UnifiedGeometry_DrawIndirectCount
{
	UnifiedGeometry_DrawIndirectCount ();
	UnifiedGeometry_DrawIndirectCount (const UnifiedGeometry_DrawIndirectCount&);
	UnifiedGeometry_DrawIndirectCount&  operator = (const UnifiedGeometry_DrawIndirectCount&);
	void  MaxDrawCount (const RC<DynamicUInt> &);

	// Set buffer which contains array of 'DrawIndirectCommand' (16 bytes) structs, array size must be at least 'maxDrawCount'.
	void  IndirectBuffer (const RC<Buffer> & buffer);
	void  IndirectBuffer (const RC<Buffer> & buffer, uint64 offset);
	void  IndirectBuffer (const RC<Buffer> & buffer, const string & field);

	// Set buffer which contains single 'uint' (4 bytes) value.
	void  CountBuffer (const RC<Buffer> & buffer);
	void  CountBuffer (const RC<Buffer> & buffer, uint64 offset);
	void  CountBuffer (const RC<Buffer> & buffer, const string & field);

	// Pattern to choose pipeline if supported multiple pipelines.
	void  PipelineHint (const string &);

	// Stride must be at least 16 bytes and multiple of 4.
	uint stride;
	uint maxDrawCount;
};

struct UnifiedGeometry_DrawIndexedIndirectCount
{
	UnifiedGeometry_DrawIndexedIndirectCount ();
	UnifiedGeometry_DrawIndexedIndirectCount (const UnifiedGeometry_DrawIndexedIndirectCount&);
	UnifiedGeometry_DrawIndexedIndirectCount&  operator = (const UnifiedGeometry_DrawIndexedIndirectCount&);
	void  MaxDrawCount (const RC<DynamicUInt> &);

	// Set buffer which contains array of 'ushort/uint' (2/4 bytes) indices, array size must be at least 'indexCount'.
	void  IndexBuffer (EIndex type, const RC<Buffer> & buffer);
	void  IndexBuffer (EIndex type, const RC<Buffer> & buffer, uint64 offset);
	void  IndexBuffer (const RC<Buffer> & buffer, const string & field);

	// Set buffer which contains array of 'DrawIndexedIndirectCommand' (20 bytes) structs, array size must be at least 'maxDrawCount'.
	void  IndirectBuffer (const RC<Buffer> & buffer);
	void  IndirectBuffer (const RC<Buffer> & buffer, uint64 offset);
	void  IndirectBuffer (const RC<Buffer> & buffer, const string & field);

	// Set buffer which contains single 'uint' (4 bytes) value.
	void  CountBuffer (const RC<Buffer> & buffer);
	void  CountBuffer (const RC<Buffer> & buffer, uint64 offset);
	void  CountBuffer (const RC<Buffer> & buffer, const string & field);

	// Pattern to choose pipeline if supported multiple pipelines.
	void  PipelineHint (const string &);

	// Stride must be at least 20 bytes and multiple of 4.
	uint stride;
	uint maxDrawCount;
};

struct UnifiedGeometry_DrawMeshTasksIndirectCount
{
	UnifiedGeometry_DrawMeshTasksIndirectCount ();
	UnifiedGeometry_DrawMeshTasksIndirectCount (const UnifiedGeometry_DrawMeshTasksIndirectCount&);
	UnifiedGeometry_DrawMeshTasksIndirectCount&  operator = (const UnifiedGeometry_DrawMeshTasksIndirectCount&);
	void  MaxDrawCount (const RC<DynamicUInt> &);

	// Set buffer which contains array of 'DrawMeshTasksIndirectCommand' (12 bytes) structs, array size must be at least 'maxDrawCount'.
	void  IndirectBuffer (const RC<Buffer> & buffer);
	void  IndirectBuffer (const RC<Buffer> & buffer, uint64 offset);
	void  IndirectBuffer (const RC<Buffer> & buffer, const string & field);

	// Set buffer which contains single 'uint' (4 bytes) value.
	void  CountBuffer (const RC<Buffer> & buffer);
	void  CountBuffer (const RC<Buffer> & buffer, uint64 offset);
	void  CountBuffer (const RC<Buffer> & buffer, const string & field);

	// Pattern to choose pipeline if supported multiple pipelines.
	void  PipelineHint (const string &);

	// Stride must be at least 12 bytes and multiple of 4.
	uint stride;
	uint maxDrawCount;
};

struct UnifiedGeometry
{
	UnifiedGeometry ();

	// Add resource to all shaders in the current pass.
	// In - resource is used for read access.
	// Out - resource is used for write access.
	void  ArgIn (const string & uniformName, const RC<RTScene> & resource);
	void  ArgIn (const string & uniformName, const RC<Buffer> & resource);
	void  ArgOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgInOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource);
	void  ArgOut (const string & uniformName, const RC<Image> & resource);
	void  ArgInOut (const string & uniformName, const RC<Image> & resource);
	void  ArgIn (const string & uniformName, const array<Image@> & resources);
	void  ArgOut (const string & uniformName, const array<Image@> & resources);
	void  ArgInOut (const string & uniformName, const array<Image@> & resources);
	void  ArgTex (const string & uniformName, const RC<Image> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource, const string & samplerName);
	void  ArgTex (const string & uniformName, const array<Image@> & resources);
	void  ArgIn (const string & uniformName, const array<Image@> & resources, const string & samplerName);
	void  ArgIn (const string & uniformName, const RC<VideoImage> & resource, const string & samplerName);
	void  Draw (const UnifiedGeometry_Draw &);
	void  Draw (const UnifiedGeometry_DrawIndexed &);
	void  Draw (const UnifiedGeometry_DrawIndirect &);
	void  Draw (const UnifiedGeometry_DrawIndexedIndirect &);
	void  Draw (const UnifiedGeometry_DrawMeshTasks &);
	void  Draw (const UnifiedGeometry_DrawMeshTasksIndirect &);
	void  Draw (const UnifiedGeometry_DrawIndirectCount &);
	void  Draw (const UnifiedGeometry_DrawIndexedIndirectCount &);
	void  Draw (const UnifiedGeometry_DrawMeshTasksIndirectCount &);
	RC<UnifiedGeometry>  Clone () const;
};

struct Model
{

	// Add resource to all shaders in the current pass.
	// In - resource is used for read access.
	// Out - resource is used for write access.
	void  ArgIn (const string & uniformName, const RC<RTScene> & resource);
	void  ArgIn (const string & uniformName, const RC<Buffer> & resource);
	void  ArgOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgInOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource);
	void  ArgOut (const string & uniformName, const RC<Image> & resource);
	void  ArgInOut (const string & uniformName, const RC<Image> & resource);
	void  ArgIn (const string & uniformName, const array<Image@> & resources);
	void  ArgOut (const string & uniformName, const array<Image@> & resources);
	void  ArgInOut (const string & uniformName, const array<Image@> & resources);
	void  ArgTex (const string & uniformName, const RC<Image> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource, const string & samplerName);
	void  ArgTex (const string & uniformName, const array<Image@> & resources);
	void  ArgIn (const string & uniformName, const array<Image@> & resources, const string & samplerName);
	void  ArgIn (const string & uniformName, const RC<VideoImage> & resource, const string & samplerName);
	Model (const string & scenePathInVFS);

	// Set resource name. It is used for debugging.
	void  Name (const string &);

	// Add directory where to search required textures.
	void  TextureSearchDir (const string & folder);

	// Set transformation for model root node.
	void  InitialTransform (const float4x4 &);
	void  InitialTransform (const float3 & position, const float3 & rotation, float scale);
	void  AddOmniLight (const float3 & position, const float3 & attenuation, const RGBA32f & color);
	void  AddConeLight (const float3 & position, const float3 & direction, const float3 & coneAngle, const float2 & attenuation, const RGBA32f & color);
	void  AddDirLight (const float3 & direction, const float3 & attenuation, const RGBA32f & color);
};

struct Collection
{
	Collection ();

	// Add item to collection.
	void  Add (const string & key, const RC<DynamicDim> & value);
	void  Add (const string & key, const RC<DynamicInt4> & value);
	void  Add (const string & key, const RC<DynamicFloat4> & value);
	void  Add (const string & key, const RC<Image> & value);
	void  Add (const string & key, const RC<VideoImage> & value);
	void  Add (const string & key, const RC<Buffer> & value);
	void  Add (const string & key, const RC<GeomSource> & value);
	void  Add (const string & key, const RC<BaseController> & value);
	void  Add (const string & key, const RC<RTGeometry> & value);
	void  Add (const string & key, const RC<RTScene> & value);

	// Returns dynamic values.
	RC<DynamicDim>  DynDim (const string & key) const;
	RC<DynamicInt4>  DynInt4 (const string & key) const;
	RC<DynamicFloat4>  DynFloat4 (const string & key) const;

	// Returns resources.
	RC<Image>  Image (const string & key) const;
	RC<VideoImage>  VideoImage (const string & key) const;
	RC<Buffer>  Buffer (const string & key) const;
	RC<GeomSource>  Geometry (const string & key) const;
	RC<BaseController>  Controller (const string & key) const;
	RC<RTGeometry>  RTGeometry (const string & key) const;
	RC<RTScene>  RTScene (const string & key) const;
};

struct EPostprocess
{
	EPostprocess () {}
	EPostprocess (uint32) {}
	operator uint32 () const;

	// Entry point: 'Main'
	static constexpr uint32 None = 0;

	// Entry point: 'void mainImage (out float4 fragColor, in float2 fragCoord)'
	static constexpr uint32 Shadertoy = 1;

	// Entry point: 'void mainVR (out float4 fragColor, in float2 fragCoord, in float3 fragRayOri, in float3 fragRayDir)'
	static constexpr uint32 ShadertoyVR = 2;
	static constexpr uint32 ShadertoyVR_180 = 3;
	static constexpr uint32 ShadertoyVR_360 = 4;
	static constexpr uint32 Shadertoy_360 = 5;
};

struct EPassFlags
{
	EPassFlags () {}
	EPassFlags (uint8) {}
	operator uint8 () const;
	static constexpr uint8 None = 0;

	// ShaderTrace - record all variables, function result, etc and save it to file.
	// It is very useful to debug shaders. In UI select 'Debugging' menu, select pass,'Trace' and shader stage then click 'G' key to record trace for pixel under cursor.
	// Reference to the last recorded trace will be added to console and IDE log, click on it to open file.
	static constexpr uint8 Enable_ShaderTrace = 1;

	// ShaderFunctionProfiling - record time of user function calls, sort it and save to file.
	static constexpr uint8 Enable_ShaderFnProf = 2;

	// Enable all debug features.
	static constexpr uint8 Enable_AllShaderDbg = 7;
};

struct Postprocess
{

	// Set debug label and color. It is used in graphics profiler.
	void  SetDebugLabel (const string & label);
	void  SetDebugLabel (const string & label, const RGBA8u & color);
	void  AddFlag (EPassFlags);

	// Add slider to UI. Data passed to all shaders in the current pass.
	void  SliderI (const string & name);
	void  Slider (const string & name, int min, int max);
	void  Slider (const string & name, const int2 & min, const int2 & max);
	void  Slider (const string & name, const int3 & min, const int3 & max);
	void  Slider (const string & name, const int4 & min, const int4 & max);
	void  Slider (const string & name, int min, int max, int initial);
	void  Slider (const string & name, const int2 & min, const int2 & max, const int2 & initial);
	void  Slider (const string & name, const int3 & min, const int3 & max, const int3 & initial);
	void  Slider (const string & name, const int4 & min, const int4 & max, const int4 & initial);
	void  SliderF (const string & name);
	void  Slider (const string & name, float min, float max);
	void  Slider (const string & name, const float2 & min, const float2 & max);
	void  Slider (const string & name, const float3 & min, const float3 & max);
	void  Slider (const string & name, const float4 & min, const float4 & max);
	void  Slider (const string & name, float min, float max, float initial);
	void  Slider (const string & name, const float2 & min, const float2 & max, const float2 & initial);
	void  Slider (const string & name, const float3 & min, const float3 & max, const float3 & initial);
	void  Slider (const string & name, const float4 & min, const float4 & max, const float4 & initial);
	void  ColorSelector (const string & name);
	void  ColorSelector (const string & name, const RGBA32f & initial);
	void  ColorSelector (const string & name, const RGBA8u & initial);

	// TODO
	void  Constant (const string & name, const RC<DynamicFloat> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat4> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt4> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt4> & dynamicValue);

	// Returns dynamic dimension of the pass.
	// It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches.
	RC<DynamicDim>  Dimension ();
	void  EnableIfEqual (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfLess (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfGreater (const RC<DynamicUInt> & dynamic, uint refValue);

	// Add resource to all shaders in the current pass.
	// In  - resource is used for read access.
	// Out - resource is used for write access.
	void  ArgIn (const string & uniformName, const RC<RTScene> & resource);
	void  ArgIn (const string & uniformName, const RC<Buffer> & resource);
	void  ArgOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgInOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource);
	void  ArgOut (const string & uniformName, const RC<Image> & resource);
	void  ArgInOut (const string & uniformName, const RC<Image> & resource);
	void  ArgTex (const string & uniformName, const RC<Image> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource, const string & samplerName);
	void  ArgTex (const string & uniformName, const array<Image@> & resources);
	void  ArgIn (const string & uniformName, const array<Image@> & resources, const string & samplerName);
	void  ArgIn (const string & uniformName, const RC<VideoImage> & resource, const string & samplerName);
	void  Set (const RC<BaseController> & camera);
	void  ArgIn (const string & uniformName, const array<Image@> & resources);
	void  ArgOut (const string & uniformName, const array<Image@> & resources);
	void  ArgInOut (const string & uniformName, const array<Image@> & resources);

	// Add color/depth render target.
	// Implicitly name will be 'out_Color' + index.
	void  Output (const RC<Image> &);
	void  Output (const RC<Image> & image, const MipmapLevel & mipmap);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap);
	void  Output (const RC<Image> & image, const RGBA32f & clearColor);
	void  Output (const RC<Image> & image, const MipmapLevel & mipmap, const RGBA32f & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const RGBA32f & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const RGBA32f & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const RGBA32f & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const RGBA32f & clearColor);
	void  Output (const RC<Image> &, const RGBA32u &);
	void  Output (const RC<Image> &, const MipmapLevel &, const RGBA32u &);
	void  Output (const RC<Image> &, const ImageLayer &, const RGBA32u &);
	void  Output (const RC<Image> &, const ImageLayer &, const MipmapLevel &, const RGBA32u &);
	void  Output (const RC<Image> &, const ImageLayer &, uint, const RGBA32u &);
	void  Output (const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32u &);
	void  Output (const RC<Image> &, const RGBA32i &);
	void  Output (const RC<Image> &, const MipmapLevel &, const RGBA32i &);
	void  Output (const RC<Image> &, const ImageLayer &, const RGBA32i &);
	void  Output (const RC<Image> &, const ImageLayer &, const MipmapLevel &, const RGBA32i &);
	void  Output (const RC<Image> &, const ImageLayer &, uint, const RGBA32i &);
	void  Output (const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32i &);
	void  Output (const RC<Image> &, const DepthStencil &);
	void  Output (const RC<Image> &, const MipmapLevel &, const DepthStencil &);
	void  Output (const RC<Image> &, const ImageLayer &, const DepthStencil &);
	void  Output (const RC<Image> &, const ImageLayer &, const MipmapLevel &, const DepthStencil &);
	void  Output (const RC<Image> &, const ImageLayer &, uint, const DepthStencil &);
	void  Output (const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, const DepthStencil &);

	// Add color/depth render target with explicit name.
	void  Output (const string &, const RC<Image> &);
	void  Output (const string &, const RC<Image> &, const MipmapLevel &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const MipmapLevel &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &);
	void  Output (const string &, const RC<Image> &, const RGBA32f &);
	void  Output (const string &, const RC<Image> &, const MipmapLevel &, const RGBA32f &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const RGBA32f &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const MipmapLevel &, const RGBA32f &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const RGBA32f &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32f &);
	void  Output (const string &, const RC<Image> &, const RGBA32u &);
	void  Output (const string &, const RC<Image> &, const MipmapLevel &, const RGBA32u &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const RGBA32u &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const MipmapLevel &, const RGBA32u &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const RGBA32u &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32u &);
	void  Output (const string &, const RC<Image> &, const RGBA32i &);
	void  Output (const string &, const RC<Image> &, const MipmapLevel &, const RGBA32i &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const RGBA32i &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const MipmapLevel &, const RGBA32i &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const RGBA32i &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32i &);
	void  Output (const string &, const RC<Image> &, const DepthStencil &);
	void  Output (const string &, const RC<Image> &, const MipmapLevel &, const DepthStencil &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const DepthStencil &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const MipmapLevel &, const DepthStencil &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const DepthStencil &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, const DepthStencil &);

	// Add color render target with blend operation.
	// Implicitly name will be 'out_Color' + index.
	void  OutputBlend (const RC<Image> &, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const RC<Image> &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const RC<Image> &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const RC<Image> &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const RC<Image> &, const ImageLayer &, uint, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const RC<Image> &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const RC<Image> &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const RC<Image> &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const RC<Image> &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const RC<Image> &, const ImageLayer &, uint, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp);

	// Add color render target with blend operation and with explicit name.
	void  OutputBlend (const string &, const RC<Image> &, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const string &, const RC<Image> &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const string &, const RC<Image> &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const string &, const RC<Image> &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const string &, const RC<Image> &, const ImageLayer &, uint, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const string &, const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const string &, const RC<Image> &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const string &, const RC<Image> &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const string &, const RC<Image> &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const string &, const RC<Image> &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const string &, const RC<Image> &, const ImageLayer &, uint, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp);
	void  OutputBlend (const string &, const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp);
	void  DepthRange (float min, float max);

	// Set path to fragment shader, empty - load current file.
	Postprocess  ();
	Postprocess (const string & shaderPath);
	Postprocess (const string & shaderPath, EPostprocess postprocessFlags);
	Postprocess (EPostprocess postprocessFlags);
	Postprocess (EPostprocess postprocessFlags, const string & defines);
	Postprocess (const string & shaderPath, const string & defines);
};

struct ComputePass
{

	// Set debug label and color. It is used in graphics profiler.
	void  SetDebugLabel (const string & label);
	void  SetDebugLabel (const string & label, const RGBA8u & color);
	void  AddFlag (EPassFlags);

	// Add slider to UI. Data passed to all shaders in the current pass.
	void  SliderI (const string & name);
	void  Slider (const string & name, int min, int max);
	void  Slider (const string & name, const int2 & min, const int2 & max);
	void  Slider (const string & name, const int3 & min, const int3 & max);
	void  Slider (const string & name, const int4 & min, const int4 & max);
	void  Slider (const string & name, int min, int max, int initial);
	void  Slider (const string & name, const int2 & min, const int2 & max, const int2 & initial);
	void  Slider (const string & name, const int3 & min, const int3 & max, const int3 & initial);
	void  Slider (const string & name, const int4 & min, const int4 & max, const int4 & initial);
	void  SliderF (const string & name);
	void  Slider (const string & name, float min, float max);
	void  Slider (const string & name, const float2 & min, const float2 & max);
	void  Slider (const string & name, const float3 & min, const float3 & max);
	void  Slider (const string & name, const float4 & min, const float4 & max);
	void  Slider (const string & name, float min, float max, float initial);
	void  Slider (const string & name, const float2 & min, const float2 & max, const float2 & initial);
	void  Slider (const string & name, const float3 & min, const float3 & max, const float3 & initial);
	void  Slider (const string & name, const float4 & min, const float4 & max, const float4 & initial);
	void  ColorSelector (const string & name);
	void  ColorSelector (const string & name, const RGBA32f & initial);
	void  ColorSelector (const string & name, const RGBA8u & initial);

	// TODO
	void  Constant (const string & name, const RC<DynamicFloat> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat4> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt4> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt4> & dynamicValue);

	// Returns dynamic dimension of the pass.
	// It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches.
	RC<DynamicDim>  Dimension ();
	void  EnableIfEqual (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfLess (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfGreater (const RC<DynamicUInt> & dynamic, uint refValue);

	// Add resource to all shaders in the current pass.
	// In  - resource is used for read access.
	// Out - resource is used for write access.
	void  ArgIn (const string & uniformName, const RC<RTScene> & resource);
	void  ArgIn (const string & uniformName, const RC<Buffer> & resource);
	void  ArgOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgInOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource);
	void  ArgOut (const string & uniformName, const RC<Image> & resource);
	void  ArgInOut (const string & uniformName, const RC<Image> & resource);
	void  ArgTex (const string & uniformName, const RC<Image> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource, const string & samplerName);
	void  ArgTex (const string & uniformName, const array<Image@> & resources);
	void  ArgIn (const string & uniformName, const array<Image@> & resources, const string & samplerName);
	void  ArgIn (const string & uniformName, const RC<VideoImage> & resource, const string & samplerName);
	void  Set (const RC<BaseController> & camera);
	void  ArgIn (const string & uniformName, const array<Image@> & resources);
	void  ArgOut (const string & uniformName, const array<Image@> & resources);
	void  ArgInOut (const string & uniformName, const array<Image@> & resources);
	ComputePass  ();
	ComputePass (const string & shaderPath);
	ComputePass (const string & shaderPath, const string & defines);

	// Set workgroup size - number of threads which can access shared memory.
	void  LocalSize (uint x);
	void  LocalSize (uint x, uint y);
	void  LocalSize (uint x, uint y, uint z);
	void  LocalSize (const uint2 &);
	void  LocalSize (const uint3 &);

	// Execute compute shader with number of the workgroups.
	// Total number of threads is 'groupCount * localSize'.
	void  DispatchGroups (uint groupCountX);
	void  DispatchGroups (uint groupCountX, uint groupCountY);
	void  DispatchGroups (uint groupCountX, uint groupCountY, uint groupCountZ);
	void  DispatchGroups (const uint2 & groupCount);
	void  DispatchGroups (const uint3 & groupCount);
	void  DispatchGroups (const RC<DynamicDim> & dynamicGroupCount);
	void  DispatchGroups (const RC<DynamicUInt> & dynamicGroupCount);
	void  DispatchGroups (const RC<DynamicUInt2> & dynamicGroupCount);
	void  DispatchGroups (const RC<DynamicUInt3> & dynamicGroupCount);

	// Execute compute shader with total number of the threads.
	void  DispatchThreads (uint threadsX);
	void  DispatchThreads (uint threadsX, uint threadsY);
	void  DispatchThreads (uint threadsX, uint threadsY, uint threadsZ);
	void  DispatchThreads (const uint2 & threads);
	void  DispatchThreads (const uint3 & threads);
	void  DispatchThreads (const RC<DynamicDim> & dynamicThreadCount);
	void  DispatchThreads (const RC<DynamicUInt> & dynamicThreadCount);
	void  DispatchThreads (const RC<DynamicUInt2> & dynamicThreadCount);
	void  DispatchThreads (const RC<DynamicUInt3> & dynamicThreadCount);

	// Execute compute shader with indirect command.
	// Indirect buffer must contains 'DispatchIndirectCommand' data.
	void  DispatchGroupsIndirect (const RC<Buffer> & indirectBuffer);
	void  DispatchGroupsIndirect (const RC<Buffer> & indirectBuffer, uint64 indirectBufferOffset);
	void  DispatchGroupsIndirect (const RC<Buffer> & indirectBuffer, const string & indirectBufferField);
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

struct RTShader
{
	RTShader ();
	RTShader (const RTShader&);
	RTShader&  operator = (const RTShader&);
	RTShader (const string & filename);
	RTShader (const string & filename, const string & defines);
};

struct RayTracingPass
{

	// Set debug label and color. It is used in graphics profiler.
	void  SetDebugLabel (const string & label);
	void  SetDebugLabel (const string & label, const RGBA8u & color);
	void  AddFlag (EPassFlags);

	// Add slider to UI. Data passed to all shaders in the current pass.
	void  SliderI (const string & name);
	void  Slider (const string & name, int min, int max);
	void  Slider (const string & name, const int2 & min, const int2 & max);
	void  Slider (const string & name, const int3 & min, const int3 & max);
	void  Slider (const string & name, const int4 & min, const int4 & max);
	void  Slider (const string & name, int min, int max, int initial);
	void  Slider (const string & name, const int2 & min, const int2 & max, const int2 & initial);
	void  Slider (const string & name, const int3 & min, const int3 & max, const int3 & initial);
	void  Slider (const string & name, const int4 & min, const int4 & max, const int4 & initial);
	void  SliderF (const string & name);
	void  Slider (const string & name, float min, float max);
	void  Slider (const string & name, const float2 & min, const float2 & max);
	void  Slider (const string & name, const float3 & min, const float3 & max);
	void  Slider (const string & name, const float4 & min, const float4 & max);
	void  Slider (const string & name, float min, float max, float initial);
	void  Slider (const string & name, const float2 & min, const float2 & max, const float2 & initial);
	void  Slider (const string & name, const float3 & min, const float3 & max, const float3 & initial);
	void  Slider (const string & name, const float4 & min, const float4 & max, const float4 & initial);
	void  ColorSelector (const string & name);
	void  ColorSelector (const string & name, const RGBA32f & initial);
	void  ColorSelector (const string & name, const RGBA8u & initial);

	// TODO
	void  Constant (const string & name, const RC<DynamicFloat> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat4> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt4> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt4> & dynamicValue);

	// Returns dynamic dimension of the pass.
	// It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches.
	RC<DynamicDim>  Dimension ();
	void  EnableIfEqual (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfLess (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfGreater (const RC<DynamicUInt> & dynamic, uint refValue);

	// Add resource to all shaders in the current pass.
	// In  - resource is used for read access.
	// Out - resource is used for write access.
	void  ArgIn (const string & uniformName, const RC<RTScene> & resource);
	void  ArgIn (const string & uniformName, const RC<Buffer> & resource);
	void  ArgOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgInOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource);
	void  ArgOut (const string & uniformName, const RC<Image> & resource);
	void  ArgInOut (const string & uniformName, const RC<Image> & resource);
	void  ArgTex (const string & uniformName, const RC<Image> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource, const string & samplerName);
	void  ArgTex (const string & uniformName, const array<Image@> & resources);
	void  ArgIn (const string & uniformName, const array<Image@> & resources, const string & samplerName);
	void  ArgIn (const string & uniformName, const RC<VideoImage> & resource, const string & samplerName);
	void  Set (const RC<BaseController> & camera);
	void  ArgIn (const string & uniformName, const array<Image@> & resources);
	void  ArgOut (const string & uniformName, const array<Image@> & resources);
	void  ArgInOut (const string & uniformName, const array<Image@> & resources);
	RayTracingPass  ();
	RayTracingPass (const string & defines);

	// Run RayGen shader with specified number of threads.
	void  Dispatch (uint threadsX);
	void  Dispatch (uint threadsX, uint threadsY);
	void  Dispatch (uint threadsX, uint threadsY, uint threadsZ);
	void  Dispatch (const uint2 & threads);
	void  Dispatch (const uint3 & threads);
	void  Dispatch (const RC<DynamicDim> & dynamicThreadCount);
	void  Dispatch (const RC<DynamicUInt> & dynamicThreadCount);

	// Run RayGen shader with number of threads from indirect command.
	void  DispatchIndirect (const RC<Buffer> & indirectBuffer);
	void  DispatchIndirect (const RC<Buffer> & indirectBuffer, uint64 indirectBufferOffset);
	void  DispatchIndirect (const RC<Buffer> & indirectBuffer, const string & indirectBufferFieldName);
	void  RayGen (const RTShader &);
	void  MaxRayTypes (uint);
	void  RayMiss (const RayIndex & missIndex, const RTShader & missShader);
	void  Callable (const CallableIndex & callableIndex, const RTShader & callableShader);
	void  TriangleHit (const RayIndex & rayIndex, const InstanceIndex & instanceIndex, const RTShader & closestHit);
	void  TriangleHit (const RayIndex & rayIndex, const InstanceIndex & instanceIndex, const RTShader & closestHit, const RTShader & anyHit);
	void  ProceduralHit (const RayIndex & rayIndex, const InstanceIndex & instanceIndex, const RTShader & intersection, const RTShader & closestHit);
	void  ProceduralHit (const RayIndex & rayIndex, const InstanceIndex & instanceIndex, const RTShader & intersection, const RTShader & closestHit, const RTShader & anyHit);
	void  MaxRayRecursion (uint);
	void  MaxRayRecursion (const RC<DynamicUInt> &);
	void  MaxCallableRecursion (uint);
	void  MaxCallableRecursion (const RC<DynamicUInt> &);
};

struct ERenderLayer
{
	ERenderLayer () {}
	ERenderLayer (uint32) {}
	operator uint32 () const;
	static constexpr uint32 Opaque = 0;
	static constexpr uint32 Translucent = 1;
	static constexpr uint32 PostProcess = 2;
};

struct SceneGraphicsPass
{

	// Set debug label and color. It is used in graphics profiler.
	void  SetDebugLabel (const string & label);
	void  SetDebugLabel (const string & label, const RGBA8u & color);
	void  AddFlag (EPassFlags);

	// Add slider to UI. Data passed to all shaders in the current pass.
	void  SliderI (const string & name);
	void  Slider (const string & name, int min, int max);
	void  Slider (const string & name, const int2 & min, const int2 & max);
	void  Slider (const string & name, const int3 & min, const int3 & max);
	void  Slider (const string & name, const int4 & min, const int4 & max);
	void  Slider (const string & name, int min, int max, int initial);
	void  Slider (const string & name, const int2 & min, const int2 & max, const int2 & initial);
	void  Slider (const string & name, const int3 & min, const int3 & max, const int3 & initial);
	void  Slider (const string & name, const int4 & min, const int4 & max, const int4 & initial);
	void  SliderF (const string & name);
	void  Slider (const string & name, float min, float max);
	void  Slider (const string & name, const float2 & min, const float2 & max);
	void  Slider (const string & name, const float3 & min, const float3 & max);
	void  Slider (const string & name, const float4 & min, const float4 & max);
	void  Slider (const string & name, float min, float max, float initial);
	void  Slider (const string & name, const float2 & min, const float2 & max, const float2 & initial);
	void  Slider (const string & name, const float3 & min, const float3 & max, const float3 & initial);
	void  Slider (const string & name, const float4 & min, const float4 & max, const float4 & initial);
	void  ColorSelector (const string & name);
	void  ColorSelector (const string & name, const RGBA32f & initial);
	void  ColorSelector (const string & name, const RGBA8u & initial);

	// TODO
	void  Constant (const string & name, const RC<DynamicFloat> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat4> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt4> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt4> & dynamicValue);

	// Returns dynamic dimension of the pass.
	// It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches.
	RC<DynamicDim>  Dimension ();
	void  EnableIfEqual (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfLess (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfGreater (const RC<DynamicUInt> & dynamic, uint refValue);

	// Add resource to all shaders in the current pass.
	// In  - resource is used for read access.
	// Out - resource is used for write access.
	void  ArgIn (const string & uniformName, const RC<RTScene> & resource);
	void  ArgIn (const string & uniformName, const RC<Buffer> & resource);
	void  ArgOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgInOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource);
	void  ArgOut (const string & uniformName, const RC<Image> & resource);
	void  ArgInOut (const string & uniformName, const RC<Image> & resource);
	void  ArgTex (const string & uniformName, const RC<Image> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource, const string & samplerName);
	void  ArgTex (const string & uniformName, const array<Image@> & resources);
	void  ArgIn (const string & uniformName, const array<Image@> & resources, const string & samplerName);
	void  ArgIn (const string & uniformName, const RC<VideoImage> & resource, const string & samplerName);
	void  Set (const RC<BaseController> & camera);
	void  ArgIn (const string & uniformName, const array<Image@> & resources);
	void  ArgOut (const string & uniformName, const array<Image@> & resources);
	void  ArgInOut (const string & uniformName, const array<Image@> & resources);

	// Add color/depth render target.
	// Implicitly name will be 'out_Color' + index.
	void  Output (const RC<Image> &);
	void  Output (const RC<Image> & image, const MipmapLevel & mipmap);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap);
	void  Output (const RC<Image> & image, const RGBA32f & clearColor);
	void  Output (const RC<Image> & image, const MipmapLevel & mipmap, const RGBA32f & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const RGBA32f & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const RGBA32f & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const RGBA32f & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const RGBA32f & clearColor);
	void  Output (const RC<Image> &, const RGBA32u &);
	void  Output (const RC<Image> &, const MipmapLevel &, const RGBA32u &);
	void  Output (const RC<Image> &, const ImageLayer &, const RGBA32u &);
	void  Output (const RC<Image> &, const ImageLayer &, const MipmapLevel &, const RGBA32u &);
	void  Output (const RC<Image> &, const ImageLayer &, uint, const RGBA32u &);
	void  Output (const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32u &);
	void  Output (const RC<Image> &, const RGBA32i &);
	void  Output (const RC<Image> &, const MipmapLevel &, const RGBA32i &);
	void  Output (const RC<Image> &, const ImageLayer &, const RGBA32i &);
	void  Output (const RC<Image> &, const ImageLayer &, const MipmapLevel &, const RGBA32i &);
	void  Output (const RC<Image> &, const ImageLayer &, uint, const RGBA32i &);
	void  Output (const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32i &);
	void  Output (const RC<Image> &, const DepthStencil &);
	void  Output (const RC<Image> &, const MipmapLevel &, const DepthStencil &);
	void  Output (const RC<Image> &, const ImageLayer &, const DepthStencil &);
	void  Output (const RC<Image> &, const ImageLayer &, const MipmapLevel &, const DepthStencil &);
	void  Output (const RC<Image> &, const ImageLayer &, uint, const DepthStencil &);
	void  Output (const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, const DepthStencil &);

	// Add color/depth render target with explicit name.
	void  Output (const string &, const RC<Image> &);
	void  Output (const string &, const RC<Image> &, const MipmapLevel &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const MipmapLevel &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &);
	void  Output (const string &, const RC<Image> &, const RGBA32f &);
	void  Output (const string &, const RC<Image> &, const MipmapLevel &, const RGBA32f &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const RGBA32f &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const MipmapLevel &, const RGBA32f &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const RGBA32f &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32f &);
	void  Output (const string &, const RC<Image> &, const RGBA32u &);
	void  Output (const string &, const RC<Image> &, const MipmapLevel &, const RGBA32u &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const RGBA32u &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const MipmapLevel &, const RGBA32u &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const RGBA32u &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32u &);
	void  Output (const string &, const RC<Image> &, const RGBA32i &);
	void  Output (const string &, const RC<Image> &, const MipmapLevel &, const RGBA32i &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const RGBA32i &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const MipmapLevel &, const RGBA32i &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const RGBA32i &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32i &);
	void  Output (const string &, const RC<Image> &, const DepthStencil &);
	void  Output (const string &, const RC<Image> &, const MipmapLevel &, const DepthStencil &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const DepthStencil &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, const MipmapLevel &, const DepthStencil &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const DepthStencil &);
	void  Output (const string &, const RC<Image> &, const ImageLayer &, uint, const MipmapLevel &, const DepthStencil &);
	void  DepthRange (float min, float max);

	// Add path to single pipeline or folder with pipelines.
	// Scene geometry will be linked with compatible pipeline or error will be generated.
	void  AddPipeline (const string & pplnFile);
	void  AddPipelines (const string & pplnFolder);
	void  Layer (ERenderLayer);
};

struct SceneRayTracingPass
{

	// Set debug label and color. It is used in graphics profiler.
	void  SetDebugLabel (const string & label);
	void  SetDebugLabel (const string & label, const RGBA8u & color);
	void  AddFlag (EPassFlags);

	// Add slider to UI. Data passed to all shaders in the current pass.
	void  SliderI (const string & name);
	void  Slider (const string & name, int min, int max);
	void  Slider (const string & name, const int2 & min, const int2 & max);
	void  Slider (const string & name, const int3 & min, const int3 & max);
	void  Slider (const string & name, const int4 & min, const int4 & max);
	void  Slider (const string & name, int min, int max, int initial);
	void  Slider (const string & name, const int2 & min, const int2 & max, const int2 & initial);
	void  Slider (const string & name, const int3 & min, const int3 & max, const int3 & initial);
	void  Slider (const string & name, const int4 & min, const int4 & max, const int4 & initial);
	void  SliderF (const string & name);
	void  Slider (const string & name, float min, float max);
	void  Slider (const string & name, const float2 & min, const float2 & max);
	void  Slider (const string & name, const float3 & min, const float3 & max);
	void  Slider (const string & name, const float4 & min, const float4 & max);
	void  Slider (const string & name, float min, float max, float initial);
	void  Slider (const string & name, const float2 & min, const float2 & max, const float2 & initial);
	void  Slider (const string & name, const float3 & min, const float3 & max, const float3 & initial);
	void  Slider (const string & name, const float4 & min, const float4 & max, const float4 & initial);
	void  ColorSelector (const string & name);
	void  ColorSelector (const string & name, const RGBA32f & initial);
	void  ColorSelector (const string & name, const RGBA8u & initial);

	// TODO
	void  Constant (const string & name, const RC<DynamicFloat> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicFloat4> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicInt4> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt2> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt3> & dynamicValue);
	void  Constant (const string & name, const RC<DynamicUInt4> & dynamicValue);

	// Returns dynamic dimension of the pass.
	// It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches.
	RC<DynamicDim>  Dimension ();
	void  EnableIfEqual (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfLess (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfGreater (const RC<DynamicUInt> & dynamic, uint refValue);

	// Add resource to all shaders in the current pass.
	// In  - resource is used for read access.
	// Out - resource is used for write access.
	void  ArgIn (const string & uniformName, const RC<RTScene> & resource);
	void  ArgIn (const string & uniformName, const RC<Buffer> & resource);
	void  ArgOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgInOut (const string & uniformName, const RC<Buffer> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource);
	void  ArgOut (const string & uniformName, const RC<Image> & resource);
	void  ArgInOut (const string & uniformName, const RC<Image> & resource);
	void  ArgTex (const string & uniformName, const RC<Image> & resource);
	void  ArgIn (const string & uniformName, const RC<Image> & resource, const string & samplerName);
	void  ArgTex (const string & uniformName, const array<Image@> & resources);
	void  ArgIn (const string & uniformName, const array<Image@> & resources, const string & samplerName);
	void  ArgIn (const string & uniformName, const RC<VideoImage> & resource, const string & samplerName);
	void  Set (const RC<BaseController> & camera);
	void  ArgIn (const string & uniformName, const array<Image@> & resources);
	void  ArgOut (const string & uniformName, const array<Image@> & resources);
	void  ArgInOut (const string & uniformName, const array<Image@> & resources);

	// Set path to single pipeline.
	// Scene geometry will be linked with compatible pipeline or error will be generated.
	void  SetPipeline (const string & pplnFile);

	// Run RayGen shader with specified number of threads.
	void  Dispatch (uint threadsX);
	void  Dispatch (uint threadsX, uint threadsY);
	void  Dispatch (uint threadsX, uint threadsY, uint threadsZ);
	void  Dispatch (const uint2 & threads);
	void  Dispatch (const uint3 & threads);
	void  Dispatch (const RC<DynamicDim> & dynamicThreadCount);
	void  Dispatch (const RC<DynamicUInt> & dynamicThreadCount);

	// Run RayGen shader with number of threads from indirect command.
	void  DispatchIndirect (const RC<Buffer> & indirectBuffer);
	void  DispatchIndirect (const RC<Buffer> & indirectBuffer, uint64 indirectBufferOffset);
	void  DispatchIndirect (const RC<Buffer> & indirectBuffer, const string & indirectBufferFieldName);
};

struct Scene
{
	Scene ();

	// Attach geometry to scene.
	void  Add (const RC<GeomSource> & geometry, const float3 & position, const float3 & rotationInRads, float scale);
	void  Add (const RC<GeomSource> & geometry, const float3 & position);
	void  Add (const RC<GeomSource> & geometry);
	void  Add (const RC<GeomSource> & geometry, const float4x4 & transform);

	// Set camera to scene.
	void  Set (const RC<BaseController> & controller);

	// Add graphics pass. It will link geometries with pipelines and draw it.
	RC<SceneGraphicsPass>  AddGraphicsPass (const string & name);
	RC<SceneRayTracingPass>  AddRayTracingPass (const string & name);
};


// Returns dynamic dimensions of the screen surface.
RC<DynamicDim>  SurfaceSize ();

// Present image to the screen.
void  Present (const RC<Image> &);
void  Present (const RC<Image> &, const MipmapLevel &);
void  Present (const RC<Image> &, const ImageLayer &);
void  Present (const RC<Image> &, const ImageLayer &, const MipmapLevel &);
void  Present (const RC<Image> &, EColorSpace);
void  Present (const RC<Image> &, const ImageLayer &, const MipmapLevel &, EColorSpace);

// Draw image in child window for debugging.
void  DbgView (const RC<Image> &, DbgViewFlags);
void  DbgView (const RC<Image> &, const MipmapLevel &, DbgViewFlags);
void  DbgView (const RC<Image> &, const ImageLayer &, DbgViewFlags);
void  DbgView (const RC<Image> &, const ImageLayer &, const MipmapLevel &, DbgViewFlags);

// Pass which generates mipmaps for image.
void  GenMipmaps (const RC<Image> &);

// Pass which copy image content to another image.
void  CopyImage (const RC<Image> &, const RC<Image> &);

// Pass to clear float-color image.
void  ClearImage (const RC<Image> &, const RGBA32f &);

// Pass to clear uint-color image.
void  ClearImage (const RC<Image> &, const RGBA32u &);

// Pass to clear int-color image.
void  ClearImage (const RC<Image> &, const RGBA32i &);

// Pass to clear buffer.
void  ClearBuffer (const RC<Buffer> &, uint);
void  ClearBuffer (const RC<Buffer> & buffer, uint64 offset, uint64 size, uint value);

// Readback the image and save it to a file in DDS format. Rendering will be paused until the readback is completed.
void  Export (const RC<Image> & image, const string & prefix);

// Readback the buffer and save it to a file in structured format. Rendering will be paused until the readback is completed.
void  DbgExport (const RC<Buffer> & buffer, const string & prefix);

// Readback the buffer and save it to a file in binary format. Rendering will be paused until the readback is completed.
void  Export (const RC<Buffer> & buffer, const string & prefix);

// Pass to build RTGeometry, executed every frame.
void  BuildRTGeometry (const RC<RTGeometry> &);

// Pass to indirect build RTGeometry, executed every frame.
void  BuildRTGeometryIndirect (const RC<RTGeometry> &);

// Pass to build RTScene, executed every frame.
void  BuildRTScene (const RC<RTScene> &);

// Pass to indirect build RTScene, executed every frame.
void  BuildRTSceneIndirect (const RC<RTScene> &);
void  GetCube (array<float3> & positions, array<float3> & normals, array<uint> & indices);
void  GetCube (array<float3> & positions, array<float3> & normals, array<float3> & tangents, array<float3> & bitangents, array<float2> & texcoords2d, array<uint> & indices);
void  GetCube (array<float3> & positions, array<float3> & normals, array<float3> & tangents, array<float3> & bitangents, array<float3> & cubemapTexcoords, array<uint> & indices);

// Returns (size * size) grid
void  GetGrid (uint size, array<float2> & unorm2Positions, array<uint> & indices);

// Returns (size * size) grid in XY space.
void  GetGrid (uint size, array<float3> & unorm3Positions, array<uint> & indices);

// Returns spherical cube
void  GetSphere (uint lod, array<float3> & positions, array<uint> & indices);

// Returns spherical cube
void  GetSphere (uint lod, array<float3> & positions, array<float3> & cubemapTexcoords, array<uint> & indices);

// Returns spherical cube with tangential projection for cubemap.
void  GetSphere (uint lod, array<float3> & positions, array<float3> & normals, array<float3> & tangents, array<float3> & bitangents, array<float3> & cubemapTexcoords, array<uint> & indices);

// Returns spherical cube
void  GetSphere (uint lod, array<float3> & positions, array<float3> & normals, array<float3> & tangents, array<float3> & bitangents, array<float2> & texcoords2d, array<uint> & indices);

// Returns cylinder
void  GetCylinder (uint segmentCount, bool isInner, array<float3> & positions, array<float2> & texcoords, array<uint> & indices);

// Returns cylinder
void  GetCylinder (uint segmentCount, bool isInner, array<float3> & positions, array<float3> & normals, array<float3> & tangents, array<float3> & bitangents, array<float2> & texcoords, array<uint> & indices);

// Returns spherical cube without projection and rotation
void  GetSphericalCube (uint lod, array<float3> & positions, array<uint> & indices);

// Helper function to convert array of indices to array of uint3 indices per triangle
void  IndicesToPrimitives (const array<uint> & indices, array<uint3> & primitives);

// Helper function to convert matrix to 6 planes of the frustum.
void  GetFrustumPlanes (const float4x4 & viewProj, array<float4> & outPlanes);
void  MergeMesh (array<uint> & srcIndices, uint srcVertexCount, const array<uint> & indicesToAdd);

// Output is a TriangleList, front face: CCW
void  Extrude (const array<float2> & lineStrip, float height, array<float3> & positions, array<uint> & indices);

// Output is a TriangleList, front face: CCW
void  Triangulate (const array<float2> & lineStrip, float yCoord, array<float3> & positions, array<uint> & indices);

// Output is a TriangleList, front face: CCW
void  Triangulate (const array<float2> & vertices, const array<uint> & lineListIndices, float yCoord, array<float3> & positions, array<uint> & indices);

// Output is a TriangleList, front face: CCW
void  TriangulateAndExtrude (const array<float2> & lineStrip, float height, array<float3> & positions, array<uint> & indices);

// Output is a TriangleList, front face: CCW
void  TriangulateAndExtrude (const array<float2> & vertices, const array<uint> & lineListIndices, float height, array<float3> & positions, array<uint> & indices);

// Run script, path to script must be added to 'res_editor_cfg.as' as 'SecondaryScriptDir()'
RC<IPass>  RunScript (const string & filePath, const RC<Collection> & collection);
RC<IPass>  RunScript (const string & filePath, ScriptFlags flags, const RC<Collection> & collection);
void  Slider (const RC<DynamicInt> & dyn, const string & name);
void  Slider (const RC<DynamicInt> & dyn, const string & name, int min, int max);
void  Slider (const RC<DynamicInt2> & dyn, const string & name, const int2 & min, const int2 & max);
void  Slider (const RC<DynamicInt3> & dyn, const string & name, const int3 & min, const int3 & max);

// Add slider to UI.
void  Slider (const RC<DynamicInt4> & dyn, const string & name, const int4 & min, const int4 & max);
void  Slider (const RC<DynamicInt> & dyn, const string & name, int min, int max, int initial);
void  Slider (const RC<DynamicInt2> & dyn, const string & name, const int2 & min, const int2 & max, const int2 & initial);
void  Slider (const RC<DynamicInt3> & dyn, const string & name, const int3 & min, const int3 & max, const int3 & initial);
void  Slider (const RC<DynamicInt4> & dyn, const string & name, const int4 & min, const int4 & max, const int4 & initial);
void  Slider (const RC<DynamicUInt> & dyn, const string & name);
void  Slider (const RC<DynamicUInt> & dyn, const string & name, uint min, uint max);
void  Slider (const RC<DynamicUInt2> & dyn, const string & name, const uint2 & min, const uint2 & max);
void  Slider (const RC<DynamicUInt3> & dyn, const string & name, const uint3 & min, const uint3 & max);
void  Slider (const RC<DynamicUInt4> & dyn, const string & name, const uint4 & min, const uint4 & max);
void  Slider (const RC<DynamicUInt> & dyn, const string & name, uint min, uint max, uint initial);
void  Slider (const RC<DynamicUInt2> & dyn, const string & name, const uint2 & min, const uint2 & max, const uint2 & initial);
void  Slider (const RC<DynamicUInt3> & dyn, const string & name, const uint3 & min, const uint3 & max, const uint3 & initial);
void  Slider (const RC<DynamicUInt4> & dyn, const string & name, const uint4 & min, const uint4 & max, const uint4 & initial);
void  Slider (const RC<DynamicFloat> & dyn, const string & name);
void  Slider (const RC<DynamicFloat> & dyn, const string & name, float min, float max);
void  Slider (const RC<DynamicFloat2> & dyn, const string & name, const float2 & min, const float2 & max);
void  Slider (const RC<DynamicFloat3> & dyn, const string & name, const float3 & min, const float3 & max);
void  Slider (const RC<DynamicFloat4> & dyn, const string & name, const float4 & min, const float4 & max);
void  Slider (const RC<DynamicFloat> & dyn, const string & name, float min, float max, float initial);
void  Slider (const RC<DynamicFloat2> & dyn, const string & name, const float2 & min, const float2 & max, const float2 & initial);
void  Slider (const RC<DynamicFloat3> & dyn, const string & name, const float3 & min, const float3 & max, const float3 & initial);
void  Slider (const RC<DynamicFloat4> & dyn, const string & name, const float4 & min, const float4 & max, const float4 & initial);
void  WhiteColorSpectrum3 (array<float4> & wavelengthToRGB);
void  WhiteColorSpectrum7 (array<float4> & wavelengthToRGB, bool normalized);
void  WhiteColorSpectrumStep50nm (array<float4> & wavelengthToRGB, bool normalized);
void  WhiteColorSpectrumStep100nm (array<float4> & wavelengthToRGB, bool normalized);
float3  CM_CubeSC_Forward (const float3 &);
float3  CM_IdentitySC_Forward (const float3 &);
float3  CM_TangentialSC_Forward (const float3 &);
bool  Supported_GeometryShader ();
bool  Supported_TessellationShader ();
bool  Supported_SamplerAnisotropy ();
EPixelFormat  Supported_DepthFormat ();
EPixelFormat  Supported_DepthStencilFormat ();
#define SCRIPT

template <>
struct RC<BaseController> : BaseController
{
	RC (const BaseController &);
};

template <>
struct RC<FlightCamera> : FlightCamera
{
	RC (const FlightCamera &);
};

template <>
struct RC<TopDownCamera> : TopDownCamera
{
	RC (const TopDownCamera &);
};

template <>
struct RC<ScaleBiasCamera> : ScaleBiasCamera
{
	RC (const ScaleBiasCamera &);
};

template <>
struct RC<Scene> : Scene
{
	RC (const Scene &);
};

template <>
struct RC<DynamicUInt> : DynamicUInt
{
	RC (const DynamicUInt &);
};

template <>
struct RC<Model> : Model
{
	RC (const Model &);
};

template <>
struct RC<RayTracingPass> : RayTracingPass
{
	RC (const RayTracingPass &);
};

template <>
struct RC<FPSCamera> : FPSCamera
{
	RC (const FPSCamera &);
};

template <>
struct RC<DynamicDim> : DynamicDim
{
	RC (const DynamicDim &);
};

template <>
struct RC<DynamicFloat> : DynamicFloat
{
	RC (const DynamicFloat &);
};

template <>
struct RC<SphericalCube> : SphericalCube
{
	RC (const SphericalCube &);
};

template <>
struct RC<OrbitalCamera> : OrbitalCamera
{
	RC (const OrbitalCamera &);
};

template <>
struct RC<Buffer> : Buffer
{
	RC (const Buffer &);
};

template <>
struct RC<UnifiedGeometry> : UnifiedGeometry
{
	RC (const UnifiedGeometry &);
};

template <>
struct RC<ComputePass> : ComputePass
{
	RC (const ComputePass &);
};

template <>
struct RC<SceneRayTracingPass> : SceneRayTracingPass
{
	RC (const SceneRayTracingPass &);
};

template <>
struct RC<Postprocess> : Postprocess
{
	RC (const Postprocess &);
};

template <>
struct RC<RemoteCamera> : RemoteCamera
{
	RC (const RemoteCamera &);
};

template <>
struct RC<VideoImage> : VideoImage
{
	RC (const VideoImage &);
};

template <>
struct RC<FPVCamera> : FPVCamera
{
	RC (const FPVCamera &);
};

template <>
struct RC<SceneGraphicsPass> : SceneGraphicsPass
{
	RC (const SceneGraphicsPass &);
};

template <>
struct RC<DynamicULong> : DynamicULong
{
	RC (const DynamicULong &);
};

template <>
struct RC<RTScene> : RTScene
{
	RC (const RTScene &);
};

template <>
struct RC<Collection> : Collection
{
	RC (const Collection &);
};

template <>
struct RC<DynamicFloat4> : DynamicFloat4
{
	RC (const DynamicFloat4 &);
};

template <>
struct RC<DynamicUInt2> : DynamicUInt2
{
	RC (const DynamicUInt2 &);
};

template <>
struct RC<DynamicUInt3> : DynamicUInt3
{
	RC (const DynamicUInt3 &);
};

template <>
struct RC<DynamicFloat2> : DynamicFloat2
{
	RC (const DynamicFloat2 &);
};

template <>
struct RC<DynamicFloat3> : DynamicFloat3
{
	RC (const DynamicFloat3 &);
};

template <>
struct RC<DynamicUInt4> : DynamicUInt4
{
	RC (const DynamicUInt4 &);
};

template <>
struct RC<RTGeometry> : RTGeometry
{
	RC (const RTGeometry &);
};

template <>
struct RC<GeomSource> : GeomSource
{
	RC (const GeomSource &);
};

template <>
struct RC<DynamicInt4> : DynamicInt4
{
	RC (const DynamicInt4 &);
};

template <>
struct RC<DynamicInt3> : DynamicInt3
{
	RC (const DynamicInt3 &);
};

template <>
struct RC<DynamicInt2> : DynamicInt2
{
	RC (const DynamicInt2 &);
};

template <>
struct RC<Image> : Image
{
	RC (const Image &);
};

template <>
struct RC<IPass> : IPass
{
	RC (const IPass &);
};

template <>
struct RC<DynamicInt> : DynamicInt
{
	RC (const DynamicInt &);
};

