// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/StringUtils.h"
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

namespace AE::Base
{
	using Graphics::EQueueType;
	using Graphics::EPixelFormat;
	using Graphics::EAttachmentLoadOp;
	using Graphics::EAttachmentStoreOp;
	using Graphics::EResourceState;
	using Graphics::EShaderIO;
	using Graphics::EShaderStages;
	using Graphics::EShader;
	using Graphics::EPrimitive;
	using Graphics::EVertexType;
	using Graphics::EVendorID;
	using Graphics::EGraphicsDeviceID;
	using Graphics::ESubgroupOperation;
	using Graphics::EFilter;
	using Graphics::EMipmapFilter;
	using Graphics::EAddressMode;
	using Graphics::ECompareOp;
	using Graphics::EBorderColor;
	using Graphics::EReductionMode;
	using Graphics::ESamplerUsage;
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
	
/*
=================================================
	ToString (EQueueType)
=================================================
*/
	ND_ inline StringView  ToString (EQueueType queue)
	{
		BEGIN_ENUM_CHECKS();
		switch ( queue )
		{
			case EQueueType::Graphics :		return "Graphics";
			case EQueueType::AsyncCompute :	return "AsyncCompute";
			case EQueueType::AsyncTransfer:	return "AsyncTransfer";
			case EQueueType::_Count :
			case EQueueType::Unknown :		break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown queue type" );
	}
	
/*
=================================================
	ToString (EPixelFormat)
=================================================
*/
	ND_ inline StringView  ToString (EPixelFormat value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			#define AE_PIXELFORMAT_VISIT( _name_ )		case EPixelFormat::_name_ : return AE_TOSTRING( _name_ );
			AE_PIXELFORMAT_LIST( AE_PIXELFORMAT_VISIT )
			#undef AE_PIXELFORMAT_VISIT
				
			case EPixelFormat::SwapchainColor : return "SwapchainColor";
			case EPixelFormat::Unknown :		return "Unknown";
			case EPixelFormat::_Count :			break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown pixel format" );
	}

	
/*
=================================================
	ToString (EAttachmentLoadOp)
=================================================
*/
	ND_ inline StringView  ToString (EAttachmentLoadOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EAttachmentLoadOp::Invalidate :	return "Invalidate";
			case EAttachmentLoadOp::Load :			return "Load";
			case EAttachmentLoadOp::Clear :			return "Clear";
			case EAttachmentLoadOp::None :			return "None";
			case EAttachmentLoadOp::_Count :
			case EAttachmentLoadOp::Unknown :		break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown attachment load op" );
	}
	
/*
=================================================
	ToString (EAttachmentStoreOp)
=================================================
*/
	ND_ inline StringView  ToString (EAttachmentStoreOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EAttachmentStoreOp::Invalidate :					return "Invalidate";
			case EAttachmentStoreOp::Store :						return "Store";
			case EAttachmentStoreOp::None :							return "None";
			case EAttachmentStoreOp::StoreCustomSamplePositions :	return "StoreCustomSamplePositions";
			case EAttachmentStoreOp::_Count :
			case EAttachmentStoreOp::Unknown :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown attachment store op" );
	}
	
/*
=================================================
	ToString (EShaderIO)
=================================================
*/
	ND_ inline StringView  ToString (EShaderIO value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
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
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown fragment IO type" );
	}
	
/*
=================================================
	ToString (EShader)
=================================================
*/
	ND_ inline StringView  ToString (EShader value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
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
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown shader type" );
	}
	
/*
=================================================
	ToString (EShaderStages)
=================================================
*/
	ND_ inline String  ToString (EShaderStages values)
	{
		String	str;
		while ( values != Default )
		{
			EShader	bit = ExtractBitLog2<EShader>( INOUT values );

			if ( not str.empty() )
				str << " | ";

			str << ToString( bit );
		}
		return str;
	}
	
/*
=================================================
	ToString (EPrimitive)
=================================================
*/
	ND_ inline StringView  ToString (EPrimitive value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
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
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown primitive type" );
	}
	
/*
=================================================
	ToString (EVendorID)
=================================================
*/
	ND_ inline StringView  ToString (EVendorID value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EVendorID::AMD :		return "AMD";
			case EVendorID::NVidia :	return "NVidia";
			case EVendorID::Intel :		return "Intel";
			case EVendorID::ARM :		return "ARM";
			case EVendorID::Qualcomm :	return "Qualcomm";
			case EVendorID::ImgTech :	return "ImgTech";
			case EVendorID::Microsoft :	return "Microsoft";
			case EVendorID::Apple :		return "Apple";
			case EVendorID::Mesa :		return "Mesa";
			case EVendorID::Broadcom :	return "Broadcom";
			case EVendorID::Samsung :	return "Samsung";
			case EVendorID::VeriSilicon:return "VeriSilicon";
			case EVendorID::_Count :
			case EVendorID::Unknown :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown vendor id" );
	}
	
/*
=================================================
	ToString (EFilter)
=================================================
*/
	ND_ inline StringView  ToString (EFilter value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EFilter::Nearest :		return "Nearest";
			case EFilter::Linear :		return "Linear";
			case EFilter::_Count :
			case EFilter::Unknown :
			default :					break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown filter type" );
	}
	
/*
=================================================
	ToString (EMipmapFilter)
=================================================
*/
	ND_ inline StringView  ToString (EMipmapFilter value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EMipmapFilter::None :		return "None";
			case EMipmapFilter::Nearest :	return "Nearest";
			case EMipmapFilter::Linear :	return "Linear";
			case EMipmapFilter::_Count :
			case EMipmapFilter::Unknown :
			default :						break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown filter type" );
	}
	
/*
=================================================
	ToString (EAddressMode)
=================================================
*/
	ND_ inline StringView  ToString (EAddressMode value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
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
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown address mode" );
	}
	
/*
=================================================
	ToString (ECompareOp)
=================================================
*/
	ND_ inline StringView  ToString (ECompareOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
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
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown compare op" );
	}
	
/*
=================================================
	ToString (EBorderColor)
=================================================
*/
	ND_ inline StringView  ToString (EBorderColor value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
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
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown border color type" );
	}
	
/*
=================================================
	ToString (EReductionMode)
=================================================
*/
	ND_ inline StringView  ToString (EReductionMode value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EReductionMode::Average :		return "Average";
			case EReductionMode::Min :			return "Min";
			case EReductionMode::Max :			return "Max";
			case EReductionMode::_Count :
			case EReductionMode::Unknown :
			default :							break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown sampler reduction mode" );
	}
	
	
/*
=================================================
	ToString (ESamplerUsage)
=================================================
*/
	ND_ inline StringView  ToString (ESamplerUsage value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case ESamplerUsage::Default :						return "Default";
			case ESamplerUsage::Subsampled :					return "Subsampled";
			case ESamplerUsage::SubsampledCoarseReconstruction:	return "SubsampledCoarseReconstruction";
			case ESamplerUsage::_Count :
			default :											break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown sampler usage" );
	}

/*
=================================================
	ToString (EVertexType)
=================================================
*/
	ND_ inline StringView  ToString (EVertexType value)
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
	ND_ inline StringView  ToString (EGraphicsDeviceID value)
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
	ND_ inline StringView  ToString (EVertexInputRate value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EVertexInputRate::Vertex :		return "Vertex";
			case EVertexInputRate::Instance :	return "Instance";
			case EVertexInputRate::Unknown :
			case EVertexInputRate::_Count :		break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown vertex input rate" );
	}
	
/*
=================================================
	ToString (ELogicOp)
=================================================
*/
	ND_ inline StringView  ToString (ELogicOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
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
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown logic op" );
	}
	
/*
=================================================
	ToString (EBlendFactor)
=================================================
*/
	ND_ inline StringView  ToString (EBlendFactor value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
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
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown blend factor" );
	}
	
/*
=================================================
	ToString (EBlendOp)
=================================================
*/
	ND_ inline StringView  ToString (EBlendOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EBlendOp::Add :		return "Add";
			case EBlendOp::Sub :		return "Sub";
			case EBlendOp::RevSub :		return "RevSub";
			case EBlendOp::Min :		return "Min";
			case EBlendOp::Max :		return "Max";
			case EBlendOp::_Count :
			case EBlendOp::Unknown :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown blend op" );
	}
	
/*
=================================================
	ToString (EStencilOp)
=================================================
*/
	ND_ inline StringView  ToString (EStencilOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
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
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown stencil op" );
	}
	
/*
=================================================
	ToString (EPolygonMode)
=================================================
*/
	ND_ inline StringView  ToString (EPolygonMode value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EPolygonMode::Point :		return "Point";
			case EPolygonMode::Line :		return "Line";
			case EPolygonMode::Fill :		return "Fill";
			case EPolygonMode::_Count :
			case EPolygonMode::Unknown :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown polygon mode" );
	}
	
/*
=================================================
	ToString (ECullMode)
=================================================
*/
	ND_ inline StringView  ToString (ECullMode value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case ECullMode::None :			return "None";
			case ECullMode::Front :			return "Front";
			case ECullMode::Back :			return "Back";
			case ECullMode::FontAndBack :	return "FontAndBack";
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown cull mode" );
	}
	
/*
=================================================
	ToString (EPipelineDynamicState)
=================================================
*/
	ND_ inline String  ToString (EPipelineDynamicState values)
	{
		String	str;
		while ( values != Zero )
		{
			if ( not str.empty() )
				str << " | ";

			BEGIN_ENUM_CHECKS();
			switch ( ExtractBit( INOUT values ))
			{
				case EPipelineDynamicState::StencilCompareMask :	str << "StencilCompareMask";	break;
				case EPipelineDynamicState::StencilWriteMask :		str << "StencilWriteMask";		break;
				case EPipelineDynamicState::StencilReference :		str << "StencilReference";		break;
				case EPipelineDynamicState::DepthBias :				str << "DepthBias";				break;
				case EPipelineDynamicState::BlendConstants :		str << "BlendConstants";		break;
				//case EPipelineDynamicState::DepthBounds :			str << "DepthBounds";			break;
				case EPipelineDynamicState::RTStackSize :			str << "RTStackSize";			break;
				case EPipelineDynamicState::GraphicsPipelineMask :
				case EPipelineDynamicState::All :
				case EPipelineDynamicState::_Last :
				case EPipelineDynamicState::Unknown :
				default :											RETURN_ERR( "unknown pipeline dynamic state" );
			}
			END_ENUM_CHECKS();
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
	ND_ inline String  ToString (EDescSetUsage values)
	{
		String	str;
		while ( values != Zero )
		{
			if ( not str.empty() )
				str << " | ";

			BEGIN_ENUM_CHECKS();
			switch ( ExtractBit( INOUT values ))
			{
				case EDescSetUsage::AllowPartialyUpdate :	str << "AllowPartialyUpdate";	break;
				case EDescSetUsage::UpdateTemplate :		str << "UpdateTemplate";		break;
				case EDescSetUsage::ArgumentBuffer :		str << "ArgumentBuffer";		break;
				case EDescSetUsage::MutableArgBuffer :		str << "MutableArgBuffer";		break;
				case EDescSetUsage::Unknown :
				case EDescSetUsage::_Last :
				case EDescSetUsage::All :
				default :									RETURN_ERR( "unknown descriptor set usage" );
			}
			END_ENUM_CHECKS();
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
	ND_ inline String  ToString (EPipelineOpt values)
	{
		String	str;
		while ( values != Zero )
		{
			if ( not str.empty() )
				str << " | ";

			BEGIN_ENUM_CHECKS();
			switch ( ExtractBit( INOUT values ))
			{
				case EPipelineOpt::Optimize :						str << "Optimize";						break;
				case EPipelineOpt::CS_DispatchBase :				str << "CS_DispatchBase";				break;
				case EPipelineOpt::RT_NoNullAnyHitShaders :			str << "RT_NoNullAnyHitShaders";		break;
				case EPipelineOpt::RT_NoNullClosestHitShaders :		str << "RT_NoNullClosestHitShaders";	break;
				case EPipelineOpt::RT_NoNullMissShaders :			str << "RT_NoNullMissShaders";			break;
				case EPipelineOpt::RT_NoNullIntersectionShaders :	str << "RT_NoNullIntersectionShaders";	break;
				case EPipelineOpt::RT_SkipTriangles :				str << "RT_SkipTriangles";				break;
				case EPipelineOpt::RT_SkipAABBs :					str << "RT_SkipAABBs";					break;
				case EPipelineOpt::Unknown :
				case EPipelineOpt::_Last :
				case EPipelineOpt::All :
				default :											RETURN_ERR( "unknown pipeline option" );
			}
			END_ENUM_CHECKS();
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
	ND_ inline String  ToString (EMemoryType values)
	{
		String	str;
		while ( values != Zero )
		{
			if ( not str.empty() )
				str << " | ";

			BEGIN_ENUM_CHECKS();
			switch ( ExtractBit( INOUT values ))
			{
				case EMemoryType::DeviceLocal :		str << "DeviceLocal";	break;
				case EMemoryType::Transient :		str << "Transient";		break;
				case EMemoryType::HostCocherent :	str << "HostCocherent";	break;
				case EMemoryType::HostCached :		str << "HostCached";	break;
				case EMemoryType::Dedicated :		str << "Dedicated";		break;

				case EMemoryType::_External :		break;
				case EMemoryType::_Last :
				case EMemoryType::All :
				case EMemoryType::HostCachedCocherent :
				case EMemoryType::Unified :
				case EMemoryType::UnifiedCached :
				case EMemoryType::Unknown :
				default :							RETURN_ERR( "unknown memory type" );
			}
			END_ENUM_CHECKS();
		}
		if ( str.empty() )
			str << "0";
		return str;
	}

/*
=================================================
	ToString (EDescriptorType)
=================================================
*/
	ND_ inline StringView  ToString (EDescriptorType value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
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
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown descriptor type" );
	}

	ND_ inline StringView  ToString (EDescriptorType value, int)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
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
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown descriptor type" );
	}

/*
=================================================
	ToString (ESubgroupOperation)
=================================================
*/
	ND_ inline StringView  ToString (ESubgroupOperation value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			#define CASE( _name_ )	case ESubgroupOperation::_name_ : return AE_TOSTRING( _name_ )
			CASE( Size );
			CASE( InvocationID );
			CASE( Elect );
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
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown subgroup operation type" );
	}

/*
=================================================
	ToString (EResourceState)
=================================================
*/
	ND_ inline String  ToString (const EResourceState value)
	{
		String	str;
		
		BEGIN_ENUM_CHECKS();
		switch ( value & EResourceState::_AccessMask )
		{
			case EResourceState::ShaderStorage_Read :				str += "ShaderStorage_Read";					break;
			case EResourceState::ShaderStorage_Write :				str += "ShaderStorage_Write";					break;
			case EResourceState::ShaderStorage_RW :					str += "ShaderStorage_RW";						break;
			case EResourceState::ShaderUniform :					str += "ShaderUniform";							break;
			case EResourceState::ShaderSample :						str += "ShaderSample";							break;
			case EResourceState::CopySrc :							str += "CopySrc";								break;
			case EResourceState::CopyDst :							str += "CopyDst";								break;
			case EResourceState::ClearDst :							str += "ClearDst";								break;
			case EResourceState::BlitSrc :							str += "BlitSrc";								break;
			case EResourceState::BlitDst :							str += "BlitDst";								break;
			case EResourceState::InputColorAttachment :				str += "InputColorAttachment";					break;
			case EResourceState::InputColorAttachment_RW :			str += "InputColorAttachment_RW";				break;
			case EResourceState::ColorAttachment_Write :			str += "ColorAttachment_Write";					break;
			case EResourceState::ColorAttachment_RW :				str += "ColorAttachment_RW";					break;
			case EResourceState::DepthStencilAttachment_Read :		str += "DepthStencilAttachment_Read";			break;
			case EResourceState::DepthStencilAttachment_Write :		str += "DepthStencilAttachment_Write";			break;
			case EResourceState::DepthStencilAttachment_RW :		str += "DepthStencilAttachment_RW";				break;
			case EResourceState::DepthTest_StencilRW :				str += "DepthTest_StencilRW";					break;
			case EResourceState::DepthRW_StencilTest :				str += "DepthRW_StencilTest";					break;
			case EResourceState::DepthStencilTest_ShaderSample :	str += "DepthStencilTest_ShaderSample";			break;
			case EResourceState::DepthTest_DepthSample_StencilRW :	str += "DepthTest_DepthSample_StencilRW";		break;
			case EResourceState::InputDepthStencilAttachment :		str += "InputDepthStencilAttachment";			break;
			case EResourceState::InputDepthStencilAttachment_RW :	str += "InputDepthStencilAttachment_RW";		break;
			case EResourceState::Host_Read :						str += "Host_Read";								break;
			case EResourceState::Host_Write :						str += "Host_Write";							break;
			case EResourceState::Host_RW :							str += "Host_RW";								break;
			case EResourceState::PresentImage :						str += "PresentImage";							break;
			case EResourceState::IndirectBuffer :					str += "IndirectBuffer";						break;
			case EResourceState::IndexBuffer :						str += "IndexBuffer";							break;
			case EResourceState::VertexBuffer :						str += "VertexBuffer";							break;
			case EResourceState::BuildRTAS_Read :					str += "BuildRTAS_Read";						break;
			case EResourceState::BuildRTAS_Write :					str += "BuildRTAS_Write";						break;
			case EResourceState::BuildRTAS_RW :						str += "BuildRTAS_RW";							break;
			case EResourceState::BuildRTAS_ScratchBuffer :			str += "BuildRTAS_ScratchBuffer";				break;
			case EResourceState::ShaderRTAS_Read :					str += "ShaderRTAS_Read";						break;
			case EResourceState::RTShaderBindingTable :				str += "RTShaderBindingTable";					break;
			case EResourceState::ShadingRateImage :					str += "ShadingRateImage";						break;
			case EResourceState::FragmentDensityMap :				str += "FragmentDensityMap";					break;
			case EResourceState::Unknown :							str += "Unknown";								break;
			case EResourceState::Preserve :							str += "Preserve";								break;

			case EResourceState::DSTestBeforeFS :
			case EResourceState::DSTestAfterFS :
			case EResourceState::Invalidate :
			case EResourceState::_FlagsMask :
			case EResourceState::_AccessMask :
			case EResourceState::PreRasterizationShaders :
			case EResourceState::TileShader :
			case EResourceState::FragmentShader :
			case EResourceState::PostRasterizationShaders :
			case EResourceState::ComputeShader :
			case EResourceState::RayTracingShaders :
			case EResourceState::AllGraphicsShaders :
			case EResourceState::AllShaders :			// to shutup warnings
			case EResourceState::_InvalidState :
				DBG_WARNING( "unknown resource state" );
				break;
		}
		END_ENUM_CHECKS();

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

			for (; stages != Default;)
			{
				str += " | ";
				EResourceState	stage = ExtractBit( INOUT stages );
				switch ( stage )
				{
					case EResourceState::PreRasterizationShaders :	str += "PreRasterizationShaders";	break;
					case EResourceState::TileShader :				str += "TileShader";				break;
					case EResourceState::FragmentShader :			str += "FragmentShader";			break;
					case EResourceState::PostRasterizationShaders :	str += "PostRasterizationShaders";	break;
					case EResourceState::ComputeShader :			str += "ComputeShader";				break;
					case EResourceState::RayTracingShaders :		str += "RayTracingShaders";			break;
					default :										DBG_WARNING( "unknown resource state stage" );	break;
				}
			}
		}

		return str;
	}

} // AE::Base
