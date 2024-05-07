// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/EResourceState.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/ShaderEnums.h"
#include "graphics/Public/VertexEnums.h"
#include "graphics/Public/RenderStateEnums.h"
#include "graphics/Public/FeatureSetEnums.h"
#include "graphics/Public/Queue.h"
#include "graphics/Public/SamplerDesc.h"
#include "graphics/Public/DescriptorSet.h"
#include "graphics/Public/PipelineDesc.h"
#include "graphics/Public/RayTracingEnums.h"
#include "graphics/Public/VideoEnums.h"
#include "graphics/Public/ImageSwizzle.h"
#include "graphics/Private/PixelFormatDefines.h"

namespace AE::Base
{
	using Graphics::EQueueType;
	using Graphics::EPixelFormat;
	using Graphics::EResourceState;
	using Graphics::EShaderIO;
	using Graphics::EShaderStages;
	using Graphics::EShader;
	using Graphics::EPrimitive;
	using Graphics::EVertexType;
	using Graphics::EGPUVendor;
	using Graphics::EGraphicsDeviceID;
	using Graphics::ESubgroupOperation;
	using Graphics::EFilter;
	using Graphics::EMipmapFilter;
	using Graphics::EAddressMode;
	using Graphics::ECompareOp;
	using Graphics::EBorderColor;
	using Graphics::EReductionMode;
	using Graphics::ESamplerOpt;
	using Graphics::EVertexInputRate;
	using Graphics::ELogicOp;
	using Graphics::EBlendFactor;
	using Graphics::EBlendOp;
	using Graphics::EStencilOp;
	using Graphics::EPolygonMode;
	using Graphics::ECullMode;
	using Graphics::EPipelineDynamicState;
	using Graphics::EDescSetUsage;
	using Graphics::EPipelineOpt;
	using Graphics::EMemoryType;
	using Graphics::EDescriptorType;
	using Graphics::EColorSpace;
	using Graphics::EPresentMode;
	using Graphics::ESamplerChromaLocation;
	using Graphics::ESamplerYcbcrModelConversion;
	using Graphics::ESamplerYcbcrRange;
	using Graphics::ImageSwizzle;
	using Graphics::ESurfaceFormat;
	using Graphics::EVideoFormat;
	using Graphics::EVideoCodec;
	using Graphics::EPixelFormatExternal;

/*
=================================================
	ToString (EQueueType)
=================================================
*/
	ND_ inline StringView  ToString (EQueueType queue) __NE___
	{
		switch_enum( queue )
		{
			case EQueueType::Graphics :		return "Graphics";
			case EQueueType::AsyncCompute :	return "AsyncCompute";
			case EQueueType::AsyncTransfer:	return "AsyncTransfer";
			case EQueueType::VideoEncode :	return "VideoEncode";
			case EQueueType::VideoDecode :	return "VideoDecode";
			case EQueueType::_Count :
			case EQueueType::Unknown :		break;
		}
		switch_end
		RETURN_ERR( "unknown queue type" );
	}

/*
=================================================
	ToString (EPixelFormat)
=================================================
*/
	ND_ inline StringView  ToString (EPixelFormat value) __NE___
	{
		switch_enum( value )
		{
			#define AE_PIXELFORMAT_VISIT( _name_ )		case EPixelFormat::_name_ : return AE_TOSTRING( _name_ );
			AE_PIXELFORMAT_LIST( AE_PIXELFORMAT_VISIT )
			#undef AE_PIXELFORMAT_VISIT

			case EPixelFormat::SwapchainColor : return "SwapchainColor";
			case EPixelFormat::Unknown :		return "Unknown";
			case EPixelFormat::_Count :			break;
		}
		switch_end
		RETURN_ERR( "unknown pixel format" );
	}

/*
=================================================
	ToString (EShaderIO)
=================================================
*/
	ND_ inline StringView  ToString (EShaderIO value) __NE___
	{
		switch_enum( value )
		{
			case EShaderIO::Int :			return "Int";
			case EShaderIO::UInt :			return "UInt";
			case EShaderIO::Float :			return "Float";
			case EShaderIO::UFloat :		return "UFloat";
			case EShaderIO::Half :			return "Half";
			case EShaderIO::UNorm :			return "UNorm";
			case EShaderIO::SNorm :			return "SNorm";
			case EShaderIO::sRGB :			return "sRGB";
			case EShaderIO::AnyColor :		return "AnyColor";
			case EShaderIO::Depth :			return "Depth";
			case EShaderIO::Stencil :		return "Stencil";
			case EShaderIO::DepthStencil :	return "DepthStencil";
			case EShaderIO::Unknown :
			case EShaderIO::_Count :		break;
		}
		switch_end
		RETURN_ERR( "unknown fragment IO type" );
	}

/*
=================================================
	ToString (EShader)
=================================================
*/
	ND_ inline StringView  ToString (EShader value) __NE___
	{
		switch_enum( value )
		{
			case EShader::Vertex :			return "Vertex";
			case EShader::TessControl :		return "TessControl";
			case EShader::TessEvaluation :	return "TessEvaluation";
			case EShader::Geometry :		return "Geometry";
			case EShader::Fragment :		return "Fragment";
			case EShader::Compute :			return "Compute";
			case EShader::Tile :			return "Tile";
			case EShader::MeshTask :		return "MeshTask";
			case EShader::Mesh :			return "Mesh";
			case EShader::RayGen :			return "RayGen";
			case EShader::RayAnyHit :		return "RayAnyHit";
			case EShader::RayClosestHit :	return "RayClosestHit";
			case EShader::RayMiss :			return "RayMiss";
			case EShader::RayIntersection :	return "RayIntersection";
			case EShader::RayCallable :		return "RayCallable";
			case EShader::_Count :
			case EShader::Unknown :			break;
		}
		switch_end
		RETURN_ERR( "unknown shader type" );
	}

/*
=================================================
	ToString (EShaderStages)
=================================================
*/
	ND_ inline String  ToString (EShaderStages values) __Th___
	{
		String	str;

		if ( AllBits( values, EShaderStages::AllGraphics ))
		{
			if ( not str.empty() ) str << " | ";
			str << "AllGraphics";
			values &= ~EShaderStages::AllGraphics;
		}

		if ( AllBits( values, EShaderStages::AllRayTracing ))
		{
			if ( not str.empty() ) str << " | ";
			str << "AllRayTracing";
			values &= ~EShaderStages::AllRayTracing;
		}

		if ( AllBits( values, EShaderStages::MeshStages ))
		{
			if ( not str.empty() ) str << " | ";
			str << "MeshStages";
			values &= ~EShaderStages::MeshStages;
		}

		if ( AllBits( values, EShaderStages::GraphicsStages ))
		{
			if ( not str.empty() ) str << " | ";
			str << "GraphicsStages";
			values &= ~EShaderStages::GraphicsStages;
		}

		for (auto bit : BitIndexIterate<EShader>( values ))
		{
			if ( not str.empty() ) str << " | ";

			str << ToString( bit );
		}
		return str;
	}

/*
=================================================
	ToString (EPrimitive)
=================================================
*/
	ND_ inline StringView  ToString (EPrimitive value) __NE___
	{
		switch_enum( value )
		{
			case EPrimitive::Point :					return "Point";
			case EPrimitive::LineList :					return "LineList";
			case EPrimitive::LineStrip :				return "LineStrip";
			case EPrimitive::LineListAdjacency :		return "LineListAdjacency";
			case EPrimitive::LineStripAdjacency :		return "LineStripAdjacency";
			case EPrimitive::TriangleList :				return "TriangleList";
			case EPrimitive::TriangleStrip :			return "TriangleStrip";
			case EPrimitive::TriangleFan :				return "TriangleFan";
			case EPrimitive::TriangleListAdjacency :	return "TriangleListAdjacency";
			case EPrimitive::TriangleStripAdjacency :	return "TriangleStripAdjacency";
			case EPrimitive::Patch :					return "Patch";
			case EPrimitive::_Count :
			case EPrimitive::Unknown :	break;
		}
		switch_end
		RETURN_ERR( "unknown primitive type" );
	}

/*
=================================================
	ToString (EGPUVendor)
=================================================
*/
	ND_ inline StringView  ToString (EGPUVendor value) __NE___
	{
		switch_enum( value )
		{
			case EGPUVendor::AMD :			return "AMD";
			case EGPUVendor::NVidia :		return "NVidia";
			case EGPUVendor::Intel :		return "Intel";
			case EGPUVendor::ARM :			return "ARM";
			case EGPUVendor::Qualcomm :		return "Qualcomm";
			case EGPUVendor::ImgTech :		return "ImgTech";
			case EGPUVendor::Microsoft :	return "Microsoft";
			case EGPUVendor::Apple :		return "Apple";
			case EGPUVendor::Mesa :			return "Mesa";
			case EGPUVendor::Broadcom :		return "Broadcom";
			case EGPUVendor::Samsung :		return "Samsung";
			case EGPUVendor::VeriSilicon:	return "VeriSilicon";
			case EGPUVendor::Huawei :		return "Huawei";
			case EGPUVendor::_Count :		break;
		}
		switch_end
		RETURN_ERR( "unknown GPU vendor" );
	}

/*
=================================================
	ToString (EFilter)
=================================================
*/
	ND_ inline StringView  ToString (EFilter value) __NE___
	{
		switch_enum( value )
		{
			case EFilter::Nearest :		return "Nearest";
			case EFilter::Linear :		return "Linear";
			case EFilter::_Count :
			case EFilter::Unknown :
			default :					break;
		}
		switch_end
		RETURN_ERR( "unknown filter type" );
	}

/*
=================================================
	ToString (EMipmapFilter)
=================================================
*/
	ND_ inline StringView  ToString (EMipmapFilter value) __NE___
	{
		switch_enum( value )
		{
			case EMipmapFilter::None :		return "None";
			case EMipmapFilter::Nearest :	return "Nearest";
			case EMipmapFilter::Linear :	return "Linear";
			case EMipmapFilter::_Count :
			case EMipmapFilter::Unknown :
			default :						break;
		}
		switch_end
		RETURN_ERR( "unknown filter type" );
	}

/*
=================================================
	ToString (EAddressMode)
=================================================
*/
	ND_ inline StringView  ToString (EAddressMode value) __NE___
	{
		switch_enum( value )
		{
			case EAddressMode::Repeat :				return "Repeat";
			case EAddressMode::MirrorRepeat :		return "MirrorRepeat";
			case EAddressMode::ClampToEdge :		return "ClampToEdge";
			case EAddressMode::ClampToBorder :		return "ClampToBorder";
			case EAddressMode::MirrorClampToEdge :	return "MirrorClampToEdge";
			case EAddressMode::_Count :
			case EAddressMode::Unknown :
			default :								break;
		}
		switch_end
		RETURN_ERR( "unknown address mode" );
	}

/*
=================================================
	ToString (ECompareOp)
=================================================
*/
	ND_ inline StringView  ToString (ECompareOp value) __NE___
	{
		switch_enum( value )
		{
			case ECompareOp::Never :	return "Never";
			case ECompareOp::Less :		return "Less";
			case ECompareOp::Equal :	return "Equal";
			case ECompareOp::LEqual :	return "LEqual";
			case ECompareOp::Greater :	return "Greater";
			case ECompareOp::NotEqual :	return "NotEqual";
			case ECompareOp::GEqual :	return "GEqual";
			case ECompareOp::Always :	return "Always";
			case ECompareOp::_Count :
			case ECompareOp::Unknown :
			default :					break;
		}
		switch_end
		RETURN_ERR( "unknown compare op" );
	}

/*
=================================================
	ToString (EBorderColor)
=================================================
*/
	ND_ inline StringView  ToString (EBorderColor value) __NE___
	{
		switch_enum( value )
		{
			case EBorderColor::FloatTransparentBlack :	return "FloatTransparentBlack";
			case EBorderColor::FloatOpaqueBlack :		return "FloatOpaqueBlack";
			case EBorderColor::FloatOpaqueWhite :		return "FloatOpaqueWhite";
			case EBorderColor::IntTransparentBlack :	return "IntTransparentBlack";
			case EBorderColor::IntOpaqueBlack :			return "IntOpaqueBlack";
			case EBorderColor::IntOpaqueWhite :			return "IntOpaqueWhite";
			case EBorderColor::_Count :
			case EBorderColor::Unknown :
			default :									break;
		}
		switch_end
		RETURN_ERR( "unknown border color type" );
	}

/*
=================================================
	ToString (EReductionMode)
=================================================
*/
	ND_ inline StringView  ToString (EReductionMode value) __NE___
	{
		switch_enum( value )
		{
			case EReductionMode::Average :		return "Average";
			case EReductionMode::Min :			return "Min";
			case EReductionMode::Max :			return "Max";
			case EReductionMode::_Count :
			case EReductionMode::Unknown :
			default :							break;
		}
		switch_end
		RETURN_ERR( "unknown sampler reduction mode" );
	}

/*
=================================================
	ToString (ESamplerOpt)
=================================================
*/
	ND_ inline String  ToString (ESamplerOpt values) __NE___
	{
		String	str;
		for (auto t : BitfieldIterate( values ))
		{
			if ( not str.empty() )
				str << " | ";

			switch_enum( t )
			{
				case ESamplerOpt::ArgumentBuffer :					return "ArgumentBuffer";
				case ESamplerOpt::NonSeamlessCubeMap :				return "NonSeamlessCubeMap";
				case ESamplerOpt::UnnormalizedCoordinates :			return "UnnormalizedCoordinates";
			//	case ESamplerOpt::Subsampled :						return "Subsampled";
			//	case ESamplerOpt::SubsampledCoarseReconstruction:	return "SubsampledCoarseReconstruction";

				case ESamplerOpt::Unknown :
				case ESamplerOpt::_Last :
				case ESamplerOpt::All :
				default :											RETURN_ERR( "unknown sampler usage" );
			}
			switch_end
		}
		return str;
	}

/*
=================================================
	ToString (EVertexType)
=================================================
*/
	ND_ inline StringView  ToString (EVertexType value) __NE___
	{
		switch ( value )
		{
			#define AE_VERTEXTYPE_VISIT( _name_, _value_ )		case EVertexType::_name_ : return AE_TOSTRING( _name_ );
			AE_VERTEXTYPE_LIST( AE_VERTEXTYPE_VISIT )
			#undef AE_VERTEXTYPE_VISIT
		}
		RETURN_ERR( "unknown vertex type" );
	}

/*
=================================================
	ToString (EGraphicsDeviceID)
=================================================
*/
	ND_ inline StringView  ToString (EGraphicsDeviceID value) __NE___
	{
		switch ( value )
		{
			#define AE_GRAPHICS_DEVICE_VISIT( _name_ )		case EGraphicsDeviceID::_name_ : return AE_TOSTRING( _name_ );
			AE_GRAPHICS_DEVICE_LIST( AE_GRAPHICS_DEVICE_VISIT )
			#undef AE_GRAPHICS_DEVICE_VISIT
		}
		RETURN_ERR( "unknown graphics device type" );
	}

/*
=================================================
	ToString (EVertexInputRate)
=================================================
*/
	ND_ inline StringView  ToString (EVertexInputRate value) __NE___
	{
		switch_enum( value )
		{
			case EVertexInputRate::Vertex :		return "Vertex";
			case EVertexInputRate::Instance :	return "Instance";
			case EVertexInputRate::Unknown :
			case EVertexInputRate::_Count :		break;
		}
		switch_end
		RETURN_ERR( "unknown vertex input rate" );
	}

/*
=================================================
	ToString (ELogicOp)
=================================================
*/
	ND_ inline StringView  ToString (ELogicOp value) __NE___
	{
		switch_enum( value )
		{
			case ELogicOp::None :			return "None";
			case ELogicOp::Clear :			return "Clear";
			case ELogicOp::Set :			return "Set";
			case ELogicOp::Copy :			return "Copy";
			case ELogicOp::CopyInverted :	return "CopyInverted";
			case ELogicOp::NoOp :			return "NoOp";
			case ELogicOp::Invert :			return "Invert";
			case ELogicOp::And :			return "And";
			case ELogicOp::NotAnd :			return "NotAnd";
			case ELogicOp::Or :				return "Or";
			case ELogicOp::NotOr :			return "NotOr";
			case ELogicOp::Xor :			return "Xor";
			case ELogicOp::Equiv :			return "Equiv";
			case ELogicOp::AndReverse :		return "AndReverse";
			case ELogicOp::AndInverted :	return "AndInverted";
			case ELogicOp::OrReverse :		return "OrReverse";
			case ELogicOp::OrInverted :		return "OrInverted";
			case ELogicOp::_Count :
			case ELogicOp::Unknown :		break;
		}
		switch_end
		RETURN_ERR( "unknown logic op" );
	}

/*
=================================================
	ToString (EBlendFactor)
=================================================
*/
	ND_ inline StringView  ToString (EBlendFactor value) __NE___
	{
		switch_enum( value )
		{
			case EBlendFactor::Zero :				return "Zero";
			case EBlendFactor::One :				return "One";
			case EBlendFactor::SrcColor :			return "SrcColor";
			case EBlendFactor::OneMinusSrcColor :	return "OneMinusSrcColor";
			case EBlendFactor::DstColor :			return "DstColor";
			case EBlendFactor::OneMinusDstColor :	return "OneMinusDstColor";
			case EBlendFactor::SrcAlpha :			return "SrcAlpha";
			case EBlendFactor::OneMinusSrcAlpha :	return "OneMinusSrcAlpha";
			case EBlendFactor::DstAlpha :			return "DstAlpha";
			case EBlendFactor::OneMinusDstAlpha :	return "OneMinusDstAlpha";
			case EBlendFactor::ConstColor :			return "ConstColor";
			case EBlendFactor::OneMinusConstColor :	return "OneMinusConstColor";
			case EBlendFactor::ConstAlpha :			return "ConstAlpha";
			case EBlendFactor::OneMinusConstAlpha :	return "OneMinusConstAlpha";
			case EBlendFactor::SrcAlphaSaturate :	return "SrcAlphaSaturate";
			case EBlendFactor::Src1Color :			return "Src1Color";
			case EBlendFactor::OneMinusSrc1Color :	return "OneMinusSrc1Color";
			case EBlendFactor::Src1Alpha :			return "Src1Alpha";
			case EBlendFactor::OneMinusSrc1Alpha :	return "OneMinusSrc1Alpha";
			case EBlendFactor::_Count :
			case EBlendFactor::Unknown :			break;
		}
		switch_end
		RETURN_ERR( "unknown blend factor" );
	}

/*
=================================================
	ToString (EBlendOp)
=================================================
*/
	ND_ inline StringView  ToString (EBlendOp value) __NE___
	{
		switch_enum( value )
		{
			case EBlendOp::Add :		return "Add";
			case EBlendOp::Sub :		return "Sub";
			case EBlendOp::RevSub :		return "RevSub";
			case EBlendOp::Min :		return "Min";
			case EBlendOp::Max :		return "Max";
			case EBlendOp::_Count :
			case EBlendOp::Unknown :	break;
		}
		switch_end
		RETURN_ERR( "unknown blend op" );
	}

/*
=================================================
	ToString (EStencilOp)
=================================================
*/
	ND_ inline StringView  ToString (EStencilOp value) __NE___
	{
		switch_enum( value )
		{
			case EStencilOp::Keep :			return "Keep";
			case EStencilOp::Zero :			return "Zero";
			case EStencilOp::Replace :		return "Replace";
			case EStencilOp::Incr :			return "Incr";
			case EStencilOp::IncrWrap :		return "IncrWrap";
			case EStencilOp::Decr :			return "Decr";
			case EStencilOp::DecrWrap :		return "DecrWrap";
			case EStencilOp::Invert :		return "Invert";
			case EStencilOp::_Count :
			case EStencilOp::Unknown :		break;
		}
		switch_end
		RETURN_ERR( "unknown stencil op" );
	}

/*
=================================================
	ToString (EPolygonMode)
=================================================
*/
	ND_ inline StringView  ToString (EPolygonMode value) __NE___
	{
		switch_enum( value )
		{
			case EPolygonMode::Point :		return "Point";
			case EPolygonMode::Line :		return "Line";
			case EPolygonMode::Fill :		return "Fill";
			case EPolygonMode::_Count :
			case EPolygonMode::Unknown :	break;
		}
		switch_end
		RETURN_ERR( "unknown polygon mode" );
	}

/*
=================================================
	ToString (ECullMode)
=================================================
*/
	ND_ inline StringView  ToString (ECullMode value) __NE___
	{
		switch_enum( value )
		{
			case ECullMode::None :			return "None";
			case ECullMode::Front :			return "Front";
			case ECullMode::Back :			return "Back";
			case ECullMode::FontAndBack :	return "FontAndBack";
		}
		switch_end
		RETURN_ERR( "unknown cull mode" );
	}

/*
=================================================
	ToString (EPipelineDynamicState)
=================================================
*/
	ND_ inline String  ToString (EPipelineDynamicState values) __Th___
	{
		String	str;
		for (auto t : BitfieldIterate( values ))
		{
			if ( not str.empty() )
				str << " | ";

			switch_enum( t )
			{
				case EPipelineDynamicState::StencilCompareMask :	str << "StencilCompareMask";	break;
				case EPipelineDynamicState::StencilWriteMask :		str << "StencilWriteMask";		break;
				case EPipelineDynamicState::StencilReference :		str << "StencilReference";		break;
				case EPipelineDynamicState::DepthBias :				str << "DepthBias";				break;
				case EPipelineDynamicState::BlendConstants :		str << "BlendConstants";		break;
				//case EPipelineDynamicState::DepthBounds :			str << "DepthBounds";			break;
				case EPipelineDynamicState::RTStackSize :			str << "RTStackSize";			break;
				case EPipelineDynamicState::FragmentShadingRate :	str << "FragmentShadingRate";	break;
				case EPipelineDynamicState::GraphicsPipelineMask :
				case EPipelineDynamicState::All :
				case EPipelineDynamicState::_Last :
				case EPipelineDynamicState::Unknown :
				default :											RETURN_ERR( "unknown pipeline dynamic state" );
			}
			switch_end
		}
		if ( str.empty() )
			str << "0";
		return str;
	}

/*
=================================================
	ToString (EDescSetUsage)
=================================================
*/
	ND_ inline String  ToString (EDescSetUsage values) __Th___
	{
		String	str;
		for (auto t : BitfieldIterate( values ))
		{
			if ( not str.empty() )
				str << " | ";

			switch_enum( t )
			{
				case EDescSetUsage::AllowPartialyUpdate :	str << "AllowPartialyUpdate";	break;
				case EDescSetUsage::UpdateTemplate :		str << "UpdateTemplate";		break;
				case EDescSetUsage::ArgumentBuffer :		str << "ArgumentBuffer";		break;
				case EDescSetUsage::MutableArgBuffer :		str << "MutableArgBuffer";		break;
				case EDescSetUsage::MaybeUnsupported :		str << "MaybeUnsupported";		break;
				case EDescSetUsage::Unknown :
				case EDescSetUsage::_Last :
				case EDescSetUsage::All :
				default :									RETURN_ERR( "unknown descriptor set usage" );
			}
			switch_end
		}
		if ( str.empty() )
			str << "0";
		return str;
	}

/*
=================================================
	ToString (EPipelineOpt)
=================================================
*/
	ND_ inline String  ToString (EPipelineOpt values) __Th___
	{
		String	str;
		for (auto t : BitfieldIterate( values ))
		{
			if ( not str.empty() )
				str << " | ";

			switch_enum( t )
			{
				case EPipelineOpt::Optimize :						str << "Optimize";						break;
				case EPipelineOpt::CS_DispatchBase :				str << "CS_DispatchBase";				break;
				case EPipelineOpt::RT_NoNullAnyHitShaders :			str << "RT_NoNullAnyHitShaders";		break;
				case EPipelineOpt::RT_NoNullClosestHitShaders :		str << "RT_NoNullClosestHitShaders";	break;
				case EPipelineOpt::RT_NoNullMissShaders :			str << "RT_NoNullMissShaders";			break;
				case EPipelineOpt::RT_NoNullIntersectionShaders :	str << "RT_NoNullIntersectionShaders";	break;
				case EPipelineOpt::RT_SkipTriangles :				str << "RT_SkipTriangles";				break;
				case EPipelineOpt::RT_SkipAABBs :					str << "RT_SkipAABBs";					break;
				case EPipelineOpt::DontCompile :					str << "DontCompile";					break;
				case EPipelineOpt::CaptureStatistics :				str << "CaptureStatistics";				break;
				case EPipelineOpt::CaptureInternalRepresentation :	str << "CaptureInternalRepresentation";	break;
				case EPipelineOpt::Unknown :
				case EPipelineOpt::_Last :
				case EPipelineOpt::All :
				default :											RETURN_ERR( "unknown pipeline option" );
			}
			switch_end
		}
		if ( str.empty() )
			str << "0";
		return str;
	}

/*
=================================================
	ToString (EMemoryType)
=================================================
*/
	ND_ inline String  ToString (EMemoryType values) __Th___
	{
		switch ( values )
		{
			case EMemoryType::Unified :				return "Unified";
			case EMemoryType::UnifiedCached :		return "UnifiedCached";
			case EMemoryType::HostCachedCoherent :	return "HostCachedCoherent";
		}

		String	str;
		for (auto t : BitfieldIterate( values ))
		{
			if ( not str.empty() )
				str << " | ";

			switch_enum( t )
			{
				case EMemoryType::DeviceLocal :		str << "DeviceLocal";	break;
				case EMemoryType::Transient :		str << "Transient";		break;
				case EMemoryType::HostCoherent :	str << "HostCoherent";	break;
				case EMemoryType::HostCached :		str << "HostCached";	break;
				case EMemoryType::Dedicated :		str << "Dedicated";		break;

				case EMemoryType::_External :		break;
				case EMemoryType::_Last :
				case EMemoryType::All :
				case EMemoryType::HostCachedCoherent :
				case EMemoryType::Unified :
				case EMemoryType::UnifiedCached :
				case EMemoryType::Unknown :
				default :							RETURN_ERR( "unknown memory type" );
			}
			switch_end
		}
		if ( str.empty() )
			str << "0";
		return str;
	}

/*
=================================================
	ToString (EColorSpace)
=================================================
*/
	ND_ inline StringView  ToString (EColorSpace value) __NE___
	{
		switch_enum( value )
		{
			case EColorSpace::sRGB_nonlinear :			return "sRGB_nonlinear";
			case EColorSpace::Display_P3_nonlinear :	return "Display_P3_nonlinear";
			case EColorSpace::Extended_sRGB_linear :	return "Extended_sRGB_linear";
			case EColorSpace::Display_P3_linear :		return "Display_P3_linear";
			case EColorSpace::DCI_P3_nonlinear :		return "DCI_P3_nonlinear";
			case EColorSpace::BT709_linear :			return "BT709_linear";
			case EColorSpace::BT709_nonlinear :			return "BT709_nonlinear";
			case EColorSpace::BT2020_linear :			return "BT2020_linear";
			case EColorSpace::HDR10_ST2084 :			return "HDR10_ST2084";
			case EColorSpace::DolbyVision :				return "DolbyVision";
			case EColorSpace::HDR10_HLG :				return "HDR10_HLG";
			case EColorSpace::AdobeRGB_linear :			return "AdobeRGB_linear";
			case EColorSpace::AdobeRGB_nonlinear :		return "AdobeRGB_nonlinear";
			case EColorSpace::PassThrough :				return "PassThrough";
			case EColorSpace::Extended_sRGB_nonlinear :	return "Extended_sRGB_nonlinear";
			case EColorSpace::_Count :
			case EColorSpace::Unknown :					break;
		}
		switch_end
		RETURN_ERR( "unknown color space" );
	}

/*
=================================================
	ToString (EPresentMode)
=================================================
*/
	ND_ inline StringView  ToString (EPresentMode value) __NE___
	{
		switch_enum( value )
		{
			case EPresentMode::Immediate :					return "Immediate";
			case EPresentMode::Mailbox :					return "Mailbox";
			case EPresentMode::FIFO :						return "FIFO";
			case EPresentMode::FIFO_Relaxed :				return "FIFO_Relaxed";
			case EPresentMode::SharedDemandRefresh :		return "SharedDemandRefresh";
			case EPresentMode::SharedContinuousRefresh :	return "SharedContinuousRefresh";
			case EPresentMode::_Count :
			case EPresentMode::Unknown :					break;
		}
		switch_end
		RETURN_ERR( "unknown present mode" );
	}

/*
=================================================
	ToString (EDescriptorType)
=================================================
*/
	ND_ inline StringView  ToString (EDescriptorType value) __NE___
	{
		switch_enum( value )
		{
			case EDescriptorType::UniformBuffer :					return "UniformBuffer";
			case EDescriptorType::StorageBuffer :					return "StorageBuffer";
			case EDescriptorType::UniformTexelBuffer :				return "UniformTexelBuffer";
			case EDescriptorType::StorageTexelBuffer :				return "StorageTexelBuffer";
			case EDescriptorType::StorageImage :					return "StorageImage";
			case EDescriptorType::SampledImage :					return "SampledImage";
			case EDescriptorType::CombinedImage :					return "CombinedImage";
			case EDescriptorType::CombinedImage_ImmutableSampler :	return "CombinedImage_ImmutableSampler";
			case EDescriptorType::SubpassInput :					return "SubpassInput";
			case EDescriptorType::Sampler :							return "Sampler";
			case EDescriptorType::ImmutableSampler :				return "ImmutableSampler";
			case EDescriptorType::RayTracingScene :					return "RayTracingScene";
			case EDescriptorType::_Count :
			case EDescriptorType::Unknown :							break;
		}
		switch_end
		RETURN_ERR( "unknown descriptor type" );
	}

	ND_ inline StringView  ToString (EDescriptorType value, int) __NE___
	{
		switch_enum( value )
		{
			case EDescriptorType::UniformBuffer :
			case EDescriptorType::StorageBuffer :					return "Buffer";
			case EDescriptorType::UniformTexelBuffer :
			case EDescriptorType::StorageTexelBuffer :				return "TexelBuffer";
			case EDescriptorType::StorageImage :
			case EDescriptorType::SampledImage :
			case EDescriptorType::CombinedImage :
			case EDescriptorType::CombinedImage_ImmutableSampler :
			case EDescriptorType::SubpassInput :					return "Image";
			case EDescriptorType::Sampler :
			case EDescriptorType::ImmutableSampler :				return "Sampler";
			case EDescriptorType::RayTracingScene :					return "RayTracingScene";
			case EDescriptorType::_Count :
			case EDescriptorType::Unknown :							break;
		}
		switch_end
		RETURN_ERR( "unknown descriptor type" );
	}

/*
=================================================
	ToString (ESubgroupOperation)
=================================================
*/
	ND_ inline StringView  ToString (ESubgroupOperation value) __NE___
	{
		switch_enum( value )
		{
			#define CASE( _name_ )	case ESubgroupOperation::_name_ : return AE_TOSTRING( _name_ )
			CASE( IndexAndSize );
			CASE( Elect );
			CASE( Barrier );
			CASE( Any );
			CASE( All );
			CASE( AllEqual );
			CASE( Add );
			CASE( Mul );
			CASE( Min );
			CASE( Max );
			CASE( And );
			CASE( Or );
			CASE( Xor );
			CASE( InclusiveMul );
			CASE( InclusiveAdd );
			CASE( InclusiveMin );
			CASE( InclusiveMax );
			CASE( InclusiveAnd );
			CASE( InclusiveOr );
			CASE( InclusiveXor );
			CASE( ExclusiveAdd );
			CASE( ExclusiveMul );
			CASE( ExclusiveMin );
			CASE( ExclusiveMax );
			CASE( ExclusiveAnd );
			CASE( ExclusiveOr );
			CASE( ExclusiveXor );
			CASE( Ballot );
			CASE( Broadcast );
			CASE( BroadcastFirst );
			CASE( InverseBallot );
			CASE( BallotBitExtract );
			CASE( BallotBitCount );
			CASE( BallotInclusiveBitCount );
			CASE( BallotExclusiveBitCount );
			CASE( BallotFindLSB );
			CASE( BallotFindMSB );
			CASE( Shuffle );
			CASE( ShuffleXor );
			CASE( ShuffleUp );
			CASE( ShuffleDown );
			CASE( ClusteredAdd );
			CASE( ClusteredMul );
			CASE( ClusteredMin );
			CASE( ClusteredMax );
			CASE( ClusteredAnd );
			CASE( ClusteredOr );
			CASE( ClusteredXor );
			CASE( QuadBroadcast );
			CASE( QuadSwapHorizontal );
			CASE( QuadSwapVertical );
			CASE( QuadSwapDiagonal );
			#undef CASE
			case ESubgroupOperation::_Count :	break;
		}
		switch_end
		RETURN_ERR( "unknown subgroup operation type" );
	}

/*
=================================================
	ToString (EResourceState)
=================================================
*/
	ND_ inline String  ToString (const EResourceState value) __Th___
	{
		using _EResState = Graphics::_EResState;

		String	str;
		switch_enum( ToEResState( value ))
		{
			case _EResState::ShaderStorage_Read :				str += "ShaderStorage_Read";					break;
			case _EResState::ShaderStorage_Write :				str += "ShaderStorage_Write";					break;
			case _EResState::ShaderStorage_RW :					str += "ShaderStorage_RW";						break;
			case _EResState::ShaderUniform :					str += "ShaderUniform";							break;
			case _EResState::ShaderSample :						str += "ShaderSample";							break;
			case _EResState::CopySrc :							str += "CopySrc";								break;
			case _EResState::CopyDst :							str += "CopyDst";								break;
			case _EResState::ClearDst :							str += "ClearDst";								break;
			case _EResState::BlitSrc :							str += "BlitSrc";								break;
			case _EResState::BlitDst :							str += "BlitDst";								break;
			case _EResState::InputColorAttachment :				str += "InputColorAttachment";					break;
			case _EResState::InputColorAttachment_RW :			str += "InputColorAttachment_RW";				break;
			case _EResState::ColorAttachment :					str += "ColorAttachment";						break;
			case _EResState::DepthStencilTest :					str += "DepthStencilTest";						break;
			case _EResState::DepthStencilAttachment_RW :		str += "DepthStencilAttachment_RW";				break;
			case _EResState::DepthTest_StencilRW :				str += "DepthTest_StencilRW";					break;
			case _EResState::DepthRW_StencilTest :				str += "DepthRW_StencilTest";					break;
			case _EResState::DepthStencilTest_ShaderSample :	str += "DepthStencilTest_ShaderSample";			break;
			case _EResState::DepthTest_DepthSample_StencilRW :	str += "DepthTest_DepthSample_StencilRW";		break;
			case _EResState::InputDepthStencilAttachment :		str += "InputDepthStencilAttachment";			break;
			case _EResState::InputDepthStencilAttachment_RW :	str += "InputDepthStencilAttachment_RW";		break;
			case _EResState::Host_Read :						str += "Host_Read";								break;
			case _EResState::Host_Write :						str += "Host_Write";							break;
			case _EResState::PresentImage :						str += "PresentImage";							break;
			case _EResState::IndirectBuffer :					str += "IndirectBuffer";						break;
			case _EResState::IndexBuffer :						str += "IndexBuffer";							break;
			case _EResState::VertexBuffer :						str += "VertexBuffer";							break;
			case _EResState::CopyRTAS_Read :					str += "CopyRTAS_Read";							break;
			case _EResState::CopyRTAS_Write :					str += "CopyRTAS_Write";						break;
			case _EResState::BuildRTAS_Read :					str += "BuildRTAS_Read";						break;
			case _EResState::BuildRTAS_RW :						str += "BuildRTAS_RW";							break;
			case _EResState::BuildRTAS_IndirectBuffer :			str += "BuildRTAS_IndirectBuffer";				break;
			case _EResState::ShaderRTAS :						str += "ShaderRTAS";							break;
			case _EResState::RTShaderBindingTable :				str += "RTShaderBindingTable";					break;
			case _EResState::ShadingRateImage :					str += "ShadingRateImage";						break;
			case _EResState::Unknown :							str += "Unknown";								break;
			case _EResState::Preserve :							str += "Preserve";								break;
			case _EResState::General :							str += "General";								break;
			case _EResState::_AccessCount :
			default :											DBG_WARNING( "unknown resource state" );		break;
		}
		switch_end

		if ( AnyBits( value, EResourceState::DSTestBeforeFS ))
			str += " | DSTestBeforeFS";

		if ( AnyBits( value, EResourceState::DSTestAfterFS ))
			str += " | DSTestAfterFS";

		if ( AnyBits( value, EResourceState::Invalidate ))
			str += " | Invalidate";

		// stages
		{
			EResourceState	stages = value & EResourceState::AllShaders;

			if ( AllBits( stages, EResourceState::AllShaders ))
			{
				stages &= ~EResourceState::AllShaders;
				str += " | AllShaders";
			}
			if ( AllBits( stages, EResourceState::AllGraphicsShaders ))
			{
				stages &= ~EResourceState::AllGraphicsShaders;
				str += " | AllGraphicsShaders";
			}
			if ( AllBits( stages, EResourceState::PreRasterizationShaders ))
			{
				stages &= ~EResourceState::PreRasterizationShaders;
				str += " | PreRasterizationShaders";
			}
			if ( AllBits( stages, EResourceState::PostRasterizationShaders ))
			{
				stages &= ~EResourceState::PostRasterizationShaders;
				str += " | PostRasterizationShaders";
			}

			for (auto stage : BitfieldIterate( stages ))
			{
				str += " | ";
				switch ( stage )
				{
					case EResourceState::MeshTaskShader :			str += "MeshTaskShader";			break;
					case EResourceState::VertexProcessingShaders :	str += "VertexProcessingShaders";	break;
					case EResourceState::TileShader :				str += "TileShader";				break;
					case EResourceState::FragmentShader :			str += "FragmentShader";			break;
					case EResourceState::ComputeShader :			str += "ComputeShader";				break;
					case EResourceState::RayTracingShaders :		str += "RayTracingShaders";			break;
					default :										DBG_WARNING( "unknown resource state stage" );	break;
				}
				StaticAssert( uint(EResourceState::AllShaders) == 0x1F8000 );
			}
		}

		return str;
	}

/*
=================================================
	ToString (ESurfaceFormat)
=================================================
*/
	ND_ inline StringView  ToString (ESurfaceFormat value)
	{
		switch_enum( value )
		{
			case ESurfaceFormat::BGRA8_sRGB_nonlinear :			return "BGRA8_sRGB_nonlinear";
			case ESurfaceFormat::RGBA8_sRGB_nonlinear :			return "RGBA8_sRGB_nonlinear";
			case ESurfaceFormat::BGRA8_BT709_nonlinear :		return "BGRA8_BT709_nonlinear";
			case ESurfaceFormat::RGBA16F_Extended_sRGB_linear :	return "RGBA16F_Extended_sRGB_linear";
			case ESurfaceFormat::RGBA16F_sRGB_nonlinear :		return "RGBA16F_sRGB_nonlinear";
			case ESurfaceFormat::RGBA16F_BT709_nonlinear :		return "RGBA16F_BT709_nonlinear";
			case ESurfaceFormat::RGBA16F_HDR10_ST2084 :			return "RGBA16F_HDR10_ST2084";
			case ESurfaceFormat::RGBA16F_BT2020_linear :		return "RGBA16F_BT2020_linear";
			case ESurfaceFormat::RGB10A2_sRGB_nonlinear :		return "RGB10A2_sRGB_nonlinear";
			case ESurfaceFormat::RGB10A2_HDR10_ST2084 :			return "RGB10A2_HDR10_ST2084";
			case ESurfaceFormat::_Count :
			case ESurfaceFormat::Unknown :						break;
		}
		switch_end
		RETURN_ERR( "unknown surface format" );
	}

/*
=================================================
	ToString (ESamplerChromaLocation)
=================================================
*/
	ND_ inline StringView  ToString (ESamplerChromaLocation value) __NE___
	{
		switch_enum( value )
		{
			case ESamplerChromaLocation::CositedEven :	return "CositedEven";
			case ESamplerChromaLocation::Midpoint :		return "Midpoint";
			case ESamplerChromaLocation::_Count :
			case ESamplerChromaLocation::Unknown :		break;
		}
		switch_end
		RETURN_ERR( "unknown sampler chroma location" );
	}

/*
=================================================
	ToString (ESamplerYcbcrModelConversion)
=================================================
*/
	ND_ inline StringView  ToString (ESamplerYcbcrModelConversion value) __NE___
	{
		switch_enum( value )
		{
			case ESamplerYcbcrModelConversion::RGB_Identity :	return "RGB_Identity";
			case ESamplerYcbcrModelConversion::Ycbcr_Identity :	return "Ycbcr_Identity";
			case ESamplerYcbcrModelConversion::Ycbcr_709 :		return "Ycbcr_709";
			case ESamplerYcbcrModelConversion::Ycbcr_601 :		return "Ycbcr_601";
			case ESamplerYcbcrModelConversion::Ycbcr_2020 :		return "Ycbcr_2020";
			case ESamplerYcbcrModelConversion::_Count :
			case ESamplerYcbcrModelConversion::Unknown :		break;
		}
		switch_end
		RETURN_ERR( "unknown sampler ycbcr model conversion" );
	}

/*
=================================================
	ToString (ESamplerYcbcrRange)
=================================================
*/
	ND_ inline StringView  ToString (ESamplerYcbcrRange value) __NE___
	{
		switch_enum( value )
		{
			case ESamplerYcbcrRange::ITU_Full :		return "ITU_Full";
			case ESamplerYcbcrRange::ITU_Narrow :	return "ITU_Narrow";
			case ESamplerYcbcrRange::_Count :
			case ESamplerYcbcrRange::Unknown :		break;
		}
		switch_end
		RETURN_ERR( "unknown sampler ycbcr range" );
	}

/*
=================================================
	ToString (ImageSwizzle)
=================================================
*/
	ND_ inline String  ToString (const ImageSwizzle &value) __NE___
	{
		String			str;
		const uint4		comp		= value.ToVec();
		const char		comp_str[]	= ".RGBA01";

		for (uint i = 0; i < 4; ++i) {
			str << comp_str[ comp[i] ];
		}
		return str;
	}

/*
=================================================
	ToString (EVideoFormat)
=================================================
*/
	ND_ inline StringView  ToString (EVideoFormat value) __NE___
	{
		switch_enum( value )
		{
			case EVideoFormat::YUV420P :		return "YUV420P";
			case EVideoFormat::YUV422P :		return "YUV422P";
			case EVideoFormat::YUV444P :		return "YUV444P";

			case EVideoFormat::YUYV422 :		return "YUYV422";
			case EVideoFormat::UYVY422 :		return "UYVY422";
			case EVideoFormat::Y210 :			return "Y210";
			case EVideoFormat::Y212 :			return "Y212";
			case EVideoFormat::XV30 :			return "XV30";
			case EVideoFormat::XV36 :			return "XV36";

			case EVideoFormat::YUV420P10 :		return "YUV420P10";
			case EVideoFormat::YUV422P10 :		return "YUV422P10";
			case EVideoFormat::YUV444P10 :		return "YUV444P10";
			case EVideoFormat::YUV420P12 :		return "YUV420P12";
			case EVideoFormat::YUV422P12 :		return "YUV422P12";
			case EVideoFormat::YUV444P12 :		return "YUV444P12";
			case EVideoFormat::YUV420P16 :		return "YUV420P16";
			case EVideoFormat::YUV422P16 :		return "YUV422P16";
			case EVideoFormat::YUV444P16 :		return "YUV444P16";

			case EVideoFormat::YUVA420P :		return "YUVA420P";
			case EVideoFormat::YUVA422P :		return "YUVA422P";
			case EVideoFormat::YUVA444P :		return "YUVA444P";
			case EVideoFormat::YUVA420P10 :		return "YUVA420P10";
			case EVideoFormat::YUVA422P10 :		return "YUVA422P10";
			case EVideoFormat::YUVA444P10 :		return "YUVA444P10";
			case EVideoFormat::YUVA420P16 :		return "YUVA420P16";
			case EVideoFormat::YUVA422P16 :		return "YUVA422P16";
			case EVideoFormat::YUVA444P16 :		return "YUVA444P16";

			case EVideoFormat::NV12 :			return "NV12";
			case EVideoFormat::P010 :			return "P010";
			case EVideoFormat::P012 :			return "P012";
			case EVideoFormat::P016 :			return "P016";

			case EVideoFormat::NV16 :			return "NV16";
			case EVideoFormat::P210 :			return "P210";
			case EVideoFormat::P212 :			return "P212";
			case EVideoFormat::P216 :			return "P216";

			case EVideoFormat::NV24 :			return "NV24";
			case EVideoFormat::P410 :			return "P410";
			case EVideoFormat::P412 :			return "P412";
			case EVideoFormat::P416 :			return "P416";

			case EVideoFormat::NV21 :			return "NV21";
			case EVideoFormat::NV42 :			return "NV42";
			case EVideoFormat::NV20 :			return "NV20";

			case EVideoFormat::BGR0 :			return "BGR0";
			case EVideoFormat::BGRA :			return "BGRA";
			case EVideoFormat::RGB0 :			return "RGB0";
			case EVideoFormat::RGBA :			return "RGBA";

			case EVideoFormat::Unknown :
			case EVideoFormat::_Count :			break;
		}
		switch_end
		RETURN_ERR( "unknown video format" );
	}

/*
=================================================
	ToString (EVideoCodec)
=================================================
*/
	ND_ inline StringView  ToString (EVideoCodec value) __NE___
	{
		switch_enum( value )
		{
			case EVideoCodec::MPEG4 :	return "MPEG4";
			case EVideoCodec::H264 :	return "H264";
			case EVideoCodec::H265 :	return "H265";
			case EVideoCodec::H266 :	return "H266";
			case EVideoCodec::WEBP :	return "WEBP";
			case EVideoCodec::VP8 :		return "VP8";
			case EVideoCodec::VP9 :		return "VP9";
			case EVideoCodec::AV1 :		return "AV1";
			case EVideoCodec::Unknown :
			case EVideoCodec::_Count :	break;
		}
		switch_end
		RETURN_ERR( "unknown video codec" );
	}

/*
=================================================
	ToString (EPixelFormatExternal)
=================================================
*/
	ND_ inline StringView  ToString (EPixelFormatExternal value) __NE___
	{
		switch_enum( value )
		{
			case EPixelFormatExternal::Android_Depth16 :		return "Android_Depth16";
			case EPixelFormatExternal::Android_DepthJPEG :		return "Android_DepthJPEG";
			case EPixelFormatExternal::Android_DepthPointCloud:	return "Android_DepthPointCloud";
			case EPixelFormatExternal::Android_JPEG :			return "Android_JPEG";
			case EPixelFormatExternal::Android_Private :		return "Android_Private";
			case EPixelFormatExternal::Android_Raw16 :			return "Android_Raw16";
			case EPixelFormatExternal::Android_Raw12 :			return "Android_Raw12";
			case EPixelFormatExternal::Android_Raw10 :			return "Android_Raw10";
			case EPixelFormatExternal::Android_RawPrivate :		return "Android_RawPrivate";
			case EPixelFormatExternal::Android_NV16 :			return "Android_NV16";
			case EPixelFormatExternal::Android_NV21 :			return "Android_NV21";
			case EPixelFormatExternal::Android_YCBCR_P010 :		return "Android_YCBCR_P010";
			case EPixelFormatExternal::Android_YUV_420 :		return "Android_YUV_420";
			case EPixelFormatExternal::Android_YUV_422 :		return "Android_YUV_422";
			case EPixelFormatExternal::Android_YUV_444 :		return "Android_YUV_444";
			case EPixelFormatExternal::Android_YUY2 :			return "Android_YUY2";
			case EPixelFormatExternal::Android_YV12 :			return "Android_YV12";
			case EPixelFormatExternal::Android_Y8 :				return "Android_Y8";
			case EPixelFormatExternal::Android_HEIC :			return "Android_HEIC";

			case EPixelFormatExternal::_Android_End :
			case EPixelFormatExternal::_Count :
			case EPixelFormatExternal::Unknown :				break;
		}
		switch_end
		RETURN_ERR( "unknown external pixel format" );
	}


} // AE::Base
