// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Public/ResourceEnums.h"
# include "graphics/Public/RenderStateEnums.h"
# include "graphics/Public/ShaderEnums.h"
# include "graphics/Public/EResourceState.h"
# include "graphics/Public/MultiSamples.h"
# include "graphics/Public/VertexEnums.h"
# include "graphics/Public/PipelineDesc.h"
# include "graphics/Public/ImageSwizzle.h"
# include "graphics/Public/SamplerDesc.h"

# include "graphics/Private/PixelFormatDefines.h"
# include "graphics/Metal/MCommon.h"
# include "graphics/Metal/MInternal.h"

namespace AE::Graphics
{
	
/*
=================================================
	MEnumCast (EMemoryType)
=================================================
*/
	ND_ inline MTLResourceOptions  MEnumCast (EMemoryType value)
	{
		const MTLResourceOptions	tracking_mode = MTLResourceHazardTrackingModeDefault;

		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EMemoryType::Transient :
			{
				if ( @available( macos 11.0, ios 10.0, *))
					return MTLResourceCPUCacheModeDefaultCache	| MTLResourceStorageModeMemoryless	| tracking_mode;
				// otherwise use 'DeviceLocal'
			}
			case EMemoryType::DeviceLocal :			return MTLResourceCPUCacheModeDefaultCache	| MTLResourceStorageModePrivate	| tracking_mode;
			case EMemoryType::HostCocherent :		return MTLResourceCPUCacheModeWriteCombined	| MTLResourceStorageModeManaged	| tracking_mode;
			case EMemoryType::HostCached :			return MTLResourceCPUCacheModeDefaultCache	| MTLResourceStorageModeManaged	| tracking_mode;
			case EMemoryType::HostCachedCocherent :	return MTLResourceCPUCacheModeDefaultCache	| MTLResourceStorageModeManaged	| tracking_mode;
			case EMemoryType::Unified :				return MTLResourceCPUCacheModeWriteCombined	| MTLResourceStorageModePrivate	| tracking_mode;
			case EMemoryType::UnifiedCached :		return MTLResourceCPUCacheModeDefaultCache	| MTLResourceStorageModeManaged	| tracking_mode;

			case EMemoryType::Dedicated :
			case EMemoryType::_External :
			case EMemoryType::_Last :
			case EMemoryType::All :
			case EMemoryType::Unknown :		break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unsupported memory type", 0 );
	}

/*
=================================================
	MEnumCast (EAddressMode)
=================================================
*/
	ND_ inline MTLSamplerAddressMode  MEnumCast (EAddressMode value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EAddressMode::Repeat :				return MTLSamplerAddressModeRepeat;
			case EAddressMode::MirrorRepeat :		return MTLSamplerAddressModeMirrorRepeat;
			case EAddressMode::ClampToEdge :		return MTLSamplerAddressModeClampToEdge;
			case EAddressMode::ClampToBorder :
			{
				if ( @available( macos 10.12, ios 14.0, *))
					return MTLSamplerAddressModeClampToBorderColor;
				return MTLSamplerAddressModeClampToZero;
			}
			case EAddressMode::MirrorClampToEdge :
			{
				if ( @available( macos 10.11, ios 14.0, *))
					return MTLSamplerAddressModeMirrorClampToEdge;
				break;
			}
			case EAddressMode::_Count :
			case EAddressMode::Unknown :		break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown address mode", MTLSamplerAddressModeClampToZero );
	}

/*
=================================================
	MEnumCast (EBorderColor)
=================================================
*/
	ND_ inline MTLSamplerBorderColor  MEnumCast (EBorderColor value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EBorderColor::FloatTransparentBlack :
			case EBorderColor::IntTransparentBlack :	return MTLSamplerBorderColorTransparentBlack;

			case EBorderColor::FloatOpaqueBlack :
			case EBorderColor::IntOpaqueBlack :			return MTLSamplerBorderColorOpaqueBlack;

			case EBorderColor::FloatOpaqueWhite :
			case EBorderColor::IntOpaqueWhite :			return MTLSamplerBorderColorOpaqueWhite;
		
			case EBorderColor::_Count :
			case EBorderColor::Unknown :				break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown border color", MTLSamplerBorderColorTransparentBlack );
	}

/*
=================================================
	MEnumCast (EFilter)
=================================================
*/
	ND_ inline MTLSamplerMinMagFilter  MEnumCast (EFilter value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EFilter::Nearest :		return MTLSamplerMinMagFilterNearest;
			case EFilter::Linear :		return MTLSamplerMinMagFilterLinear;

			case EFilter::_Count :
			case EFilter::Unknown :		break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown filter mode", MTLSamplerMinMagFilterNearest );
	}

/*
=================================================
	MEnumCast (EMipmapFilter)
=================================================
*/
	ND_ inline MTLSamplerMipFilter  MEnumCast (EMipmapFilter value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EMipmapFilter::None :		return MTLSamplerMipFilterNotMipmapped;
			case EMipmapFilter::Nearest :	return MTLSamplerMipFilterNearest;
			case EMipmapFilter::Linear :	return MTLSamplerMipFilterLinear;

			case EMipmapFilter::_Count :
			case EMipmapFilter::Unknown :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown mipmap filter mode", MTLSamplerMipFilterNotMipmapped );
	}

/*
=================================================
	MEnumCast (ECompareOp)
=================================================
*/
	ND_ inline MTLCompareFunction  MEnumCast (ECompareOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case ECompareOp::Never :	return MTLCompareFunctionNever;
			case ECompareOp::Less :		return MTLCompareFunctionLess;
			case ECompareOp::Equal :	return MTLCompareFunctionEqual;
			case ECompareOp::LEqual :	return MTLCompareFunctionLessEqual;
			case ECompareOp::Greater :	return MTLCompareFunctionGreater;
			case ECompareOp::NotEqual :	return MTLCompareFunctionNotEqual;
			case ECompareOp::GEqual :	return MTLCompareFunctionGreaterEqual;
			case ECompareOp::Always :	return MTLCompareFunctionAlways;

			case ECompareOp::_Count :
			case ECompareOp::Unknown :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown compare op", MTLCompareFunctionNever );
	}

/*
=================================================
	MEnumCast (EStencilOp)
=================================================
*/
	ND_ inline MTLStencilOperation  MEnumCast (EStencilOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EStencilOp::Keep :		return MTLStencilOperationKeep;
			case EStencilOp::Zero :		return MTLStencilOperationZero;
			case EStencilOp::Replace :	return MTLStencilOperationReplace;
			case EStencilOp::Incr :		return MTLStencilOperationIncrementClamp;
			case EStencilOp::IncrWrap :	return MTLStencilOperationIncrementWrap;
			case EStencilOp::Decr :		return MTLStencilOperationDecrementClamp;
			case EStencilOp::DecrWrap :	return MTLStencilOperationDecrementWrap;
			case EStencilOp::Invert :	return MTLStencilOperationInvert;
			case EStencilOp::_Count :
			case EStencilOp::Unknown :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown stencil op", MTLStencilOperationKeep );
	}

/*
=================================================
	MEnumCast (EImage)
=================================================
*/
	ND_ inline MTLTextureType  MEnumCast (EImage value, MultiSamples samples)
	{
		const bool	ms = samples > 1_samples;

		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EImage::_1D :			ASSERT( not ms );  return MTLTextureType1D;
			case EImage::_1DArray :		ASSERT( not ms );  return MTLTextureType1DArray;
			case EImage::_2D :			return ms ? MTLTextureType2DMultisample : MTLTextureType2D;
			case EImage::_2DArray :		return ms ? MTLTextureType2DMultisampleArray : MTLTextureType2DArray;
			case EImage::_3D :			ASSERT( not ms );  return MTLTextureType3D;
			case EImage::Cube :			ASSERT( not ms );  return MTLTextureTypeCube;
			case EImage::CubeArray :	ASSERT( not ms );  return MTLTextureTypeCubeArray;
			case EImage::Unknown :		break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown image type", MTLTextureType2D );
	}
	
/*
=================================================
	MEnumCast (ImageSwizzle)
=================================================
*/
	ND_ inline MTLTextureSwizzleChannels  MEnumCast (ImageSwizzle value)
	{
		const MTLTextureSwizzle	components[] = {
			MTLTextureSwizzleZero,	// unknown
			MTLTextureSwizzleRed,
			MTLTextureSwizzleGreen,
			MTLTextureSwizzleBlue,
			MTLTextureSwizzleAlpha,
			MTLTextureSwizzleZero,
			MTLTextureSwizzleOne,
		};
		
		const uint4		swizzle	= Min( uint4(uint(CountOf(components)-1)), value.ToVec() );

		return MTLTextureSwizzleChannelsMake( components[swizzle.x],
											  components[swizzle.y],
											  components[swizzle.z],
											  components[swizzle.w] );
	}

/*
=================================================
	MEnumCast (EImageUsage, EImageOpt)
=================================================
*/
	ND_ inline MTLTextureUsage  MEnumCast (EImageUsage usage, EImageOpt options)
	{
		MTLTextureUsage		result = 0;
		
		while ( usage != Zero )
		{
			EImageUsage	t = ExtractBit( INOUT usage );
			
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case EImageUsage::TransferSrc :				break;
				case EImageUsage::TransferDst :				break;
				case EImageUsage::Sampled :					result |= MTLTextureUsageShaderRead;	break;
				case EImageUsage::Storage :					result |= MTLTextureUsageShaderWrite;	break;
				case EImageUsage::ColorAttachment :			result |= MTLTextureUsageRenderTarget;	break;
				case EImageUsage::DepthStencilAttachment :	result |= MTLTextureUsageRenderTarget;	break;
				case EImageUsage::InputAttachment :			result |= MTLTextureUsageRenderTarget;	break;
				case EImageUsage::ShadingRate :
				case EImageUsage::FragmentDensityMap :
				case EImageUsage::_Last :
				case EImageUsage::Unknown :
				case EImageUsage::Transfer :
				case EImageUsage::RWAttachment :
				case EImageUsage::All :						// to shutup warnings
				default :									RETURN_ERR( "unsupported image usage", MTLTextureUsageUnknown );
			}
			END_ENUM_CHECKS();
		}

		while ( options != Zero )
		{
			EImageOpt	t = ExtractBit( INOUT options );
		
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case EImageOpt::MutableFormat :				result |= MTLTextureUsagePixelFormatView;	break;
					
				case EImageOpt::Array2DCompatible :
				case EImageOpt::BlockTexelViewCompatible:
				case EImageOpt::CubeCompatible :
				case EImageOpt::Alias :						break;	// ignore

				case EImageOpt::SparseResidency :
				case EImageOpt::SparseAliased :
				case EImageOpt::LosslessRTCompression :		break;
					
				case EImageOpt::StorageAtomic :
				case EImageOpt::ColorAttachmentBlend :
				case EImageOpt::SampledLinear :
				case EImageOpt::SampledMinMax :
				case EImageOpt::VertexPplnStore :
				case EImageOpt::FragmentPplnStore :			break;	// options used only to check supported usage

				case EImageOpt::SampleLocationsCompatible :
				case EImageOpt::Subsampled :				RETURN_ERR( "unsupported image flag", Zero );

				case EImageOpt::_Last :
				case EImageOpt::All :
				case EImageOpt::Unknown :
				case EImageOpt::SparseResidencyAliased :
				default :									RETURN_ERR( "unknown image flag", Zero );
			}
			END_ENUM_CHECKS();
		}

		ASSERT( result != 0 );
		return result;
	}

/*
=================================================
	MEnumCast (EVertexType)
=================================================
*/
	ND_ inline MTLVertexFormat  MEnumCast (EVertexType value)
	{
		#define FMT_BUILDER( _engineFmt_, _mtlFormat_ ) \
			case EVertexType::_engineFmt_ : return MTLVertexFormat ## _mtlFormat_;

		switch ( value )
		{
			AE_PRIVATE_MTLVERTEXFORMATS( FMT_BUILDER )
		}
		RETURN_ERR( "unknown vertex type!", MTLVertexFormatInvalid );
		#undef FMT_BUILDER
	}
	
/*
=================================================
	MEnumCast (EVertexInputRate)
=================================================
*/
	ND_ inline MTLVertexStepFunction  MEnumCast (EVertexInputRate value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EVertexInputRate::Vertex :		return MTLVertexStepFunctionPerVertex;
			case EVertexInputRate::Instance :	return MTLVertexStepFunctionPerInstance;
			case EVertexInputRate::_Count :
			case EVertexInputRate::Unknown :	break;
			// TODO: MTLVertexStepFunctionConstant, MTLVertexStepFunctionPerPatch, MTLVertexStepFunctionPerPatchControlPoint
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid vertex input rate", MTLVertexStepFunctionConstant );
	}
	
/*
=================================================
	MEnumCast (EIndex)
=================================================
*/
	ND_ inline MTLIndexType  MEnumCast (EIndex value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EIndex::UShort :	return MTLIndexTypeUInt16;
			case EIndex::UInt :		return MTLIndexTypeUInt32;
			case EIndex::_Count :
			case EIndex::Unknown :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid index type", MTLIndexTypeUInt32 );
	}

/*
=================================================
	MEnumCast (EBlendOp)
=================================================
*/
	ND_ inline MTLBlendOperation  MEnumCast (EBlendOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EBlendOp::Add :		return MTLBlendOperationAdd;
			case EBlendOp::Sub :		return MTLBlendOperationSubtract;
			case EBlendOp::RevSub :		return MTLBlendOperationReverseSubtract;
			case EBlendOp::Min :		return MTLBlendOperationMin;
			case EBlendOp::Max :		return MTLBlendOperationMax;
			case EBlendOp::_Count :
			case EBlendOp::Unknown :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid blend operation", MTLBlendOperationAdd );
	}
	
/*
=================================================
	MEnumCast (EBlendFactor)
=================================================
*/
	ND_ inline MTLBlendFactor  MEnumCast (EBlendFactor value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EBlendFactor::Zero :				return MTLBlendFactorZero;
			case EBlendFactor::One :				return MTLBlendFactorOne;
			case EBlendFactor::SrcColor :			return MTLBlendFactorSourceColor;
			case EBlendFactor::OneMinusSrcColor :	return MTLBlendFactorOneMinusSourceColor;
			case EBlendFactor::DstColor :			return MTLBlendFactorDestinationColor;
			case EBlendFactor::OneMinusDstColor :	return MTLBlendFactorOneMinusDestinationColor;
			case EBlendFactor::SrcAlpha :			return MTLBlendFactorSourceAlpha;
			case EBlendFactor::OneMinusSrcAlpha :	return MTLBlendFactorOneMinusSourceAlpha;
			case EBlendFactor::DstAlpha :			return MTLBlendFactorDestinationAlpha;
			case EBlendFactor::OneMinusDstAlpha :	return MTLBlendFactorOneMinusDestinationAlpha;
			case EBlendFactor::ConstColor :			return MTLBlendFactorBlendColor;
			case EBlendFactor::OneMinusConstColor :	return MTLBlendFactorOneMinusBlendColor;
			case EBlendFactor::ConstAlpha :			return MTLBlendFactorBlendAlpha;
			case EBlendFactor::OneMinusConstAlpha :	return MTLBlendFactorOneMinusBlendAlpha;
			case EBlendFactor::SrcAlphaSaturate :	return MTLBlendFactorSourceAlphaSaturated;

			case EBlendFactor::Src1Color :			return MTLBlendFactorSource1Color;
			case EBlendFactor::OneMinusSrc1Color :	return MTLBlendFactorOneMinusSource1Color;
			case EBlendFactor::Src1Alpha :			return MTLBlendFactorSource1Alpha;
			case EBlendFactor::OneMinusSrc1Alpha :	return MTLBlendFactorOneMinusSource1Alpha;

			case EBlendFactor::_Count :
			case EBlendFactor::Unknown :			break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid blend factor", MTLBlendFactorZero );
	}
	
/*
=================================================
	MEnumCast (EPrimitive)
=================================================
*/
	ND_ inline MTLPrimitiveType  MEnumCast (EPrimitive value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EPrimitive::Point :					return MTLPrimitiveTypePoint;
			case EPrimitive::LineList :					return MTLPrimitiveTypeLine;
			case EPrimitive::LineStrip :				return MTLPrimitiveTypeLineStrip;
			case EPrimitive::TriangleList :				return MTLPrimitiveTypeTriangle;
			case EPrimitive::TriangleStrip :			return MTLPrimitiveTypeTriangleStrip;

			case EPrimitive::TriangleFan :
			case EPrimitive::TriangleListAdjacency :
			case EPrimitive::TriangleStripAdjacency :
			case EPrimitive::Patch :
			case EPrimitive::LineListAdjacency :
			case EPrimitive::LineStripAdjacency :
			case EPrimitive::_Count :
			case EPrimitive::Unknown :					break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid primitive type", MTLPrimitiveTypeTriangle );
	}
	
/*
=================================================
	MEnumCast_TopologyClass (EPrimitive)
=================================================
*/
	ND_ inline MTLPrimitiveTopologyClass  MEnumCast_TopologyClass (EPrimitive value) API_AVAILABLE(macos(10.11), ios(12))
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EPrimitive::Point :					return MTLPrimitiveTopologyClassPoint;
			case EPrimitive::LineList :
			case EPrimitive::LineStrip :
			case EPrimitive::LineListAdjacency :
			case EPrimitive::LineStripAdjacency :		return MTLPrimitiveTopologyClassLine;
			case EPrimitive::TriangleList :
			case EPrimitive::TriangleStrip :
			case EPrimitive::TriangleFan :
			case EPrimitive::TriangleListAdjacency :
			case EPrimitive::TriangleStripAdjacency :
			case EPrimitive::Patch :					return MTLPrimitiveTopologyClassTriangle;
			
			case EPrimitive::_Count :
			case EPrimitive::Unknown :					break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid primitive type", MTLPrimitiveTopologyClassUnspecified );
	}
	
/*
=================================================
	MEnumCast (EPolygonMode)
=================================================
*/
	ND_ inline MTLTriangleFillMode  MEnumCast (EPolygonMode value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case_likely EPolygonMode::Fill :	return MTLTriangleFillModeFill;
			case EPolygonMode::Line :			return MTLTriangleFillModeLines;

			case EPolygonMode::Point :
			case EPolygonMode::_Count :
			case EPolygonMode::Unknown :		break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid polygon mode", MTLTriangleFillModeFill );
	}
	
/*
=================================================
	MEnumCast (ECullMode)
=================================================
*/
	ND_ inline MTLCullMode  MEnumCast (ECullMode value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case ECullMode::None :			return MTLCullModeNone;
			case ECullMode::Front :			return MTLCullModeFront;
			case ECullMode::Back :			return MTLCullModeBack;
			case ECullMode::FontAndBack	:	return MTLCullModeFront | MTLCullModeBack;
			case ECullMode::_Last :			break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid cull mode", MTLCullModeNone );
	}

/*
=================================================
	MEnumCast (EPixelFormat)
=================================================
*/
	ND_ inline MTLPixelFormat  MEnumCast (EPixelFormat value)
	{
		#define FMT_BUILDER( _engineFmt_, _mtlFormat_, _apiVer_ ) \
			case EPixelFormat::_engineFmt_ : if (_apiVer_) return MTLPixelFormat ## _mtlFormat_; else break;
		
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			AE_PRIVATE_MTLPIXELFORMATS( FMT_BUILDER )
			case EPixelFormat::RGB16_SNorm :
			case EPixelFormat::RGB8_SNorm :
			case EPixelFormat::RGB16_UNorm :
			case EPixelFormat::RGB8_UNorm :
			case EPixelFormat::BGR8_UNorm :
			case EPixelFormat::sRGB8 :
			case EPixelFormat::sBGR8 :
			case EPixelFormat::RGB8I :
			case EPixelFormat::RGB16I :
			case EPixelFormat::RGB32I :
			case EPixelFormat::R64I :
			case EPixelFormat::RGB8U :
			case EPixelFormat::RGB16U :
			case EPixelFormat::RGB32U :
			case EPixelFormat::R64U :
			case EPixelFormat::RGB16F :
			case EPixelFormat::RGB32F :
			case EPixelFormat::Depth24 :
			case EPixelFormat::Depth16_Stencil8 :
			case EPixelFormat::BC1_RGB8_UNorm :
			case EPixelFormat::BC1_sRGB8 :
			case EPixelFormat::_Count :
			case EPixelFormat::SwapchainColor :
			case EPixelFormat::Unknown :		break;
		}
		END_ENUM_CHECKS();

		RETURN_ERR( "invalid pixel format", MTLPixelFormatInvalid );
		#undef FMT_BUILDER
	}

/*
=================================================
	MEnumCast (ERTASOptions)
=================================================
*/
	ND_ inline MTLAccelerationStructureUsage  MEnumCast (ERTASOptions value)
	{
		MTLAccelerationStructureUsage 	result = MTLAccelerationStructureUsageNone;
		
		while ( values != Zero )
		{
			auto	t = ExtractBit( INOUT values );
			
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case ERTASOptions::AllowUpdate :		result |= MTLAccelerationStructureUsageRefit;			break;
				case ERTASOptions::AllowCompaction :	break;
				case ERTASOptions::PreferFastTrace :	break;
				case ERTASOptions::PreferFastBuild :	result |= MTLAccelerationStructureUsagePreferFastBuild;	break;
				case ERTASOptions::LowMemory :			break;
				// TODO: MTLAccelerationStructureUsageExtendedLimits 
				case ERTASOptions::_Last :
				case ERTASOptions::Unknown :
				default :								RETURN_ERR( "unknown RTAS options", MTLAccelerationStructureUsageNone );	break;
			}
			END_ENUM_CHECKS();
		}
		return result;
	}
	
/*
=================================================
	AEEnumCast (MTLPixelFormat)
=================================================
*/
	ND_ inline EPixelFormat  AEEnumCast (MTLPixelFormat value)
	{
		#define FMT_BUILDER( _engineFmt_, _mtlFormat_, _apiVer_ ) \
			case MTLPixelFormat ## _mtlFormat_ : return EPixelFormat::_engineFmt_;
		
		switch ( value )
		{
			AE_PRIVATE_MTLPIXELFORMATS( FMT_BUILDER )
		}

		RETURN_ERR( "invalid pixel format" );
		#undef FMT_BUILDER
	}
	
/*
=================================================
	AEEnumCast (MTLVertexFormat)
=================================================
*/
	ND_ inline EVertexType  AEEnumCast (MTLVertexFormat value)
	{
		#define FMT_BUILDER( _engineFmt_, _mtlFormat_ ) \
			case MTLVertexFormat ## _mtlFormat_ : return EVertexType::_engineFmt_;
		
		switch ( value )
		{
			AE_PRIVATE_MTLVERTEXFORMATS( FMT_BUILDER )
		}

		RETURN_ERR( "invalid vertex type" );
		#undef FMT_BUILDER
	}
	
/*
=================================================
	AEEnumCast (MTLResourceOptions)
=================================================
*/
	ND_ inline EMemoryType  AEEnumCast (MTLResourceOptions values, bool isExternal)
	{
		EMemoryType	result = isExternal ? EMemoryType::_External : Zero;
		
		if ( @available( macos 11.0, ios 10.0, *))
		{
			if ( AllBits( values, MTLResourceCPUCacheModeDefaultCache | MTLResourceStorageModeMemoryless ))
				return result | EMemoryType::Transient;
		}
		if ( AllBits( values, MTLResourceCPUCacheModeDefaultCache	| MTLResourceStorageModePrivate ))	return result | EMemoryType::DeviceLocal;
		if ( AllBits( values, MTLResourceCPUCacheModeWriteCombined	| MTLResourceStorageModeManaged ))	return result | EMemoryType::HostCocherent;
		if ( AllBits( values, MTLResourceCPUCacheModeDefaultCache	| MTLResourceStorageModeManaged ))	return result | EMemoryType::HostCached;	// HostCachedCocherent ?
		if ( AllBits( values, MTLResourceCPUCacheModeWriteCombined	| MTLResourceStorageModePrivate ))	return result | EMemoryType::Unified;
		
		RETURN_ERR( "unsupported resource options" );
	}
	
/*
=================================================
	AEEnumCast (MtlTextureType)
=================================================
*/
	ND_ inline EImageDim  AEEnumCast (MTLTextureType value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case MTLTextureType1D :
			case MTLTextureType1DArray :			return EImageDim_1D;
			case MTLTextureType2D :
			case MTLTextureType2DArray :
			case MTLTextureTypeCube :
			case MTLTextureTypeCubeArray :
			case MTLTextureType2DMultisample :
			case MTLTextureType2DMultisampleArray :	return EImageDim_2D;
			case MTLTextureType3D :					return EImageDim_3D;
			case MTLTextureTypeTextureBuffer :		break;
		}
		END_ENUM_CHECKS();

		RETURN_ERR( "unknown texture type" );
	}
	
/*
=================================================
	AEEnumCast (MTLTextureUsage)
=================================================
*/
	ND_ inline Tuple<EImageUsage, EImageOpt>  AEEnumCast (MTLTextureUsage values)
	{
		EImageUsage	usage	= Default;
		EImageOpt	options	= Default;

		while ( values != 0 )
		{
			auto	t = ExtractBit( INOUT values );

			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case MTLTextureUsageShaderRead :		usage |= EImageUsage::Sampled;		break;
				case MTLTextureUsageShaderWrite :		usage |= EImageUsage::Storage;		break;
				case MTLTextureUsageRenderTarget :		usage |= EImageUsage::ColorAttachment | EImageUsage::DepthStencilAttachment | EImageUsage::InputAttachment;	break;
				case MTLTextureUsagePixelFormatView :	options |= EImageOpt::MutableFormat;	break;
				case MTLTextureUsageUnknown :
				default :								RETURN_ERR( "unknown texture usage" );
			}
			END_ENUM_CHECKS();
		}

		return Tuple<EImageUsage, EImageOpt>{ usage, options };
	}

} // AE::Graphics

#endif // AE_ENABLE_METAL
