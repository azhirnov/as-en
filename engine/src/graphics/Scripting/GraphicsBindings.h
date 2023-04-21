// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_SCRIPTING
# include "scripting/Bindings/CoreBindings.h"
# include "graphics/Public/ResourceEnums.h"
# include "graphics/Public/EResourceState.h"
# include "graphics/Public/RenderState.h"
# include "graphics/Public/FeatureSet.h"
# include "graphics/Public/SamplerDesc.h"
# include "graphics/Public/DescriptorSet.h"
# include "graphics/Public/PipelineDesc.h"
# include "graphics/Public/VideoEnums.h"

namespace AE::Scripting
{
	
	//
	// Graphics Bindings
	//

	struct GraphicsBindings final : Noninstancable
	{
		static void  BindEnums (const ScriptEnginePtr &se)			__Th___;
		static void  BindTypes (const ScriptEnginePtr &se)			__Th___;
		static void  BindRenderState (const ScriptEnginePtr &se)	__Th___;
	};

	
	// BindEnums
	AE_DECL_SCRIPT_TYPE( Graphics::EPixelFormat,				"EPixelFormat"			);
	AE_DECL_SCRIPT_TYPE( Graphics::EAttachmentLoadOp,			"EAttachmentLoadOp"		);
	AE_DECL_SCRIPT_TYPE( Graphics::EAttachmentStoreOp,			"EAttachmentStoreOp"	);
	AE_DECL_SCRIPT_TYPE( Graphics::ECompareOp,					"ECompareOp"			);
	AE_DECL_SCRIPT_TYPE( Graphics::EBlendFactor,				"EBlendFactor"			);
	AE_DECL_SCRIPT_TYPE( Graphics::EBlendOp,					"EBlendOp"				);
	AE_DECL_SCRIPT_TYPE( Graphics::ELogicOp,					"ELogicOp"				);
	AE_DECL_SCRIPT_TYPE( Graphics::EStencilOp,					"EStencilOp"			);
	AE_DECL_SCRIPT_TYPE( Graphics::EPolygonMode,				"EPolygonMode"			);
	AE_DECL_SCRIPT_TYPE( Graphics::EPrimitive,					"EPrimitive"			);
	AE_DECL_SCRIPT_TYPE( Graphics::ECullMode,					"ECullMode"				);
	AE_DECL_SCRIPT_TYPE( Graphics::EPipelineDynamicState,		"EPipelineDynamicState"	);
	AE_DECL_SCRIPT_TYPE( Graphics::EResourceState,				"EResourceState"		);
	AE_DECL_SCRIPT_TYPE( Graphics::EImageAspect,				"EImageAspect"			);
	AE_DECL_SCRIPT_TYPE( Graphics::EShaderIO,					"EShaderIO"				);
	AE_DECL_SCRIPT_TYPE( Graphics::ESubgroupTypes,				"ESubgroupTypes"		);
	AE_DECL_SCRIPT_TYPE( Graphics::ESubgroupOperation,			"ESubgroupOperation"	);
	AE_DECL_SCRIPT_TYPE( Graphics::EFeature,					"EFeature"				);
	AE_DECL_SCRIPT_TYPE( Graphics::EShader,						"EShader"				);
	AE_DECL_SCRIPT_TYPE( Graphics::EShaderStages,				"EShaderStages"			);
	AE_DECL_SCRIPT_TYPE( Graphics::EVendorID,					"EVendorID"				);
	AE_DECL_SCRIPT_TYPE( Graphics::EVertexType,					"EVertexType"			);
	AE_DECL_SCRIPT_TYPE( Graphics::EGraphicsDeviceID,			"EGraphicsDeviceID"		);
	AE_DECL_SCRIPT_TYPE( Graphics::EFilter,						"EFilter"				);
	AE_DECL_SCRIPT_TYPE( Graphics::EMipmapFilter,				"EMipmapFilter"			);
	AE_DECL_SCRIPT_TYPE( Graphics::EAddressMode,				"EAddressMode"			);
	AE_DECL_SCRIPT_TYPE( Graphics::EBorderColor,				"EBorderColor"			);
	AE_DECL_SCRIPT_TYPE( Graphics::EReductionMode,				"EReductionMode"		);
	AE_DECL_SCRIPT_TYPE( Graphics::ESamplerUsage,				"ESamplerUsage"			);
	AE_DECL_SCRIPT_TYPE( Graphics::EVertexInputRate,			"EVertexInputRate"		);
	AE_DECL_SCRIPT_TYPE( Graphics::EDescSetUsage,				"EDescSetUsage"			);
	AE_DECL_SCRIPT_TYPE( Graphics::EPipelineOpt,				"EPipelineOpt"			);
	AE_DECL_SCRIPT_TYPE( Graphics::EQueueMask,					"EQueueMask"			);
	AE_DECL_SCRIPT_TYPE( Graphics::ESamplerChromaLocation,		"ESamplerChromaLocation"		);
	AE_DECL_SCRIPT_TYPE( Graphics::ESamplerYcbcrModelConversion,"ESamplerYcbcrModelConversion"	);
	AE_DECL_SCRIPT_TYPE( Graphics::ESamplerYcbcrRange,			"ESamplerYcbcrRange"	);
	AE_DECL_SCRIPT_TYPE( Graphics::ESurfaceFormat,				"ESurfaceFormat"		);

	// BindTypes
	AE_DECL_SCRIPT_OBJ(	 Graphics::MultiSamples,		"MultiSamples"			);
	AE_DECL_SCRIPT_OBJ(	 Graphics::ImageLayer,			"ImageLayer"			);
	AE_DECL_SCRIPT_OBJ(	 Graphics::MipmapLevel,			"MipmapLevel"			);

	// BindRenderState
	AE_DECL_SCRIPT_OBJ(	 Graphics::RenderState::ColorBuffer::ColorPair<Graphics::EBlendFactor>,	"RenderState_ColorBuffer_BlendFactor"	);
	AE_DECL_SCRIPT_OBJ(	 Graphics::RenderState::ColorBuffer::ColorPair<Graphics::EBlendOp>,		"RenderState_ColorBuffer_BlendOp"		);
	AE_DECL_SCRIPT_OBJ(	 Graphics::RenderState::ColorBuffer::ColorMask,							"RenderState_ColorBuffer_ColorMask"		);
	AE_DECL_SCRIPT_OBJ(	 Graphics::RenderState::ColorBuffer,		"RenderState_ColorBuffer"			);
	AE_DECL_SCRIPT_OBJ(	 Graphics::RenderState::ColorBuffersState,	"RenderState_ColorBuffersState"		);
	AE_DECL_SCRIPT_OBJ(	 Graphics::RenderState::StencilFaceState,	"RenderState_StencilFaceState"		);
	AE_DECL_SCRIPT_OBJ(	 Graphics::RenderState::StencilBufferState,	"RenderState_StencilBufferState"	);
	AE_DECL_SCRIPT_OBJ(	 Graphics::RenderState::DepthBufferState,	"RenderState_DepthBufferState"		);
	AE_DECL_SCRIPT_OBJ(	 Graphics::RenderState::InputAssemblyState,	"RenderState_InputAssemblyState"	);
	AE_DECL_SCRIPT_OBJ(	 Graphics::RenderState::RasterizationState,	"RenderState_RasterizationState"	);
	AE_DECL_SCRIPT_OBJ(	 Graphics::RenderState::MultisampleState,	"RenderState_MultisampleState"		);
	AE_DECL_SCRIPT_OBJ(	 Graphics::RenderState,						"RenderState"						);


} // AE::Scripting

#endif // AE_ENABLE_SCRIPTING
