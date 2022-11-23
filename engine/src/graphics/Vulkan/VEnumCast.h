// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/ResourceEnums.h"
# include "graphics/Public/RenderStateEnums.h"
# include "graphics/Public/ShaderEnums.h"
# include "graphics/Public/EResourceState.h"
# include "graphics/Public/MultiSamples.h"
# include "graphics/Public/VertexEnums.h"
# include "graphics/Public/PipelineDesc.h"
# include "graphics/Public/RayTracingEnums.h"
# include "graphics/Private/EnumUtils.h"
# include "graphics/Private/PixelFormatDefines.h"
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{
	
/*
=================================================
	VEnumCast (EBlitFilter)
=================================================
*/
	ND_ inline VkFilter  VEnumCast (EBlitFilter value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EBlitFilter::Nearest :	return VK_FILTER_NEAREST;
			case EBlitFilter::Linear :	return VK_FILTER_LINEAR;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown filter type", VK_FILTER_MAX_ENUM );
	}

/*
=================================================
	VEnumCast (MultiSamples)
=================================================
*/
	ND_ inline VkSampleCountFlagBits  VEnumCast (MultiSamples value)
	{
		return Clamp( VkSampleCountFlagBits(value.Get()), VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_64_BIT );
	}
	
/*
=================================================
	VEnumCast (ELogicOp)
=================================================
*/
	ND_ inline VkLogicOp  VEnumCast (ELogicOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case ELogicOp::Clear :			return VK_LOGIC_OP_CLEAR;
			case ELogicOp::Set :			return VK_LOGIC_OP_SET;
			case ELogicOp::Copy :			return VK_LOGIC_OP_COPY;
			case ELogicOp::CopyInverted :	return VK_LOGIC_OP_COPY_INVERTED;
			case ELogicOp::NoOp :			return VK_LOGIC_OP_NO_OP;
			case ELogicOp::Invert :			return VK_LOGIC_OP_INVERT;
			case ELogicOp::And :			return VK_LOGIC_OP_AND;
			case ELogicOp::NotAnd :			return VK_LOGIC_OP_NAND;
			case ELogicOp::Or :				return VK_LOGIC_OP_OR;
			case ELogicOp::NotOr :			return VK_LOGIC_OP_NOR;
			case ELogicOp::Xor :			return VK_LOGIC_OP_XOR;
			case ELogicOp::Equiv :			return VK_LOGIC_OP_EQUIVALENT;
			case ELogicOp::AndReverse :		return VK_LOGIC_OP_AND_REVERSE;
			case ELogicOp::AndInverted :	return VK_LOGIC_OP_AND_INVERTED;
			case ELogicOp::OrReverse :		return VK_LOGIC_OP_OR_REVERSE;
			case ELogicOp::OrInverted :		return VK_LOGIC_OP_OR_INVERTED;
			case ELogicOp::None :
			case ELogicOp::_Count :
			case ELogicOp::Unknown :		break;	// not supported
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid logical op", VK_LOGIC_OP_MAX_ENUM );
	}
	
/*
=================================================
	VEnumCast (EBlendFactor)
=================================================
*/
	ND_ inline VkBlendFactor  VEnumCast (EBlendFactor value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EBlendFactor::Zero :				return VK_BLEND_FACTOR_ZERO;
			case EBlendFactor::One :				return VK_BLEND_FACTOR_ONE;
			case EBlendFactor::SrcColor :			return VK_BLEND_FACTOR_SRC_COLOR;
			case EBlendFactor::OneMinusSrcColor :	return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			case EBlendFactor::DstColor :			return VK_BLEND_FACTOR_DST_COLOR;
			case EBlendFactor::OneMinusDstColor :	return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
			case EBlendFactor::SrcAlpha :			return VK_BLEND_FACTOR_SRC_ALPHA;
			case EBlendFactor::OneMinusSrcAlpha :	return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			case EBlendFactor::DstAlpha :			return VK_BLEND_FACTOR_DST_ALPHA;
			case EBlendFactor::OneMinusDstAlpha :	return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
			case EBlendFactor::ConstColor :			return VK_BLEND_FACTOR_CONSTANT_COLOR;
			case EBlendFactor::OneMinusConstColor :	return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
			case EBlendFactor::ConstAlpha :			return VK_BLEND_FACTOR_CONSTANT_ALPHA;
			case EBlendFactor::OneMinusConstAlpha :	return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
			case EBlendFactor::SrcAlphaSaturate :	return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
			case EBlendFactor::Src1Color :			return VK_BLEND_FACTOR_SRC1_COLOR;
			case EBlendFactor::OneMinusSrc1Color :	return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
			case EBlendFactor::Src1Alpha :			return VK_BLEND_FACTOR_SRC1_ALPHA;
			case EBlendFactor::OneMinusSrc1Alpha :	return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
			case EBlendFactor::_Count :
			case EBlendFactor::Unknown :			break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid blend func", VK_BLEND_FACTOR_MAX_ENUM );
	}
	
/*
=================================================
	VEnumCast (EBlendOp)
=================================================
*/
	ND_ inline VkBlendOp  VEnumCast (EBlendOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EBlendOp::Add :	return VK_BLEND_OP_ADD;
			case EBlendOp::Sub :	return VK_BLEND_OP_SUBTRACT;
			case EBlendOp::RevSub : return VK_BLEND_OP_REVERSE_SUBTRACT;
			case EBlendOp::Min :	return VK_BLEND_OP_MIN;
			case EBlendOp::Max :	return VK_BLEND_OP_MAX;
			case EBlendOp::_Count :
			case EBlendOp::Unknown:	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid blend equation", VK_BLEND_OP_MAX_ENUM );
	}

/*
=================================================
	VEnumCast (EVertexType)
=================================================
*/
	ND_ inline VkFormat  VEnumCast (EVertexType value)
	{
	#define FMT_BUILDER( _engineFmt_, _vkFormat_ ) \
			case EVertexType::_engineFmt_ : return _vkFormat_;

		switch ( value )
		{
			AE_PRIVATE_VKVERTEXFORMATS( FMT_BUILDER )
		}
		RETURN_ERR( "unknown vertex type!", VK_FORMAT_MAX_ENUM );
	#undef FMT_BUILDER
	}
	
/*
=================================================
	VEnumCast (EVertexInputRate)
=================================================
*/
	ND_ inline VkVertexInputRate  VEnumCast (EVertexInputRate value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EVertexInputRate::Vertex :		return VK_VERTEX_INPUT_RATE_VERTEX;
			case EVertexInputRate::Instance :	return VK_VERTEX_INPUT_RATE_INSTANCE;
			case EVertexInputRate::Unknown :
			case EVertexInputRate::_Count :		break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown vertex input rate", VK_VERTEX_INPUT_RATE_MAX_ENUM );
	}
	
/*
=================================================
	VEnumCast (EShader)
=================================================
*/
	ND_ inline VkShaderStageFlagBits  VEnumCast (EShader value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EShader::Vertex :			return VK_SHADER_STAGE_VERTEX_BIT;
			case EShader::TessControl :		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			case EShader::TessEvaluation :	return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			case EShader::Geometry :		return VK_SHADER_STAGE_GEOMETRY_BIT;
			case EShader::Fragment :		return VK_SHADER_STAGE_FRAGMENT_BIT;
			case EShader::Compute :			return VK_SHADER_STAGE_COMPUTE_BIT;
			case EShader::MeshTask :		return VK_SHADER_STAGE_TASK_BIT_EXT;
			case EShader::Mesh :			return VK_SHADER_STAGE_MESH_BIT_EXT;
			case EShader::RayGen :			return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
			case EShader::RayAnyHit :		return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
			case EShader::RayClosestHit :	return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			case EShader::RayMiss :			return VK_SHADER_STAGE_MISS_BIT_KHR;
			case EShader::RayIntersection :	return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
			case EShader::RayCallable :		return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
			case EShader::Tile :			return VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI;
			case EShader::Unknown :
			case EShader::_Count :			break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown shader type!", VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM );
	}

/*
=================================================
	VEnumCast (EShaderStages)
=================================================
*/
	ND_ inline VkShaderStageFlagBits  VEnumCast (EShaderStages values)
	{
		if ( values == EShaderStages::AllGraphics )
			return VK_SHADER_STAGE_ALL_GRAPHICS;

		if ( values == EShaderStages::All )
			return VK_SHADER_STAGE_ALL;

		VkShaderStageFlagBits	flags = Zero;
		while ( values != Zero )
		{
			EShaderStages	t = ExtractBit( INOUT values );

			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case EShaderStages::Vertex :			flags |= VK_SHADER_STAGE_VERTEX_BIT;					break;
				case EShaderStages::TessControl :		flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;		break;
				case EShaderStages::TessEvaluation :	flags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;	break;
				case EShaderStages::Geometry :			flags |= VK_SHADER_STAGE_GEOMETRY_BIT;					break;
				case EShaderStages::Fragment :			flags |= VK_SHADER_STAGE_FRAGMENT_BIT;					break;
				case EShaderStages::Compute :			flags |= VK_SHADER_STAGE_COMPUTE_BIT;					break;
				case EShaderStages::MeshTask :			flags |= VK_SHADER_STAGE_TASK_BIT_EXT;					break;
				case EShaderStages::Mesh :				flags |= VK_SHADER_STAGE_MESH_BIT_EXT;					break;
				case EShaderStages::RayGen :			flags |= VK_SHADER_STAGE_RAYGEN_BIT_KHR;				break;
				case EShaderStages::RayAnyHit :			flags |= VK_SHADER_STAGE_ANY_HIT_BIT_KHR;				break;
				case EShaderStages::RayClosestHit :		flags |= VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;			break;
				case EShaderStages::RayMiss :			flags |= VK_SHADER_STAGE_MISS_BIT_KHR;					break;
				case EShaderStages::RayIntersection :	flags |= VK_SHADER_STAGE_INTERSECTION_BIT_KHR;			break;
				case EShaderStages::RayCallable :		flags |= VK_SHADER_STAGE_CALLABLE_BIT_KHR;				break;
				case EShaderStages::Tile :				flags |= VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI;	break;
				case EShaderStages::_Last :
				case EShaderStages::Unknown :
				case EShaderStages::GraphicsStages :
				case EShaderStages::MeshStages :
				case EShaderStages::AllGraphics :
				case EShaderStages::AllRayTracing :
				case EShaderStages::All :				// to shutup warnings	
				default :								RETURN_ERR( "unknown shader type!", Zero );
			}
			END_ENUM_CHECKS();
		}
		ASSERT( flags != Zero );
		return flags;
	}

/*
=================================================
	VEnumCast (EPipelineDynamicState)
=================================================
*/
	ND_ inline VkDynamicState  VEnumCast (EPipelineDynamicState value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EPipelineDynamicState::StencilCompareMask:	return VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
			case EPipelineDynamicState::StencilWriteMask :	return VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
			case EPipelineDynamicState::StencilReference :	return VK_DYNAMIC_STATE_STENCIL_REFERENCE;
			case EPipelineDynamicState::DepthBias:			return VK_DYNAMIC_STATE_DEPTH_BIAS;
			case EPipelineDynamicState::BlendConstants:		return VK_DYNAMIC_STATE_BLEND_CONSTANTS;
			//case EPipelineDynamicState::DepthBounds:		return VK_DYNAMIC_STATE_DEPTH_BOUNDS;

			case EPipelineDynamicState::RTStackSize :		return VK_DYNAMIC_STATE_RAY_TRACING_PIPELINE_STACK_SIZE_KHR;

			// TODO:
			//	VK_DYNAMIC_STATE_SAMPLE_LOCATIONS_EXT
			//	VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR

			case EPipelineDynamicState::GraphicsPipelineMask :
			case EPipelineDynamicState::Unknown :
			case EPipelineDynamicState::All :
			case EPipelineDynamicState::_Last :				break;	// to shutup warnings
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown dynamic state type!", VK_DYNAMIC_STATE_MAX_ENUM );
	}

/*
=================================================
	VEnumCast (EAttachmentLoadOp)
=================================================
*/
	ND_ inline VkAttachmentLoadOp  VEnumCast (EAttachmentLoadOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EAttachmentLoadOp::Invalidate :	return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			case EAttachmentLoadOp::Load :			return VK_ATTACHMENT_LOAD_OP_LOAD;
			case EAttachmentLoadOp::Clear :			return VK_ATTACHMENT_LOAD_OP_CLEAR;
			case EAttachmentLoadOp::None :			return VK_ATTACHMENT_LOAD_OP_NONE_EXT;	// emulated if not supported
			case EAttachmentLoadOp::_Count :
			case EAttachmentLoadOp::Unknown :		break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid load op type", VK_ATTACHMENT_LOAD_OP_MAX_ENUM );
	}
	
/*
=================================================
	VEnumCast (EAttachmentStoreOp)
=================================================
*/
	ND_ inline VkAttachmentStoreOp  VEnumCast (EAttachmentStoreOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EAttachmentStoreOp::Invalidate :	return VK_ATTACHMENT_STORE_OP_DONT_CARE;
			case EAttachmentStoreOp::StoreCustomSamplePositions :
			case EAttachmentStoreOp::Store :		return VK_ATTACHMENT_STORE_OP_STORE;
			case EAttachmentStoreOp::None :			return VK_ATTACHMENT_STORE_OP_NONE_EXT;	// emulated if not supported
			case EAttachmentStoreOp::_Count :
			case EAttachmentStoreOp::Unknown :		break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid store op type", VK_ATTACHMENT_STORE_OP_MAX_ENUM );
	}

/*
=================================================
	VEnumCast (ECompareOp)
=================================================
*/
	ND_ inline VkCompareOp  VEnumCast (ECompareOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case ECompareOp::Never :	return VK_COMPARE_OP_NEVER;
			case ECompareOp::Less :		return VK_COMPARE_OP_LESS;
			case ECompareOp::Equal :	return VK_COMPARE_OP_EQUAL;
			case ECompareOp::LEqual :	return VK_COMPARE_OP_LESS_OR_EQUAL;
			case ECompareOp::Greater :	return VK_COMPARE_OP_GREATER;
			case ECompareOp::NotEqual :	return VK_COMPARE_OP_NOT_EQUAL;
			case ECompareOp::GEqual :	return VK_COMPARE_OP_GREATER_OR_EQUAL;
			case ECompareOp::Always :	return VK_COMPARE_OP_ALWAYS;
			case ECompareOp::_Count :
			case ECompareOp::Unknown :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid compare op", VK_COMPARE_OP_MAX_ENUM );
	}

/*
=================================================
	VEnumCast (EStencilOp)
=================================================
*/
	ND_ inline VkStencilOp  VEnumCast (EStencilOp value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EStencilOp::Keep :		return VK_STENCIL_OP_KEEP;
			case EStencilOp::Zero :		return VK_STENCIL_OP_ZERO;
			case EStencilOp::Replace :	return VK_STENCIL_OP_REPLACE;
			case EStencilOp::Incr :		return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
			case EStencilOp::IncrWrap :	return VK_STENCIL_OP_INCREMENT_AND_WRAP;
			case EStencilOp::Decr :		return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
			case EStencilOp::DecrWrap :	return VK_STENCIL_OP_DECREMENT_AND_WRAP;
			case EStencilOp::Invert :	return VK_STENCIL_OP_INVERT;
			case EStencilOp::_Count :
			case EStencilOp::Unknown :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid stencil op", VK_STENCIL_OP_MAX_ENUM );
	}

/*
=================================================
	VEnumCast (EPolygonMode)
=================================================
*/
	ND_ inline VkPolygonMode  VEnumCast (EPolygonMode value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EPolygonMode::Point :		return VK_POLYGON_MODE_POINT;
			case EPolygonMode::Line :		return VK_POLYGON_MODE_LINE;
			case EPolygonMode::Fill :		return VK_POLYGON_MODE_FILL;
			case EPolygonMode::_Count :
			case EPolygonMode::Unknown :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid polygon mode", VK_POLYGON_MODE_MAX_ENUM );
	}

/*
=================================================
	VEnumCast (ECullMode)
=================================================
*/
	ND_ inline VkCullModeFlagBits  VEnumCast (ECullMode value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case ECullMode::None :			return VK_CULL_MODE_NONE;
			case ECullMode::Front :			return VK_CULL_MODE_FRONT_BIT;
			case ECullMode::Back :			return VK_CULL_MODE_BACK_BIT;
			case ECullMode::FontAndBack :	return VK_CULL_MODE_FRONT_AND_BACK;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown cull mode", VK_CULL_MODE_NONE );
	}
	
/*
=================================================
	VEnumCast (EImageOpt)
=================================================
*/
	ND_ inline VkImageCreateFlagBits  VEnumCast (EImageOpt values)
	{
		VkImageCreateFlagBits	flags = Zero;

		while ( values != Zero )
		{
			EImageOpt	t = ExtractBit( INOUT values );
		
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case EImageOpt::CubeCompatible :				flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;				break;
				case EImageOpt::MutableFormat :					flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;				break;
				case EImageOpt::Array2DCompatible :				flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;			break;
				case EImageOpt::BlockTexelViewCompatible:		flags |= VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT;	break;
					
				case EImageOpt::SparseResidency :				flags |= VK_IMAGE_CREATE_SPARSE_BINDING_BIT | VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT;	break;
				case EImageOpt::SparseAliased :					flags |= VK_IMAGE_CREATE_SPARSE_BINDING_BIT | VK_IMAGE_CREATE_SPARSE_ALIASED_BIT;	break;
				case EImageOpt::Alias :							flags |= VK_IMAGE_CREATE_ALIAS_BIT;													break;
				case EImageOpt::SampleLocationsCompatible :		flags |= VK_IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT;					break;
				case EImageOpt::Subsampled :					flags |= VK_IMAGE_CREATE_SUBSAMPLED_BIT_EXT;										break;
					
				case EImageOpt::StorageAtomic :
				case EImageOpt::ColorAttachmentBlend :
				case EImageOpt::SampledLinear :
				case EImageOpt::SampledMinMax :
				case EImageOpt::VertexPplnStore :
				case EImageOpt::FragmentPplnStore :				break;	// options used only to check supported usage

				case EImageOpt::LosslessRTCompression :			RETURN_ERR( "unsupported image flag", Zero );

				case EImageOpt::_Last :
				case EImageOpt::All :
				case EImageOpt::Unknown :
				case EImageOpt::SparseResidencyAliased :
				default :										RETURN_ERR( "unknown image flag", Zero );
			}
			END_ENUM_CHECKS();
		}
		return flags;
	}
	
/*
=================================================
	VEnumCast (EImageDim)
=================================================
*/
	ND_ inline VkImageType  VEnumCast (EImageDim value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EImageDim_1D :			return VK_IMAGE_TYPE_1D;
			case EImageDim_2D :			return VK_IMAGE_TYPE_2D;
			case EImageDim_3D :			return VK_IMAGE_TYPE_3D;
			case EImageDim::Unknown :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unsupported image type", VK_IMAGE_TYPE_MAX_ENUM );
	}

/*
=================================================
	VEnumCast (EImage)
=================================================
*/
	ND_ inline VkImageViewType  VEnumCast (EImage value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EImage_1D :			return VK_IMAGE_VIEW_TYPE_1D;
			case EImage_1DArray :		return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
			case EImage_2D :			return VK_IMAGE_VIEW_TYPE_2D;
			case EImage_2DArray :		return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			case EImage_Cube :			return VK_IMAGE_VIEW_TYPE_CUBE;
			case EImage_CubeArray :		return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
			case EImage_3D :			return VK_IMAGE_VIEW_TYPE_3D;
			case EImage::Unknown :		break;	// not supported
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unsupported image view type", VK_IMAGE_VIEW_TYPE_MAX_ENUM );
	}

/*
=================================================
	VEnumCast (EImageUsage)
=================================================
*/
	ND_ inline VkImageUsageFlagBits  VEnumCast (EImageUsage usage, EMemoryType memType)
	{
		VkImageUsageFlagBits	flags = Zero;

		while ( usage != Zero )
		{
			EImageUsage	t = ExtractBit( INOUT usage );
			
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case EImageUsage::TransferSrc :				flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;				break;
				case EImageUsage::TransferDst :				flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;				break;
				case EImageUsage::Sampled :					flags |= VK_IMAGE_USAGE_SAMPLED_BIT;					break;
				case EImageUsage::Storage :					flags |= VK_IMAGE_USAGE_STORAGE_BIT;					break;
				case EImageUsage::ColorAttachment :			flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;			break;
				case EImageUsage::DepthStencilAttachment :	flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;	break;
				case EImageUsage::InputAttachment :			flags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;			break;
				case EImageUsage::ShadingRate :				flags |= VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;	break;
				case EImageUsage::FragmentDensityMap :		flags |= VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT;	break;
				case EImageUsage::_Last :
				case EImageUsage::Unknown :
				case EImageUsage::Transfer :
				case EImageUsage::RWAttachment :
				case EImageUsage::All :						// to shutup warnings
				default :									RETURN_ERR( "invalid image usage type", Zero );
			}
			END_ENUM_CHECKS();
		}

		if ( AllBits( memType, EMemoryType::Transient ))
			flags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;

		ASSERT( flags != Zero );
		return flags;
	}

/*
=================================================
	VEnumCast (EImageAspect)
=================================================
*/
	ND_ inline VkImageAspectFlagBits  VEnumCast (EImageAspect values)
	{
		VkImageAspectFlagBits	flags = Zero;
		
		while ( values != Zero )
		{
			EImageAspect	t = ExtractBit( INOUT values );
			
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case EImageAspect::Color :		flags |= VK_IMAGE_ASPECT_COLOR_BIT;		break;
				case EImageAspect::Depth :		flags |= VK_IMAGE_ASPECT_DEPTH_BIT;		break;
				case EImageAspect::Stencil :	flags |= VK_IMAGE_ASPECT_STENCIL_BIT;	break;
				case EImageAspect::Metadata :	flags |= VK_IMAGE_ASPECT_METADATA_BIT;	break;
				case EImageAspect::_Last :
				case EImageAspect::DepthStencil :
				case EImageAspect::Unknown :	// to shutup warnings
				default :						RETURN_ERR( "invalid image aspect type", Zero );
			}
			END_ENUM_CHECKS();
		}
		ASSERT( flags != Zero );
		return flags;
	}
	
/*
=================================================
	VEnumCast (EBufferUsage)
=================================================
*/
	ND_ inline VkBufferUsageFlagBits  VEnumCast (EBufferUsage values)
	{
		VkBufferUsageFlagBits	result = Zero;
		
		while ( values != Zero )
		{
			EBufferUsage	t = ExtractBit( INOUT values );
			
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case EBufferUsage::TransferSrc :		result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;					break;
				case EBufferUsage::TransferDst :		result |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;					break;
				case EBufferUsage::UniformTexel :		result |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;			break;
				case EBufferUsage::StorageTexel :		result |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;			break;
				case EBufferUsage::Uniform :			result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;				break;
				case EBufferUsage::Storage :			result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;				break;
				case EBufferUsage::Index :				result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;					break;
				case EBufferUsage::Vertex :				result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;				break;
				case EBufferUsage::Indirect :			result |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;				break;
				case EBufferUsage::ShaderAddress :		result |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;		break;
				case EBufferUsage::ShaderBindingTable :	result |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;							break;
				case EBufferUsage::ASBuild_ReadOnly :	result |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;	break;
				case EBufferUsage::ASBuild_Scratch :	result |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;									break;
				case EBufferUsage::_Last :
				case EBufferUsage::Transfer :
				case EBufferUsage::Unknown :
				case EBufferUsage::All :				// to shutup warnings
				default :								RETURN_ERR( "invalid buffer usage", Zero );
			}
			END_ENUM_CHECKS();
		}
		return result;
	}

/*
=================================================
	VEnumCast (EIndex)
=================================================
*/
	ND_ inline VkIndexType  VEnumCast (EIndex value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EIndex::UShort :	return VK_INDEX_TYPE_UINT16;
			case EIndex::UInt :		return VK_INDEX_TYPE_UINT32;
			case EIndex::Unknown :
			case EIndex::_Count :	break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid index type", VK_INDEX_TYPE_MAX_ENUM );
	}

/*
=================================================
	VEnumCast (EPrimitive)
=================================================
*/
	ND_ inline VkPrimitiveTopology  VEnumCast (EPrimitive value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EPrimitive::Point					: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			case EPrimitive::LineList				: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			case EPrimitive::LineStrip				: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			case EPrimitive::LineListAdjacency		: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
			case EPrimitive::LineStripAdjacency		: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
			case EPrimitive::TriangleList			: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			case EPrimitive::TriangleStrip			: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			case EPrimitive::TriangleFan			: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
			case EPrimitive::TriangleListAdjacency	: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
			case EPrimitive::TriangleStripAdjacency	: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
			case EPrimitive::Patch					: return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
			case EPrimitive::Unknown				:
			case EPrimitive::_Count					: break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "invalid primitive type", VK_PRIMITIVE_TOPOLOGY_MAX_ENUM );
	}
	
/*
=================================================
	EResourceState_ToShaderStages
=================================================
*/
	ND_ inline VkShaderStageFlags  EResourceState_ToShaderStages (EResourceState value)
	{
		ASSERT( AnyBits( value, EResourceState::AllShaders ));
		
		constexpr auto PreRasterizationShaders =
			VK_SHADER_STAGE_VERTEX_BIT |
			VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT |
			VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT |
			VK_SHADER_STAGE_GEOMETRY_BIT |
			VK_SHADER_STAGE_TASK_BIT_EXT |
			VK_SHADER_STAGE_MESH_BIT_EXT;

		constexpr auto RayTracingShaders =
			VK_SHADER_STAGE_RAYGEN_BIT_KHR |
			VK_SHADER_STAGE_ANY_HIT_BIT_KHR |
			VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR |
			VK_SHADER_STAGE_MISS_BIT_KHR |
			VK_SHADER_STAGE_INTERSECTION_BIT_KHR |
			VK_SHADER_STAGE_CALLABLE_BIT_KHR;

		VkShaderStageFlags	result = Zero;
		if ( AllBits( value, EResourceState::PreRasterizationShaders ))	result |= PreRasterizationShaders;
		if ( AllBits( value, EResourceState::TileShader ))				result |= VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI;
		if ( AllBits( value, EResourceState::FragmentShader ))			result |= VK_SHADER_STAGE_FRAGMENT_BIT;
		if ( AllBits( value, EResourceState::ComputeShader ))			result |= VK_SHADER_STAGE_COMPUTE_BIT;
		if ( AnyBits( value, EResourceState::RayTracingShaders ))		result |= RayTracingShaders;
		return result;
	}

/*
=================================================
	EResourceState_ToStageAccessLayout
=================================================
*/
	inline bool  EResourceState_ToDstStageAccessLayout (EResourceState value, OUT VkPipelineStageFlagBits2 &outStage, OUT VkAccessFlagBits2 &outAccess, OUT VkImageLayout &outLayout)
	{
		outLayout = VK_IMAGE_LAYOUT_MAX_ENUM;

		constexpr auto PreRasterizationShaders =
			VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
			VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT |
			VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT |
			VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT |
			VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT |
			VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT;
		
		DBG_CHECK_MSG( EResourceState_RequireShaderStage( value ) == AnyBits( value, EResourceState::AllShaders ),
					"shader stage is not compatible with access mask" );

		VkPipelineStageFlagBits2	sh_stages	= Zero;
		VkPipelineStageFlagBits2	ds_stages	= Zero;

		sh_stages |= AnyBits( value, EResourceState::PreRasterizationShaders )	? PreRasterizationShaders						: 0;
		sh_stages |= AnyBits( value, EResourceState::TileShader )				? VK_PIPELINE_STAGE_2_SUBPASS_SHADING_BIT_HUAWEI: 0;
		sh_stages |= AnyBits( value, EResourceState::FragmentShader )			? VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT		: 0;
		// ignore PostRasterizationShaders
		sh_stages |= AnyBits( value, EResourceState::ComputeShader )			? VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT		: 0;
		sh_stages |= AnyBits( value, EResourceState::RayTracingShaders )		? VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR: 0;
		
		ds_stages |= AnyBits( value, EResourceState::DSTestBeforeFS )			? VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT	: 0;
		ds_stages |= AnyBits( value, EResourceState::DSTestAfterFS )			? VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT	: 0;


		BEGIN_ENUM_CHECKS();
		switch ( value & EResourceState::_AccessMask )
		{
			case EResourceState::Unknown :							outStage = VK_PIPELINE_STAGE_2_NONE;									outAccess = VK_ACCESS_2_NONE;																														outLayout = VK_IMAGE_LAYOUT_UNDEFINED;															break;
			case EResourceState::ShaderUniform :					outStage = sh_stages;													outAccess = VK_ACCESS_2_UNIFORM_READ_BIT;																																													ASSERT( sh_stages );	break;
			case EResourceState::ShaderSample :						outStage = sh_stages;													outAccess = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;																									outLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;					ASSERT( sh_stages );	break;
			case EResourceState::ShaderStorage_Read :				outStage = sh_stages;													outAccess = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;																									outLayout = VK_IMAGE_LAYOUT_GENERAL; 									ASSERT( sh_stages );	break;
			case EResourceState::ShaderStorage_Write :				outStage = sh_stages;													outAccess = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;																									outLayout = VK_IMAGE_LAYOUT_GENERAL; 									ASSERT( sh_stages );	break;
			case EResourceState::ShaderStorage_RW :					outStage = sh_stages;													outAccess = VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;																outLayout = VK_IMAGE_LAYOUT_GENERAL; 									ASSERT( sh_stages );	break;

			case EResourceState::CopySrc :							outStage = VK_PIPELINE_STAGE_2_COPY_BIT;								outAccess =	VK_ACCESS_2_TRANSFER_READ_BIT;																											outLayout =	VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;												break;
			case EResourceState::CopyDst :							outStage = VK_PIPELINE_STAGE_2_COPY_BIT;								outAccess =	VK_ACCESS_2_TRANSFER_WRITE_BIT;																											outLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;												break;
			case EResourceState::ClearDst :							outStage = VK_PIPELINE_STAGE_2_CLEAR_BIT;								outAccess =	VK_ACCESS_2_TRANSFER_WRITE_BIT;																											outLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;												break;

			case EResourceState::BlitSrc :							outStage = VK_PIPELINE_STAGE_2_BLIT_BIT;								outAccess =	VK_ACCESS_2_TRANSFER_READ_BIT;																											outLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;												break;
			case EResourceState::BlitDst :							outStage = VK_PIPELINE_STAGE_2_BLIT_BIT;								outAccess =	VK_ACCESS_2_TRANSFER_WRITE_BIT;																											outLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;												break;

			case EResourceState::InputColorAttachment :				outStage = sh_stages;													outAccess = VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT;																									outLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;					ASSERT( sh_stages );	break;
			case EResourceState::InputColorAttachment_RW :			outStage = sh_stages | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;	outAccess = VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;					outLayout = VK_IMAGE_LAYOUT_GENERAL;									ASSERT( sh_stages );	break;
			
			case EResourceState::ColorAttachment_Write :			outStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;				outAccess = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;																									outLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;											break;
			case EResourceState::ColorAttachment_RW :				outStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;				outAccess = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;															outLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;											break;

			case EResourceState::DepthStencilAttachment_Read :		outStage = ds_stages;													outAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;																							outLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;			ASSERT( ds_stages );	break;
			case EResourceState::DepthStencilAttachment_Write:		outStage = ds_stages;													outAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;																							outLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;			ASSERT( ds_stages );	break;
			case EResourceState::DepthStencilAttachment_RW :		outStage = ds_stages;													outAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;	 										outLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;			ASSERT( ds_stages );	break;
			case EResourceState::DepthTest_StencilRW :				outStage = ds_stages;													outAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;											outLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;	ASSERT( ds_stages );	break;
			case EResourceState::DepthRW_StencilTest :				outStage = ds_stages;													outAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;								 			outLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;	ASSERT( ds_stages );	break;
			
			case EResourceState::InputDepthStencilAttachment :		outStage = sh_stages;													outAccess = VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT;																									outLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;					ASSERT( sh_stages );	break;
			case EResourceState::InputDepthStencilAttachment_RW :	outStage = ds_stages | sh_stages;										outAccess = VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;	outLayout = VK_IMAGE_LAYOUT_GENERAL;									ASSERT( ds_stages and sh_stages );	break;
			
			case EResourceState::DepthStencilTest_ShaderSample :	outStage = ds_stages | sh_stages;										outAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;													outLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;			ASSERT( ds_stages and sh_stages );	break;
			case EResourceState::DepthTest_DepthSample_StencilRW :	outStage = ds_stages | sh_stages;										outAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;	outLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL; ASSERT( ds_stages and sh_stages );	break;

			case EResourceState::Host_Read :						outStage = VK_PIPELINE_STAGE_2_HOST_BIT;								outAccess = VK_ACCESS_2_HOST_READ_BIT; 																												outLayout = VK_IMAGE_LAYOUT_GENERAL;															break;
			case EResourceState::Host_Write :						outStage = VK_PIPELINE_STAGE_2_HOST_BIT;								outAccess = VK_ACCESS_2_HOST_WRITE_BIT; 																											outLayout = VK_IMAGE_LAYOUT_GENERAL;															break;
			case EResourceState::Host_RW :							outStage = VK_PIPELINE_STAGE_2_HOST_BIT;								outAccess = VK_ACCESS_2_HOST_READ_BIT | VK_ACCESS_2_HOST_WRITE_BIT; 																				outLayout = VK_IMAGE_LAYOUT_GENERAL;															break;

			// swapchain semaphore creates memory dependency
			case EResourceState::PresentImage :						outStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;				outAccess = VK_ACCESS_2_NONE;																														outLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;													break;

			case EResourceState::IndirectBuffer :					outStage = VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;						outAccess = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;																																																	break;
			case EResourceState::IndexBuffer :						outStage = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT;							outAccess = VK_ACCESS_2_INDEX_READ_BIT;																																																				break;
			case EResourceState::VertexBuffer :						outStage = VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT;				outAccess = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;																																																	break;
			case EResourceState::ShadingRateImage :					outStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;outAccess = VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;																				outLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;						break;
			case EResourceState::FragmentDensityMap :				outStage = VK_PIPELINE_STAGE_2_FRAGMENT_DENSITY_PROCESS_BIT_EXT;		outAccess = VK_ACCESS_2_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;																							outLayout = VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT;									break;

			case EResourceState::CopyRTAS_Read :					outStage = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_COPY_BIT_KHR;		outAccess = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_2_TRANSFER_READ_BIT;																																						break;
			case EResourceState::CopyRTAS_Write :					outStage = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_COPY_BIT_KHR;		outAccess = VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VK_ACCESS_2_TRANSFER_WRITE_BIT;																																						break;
			case EResourceState::BuildRTAS_Read :					outStage = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;	outAccess = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR; 																																														break;
			case EResourceState::BuildRTAS_Write :					outStage = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;	outAccess = VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR; 																																														break;
			case EResourceState::BuildRTAS_RW :						outStage = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;	outAccess = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR; 																																	break;
			case EResourceState::BuildRTAS_ScratchBuffer :			outStage = VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;	outAccess = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR; 																																														break;
				
			case EResourceState::ShaderRTAS_Read :					outStage = sh_stages;													outAccess = VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;																																								ASSERT( sh_stages );	break;
			case EResourceState::RTShaderBindingTable :				outStage = VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;				outAccess = VK_ACCESS_2_SHADER_BINDING_TABLE_READ_BIT_KHR;																																															break;

			case EResourceState::Preserve :
			case EResourceState::_AccessMask :
			case EResourceState::DSTestBeforeFS :
			case EResourceState::DSTestAfterFS :
			case EResourceState::Invalidate :
			case EResourceState::_FlagsMask :
			case EResourceState::PreRasterizationShaders :
			case EResourceState::TileShader :
			case EResourceState::FragmentShader :
			case EResourceState::PostRasterizationShaders :
			case EResourceState::ComputeShader :
			case EResourceState::RayTracingShaders :
			case EResourceState::AllGraphicsShaders :
			case EResourceState::AllShaders :			// to shutup warnings
			case EResourceState::_InvalidState :
			default :
				outStage	= 0;
				outAccess	= 0;
				RETURN_ERR( "unknown resource state!" );
		}
		END_ENUM_CHECKS();

		return true;
	}
	
	inline bool  EResourceState_ToSrcStageAccessLayout (EResourceState value, OUT VkPipelineStageFlagBits2 &outStage, OUT VkAccessFlagBits2 &outAccess, OUT VkImageLayout &outLayout)
	{
		bool	res = EResourceState_ToDstStageAccessLayout( value, outStage, outAccess, outLayout );
		outLayout = AnyBits( value, EResourceState::Invalidate ) ? VK_IMAGE_LAYOUT_UNDEFINED : outLayout;
		return res;
	}

	inline bool  EResourceState_ToStageAccess (EResourceState value, OUT VkPipelineStageFlagBits2 &outStage, OUT VkAccessFlagBits2 &outAccess)
	{
		VkImageLayout				layout;
		return EResourceState_ToDstStageAccessLayout( value, OUT outStage, OUT outAccess, OUT layout );
	}

	ND_ inline VkPipelineStageFlagBits2  EResourceState_ToPipelineStages (EResourceState value)
	{
		VkPipelineStageFlagBits2	stage;
		VkAccessFlagBits2			access;
		VkImageLayout				layout;
		EResourceState_ToDstStageAccessLayout( value, OUT stage, OUT access, OUT layout );
		return stage;
	}

	ND_ inline VkAccessFlagBits2  EResourceState_ToAccessMask (EResourceState value)
	{
		VkPipelineStageFlagBits2	stage;
		VkAccessFlagBits2			access;
		VkImageLayout				layout;
		EResourceState_ToDstStageAccessLayout( value, OUT stage, OUT access, OUT layout );
		return access;
	}
	
	ND_ inline VkImageLayout  EResourceState_ToSrcImageLayout (EResourceState value)
	{
		VkPipelineStageFlagBits2	stage;
		VkAccessFlagBits2			access;
		VkImageLayout				layout;
		EResourceState_ToSrcStageAccessLayout( value, OUT stage, OUT access, OUT layout );
		return layout;
	}

	ND_ inline VkImageLayout  EResourceState_ToDstImageLayout (EResourceState value)
	{
		VkPipelineStageFlagBits2	stage;
		VkAccessFlagBits2			access;
		VkImageLayout				layout;
		EResourceState_ToDstStageAccessLayout( value, OUT stage, OUT access, OUT layout );
		return layout;
	}
			
/*
=================================================
	VEnumCast (EPixelFormat)
=================================================
*/
	ND_ inline VkFormat  VEnumCast (EPixelFormat value)
	{
#		define FMT_BUILDER( _engineFmt_, _vkFormat_ ) \
			case EPixelFormat::_engineFmt_ : return _vkFormat_;
		
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			AE_PRIVATE_VKPIXELFORMATS( FMT_BUILDER )
			case EPixelFormat::_Count :
			case EPixelFormat::SwapchainColor :
			case EPixelFormat::Unknown :		break;
		}
		END_ENUM_CHECKS();

#		undef FMT_BUILDER

		RETURN_ERR( "invalid pixel format", VK_FORMAT_MAX_ENUM );
	}
	
/*
=================================================
	AEEnumCast (VkFormat)
=================================================
*/
	ND_ inline EPixelFormat  AEEnumCast (VkFormat value)
	{
#		define FMT_BUILDER( _engineFmt_, _vkFormat_ ) \
			case _vkFormat_ : return EPixelFormat::_engineFmt_;
		
		switch ( value )
		{
			AE_PRIVATE_VKPIXELFORMATS( FMT_BUILDER )
		}

#		undef FMT_BUILDER

		RETURN_ERR( "invalid pixel format" );
	}

/*
=================================================
	AEEnumCast (VkImageType)
=================================================
*/
	ND_ inline EImageDim  AEEnumCast (VkImageType value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case VK_IMAGE_TYPE_1D :			return EImageDim_1D;
			case VK_IMAGE_TYPE_2D :			return EImageDim_2D;
			case VK_IMAGE_TYPE_3D :			return EImageDim_3D;
			case VK_IMAGE_TYPE_MAX_ENUM :
			default :						break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown vulkan image type" );
	}
	
/*
=================================================
	AEEnumCast (VkImageUsageFlagBits)
=================================================
*/
	inline void  AEEnumCast (VkImageUsageFlagBits usage, OUT EImageUsage& outUsage, OUT EMemoryType& outMemType)
	{
		outUsage	= Default;
		outMemType	= Default;
		
		STATIC_ASSERT( uint(EImageUsage::All) == 0x1FF );
		while ( usage != Zero )
		{
			auto	t = VkImageUsageFlagBits( ExtractBit( INOUT usage ));
			
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case VK_IMAGE_USAGE_TRANSFER_SRC_BIT :				outUsage |= EImageUsage::TransferSrc;				break;
				case VK_IMAGE_USAGE_TRANSFER_DST_BIT :				outUsage |= EImageUsage::TransferDst;				break;
				case VK_IMAGE_USAGE_SAMPLED_BIT :					outUsage |= EImageUsage::Sampled;					outMemType |= EMemoryType::DeviceLocal;	break;
				case VK_IMAGE_USAGE_STORAGE_BIT :					outUsage |= EImageUsage::Storage;					outMemType |= EMemoryType::DeviceLocal;	break;
				case VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT :			outUsage |= EImageUsage::ColorAttachment;			outMemType |= EMemoryType::DeviceLocal;	break;
				case VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT :	outUsage |= EImageUsage::DepthStencilAttachment;	outMemType |= EMemoryType::DeviceLocal;	break;
				case VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT :			outUsage |= EImageUsage::InputAttachment;			outMemType |= EMemoryType::DeviceLocal;	break;
				case VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT :	outUsage |= EImageUsage::FragmentDensityMap;		break;
				case VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR: outUsage |= EImageUsage::ShadingRate;		outMemType |= EMemoryType::DeviceLocal;	break;
				case VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT :
					outUsage	|= EImageUsage::ColorAttachment | EImageUsage::DepthStencilAttachment | EImageUsage::InputAttachment;
					outMemType	|= EMemoryType::Transient;
					break;

				case VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR :
				case VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR :
				case VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR :
				case VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR :
				case VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR :
				case VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR :
				case VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI:
				case VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT :
				case VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM :
				case VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM :
				case VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM :
				default :											RETURN_ERR( "not supported", void() );
			}
			END_ENUM_CHECKS();
		}
	}
	
/*
=================================================
	AEEnumCast (VkSampleCountFlagBits)
=================================================
*/
	ND_ inline  MultiSamples  AEEnumCast (VkSampleCountFlagBits samples)
	{
		if ( samples == 0 )
			return 1_samples;

		ASSERT( IsPowerOfTwo( samples ));
		return MultiSamples{ uint(samples) };
	}
	
/*
=================================================
	AEEnumCast (VkImageCreateFlagBits)
=================================================
*/
	ND_ inline EImageOpt  AEEnumCast (VkImageCreateFlagBits values)
	{
		EImageOpt	result = Zero;
		
		STATIC_ASSERT( uint(EImageOpt::All) == 0xFFFF );
		while ( values != Zero )
		{
			VkImageCreateFlagBits	t = ExtractBit( INOUT values );
		
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT :		result |= EImageOpt::CubeCompatible;		break;
				case VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT :	result |= EImageOpt::Array2DCompatible;		break;
				case VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT :		result |= EImageOpt::MutableFormat;			break;
				case VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT :		result |= EImageOpt::SparseResidency;		break;
				case VK_IMAGE_CREATE_SPARSE_ALIASED_BIT :		result |= EImageOpt::SparseAliased;			break;
				case VK_IMAGE_CREATE_ALIAS_BIT :				result |= EImageOpt::Alias;					break;
				case VK_IMAGE_CREATE_SUBSAMPLED_BIT_EXT :		result |= EImageOpt::Subsampled;			break;
				case VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT :				result |= EImageOpt::BlockTexelViewCompatible;	break;
				case VK_IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT :	result |= EImageOpt::SampleLocationsCompatible;	break;

				case VK_IMAGE_CREATE_SPARSE_BINDING_BIT :
				case VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT :
				case VK_IMAGE_CREATE_EXTENDED_USAGE_BIT :
				case VK_IMAGE_CREATE_PROTECTED_BIT :
				case VK_IMAGE_CREATE_DISJOINT_BIT :
				case VK_IMAGE_CREATE_FLAG_BITS_MAX_ENUM :
				case VK_IMAGE_CREATE_CORNER_SAMPLED_BIT_NV :
				case VK_IMAGE_CREATE_2D_VIEW_COMPATIBLE_BIT_EXT :
				case VK_IMAGE_CREATE_FRAGMENT_DENSITY_MAP_OFFSET_BIT_QCOM :
				case VK_IMAGE_CREATE_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_BIT_EXT :

				default :
					RETURN_ERR( "unsupported image create flags" );
			}
			END_ENUM_CHECKS();
		}
		return result;
	}
	
/*
=================================================
	AEEnumCast (VkBufferUsageFlagBits)
=================================================
*/
	ND_ inline EBufferUsage  AEEnumCast (VkBufferUsageFlagBits values)
	{
		EBufferUsage	result = Default;
		
		STATIC_ASSERT( uint(EBufferUsage::All) == 0x1FFF );
		while ( values != Zero )
		{
			VkBufferUsageFlagBits	t = ExtractBit( INOUT values );
			
			BEGIN_ENUM_CHECKS();
			switch ( VkBufferUsageFlagBits(t) )
			{
				case VK_BUFFER_USAGE_TRANSFER_SRC_BIT :				result |= EBufferUsage::TransferSrc;	break;
				case VK_BUFFER_USAGE_TRANSFER_DST_BIT :				result |= EBufferUsage::TransferDst;	break;
				case VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT :		result |= EBufferUsage::UniformTexel;	break;
				case VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT :		result |= EBufferUsage::StorageTexel;	break;
				case VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT :			result |= EBufferUsage::Uniform;		break;
				case VK_BUFFER_USAGE_STORAGE_BUFFER_BIT :			result |= EBufferUsage::Storage;		break;
				case VK_BUFFER_USAGE_INDEX_BUFFER_BIT :				result |= EBufferUsage::Index;			break;
				case VK_BUFFER_USAGE_VERTEX_BUFFER_BIT :			result |= EBufferUsage::Vertex;			break;
				case VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT :			result |= EBufferUsage::Indirect;		break;
				case VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT:		result |= EBufferUsage::ShaderAddress | EBufferUsage::ASBuild_Scratch;	break;
				case VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR :	result |= EBufferUsage::ShaderBindingTable;								break;
				case VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR :	result |= EBufferUsage::ASBuild_ReadOnly;		break;

				case VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR :
				case VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT :
				case VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT :
				case VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT :
				case VK_BUFFER_USAGE_VIDEO_DECODE_DST_BIT_KHR :
				case VK_BUFFER_USAGE_VIDEO_DECODE_SRC_BIT_KHR :
				case VK_BUFFER_USAGE_VIDEO_ENCODE_DST_BIT_KHR :
				case VK_BUFFER_USAGE_VIDEO_ENCODE_SRC_BIT_KHR :
				case VK_BUFFER_USAGE_MICROMAP_BUILD_INPUT_READ_ONLY_BIT_EXT :
				case VK_BUFFER_USAGE_MICROMAP_STORAGE_BIT_EXT :
				case VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM :
				default :											RETURN_ERR( "invalid buffer usage" );
			}
			END_ENUM_CHECKS();
		}
		return result;
	}
	
/*
=================================================
	VEnumCast (EMemoryType)
=================================================
*/
	ND_ inline VkMemoryPropertyFlagBits  VEnumCast (EMemoryType values)
	{
		VkMemoryPropertyFlagBits	result = Zero;
		
		while ( values != Zero )
		{
			auto	t = ExtractBit( INOUT values );
			
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case EMemoryType::DeviceLocal :		result |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;											break;
				case EMemoryType::Transient :		result |= VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;										break;
				case EMemoryType::HostCocherent :	result |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;	break;
				case EMemoryType::HostCached :		result |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;		break;

				case EMemoryType::Dedicated :
				case EMemoryType::_External :		break;
				case EMemoryType::_Last :
				case EMemoryType::HostCachedCocherent :
				case EMemoryType::Unified :
				case EMemoryType::UnifiedCached :
				case EMemoryType::Unknown :
				case EMemoryType::All :
				default :							RETURN_ERR( "unknown memory type", Zero );
			}
			END_ENUM_CHECKS();
		}
		return result;
	}

/*
=================================================
	AEEnumCast (VkMemoryPropertyFlagBits)
=================================================
*/
	ND_ inline EMemoryType  AEEnumCast (VkMemoryPropertyFlagBits values, bool isExternal)
	{
		EMemoryType	result = Default;

		while ( values != Zero )
		{
			auto	t = ExtractBit( INOUT values );

			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT :		result |= EMemoryType::DeviceLocal;		break;
				case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT :
				case VK_MEMORY_PROPERTY_HOST_COHERENT_BIT :		result |= EMemoryType::HostCocherent;	break;
				case VK_MEMORY_PROPERTY_HOST_CACHED_BIT :		result |= EMemoryType::HostCached;		break;
				case VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT :	result |= EMemoryType::Transient;		break;
				case VK_MEMORY_PROPERTY_PROTECTED_BIT :
				case VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD :
				case VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD :
				case VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV :	break;
				case VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM :
				default :										RETURN_ERR( "unknown memory property" );
			}
			END_ENUM_CHECKS();
		}

		if ( isExternal )
			result |= EMemoryType::_External;

		return result;
	}
	
/*
=================================================
	AEEnumCast (VkShaderStageFlagBits)
=================================================
*/
	ND_ inline EShaderStages  AEEnumCast (VkShaderStageFlagBits stages)
	{
		if ( stages == VK_SHADER_STAGE_ALL )
			return EShaderStages::All;

		EShaderStages	result = Default;

		while ( stages != Zero )
		{
			auto	t = ExtractBit( INOUT stages );

			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case VK_SHADER_STAGE_VERTEX_BIT :					result |= EShaderStages::Vertex;			break;
				case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT :		result |= EShaderStages::TessControl;		break;
				case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT :	result |= EShaderStages::TessEvaluation;	break;
				case VK_SHADER_STAGE_GEOMETRY_BIT :					result |= EShaderStages::Geometry;			break;
				case VK_SHADER_STAGE_FRAGMENT_BIT :					result |= EShaderStages::Fragment;			break;
				case VK_SHADER_STAGE_COMPUTE_BIT :					result |= EShaderStages::Compute;			break;
				case VK_SHADER_STAGE_RAYGEN_BIT_KHR :				result |= EShaderStages::RayGen;			break;
				case VK_SHADER_STAGE_ANY_HIT_BIT_KHR :				result |= EShaderStages::RayAnyHit;			break;
				case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR :			result |= EShaderStages::RayClosestHit;		break;
				case VK_SHADER_STAGE_MISS_BIT_KHR :					result |= EShaderStages::RayMiss;			break;
				case VK_SHADER_STAGE_INTERSECTION_BIT_KHR :			result |= EShaderStages::RayIntersection;	break;
				case VK_SHADER_STAGE_CALLABLE_BIT_KHR :				result |= EShaderStages::RayCallable;		break;
				case VK_SHADER_STAGE_TASK_BIT_EXT :					result |= EShaderStages::MeshTask;			break;
				case VK_SHADER_STAGE_MESH_BIT_EXT :					result |= EShaderStages::Mesh;				break;
				case VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI :	result |= EShaderStages::Tile;				break;
				case VK_SHADER_STAGE_ALL_GRAPHICS :
				case VK_SHADER_STAGE_ALL :
				default :											RETURN_ERR( "unknown shader stage" );		break;
			}
			END_ENUM_CHECKS();
		}
		return result;
	}
	
/*
=================================================
	VEnumCast (EPipelineOpt)
=================================================
*/
	ND_ inline VkPipelineCreateFlagBits  VEnumCast (EPipelineOpt values)
	{
		VkPipelineCreateFlagBits	result = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
		
		while ( values != Zero )
		{
			auto	t = ExtractBit( INOUT values );
			
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case EPipelineOpt::Optimize :						result &= ~VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;							break;
				case EPipelineOpt::CS_DispatchBase :				result |= VK_PIPELINE_CREATE_DISPATCH_BASE_BIT;									break;
				case EPipelineOpt::RT_NoNullAnyHitShaders :			result |= VK_PIPELINE_CREATE_RAY_TRACING_NO_NULL_ANY_HIT_SHADERS_BIT_KHR;		break;
				case EPipelineOpt::RT_NoNullClosestHitShaders :		result |= VK_PIPELINE_CREATE_RAY_TRACING_NO_NULL_CLOSEST_HIT_SHADERS_BIT_KHR;	break;
				case EPipelineOpt::RT_NoNullMissShaders :			result |= VK_PIPELINE_CREATE_RAY_TRACING_NO_NULL_MISS_SHADERS_BIT_KHR;			break;
				case EPipelineOpt::RT_NoNullIntersectionShaders :	result |= VK_PIPELINE_CREATE_RAY_TRACING_NO_NULL_INTERSECTION_SHADERS_BIT_KHR;	break;
				case EPipelineOpt::RT_SkipTriangles :				result |= VK_PIPELINE_CREATE_RAY_TRACING_SKIP_TRIANGLES_BIT_KHR;				break;
				case EPipelineOpt::RT_SkipAABBs :					result |= VK_PIPELINE_CREATE_RAY_TRACING_SKIP_AABBS_BIT_KHR;					break;
				case EPipelineOpt::_Last :
				case EPipelineOpt::All :
				case EPipelineOpt::Unknown :
				default :											RETURN_ERR( "unknown pipeline options", Zero );									break;
			}
			END_ENUM_CHECKS();
		}
		return result;
	}
	
/*
=================================================
	VEnumCast (ERTASOptions)
=================================================
*/
	ND_ inline VkBuildAccelerationStructureFlagBitsKHR  VEnumCast (ERTASOptions values)
	{
		VkBuildAccelerationStructureFlagBitsKHR		result = Zero;
		
		while ( values != Zero )
		{
			auto	t = ExtractBit( INOUT values );
			
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case ERTASOptions::AllowUpdate :		result |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;			break;
				case ERTASOptions::AllowCompaction :	result |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR;		break;
				case ERTASOptions::PreferFastTrace :	result |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;	break;
				case ERTASOptions::PreferFastBuild :	result |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;	break;
				case ERTASOptions::LowMemory :			result |= VK_BUILD_ACCELERATION_STRUCTURE_LOW_MEMORY_BIT_KHR;			break;
				case ERTASOptions::_Last :
				case ERTASOptions::Unknown :
				default :								RETURN_ERR( "unknown RTAS options", Zero );								break;
			}
			END_ENUM_CHECKS();
		}
		return result;
	}
	
/*
=================================================
	VEnumCast (ERTGeometryOpt)
=================================================
*/
	ND_ inline VkGeometryFlagBitsKHR  VEnumCast (ERTGeometryOpt values)
	{
		VkGeometryFlagBitsKHR	result = Zero;
		
		while ( values != Zero )
		{
			auto	t = ExtractBit( INOUT values );
			
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case ERTGeometryOpt::Opaque :						result |= VK_GEOMETRY_OPAQUE_BIT_KHR;							break;
				case ERTGeometryOpt::NoDuplicateAnyHitInvocation :	result |= VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;	break;
				case ERTGeometryOpt::_Last :
				case ERTGeometryOpt::Unknown :
				default :											RETURN_ERR( "unknown RT geometry options", Zero );				break;
			}
			END_ENUM_CHECKS();
		}
		return result;
	}
	
/*
=================================================
	VEnumCast (ERTInstanceOpt)
=================================================
*/
	ND_ inline VkGeometryInstanceFlagBitsKHR  VEnumCast (ERTInstanceOpt values)
	{
		VkGeometryInstanceFlagBitsKHR	result = Zero;
		
		while ( values != Zero )
		{
			auto	t = ExtractBit( INOUT values );
			
			BEGIN_ENUM_CHECKS();
			switch ( t )
			{
				case ERTInstanceOpt::TriangleCullDisable :	result |= VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;	break;
				case ERTInstanceOpt::TriangleFrontCCW :		result |= VK_GEOMETRY_INSTANCE_TRIANGLE_FLIP_FACING_BIT_KHR;			break;
				case ERTInstanceOpt::ForceOpaque :			result |= VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR;					break;
				case ERTInstanceOpt::ForceNonOpaque :		result |= VK_GEOMETRY_INSTANCE_FORCE_NO_OPAQUE_BIT_KHR;					break;
				case ERTInstanceOpt::_Last :
				case ERTInstanceOpt::Unknown :
				default :									RETURN_ERR( "unknown RT instance options", Zero );						break;
			}
			END_ENUM_CHECKS();
		}
		return result;
	}
	
/*
=================================================
	VEnumCast (ERTASCopyMode)
=================================================
*/
	ND_ inline VkCopyAccelerationStructureModeKHR  VEnumCast (ERTASCopyMode value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case ERTASCopyMode::Clone :		return VK_COPY_ACCELERATION_STRUCTURE_MODE_CLONE_KHR;
			case ERTASCopyMode::Compaction:	return VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown RT AS copy mode", VK_COPY_ACCELERATION_STRUCTURE_MODE_MAX_ENUM_KHR );
	}
	
/*
=================================================
	VEnumCast (EColorSpace)
=================================================
*/
	ND_ inline VkColorSpaceKHR  VEnumCast (EColorSpace value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EColorSpace::sRGB_nonlinear :			return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

			// requires 'swapchainColorspace' (VK_EXT_swapchain_colorspace)
			case EColorSpace::Display_P3_nonlinear :	return VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT;
			case EColorSpace::Extended_sRGB_linear :	return VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT;
			case EColorSpace::Display_P3_linear :		return VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT;
			case EColorSpace::DCI_P3_nonlinear :		return VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT;
			case EColorSpace::BT709_linear :			return VK_COLOR_SPACE_BT709_LINEAR_EXT;
			case EColorSpace::BT709_nonlinear :			return VK_COLOR_SPACE_BT709_NONLINEAR_EXT;
			case EColorSpace::BT2020_linear :			return VK_COLOR_SPACE_BT2020_LINEAR_EXT;
			case EColorSpace::HDR10_ST2084 :			return VK_COLOR_SPACE_HDR10_ST2084_EXT;
			case EColorSpace::DolbyVision :				return VK_COLOR_SPACE_DOLBYVISION_EXT;
			case EColorSpace::HDR10_HLG :				return VK_COLOR_SPACE_HDR10_HLG_EXT;
			case EColorSpace::AdobeRGB_linear :			return VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT;
			case EColorSpace::AdobeRGB_nonlinear :		return VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT;
			case EColorSpace::PassThrough :				return VK_COLOR_SPACE_PASS_THROUGH_EXT;
			case EColorSpace::Extended_sRGB_nonlinear :	return VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT;

			case EColorSpace::_Count :
			case EColorSpace::Unknown :					break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown color space", VK_COLOR_SPACE_MAX_ENUM_KHR );
	}
	
/*
=================================================
	VEnumCast (EPresentMode)
=================================================
*/
	ND_ inline VkPresentModeKHR  VEnumCast (EPresentMode value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EPresentMode::Immediate :				return VK_PRESENT_MODE_IMMEDIATE_KHR;
			case EPresentMode::Mailbox :				return VK_PRESENT_MODE_MAILBOX_KHR;
			case EPresentMode::FIFO :					return VK_PRESENT_MODE_FIFO_KHR;
			case EPresentMode::FIFO_Relaxed :			return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
			case EPresentMode::SharedDemandRefresh :	return VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR;
			case EPresentMode::SharedContinuousRefresh:	return VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR;

			case EPresentMode::_Count :
			case EPresentMode::Unknown :				break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown present mode", VK_PRESENT_MODE_MAX_ENUM_KHR );
	}
	
/*
=================================================
	VEnumCast (EQueryType)
=================================================
*/
	ND_ inline VkQueryType  VEnumCast (EQueryType value)
	{
		BEGIN_ENUM_CHECKS();
		switch ( value )
		{
			case EQueryType::Timestamp :					return VK_QUERY_TYPE_TIMESTAMP;
			case EQueryType::PipelineStatistic :			return VK_QUERY_TYPE_PIPELINE_STATISTICS;
			case EQueryType::Performance :					return VK_QUERY_TYPE_PERFORMANCE_QUERY_KHR;
			case EQueryType::AccelStructCompactedSize :		return VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR;
			case EQueryType::AccelStructSize :				return VK_QUERY_TYPE_ACCELERATION_STRUCTURE_SIZE_KHR;				// require 'VK_KHR_ray_tracing_maintenance1'
			case EQueryType::AccelStructSerializationSize :	return VK_QUERY_TYPE_ACCELERATION_STRUCTURE_SERIALIZATION_SIZE_KHR;
			// TODO: VK_QUERY_TYPE_ACCELERATION_STRUCTURE_SERIALIZATION_BOTTOM_LEVEL_POINTERS_KHR				// require 'VK_KHR_ray_tracing_maintenance1'

			case EQueryType::_Count :
			case EQueryType::Unknown :						break;
		}
		END_ENUM_CHECKS();
		RETURN_ERR( "unknown query type", VK_QUERY_TYPE_MAX_ENUM );
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
