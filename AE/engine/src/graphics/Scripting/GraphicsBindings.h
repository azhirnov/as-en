// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_SCRIPTING
# include "pch/Scripting.h"

# include "graphics/Public/ResourceEnums.h"
# include "graphics/Public/EResourceState.h"
# include "graphics/Public/RenderState.h"
# include "graphics/Public/FeatureSet.h"
# include "graphics/Public/SamplerDesc.h"
# include "graphics/Public/DescriptorSet.h"
# include "graphics/Public/PipelineDesc.h"
# include "graphics/Public/VideoEnums.h"
# include "graphics/Public/RayTracingEnums.h"

namespace AE::Scripting
{

	//
	// Graphics Bindings
	//

	struct GraphicsBindings final : Noninstanceable
	{
		static void  BindEnums (const ScriptEnginePtr &se)			__Th___;
		static void  BindTypes (const ScriptEnginePtr &se)			__Th___;
		static void  BindRenderState (const ScriptEnginePtr &se)	__Th___;
	};

} // AE::Scripting


// BindEnums
AE_DECL_SCRIPT_TYPE( AE::Graphics::EImage,						"EImage"				);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EIndex,						"EIndex"				);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EPixelFormat,				"EPixelFormat"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EPixelFormatExternal,		"EPixelFormatExternal"	);
AE_DECL_SCRIPT_TYPE( AE::Graphics::ECompareOp,					"ECompareOp"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EBlendFactor,				"EBlendFactor"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EBlendOp,					"EBlendOp"				);
AE_DECL_SCRIPT_TYPE( AE::Graphics::ELogicOp,					"ELogicOp"				);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EStencilOp,					"EStencilOp"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EPolygonMode,				"EPolygonMode"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EPrimitive,					"EPrimitive"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::ECullMode,					"ECullMode"				);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EPipelineDynamicState,		"EPipelineDynamicState"	);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EResourceState,				"EResourceState"		);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EImageAspect,				"EImageAspect"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EShaderIO,					"EShaderIO"				);
AE_DECL_SCRIPT_TYPE( AE::Graphics::ESubgroupTypes,				"ESubgroupTypes"		);
AE_DECL_SCRIPT_TYPE( AE::Graphics::ESubgroupOperation,			"ESubgroupOperation"	);
AE_DECL_SCRIPT_TYPE( AE::Graphics::FeatureSet::EFeature,		"EFeature"				);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EShader,						"EShader"				);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EShaderStages,				"EShaderStages"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EGPUVendor,					"EGPUVendor"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EVertexType,					"EVertexType"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EGraphicsDeviceID,			"EGraphicsDeviceID"		);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EFilter,						"EFilter"				);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EMipmapFilter,				"EMipmapFilter"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EAddressMode,				"EAddressMode"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EBorderColor,				"EBorderColor"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EReductionMode,				"EReductionMode"		);
AE_DECL_SCRIPT_TYPE( AE::Graphics::ESamplerOpt,					"ESamplerOpt"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EVertexInputRate,			"EVertexInputRate"		);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EDescSetUsage,				"EDescSetUsage"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EPipelineOpt,				"EPipelineOpt"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::EQueueMask,					"EQueueMask"			);
AE_DECL_SCRIPT_TYPE( AE::Graphics::ESamplerChromaLocation,		"ESamplerChromaLocation"		);
AE_DECL_SCRIPT_TYPE( AE::Graphics::ESamplerYcbcrModelConversion,"ESamplerYcbcrModelConversion"	);
AE_DECL_SCRIPT_TYPE( AE::Graphics::ESamplerYcbcrRange,			"ESamplerYcbcrRange"	);
AE_DECL_SCRIPT_TYPE( AE::Graphics::ESurfaceFormat,				"ESurfaceFormat"		);
AE_DECL_SCRIPT_TYPE( AE::Graphics::ERTInstanceOpt,				"ERTInstanceOpt"		);

// BindTypes
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::MultiSamples,				"MultiSamples"			);
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::ImageLayer,					"ImageLayer"			);
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::MipmapLevel,					"MipmapLevel"			);

// BindRenderState
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::RenderState::ColorBuffer::ColorPair< AE::Graphics::EBlendFactor >,	"RenderState_ColorBuffer_BlendFactor"	);
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::RenderState::ColorBuffer::ColorPair< AE::Graphics::EBlendOp >,		"RenderState_ColorBuffer_BlendOp"		);
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::RenderState::ColorBuffer::ColorMask,									"RenderState_ColorBuffer_ColorMask"		);
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::RenderState::ColorBuffer,		"RenderState_ColorBuffer"			);
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::RenderState::ColorBuffersState,	"RenderState_ColorBuffersState"		);
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::RenderState::StencilFaceState,	"RenderState_StencilFaceState"		);
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::RenderState::StencilBufferState,	"RenderState_StencilBufferState"	);
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::RenderState::DepthBufferState,	"RenderState_DepthBufferState"		);
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::RenderState::InputAssemblyState,	"RenderState_InputAssemblyState"	);
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::RenderState::RasterizationState,	"RenderState_RasterizationState"	);
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::RenderState::MultisampleState,	"RenderState_MultisampleState"		);
AE_DECL_SCRIPT_OBJ(	 AE::Graphics::RenderState,						"RenderState"						);

#endif // AE_ENABLE_SCRIPTING
