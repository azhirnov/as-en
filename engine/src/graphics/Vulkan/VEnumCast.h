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
	ND_ inline VkFilter  VEnumCast (EBlitFilter value);

/*
=================================================
	VEnumCast (MultiSamples)
=================================================
*/
	ND_ inline VkSampleCountFlagBits  VEnumCast (MultiSamples value);

/*
=================================================
	VEnumCast (ELogicOp)
=================================================
*/
	ND_ inline VkLogicOp  VEnumCast (ELogicOp value);
	
/*
=================================================
	VEnumCast (EBlendFactor)
=================================================
*/
	ND_ inline VkBlendFactor  VEnumCast (EBlendFactor value);
	
/*
=================================================
	VEnumCast (EBlendOp)
=================================================
*/
	ND_ inline VkBlendOp  VEnumCast (EBlendOp value);

/*
=================================================
	VEnumCast (EVertexType)
=================================================
*/
	ND_ inline VkFormat  VEnumCast (EVertexType value);
	
/*
=================================================
	VEnumCast (EVertexInputRate)
=================================================
*/
	ND_ inline VkVertexInputRate  VEnumCast (EVertexInputRate value);
	
/*
=================================================
	VEnumCast (EShader)
=================================================
*/
	ND_ inline VkShaderStageFlagBits  VEnumCast (EShader value);

/*
=================================================
	VEnumCast (EShaderStages)
=================================================
*/
	ND_ inline VkShaderStageFlagBits  VEnumCast (EShaderStages values);

/*
=================================================
	VEnumCast (EPipelineDynamicState)
=================================================
*/
	ND_ inline VkDynamicState  VEnumCast (EPipelineDynamicState value);

/*
=================================================
	VEnumCast (EAttachmentLoadOp)
=================================================
*/
	ND_ inline VkAttachmentLoadOp  VEnumCast (EAttachmentLoadOp value);
	
/*
=================================================
	VEnumCast (EAttachmentStoreOp)
=================================================
*/
	ND_ inline VkAttachmentStoreOp  VEnumCast (EAttachmentStoreOp value);

/*
=================================================
	VEnumCast (ECompareOp)
=================================================
*/
	ND_ inline VkCompareOp  VEnumCast (ECompareOp value);

/*
=================================================
	VEnumCast (EStencilOp)
=================================================
*/
	ND_ inline VkStencilOp  VEnumCast (EStencilOp value);

/*
=================================================
	VEnumCast (EPolygonMode)
=================================================
*/
	ND_ inline VkPolygonMode  VEnumCast (EPolygonMode value);

/*
=================================================
	VEnumCast (ECullMode)
=================================================
*/
	ND_ inline VkCullModeFlagBits  VEnumCast (ECullMode value);
	
/*
=================================================
	VEnumCast (EImageOpt)
=================================================
*/
	ND_ inline VkImageCreateFlagBits  VEnumCast (EImageOpt values);
	
/*
=================================================
	VEnumCast (EImageDim)
=================================================
*/
	ND_ inline VkImageType  VEnumCast (EImageDim value);

/*
=================================================
	VEnumCast (EImage)
=================================================
*/
	ND_ inline VkImageViewType  VEnumCast (EImage value);

/*
=================================================
	VEnumCast (EImageUsage)
=================================================
*/
	ND_ inline VkImageUsageFlagBits  VEnumCast (EImageUsage usage, EMemoryType memType);

/*
=================================================
	VEnumCast (EImageAspect)
=================================================
*/
	ND_ inline VkImageAspectFlagBits  VEnumCast (EImageAspect values);
	
/*
=================================================
	VEnumCast (EBufferUsage)
=================================================
*/
	ND_ inline VkBufferUsageFlagBits  VEnumCast (EBufferUsage values);

/*
=================================================
	VEnumCast (EIndex)
=================================================
*/
	ND_ inline VkIndexType  VEnumCast (EIndex value);

/*
=================================================
	VEnumCast (EPrimitive)
=================================================
*/
	ND_ inline VkPrimitiveTopology  VEnumCast (EPrimitive value);
	
/*
=================================================
	EResourceState_ToShaderStages
=================================================
*/
	ND_ inline VkShaderStageFlags  EResourceState_ToShaderStages (EResourceState value);

/*
=================================================
	EResourceState_ToStageAccessLayout
=================================================
*/
	inline bool  EResourceState_ToDstStageAccessLayout (EResourceState value, OUT VkPipelineStageFlagBits2 &outStage, OUT VkAccessFlagBits2 &outAccess, OUT VkImageLayout &outLayout);
	inline bool  EResourceState_ToSrcStageAccessLayout (EResourceState value, OUT VkPipelineStageFlagBits2 &outStage, OUT VkAccessFlagBits2 &outAccess, OUT VkImageLayout &outLayout);
	inline bool  EResourceState_ToStageAccess (EResourceState value, OUT VkPipelineStageFlagBits2 &outStage, OUT VkAccessFlagBits2 &outAccess);
	ND_ inline VkPipelineStageFlagBits2  EResourceState_ToPipelineStages (EResourceState value);
	ND_ inline VkAccessFlagBits2  EResourceState_ToAccessMask (EResourceState value);
	ND_ inline VkImageLayout  EResourceState_ToSrcImageLayout (EResourceState value);
	ND_ inline VkImageLayout  EResourceState_ToDstImageLayout (EResourceState value);
			
/*
=================================================
	VEnumCast (EPixelFormat)
=================================================
*/
	ND_ inline VkFormat  VEnumCast (EPixelFormat value);
	
/*
=================================================
	AEEnumCast (VkFormat)
=================================================
*/
	ND_ inline EPixelFormat  AEEnumCast (VkFormat value);

/*
=================================================
	AEEnumCast (VkImageType)
=================================================
*/
	ND_ inline EImageDim  AEEnumCast (VkImageType value);
	
/*
=================================================
	AEEnumCast (VkImageUsageFlagBits)
=================================================
*/
	inline void  AEEnumCast (VkImageUsageFlagBits usage, OUT EImageUsage& outUsage, OUT EMemoryType& outMemType);
	
/*
=================================================
	AEEnumCast (VkSampleCountFlagBits)
=================================================
*/
	ND_ inline  MultiSamples  AEEnumCast (VkSampleCountFlagBits samples);
	
/*
=================================================
	AEEnumCast (VkImageCreateFlagBits)
=================================================
*/
	ND_ inline EImageOpt  AEEnumCast (VkImageCreateFlagBits values);
	
/*
=================================================
	AEEnumCast (VkBufferUsageFlagBits)
=================================================
*/
	ND_ inline EBufferUsage  AEEnumCast (VkBufferUsageFlagBits values);
	
/*
=================================================
	VEnumCast (EMemoryType)
=================================================
*/
	ND_ inline VkMemoryPropertyFlagBits  VEnumCast (EMemoryType values);

/*
=================================================
	AEEnumCast (VkMemoryPropertyFlagBits)
=================================================
*/
	ND_ inline EMemoryType  AEEnumCast (VkMemoryPropertyFlagBits values, bool isExternal);
	
/*
=================================================
	AEEnumCast (VkShaderStageFlagBits)
=================================================
*/
	ND_ inline EShaderStages  AEEnumCast (VkShaderStageFlagBits stages);
	
/*
=================================================
	VEnumCast (EPipelineOpt)
=================================================
*/
	ND_ inline VkPipelineCreateFlagBits  VEnumCast (EPipelineOpt values);
	
/*
=================================================
	VEnumCast (ERTASOptions)
=================================================
*/
	ND_ inline VkBuildAccelerationStructureFlagBitsKHR  VEnumCast (ERTASOptions values);
	
/*
=================================================
	VEnumCast (ERTGeometryOpt)
=================================================
*/
	ND_ inline VkGeometryFlagBitsKHR  VEnumCast (ERTGeometryOpt values);
	
/*
=================================================
	VEnumCast (ERTInstanceOpt)
=================================================
*/
	ND_ inline VkGeometryInstanceFlagBitsKHR  VEnumCast (ERTInstanceOpt values);
	
/*
=================================================
	VEnumCast (ERTASCopyMode)
=================================================
*/
	ND_ inline VkCopyAccelerationStructureModeKHR  VEnumCast (ERTASCopyMode value);
	
/*
=================================================
	VEnumCast (EColorSpace)
=================================================
*/
	ND_ inline VkColorSpaceKHR  VEnumCast (EColorSpace value);
	
/*
=================================================
	VEnumCast (EPresentMode)
=================================================
*/
	ND_ inline VkPresentModeKHR  VEnumCast (EPresentMode value);


}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
