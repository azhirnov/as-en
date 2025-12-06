//a9d0c05b
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

struct ubyte4;
struct ubyte2;
struct ubyte3;
struct PaddingLayout;
struct RGBA32f;
struct bool3;
struct HSVColor;
struct bool2;
struct bool4;
struct RGBA32i;
struct float4;
struct ButtonController;
struct AlignedLayout;
struct sbyte3;
struct Texture;
struct sbyte4;
struct short3;
struct ushort4;
struct short2;
struct UIColorStyle;
struct MultiSamples;
struct float3;
struct float2;
struct RasterFont;
struct MipmapLevel;
struct FixedLayout;
struct Material;
struct Mesh;
struct DepthStencil;
struct sbyte2;
struct ushort2;
struct short4;
struct ushort3;
struct SharedImage;
struct BaseUIDrawable;
struct int3;
struct ImageDrawable;
struct int4;
struct ImageLayer;
struct int2;
struct Model;
struct RectU;
struct UIImageStyle;
struct uint2;
struct uint3;
struct uint4;
struct RectI;
struct RectangleDrawable;
struct MetaData;
struct FillStackLayout;
struct RectF;
struct RGBA32u;
struct ImageAtlas;
struct RGBA8u;
struct BaseUIController;
struct UIWidget;
struct BaseLayout;
struct UIStyleCollection;

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
	Intel_Gen12_7,
	PowerVR_Series8,
	PowerVR_Series9,
	PowerVR_SeriesA,
	PowerVR_SeriesB,
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

enum class ECubeFace : uint32
{
	XPos,
	XNeg,
	YPos,
	YNeg,
	ZPos,
	ZNeg,
};
uint32  operator | (ECubeFace lhs, ECubeFace rhs);
uint32  operator | (uint32 lhs, ECubeFace rhs);
uint32  operator | (ECubeFace lhs, uint32 rhs);

enum class ERasterFontMode : uint32
{

	// Raster font, glyph dimension on screen should be multiple of glyph bitmap size.
	Raster,

	// SDF font, can be scaled, but have smooth angles.
	SDF,

	// Multichannel SDF font, can be scaled, have sharp angles, but a bit less performance.
	MC_SDF,
};
uint32  operator | (ERasterFontMode lhs, ERasterFontMode rhs);
uint32  operator | (uint32 lhs, ERasterFontMode rhs);
uint32  operator | (ERasterFontMode lhs, uint32 rhs);

enum class ELayoutType : uint8
{
	FixedLayoutPx,
	FixedLayoutMm,
	PaddingLayoutPx,
	PaddingLayoutMm,
	PaddingLayoutRel,
	AlignedLayoutPx,
	AlignedLayoutMm,
	AlignedLayoutRel,
	StackLayoutL,
	StackLayoutR,
	StackLayoutB,
	StackLayoutT,
	FillStackLayout,
};
uint8  operator | (ELayoutType lhs, ELayoutType rhs);
uint8  operator | (uint8 lhs, ELayoutType rhs);
uint8  operator | (ELayoutType lhs, uint8 rhs);

enum class ELayoutAlign : uint8
{
	Left,
	Right,
	Bottom,
	Top,
	CenterX,
	CenterY,
	FillX,
	FillY,
	Center,
	Fill,
};
uint8  operator | (ELayoutAlign lhs, ELayoutAlign rhs);
uint8  operator | (uint8 lhs, ELayoutAlign rhs);
uint8  operator | (ELayoutAlign lhs, uint8 rhs);

enum class EStackOrigin : uint8
{
	Left,
	Right,
	Bottom,
	Top,
};
uint8  operator | (EStackOrigin lhs, EStackOrigin rhs);
uint8  operator | (uint8 lhs, EStackOrigin rhs);
uint8  operator | (EStackOrigin lhs, uint8 rhs);

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

string  FindAndReplace (const string &, const string &, const string &);
bool  StartsWith (const string &, const string &);
bool  StartsWithIC (const string &, const string &);
bool  EndsWith (const string &, const string &);
bool  EndsWithIC (const string &, const string &);
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

struct MetaData
{
	MetaData ();
	void  Store (const string & nameInArchive);
};

struct SharedImage
{
	SharedImage ();
	void  Store (const string & nameInArchive);
	void  Format (EPixelFormat newFormat);
	void  PutMeta (const RC<MetaData> & metaFile, const string & nameInMeta);
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
	void  Add (const string & imageNameInAtlas, const string & filename, const RectU & regionInSrcImage);
	void  Store (const string & nameInArchive);
	void  StoreData (const string & nameInArchive);
	void  PutMeta (const RC<MetaData> & metaFile, const string & nameInMeta);
	void  PutData (const RC<SharedImage> & image);
	void  Padding (uint paddingInPixels);
	void  Format (EPixelFormat newFormat);
};

struct RasterFont
{
	RasterFont ();
	void  Load (const string & fontFile);
	void  AddCharset (uint firstCharIndexInUnicode, uint lastCharIndexInUnicode);
	void  AddCharset_Ascii ();
	void  AddCharset_Rus ();
	void  GlyphSize (uint heightInPixels);
	void  GlyphPadding (uint paddingInPixels);
	void  Store (const string & nameInArchive);
	void  PutMeta (const RC<MetaData> & metaFile, const string & nameInMeta);
	void  PutData (const RC<SharedImage> & image);
	void  Format (EPixelFormat newFormat);
	void  RasterMode (ERasterFontMode);
	void  SDFGlyphBorder (uint borderSizeInPixels);
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

struct UIColorStyle
{
	void  Disabled (const RGBA8u & colorWhenDisabled);
	void  Enabled (const RGBA8u & colorWhenEnabled);
	void  MouseOver (const RGBA8u & colorWhenMouseOver);
	void  TouchDown (const RGBA8u & colorWhenTouchDown);
	void  Selected (const RGBA8u & colorWhenSelected);
	void  Pipeline (const string & pplnName);
};

struct UIImageStyle
{
	void  Disabled (const RGBA8u & colorWhenDisabled);
	void  Enabled (const RGBA8u & colorWhenEnabled);
	void  MouseOver (const RGBA8u & colorWhenMouseOver);
	void  TouchDown (const RGBA8u & colorWhenTouchDown);
	void  Selected (const RGBA8u & colorWhenSelected);
	void  Disabled (const RGBA8u & colorWhenDisabled, float scale);
	void  Enabled (const RGBA8u & colorWhenEnabled, float scale);
	void  MouseOver (const RGBA8u & colorWhenMouseOver, float scale);
	void  TouchDown (const RGBA8u & colorWhenTouchDown, float scale);
	void  Selected (const RGBA8u & colorWhenSelected, float scale);
	void  Pipeline (const string & pplnName);
	void  Image (const string & atlasMetaResName, const string & imageInAtlas);
};

struct UIStyleCollection
{
	UIStyleCollection ();
	void  Resources (const string & metaDataFileNameInArchive);
	void  DebugPipeline (const string & pplnName);
	RC<UIColorStyle>  AddColorStyle (const string & name);
	RC<UIImageStyle>  AddImageStyle (const string & name);
	void  Store (const string & nameInArchive);
};

struct BaseUIDrawable
{
};

struct RectangleDrawable
{
	RectangleDrawable (const string & styleName);
};

struct ImageDrawable
{
	ImageDrawable (const string & styleName);
};

struct BaseUIController
{
};

struct ButtonController
{
	ButtonController ();
	void  OnClick (const string &);
	void  OnDoubleClick (const string &);
	void  OnLongPress (const string &);
};

struct BaseLayout
{
};

struct FixedLayout
{
	FixedLayout ();
	FixedLayout (const ELayoutType &);
	void  Region (const RectF &);
	void  AddChild (const RC<BaseLayout> &);
	void  SetDrawable (const RC<BaseUIDrawable> &);
	void  SetController (const RC<BaseUIController> &);
};

struct PaddingLayout
{
	PaddingLayout ();
	PaddingLayout (const ELayoutType &);
	void  PaddingX (float, float);
	void  PaddingY (float, float);
	void  Padding (float);
	void  AddChild (const RC<BaseLayout> &);
	void  SetDrawable (const RC<BaseUIDrawable> &);
	void  SetController (const RC<BaseUIController> &);
};

struct AlignedLayout
{
	AlignedLayout ();
	AlignedLayout (const ELayoutType &);
	void  Size (float width, float height);
	void  Size (const float2 &);
	void  Align (ELayoutAlign);
	void  Align (int);
	void  AddChild (const RC<BaseLayout> &);
	void  SetDrawable (const RC<BaseUIDrawable> &);
	void  SetController (const RC<BaseUIController> &);
};

struct FillStackLayout
{
	FillStackLayout ();
	void  Origin (EStackOrigin);
	void  AddChild (const RC<BaseLayout> &);
	void  SetDrawable (const RC<BaseUIDrawable> &);
	void  SetController (const RC<BaseUIController> &);
};

struct UIWidget
{
	UIWidget ();
	void  Initialize (const RC<BaseLayout> &);
	void  Store (const string &);
};

template <>
struct RC<PaddingLayout> : PaddingLayout
{
	RC (const PaddingLayout &);
};

template <>
struct RC<ButtonController> : ButtonController
{
	RC (const ButtonController &);
};

template <>
struct RC<AlignedLayout> : AlignedLayout
{
	RC (const AlignedLayout &);
};

template <>
struct RC<Texture> : Texture
{
	RC (const Texture &);
};

template <>
struct RC<UIColorStyle> : UIColorStyle
{
	RC (const UIColorStyle &);
};

template <>
struct RC<RasterFont> : RasterFont
{
	RC (const RasterFont &);
};

template <>
struct RC<FixedLayout> : FixedLayout
{
	RC (const FixedLayout &);
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
struct RC<SharedImage> : SharedImage
{
	RC (const SharedImage &);
};

template <>
struct RC<BaseUIDrawable> : BaseUIDrawable
{
	RC (const BaseUIDrawable &);
};

template <>
struct RC<ImageDrawable> : ImageDrawable
{
	RC (const ImageDrawable &);
};

template <>
struct RC<Model> : Model
{
	RC (const Model &);
};

template <>
struct RC<UIImageStyle> : UIImageStyle
{
	RC (const UIImageStyle &);
};

template <>
struct RC<RectangleDrawable> : RectangleDrawable
{
	RC (const RectangleDrawable &);
};

template <>
struct RC<MetaData> : MetaData
{
	RC (const MetaData &);
};

template <>
struct RC<FillStackLayout> : FillStackLayout
{
	RC (const FillStackLayout &);
};

template <>
struct RC<ImageAtlas> : ImageAtlas
{
	RC (const ImageAtlas &);
};

template <>
struct RC<BaseUIController> : BaseUIController
{
	RC (const BaseUIController &);
};

template <>
struct RC<UIWidget> : UIWidget
{
	RC (const UIWidget &);
};

template <>
struct RC<BaseLayout> : BaseLayout
{
	RC (const BaseLayout &);
};

template <>
struct RC<UIStyleCollection> : UIStyleCollection
{
	RC (const UIStyleCollection &);
};

