//450ed71e
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

struct MeshPipelineSpec;
struct ComputePipelineSpec;
struct RenderState_RasterizationState;
struct RenderState_InputAssemblyState;
struct ubyte2;
struct ubyte3;
struct CompatibleRenderPass;
struct ubyte4;
struct GlobalConfig;
struct RenderState_RasterizationOrderAccess;
struct Shader;
struct ShaderStructType;
struct FS_DirectoryIterator;
struct RenderState_MultisampleState;
struct RayIndex;
struct bool4;
struct RGBA32i;
struct bool3;
struct bool2;
struct MultiViewMask;
struct HSVColor;
struct RGBA32f;
struct RenderState_ColorBuffer_ColorMask;
struct short3;
struct short2;
struct ushort4;
struct GraphicsPass;
struct sbyte3;
struct sbyte4;
struct Align;
struct ComputePipeline;
struct float4;
struct float3;
struct float2;
struct InstanceIndex;
struct DescriptorSetLayout;
struct RenderTechnique;
struct RayTracingShaderBinding;
struct RayTracingPipeline;
struct MultiSamples;
struct RenderState_StencilBufferState;
struct DepthStencil;
struct RenderState;
struct AttachmentSpec;
struct RayTracingPipelineSpec;
struct RenderState_ColorBuffersState;
struct MipmapLevel;
struct TilePipelineSpec;
struct TilePipeline;
struct GraphicsPipelineSpec;
struct FS_RecursiveDirectoryIter;
struct PipelineLayout;
struct int2;
struct int3;
struct ArraySize;
struct int4;
struct ImageLayer;
struct RenderPass;
struct RenderState_ColorBuffer;
struct ushort2;
struct sbyte2;
struct ushort3;
struct short4;
struct RGBA8u;
struct VertexBufferInput;
struct Attachment;
struct RGBA32u;
struct RenderState_DepthBufferState;
struct uint3;
struct uint2;
struct ShaderIO;
struct uint4;
struct RenderState_StencilFaceState;
struct FeatureSet;
struct NamedRenderState;
struct ComputePass;
struct MeshPipeline;
struct VertexDivisor;
struct Sampler;
struct CallableIndex;
struct GraphicsPipeline;

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

enum class EShaderVersion : uint32
{

	// Vulkan 1.0
	SPIRV_1_0,
	SPIRV_1_1,
	SPIRV_1_2,

	// Vulkan 1.1
	SPIRV_1_3,

	// Vulkan 1.1 extension
	SPIRV_1_4,

	// Vulkan 1.2
	SPIRV_1_5,

	// Vulkan 1.3
	SPIRV_1_6,

	// Vulkan 1.0
	Slang_SPIRV_1_0,
	Slang_SPIRV_1_1,
	Slang_SPIRV_1_2,

	// Vulkan 1.1
	Slang_SPIRV_1_3,

	// Vulkan 1.1 extension
	Slang_SPIRV_1_4,

	// Vulkan 1.2
	Slang_SPIRV_1_5,

	// Vulkan 1.3
	Slang_SPIRV_1_6,

	// Metal API
	Metal_2_0,
	Metal_2_1,
	Metal_2_2,

	// Added ray tracing.
	Metal_2_3,
	Metal_2_4,

	// Added mesh shading.
	Metal_3_0,
	Metal_3_1,
	Metal_3_2,
	Metal_4_0,

	// Compile for iOS.
	Metal_iOS_2_0,
	Metal_iOS_2_1,
	Metal_iOS_2_2,
	Metal_iOS_2_3,
	Metal_iOS_2_4,
	Metal_iOS_3_0,
	Metal_iOS_3_1,
	Metal_iOS_3_2,

	// Compile for MacOS.
	Metal_Mac_2_0,
	Metal_Mac_2_1,
	Metal_Mac_2_2,
	Metal_Mac_2_3,
	Metal_Mac_2_4,
	Metal_Mac_3_0,
	Metal_Mac_3_1,
	Metal_Mac_3_2,
};
uint32  operator | (EShaderVersion lhs, EShaderVersion rhs);
uint32  operator | (uint32 lhs, EShaderVersion rhs);
uint32  operator | (EShaderVersion lhs, uint32 rhs);

enum class EShaderOpt : uint32
{
	None,

	// Add debug information. Used in RenderDoc shader debugger.
	DebugInfo,

	// Insert shader trace recording. Shader will be very slow.
	Trace,

	// Insert shader function profiling. Shader will be very slow.
	FnProfiling,

	// Insert whole shader time measurement. Shader will be a bit slow.
	TimeHeatMap,

	// Enable optimizations. Take a lot of CPU time at shader compilation.
	Optimize,

	// Enable bytecode size optimizations. Take a lot of CPU time at shader compilation.
	OptimizeSize,

	// Enable strong optimizations. Take a lot of CPU time at shader compilation.
	StrongOptimization,
	WarnAsError,
};
uint32  operator | (EShaderOpt lhs, EShaderOpt rhs);
uint32  operator | (uint32 lhs, EShaderOpt rhs);
uint32  operator | (EShaderOpt lhs, uint32 rhs);

enum class EAccessType : uint32
{
	Coherent,
	Volatile,
	Restrict,

	// Require 'vulkanMemoryModel' feature.
	DeviceCoherent,
	QueueFamilyCoherent,
	WorkgroupCoherent,
	SubgroupCoherent,
	NonPrivate,
};
uint32  operator | (EAccessType lhs, EAccessType rhs);
uint32  operator | (uint32 lhs, EAccessType rhs);
uint32  operator | (EAccessType lhs, uint32 rhs);

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

enum class ECompilationTarget : uint32
{
	Vulkan,
	Metal_iOS,
	Metal_Mac,
};
uint32  operator | (ECompilationTarget lhs, ECompilationTarget rhs);
uint32  operator | (uint32 lhs, ECompilationTarget rhs);
uint32  operator | (ECompilationTarget lhs, uint32 rhs);

enum class EStructLayout : uint8
{

	// Apply GLSL std140 rules but structure must be compatible with Metal and HLSL cbuffer, otherwise will throw exception.
	Compatible_Std140,

	// Apply GLSL std430 rules but structure must be compatible with Metal and HLSL StructBuffer, otherwise will throw exception.
	Compatible_Std430,

	// Apply MSL rules.
	Metal,

	// Apply GLSL std140 rules.
	Std140,

	// Apply GLSL std430 rules.
	Std430,

	// Platform depended layout.
	InternalIO,

	// Apply HLSL cbuffer rules.
	HLSL_Const,

	// Apply HLSL structured buffer rules.
	HLSL_Struct,
};
uint8  operator | (EStructLayout lhs, EStructLayout rhs);
uint8  operator | (uint8 lhs, EStructLayout rhs);
uint8  operator | (EStructLayout lhs, uint8 rhs);

enum class EValueType : uint8
{
	Bool8,
	Bool32,
	Int8,
	Int16,
	Int32,
	Int64,
	UInt8,
	UInt16,
	UInt32,
	UInt64,
	Float16,
	Float32,
	Float64,
	Int8_Norm,
	Int16_Norm,
	UInt8_Norm,
	UInt16_Norm,
	DeviceAddress,
};
uint8  operator | (EValueType lhs, EValueType rhs);
uint8  operator | (uint8 lhs, EValueType rhs);
uint8  operator | (EValueType lhs, uint8 rhs);

enum class EShaderPreprocessor : uint32
{
	None,

	// Use <aestyle.glsl.h> for auto-complete in IDE.
	AEStyle,
};
uint32  operator | (EShaderPreprocessor lhs, EShaderPreprocessor rhs);
uint32  operator | (uint32 lhs, EShaderPreprocessor rhs);
uint32  operator | (EShaderPreprocessor lhs, uint32 rhs);

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

enum class ETessPatch : uint32
{
	Points,
	Isolines,
	Triangles,
	Quads,
};
uint32  operator | (ETessPatch lhs, ETessPatch rhs);
uint32  operator | (uint32 lhs, ETessPatch rhs);
uint32  operator | (ETessPatch lhs, uint32 rhs);

enum class ETessSpacing : uint32
{
	Equal,
	FractionalEven,
	FractionalOdd,
};
uint32  operator | (ETessSpacing lhs, ETessSpacing rhs);
uint32  operator | (uint32 lhs, ETessSpacing rhs);
uint32  operator | (ETessSpacing lhs, uint32 rhs);

enum class ShaderStructTypeUsage : uint32
{

	// Used as interface between graphics pipeline stages. Reflection to C++ is not supported.
	ShaderIO,

	// Used as vertex buffer layout. Enables reflection to C++.
	VertexLayout,

	// Used as vertex attributes in shader.
	// Reflection to C++ is not enabled, use 'VertexLayout' to enable it.
	VertexAttribs,

	// Used as uniform/storage buffer. Layout must be same in GLSL/MSL and C++. Enables reflection to C++.
	BufferLayout,

	// Used as buffer reference in shader. Layout must be same between shaders in single platform.
	// Reflection to C++ is not enabled, use 'BufferLayout' to enable it.
	BufferReference,
};
uint32  operator | (ShaderStructTypeUsage lhs, ShaderStructTypeUsage rhs);
uint32  operator | (uint32 lhs, ShaderStructTypeUsage rhs);
uint32  operator | (ShaderStructTypeUsage lhs, uint32 rhs);

enum class EMutableRenderState : uint32
{
};
uint32  operator | (EMutableRenderState lhs, EMutableRenderState rhs);
uint32  operator | (uint32 lhs, EMutableRenderState rhs);
uint32  operator | (EMutableRenderState lhs, uint32 rhs);

enum class EAttachment : uint32
{

	// Discard previous content. Used as optimization for TBDR architectures.
	Invalidate,

	// Color attachment.
	Color,

	// Resolve attachment - will get content from multisampled color attachment.
	ColorResolve,

	// Used as input attachment and color attachment.
	// Only one primitive can modify pixel, otherwise pipeline barrier is required between draws.
	ReadWrite,

	// Input attachment.
	Input,

	// Depth attachment.
	Depth,

	// Depth and stencil attachment.
	DepthStencil,

	// Keep attachment content between passes.
	Preserve,

	// Fragment shading rate attachment.
	// Requires 'attachmentFragmentShadingRate' feature.
	ShadingRate,

	// Fragment density (read-only) attachment.
	// Requires 'fragmentDensityMap' feature.
	FragmentDensity,

	// Used as input attachment and color attachment.
	// Allows to access framebuffer content in rasterization order, without explicit synchronization.
	// Awailable in TBDR architectures. Requires 'rasterizationOrderColorAttachmentAccess' feature.
	RasterOrder,
};
uint32  operator | (EAttachment lhs, EAttachment rhs);
uint32  operator | (uint32 lhs, EAttachment rhs);
uint32  operator | (EAttachment lhs, uint32 rhs);

enum class EAttachmentLoadOp : uint8
{

	// Previous content will not be preserved.
	// In TBDR it allow to avoid transfer from global memory to cache.
	Invalidate,

	// Preserve attachment content.
	// In TBDR contents in global memory will be copied to cache.
	Load,

	// Clear attachment before first pass.
	// In TBDR it allow to avoid transfer from global memory to cache.
	Clear,

	// Attachment is not used at all.
	// Can be used to keep one compatible render pass and avoid unnecessary synchronizations for unused attachment.
	None,
};
uint8  operator | (EAttachmentLoadOp lhs, EAttachmentLoadOp rhs);
uint8  operator | (uint8 lhs, EAttachmentLoadOp rhs);
uint8  operator | (EAttachmentLoadOp lhs, uint8 rhs);

enum class EAttachmentStoreOp : uint8
{

	// Attachment content will not needed after rendering.
	// In TBDR it allow to avoid transfer from cache to global memory.
	Invalidate,

	// Attachment content will be written to global memory.
	Store,

	// Attachment is read-only. Content may not be written to memory, but if changed then content in memory will be undefined.
	// In TBDR it allow to avoid transfer from cache to global memory.
	None,
};
uint8  operator | (EAttachmentStoreOp lhs, EAttachmentStoreOp rhs);
uint8  operator | (uint8 lhs, EAttachmentStoreOp rhs);
uint8  operator | (EAttachmentStoreOp lhs, uint8 rhs);

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
struct FS_DirectoryIterator
{
	FS_DirectoryIterator ();
	FS_DirectoryIterator (const FS_DirectoryIterator&);
	FS_DirectoryIterator&  operator = (const FS_DirectoryIterator&);
	FS_DirectoryIterator (const string & folder);
	bool  IsFile () const;
	bool  IsDirectory () const;
	bool  IsValid () const;
	void  Inc ();
	string  Path () const;
	string  Extension () const;
	string  FileName () const;
	string  Stem () const;
	string  ParentPath () const;
};

struct FS_RecursiveDirectoryIter
{
	FS_RecursiveDirectoryIter ();
	FS_RecursiveDirectoryIter (const FS_RecursiveDirectoryIter&);
	FS_RecursiveDirectoryIter&  operator = (const FS_RecursiveDirectoryIter&);
	FS_RecursiveDirectoryIter (const string & folder);
	bool  IsFile () const;
	bool  IsDirectory () const;
	bool  IsValid () const;
	void  Inc ();
	string  Path () const;
	string  Extension () const;
	string  FileName () const;
	string  Stem () const;
	string  ParentPath () const;
};

string  ReadTextFile (const string & path);
void  WriteFile (const string & path, const string & text);
void  AppendFile (const string & path, const string & text);
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

	// Stencil test compare operator.
	// if '(stencilAttachment & CompareMask) [CompareOp] (Reference & CompareMask)' then sample passed stencil test.
	void  CompareOp (ECompareOp);
	void  Reference (uint);
	void  CompareMask (uint);

	// Action performed on samples that fail the stencil test.
	// 'stencilValue = FailOp( stencilAttachment )'
	// See 'CompareOp', 'Reference' and 'CompareMask' to know how stencil test is performed.
	void  FailOp (EStencilOp);

	// Action performed on samples that pass the stencil test and fail the depth test.
	// 'stencilValue = DepthFailOp( stencilAttachment )'
	// Depth test happens after stencil test and before stencil update.
	void  DepthFailOp (EStencilOp);

	// Action performed on samples that pass both the depth and stencil tests.
	// 'stencilValue = PassOp( stencilAttachment )'
	void  PassOp (EStencilOp);

	// Bitmask which is ANDed with new stencil value and stencil attachment value before updating stencil attachment.
	// 'stencilAttachment = (stencilAttachment & WriteMask) | (stencilValue & WriteMask)'
	void  WriteMask (uint);
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
	uint8 lineWidth;
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

struct RenderState_RasterizationOrderAccess
{
	RenderState_RasterizationOrderAccess ();
	RenderState_RasterizationOrderAccess (const RenderState_RasterizationOrderAccess&);
	RenderState_RasterizationOrderAccess&  operator = (const RenderState_RasterizationOrderAccess&);
	bool color;
	bool depth;
	bool stencil;
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
	RenderState_RasterizationOrderAccess rasterOrderAccess;
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

	// Set defines which will be used in all shaders.
	// Format: MACROS = value \n DEF \n ...
	void  SetShaderDefines (const string &);

	// Set default DescriptorSetLayout usage.
	void  SetDefaultDescSetUsage (EDescSetUsage);
	void  SetDefaultDescSetUsage (uint);
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
	// Format: MACROS = value \n DEF \n ...
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
	// Format: MACROS = value \n DEF \n ...
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
	void  StorageTexelBuffer2 (EShaderStages shaderStages, const string & uniform, EImageType imageType);
	void  StorageTexelBuffer2 (uint shaderStages, const string & uniform, EImageType imageType);
	void  StorageTexelBuffer2 (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType);
	void  StorageTexelBuffer2 (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType);
	void  StorageTexelBuffer2 (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state);
	void  StorageTexelBuffer2 (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state);
	void  StorageTexelBuffer2 (EShaderStages shaderStages, const string & uniform, EImageType imageType, EAccessType access);
	void  StorageTexelBuffer2 (uint shaderStages, const string & uniform, EImageType imageType, EAccessType access);
	void  StorageTexelBuffer2 (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access);
	void  StorageTexelBuffer2 (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access);
	void  StorageTexelBuffer2 (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access, EResourceState state);
	void  StorageTexelBuffer2 (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access, EResourceState state);

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
	void  StorageImage2 (EShaderStages shaderStages, const string & uniform, EImageType imageType);
	void  StorageImage2 (uint shaderStages, const string & uniform, EImageType imageType);
	void  StorageImage2 (EShaderStages shaderStages, const string & uniform, EImageType imageType, EAccessType access);
	void  StorageImage2 (uint shaderStages, const string & uniform, EImageType imageType, EAccessType access);
	void  StorageImage2 (EShaderStages shaderStages, const string & uniform, EImageType imageType, EResourceState state);
	void  StorageImage2 (uint shaderStages, const string & uniform, EImageType imageType, EResourceState state);
	void  StorageImage2 (EShaderStages shaderStages, const string & uniform, EImageType imageType, EAccessType access, EResourceState state);
	void  StorageImage2 (uint shaderStages, const string & uniform, EImageType imageType, EAccessType access, EResourceState state);
	void  StorageImage2 (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType);
	void  StorageImage2 (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType);
	void  StorageImage2 (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access);
	void  StorageImage2 (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access);
	void  StorageImage2 (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state);
	void  StorageImage2 (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EResourceState state);
	void  StorageImage2 (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access, EResourceState state);
	void  StorageImage2 (uint shaderStages, const string & uniform, const ArraySize & arraySize, EImageType imageType, EAccessType access, EResourceState state);

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

	// Add ray tracing scene (top level acceleration structure, TLAS).
	void  RayTracingScene (EShaderStages shaderStages, const string & uniform);
	void  RayTracingScene (uint shaderStages, const string & uniform);
	void  RayTracingScene (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize);
	void  RayTracingScene (uint shaderStages, const string & uniform, const ArraySize & arraySize);

	// Add ray tracing partitioned scene (partitioned top level acceleration structure, PTLAS).
	void  RayTracingPartitionedScene (EShaderStages shaderStages, const string & uniform);
	void  RayTracingPartitionedScene (uint shaderStages, const string & uniform);
	void  RayTracingPartitionedScene (EShaderStages shaderStages, const string & uniform, const ArraySize & arraySize);
	void  RayTracingPartitionedScene (uint shaderStages, const string & uniform, const ArraySize & arraySize);

	// Check is image description is supported by feature set.
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImageOpt options, const ImageLayer & arrayLayers, const MultiSamples & samples);
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImageOpt options, const MultiSamples & samples);
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImageOpt options, const ImageLayer & arrayLayers);
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImageOpt options);
	bool  IsSupported (EPixelFormat format, EImageUsage usage);

	// Check is image view description is supported by feature set.
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImageOpt options, const ImageLayer & arrayLayers, const MultiSamples & samples, EImage imageType, EPixelFormat viewFormat, EImageUsage viewUsage);
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImageOpt options, const ImageLayer & arrayLayers, const MultiSamples & samples, EImage imageType, EPixelFormat viewFormat);
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImageOpt options, const ImageLayer & arrayLayers, const MultiSamples & samples, EImage imageType);
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImageOpt options, const ImageLayer & arrayLayers, EImage imageType, EPixelFormat viewFormat, EImageUsage viewUsage);
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImageOpt options, const ImageLayer & arrayLayers, EImage imageType, EPixelFormat viewFormat);
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImageOpt options, const ImageLayer & arrayLayers, EImage imageType);
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImageOpt options, EImage imageType, EPixelFormat viewFormat, EImageUsage viewUsage);
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImageOpt options, EImage imageType, EPixelFormat viewFormat);
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImageOpt options, EImage imageType);
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImage imageType, EPixelFormat viewFormat, EImageUsage viewUsage);
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImage imageType, EPixelFormat viewFormat);
	bool  IsSupported (EPixelFormat format, EImageUsage usage, EImage imageType);

	// Check is buffer description is supported by feature set.
	bool  IsSupported (EBufferUsage usage, EBufferOpt options);
	bool  IsSupported (EBufferUsage usage);

	// Check is buffer view description is supported by feature set.
	bool  IsSupported (EBufferUsage usage, EBufferOpt options, EPixelFormat format);
	bool  IsSupported (EBufferUsage usage, EPixelFormat format);
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
	// Format: MACROS = value \n DEF \n ...
	void  Define (const string &);
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
	// All pipelines must contains this DSLayout in 0 binding.
	void  SetDSLayout (const string & typeName);
	void  SetDSLayout (const RC<DescriptorSetLayout> & dsl);
};

struct ComputePass
{
	ComputePass ();

	// Set per-pass descriptor set layout.
	// All pipelines must contains this DSLayout in 0 binding.
	void  SetDSLayout (const string & typeName);
	void  SetDSLayout (const RC<DescriptorSetLayout> & dsl);
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
	// Render technique will create all attached pipelines during its creation.
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
	// Format: MACROS = value \n DEF \n ...
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

	// Task shader workgroup size will be set at load time in 'RenderTechDesc::taskLocalSize'.
	void  LoadTimeTaskLocalSize ();

	// Set mesh shader workgroup size. All threads in workgroup can use same (shared) memory.
	// Shader must use 'SetMeshSpec1/2/3()' to define specialization constant.
	void  SetMeshLocalSize (uint x);
	void  SetMeshLocalSize (uint x, uint y);
	void  SetMeshLocalSize (uint x, uint y, uint z);

	// Mesh shader workgroup size will be set at load time in 'RenderTechDesc::meshLocalSize'.
	void  LoadTimeMeshLocalSize ();

	// Attach pipeline to the render technique.
	// Render technique will create all attached pipelines during its creation.
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
	// Format: MACROS = value \n DEF \n ...
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

	// Set subgroup size.
	// Requires 'subgroupSizeControl' feature, value must be in range [minSubgroupSize, maxSubgroupSize].
	void  SubgroupSize (uint);

	// Set dynamic states (EPipelineDynamicState).
	// None of the states are supported for compute pipeline.
	void  SetDynamicState (uint states);

	// Set compute shader workgroup size. All threads in workgroup can use same (shared) memory.
	// Shader must use 'ComputeSpec1/2/3()' to define specialization constant.
	void  SetLocalSize (uint x);
	void  SetLocalSize (uint x, uint y);
	void  SetLocalSize (uint x, uint y, uint z);

	// Compute shader workgroup size will be set at load time in 'RenderTechDesc::computeLocalSize'.
	void  LoadTimeLocalSize ();

	// Attach pipeline to the render technique.
	// Render technique will create all attached pipelines during its creation.
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
	// Format: MACROS = value \n DEF \n ...
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
	// Render technique will create all attached pipelines during its creation.
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
	// Format: MACROS = value \n DEF \n ...
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
	// Render technique will create all attached pipelines during its creation.
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
	// Format: MACROS = value \n DEF \n ...
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

struct MultiViewMask
{
	MultiViewMask ();
	MultiViewMask (const MultiViewMask&);
	MultiViewMask&  operator = (const MultiViewMask&);
	MultiViewMask (uint);
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
	// Use 'Subpass_ExternalIn' and 'Subpass_ExternalOut' to define state before and after render pass.
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
	void  AddSubpass (const string & subpassName, const MultiViewMask & viewMask);

	// Add FeatureSet to the render pass.
	// Render pass can use only features that are enabled in at least one FeatureSet.
	void  AddFeatureSet (const string & fsName);

	// Add indices of view which can be rendered concurrently.
	void  AddMultiViewCorrelatedViewMask (uint bitMask);

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
bool  HasDescriptorSetLayout (const string &);
bool  HasPipelineLayout (const string &);
string  FileName ();
RC<FeatureSet>  GetDefaultFeatureSet ();
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
const string Sampler_NearestClampSubsampled;
const string Sampler_MaxLinearClamp;
const string Sampler_MinLinearClamp;
template <>
struct RC<MeshPipelineSpec> : MeshPipelineSpec
{
	RC (const MeshPipelineSpec &);
};

template <>
struct RC<ComputePipelineSpec> : ComputePipelineSpec
{
	RC (const ComputePipelineSpec &);
};

template <>
struct RC<CompatibleRenderPass> : CompatibleRenderPass
{
	RC (const CompatibleRenderPass &);
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
struct RC<GraphicsPass> : GraphicsPass
{
	RC (const GraphicsPass &);
};

template <>
struct RC<ComputePipeline> : ComputePipeline
{
	RC (const ComputePipeline &);
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
struct RC<GraphicsPipelineSpec> : GraphicsPipelineSpec
{
	RC (const GraphicsPipelineSpec &);
};

template <>
struct RC<PipelineLayout> : PipelineLayout
{
	RC (const PipelineLayout &);
};

template <>
struct RC<RenderPass> : RenderPass
{
	RC (const RenderPass &);
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
struct RC<FeatureSet> : FeatureSet
{
	RC (const FeatureSet &);
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
struct RC<MeshPipeline> : MeshPipeline
{
	RC (const MeshPipeline &);
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

