// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	enum class EMemoryType : ubyte
	{
		// device memory
		DeviceLocal			= 1 << 0,
		Transient			= 1 << 1,		// for render target only: allocate in on-chip memory, available only inside render pass

		// host memory
		HostCocherent		= 1 << 2,		// for small host writa only memory and for host read only memory
		HostCached			= 1 << 3,		// cached non-coherent memory type, for large host writa only memory

		// options
		Dedicated			= 1 << 4,		// force to use dedicated allocation
		_External			= 1 << 5,		// for internal use - don't destroy object
		_Last,
		
		All					= ((_Last-1) << 1) - 1,
		HostCachedCocherent	= HostCocherent | HostCached,
		Unified				= HostCocherent | DeviceLocal,			// Metal: don't read from host!
		UnifiedCached		= HostCachedCocherent | DeviceLocal,
		Unknown				= 0,
	};
	AE_BIT_OPERATORS( EMemoryType );


	enum class EBufferUsage : uint
	{
		TransferSrc			= 1 << 0,
		TransferDst			= 1 << 1,
		UniformTexel		= 1 << 2,		// glsl: 'uniform samplerBuffer'
		StorageTexel		= 1 << 3,		// glsl: 'uniform imageBuffer'
		Uniform				= 1 << 4,		// uniform buffer
		Storage				= 1 << 5,		// shader storage buffer
		Index				= 1 << 6,		// index buffer
		Vertex				= 1 << 7,		// vertex buffer
		Indirect			= 1 << 8,		// indirect buffer for draw and dispatch
		ShaderAddress		= 1 << 9,		// shader device address

		// ray tracing & acceleration structure
		ShaderBindingTable	= 1 << 10,		// shader binding table for ray tracing
		ASBuild_ReadOnly	= 1 << 11,		// vertex, index, AABB, instance data, transform buffers
		ASBuild_Scratch		= 1 << 12,		// scratch buffer for building acceleration structures
		_Last,
		
		All					= ((_Last-1) << 1) - 1,
		Transfer			= TransferDst | TransferSrc,
		Unknown				= 0,
	};
	AE_BIT_OPERATORS( EBufferUsage );


	enum class EBufferOpt : uint
	{
		SparseResidency			= 1 << 0,		// allows unbound ranges
		SparseAliased			= 1 << 1,		// allows bind memory block to multiple ranges or different buffers
		VertexPplnStore			= 1 << 2,		// storage buffer store and atomic operations in vertex, geometry, tessellation shaders
		FragmentPplnStore		= 1 << 3,		// storage buffer store and atomic operations in fragment shader
		StorageTexelAtomic		= 1 << 4,		// atomic ops on imageBuffer
		_Last,
		
		SparseResidencyAliased	= SparseResidency | SparseAliased,
		All						= ((_Last-1) << 1) - 1,
		Unknown					= 0,
	};
	AE_BIT_OPERATORS( EBufferOpt );

	
	enum class EImageDim : ubyte
	{
		_1D,
		_2D,
		_3D,
		OneDim		= _1D,
		TwoDim		= _2D,
		ThreeDim	= _3D,
		Unknown		= 0xFF,
	};
	
	static constexpr auto	EImageDim_1D	= EImageDim::_1D;
	static constexpr auto	EImageDim_2D	= EImageDim::_2D;
	static constexpr auto	EImageDim_3D	= EImageDim::_3D;


	enum class EImage : ubyte
	{
		_1D,
		_2D,
		_3D,
		_1DArray,
		_2DArray,
		Cube,
		CubeArray,
		OneDim			= _1D,
		TwoDim			= _2D,
		ThreeDim		= _3D,
		OneDimArray		= _1DArray,
		TwoDimArray		= _2DArray,
		Unknown			= 0xFF,
	};

	static constexpr auto	EImage_1D			= EImage::_1D;
	static constexpr auto	EImage_2D			= EImage::_2D;
	static constexpr auto	EImage_3D			= EImage::_3D;
	static constexpr auto	EImage_1DArray		= EImage::_1DArray;
	static constexpr auto	EImage_2DArray		= EImage::_2DArray;
	static constexpr auto	EImage_Cube			= EImage::Cube;
	static constexpr auto	EImage_CubeArray	= EImage::CubeArray;

	
	enum class EImageUsage : uint
	{
		TransferSrc					= 1 << 0,		// for all copy operations
		TransferDst					= 1 << 1,		// for all copy operations
		Sampled						= 1 << 2,		// access in shader as texture
		Storage						= 1 << 3,		// access in shader as image
		ColorAttachment				= 1 << 4,		// color or resolve attachment
		DepthStencilAttachment		= 1 << 5,		// depth/stencil attachment
		InputAttachment				= 1 << 6,		// input attachment in shader
		ShadingRate					= 1 << 7,
		FragmentDensityMap			= 1 << 8,
		_Last,

		All							= ((_Last-1) << 1) - 1,
		Transfer					= TransferSrc | TransferDst,
		RWAttachment				= ColorAttachment | InputAttachment,
		Unknown						= 0,
	};
	AE_BIT_OPERATORS( EImageUsage );
	

	enum class EImageOpt : uint
	{
		CubeCompatible				= 1 << 0,		// allows to create CubeMap and CubeMapArray from 2D Array
		MutableFormat				= 1 << 1,		// allows to change image format
		Array2DCompatible			= 1 << 2,		// allows to create 2D Array view from 3D image
		BlockTexelViewCompatible	= 1 << 3,		// allows to create view with uncompressed format for compressed image

		SparseResidency				= 1 << 4,		// allows unbound regions
		SparseAliased				= 1 << 5,		// allows bind memory block to multiple regions or different images

		Alias						= 1 << 6,		// two images can share same memory object
		SampleLocationsCompatible	= 1 << 7,		// only for depth/stencil
		Subsampled					= 1 << 8,		// intermediate attachments to use with fragment density map
		
		StorageAtomic				= 1 << 9,		// atomic operations on image
		ColorAttachmentBlend		= 1 << 10,		// blend operations on color attachment
		SampledLinear				= 1 << 11,		// linear filtering for sampled image
		SampledMinMax				= 1 << 12,		// minmax filtering for sampled image
		VertexPplnStore				= 1 << 13,		// storage image store and atomic operations in vertex, geometry, tessellation shaders
		FragmentPplnStore			= 1 << 14,		// storage image store and atomic operations in fragment shader

		LossyRTCompression			= 1 << 15,		// Metal only, allow to use hardware lossy compression for the color attachments

		_Last,
		SparseResidencyAliased		= SparseResidency | SparseAliased,
		All							= ((_Last-1) << 1) - 1,
		
		Unknown						= 0,
	};
	AE_BIT_OPERATORS( EImageOpt );


	enum class EImageAspect : ubyte
	{
		Color			= 1 << 0,
		Depth			= 1 << 1,
		Stencil			= 1 << 2,
		Metadata		= 1 << 3,
		_Last,

		DepthStencil	= Depth | Stencil,
		Unknown			= 0,
	};
	AE_BIT_OPERATORS( EImageAspect );
	
	
	enum class EAttachmentLoadOp : ubyte
	{
		Invalidate,
		Load,
		Clear,
		None,
		_Count,
		Unknown		= 0xFF,
	};


	enum class EAttachmentStoreOp : ubyte
	{
		Invalidate,
		Store,
		None,
		StoreCustomSamplePositions,	// MTLStoreActionOptionCustomSamplePositions
		_Count,
		Unknown		= 0xFF,
	};


	enum class EColorSpace : ubyte
	{
		sRGB_nonlinear,
		Display_P3_nonlinear,
		Extended_sRGB_linear,
		Display_P3_linear,
		DCI_P3_nonlinear,
		BT709_linear,
		BT709_nonlinear,
		BT2020_linear,
		HDR10_ST2084,
		DolbyVision,
		HDR10_HLG,
		AdobeRGB_linear,
		AdobeRGB_nonlinear,
		PassThrough,
		Extended_sRGB_nonlinear,

		_Count,
		Unknown		= 0xFF,
	};


	enum class EPresentMode : ubyte
	{
		Immediate,				// no vsync
		Mailbox,				// vsync, present last submitted
		FIFO,					// vsync, present all in submission order
		FIFO_Relaxed,
		SharedDemandRefresh,
		SharedContinuousRefresh,
		
		_Count,
		Unknown		= 0xFF,
	};
	

	enum class EPixelFormat : ubyte
	{
	#define AE_PIXELFORMAT_LIST( _visitor_ ) \
		\
		/* signed normalized */\
		_visitor_( RGBA16_SNorm		)\
		_visitor_( RGBA8_SNorm		)\
		_visitor_( RGB16_SNorm		)\
		_visitor_( RGB8_SNorm		)\
		_visitor_( RG16_SNorm		)\
		_visitor_( RG8_SNorm		)\
		_visitor_( R16_SNorm		)\
		_visitor_( R8_SNorm			)\
		\
		/* unsigned normalized */\
		_visitor_( RGBA16_UNorm		)\
		_visitor_( RGBA8_UNorm		)\
		_visitor_( RGB16_UNorm		)\
		_visitor_( RGB8_UNorm		)\
		_visitor_( RG16_UNorm		)\
		_visitor_( RG8_UNorm		)\
		_visitor_( R16_UNorm		)\
		_visitor_( R8_UNorm			)\
		_visitor_( RGB10_A2_UNorm	)\
		_visitor_( RGBA4_UNorm		)\
		_visitor_( RGB5_A1_UNorm	)\
		_visitor_( RGB_5_6_5_UNorm	)\
		\
		/* BGRA */\
		_visitor_( BGR8_UNorm		)\
		_visitor_( BGRA8_UNorm		)\
		\
		/* sRGB */\
		_visitor_( sRGB8			)\
		_visitor_( sRGB8_A8			)\
		_visitor_( sBGR8			)\
		_visitor_( sBGR8_A8			)\
		\
		/* signed integer */\
		_visitor_( R8I				)\
		_visitor_( RG8I				)\
		_visitor_( RGB8I			)\
		_visitor_( RGBA8I			)\
		_visitor_( R16I				)\
		_visitor_( RG16I			)\
		_visitor_( RGB16I			)\
		_visitor_( RGBA16I			)\
		_visitor_( R32I				)\
		_visitor_( RG32I			)\
		_visitor_( RGB32I			)\
		_visitor_( RGBA32I			)\
		_visitor_( R64I				)\
		\
		/* unsigned integer */\
		_visitor_( R8U				)\
		_visitor_( RG8U				)\
		_visitor_( RGB8U			)\
		_visitor_( RGBA8U			)\
		_visitor_( R16U				)\
		_visitor_( RG16U			)\
		_visitor_( RGB16U			)\
		_visitor_( RGBA16U			)\
		_visitor_( R32U				)\
		_visitor_( RG32U			)\
		_visitor_( RGB32U			)\
		_visitor_( RGBA32U			)\
		_visitor_( RGB10_A2U		)\
		_visitor_( R64U				)\
		\
		/* float */\
		_visitor_( R16F				)\
		_visitor_( RG16F			)\
		_visitor_( RGB16F			)\
		_visitor_( RGBA16F			)\
		_visitor_( R32F				)\
		_visitor_( RG32F			)\
		_visitor_( RGB32F			)\
		_visitor_( RGBA32F			)\
		_visitor_( RGB_11_11_10F	)\
		_visitor_( RGB_9F_E5		)\
		\
		/* depth stencil */\
		_visitor_( Depth16				)\
		_visitor_( Depth24				)\
		_visitor_( Depth32F				)\
		_visitor_( Depth16_Stencil8		)\
		_visitor_( Depth24_Stencil8		)\
		_visitor_( Depth32F_Stencil8	)\
		\
		/* compressed */\
		_visitor_( BC1_RGB8_UNorm		)\
		_visitor_( BC1_sRGB8			)\
		_visitor_( BC1_RGB8_A1_UNorm	)\
		_visitor_( BC1_sRGB8_A1			)\
		_visitor_( BC2_RGBA8_UNorm		)\
		_visitor_( BC2_sRGB8			)\
		_visitor_( BC3_RGBA8_UNorm		)\
		_visitor_( BC3_sRGB8			)\
		_visitor_( BC4_R8_SNorm			)\
		_visitor_( BC4_R8_UNorm			)\
		_visitor_( BC5_RG8_SNorm		)\
		_visitor_( BC5_RG8_UNorm		)\
		_visitor_( BC6H_RGB16F			)\
		_visitor_( BC6H_RGB16UF			)\
		_visitor_( BC7_RGBA8_UNorm		)\
		_visitor_( BC7_sRGB8_A8			)\
		_visitor_( ETC2_RGB8_UNorm		)\
		_visitor_( ECT2_sRGB8			)\
		_visitor_( ETC2_RGB8_A1_UNorm	)\
		_visitor_( ETC2_sRGB8_A1		)\
		_visitor_( ETC2_RGBA8_UNorm		)\
		_visitor_( ETC2_sRGB8_A8		)\
		_visitor_( EAC_R11_SNorm		)\
		_visitor_( EAC_R11_UNorm		)\
		_visitor_( EAC_RG11_SNorm		)\
		_visitor_( EAC_RG11_UNorm		)\
		\
		/* requires ASTC LDR feature */\
		_visitor_( ASTC_RGBA_4x4		)\
		_visitor_( ASTC_RGBA_5x4		)\
		_visitor_( ASTC_RGBA_5x5		)\
		_visitor_( ASTC_RGBA_6x5		)\
		_visitor_( ASTC_RGBA_6x6		)\
		_visitor_( ASTC_RGBA_8x5		)\
		_visitor_( ASTC_RGBA_8x6		)\
		_visitor_( ASTC_RGBA_8x8		)\
		_visitor_( ASTC_RGBA_10x5		)\
		_visitor_( ASTC_RGBA_10x6		)\
		_visitor_( ASTC_RGBA_10x8		)\
		_visitor_( ASTC_RGBA_10x10		)\
		_visitor_( ASTC_RGBA_12x10		)\
		_visitor_( ASTC_RGBA_12x12		)\
		_visitor_( ASTC_sRGB8_A8_4x4	)\
		_visitor_( ASTC_sRGB8_A8_5x4	)\
		_visitor_( ASTC_sRGB8_A8_5x5	)\
		_visitor_( ASTC_sRGB8_A8_6x5	)\
		_visitor_( ASTC_sRGB8_A8_6x6	)\
		_visitor_( ASTC_sRGB8_A8_8x5	)\
		_visitor_( ASTC_sRGB8_A8_8x6	)\
		_visitor_( ASTC_sRGB8_A8_8x8	)\
		_visitor_( ASTC_sRGB8_A8_10x5	)\
		_visitor_( ASTC_sRGB8_A8_10x6	)\
		_visitor_( ASTC_sRGB8_A8_10x8	)\
		_visitor_( ASTC_sRGB8_A8_10x10	)\
		_visitor_( ASTC_sRGB8_A8_12x10	)\
		_visitor_( ASTC_sRGB8_A8_12x12	)\
		\
		/* requires ASTC HDR extension */\
		_visitor_( ASTC_RGBA16F_4x4		)\
		_visitor_( ASTC_RGBA16F_5x4		)\
		_visitor_( ASTC_RGBA16F_5x5		)\
		_visitor_( ASTC_RGBA16F_6x5		)\
		_visitor_( ASTC_RGBA16F_6x6		)\
		_visitor_( ASTC_RGBA16F_8x5		)\
		_visitor_( ASTC_RGBA16F_8x6		)\
		_visitor_( ASTC_RGBA16F_8x8		)\
		_visitor_( ASTC_RGBA16F_10x5	)\
		_visitor_( ASTC_RGBA16F_10x6	)\
		_visitor_( ASTC_RGBA16F_10x8	)\
		_visitor_( ASTC_RGBA16F_10x10	)\
		_visitor_( ASTC_RGBA16F_12x10	)\
		_visitor_( ASTC_RGBA16F_12x12	)

		#define AE_PIXELFORMAT_VISIT( _name_ )	_name_,
		AE_PIXELFORMAT_LIST( AE_PIXELFORMAT_VISIT )
		#undef AE_PIXELFORMAT_VISIT

		_Count,
		
		// special value which will be replaced by current swapchain color format.
		SwapchainColor = 0xFE,

		Unknown			= 0xFF,
	};
	

	enum class EShaderIO : ubyte
	{
		Unknown		= 0,
		Int,
		UInt,
		Float,
		UFloat,
		Half,
		UNorm,
		SNorm,
		sRGB,
		AnyColor,		// compatible with sRGB, UNorm, Float
		Depth,
		Stencil,
		DepthStencil,
		_Count
	};


	enum class EBlitFilter : uint
	{
		Nearest,
		Linear,
		//Cubic,
	};


} // AE::Graphics
