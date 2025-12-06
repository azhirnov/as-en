//6afd84b2
#pragma once
#include <vector>
#include <string>

#define funcdef // typedef for function

using int8		= std::int8_t;
using uint8		= std::uint8_t;
using int16		= std::int16_t;
using uint16	= std::uint16_t;
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

using namespace std::string_literals;

struct CallableIndex;
struct Random_Normal3;
struct Random_Normal2;
struct Random_Normal1;
struct UnifiedGeometry_Draw;
struct ComputeMip;
struct Random_Normal4;
struct ubyte3;
struct ubyte2;
struct VideoImage;
struct FPVCamera;
struct RemoteCamera;
struct ubyte4;
struct RTInstanceCustomIndex;
struct RTInstanceSBTOffset;
struct RTScene;
struct Collection;
struct RayIndex;
struct bool4;
struct RGBA32i;
struct SceneGraphicsPass;
struct bool2;
struct bool3;
struct DynamicULong;
struct RGBA32f;
struct HSVColor;
struct ushort4;
struct short2;
struct Quat;
struct short3;
struct Random;
struct sbyte4;
struct UnifiedGeometry_DrawIndexed;
struct sbyte3;
struct Image;
struct UnifiedGeometry_DrawIndexedIndirectCount;
struct GeomSource;
struct UnifiedGeometry_DrawIndirect;
struct DynamicInt2;
struct DynamicInt3;
struct DynamicInt4;
struct DynamicUInt3;
struct DynamicUInt2;
struct RTShader;
struct DynamicUInt4;
struct DynamicFloat4;
struct RTGeometry;
struct DynamicFloat3;
struct float4;
struct DynamicFloat2;
struct float2;
struct float3;
struct DynamicInt;
struct InstanceIndex;
struct RTInstanceTransform;
struct float2x2;
struct float2x3;
struct float2x4;
struct UnifiedGeometry_DrawIndexedIndirect;
struct Random_Binomial1;
struct Random_Binomial3;
struct Random_Binomial2;
struct MultiSamples;
struct IPass;
struct Random_Binomial4;
struct DepthStencil;
struct TopDownCamera;
struct ScaleBiasCamera;
struct BaseController;
struct MipmapLevel;
struct FlightCamera;
struct float3x4;
struct Model;
struct float3x3;
struct float3x2;
struct float4x3;
struct VertexStride;
struct DynamicUInt;
struct float4x4;
struct float4x2;
struct int3;
struct int2;
struct ImageLayer;
struct int4;
struct Scene;
struct ushort3;
struct sbyte2;
struct ushort2;
struct short4;
struct RGBA8u;
struct OrbitalCamera;
struct VertexAttribDivisor;
struct UnifiedGeometry_DrawMeshTasksIndirectCount;
struct RGBA32u;
struct UnifiedGeometry_DrawMeshTasks;
struct RectF;
struct DynamicFloat;
struct RTInstanceMask;
struct SphericalCube;
struct RectI;
struct uint3;
struct uint2;
struct RectU;
struct RayTracingPass;
struct uint4;
struct DynamicDim;
struct FPSCamera;
struct FeatureSet;
struct UnifiedGeometry_DrawMeshTasksIndirect;
struct ComputePass;
struct Postprocess;
struct SceneRayTracingPass;
struct EnableLabel;
struct RasterMip;
struct UnifiedGeometry;
struct UnifiedGeometry_DrawIndirectCount;
struct Buffer;

enum class EImageType : uint16
{
	Cube,
	CubeArray,
	Buffer,
	Float,
	Half,
	SNorm,
	UNorm,
	Int,
	UInt,
	Depth,
	Stencil,
	DepthStencil,
	UFloat,
	Long,
	ULong,
	Shadow,
	sRGB,
	Float_1D,
	Float_2D,
	Float_3D,
	Float_1DArray,
	Float_2DArray,
	Float_Cube,
	Float_CubeArray,
	Float_2DMS,
	Float_2DMSArray,
	Float_Buffer,
	Float_1D_sRGB,
	Float_2D_sRGB,
	Float_3D_sRGB,
	Float_1DArray_sRGB,
	Float_2DArray_sRGB,
	Float_Cube_sRGB,
	Float_CubeArray_sRGB,
	Float_2DMS_sRGB,
	Float_2DMSArray_sRGB,
	Float_Buffer_sRGB,
	Half_1D,
	Half_2D,
	Half_3D,
	Half_1DArray,
	Half_2DArray,
	Half_Cube,
	Half_CubeArray,
	Half_2DMS,
	Half_2DMSArray,
	Half_Buffer,
	Depth_2D,
	Depth_2DArray,
	Depth_Cube,
	Depth_CubeArray,
	Depth_2DMS,
	Depth_2DMSArray,
	Cube_Shadow,
	CubeArray_Shadow,
	Int_1D,
	Int_2D,
	Int_3D,
	Int_1DArray,
	Int_2DArray,
	Int_Cube,
	Int_CubeArray,
	Int_2DMS,
	Int_2DMSArray,
	Int_Buffer,
	UInt_1D,
	UInt_2D,
	UInt_3D,
	UInt_1DArray,
	UInt_2DArray,
	UInt_Cube,
	UInt_CubeArray,
	UInt_2DMS,
	UInt_2DMSArray,
	UInt_Buffer,
	SLong_Image1D,
	SLong_Image2D,
	SLong_Image3D,
	SLong_Image1DArray,
	SLong_Image2DArray,
	SLong_ImageCube,
	SLong_ImageCubeArray,
	SLong_Image2DMS,
	SLong_Image2DMSArray,
	SLong_ImageBuffer,
	ULong_Image1D,
	ULong_Image2D,
	ULong_Image3D,
	ULong_Image1DArray,
	ULong_Image2DArray,
	ULong_ImageCube,
	ULong_ImageCubeArray,
	ULong_Image2DMS,
	ULong_Image2DMSArray,
	ULong_ImageBuffer,
};
uint16  operator | (EImageType lhs, EImageType rhs);
uint16  operator | (uint16 lhs, EImageType rhs);
uint16  operator | (EImageType lhs, uint16 rhs);
static constexpr EImageType EImageType_1D = EImageType(1);
static constexpr EImageType EImageType_1DArray = EImageType(2);
static constexpr EImageType EImageType_2D = EImageType(3);
static constexpr EImageType EImageType_2DArray = EImageType(4);
static constexpr EImageType EImageType_2DMS = EImageType(5);
static constexpr EImageType EImageType_2DMSArray = EImageType(6);
static constexpr EImageType EImageType_3D = EImageType(9);
static constexpr EImageType EImageType_1D_Shadow = EImageType(369);
static constexpr EImageType EImageType_2D_Shadow = EImageType(371);
static constexpr EImageType EImageType_1DArray_Shadow = EImageType(370);
static constexpr EImageType EImageType_2DArray_Shadow = EImageType(372);

enum class EAttachmentLoadOp : uint8
{
	Invalidate,
	Load,
	Clear,
	None,
};
uint8  operator | (EAttachmentLoadOp lhs, EAttachmentLoadOp rhs);
uint8  operator | (uint8 lhs, EAttachmentLoadOp rhs);
uint8  operator | (EAttachmentLoadOp lhs, uint8 rhs);

enum class EAttachmentStoreOp : uint8
{
	Invalidate,
	Store,
	None,
};
uint8  operator | (EAttachmentStoreOp lhs, EAttachmentStoreOp rhs);
uint8  operator | (uint8 lhs, EAttachmentStoreOp rhs);
uint8  operator | (EAttachmentStoreOp lhs, uint8 rhs);

enum class EImage : uint8
{
	Cube,
	CubeArray,
};
uint8  operator | (EImage lhs, EImage rhs);
uint8  operator | (uint8 lhs, EImage rhs);
uint8  operator | (EImage lhs, uint8 rhs);
static constexpr EImage EImage_1D = EImage(0);
static constexpr EImage EImage_2D = EImage(1);
static constexpr EImage EImage_3D = EImage(2);
static constexpr EImage EImage_1DArray = EImage(3);
static constexpr EImage EImage_2DArray = EImage(4);

enum class EIndex : uint8
{
	UShort,
	UInt,
};
uint8  operator | (EIndex lhs, EIndex rhs);
uint8  operator | (uint8 lhs, EIndex rhs);
uint8  operator | (EIndex lhs, uint8 rhs);

enum class EPixelFormat : uint8
{
	RGBA16_SNorm,
	RGBA8_SNorm,
	RGB16_SNorm,
	RGB8_SNorm,
	RG16_SNorm,
	RG8_SNorm,
	R16_SNorm,
	R8_SNorm,
	RGBA16_UNorm,
	RGBA8_UNorm,
	RGB16_UNorm,
	RGB8_UNorm,
	RG16_UNorm,
	RG8_UNorm,
	R16_UNorm,
	R8_UNorm,
	RGB10_A2_UNorm,
	RGBA4_UNorm,
	RGB5_A1_UNorm,
	R5G6B5_UNorm,
	BGR8_UNorm,
	BGRA8_UNorm,
	sRGB8,
	sRGB8_A8,
	sBGR8,
	sBGR8_A8,
	R8I,
	RG8I,
	RGB8I,
	RGBA8I,
	R16I,
	RG16I,
	RGB16I,
	RGBA16I,
	R32I,
	RG32I,
	RGB32I,
	RGBA32I,
	R64I,
	R8U,
	RG8U,
	RGB8U,
	RGBA8U,
	R16U,
	RG16U,
	RGB16U,
	RGBA16U,
	R32U,
	RG32U,
	RGB32U,
	RGBA32U,
	RGB10_A2U,
	R64U,
	R16F,
	RG16F,
	RGB16F,
	RGBA16F,
	R32F,
	RG32F,
	RGB32F,
	RGBA32F,
	R11G11B10F,
	RGB9F_E5,
	Depth16,
	Depth24,
	Depth32F,
	Depth16_Stencil8,
	Depth24_Stencil8,
	Depth32F_Stencil8,
	BC1_RGB8_UNorm,
	BC1_sRGB8,
	BC1_RGB8_A1_UNorm,
	BC1_sRGB8_A1,
	BC2_RGBA8_UNorm,
	BC2_sRGB8,
	BC3_RGBA8_UNorm,
	BC3_sRGB8,
	BC4_R8_SNorm,
	BC4_R8_UNorm,
	BC5_RG8_SNorm,
	BC5_RG8_UNorm,
	BC6H_RGB16F,
	BC6H_RGB16UF,
	BC7_RGBA8_UNorm,
	BC7_sRGB8_A8,
	ETC2_RGB8_UNorm,
	ETC2_sRGB8,
	ETC2_RGB8_A1_UNorm,
	ETC2_sRGB8_A1,
	ETC2_RGBA8_UNorm,
	ETC2_sRGB8_A8,
	EAC_R11_SNorm,
	EAC_R11_UNorm,
	EAC_RG11_SNorm,
	EAC_RG11_UNorm,
	ASTC_RGBA8_4x4,
	ASTC_RGBA8_5x4,
	ASTC_RGBA8_5x5,
	ASTC_RGBA8_6x5,
	ASTC_RGBA8_6x6,
	ASTC_RGBA8_8x5,
	ASTC_RGBA8_8x6,
	ASTC_RGBA8_8x8,
	ASTC_RGBA8_10x5,
	ASTC_RGBA8_10x6,
	ASTC_RGBA8_10x8,
	ASTC_RGBA8_10x10,
	ASTC_RGBA8_12x10,
	ASTC_RGBA8_12x12,
	ASTC_sRGB8_A8_4x4,
	ASTC_sRGB8_A8_5x4,
	ASTC_sRGB8_A8_5x5,
	ASTC_sRGB8_A8_6x5,
	ASTC_sRGB8_A8_6x6,
	ASTC_sRGB8_A8_8x5,
	ASTC_sRGB8_A8_8x6,
	ASTC_sRGB8_A8_8x8,
	ASTC_sRGB8_A8_10x5,
	ASTC_sRGB8_A8_10x6,
	ASTC_sRGB8_A8_10x8,
	ASTC_sRGB8_A8_10x10,
	ASTC_sRGB8_A8_12x10,
	ASTC_sRGB8_A8_12x12,
	ASTC_RGBA16F_4x4,
	ASTC_RGBA16F_5x4,
	ASTC_RGBA16F_5x5,
	ASTC_RGBA16F_6x5,
	ASTC_RGBA16F_6x6,
	ASTC_RGBA16F_8x5,
	ASTC_RGBA16F_8x6,
	ASTC_RGBA16F_8x8,
	ASTC_RGBA16F_10x5,
	ASTC_RGBA16F_10x6,
	ASTC_RGBA16F_10x8,
	ASTC_RGBA16F_10x10,
	ASTC_RGBA16F_12x10,
	ASTC_RGBA16F_12x12,
	G8B8G8R8_422_UNorm,
	B8G8R8G8_422_UNorm,
	G8_B8R8_420_UNorm,
	G8_B8R8_422_UNorm,
	G8_B8R8_444_UNorm,
	G8_B8_R8_420_UNorm,
	G8_B8_R8_422_UNorm,
	G8_B8_R8_444_UNorm,
	B10x6G10x6R10x6G10x6_422_UNorm,
	G10x6B10x6G10x6R10x6_422_UNorm,
	G10x6_B10x6R10x6_420_UNorm,
	G10x6_B10x6R10x6_422_UNorm,
	G10x6_B10x6R10x6_444_UNorm,
	G10x6_B10x6_R10x6_420_UNorm,
	G10x6_B10x6_R10x6_422_UNorm,
	G10x6_B10x6_R10x6_444_UNorm,
	R10x6G10x6B10x6A10x6_UNorm,
	R10x6G10x6_UNorm,
	R10x6_UNorm,
	B12x4G12x4R12x4G12x4_422_UNorm,
	G12x4B12x4G12x4R12x4_422_UNorm,
	G12x4_B12x4R12x4_420_UNorm,
	G12x4_B12x4R12x4_422_UNorm,
	G12x4_B12x4R12x4_444_UNorm,
	G12x4_B12x4_R12x4_420_UNorm,
	G12x4_B12x4_R12x4_422_UNorm,
	G12x4_B12x4_R12x4_444_UNorm,
	R12x4G12x4B12x4A12x4_UNorm,
	R12x4G12x4_UNorm,
	R12x4_UNorm,
	B16G16R16G16_422_UNorm,
	G16B16G16R16_422_UNorm,
	G16_B16R16_420_UNorm,
	G16_B16R16_422_UNorm,
	G16_B16R16_444_UNorm,
	G16_B16_R16_420_UNorm,
	G16_B16_R16_422_UNorm,
	G16_B16_R16_444_UNorm,
	SwapchainColor,
};
uint8  operator | (EPixelFormat lhs, EPixelFormat rhs);
uint8  operator | (uint8 lhs, EPixelFormat rhs);
uint8  operator | (EPixelFormat lhs, uint8 rhs);

enum class EPixelFormatExternal : uint8
{
	Android_Depth16,
	Android_DepthJPEG,
	Android_DepthPointCloud,
	Android_JPEG,
	Android_Raw16,
	Android_Raw12,
	Android_Raw10,
	Android_NV16,
	Android_NV21,
	Android_YCBCR_P010,
	Android_YUV_420,
	Android_YUV_422,
	Android_YUV_444,
	Android_YUY2,
	Android_YV12,
	Android_Y8,
	Android_HEIC,
};
uint8  operator | (EPixelFormatExternal lhs, EPixelFormatExternal rhs);
uint8  operator | (uint8 lhs, EPixelFormatExternal rhs);
uint8  operator | (EPixelFormatExternal lhs, uint8 rhs);

enum class ECompareOp : uint8
{
	Never,
	Less,
	Equal,
	LEqual,
	Greater,
	NotEqual,
	GEqual,
	Always,
	LessOrEqual,
	GreaterOrEqual,
};
uint8  operator | (ECompareOp lhs, ECompareOp rhs);
uint8  operator | (uint8 lhs, ECompareOp rhs);
uint8  operator | (ECompareOp lhs, uint8 rhs);

enum class EBlendFactor : uint8
{

	// S, srcColor - from shader
	// D, dstColor - from render target
	// S1 - from shader (dual src blend)
	// cc - constant color
	// result = srcColor * srcBlend [blendOp] dstColor * dstBlend
	//

	// 0
	Zero,

	// 1
	One,

	// S
	SrcColor,

	// 1 - S
	OneMinusSrcColor,

	// D
	DstColor,

	// 1 - D
	OneMinusDstColor,

	// S.a
	SrcAlpha,

	// 1 - S.a
	OneMinusSrcAlpha,

	// D.a
	DstAlpha,

	// 1 - D.a
	OneMinusDstAlpha,

	// cc
	ConstColor,

	// 1 - cc
	OneMinusConstColor,

	// cc.a
	ConstAlpha,

	// 1 - cc.a
	OneMinusConstAlpha,

	// rgb * min( S.a, D.a ), a * 1
	SrcAlphaSaturate,

	// S1
	Src1Color,

	// 1 - S1
	OneMinusSrc1Color,

	// S1.a
	Src1Alpha,

	// 1 - S1.a
	OneMinusSrc1Alpha,
};
uint8  operator | (EBlendFactor lhs, EBlendFactor rhs);
uint8  operator | (uint8 lhs, EBlendFactor rhs);
uint8  operator | (EBlendFactor lhs, uint8 rhs);

enum class EBlendOp : uint8
{

	// S, srcColor - from shader
	// D, dstColor - from render target
	// result = srcColor * srcBlend [blendOp] dstColor * dstBlend
	//

	// S + D
	Add,

	// S - D
	Sub,

	// D - S
	RevSub,

	// min( S, D )
	Min,

	// max( S, D )
	Max,
};
uint8  operator | (EBlendOp lhs, EBlendOp rhs);
uint8  operator | (uint8 lhs, EBlendOp rhs);
uint8  operator | (EBlendOp lhs, uint8 rhs);

enum class ELogicOp : uint8
{

	// S - from shader
	// D - from render target
	// result = S [logicOp] D
	//

	// disabled
	None,

	// 0
	Clear,

	// 1
	Set,

	// S
	Copy,

	// ~S
	CopyInverted,

	// D
	NoOp,

	// ~D
	Invert,

	// S & D
	And,

	// ~ ( S & D )
	NotAnd,

	// S | D
	Or,

	// ~ ( S | D )
	NotOr,

	// S ^ D
	Xor,

	// ~ ( S ^ D )
	Equiv,

	// S & ~D
	AndReverse,

	// ~S & D
	AndInverted,

	// S | ~D
	OrReverse,

	// ~S | D
	OrInverted,
};
uint8  operator | (ELogicOp lhs, ELogicOp rhs);
uint8  operator | (uint8 lhs, ELogicOp rhs);
uint8  operator | (ELogicOp lhs, uint8 rhs);

enum class EStencilOp : uint8
{

	// src
	Keep,

	// 0
	Zero,

	// ref
	Replace,

	// min( ++src, 0 )
	Incr,

	// ++src & maxValue
	IncrWrap,

	// max( --src, 0 )
	Decr,

	// --src & maxValue
	DecrWrap,

	// ~src
	Invert,
};
uint8  operator | (EStencilOp lhs, EStencilOp rhs);
uint8  operator | (uint8 lhs, EStencilOp rhs);
uint8  operator | (EStencilOp lhs, uint8 rhs);

enum class EPolygonMode : uint8
{
	Point,
	Line,
	Fill,
};
uint8  operator | (EPolygonMode lhs, EPolygonMode rhs);
uint8  operator | (uint8 lhs, EPolygonMode rhs);
uint8  operator | (EPolygonMode lhs, uint8 rhs);

enum class EPrimitive : uint8
{
	Point,
	LineList,
	LineStrip,
	LineListAdjacency,
	LineStripAdjacency,
	TriangleList,
	TriangleStrip,
	TriangleFan,
	TriangleListAdjacency,
	TriangleStripAdjacency,
	Patch,
};
uint8  operator | (EPrimitive lhs, EPrimitive rhs);
uint8  operator | (uint8 lhs, EPrimitive rhs);
uint8  operator | (EPrimitive lhs, uint8 rhs);

enum class ECullMode : uint8
{
	None,
	Front,
	Back,
	FontAndBack,
};
uint8  operator | (ECullMode lhs, ECullMode rhs);
uint8  operator | (uint8 lhs, ECullMode rhs);
uint8  operator | (ECullMode lhs, uint8 rhs);

enum class EPipelineDynamicState : uint16
{
	None,
	StencilCompareMask,
	StencilWriteMask,
	StencilReference,
	DepthBias,
	BlendConstants,
	RTStackSize,
	FragmentShadingRate,
	ViewportWScaling,
};
uint16  operator | (EPipelineDynamicState lhs, EPipelineDynamicState rhs);
uint16  operator | (uint16 lhs, EPipelineDynamicState rhs);
uint16  operator | (EPipelineDynamicState lhs, uint16 rhs);

enum class EResourceState : uint32
{
	Unknown,
	Preserve,
	ShaderStorage_Read,
	ShaderStorage_Write,
	ShaderStorage_RW,
	ShaderUniform,
	ShaderSample,
	CopySrc,
	CopyDst,
	ClearDst,
	BlitSrc,
	BlitDst,
	InputColorAttachment,
	InputColorAttachment_RW,
	ColorAttachment,
	ColorAttachment_Blend,
	DepthStencilAttachment_Read,
	DepthStencilAttachment_Write,
	DepthStencilAttachment_RW,
	DepthTest_StencilRW,
	DepthRW_StencilTest,
	DepthStencilTest_ShaderSample,
	DepthTest_DepthSample_StencilRW,
	InputDepthStencilAttachment,
	InputDepthStencilAttachment_RW,
	Host_Read,
	PresentImage,
	IndirectBuffer,
	IndexBuffer,
	VertexBuffer,
	ShadingRateImage,
	FragmentDensityMap,
	CopyRTAS_Read,
	CopyRTAS_Write,
	BuildRTAS_Read,
	BuildRTAS_Write,
	BuildRTAS_RW,
	BuildRTAS_IndirectBuffer,
	ShaderRTAS,
	RTShaderBindingTable,
	DSTestBeforeFS,
	DSTestAfterFS,
	Invalidate,
	General,
	MeshTaskShader,
	VertexProcessingShaders,
	TileShader,
	FragmentShader,
	PreRasterizationShaders,
	PostRasterizationShaders,
	ComputeShader,
	RayTracingShaders,
	AllGraphicsShaders,
	AllShaderStages,
	AllStages,
	CoopVecConvertStage,
	BuildRTAS_ScratchBuffer,
	InputDepthAttachment,
	DepthStencilAttachment,
};
uint32  operator | (EResourceState lhs, EResourceState rhs);
uint32  operator | (uint32 lhs, EResourceState rhs);
uint32  operator | (EResourceState lhs, uint32 rhs);

enum class EImageAspect : uint8
{
	Color,
	Depth,
	Stencil,
	DepthStencil,
	Plane_0,
	Plane_1,
	Plane_2,
};
uint8  operator | (EImageAspect lhs, EImageAspect rhs);
uint8  operator | (uint8 lhs, EImageAspect rhs);
uint8  operator | (EImageAspect lhs, uint8 rhs);

enum class EShaderIO : uint8
{
	Int,
	UInt,
	Float,
	UFloat,
	Half,
	UNorm,
	SNorm,
	sRGB,
	AnyColor,
	Depth,
	Stencil,
	DepthStencil,
};
uint8  operator | (EShaderIO lhs, EShaderIO rhs);
uint8  operator | (uint8 lhs, EShaderIO rhs);
uint8  operator | (EShaderIO lhs, uint8 rhs);

enum class ESubgroupTypes : uint8
{
	Float16,
	Float32,
	Int8,
	Int16,
	Int32,
	Int64,
};
uint8  operator | (ESubgroupTypes lhs, ESubgroupTypes rhs);
uint8  operator | (uint8 lhs, ESubgroupTypes rhs);
uint8  operator | (ESubgroupTypes lhs, uint8 rhs);

enum class ESubgroupOperation : uint32
{
	IndexAndSize,
	Elect,
	Barrier,
	Any,
	All,
	AllEqual,
	Add,
	Mul,
	Min,
	Max,
	And,
	Or,
	Xor,
	InclusiveMul,
	InclusiveAdd,
	InclusiveMin,
	InclusiveMax,
	InclusiveAnd,
	InclusiveOr,
	InclusiveXor,
	ExclusiveAdd,
	ExclusiveMul,
	ExclusiveMin,
	ExclusiveMax,
	ExclusiveAnd,
	ExclusiveOr,
	ExclusiveXor,
	Ballot,
	Broadcast,
	BroadcastFirst,
	InverseBallot,
	BallotBitExtract,
	BallotBitCount,
	BallotInclusiveBitCount,
	BallotExclusiveBitCount,
	BallotFindLSB,
	BallotFindMSB,
	Shuffle,
	ShuffleXor,
	ShuffleUp,
	ShuffleDown,
	ClusteredAdd,
	ClusteredMul,
	ClusteredMin,
	ClusteredMax,
	ClusteredAnd,
	ClusteredOr,
	ClusteredXor,
	QuadBroadcast,
	QuadSwapHorizontal,
	QuadSwapVertical,
	QuadSwapDiagonal,
	_Basic_Begin,
	_Basic_End,
	_Vote_Begin,
	_Vote_End,
	_Arithmetic_Begin,
	_Arithmetic_End,
	_Ballot_Begin,
	_Ballot_End,
	_Shuffle_Begin,
	_Shuffle_End,
	_ShuffleRelative_Begin,
	_ShuffleRelative_End,
	_Clustered_Begin,
	_Clustered_End,
	_Quad_Begin,
	_Quad_End,
};
uint32  operator | (ESubgroupOperation lhs, ESubgroupOperation rhs);
uint32  operator | (uint32 lhs, ESubgroupOperation rhs);
uint32  operator | (ESubgroupOperation lhs, uint32 rhs);

enum class EFeature : uint8
{
	Ignore,
	RequireTrue,
	RequireFalse,
};
uint8  operator | (EFeature lhs, EFeature rhs);
uint8  operator | (uint8 lhs, EFeature rhs);
uint8  operator | (EFeature lhs, uint8 rhs);

enum class EShader : uint8
{
	Vertex,
	TessControl,
	TessEvaluation,
	Geometry,
	Fragment,
	Compute,
	Tile,
	MeshTask,
	Mesh,
	RayGen,
	RayAnyHit,
	RayClosestHit,
	RayMiss,
	RayIntersection,
	RayCallable,
};
uint8  operator | (EShader lhs, EShader rhs);
uint8  operator | (uint8 lhs, EShader rhs);
uint8  operator | (EShader lhs, uint8 rhs);

enum class EShaderStages : uint16
{
	Vertex,
	TessControl,
	TessEvaluation,
	Geometry,
	Fragment,
	Compute,
	Tile,
	MeshTask,
	Mesh,
	RayGen,
	RayAnyHit,
	RayClosestHit,
	RayMiss,
	RayIntersection,
	RayCallable,
	All,
	AllGraphics,
	GraphicsPipeStages,
	MeshPipeStages,
	VertexProcessingStages,
	PreRasterizationStages,
	PostRasterizationStages,
	AllRayTracing,
};
uint16  operator | (EShaderStages lhs, EShaderStages rhs);
uint16  operator | (uint16 lhs, EShaderStages rhs);
uint16  operator | (EShaderStages lhs, uint16 rhs);

enum class EGPUVendor : uint32
{
	AMD,
	NVidia,
	Intel,
	ARM,
	Qualcomm,
	ImgTech,
	Microsoft,
	Apple,
	Mesa,
	Broadcom,
	Samsung,
	VeriSilicon,
	Huawei,
};
uint32  operator | (EGPUVendor lhs, EGPUVendor rhs);
uint32  operator | (uint32 lhs, EGPUVendor rhs);
uint32  operator | (EGPUVendor lhs, uint32 rhs);

enum class EVertexType : uint16
{
	Byte,
	Byte2,
	Byte3,
	Byte4,
	Byte_Norm,
	Byte2_Norm,
	Byte3_Norm,
	Byte4_Norm,
	Byte_Scaled,
	Byte2_Scaled,
	Byte3_Scaled,
	Byte4_Scaled,
	UByte,
	UByte2,
	UByte3,
	UByte4,
	UByte_Norm,
	UByte2_Norm,
	UByte3_Norm,
	UByte4_Norm,
	UByte_Scaled,
	UByte2_Scaled,
	UByte3_Scaled,
	UByte4_Scaled,
	Short,
	Short2,
	Short3,
	Short4,
	Short_Norm,
	Short2_Norm,
	Short3_Norm,
	Short4_Norm,
	Short_Scaled,
	Short2_Scaled,
	Short3_Scaled,
	Short4_Scaled,
	UShort,
	UShort2,
	UShort3,
	UShort4,
	UShort_Norm,
	UShort2_Norm,
	UShort3_Norm,
	UShort4_Norm,
	UShort_Scaled,
	UShort2_Scaled,
	UShort3_Scaled,
	UShort4_Scaled,
	Int,
	Int2,
	Int3,
	Int4,
	UInt,
	UInt2,
	UInt3,
	UInt4,
	Long,
	Long2,
	Long3,
	Long4,
	ULong,
	ULong2,
	ULong3,
	ULong4,
	Half,
	Half2,
	Half3,
	Half4,
	Float,
	Float2,
	Float3,
	Float4,
	Double,
	Double2,
	Double3,
	Double4,
	UInt_2_10_10_10,
	UInt_2_10_10_10_Norm,
	UInt_2_10_10_10_Scaled,
};
uint16  operator | (EVertexType lhs, EVertexType rhs);
uint16  operator | (uint16 lhs, EVertexType rhs);
uint16  operator | (EVertexType lhs, uint16 rhs);

enum class EGraphicsDeviceID : uint32
{
	Adreno_500,
	Adreno_600,
	Adreno_700,
	Adreno_800,
	AMD_GCN1,
	AMD_GCN2,
	AMD_GCN3,
	AMD_GCN4,
	AMD_GCN5,
	AMD_GCN5_APU,
	AMD_RDNA1,
	AMD_RDNA2,
	AMD_RDNA2_APU,
	AMD_RDNA3,
	AMD_RDNA3_APU,
	AMD_RDNA4,
	Apple_A8,
	Apple_A9_A10,
	Apple_A11,
	Apple_A12,
	Apple_A13,
	Apple_A14_M1,
	Apple_A15_M2,
	Apple_A16,
	Apple_A17_M3,
	Mali_Midgard_Gen2,
	Mali_Midgard_Gen3,
	Mali_Midgard_Gen4,
	Mali_Bifrost_Gen1,
	Mali_Bifrost_Gen2,
	Mali_Bifrost_Gen3,
	Mali_Valhall_Gen1,
	Mali_Valhall_Gen2,
	Mali_Valhall_Gen3,
	Mali_Valhall_Gen4,
	Mali_5thGen_Gen1,
	Mali_5thGen_Gen2,
	NV_Maxwell,
	NV_Maxwell_Tegra,
	NV_Pascal,
	NV_Pascal_MX,
	NV_Pascal_Tegra,
	NV_Volta,
	NV_Turing_16,
	NV_Turing,
	NV_Turing_MX,
	NV_Ampere,
	NV_Ampere_Orin,
	NV_Ada,
	NV_Blackwell,
	Intel_Gen7,
	Intel_Gen8,
	Intel_Gen9,
	Intel_Gen11,
	Intel_Gen12,
	Intel_Xe1,
	Intel_Xe2,
	PowerVR_Series8,
	PowerVR_Series9,
	PowerVR_SeriesA,
	PowerVR_SeriesB,
	PowerVR_SeriesC,
	PowerVR_SeriesD,
	VeriSilicon,
	SwiftShader,
};
uint32  operator | (EGraphicsDeviceID lhs, EGraphicsDeviceID rhs);
uint32  operator | (uint32 lhs, EGraphicsDeviceID rhs);
uint32  operator | (EGraphicsDeviceID lhs, uint32 rhs);

enum class EFilter : uint8
{
	Nearest,
	Linear,
};
uint8  operator | (EFilter lhs, EFilter rhs);
uint8  operator | (uint8 lhs, EFilter rhs);
uint8  operator | (EFilter lhs, uint8 rhs);

enum class EMipmapFilter : uint8
{
	None,
	Nearest,
	Linear,
};
uint8  operator | (EMipmapFilter lhs, EMipmapFilter rhs);
uint8  operator | (uint8 lhs, EMipmapFilter rhs);
uint8  operator | (EMipmapFilter lhs, uint8 rhs);

enum class EAddressMode : uint8
{
	Repeat,
	MirrorRepeat,
	ClampToEdge,
	ClampToBorder,
	MirrorClampToEdge,
	Clamp,
	MirrorClamp,
};
uint8  operator | (EAddressMode lhs, EAddressMode rhs);
uint8  operator | (uint8 lhs, EAddressMode rhs);
uint8  operator | (EAddressMode lhs, uint8 rhs);

enum class EBorderColor : uint8
{
	FloatTransparentBlack,
	FloatOpaqueBlack,
	FloatOpaqueWhite,
	IntTransparentBlack,
	IntOpaqueBlack,
	IntOpaqueWhite,
};
uint8  operator | (EBorderColor lhs, EBorderColor rhs);
uint8  operator | (uint8 lhs, EBorderColor rhs);
uint8  operator | (EBorderColor lhs, uint8 rhs);

enum class EReductionMode : uint8
{
	Average,
	Min,
	Max,
};
uint8  operator | (EReductionMode lhs, EReductionMode rhs);
uint8  operator | (uint8 lhs, EReductionMode rhs);
uint8  operator | (EReductionMode lhs, uint8 rhs);

enum class ESamplerOpt : uint8
{
	ArgumentBuffer,
	UnnormalizedCoordinates,
	NonSeamlessCubeMap,
	Subsampled,
	SubsampledCoarseReconstruction,
};
uint8  operator | (ESamplerOpt lhs, ESamplerOpt rhs);
uint8  operator | (uint8 lhs, ESamplerOpt rhs);
uint8  operator | (ESamplerOpt lhs, uint8 rhs);

enum class EVertexInputRate : uint8
{
	Vertex,
	Instance,
};
uint8  operator | (EVertexInputRate lhs, EVertexInputRate rhs);
uint8  operator | (uint8 lhs, EVertexInputRate rhs);
uint8  operator | (EVertexInputRate lhs, uint8 rhs);

enum class EDescSetUsage : uint8
{
	AllowPartialyUpdate,
	UpdateTemplate,
	ArgumentBuffer,
	MutableArgBuffer,
	MaybeUnsupported,
};
uint8  operator | (EDescSetUsage lhs, EDescSetUsage rhs);
uint8  operator | (uint8 lhs, EDescSetUsage rhs);
uint8  operator | (EDescSetUsage lhs, uint8 rhs);

enum class EPipelineOpt : uint16
{

	// Optimize pipeline during creation, may be slow.
	Optimize,
	CS_DispatchBase,
	RT_NoNullAnyHitShaders,
	RT_NoNullClosestHitShaders,
	RT_NoNullMissShaders,
	RT_NoNullIntersectionShaders,
	RT_SkipTriangles,
	RT_SkipAABBs,
	RT_AllowClusterAccelStruct,

	// Pipeline creation will fail if it is not exists in cache.
	DontCompile,

	// When a pipeline is created, its state and shaders are compiled into zero or more device-specific executables,
	// which are used when executing commands against that pipeline.
	CaptureStatistics,

	// May include the final shader assembly, a binary form of the compiled shader,
	// or the shader compiler’s internal representation at any number of intermediate compile steps.
	CaptureInternalRepresentation,

	// Disable pipeline optimization to speedup creation.
	DontOptimize,
	None,
};
uint16  operator | (EPipelineOpt lhs, EPipelineOpt rhs);
uint16  operator | (uint16 lhs, EPipelineOpt rhs);
uint16  operator | (EPipelineOpt lhs, uint16 rhs);

enum class EQueueMask : uint8
{
	Graphics,
	AsyncCompute,
	AsyncTransfer,
	VideoEncode,
	VideoDecode,
	All,
};
uint8  operator | (EQueueMask lhs, EQueueMask rhs);
uint8  operator | (uint8 lhs, EQueueMask rhs);
uint8  operator | (EQueueMask lhs, uint8 rhs);

enum class ESamplerChromaLocation : uint8
{
	CositedEven,
	Midpoint,
};
uint8  operator | (ESamplerChromaLocation lhs, ESamplerChromaLocation rhs);
uint8  operator | (uint8 lhs, ESamplerChromaLocation rhs);
uint8  operator | (ESamplerChromaLocation lhs, uint8 rhs);

enum class ESamplerYcbcrModelConversion : uint8
{
	RGB_Identity,
	Ycbcr_Identity,
	Ycbcr_709,
	Ycbcr_601,
	Ycbcr_2020,
};
uint8  operator | (ESamplerYcbcrModelConversion lhs, ESamplerYcbcrModelConversion rhs);
uint8  operator | (uint8 lhs, ESamplerYcbcrModelConversion rhs);
uint8  operator | (ESamplerYcbcrModelConversion lhs, uint8 rhs);

enum class ESamplerYcbcrRange : uint8
{
	ITU_Full,
	ITU_Narrow,
};
uint8  operator | (ESamplerYcbcrRange lhs, ESamplerYcbcrRange rhs);
uint8  operator | (uint8 lhs, ESamplerYcbcrRange rhs);
uint8  operator | (ESamplerYcbcrRange lhs, uint8 rhs);

enum class ESurfaceFormat : uint8
{
	BGRA8_sRGB_nonlinear,
	RGBA8_sRGB_nonlinear,
	BGRA8_BT709_nonlinear,
	RGBA16F_sRGB_nonlinear,
	RGBA16F_Extended_sRGB_linear,
	RGBA16F_Extended_sRGB_nonlinear,
	RGBA16F_BT709_nonlinear,
	RGBA16F_HDR10_ST2084,
	RGBA16F_BT2020_linear,
	RGB10A2_sRGB_nonlinear,
	RGB10A2_HDR10_ST2084,
};
uint8  operator | (ESurfaceFormat lhs, ESurfaceFormat rhs);
uint8  operator | (uint8 lhs, ESurfaceFormat rhs);
uint8  operator | (ESurfaceFormat lhs, uint8 rhs);

enum class ERTInstanceOpt : uint8
{
	TriangleCullDisable,
	TriangleFrontCCW,
	ForceOpaque,
	ForceNonOpaque,
	TriangleCullBack,
	TriangleFrontCW,
};
uint8  operator | (ERTInstanceOpt lhs, ERTInstanceOpt rhs);
uint8  operator | (uint8 lhs, ERTInstanceOpt rhs);
uint8  operator | (ERTInstanceOpt lhs, uint8 rhs);

enum class EImageUsage : uint16
{
	TransferSrc,
	TransferDst,
	Sampled,
	Storage,
	ColorAttachment,
	DepthStencilAttachment,
	InputAttachment,
	ShadingRate,
	FragmentDensityMap,
	All,
	Transfer,
	RWAttachment,
};
uint16  operator | (EImageUsage lhs, EImageUsage rhs);
uint16  operator | (uint16 lhs, EImageUsage rhs);
uint16  operator | (EImageUsage lhs, uint16 rhs);

enum class EImageOpt : uint32
{
	BlitSrc,
	BlitDst,
	CubeCompatible,
	MutableFormat,
	Array2DCompatible,
	BlockTexelViewCompatible,
	SparseResidency,
	SparseAliased,
	Alias,
	SampleLocationsCompatible,
	StorageAtomic,
	ColorAttachmentBlend,
	SampledLinear,
	SampledMinMax,
	VertexPplnStore,
	FragmentPplnStore,
	LossyRTCompression,
	ExtendedUsage,
	Subsampled,
	All,
	SparseResidencyAliased,
	Blit,
};
uint32  operator | (EImageOpt lhs, EImageOpt rhs);
uint32  operator | (uint32 lhs, EImageOpt rhs);
uint32  operator | (EImageOpt lhs, uint32 rhs);

enum class EBufferUsage : uint32
{
	TransferSrc,
	TransferDst,
	UniformTexel,
	StorageTexel,
	Uniform,
	Storage,
	Index,
	Vertex,
	Indirect,
	ShaderAddress,
	ShaderBindingTable,
	ASBuild_ReadOnly,
	ASBuild_Scratch,
	RTAS_Storage,
	All,
	Transfer,
};
uint32  operator | (EBufferUsage lhs, EBufferUsage rhs);
uint32  operator | (uint32 lhs, EBufferUsage rhs);
uint32  operator | (EBufferUsage lhs, uint32 rhs);

enum class EBufferOpt : uint32
{
	SparseResidency,
	SparseAliased,
	VertexPplnStore,
	FragmentPplnStore,
	StorageTexelAtomic,
	All,
	SparseResidencyAliased,
};
uint32  operator | (EBufferOpt lhs, EBufferOpt rhs);
uint32  operator | (uint32 lhs, EBufferOpt rhs);
uint32  operator | (EBufferOpt lhs, uint32 rhs);

enum class EShadingRate : uint8
{
	Size1x1,
	Size1x2,
	Size1x4,
	Size2x1,
	Size2x2,
	Size2x4,
	Size4x1,
	Size4x2,
	Size4x4,
};
uint8  operator | (EShadingRate lhs, EShadingRate rhs);
uint8  operator | (uint8 lhs, EShadingRate rhs);
uint8  operator | (EShadingRate lhs, uint8 rhs);

enum class EShadingRateCombinerOp : uint8
{

	// S - original rate
	// D - new rate

	// S
	Keep,

	// D
	Replace,

	// min( S, D )
	Min,

	// max( S, D )
	Max,

	// S + D  -- check 'fragmentShadingRateStrictMultiplyCombiner' feature
	Sum,

	// S * D  -- check 'fragmentShadingRateStrictMultiplyCombiner' feature
	Mul,
};
uint8  operator | (EShadingRateCombinerOp lhs, EShadingRateCombinerOp rhs);
uint8  operator | (uint8 lhs, EShadingRateCombinerOp rhs);
uint8  operator | (EShadingRateCombinerOp lhs, uint8 rhs);

enum class EIntegerDotProductFeat : uint8
{
	Unsigned8bit,
	Signed8bit,
	MixedSignedness8bit,
	Unsigned4x8bit,
	Signed4x8bit,
	MixedSignedness4x8bit,
	Unsigned16bit,
	Signed16bit,
	MixedSignedness16bit,
	Unsigned32bit,
	Signed32bit,
	MixedSignedness32bit,
	Unsigned64bit,
	Signed64bit,
	MixedSignedness64bit,
	AccSat_Unsigned8bit,
	AccSat_Signed8bit,
	AccSat_MixedSignedness8bit,
	AccSat_Unsigned4x8bit,
	AccSat_Signed4x8bit,
	AccSat_MixedSignedness4x8bit,
	AccSat_Unsigned16bit,
	AccSat_Signed16bit,
	AccSat_MixedSignedness16bit,
	AccSat_Unsigned32bit,
	AccSat_Signed32bit,
	AccSat_MixedSignedness32bit,
	AccSat_Unsigned64bit,
	AccSat_Signed64bit,
	AccSat_MixedSignedness64bit,
};
uint8  operator | (EIntegerDotProductFeat lhs, EIntegerDotProductFeat rhs);
uint8  operator | (uint8 lhs, EIntegerDotProductFeat rhs);
uint8  operator | (EIntegerDotProductFeat lhs, uint8 rhs);

enum class ECoopMatrixComponentType : uint8
{
	Float16,
	Float32,
	Float64,
	BFloat16,
	Float8_E4M3,
	Float8_E5M2,
	SInt8,
	SInt16,
	SInt32,
	SInt64,
	SInt8x4,
	UInt8,
	UInt16,
	UInt32,
	UInt64,
	UInt8x4,
};
uint8  operator | (ECoopMatrixComponentType lhs, ECoopMatrixComponentType rhs);
uint8  operator | (uint8 lhs, ECoopMatrixComponentType rhs);
uint8  operator | (ECoopMatrixComponentType lhs, uint8 rhs);

enum class ECoopVecMatrixLayout : uint8
{
	RowMajor,
	ColumnMajor,
	InferencingOptimal,
	TrainingOptimal,
};
uint8  operator | (ECoopVecMatrixLayout lhs, ECoopVecMatrixLayout rhs);
uint8  operator | (uint8 lhs, ECoopVecMatrixLayout rhs);
uint8  operator | (ECoopVecMatrixLayout lhs, uint8 rhs);

enum class ECoopMatrixCfg : uint8
{
	Afp16_Bfp16_Cfp16_Rfp16_M16_N16_K16,
	Afp16_Bfp16_Cfp32_Rfp32_M16_N16_K16,
	Afp16_Bfp16_Cfp32_Rfp32_M8_N8_K16,
	Au8_Bu8_Cu32_Ru32_M16_N16_K32,
	As8_Bs8_Cs32_Rs32_M16_N16_K32,
	Au8_Bu8_Cu32_Ru32_M8_N8_K32,
	As8_Bs8_Cs32_Rs32_M8_N8_K32,
};
uint8  operator | (ECoopMatrixCfg lhs, ECoopMatrixCfg rhs);
uint8  operator | (uint8 lhs, ECoopMatrixCfg rhs);
uint8  operator | (ECoopMatrixCfg lhs, uint8 rhs);

enum class ECoopVecCfg : uint8
{
	Tfp16_Ifp16_Mfp16_Bfp16_Rfp16_Tp,
	Tfp16_Ifp8e4m3_Mfp8e4m3_Bfp16_Rfp16,
	Tfp16_Ifp8e5m2_Mfp8e5m2_Bfp16_Rfp16,
	Ts8_Is8_Ms8_Bs32_Rs32,
};
uint8  operator | (ECoopVecCfg lhs, ECoopVecCfg rhs);
uint8  operator | (uint8 lhs, ECoopVecCfg rhs);
uint8  operator | (ECoopVecCfg lhs, uint8 rhs);

enum class EColorSpace : uint8
{
	sRGB_nonlinear,
	BT709_nonlinear,
	Extended_sRGB_linear,
	HDR10_ST2084,
	BT2020_linear,
};
uint8  operator | (EColorSpace lhs, EColorSpace rhs);
uint8  operator | (uint8 lhs, EColorSpace rhs);
uint8  operator | (EColorSpace lhs, uint8 rhs);

enum class ERenderLayer : uint32
{
	Opaque,
	Translucent,
	PostProcess,
};
uint32  operator | (ERenderLayer lhs, ERenderLayer rhs);
uint32  operator | (uint32 lhs, ERenderLayer rhs);
uint32  operator | (ERenderLayer lhs, uint32 rhs);

enum class DbgViewFlags : uint32
{
	NoCopy,
	Copy,
	Histogram,
	LinearDepth,
	Stencil,
};
uint32  operator | (DbgViewFlags lhs, DbgViewFlags rhs);
uint32  operator | (uint32 lhs, DbgViewFlags rhs);
uint32  operator | (DbgViewFlags lhs, uint32 rhs);

enum class ScriptFlags : uint32
{
	RunOnce,
	OnRequest,
	RunOnce_AfterLoading,
};
uint32  operator | (ScriptFlags lhs, ScriptFlags rhs);
uint32  operator | (uint32 lhs, ScriptFlags rhs);
uint32  operator | (ScriptFlags lhs, uint32 rhs);

enum class ImageLoadOpFlags : uint32
{

	// Generate mipmaps after loading
	GenMipmaps,
};
uint32  operator | (ImageLoadOpFlags lhs, ImageLoadOpFlags rhs);
uint32  operator | (uint32 lhs, ImageLoadOpFlags rhs);
uint32  operator | (ImageLoadOpFlags lhs, uint32 rhs);

enum class EPostprocess : uint32
{

	// Entry point: 'Main'
	None,

	// Entry point: 'void mainImage (out float4 fragColor, in float2 fragCoord)'
	Shadertoy,

	// Entry point: 'void mainCubemap (out float4 fragColor, in float2 fragCoord, in float3 rayOri, in float3 rayDir)'
	ShadertoyCubemap,

	// Entry point: 'void mainVR (out float4 fragColor, in float2 fragCoord, in float3 fragRayOri, in float3 fragRayDir)'
	ShadertoyVR,
	ShadertoyVR_180,
	ShadertoyVR_360,
	Shadertoy_360,
};
uint32  operator | (EPostprocess lhs, EPostprocess rhs);
uint32  operator | (uint32 lhs, EPostprocess rhs);
uint32  operator | (EPostprocess lhs, uint32 rhs);

enum class EPassFlags : uint8
{
	None,

	// ShaderTrace - record all variables, function result, etc and save it to file.
	// It is very useful to debug shaders. In UI select 'Debugging' menu, select pass,'Trace' and shader stage then click 'G' key to record trace for pixel under cursor.
	// Reference to the last recorded trace will be added to console and IDE log, click on it to open file.
	Enable_ShaderTrace,

	// ShaderFunctionProfiling - record time of user function calls, then sort results and save to file.
	Enable_ShaderFnProf,

	// Enable all debug features.
	Enable_AllShaderDbg,

	// Compile shader using Slang compiler.
	UseSLang,
};
uint8  operator | (EPassFlags lhs, EPassFlags rhs);
uint8  operator | (uint8 lhs, EPassFlags rhs);
uint8  operator | (EPassFlags lhs, uint8 rhs);

enum class EFormatFeature : uint32
{
	StorageImageAtomic,
	StorageImage,
	AttachmentBlend,
	Attachment,
	LinearSampled,
	UniformTexelBuffer,
	StorageTexelBuffer,
	StorageTexelBufferAtomic,
	HWCompressedAttachment,
	LossyCompressedAttachment,
};
uint32  operator | (EFormatFeature lhs, EFormatFeature rhs);
uint32  operator | (uint32 lhs, EFormatFeature rhs);
uint32  operator | (EFormatFeature lhs, uint32 rhs);

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
const string Sampler_NearestClampSubsampled;
const string Sampler_MaxLinearClamp;
const string Sampler_MinLinearClamp;
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
int  SafeDiv (int x, int y, int defVal);
int  DivCeil (int x, int y);
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
uint  SafeDiv (uint x, uint y, uint defVal);
uint  DivCeil (uint x, uint y);
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
	float4 (const Quat & quat);
	float4 (const RGBA32f & x);
};

bool  All (const bool2 & x);
bool  Any (const bool2 & x);
bool  All (const bool3 & x);
bool  Any (const bool3 & x);
bool  All (const bool4 & x);
bool  Any (const bool4 & x);
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
sbyte2  DivCeil (const sbyte2 & x, const sbyte2 & y);
sbyte2  DivCeil (const sbyte2 & x, int8 y);
sbyte3  DivCeil (const sbyte3 & x, const sbyte3 & y);
sbyte3  DivCeil (const sbyte3 & x, int8 y);
sbyte4  DivCeil (const sbyte4 & x, const sbyte4 & y);
sbyte4  DivCeil (const sbyte4 & x, int8 y);
ubyte2  DivCeil (const ubyte2 & x, const ubyte2 & y);
ubyte2  DivCeil (const ubyte2 & x, uint8 y);
ubyte3  DivCeil (const ubyte3 & x, const ubyte3 & y);
ubyte3  DivCeil (const ubyte3 & x, uint8 y);
ubyte4  DivCeil (const ubyte4 & x, const ubyte4 & y);
ubyte4  DivCeil (const ubyte4 & x, uint8 y);
short2  DivCeil (const short2 & x, const short2 & y);
short2  DivCeil (const short2 & x, int16 y);
short3  DivCeil (const short3 & x, const short3 & y);
short3  DivCeil (const short3 & x, int16 y);
short4  DivCeil (const short4 & x, const short4 & y);
short4  DivCeil (const short4 & x, int16 y);
ushort2  DivCeil (const ushort2 & x, const ushort2 & y);
ushort2  DivCeil (const ushort2 & x, uint16 y);
ushort3  DivCeil (const ushort3 & x, const ushort3 & y);
ushort3  DivCeil (const ushort3 & x, uint16 y);
ushort4  DivCeil (const ushort4 & x, const ushort4 & y);
ushort4  DivCeil (const ushort4 & x, uint16 y);
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
int2  DivCeil (const int2 & x, const int2 & y);
int2  DivCeil (const int2 & x, int y);
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
int3  DivCeil (const int3 & x, const int3 & y);
int3  DivCeil (const int3 & x, int y);
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
int4  DivCeil (const int4 & x, const int4 & y);
int4  DivCeil (const int4 & x, int y);
uint  Sum (const uint2 & x);
uint  Area (const uint2 & x);
uint2  Min (const uint2 & x, const uint2 & y);
uint2  Max (const uint2 & x, const uint2 & y);
uint2  Clamp (const uint2 & val, const uint2 & min, const uint2 & max);
uint2  Clamp (const uint2 & val, uint min, uint max);
uint2  Wrap (const uint2 & val, const uint2 & min, const uint2 & max);
uint2  Wrap (const uint2 & val, uint min, uint max);
uint  VecToLinear (const uint2 & pos, const uint2 & dim);
uint2  DivCeil (const uint2 & x, const uint2 & y);
uint2  DivCeil (const uint2 & x, uint y);
uint  Sum (const uint3 & x);
uint  Area (const uint3 & x);
uint3  Min (const uint3 & x, const uint3 & y);
uint3  Max (const uint3 & x, const uint3 & y);
uint3  Clamp (const uint3 & val, const uint3 & min, const uint3 & max);
uint3  Clamp (const uint3 & val, uint min, uint max);
uint3  Wrap (const uint3 & val, const uint3 & min, const uint3 & max);
uint3  Wrap (const uint3 & val, uint min, uint max);
uint  VecToLinear (const uint3 & pos, const uint3 & dim);
uint3  DivCeil (const uint3 & x, const uint3 & y);
uint3  DivCeil (const uint3 & x, uint y);
uint  Sum (const uint4 & x);
uint  Area (const uint4 & x);
uint4  Min (const uint4 & x, const uint4 & y);
uint4  Max (const uint4 & x, const uint4 & y);
uint4  Clamp (const uint4 & val, const uint4 & min, const uint4 & max);
uint4  Clamp (const uint4 & val, uint min, uint max);
uint4  Wrap (const uint4 & val, const uint4 & min, const uint4 & max);
uint4  Wrap (const uint4 & val, uint min, uint max);
uint  VecToLinear (const uint4 & pos, const uint4 & dim);
uint4  DivCeil (const uint4 & x, const uint4 & y);
uint4  DivCeil (const uint4 & x, uint y);
struct Quat
{
	Quat ();
	Quat (const Quat&);
	Quat&  operator = (const Quat&);
	Quat (float w, float x, float y, float z);
	float x;
	float y;
	float z;
	float w;
	Quat  RotateX (float angle) const;
	Quat  RotateY (float angle) const;
	Quat  RotateZ (float angle) const;
	Quat  Rotate (float angle, const float3 & axis) const;
	Quat  Rotate (const float3 & angles) const;
	Quat  Rotate2 (const float3 & angles) const;
	Quat  Rotate (float angleX, float angleY, float angleZ) const;
	Quat  LookAt (const float3 & dir, const float3 & up) const;
	Quat  From2Normals (const float3 & norm1, const float3 & norm2) const;
	Quat  FromAngleAxis (float angle, const float3 & axis) const;
};

struct RectI
{
	RectI ();
	RectI (const RectI&);
	RectI&  operator = (const RectI&);
	int left;
	int top;
	int right;
	int bottom;
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
	float4x4  InfinitePerspective (float fovY, float aspectRatio, float zNear) const;
	float4x4  Perspective (float fovY, float aspectRatio, const float2 & range) const;
	float4x4  Perspective (float fovY, const float2 & viewport, const float2 & range) const;
	float4x4  Frustum (const RectF & viewport, const float2 & range) const;
	float4x4  InfiniteFrustum (const RectF & viewport, float zNear) const;
	float4x4  ReverseZTransform () const;
	float4x4  Translate (const float3 & translation) const;
	float4x4  Scale (const float3 & scale) const;
	float4x4  Scale (float scale) const;
	float4  Project (const float3 & pos, const RectF & viewport) const;
	float3  UnProject (const float3 & pos, const RectF & viewport) const;
	float4x4  LookAt (const float3 & eye, const float3 & center, const float3 & up) const;
};

RGBA32f  Lerp (const RGBA32f & x, const RGBA32f & y, float factor);
RGBA32f  AdjustContrast (const RGBA32f & col, float factor);
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
	float  Luminance () const;
	RGBA32f &  OpaqueBlack ();
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
RGBA8u  AdjustSaturation (const RGBA8u & col, float factor);
RGBA8u  Lerp (const RGBA8u & x, const RGBA8u & y, float factor);
RGBA8u &  OpaqueBlack (RGBA8u &);
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
	float  Luminance () const;
	uint  ToUInt () const;
	void  FromUintARGB (uint);
	void  FromUint (uint);
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

uint  IHash (int);
uint  IHash (uint);
uint  IHash (float);
uint2  IHash (const int2 &);
uint2  IHash (const uint2 &);
uint2  IHash (const float2 &);
uint3  IHash (const int3 &);
uint3  IHash (const uint3 &);
uint3  IHash (const float3 &);
uint4  IHash (const int4 &);
uint4  IHash (const uint4 &);
uint4  IHash (const float4 &);
uint  IHash1 (const int2 &);
uint  IHash1 (const uint2 &);
uint  IHash1 (const float2 &);
uint  IHash1 (const int3 &);
uint  IHash1 (const uint3 &);
uint  IHash1 (const float3 &);
uint  IHash1 (const int4 &);
uint  IHash1 (const uint4 &);
uint  IHash1 (const float4 &);
float  FHash (int);
float  FHash (uint);
float  FHash (float);
float2  FHash (const int2 &);
float2  FHash (const uint2 &);
float2  FHash (const float2 &);
float3  FHash (const int3 &);
float3  FHash (const uint3 &);
float3  FHash (const float3 &);
float4  FHash (const int4 &);
float4  FHash (const uint4 &);
float4  FHash (const float4 &);
float  FHash1 (const int2 &);
float  FHash1 (const uint2 &);
float  FHash1 (const float2 &);
float  FHash1 (const int3 &);
float  FHash1 (const uint3 &);
float  FHash1 (const float3 &);
float  FHash1 (const int4 &);
float  FHash1 (const uint4 &);
float  FHash1 (const float4 &);
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

struct DynamicUInt
{
	DynamicUInt ();
	DynamicUInt (uint);
	RC<DynamicUInt>  Mul (uint);
	RC<DynamicUInt>  Div (uint);
	RC<DynamicUInt>  DivNear (uint);
	RC<DynamicUInt>  DivCeil (uint);
	RC<DynamicUInt>  Add (uint);
	RC<DynamicUInt>  Sub (uint);
	RC<DynamicUInt>  Pow (uint);
	RC<DynamicUInt>  Exp2 ();
	RC<DynamicUInt>  Exp2 (uint scale);
	RC<DynamicUInt>  Min (uint);
	RC<DynamicUInt>  Max (uint);
	RC<DynamicUInt>  FloorPOT ();
	RC<DynamicUInt>  CeilPOT ();
	RC<DynamicUInt>  NearPOT ();
	RC<DynamicUInt>  Mul (const RC<DynamicUInt> &);
	RC<DynamicUInt>  Div (const RC<DynamicUInt> &);
	RC<DynamicUInt>  Add (const RC<DynamicUInt> &);
	RC<DynamicUInt>  Sub (const RC<DynamicUInt> &);
	RC<DynamicUInt>  Min (const RC<DynamicUInt> &);
	RC<DynamicUInt>  Max (const RC<DynamicUInt> &);
	RC<DynamicUInt2>  X1 ();
	RC<DynamicUInt2>  XX ();
	RC<DynamicUInt3>  X11 ();
	RC<DynamicUInt3>  XXX ();
	RC<DynamicFloat>  ToFloat ();
	RC<DynamicFloat>  AsFloat ();
	RC<DynamicFloat>  Percent (const RC<DynamicUInt> &);
	RC<DynamicDim>  Dimension2 ();
	RC<DynamicDim>  Dimension3 ();
};

struct DynamicUInt2
{
	DynamicUInt2 ();
	DynamicUInt2 (const uint2 &);
	RC<DynamicUInt>  X () const;
	RC<DynamicUInt>  Y () const;
	RC<DynamicUInt2>  Exp2 ();
	RC<DynamicUInt2>  Mul (const uint2 &);
	RC<DynamicUInt2>  Div (const uint2 &);
	RC<DynamicUInt2>  DivNear (const uint2 &);
	RC<DynamicUInt2>  DivCeil (const uint2 &);
	RC<DynamicUInt2>  Add (const uint2 &);
	RC<DynamicUInt2>  Sub (const uint2 &);
	RC<DynamicUInt2>  Pow (const uint2 &);
	RC<DynamicUInt2>  Min (const uint2 &);
	RC<DynamicUInt2>  Max (const uint2 &);
	RC<DynamicUInt2>  FloorPOT ();
	RC<DynamicUInt2>  CeilPOT ();
	RC<DynamicUInt2>  NearPOT ();
	RC<DynamicUInt2>  Mul (const RC<DynamicUInt2> &);
	RC<DynamicUInt2>  Div (const RC<DynamicUInt2> &);
	RC<DynamicUInt2>  Add (const RC<DynamicUInt2> &);
	RC<DynamicUInt2>  Sub (const RC<DynamicUInt2> &);
	RC<DynamicUInt2>  Min (const RC<DynamicUInt2> &);
	RC<DynamicUInt2>  Max (const RC<DynamicUInt2> &);
	RC<DynamicUInt2>  DivNear (const RC<DynamicUInt2> &);
	RC<DynamicUInt2>  DivCeil (const RC<DynamicUInt2> &);
	RC<DynamicUInt>  Area () const;
	RC<DynamicDim>  Dimension ();
};

struct DynamicUInt3
{
	DynamicUInt3 ();
	DynamicUInt3 (const uint3 &);
	RC<DynamicUInt>  X () const;
	RC<DynamicUInt>  Y () const;
	RC<DynamicUInt>  Z () const;
	RC<DynamicUInt2>  XY () const;
	RC<DynamicUInt3>  Mul (const uint3 &);
	RC<DynamicUInt3>  Div (const uint3 &);
	RC<DynamicUInt3>  DivNear (const uint3 &);
	RC<DynamicUInt3>  DivCeil (const uint3 &);
	RC<DynamicUInt3>  Add (const uint3 &);
	RC<DynamicUInt3>  Sub (const uint3 &);
	RC<DynamicUInt3>  Pow (const uint3 &);
	RC<DynamicUInt3>  Min (const uint3 &);
	RC<DynamicUInt3>  Max (const uint3 &);
	RC<DynamicUInt3>  FloorPOT ();
	RC<DynamicUInt3>  CeilPOT ();
	RC<DynamicUInt3>  NearPOT ();
	RC<DynamicUInt3>  Mul (const RC<DynamicUInt3> &);
	RC<DynamicUInt3>  Div (const RC<DynamicUInt3> &);
	RC<DynamicUInt3>  Add (const RC<DynamicUInt3> &);
	RC<DynamicUInt3>  Sub (const RC<DynamicUInt3> &);
	RC<DynamicUInt3>  Min (const RC<DynamicUInt3> &);
	RC<DynamicUInt3>  Max (const RC<DynamicUInt3> &);
	RC<DynamicUInt3>  DivNear (const RC<DynamicUInt3> &);
	RC<DynamicUInt3>  DivCeil (const RC<DynamicUInt3> &);
	RC<DynamicUInt>  Volume () const;
	RC<DynamicDim>  Dimension ();
};

struct DynamicUInt4
{
	DynamicUInt4 ();
	DynamicUInt4 (const uint4 &);
	RC<DynamicUInt>  X () const;
	RC<DynamicUInt>  Y () const;
	RC<DynamicUInt>  Z () const;
	RC<DynamicUInt>  W () const;
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
	RC<DynamicInt>  X () const;
	RC<DynamicInt>  Y () const;
};

struct DynamicInt3
{
	DynamicInt3 ();
	DynamicInt3 (const int3 &);
	RC<DynamicInt>  X () const;
	RC<DynamicInt>  Y () const;
	RC<DynamicInt>  Z () const;
};

struct DynamicInt4
{
	DynamicInt4 ();
	DynamicInt4 (const int4 &);
	RC<DynamicInt>  X () const;
	RC<DynamicInt>  Y () const;
	RC<DynamicInt>  Z () const;
	RC<DynamicInt>  W () const;
};

struct DynamicFloat
{
	DynamicFloat ();
	DynamicFloat (float);
	RC<DynamicFloat>  Mul (float);
	RC<DynamicFloat>  Div (float);
	RC<DynamicFloat>  DivNear (float);
	RC<DynamicFloat>  DivCeil (float);
	RC<DynamicFloat>  Add (float);
	RC<DynamicFloat>  Sub (float);
	RC<DynamicFloat>  Pow (float);
	RC<DynamicFloat>  Exp2 ();
	RC<DynamicFloat>  Exp2 (float scale);
	RC<DynamicFloat>  Min (float);
	RC<DynamicFloat>  Max (float);
	RC<DynamicFloat>  Mul (const RC<DynamicFloat> &);
	RC<DynamicFloat>  Div (const RC<DynamicFloat> &);
	RC<DynamicFloat>  Add (const RC<DynamicFloat> &);
	RC<DynamicFloat>  Sub (const RC<DynamicFloat> &);
	RC<DynamicFloat>  Min (const RC<DynamicFloat> &);
	RC<DynamicFloat>  Max (const RC<DynamicFloat> &);
	RC<DynamicUInt>  AsUInt ();
	RC<DynamicUInt>  ToUInt ();
	RC<DynamicInt>  ToInt ();
};

struct DynamicFloat2
{
	DynamicFloat2 ();
	DynamicFloat2 (const float2 &);
	RC<DynamicFloat>  X () const;
	RC<DynamicFloat>  Y () const;
};

struct DynamicFloat3
{
	DynamicFloat3 ();
	DynamicFloat3 (const float3 &);
	RC<DynamicFloat>  X () const;
	RC<DynamicFloat>  Y () const;
	RC<DynamicFloat>  Z () const;
};

struct DynamicFloat4
{
	DynamicFloat4 ();
	DynamicFloat4 (const float4 &);
	RC<DynamicFloat>  X () const;
	RC<DynamicFloat>  Y () const;
	RC<DynamicFloat>  Z () const;
	RC<DynamicFloat>  W () const;
};

struct DynamicULong
{
	DynamicULong ();
	DynamicULong (uint64);
};

struct DynamicDim
{
	DynamicDim (uint3);
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
	RC<DynamicDim>  FloorPOT () const;
	RC<DynamicUInt>  X () const;
	RC<DynamicUInt>  Y () const;
	RC<DynamicUInt2>  XY () const;
	RC<DynamicUInt>  Area () const;
	RC<DynamicUInt>  Volume () const;
	RC<DynamicFloat2>  ToFloat2 () const;
	RC<DynamicFloat2>  Inverse () const;
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
	void  AddOption (EImageOpt);
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
	// Layout will be '{ <arrayElementTypeName>  elements [<count>]; }'.
	// 'arrayElementTypeName' must be previously declared or one of built-in type:
	// 	'DispatchIndirectCommand', 'DrawIndirectCommand', 'DrawIndexedIndirectCommand',
	// 	'DrawMeshTasksIndirectCommand', 'TraceRayIndirectCommand', 'ASBuildIndirectCommand'
	// 	'AccelStructInstance'.
	// Buffer typename: '<arrayElementTypeName>_Array'.
	void  ArrayLayout (const string & arrayElementTypeName, uint count);
	void  ArrayLayout (const string & arrayElementTypeName, const RC<DynamicUInt> & count);

	// Allow to declare array of struct with constant or dynamic size.
	// Created a new structure with type 'arrayElementTypeName' and fields in 'arrayElementSource'.
	// See field declaration rules for 'ShaderStructType::Set()' method in [pipeline_compiler.as](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/scripts/pipeline_compiler.as).
	// Buffer typename: '<arrayElementTypeName>_Array'.
	void  ArrayLayout (const string & arrayElementTypeName, const string & arrayElementSource, uint count);
	void  ArrayLayout (const string & arrayElementTypeName, const string & arrayElementSource, const RC<DynamicUInt> & count);

	// Buffer typename: '<arrayElementTypeName>_Array2'.
	void  ArrayLayout (const string & arrayElementTypeName, const string & arrayElementSource, const string & staticSource, uint count);
	void  ArrayLayout (const string & arrayElementTypeName, const string & arrayElementSource, const string & staticSource, const RC<DynamicUInt> & count);

	// Allow to declare single structure as a buffer layout.
	// 'typeName' must be previously declared or one of built-in type (see 'ArrayLayout').
	void  UseLayout (const string & typeName);

	// Created a new structure with type 'typeName' and fields in 'source'.
	// See field declaration rules for 'ShaderStructType::Set()' method in [pipeline_compiler.as](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/scripts/pipeline_compiler.as).
	void  UseLayout (const string & typeName, const string & source);

	// Returns buffer device address.
	// Requires 'GL_EXT_buffer_reference' extension in GLSL.
	// It passed as 'uint64' type so you should cast it to buffer reference type.
	uint64  DeviceAddress ();

	// Force enable buffer content history.
	// It store copy of the buffer content on last N frames.
	void  EnableHistory ();

	// Call this method if 'DeviceAddress()' of another buffer is used in current buffer to avoid missed synchronizations.
	void  AddReference (const RC<Buffer> &);

	// Dynamic array size, can be used for draw call.
	RC<DynamicUInt>  ArraySize () const;

	// Constant array size, can be used for draw call.
	uint  ConstArraySize () const;

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
	// In trace ray call: 'if (cullMask_argument & instance_cullMask) != 0' then instance is visible.
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
	void  AddInstance (const RC<RTGeometry> & rtGeometry);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceMask & mask);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceSBTOffset & sbtOffset);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceCustomIndex & customIndex);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceCustomIndex & customIndex, const RTInstanceMask & mask);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceCustomIndex & customIndex, const RTInstanceSBTOffset & sbtOffset);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceCustomIndex & customIndex, const RTInstanceMask & mask, const RTInstanceSBTOffset & sbtOffset);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, ERTInstanceOpt options);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceMask & mask, ERTInstanceOpt options);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceSBTOffset & sbtOffset, ERTInstanceOpt options);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceCustomIndex & customIndex, ERTInstanceOpt options);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceCustomIndex & customIndex, const RTInstanceMask & mask, ERTInstanceOpt options);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceCustomIndex & customIndex, const RTInstanceSBTOffset & sbtOffset, ERTInstanceOpt options);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceCustomIndex & customIndex, const RTInstanceMask & mask, const RTInstanceSBTOffset & sbtOffset, ERTInstanceOpt options);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const float3 & position);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const float3 & position, const RTInstanceMask & mask);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const float3 & position, const RTInstanceSBTOffset & sbtOffset);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const float3 & position, const RTInstanceCustomIndex & customIndex);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const float3 & position, const RTInstanceCustomIndex & customIndex, const RTInstanceMask & mask);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const float3 & position, const RTInstanceCustomIndex & customIndex, const RTInstanceSBTOffset & sbtOffset);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const float3 & position, const RTInstanceCustomIndex & customIndex, const RTInstanceMask & mask, const RTInstanceSBTOffset & sbtOffset);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceTransform & transform);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceTransform & transform, const RTInstanceMask & mask);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceTransform & transform, const RTInstanceSBTOffset & sbtOffset);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceTransform & transform, const RTInstanceCustomIndex & customIndex);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceTransform & transform, const RTInstanceCustomIndex & customIndex, const RTInstanceMask & mask);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceTransform & transform, const RTInstanceCustomIndex & customIndex, const RTInstanceSBTOffset & sbtOffset);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceTransform & transform, const RTInstanceCustomIndex & customIndex, const RTInstanceMask & mask, const RTInstanceSBTOffset & sbtOffset);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceTransform & transform, ERTInstanceOpt options);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceTransform & transform, const RTInstanceMask & mask, ERTInstanceOpt options);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceTransform & transform, const RTInstanceSBTOffset & sbtOffset, ERTInstanceOpt options);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceTransform & transform, const RTInstanceCustomIndex & customIndex, ERTInstanceOpt options);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceTransform & transform, const RTInstanceCustomIndex & customIndex, const RTInstanceMask & mask, ERTInstanceOpt options);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceTransform & transform, const RTInstanceCustomIndex & customIndex, const RTInstanceSBTOffset & sbtOffset, ERTInstanceOpt options);
	void  AddInstance (const RC<RTGeometry> & rtGeometry, const RTInstanceTransform & transform, const RTInstanceCustomIndex & customIndex, const RTInstanceMask & mask, const RTInstanceSBTOffset & sbtOffset, ERTInstanceOpt options);

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
	void  InstanceCount (uint);
};

struct VertexStride
{
	VertexStride ();
	VertexStride (const VertexStride&);
	VertexStride&  operator = (const VertexStride&);
	VertexStride (uint);
};

struct VertexAttribDivisor
{
	VertexAttribDivisor ();
	VertexAttribDivisor (const VertexAttribDivisor&);
	VertexAttribDivisor&  operator = (const VertexAttribDivisor&);
	VertexAttribDivisor (uint);
};

struct UnifiedGeometry_Draw
{
	UnifiedGeometry_Draw ();
	UnifiedGeometry_Draw (const UnifiedGeometry_Draw&);
	UnifiedGeometry_Draw&  operator = (const UnifiedGeometry_Draw&);
	void  VertexCount (const RC<DynamicUInt> &);
	void  InstanceCount (const RC<DynamicUInt> &);

	// Pattern to choose pipeline if found multiple variants.
	void  PipelineHint (const string &);
	uint vertexCount;
	uint instanceCount;
	uint firstVertex;
	uint firstInstance;
	ERenderLayer layer;
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

	// Pattern to choose pipeline if found multiple variants.
	void  PipelineHint (const string &);
	uint indexCount;
	uint instanceCount;
	uint firstIndex;
	int vertexOffset;
	uint firstInstance;
	ERenderLayer layer;
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

	// Pattern to choose pipeline if found multiple variants.
	void  PipelineHint (const string &);

	// Stride must be at least 16 bytes and multiple of 4.
	uint stride;
	uint drawCount;
	ERenderLayer layer;
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

	// Pattern to choose pipeline if found multiple variants.
	void  PipelineHint (const string &);

	// Stride must be at least 20 bytes and multiple of 4.
	uint stride;
	uint drawCount;
	ERenderLayer layer;
};

struct UnifiedGeometry_DrawMeshTasks
{
	UnifiedGeometry_DrawMeshTasks ();
	UnifiedGeometry_DrawMeshTasks (const UnifiedGeometry_DrawMeshTasks&);
	UnifiedGeometry_DrawMeshTasks&  operator = (const UnifiedGeometry_DrawMeshTasks&);
	void  TaskCount (const RC<DynamicUInt3> &);
	void  TaskCount (const RC<DynamicUInt> &);
	uint3 taskCount;
	ERenderLayer layer;
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

	// Pattern to choose pipeline if found multiple variants.
	void  PipelineHint (const string &);

	// Stride must be at least 12 bytes and multiple of 4.
	uint stride;
	uint drawCount;
	ERenderLayer layer;
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

	// Pattern to choose pipeline if found multiple variants.
	void  PipelineHint (const string &);

	// Stride must be at least 16 bytes and multiple of 4.
	uint stride;
	uint maxDrawCount;
	ERenderLayer layer;
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

	// Pattern to choose pipeline if found multiple variants.
	void  PipelineHint (const string &);

	// Stride must be at least 20 bytes and multiple of 4.
	uint stride;
	uint maxDrawCount;
	ERenderLayer layer;
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

	// Pattern to choose pipeline if found multiple variants.
	void  PipelineHint (const string &);

	// Stride must be at least 12 bytes and multiple of 4.
	uint stride;
	uint maxDrawCount;
	ERenderLayer layer;
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
	void  VertexBuffer (const string & attrib, EVertexType type, const RC<Buffer> & buffer);
	void  VertexBuffer (const string & attrib, EVertexType type, const RC<Buffer> & buffer, uint bufferOffset);
	void  VertexBuffer (const string & attrib, EVertexType type, const VertexStride & stride, const RC<Buffer> & buffer);
	void  VertexBuffer (const string & attrib, EVertexType type, const VertexStride & stride, const RC<Buffer> & buffer, uint bufferOffset);
	void  VertexBuffer (const string & attrib, EVertexType type, const RC<Buffer> & buffer, const VertexAttribDivisor & divisor);
	void  VertexBuffer (const string & attrib, EVertexType type, const RC<Buffer> & buffer, uint bufferOffset, const VertexAttribDivisor & divisor);
	void  VertexBuffer (const string & attrib, EVertexType type, const VertexStride & stride, const RC<Buffer> & buffer, const VertexAttribDivisor & divisor);
	void  VertexBuffer (const string & attrib, EVertexType type, const VertexStride & stride, const RC<Buffer> & buffer, uint bufferOffset, const VertexAttribDivisor & divisor);
};

struct Model
{
	Model (const string & scenePathInVFS);

	// Set resource name. It is used for debugging.
	void  Name (const string &);

	// Add directory where to search required textures.
	void  TextureSearchDir (const string & folder);

	// Set transformation for model root node.
	void  InitialTransform (const float4x4 &);
	void  InitialTransform (const float3 & position, const float3 & rotation, float scale);
	void  InstanceCount (uint);

	// Add light source.
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
	void  Add (const string & key, const array<Image@> & value);
	void  Add (const string & key, const array<Buffer@> & value);

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
	void  ImageArray (const string & key, array<Image@> & result) const;
	void  BufferArray (const string & key, array<Buffer@> & result) const;
};

struct Postprocess
{

	// Set debug label and color. It is used in graphics profiler.
	void  SetDebugLabel (const string & label);
	void  SetDebugLabel (const string & label, const RGBA8u & color);
	void  SetDebugLabel (const string & label, const RGBA32f & color);
	void  AddFlag (EPassFlags);

	// Measure GPU time for this pass. If pass executed multiple times during 'Repeat()' then average time will be returned.
	void  MeasureTime (const RC<DynamicFloat> & timeInSeconds);

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

	// Value of 'dynamicValue' will be passed to all shaders in the current pass.
	// Value of 'dynamicValue' is constant for whole frame and also can be used in draw call or another pass.
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
	void  Constant (const string & name, const RC<DynamicDim> & dynamicValue);
	void  Constant (const string & name, float constValue);
	void  Constant (const string & name, const float2 & constValue);
	void  Constant (const string & name, const float3 & constValue);
	void  Constant (const string & name, const float4 & constValue);
	void  Constant (const string & name, int constValue);
	void  Constant (const string & name, const int2 & constValue);
	void  Constant (const string & name, const int3 & constValue);
	void  Constant (const string & name, const int4 & constValue);
	void  Constant (const string & name, uint constValue);
	void  Constant (const string & name, const uint2 & constValue);
	void  Constant (const string & name, const uint3 & constValue);
	void  Constant (const string & name, const uint4 & constValue);

	// Returns dynamic dimension of the pass.
	// It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches.
	RC<DynamicDim>  Dimension ();
	void  EnableIfEqual (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfLess (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfGreater (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfAnyBit (const RC<DynamicUInt> & dynamic, uint refValue);

	// Repeat pass multiple times.
	// Can be used for performance tests.
	void  Repeat (const RC<DynamicUInt> &);

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
	void  Output (const RC<Image> & image);
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
	void  Output (const RC<Image> & image, const RGBA32u & clearColor);
	void  Output (const RC<Image> & image, const MipmapLevel & mipmap, const RGBA32u & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const RGBA32u & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const RGBA32u & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const RGBA32u & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const RGBA32u & clearColor);
	void  Output (const RC<Image> & image, const RGBA32i & clearColor);
	void  Output (const RC<Image> & image, const MipmapLevel & mipmap, const RGBA32i & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const RGBA32i & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const RGBA32i & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const RGBA32i & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const RGBA32i & clearColor);
	void  Output (const RC<Image> & image, const DepthStencil & clearDepthStencil);
	void  Output (const RC<Image> & image, const MipmapLevel & mipmap, const DepthStencil & clearDepthStencil);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const DepthStencil & clearDepthStencil);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const DepthStencil & clearDepthStencil);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const DepthStencil & clearDepthStencil);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const DepthStencil & clearDepthStencil);

	// Add color/depth render target with explicit name.
	void  Output (const string & name, const RC<Image> & image);
	void  Output (const string & name, const RC<Image> & image, const MipmapLevel & mipmap);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap);
	void  Output (const string & name, const RC<Image> & image, const RGBA32f & clearColor);
	void  Output (const string & name, const RC<Image> & image, const MipmapLevel & mipmap, const RGBA32f & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const RGBA32f & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const RGBA32f & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const RGBA32f & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const RGBA32f & clearColor);
	void  Output (const string & name, const RC<Image> & image, const RGBA32u & clearColor);
	void  Output (const string & name, const RC<Image> & image, const MipmapLevel & mipmap, const RGBA32u & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const RGBA32u & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const RGBA32u & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const RGBA32u & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const RGBA32u & clearColor);
	void  Output (const string & name, const RC<Image> & image, const RGBA32i & clearColor);
	void  Output (const string & name, const RC<Image> & image, const MipmapLevel & mipmap, const RGBA32i & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const RGBA32i & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const RGBA32i & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const RGBA32i & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const RGBA32i & clearColor);
	void  Output (const string & name, const RC<Image> & image, const DepthStencil & clearDepthStencil);
	void  Output (const string & name, const RC<Image> & image, const MipmapLevel & mipmap, const DepthStencil & clearDepthStencil);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const DepthStencil & clearDepthStencil);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const DepthStencil & clearDepthStencil);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const DepthStencil & clearDepthStencil);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const DepthStencil & clearDepthStencil);
	void  OutputLS (const RC<Image> & image, EAttachmentLoadOp loadOp, EAttachmentStoreOp storeOp);
	void  OutputLS (const string & name, const RC<Image> & image, EAttachmentLoadOp loadOp, EAttachmentStoreOp storeOp);

	// Add color render target with blend operation.
	// Implicitly name will be 'out_Color' + index.
	void  OutputBlend (const RC<Image> & image, EBlendFactor src, EBlendFactor dst, EBlendOp op);
	void  OutputBlend (const RC<Image> & image, const MipmapLevel & mipmap, EBlendFactor src, EBlendFactor dst, EBlendOp op);
	void  OutputBlend (const RC<Image> & image, const ImageLayer & baseLayer, EBlendFactor src, EBlendFactor dst, EBlendOp op);
	void  OutputBlend (const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, EBlendFactor src, EBlendFactor dst, EBlendOp op);
	void  OutputBlend (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, EBlendFactor src, EBlendFactor dst, EBlendOp op);
	void  OutputBlend (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, EBlendFactor src, EBlendFactor dst, EBlendOp op);
	void  OutputBlend (const RC<Image> & image, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA);
	void  OutputBlend (const RC<Image> & image, const MipmapLevel & mipmap, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA);
	void  OutputBlend (const RC<Image> & image, const ImageLayer & baseLayer, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA);
	void  OutputBlend (const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA);
	void  OutputBlend (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA);
	void  OutputBlend (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA);

	// Add color render target with blend operation and with explicit name.
	void  OutputBlend (const string & name, const RC<Image> & image, EBlendFactor src, EBlendFactor dst, EBlendOp op);
	void  OutputBlend (const string & name, const RC<Image> & image, const MipmapLevel & mipmap, EBlendFactor src, EBlendFactor dst, EBlendOp op);
	void  OutputBlend (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, EBlendFactor src, EBlendFactor dst, EBlendOp op);
	void  OutputBlend (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, EBlendFactor src, EBlendFactor dst, EBlendOp op);
	void  OutputBlend (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, EBlendFactor src, EBlendFactor dst, EBlendOp op);
	void  OutputBlend (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, EBlendFactor src, EBlendFactor dst, EBlendOp op);
	void  OutputBlend (const string & name, const RC<Image> & image, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA);
	void  OutputBlend (const string & name, const RC<Image> & image, const MipmapLevel & mipmap, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA);
	void  OutputBlend (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA);
	void  OutputBlend (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA);
	void  OutputBlend (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA);
	void  OutputBlend (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, EBlendFactor srcRGB, EBlendFactor dstRGB, EBlendOp opRGB, EBlendFactor srcA, EBlendFactor dstA, EBlendOp opA);

	// Used instead of 'Output()' to define image as input attachment & color attachment (read/write input attachment).
	void  InOut (const string & inName, const string & outName, const RC<Image> & image);
	void  InOut (const string & inName, const string & outName, const RC<Image> & image, const RGBA32f & clearColor);
	void  InOut (const string & inName, const string & outName, const RC<Image> & image, const RGBA32i & clearColor);
	void  InOut (const string & inName, const string & outName, const RC<Image> & image, const RGBA32u & clearColor);
	void  InOut (const string & inName, const string & outName, const RC<Image> & image, const DepthStencil & clearDS);
	void  InOut (const string & inName, const RC<Image> & image, const DepthStencil & clearDS);

	// Used instead of 'ArgIn' to define image as input attachment. Supports color and depth formats.
	void  Input (const string & inName, const RC<Image> & image, const string & attachmentName);

	// Add fragment shading rate attachment.
	void  FragmentShadingRate (const RC<Image> & image);

	// Add fragment density map attachment.
	void  FragmentDensityMap (const RC<Image> & image);
	void  DepthRange (float min, float max);

	// Add viewport. 'rect' defined in unorm coords.
	void  AddViewport (const RectF & rect, float minDepth, float maxDepth, const RectF & scissor, const float2 & wScale);
	void  AddViewport (const RectF & rect, float minDepth, float maxDepth);
	void  AddViewport (const RectF & rect);
	void  AddViewport (float left, float top, float right, float bottom);
	void  AddViewport (const RectF & rect, float minDepth, float maxDepth, const RectF & scissor);

	// Set path to fragment shader, empty - load current file.
	Postprocess  ();
	Postprocess (const string & shaderPath);
	Postprocess (const string & shaderPath, EPostprocess postprocessFlags);
	Postprocess (EPostprocess postprocessFlags);
	Postprocess (EPostprocess postprocessFlags, const string & defines);
	Postprocess (const string & shaderPath, const string & defines);

	// Can be used only if pass hasn't attachments.
	void  SetDimension (const RC<DynamicDim> &);
};

struct ComputePass
{

	// Set debug label and color. It is used in graphics profiler.
	void  SetDebugLabel (const string & label);
	void  SetDebugLabel (const string & label, const RGBA8u & color);
	void  SetDebugLabel (const string & label, const RGBA32f & color);
	void  AddFlag (EPassFlags);

	// Measure GPU time for this pass. If pass executed multiple times during 'Repeat()' then average time will be returned.
	void  MeasureTime (const RC<DynamicFloat> & timeInSeconds);

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

	// Value of 'dynamicValue' will be passed to all shaders in the current pass.
	// Value of 'dynamicValue' is constant for whole frame and also can be used in draw call or another pass.
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
	void  Constant (const string & name, const RC<DynamicDim> & dynamicValue);
	void  Constant (const string & name, float constValue);
	void  Constant (const string & name, const float2 & constValue);
	void  Constant (const string & name, const float3 & constValue);
	void  Constant (const string & name, const float4 & constValue);
	void  Constant (const string & name, int constValue);
	void  Constant (const string & name, const int2 & constValue);
	void  Constant (const string & name, const int3 & constValue);
	void  Constant (const string & name, const int4 & constValue);
	void  Constant (const string & name, uint constValue);
	void  Constant (const string & name, const uint2 & constValue);
	void  Constant (const string & name, const uint3 & constValue);
	void  Constant (const string & name, const uint4 & constValue);

	// Returns dynamic dimension of the pass.
	// It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches.
	RC<DynamicDim>  Dimension ();
	void  EnableIfEqual (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfLess (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfGreater (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfAnyBit (const RC<DynamicUInt> & dynamic, uint refValue);

	// Repeat pass multiple times.
	// Can be used for performance tests.
	void  Repeat (const RC<DynamicUInt> &);

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

	// Set subgroup size.
	void  SubgroupSize (uint);

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

struct ComputeMip
{

	// Set debug label and color. It is used in graphics profiler.
	void  SetDebugLabel (const string & label);
	void  SetDebugLabel (const string & label, const RGBA8u & color);
	void  SetDebugLabel (const string & label, const RGBA32f & color);
	void  AddFlag (EPassFlags);

	// Measure GPU time for this pass. If pass executed multiple times during 'Repeat()' then average time will be returned.
	void  MeasureTime (const RC<DynamicFloat> & timeInSeconds);

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

	// Value of 'dynamicValue' will be passed to all shaders in the current pass.
	// Value of 'dynamicValue' is constant for whole frame and also can be used in draw call or another pass.
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
	void  Constant (const string & name, const RC<DynamicDim> & dynamicValue);
	void  Constant (const string & name, float constValue);
	void  Constant (const string & name, const float2 & constValue);
	void  Constant (const string & name, const float3 & constValue);
	void  Constant (const string & name, const float4 & constValue);
	void  Constant (const string & name, int constValue);
	void  Constant (const string & name, const int2 & constValue);
	void  Constant (const string & name, const int3 & constValue);
	void  Constant (const string & name, const int4 & constValue);
	void  Constant (const string & name, uint constValue);
	void  Constant (const string & name, const uint2 & constValue);
	void  Constant (const string & name, const uint3 & constValue);
	void  Constant (const string & name, const uint4 & constValue);

	// Returns dynamic dimension of the pass.
	// It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches.
	RC<DynamicDim>  Dimension ();
	void  EnableIfEqual (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfLess (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfGreater (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfAnyBit (const RC<DynamicUInt> & dynamic, uint refValue);

	// Repeat pass multiple times.
	// Can be used for performance tests.
	void  Repeat (const RC<DynamicUInt> &);

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
	ComputeMip  ();
	ComputeMip (const string & shaderPath);
	ComputeMip (const string & shaderPath, const string & defines);
	void  Variable (const string & inName, const string & outName, const RC<Image> & image);
	void  Variable (const string & inName, const string & outName, const RC<Image> & image, const string & sampler);
	void  Variable (const string & inName, const string & outName, const RC<Image> & image, const MipmapLevel & baseMipmap);
	void  Variable (const string & inName, const string & outName, const RC<Image> & image, const MipmapLevel & baseMipmap, const string & sampler);
};

struct RasterMip
{

	// Set debug label and color. It is used in graphics profiler.
	void  SetDebugLabel (const string & label);
	void  SetDebugLabel (const string & label, const RGBA8u & color);
	void  SetDebugLabel (const string & label, const RGBA32f & color);
	void  AddFlag (EPassFlags);

	// Measure GPU time for this pass. If pass executed multiple times during 'Repeat()' then average time will be returned.
	void  MeasureTime (const RC<DynamicFloat> & timeInSeconds);

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

	// Value of 'dynamicValue' will be passed to all shaders in the current pass.
	// Value of 'dynamicValue' is constant for whole frame and also can be used in draw call or another pass.
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
	void  Constant (const string & name, const RC<DynamicDim> & dynamicValue);
	void  Constant (const string & name, float constValue);
	void  Constant (const string & name, const float2 & constValue);
	void  Constant (const string & name, const float3 & constValue);
	void  Constant (const string & name, const float4 & constValue);
	void  Constant (const string & name, int constValue);
	void  Constant (const string & name, const int2 & constValue);
	void  Constant (const string & name, const int3 & constValue);
	void  Constant (const string & name, const int4 & constValue);
	void  Constant (const string & name, uint constValue);
	void  Constant (const string & name, const uint2 & constValue);
	void  Constant (const string & name, const uint3 & constValue);
	void  Constant (const string & name, const uint4 & constValue);

	// Returns dynamic dimension of the pass.
	// It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches.
	RC<DynamicDim>  Dimension ();
	void  EnableIfEqual (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfLess (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfGreater (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfAnyBit (const RC<DynamicUInt> & dynamic, uint refValue);

	// Repeat pass multiple times.
	// Can be used for performance tests.
	void  Repeat (const RC<DynamicUInt> &);

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
	RasterMip  ();
	RasterMip (const string & shaderPath);
	RasterMip (const string & shaderPath, const string & defines);
	void  Variable (const string & inName, const string & outName, const RC<Image> & image, const string & sampler);
	void  Variable (const string & inName, const string & outName, const RC<Image> & image, const MipmapLevel & baseMipmap, const string & sampler);
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
	void  SetDebugLabel (const string & label, const RGBA32f & color);
	void  AddFlag (EPassFlags);

	// Measure GPU time for this pass. If pass executed multiple times during 'Repeat()' then average time will be returned.
	void  MeasureTime (const RC<DynamicFloat> & timeInSeconds);

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

	// Value of 'dynamicValue' will be passed to all shaders in the current pass.
	// Value of 'dynamicValue' is constant for whole frame and also can be used in draw call or another pass.
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
	void  Constant (const string & name, const RC<DynamicDim> & dynamicValue);
	void  Constant (const string & name, float constValue);
	void  Constant (const string & name, const float2 & constValue);
	void  Constant (const string & name, const float3 & constValue);
	void  Constant (const string & name, const float4 & constValue);
	void  Constant (const string & name, int constValue);
	void  Constant (const string & name, const int2 & constValue);
	void  Constant (const string & name, const int3 & constValue);
	void  Constant (const string & name, const int4 & constValue);
	void  Constant (const string & name, uint constValue);
	void  Constant (const string & name, const uint2 & constValue);
	void  Constant (const string & name, const uint3 & constValue);
	void  Constant (const string & name, const uint4 & constValue);

	// Returns dynamic dimension of the pass.
	// It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches.
	RC<DynamicDim>  Dimension ();
	void  EnableIfEqual (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfLess (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfGreater (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfAnyBit (const RC<DynamicUInt> & dynamic, uint refValue);

	// Repeat pass multiple times.
	// Can be used for performance tests.
	void  Repeat (const RC<DynamicUInt> &);

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

struct SceneGraphicsPass
{

	// Set debug label and color. It is used in graphics profiler.
	void  SetDebugLabel (const string & label);
	void  SetDebugLabel (const string & label, const RGBA8u & color);
	void  SetDebugLabel (const string & label, const RGBA32f & color);
	void  AddFlag (EPassFlags);

	// Measure GPU time for this pass. If pass executed multiple times during 'Repeat()' then average time will be returned.
	void  MeasureTime (const RC<DynamicFloat> & timeInSeconds);

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

	// Value of 'dynamicValue' will be passed to all shaders in the current pass.
	// Value of 'dynamicValue' is constant for whole frame and also can be used in draw call or another pass.
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
	void  Constant (const string & name, const RC<DynamicDim> & dynamicValue);
	void  Constant (const string & name, float constValue);
	void  Constant (const string & name, const float2 & constValue);
	void  Constant (const string & name, const float3 & constValue);
	void  Constant (const string & name, const float4 & constValue);
	void  Constant (const string & name, int constValue);
	void  Constant (const string & name, const int2 & constValue);
	void  Constant (const string & name, const int3 & constValue);
	void  Constant (const string & name, const int4 & constValue);
	void  Constant (const string & name, uint constValue);
	void  Constant (const string & name, const uint2 & constValue);
	void  Constant (const string & name, const uint3 & constValue);
	void  Constant (const string & name, const uint4 & constValue);

	// Returns dynamic dimension of the pass.
	// It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches.
	RC<DynamicDim>  Dimension ();
	void  EnableIfEqual (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfLess (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfGreater (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfAnyBit (const RC<DynamicUInt> & dynamic, uint refValue);

	// Repeat pass multiple times.
	// Can be used for performance tests.
	void  Repeat (const RC<DynamicUInt> &);

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
	void  Output (const RC<Image> & image);
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
	void  Output (const RC<Image> & image, const RGBA32u & clearColor);
	void  Output (const RC<Image> & image, const MipmapLevel & mipmap, const RGBA32u & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const RGBA32u & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const RGBA32u & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const RGBA32u & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const RGBA32u & clearColor);
	void  Output (const RC<Image> & image, const RGBA32i & clearColor);
	void  Output (const RC<Image> & image, const MipmapLevel & mipmap, const RGBA32i & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const RGBA32i & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const RGBA32i & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const RGBA32i & clearColor);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const RGBA32i & clearColor);
	void  Output (const RC<Image> & image, const DepthStencil & clearDepthStencil);
	void  Output (const RC<Image> & image, const MipmapLevel & mipmap, const DepthStencil & clearDepthStencil);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const DepthStencil & clearDepthStencil);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const DepthStencil & clearDepthStencil);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const DepthStencil & clearDepthStencil);
	void  Output (const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const DepthStencil & clearDepthStencil);

	// Add color/depth render target with explicit name.
	void  Output (const string & name, const RC<Image> & image);
	void  Output (const string & name, const RC<Image> & image, const MipmapLevel & mipmap);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap);
	void  Output (const string & name, const RC<Image> & image, const RGBA32f & clearColor);
	void  Output (const string & name, const RC<Image> & image, const MipmapLevel & mipmap, const RGBA32f & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const RGBA32f & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const RGBA32f & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const RGBA32f & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const RGBA32f & clearColor);
	void  Output (const string & name, const RC<Image> & image, const RGBA32u & clearColor);
	void  Output (const string & name, const RC<Image> & image, const MipmapLevel & mipmap, const RGBA32u & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const RGBA32u & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const RGBA32u & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const RGBA32u & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const RGBA32u & clearColor);
	void  Output (const string & name, const RC<Image> & image, const RGBA32i & clearColor);
	void  Output (const string & name, const RC<Image> & image, const MipmapLevel & mipmap, const RGBA32i & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const RGBA32i & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const RGBA32i & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const RGBA32i & clearColor);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const RGBA32i & clearColor);
	void  Output (const string & name, const RC<Image> & image, const DepthStencil & clearDepthStencil);
	void  Output (const string & name, const RC<Image> & image, const MipmapLevel & mipmap, const DepthStencil & clearDepthStencil);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const DepthStencil & clearDepthStencil);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, const MipmapLevel & mipmap, const DepthStencil & clearDepthStencil);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const DepthStencil & clearDepthStencil);
	void  Output (const string & name, const RC<Image> & image, const ImageLayer & baseLayer, uint layerCount, const MipmapLevel & mipmap, const DepthStencil & clearDepthStencil);
	void  OutputLS (const RC<Image> & image, EAttachmentLoadOp loadOp, EAttachmentStoreOp storeOp);
	void  OutputLS (const string & name, const RC<Image> & image, EAttachmentLoadOp loadOp, EAttachmentStoreOp storeOp);

	// Used instead of 'Output()' to define image as input attachment & color attachment (read/write input attachment).
	void  InOut (const string & inName, const string & outName, const RC<Image> & image);
	void  InOut (const string & inName, const string & outName, const RC<Image> & image, const RGBA32f & clearColor);
	void  InOut (const string & inName, const string & outName, const RC<Image> & image, const RGBA32i & clearColor);
	void  InOut (const string & inName, const string & outName, const RC<Image> & image, const RGBA32u & clearColor);
	void  InOut (const string & inName, const string & outName, const RC<Image> & image, const DepthStencil & clearDS);
	void  InOut (const string & inName, const RC<Image> & image, const DepthStencil & clearDS);

	// Used instead of 'ArgIn' to define image as input attachment. Supports color and depth formats.
	void  Input (const string & inName, const RC<Image> & image, const string & attachmentName);

	// Add fragment shading rate attachment.
	void  FragmentShadingRate (const RC<Image> & image);

	// Add fragment density map attachment.
	void  FragmentDensityMap (const RC<Image> & image);
	void  DepthRange (float min, float max);

	// Add viewport. 'rect' defined in unorm coords.
	void  AddViewport (const RectF & rect, float minDepth, float maxDepth, const RectF & scissor, const float2 & wScale);
	void  AddViewport (const RectF & rect, float minDepth, float maxDepth);
	void  AddViewport (const RectF & rect);
	void  AddViewport (float left, float top, float right, float bottom);
	void  AddViewport (const RectF & rect, float minDepth, float maxDepth, const RectF & scissor);

	// Add path to single pipeline or folder with pipelines.
	// Scene geometry will be linked with compatible pipeline or error will be generated.
	// Use star symbol: '*<file-suffix>' to add current file path without extension to suffix.
	void  AddPipeline (const string & pplnFile);
	void  AddPipelines (const string & pplnFolder);
	void  Layer (ERenderLayer);
	void  FragmentShadingRate (EShadingRate rate, EShadingRateCombinerOp primitiveOp, EShadingRateCombinerOp textureOp);

	// Can be used only if pass hasn't attachments.
	void  SetDimension (const RC<DynamicDim> &);
	void  NextSubpass ();
	void  NextSubpass (const string & passName);
};

struct SceneRayTracingPass
{

	// Set debug label and color. It is used in graphics profiler.
	void  SetDebugLabel (const string & label);
	void  SetDebugLabel (const string & label, const RGBA8u & color);
	void  SetDebugLabel (const string & label, const RGBA32f & color);
	void  AddFlag (EPassFlags);

	// Measure GPU time for this pass. If pass executed multiple times during 'Repeat()' then average time will be returned.
	void  MeasureTime (const RC<DynamicFloat> & timeInSeconds);

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

	// Value of 'dynamicValue' will be passed to all shaders in the current pass.
	// Value of 'dynamicValue' is constant for whole frame and also can be used in draw call or another pass.
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
	void  Constant (const string & name, const RC<DynamicDim> & dynamicValue);
	void  Constant (const string & name, float constValue);
	void  Constant (const string & name, const float2 & constValue);
	void  Constant (const string & name, const float3 & constValue);
	void  Constant (const string & name, const float4 & constValue);
	void  Constant (const string & name, int constValue);
	void  Constant (const string & name, const int2 & constValue);
	void  Constant (const string & name, const int3 & constValue);
	void  Constant (const string & name, const int4 & constValue);
	void  Constant (const string & name, uint constValue);
	void  Constant (const string & name, const uint2 & constValue);
	void  Constant (const string & name, const uint3 & constValue);
	void  Constant (const string & name, const uint4 & constValue);

	// Returns dynamic dimension of the pass.
	// It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches.
	RC<DynamicDim>  Dimension ();
	void  EnableIfEqual (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfLess (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfGreater (const RC<DynamicUInt> & dynamic, uint refValue);
	void  EnableIfAnyBit (const RC<DynamicUInt> & dynamic, uint refValue);

	// Repeat pass multiple times.
	// Can be used for performance tests.
	void  Repeat (const RC<DynamicUInt> &);

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
	void  AddIntegerDotProduct (const array<EIntegerDotProductFeat> &);
	bool  hasSubgroupOperation (ESubgroupOperation);
	bool  hasUniformTexBufferFormat (EPixelFormat);
	bool  hasStorageTexBufferFormat (EPixelFormat);
	bool  hasStorageTexBufferAtomicFormat (EPixelFormat);
	bool  hasStorageImageAtomicFormat (EPixelFormat);
	bool  hasStorageImageFormat (EPixelFormat);
	bool  hasAttachmentBlendFormat (EPixelFormat);
	bool  hasAttachmentFormat (EPixelFormat);
	bool  hasLinearSampledFormat (EPixelFormat);
	bool  hasMinmaxFilterFormat (EPixelFormat);
	bool  hasHwCompressedAttachmentFormat (EPixelFormat);
	bool  hasLossyCompressedAttachmentFormat (EPixelFormat);
	bool  hasVertexFormat (EVertexType);
	bool  hasAccelStructVertexFormat (EVertexType);
	bool  hasSurfaceFormat (ESurfaceFormat);
	bool  hasCooperativeMatrixConfig (ECoopMatrixCfg);
	bool  hasCooperativeVectorConfig (ECoopVecCfg);
	bool  hasIntegerDotProductFeature (EIntegerDotProductFeat);
	void  alphaToOne (EFeature);
	bool  hasAlphaToOne ();
	void  depthBiasClamp (EFeature);
	bool  hasDepthBiasClamp ();
	void  depthBounds (EFeature);
	bool  hasDepthBounds ();
	void  depthClamp (EFeature);
	bool  hasDepthClamp ();
	void  dualSrcBlend (EFeature);
	bool  hasDualSrcBlend ();
	void  fillModeNonSolid (EFeature);
	bool  hasFillModeNonSolid ();
	void  independentBlend (EFeature);
	bool  hasIndependentBlend ();
	void  logicOp (EFeature);
	bool  hasLogicOp ();
	void  sampleRateShading (EFeature);
	bool  hasSampleRateShading ();
	void  constantAlphaColorBlendFactors (EFeature);
	bool  hasConstantAlphaColorBlendFactors ();
	void  pointPolygons (EFeature);
	bool  hasPointPolygons ();
	void  triangleFans (EFeature);
	bool  hasTriangleFans ();
	void  largePoints (EFeature);
	bool  hasLargePoints ();
	void  wideLines (EFeature);
	bool  hasWideLines ();
	void  subgroupTypes (uint);
	ESubgroupTypes  getSubgroupTypes ();
	void  subgroupStages (uint);
	EShaderStages  getSubgroupStages ();
	void  subgroupQuadStages (uint);
	EShaderStages  getSubgroupQuadStages ();
	void  requiredSubgroupSizeStages (uint);
	EShaderStages  getRequiredSubgroupSizeStages ();
	void  minSubgroupSize (uint);
	uint  getMinSubgroupSize ();
	void  maxSubgroupSize (uint);
	uint  getMaxSubgroupSize ();
	void  subgroup (EFeature);
	bool  hasSubgroup ();
	void  subgroupBroadcastDynamicId (EFeature);
	bool  hasSubgroupBroadcastDynamicId ();
	void  subgroupSizeControl (EFeature);
	bool  hasSubgroupSizeControl ();
	void  shaderSubgroupUniformControlFlow (EFeature);
	bool  hasShaderSubgroupUniformControlFlow ();
	void  shaderMaximalReconvergence (EFeature);
	bool  hasShaderMaximalReconvergence ();
	void  shaderQuadControl (EFeature);
	bool  hasShaderQuadControl ();
	void  shaderInt8 (EFeature);
	bool  hasShaderInt8 ();
	void  shaderInt16 (EFeature);
	bool  hasShaderInt16 ();
	void  shaderInt64 (EFeature);
	bool  hasShaderInt64 ();
	void  shaderFloat16 (EFeature);
	bool  hasShaderFloat16 ();
	void  shaderFloat64 (EFeature);
	bool  hasShaderFloat64 ();
	void  shaderBFloat16Type (EFeature);
	bool  hasShaderBFloat16Type ();
	void  shaderBFloat16DotProduct (EFeature);
	bool  hasShaderBFloat16DotProduct ();
	void  shaderBFloat16CooperativeMatrix (EFeature);
	bool  hasShaderBFloat16CooperativeMatrix ();
	void  shaderFloat8 (EFeature);
	bool  hasShaderFloat8 ();
	void  shaderFloat8CooperativeMatrix (EFeature);
	bool  hasShaderFloat8CooperativeMatrix ();
	void  storageBuffer16BitAccess (EFeature);
	bool  hasStorageBuffer16BitAccess ();
	void  uniformAndStorageBuffer16BitAccess (EFeature);
	bool  hasUniformAndStorageBuffer16BitAccess ();
	void  storageInputOutput16 (EFeature);
	bool  hasStorageInputOutput16 ();
	void  storageBuffer8BitAccess (EFeature);
	bool  hasStorageBuffer8BitAccess ();
	void  uniformAndStorageBuffer8BitAccess (EFeature);
	bool  hasUniformAndStorageBuffer8BitAccess ();
	void  uniformBufferStandardLayout (EFeature);
	bool  hasUniformBufferStandardLayout ();
	void  scalarBlockLayout (EFeature);
	bool  hasScalarBlockLayout ();
	void  bufferDeviceAddress (EFeature);
	bool  hasBufferDeviceAddress ();
	void  storagePushConstant8 (EFeature);
	bool  hasStoragePushConstant8 ();
	void  storagePushConstant16 (EFeature);
	bool  hasStoragePushConstant16 ();
	void  fragmentStoresAndAtomics (EFeature);
	bool  hasFragmentStoresAndAtomics ();
	void  vertexPipelineStoresAndAtomics (EFeature);
	bool  hasVertexPipelineStoresAndAtomics ();
	void  shaderImageInt64Atomics (EFeature);
	bool  hasShaderImageInt64Atomics ();
	void  shaderBufferInt64Atomics (EFeature);
	bool  hasShaderBufferInt64Atomics ();
	void  shaderSharedInt64Atomics (EFeature);
	bool  hasShaderSharedInt64Atomics ();
	void  shaderBufferFloat32Atomics (EFeature);
	bool  hasShaderBufferFloat32Atomics ();
	void  shaderBufferFloat32AtomicAdd (EFeature);
	bool  hasShaderBufferFloat32AtomicAdd ();
	void  shaderBufferFloat64Atomics (EFeature);
	bool  hasShaderBufferFloat64Atomics ();
	void  shaderBufferFloat64AtomicAdd (EFeature);
	bool  hasShaderBufferFloat64AtomicAdd ();
	void  shaderSharedFloat32Atomics (EFeature);
	bool  hasShaderSharedFloat32Atomics ();
	void  shaderSharedFloat32AtomicAdd (EFeature);
	bool  hasShaderSharedFloat32AtomicAdd ();
	void  shaderSharedFloat64Atomics (EFeature);
	bool  hasShaderSharedFloat64Atomics ();
	void  shaderSharedFloat64AtomicAdd (EFeature);
	bool  hasShaderSharedFloat64AtomicAdd ();
	void  shaderImageFloat32Atomics (EFeature);
	bool  hasShaderImageFloat32Atomics ();
	void  shaderImageFloat32AtomicAdd (EFeature);
	bool  hasShaderImageFloat32AtomicAdd ();
	void  shaderBufferFloat16Atomics (EFeature);
	bool  hasShaderBufferFloat16Atomics ();
	void  shaderBufferFloat16AtomicAdd (EFeature);
	bool  hasShaderBufferFloat16AtomicAdd ();
	void  shaderBufferFloat16AtomicMinMax (EFeature);
	bool  hasShaderBufferFloat16AtomicMinMax ();
	void  shaderBufferFloat32AtomicMinMax (EFeature);
	bool  hasShaderBufferFloat32AtomicMinMax ();
	void  shaderBufferFloat64AtomicMinMax (EFeature);
	bool  hasShaderBufferFloat64AtomicMinMax ();
	void  shaderSharedFloat16Atomics (EFeature);
	bool  hasShaderSharedFloat16Atomics ();
	void  shaderSharedFloat16AtomicAdd (EFeature);
	bool  hasShaderSharedFloat16AtomicAdd ();
	void  shaderSharedFloat16AtomicMinMax (EFeature);
	bool  hasShaderSharedFloat16AtomicMinMax ();
	void  shaderSharedFloat32AtomicMinMax (EFeature);
	bool  hasShaderSharedFloat32AtomicMinMax ();
	void  shaderSharedFloat64AtomicMinMax (EFeature);
	bool  hasShaderSharedFloat64AtomicMinMax ();
	void  shaderImageFloat32AtomicMinMax (EFeature);
	bool  hasShaderImageFloat32AtomicMinMax ();
	void  sparseImageFloat32AtomicMinMax (EFeature);
	bool  hasSparseImageFloat32AtomicMinMax ();
	void  shaderAtomicPackedFp16 (EFeature);
	bool  hasShaderAtomicPackedFp16 ();
	void  shaderOutputViewportIndex (EFeature);
	bool  hasShaderOutputViewportIndex ();
	void  shaderOutputLayer (EFeature);
	bool  hasShaderOutputLayer ();
	void  shaderSubgroupClock (EFeature);
	bool  hasShaderSubgroupClock ();
	void  shaderDeviceClock (EFeature);
	bool  hasShaderDeviceClock ();
	void  cooperativeMatrix (EFeature);
	bool  hasCooperativeMatrix ();
	void  cooperativeMatrixStages (uint);
	EShaderStages  getCooperativeMatrixStages ();
	void  cooperativeVector (EFeature);
	bool  hasCooperativeVector ();
	void  cooperativeVectorTraining (EFeature);
	bool  hasCooperativeVectorTraining ();
	void  shaderIntegerDotProduct (EFeature);
	bool  hasShaderIntegerDotProduct ();
	void  shaderClipDistance (EFeature);
	bool  hasShaderClipDistance ();
	void  shaderCullDistance (EFeature);
	bool  hasShaderCullDistance ();
	void  shaderResourceMinLod (EFeature);
	bool  hasShaderResourceMinLod ();
	void  shaderDrawParameters (EFeature);
	bool  hasShaderDrawParameters ();
	void  runtimeDescriptorArray (EFeature);
	bool  hasRuntimeDescriptorArray ();
	void  shaderSMBuiltinsNV (EFeature);
	bool  hasShaderSMBuiltinsNV ();
	void  shaderCoreBuiltinsARM (EFeature);
	bool  hasShaderCoreBuiltinsARM ();
	void  shaderSampleRateInterpolationFunctions (EFeature);
	bool  hasShaderSampleRateInterpolationFunctions ();
	void  shaderStencilExport (EFeature);
	bool  hasShaderStencilExport ();
	void  shaderExpectAssume (EFeature);
	bool  hasShaderExpectAssume ();
	void  clipSpaceWScalingNV (EFeature);
	bool  hasClipSpaceWScalingNV ();
	void  shaderSampledImageArrayDynamicIndexing (EFeature);
	bool  hasShaderSampledImageArrayDynamicIndexing ();
	void  shaderStorageBufferArrayDynamicIndexing (EFeature);
	bool  hasShaderStorageBufferArrayDynamicIndexing ();
	void  shaderStorageImageArrayDynamicIndexing (EFeature);
	bool  hasShaderStorageImageArrayDynamicIndexing ();
	void  shaderUniformBufferArrayDynamicIndexing (EFeature);
	bool  hasShaderUniformBufferArrayDynamicIndexing ();
	void  shaderInputAttachmentArrayDynamicIndexing (EFeature);
	bool  hasShaderInputAttachmentArrayDynamicIndexing ();
	void  shaderUniformTexelBufferArrayDynamicIndexing (EFeature);
	bool  hasShaderUniformTexelBufferArrayDynamicIndexing ();
	void  shaderStorageTexelBufferArrayDynamicIndexing (EFeature);
	bool  hasShaderStorageTexelBufferArrayDynamicIndexing ();
	void  shaderUniformBufferArrayNonUniformIndexing (EFeature);
	bool  hasShaderUniformBufferArrayNonUniformIndexing ();
	void  shaderSampledImageArrayNonUniformIndexing (EFeature);
	bool  hasShaderSampledImageArrayNonUniformIndexing ();
	void  shaderStorageBufferArrayNonUniformIndexing (EFeature);
	bool  hasShaderStorageBufferArrayNonUniformIndexing ();
	void  shaderStorageImageArrayNonUniformIndexing (EFeature);
	bool  hasShaderStorageImageArrayNonUniformIndexing ();
	void  shaderInputAttachmentArrayNonUniformIndexing (EFeature);
	bool  hasShaderInputAttachmentArrayNonUniformIndexing ();
	void  shaderUniformTexelBufferArrayNonUniformIndexing (EFeature);
	bool  hasShaderUniformTexelBufferArrayNonUniformIndexing ();
	void  shaderStorageTexelBufferArrayNonUniformIndexing (EFeature);
	bool  hasShaderStorageTexelBufferArrayNonUniformIndexing ();
	void  shaderUniformBufferArrayNonUniformIndexingNative (EFeature);
	bool  hasShaderUniformBufferArrayNonUniformIndexingNative ();
	void  shaderSampledImageArrayNonUniformIndexingNative (EFeature);
	bool  hasShaderSampledImageArrayNonUniformIndexingNative ();
	void  shaderStorageBufferArrayNonUniformIndexingNative (EFeature);
	bool  hasShaderStorageBufferArrayNonUniformIndexingNative ();
	void  shaderStorageImageArrayNonUniformIndexingNative (EFeature);
	bool  hasShaderStorageImageArrayNonUniformIndexingNative ();
	void  shaderInputAttachmentArrayNonUniformIndexingNative (EFeature);
	bool  hasShaderInputAttachmentArrayNonUniformIndexingNative ();
	void  quadDivergentImplicitLod (EFeature);
	bool  hasQuadDivergentImplicitLod ();
	void  shaderStorageImageMultisample (EFeature);
	bool  hasShaderStorageImageMultisample ();
	void  shaderStorageImageReadWithoutFormat (EFeature);
	bool  hasShaderStorageImageReadWithoutFormat ();
	void  shaderStorageImageWriteWithoutFormat (EFeature);
	bool  hasShaderStorageImageWriteWithoutFormat ();
	void  vulkanMemoryModel (EFeature);
	bool  hasVulkanMemoryModel ();
	void  vulkanMemoryModelDeviceScope (EFeature);
	bool  hasVulkanMemoryModelDeviceScope ();
	void  vulkanMemoryModelAvailabilityVisibilityChains (EFeature);
	bool  hasVulkanMemoryModelAvailabilityVisibilityChains ();
	void  shaderDemoteToHelperInvocation (EFeature);
	bool  hasShaderDemoteToHelperInvocation ();
	void  shaderTerminateInvocation (EFeature);
	bool  hasShaderTerminateInvocation ();
	void  shaderZeroInitializeWorkgroupMemory (EFeature);
	bool  hasShaderZeroInitializeWorkgroupMemory ();
	void  fragmentShaderSampleInterlock (EFeature);
	bool  hasFragmentShaderSampleInterlock ();
	void  fragmentShaderPixelInterlock (EFeature);
	bool  hasFragmentShaderPixelInterlock ();
	void  fragmentShaderShadingRateInterlock (EFeature);
	bool  hasFragmentShaderShadingRateInterlock ();
	void  fragmentShaderBarycentric (EFeature);
	bool  hasFragmentShaderBarycentric ();
	void  pipelineFragmentShadingRate (EFeature);
	bool  hasPipelineFragmentShadingRate ();
	void  primitiveFragmentShadingRate (EFeature);
	bool  hasPrimitiveFragmentShadingRate ();
	void  attachmentFragmentShadingRate (EFeature);
	bool  hasAttachmentFragmentShadingRate ();
	void  primitiveFragmentShadingRateWithMultipleViewports (EFeature);
	bool  hasPrimitiveFragmentShadingRateWithMultipleViewports ();
	void  layeredShadingRateAttachments (EFeature);
	bool  hasLayeredShadingRateAttachments ();
	void  fragmentShadingRateWithShaderDepthStencilWrites (EFeature);
	bool  hasFragmentShadingRateWithShaderDepthStencilWrites ();
	void  fragmentShadingRateWithSampleMask (EFeature);
	bool  hasFragmentShadingRateWithSampleMask ();
	void  fragmentShadingRateWithShaderSampleMask (EFeature);
	bool  hasFragmentShadingRateWithShaderSampleMask ();
	void  fragmentShadingRateWithFragmentShaderInterlock (EFeature);
	bool  hasFragmentShadingRateWithFragmentShaderInterlock ();
	void  fragmentShadingRateWithCustomSampleLocations (EFeature);
	bool  hasFragmentShadingRateWithCustomSampleLocations ();
	void  fragmentDensityMap (EFeature);
	bool  hasFragmentDensityMap ();
	void  fragmentDensityMapDynamic (EFeature);
	bool  hasFragmentDensityMapDynamic ();
	void  fragmentDensityMapNonSubsampledImages (EFeature);
	bool  hasFragmentDensityMapNonSubsampledImages ();
	void  fragmentDensityInvocations (EFeature);
	bool  hasFragmentDensityInvocations ();
	void  subsampledLoads (EFeature);
	bool  hasSubsampledLoads ();
	void  maxSubsampledArrayLayers (uint);
	uint  getMaxSubsampledArrayLayers ();
	void  perPipeline_maxSubsampledSamplers (uint);
	uint8  getPerPipeline_maxSubsampledSamplers ();
	void  accelerationStructureIndirectBuild (EFeature);
	bool  hasAccelerationStructureIndirectBuild ();
	void  clusterAccelerationStructure (EFeature);
	bool  hasClusterAccelerationStructure ();
	void  partitionedAccelerationStructure (EFeature);
	bool  hasPartitionedAccelerationStructure ();
	void  rayQuery (EFeature);
	bool  hasRayQuery ();
	void  rayQueryStages (uint);
	EShaderStages  getRayQueryStages ();
	void  rayTracingPipeline (EFeature);
	bool  hasRayTracingPipeline ();
	void  rayTraversalPrimitiveCulling (EFeature);
	bool  hasRayTraversalPrimitiveCulling ();
	void  maxRayRecursionDepth (uint);
	uint16  getMaxRayRecursionDepth ();
	void  drawIndirectFirstInstance (EFeature);
	bool  hasDrawIndirectFirstInstance ();
	void  drawIndirectCount (EFeature);
	bool  hasDrawIndirectCount ();
	void  maxDrawIndirectCount (uint);
	uint  getMaxDrawIndirectCount ();
	void  multiview (EFeature);
	bool  hasMultiview ();
	void  multiviewGeometryShader (EFeature);
	bool  hasMultiviewGeometryShader ();
	void  multiviewTessellationShader (EFeature);
	bool  hasMultiviewTessellationShader ();
	void  maxMultiviewViewCount (uint);
	uint8  getMaxMultiviewViewCount ();
	void  multiViewport (EFeature);
	bool  hasMultiViewport ();
	void  maxViewports (uint);
	uint8  getMaxViewports ();
	void  sampleLocations (EFeature);
	bool  hasSampleLocations ();
	void  variableSampleLocations (EFeature);
	bool  hasVariableSampleLocations ();
	void  tessellationIsolines (EFeature);
	bool  hasTessellationIsolines ();
	void  tessellationPointMode (EFeature);
	bool  hasTessellationPointMode ();
	void  maxTexelBufferElements (uint);
	uint  getMaxTexelBufferElements ();
	void  maxUniformBufferSize (uint);
	void  maxStorageBufferSize (uint);
	void  perPipeline_maxUniformBuffersDynamic (uint);
	uint8  getPerPipeline_maxUniformBuffersDynamic ();
	void  perPipeline_maxStorageBuffersDynamic (uint);
	uint8  getPerPipeline_maxStorageBuffersDynamic ();
	void  perPipeline_maxTotalBuffersDynamic (uint);
	uint8  getPerPipeline_maxTotalBuffersDynamic ();
	void  maxDescriptorSets (uint);
	uint8  getMaxDescriptorSets ();
	void  maxTexelOffset (uint);
	uint8  getMaxTexelOffset ();
	void  maxTexelGatherOffset (uint);
	uint8  getMaxTexelGatherOffset ();
	void  maxFragmentOutputAttachments (uint);
	uint8  getMaxFragmentOutputAttachments ();
	void  maxFragmentDualSrcAttachments (uint);
	uint8  getMaxFragmentDualSrcAttachments ();
	void  maxFragmentCombinedOutputResources (uint);
	uint  getMaxFragmentCombinedOutputResources ();
	void  maxPushConstantsSize (uint);
	uint  getMaxPushConstantsSize ();
	void  maxVertAmplification (uint);
	uint8  getMaxVertAmplification ();
	void  maxTotalThreadgroupSize (uint);
	void  maxTotalTileMemory (uint);
	void  maxComputeSharedMemorySize (uint);
	void  maxComputeWorkGroupInvocations (uint);
	uint  getMaxComputeWorkGroupInvocations ();
	void  maxComputeWorkGroupSizeX (uint);
	uint  getMaxComputeWorkGroupSizeX ();
	void  maxComputeWorkGroupSizeY (uint);
	uint  getMaxComputeWorkGroupSizeY ();
	void  maxComputeWorkGroupSizeZ (uint);
	uint  getMaxComputeWorkGroupSizeZ ();
	void  taskShader (EFeature);
	bool  hasTaskShader ();
	void  meshShader (EFeature);
	bool  hasMeshShader ();
	void  maxTaskWorkGroupSize (uint);
	uint  getMaxTaskWorkGroupSize ();
	void  maxMeshWorkGroupSize (uint);
	uint  getMaxMeshWorkGroupSize ();
	void  maxMeshOutputVertices (uint);
	uint  getMaxMeshOutputVertices ();
	void  maxMeshOutputPrimitives (uint);
	uint  getMaxMeshOutputPrimitives ();
	void  maxMeshOutputPerVertexGranularity (uint);
	uint  getMaxMeshOutputPerVertexGranularity ();
	void  maxMeshOutputPerPrimitiveGranularity (uint);
	uint  getMaxMeshOutputPerPrimitiveGranularity ();
	void  maxTaskPayloadSize (uint);
	void  maxTaskSharedMemorySize (uint);
	void  maxMeshSharedMemorySize (uint);
	void  maxMeshOutputMemorySize (uint);
	void  maxTaskPayloadAndSharedMemorySize (uint);
	void  maxMeshPayloadAndSharedMemorySize (uint);
	void  maxMeshPayloadAndOutputMemorySize (uint);
	void  maxMeshMultiviewViewCount (uint);
	uint8  getMaxMeshMultiviewViewCount ();
	void  maxPreferredTaskWorkGroupInvocations (uint);
	uint  getMaxPreferredTaskWorkGroupInvocations ();
	void  maxPreferredMeshWorkGroupInvocations (uint);
	uint  getMaxPreferredMeshWorkGroupInvocations ();
	void  maxRasterOrderGroups (uint);
	uint16  getMaxRasterOrderGroups ();
	void  geometryShader (EFeature);
	bool  hasGeometryShader ();
	void  tessellationShader (EFeature);
	bool  hasTessellationShader ();
	void  computeShader (EFeature);
	bool  hasComputeShader ();
	void  tileShader (EFeature);
	bool  hasTileShader ();
	void  vertexDivisor (EFeature);
	bool  hasVertexDivisor ();
	void  maxVertexAttribDivisor (uint);
	uint  getMaxVertexAttribDivisor ();
	void  maxVertexAttributes (uint);
	uint8  getMaxVertexAttributes ();
	void  maxVertexBuffers (uint);
	uint8  getMaxVertexBuffers ();
	void  rasterizationOrderColorAttachmentAccess (EFeature);
	bool  hasRasterizationOrderColorAttachmentAccess ();
	void  rasterizationOrderDepthAttachmentAccess (EFeature);
	bool  hasRasterizationOrderDepthAttachmentAccess ();
	void  rasterizationOrderStencilAttachmentAccess (EFeature);
	bool  hasRasterizationOrderStencilAttachmentAccess ();
	void  imageCubeArray (EFeature);
	bool  hasImageCubeArray ();
	void  textureCompressionASTC_LDR (EFeature);
	bool  hasTextureCompressionASTC_LDR ();
	void  textureCompressionASTC_HDR (EFeature);
	bool  hasTextureCompressionASTC_HDR ();
	void  textureCompressionBC (EFeature);
	bool  hasTextureCompressionBC ();
	void  textureCompressionETC2 (EFeature);
	bool  hasTextureCompressionETC2 ();
	void  multisampleArrayImage (EFeature);
	bool  hasMultisampleArrayImage ();
	void  imageViewFormatList (EFeature);
	bool  hasImageViewFormatList ();
	void  imageViewExtendedUsage (EFeature);
	bool  hasImageViewExtendedUsage ();
	void  maxImageDimension1D (uint);
	uint  getMaxImageDimension1D ();
	void  maxImageDimension2D (uint);
	uint  getMaxImageDimension2D ();
	void  maxImageDimension3D (uint);
	uint  getMaxImageDimension3D ();
	void  maxImageDimensionCube (uint);
	uint  getMaxImageDimensionCube ();
	void  maxImageArrayLayers (uint);
	uint  getMaxImageArrayLayers ();
	void  samplerAnisotropy (EFeature);
	bool  hasSamplerAnisotropy ();
	void  samplerMirrorClampToEdge (EFeature);
	bool  hasSamplerMirrorClampToEdge ();
	void  samplerFilterMinmax (EFeature);
	bool  hasSamplerFilterMinmax ();
	void  filterMinmaxImageComponentMapping (EFeature);
	bool  hasFilterMinmaxImageComponentMapping ();
	void  samplerMipLodBias (EFeature);
	bool  hasSamplerMipLodBias ();
	void  samplerYcbcrConversion (EFeature);
	bool  hasSamplerYcbcrConversion ();
	void  ycbcr2Plane444 (EFeature);
	bool  hasYcbcr2Plane444 ();
	void  nonSeamlessCubeMap (EFeature);
	bool  hasNonSeamlessCubeMap ();
	void  maxSamplerAnisotropy (float);
	void  maxSamplerLodBias (float);
	uint  getFramebufferColorSampleCounts ();
	uint  getFramebufferDepthSampleCounts ();
	void  maxFramebufferLayers (uint);
	uint  getMaxFramebufferLayers ();
	void  variableMultisampleRate (EFeature);
	bool  hasVariableMultisampleRate ();
	void  externalFormatAndroid (EFeature);
	bool  hasExternalFormatAndroid ();
	void  metalArgBufferTier (uint);
	uint8  getMetalArgBufferTier ();
	void  perPipeline_maxInputAttachments (uint);
	uint  getPerPipeline_maxInputAttachments ();
	void  perPipeline_maxSampledImages (uint);
	uint  getPerPipeline_maxSampledImages ();
	void  perPipeline_maxSamplers (uint);
	uint  getPerPipeline_maxSamplers ();
	void  perPipeline_maxStorageBuffers (uint);
	uint  getPerPipeline_maxStorageBuffers ();
	void  perPipeline_maxStorageImages (uint);
	uint  getPerPipeline_maxStorageImages ();
	void  perPipeline_maxUniformBuffers (uint);
	uint  getPerPipeline_maxUniformBuffers ();
	void  perPipeline_maxAccelStructures (uint);
	uint  getPerPipeline_maxAccelStructures ();
	void  perPipeline_maxTotalResources (uint);
	uint  getPerPipeline_maxTotalResources ();
	void  perStage_maxInputAttachments (uint);
	uint  getPerStage_maxInputAttachments ();
	void  perStage_maxSampledImages (uint);
	uint  getPerStage_maxSampledImages ();
	void  perStage_maxSamplers (uint);
	uint  getPerStage_maxSamplers ();
	void  perStage_maxStorageBuffers (uint);
	uint  getPerStage_maxStorageBuffers ();
	void  perStage_maxStorageImages (uint);
	uint  getPerStage_maxStorageImages ();
	void  perStage_maxUniformBuffers (uint);
	uint  getPerStage_maxUniformBuffers ();
	void  perStage_maxAccelStructures (uint);
	uint  getPerStage_maxAccelStructures ();
	void  perStage_maxTotalResources (uint);
	uint  getPerStage_maxTotalResources ();
};

RC<FeatureSet>  FindFeatureSet (const string & name);
struct EnableLabel
{
	EnableLabel ();
	EnableLabel (const EnableLabel&);
	EnableLabel&  operator = (const EnableLabel&);
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
void  CopyImage (const RC<Image> & src, const RC<Image> & dst);

// Pass which copy image content to another image without format restrictions.
void  CopyImage2 (const RC<Image> & src, const RC<Image> & dst);

// Pass which blits image to another image.
void  BlitImage (const RC<Image> & src, const RC<Image> & dst);

// Pass which resolve multisample image to another single-sampled image.
void  ResolveImage (const RC<Image> & src, const RC<Image> & dst);

// Pass which compress image on CPU or GPU.
void  CompressImage (const RC<Image> & src, const RC<Image> & dst);

// Pass which compress image on CPU or GPU.
// 'dstFormat' may not be supported by current GPU, but may be used for software decoding.
// 'dst' image must be compatible with 'dstFormat'.
void  CompressImage (const RC<Image> & src, const RC<Image> & dst, EPixelFormat dstFormat);

// Pass to clear float-color image.
void  ClearImage (const RC<Image> &, const RGBA32f &);

// Pass to clear uint-color image.
void  ClearImage (const RC<Image> &, const RGBA32u &);

// Pass to clear int-color image.
void  ClearImage (const RC<Image> &, const RGBA32i &);

// Pass to clear buffer.
void  ClearBuffer (const RC<Buffer> &, uint);
void  ClearBuffer (const RC<Buffer> & buffer, uint64 offset, uint64 size, uint value);
void  ConvertCooperativeVectorMatrix (uint numRows, uint numColumns, ECoopMatrixComponentType srcType, const RC<Buffer> & srcBuffer, uint srcOffset, uint srcSize, uint srcStride, ECoopVecMatrixLayout srcLayout, ECoopMatrixComponentType dstType, const RC<Buffer> & dstBuffer, uint dstOffset, uint dstSize, uint dstStride, ECoopVecMatrixLayout dstLayout);

// Readback the image and save it to a file in DDS format. Rendering will be paused until the readback is completed.
void  Export (const RC<Image> & image, const string & prefix);

// Readback the buffer and save it to a file in structured format. Rendering will be paused until the readback is completed.
void  DbgExport (const RC<Buffer> & buffer, const string & prefix);

// Readback the buffer and save it to a file in binary format. Rendering will be paused until the readback is completed.
void  Export (const RC<Buffer> & buffer, const string & prefix);
void  Export (const RC<Buffer> & buffer, const string & prefix, uint offset, uint size);

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

// Returns spherical cube with 2D UV
void  GetSphere (uint lod, array<float3> & positions, array<float2> & texcoords2d, array<uint> & indices);

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

// Returns cone, apex in +Z
void  GetCone (uint segmentCount, float radius, float height, array<float3> & positions, array<uint> & indices);

// Returns cone, apex in +Z
void  GetCone (uint segmentCount, float radius, float height, array<float3> & positions, array<float3> & normals, array<float2> & texcoords, array<uint> & indices);

// Returns cone, apex in +Z
void  GetCone (uint segmentCount, float radius, float height, array<float3> & positions, array<float3> & normals, array<float3> & tangents, array<float3> & bitangents, array<float2> & texcoords, array<uint> & indices);

// Returns spherical cube without projection and face rotation.
// In 'positions': xy - pos on face, z - face index.
void  GetSphericalCube (uint lod, array<float3> & positions, array<uint> & indices);

// Helper function to convert array of indices to array of uint3 indices per triangle
void  IndicesToPrimitives (const array<uint> & indices, array<uint3> & primitives);
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

// Add slider to UI.
void  Slider (const RC<DynamicInt> & dyn, const string & name);
void  Slider (const RC<DynamicInt> & dyn, const string & name, int min, int max);
void  Slider (const RC<DynamicInt2> & dyn, const string & name, const int2 & min, const int2 & max);
void  Slider (const RC<DynamicInt3> & dyn, const string & name, const int3 & min, const int3 & max);
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

// Add label to UI.
void  Label (const RC<DynamicInt> & dyn, const string & name);
void  Label (const RC<DynamicInt2> & dyn, const string & name);
void  Label (const RC<DynamicInt3> & dyn, const string & name);
void  Label (const RC<DynamicInt4> & dyn, const string & name);
void  Label (const RC<DynamicUInt> & dyn, const string & name);
void  Label (const RC<DynamicUInt2> & dyn, const string & name);
void  Label (const RC<DynamicUInt3> & dyn, const string & name);
void  Label (const RC<DynamicUInt4> & dyn, const string & name);
void  Label (const RC<DynamicFloat> & dyn, const string & name);
void  Label (const RC<DynamicFloat2> & dyn, const string & name);
void  Label (const RC<DynamicFloat3> & dyn, const string & name);
void  Label (const RC<DynamicFloat4> & dyn, const string & name);
void  Label (const RC<DynamicInt> & dyn, const string & name, const EnableLabel & enableIf);
void  Label (const RC<DynamicInt2> & dyn, const string & name, const EnableLabel & enableIf);
void  Label (const RC<DynamicInt3> & dyn, const string & name, const EnableLabel & enableIf);
void  Label (const RC<DynamicInt4> & dyn, const string & name, const EnableLabel & enableIf);
void  Label (const RC<DynamicUInt> & dyn, const string & name, const EnableLabel & enableIf);
void  Label (const RC<DynamicUInt2> & dyn, const string & name, const EnableLabel & enableIf);
void  Label (const RC<DynamicUInt3> & dyn, const string & name, const EnableLabel & enableIf);
void  Label (const RC<DynamicUInt4> & dyn, const string & name, const EnableLabel & enableIf);
void  Label (const RC<DynamicFloat> & dyn, const string & name, const EnableLabel & enableIf);
void  Label (const RC<DynamicFloat2> & dyn, const string & name, const EnableLabel & enableIf);
void  Label (const RC<DynamicFloat3> & dyn, const string & name, const EnableLabel & enableIf);
void  Label (const RC<DynamicFloat4> & dyn, const string & name, const EnableLabel & enableIf);
EnableLabel  EnableIfEqual (const RC<DynamicUInt> & dyn, uint ref);
EnableLabel  EnableIfGreater (const RC<DynamicUInt> & dyn, uint ref);
EnableLabel  EnableIfLess (const RC<DynamicUInt> & dyn, uint ref);
EnableLabel  EnableIfAnyBit (const RC<DynamicUInt> & dyn, uint ref);

// Read field from buffer and copy to dynamic variable.
void  ReadBuffer (const RC<DynamicInt> & dyn, const RC<Buffer> & buffer, const string & field);
void  ReadBuffer (const RC<DynamicInt2> & dyn, const RC<Buffer> & buffer, const string & field);
void  ReadBuffer (const RC<DynamicInt3> & dyn, const RC<Buffer> & buffer, const string & field);
void  ReadBuffer (const RC<DynamicInt4> & dyn, const RC<Buffer> & buffer, const string & field);
void  ReadBuffer (const RC<DynamicUInt> & dyn, const RC<Buffer> & buffer, const string & field);
void  ReadBuffer (const RC<DynamicUInt2> & dyn, const RC<Buffer> & buffer, const string & field);
void  ReadBuffer (const RC<DynamicUInt3> & dyn, const RC<Buffer> & buffer, const string & field);
void  ReadBuffer (const RC<DynamicUInt4> & dyn, const RC<Buffer> & buffer, const string & field);
void  ReadBuffer (const RC<DynamicFloat> & dyn, const RC<Buffer> & buffer, const string & field);
void  ReadBuffer (const RC<DynamicFloat2> & dyn, const RC<Buffer> & buffer, const string & field);
void  ReadBuffer (const RC<DynamicFloat3> & dyn, const RC<Buffer> & buffer, const string & field);
void  ReadBuffer (const RC<DynamicFloat4> & dyn, const RC<Buffer> & buffer, const string & field);

// Returns array with 3 elements, where x - wavelength in nm, yzw - RGB color in linear space.
void  WhiteColorSpectrum3 (array<float4> & wavelengthToRGB);

// Returns array with 7 elements, where x - wavelength in nm, yzw - RGB color in linear space.
// normalized - sum of colors will be 1.
void  WhiteColorSpectrum7 (array<float4> & wavelengthToRGB, bool normalized);

// Returns array 4 elements with visible light spectrum with step 100nm, where x - wavelength in nm, yzw - RGB color in linear space.
// normalized - sum of colors will be 1.
void  WhiteColorSpectrumStep100nm (array<float4> & wavelengthToRGB, bool normalized);

// Returns array 7 elements with visible light spectrum with step 50nm, where x - wavelength in nm, yzw - RGB color in linear space.
// normalized - sum of colors will be 1.
void  WhiteColorSpectrumStep50nm (array<float4> & wavelengthToRGB, bool normalized);

// Convert 2D regular grid on cube face to 3D position on cube.
float3  CM_CubeSC_Forward (const float3 & snormCoord_cubeFace);

// Convert 2D regular grid on cube face to 3D position on sphere using identity projection (normalization).
float3  CM_IdentitySC_Forward (const float3 & snormCoord_cubeFace);

// Convert 2D regular grid on cube face to 3D position on sphere using tangential projection.
float3  CM_TangentialSC_Forward (const float3 & snormCoord_cubeFace);
void  GetMarchingCubeTable (array<uint> & edgeTable, array<int> & triangleTable, array<float3> & uvw);
EGPUVendor  GPUVendor ();
bool  IsDiscreteGPU ();
bool  IsRemoteGPU ();
bool  Supports_GeometryShader ();
bool  Supports_MeshShader ();
bool  Supports_TessellationShader ();
bool  Supports_SamplerAnisotropy ();
EPixelFormat  Supported_DepthFormat ();
EPixelFormat  Supported_DepthStencilFormat ();
bool  Supports_AttachmentFormat (EPixelFormat);
bool  Supports_AttachmentBlendFormat (EPixelFormat);
bool  Supports_LinearSampledFormat (EPixelFormat);
bool  Supports_StorageImageFormat (EPixelFormat);
bool  Supports_StorageImageAtomicFormat (EPixelFormat);
uint  GetSubgroupSize ();
RC<FeatureSet>  GetFeatureSet ();
#define SCRIPT

template <>
struct RC<ComputeMip> : ComputeMip
{
	RC (const ComputeMip &);
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
struct RC<RemoteCamera> : RemoteCamera
{
	RC (const RemoteCamera &);
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
struct RC<Image> : Image
{
	RC (const Image &);
};

template <>
struct RC<GeomSource> : GeomSource
{
	RC (const GeomSource &);
};

template <>
struct RC<DynamicInt2> : DynamicInt2
{
	RC (const DynamicInt2 &);
};

template <>
struct RC<DynamicInt3> : DynamicInt3
{
	RC (const DynamicInt3 &);
};

template <>
struct RC<DynamicInt4> : DynamicInt4
{
	RC (const DynamicInt4 &);
};

template <>
struct RC<DynamicUInt3> : DynamicUInt3
{
	RC (const DynamicUInt3 &);
};

template <>
struct RC<DynamicUInt2> : DynamicUInt2
{
	RC (const DynamicUInt2 &);
};

template <>
struct RC<DynamicUInt4> : DynamicUInt4
{
	RC (const DynamicUInt4 &);
};

template <>
struct RC<DynamicFloat4> : DynamicFloat4
{
	RC (const DynamicFloat4 &);
};

template <>
struct RC<RTGeometry> : RTGeometry
{
	RC (const RTGeometry &);
};

template <>
struct RC<DynamicFloat3> : DynamicFloat3
{
	RC (const DynamicFloat3 &);
};

template <>
struct RC<DynamicFloat2> : DynamicFloat2
{
	RC (const DynamicFloat2 &);
};

template <>
struct RC<DynamicInt> : DynamicInt
{
	RC (const DynamicInt &);
};

template <>
struct RC<IPass> : IPass
{
	RC (const IPass &);
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
struct RC<Model> : Model
{
	RC (const Model &);
};

template <>
struct RC<DynamicUInt> : DynamicUInt
{
	RC (const DynamicUInt &);
};

template <>
struct RC<Scene> : Scene
{
	RC (const Scene &);
};

template <>
struct RC<OrbitalCamera> : OrbitalCamera
{
	RC (const OrbitalCamera &);
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
struct RC<RayTracingPass> : RayTracingPass
{
	RC (const RayTracingPass &);
};

template <>
struct RC<DynamicDim> : DynamicDim
{
	RC (const DynamicDim &);
};

template <>
struct RC<FPSCamera> : FPSCamera
{
	RC (const FPSCamera &);
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
struct RC<Postprocess> : Postprocess
{
	RC (const Postprocess &);
};

template <>
struct RC<SceneRayTracingPass> : SceneRayTracingPass
{
	RC (const SceneRayTracingPass &);
};

template <>
struct RC<RasterMip> : RasterMip
{
	RC (const RasterMip &);
};

template <>
struct RC<UnifiedGeometry> : UnifiedGeometry
{
	RC (const UnifiedGeometry &);
};

template <>
struct RC<Buffer> : Buffer
{
	RC (const Buffer &);
};

