// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "base/Containers/FixedTupleArray.h"
# include "graphics/Public/RenderPassDesc.h"
# include "graphics/Vulkan/VCommon.h"
# include "Packer/RenderPassPack.h"

namespace AE::Graphics
{
	using AE::PipelineCompiler::SerializableRenderPassInfo;
	using AE::PipelineCompiler::SerializableVkRenderPass;



	//
	// Vulkan Render Pass
	//

	class VRenderPass final
	{
	// types
	public:
		using ShaderIOArr_t			= PipelineCompiler::SubpassShaderIO::ShaderIOArr_t;
		using AttachmentStates_t	= SerializableVkRenderPass::AttachmentStates_t;

		struct SubpassInfo
		{
			ShaderIOArr_t	colorAttachments;
			ShaderIOArr_t	inputAttachments;
		};

		struct AttachmentIdx
		{
			ubyte			index		= UMax;
			ubyte			clearIdx	= UMax;
		};

	private:
		using AttachmentMap_t	= FixedMap< AttachmentName::Optimized_t, AttachmentIdx, GraphicsConfig::MaxAttachments >;
		using SPNameToIdx_t		= FixedMap< SubpassName::Optimized_t, ubyte, GraphicsConfig::MaxSubpasses >;
		using Subpasses_t		= FixedArray< SubpassInfo, GraphicsConfig::MaxSubpasses >;


	// variables
	private:
		VkRenderPass				_renderPass		= Default;
		Strong<VRenderPassID>		_compatibleRP;	// this RP will be used to create pipelines and framebuffers

		AttachmentMap_t				_attachmentMap;
		AttachmentStates_t			_attStates;
		SPNameToIdx_t				_subpassMap;
		Subpasses_t					_subpasses;
		
		DEBUG_ONLY( SubpassName::Optimized_t	_firstSPName; )
			
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VRenderPass ()																__NE___	{}
		~VRenderPass ()																__NE___;

		ND_ bool  Create (VResourceManager& resMngr, const SerializableRenderPassInfo &compatInfo, const SerializableVkRenderPass &vkInfo,
						  VRenderPassID compatId, StringView dbgName = Default)		__NE___;
			void  Destroy (VResourceManager &)										__NE___;

		ND_ bool  GetMaxTileWorkgroupSize (const VDevice &dev, OUT uint2 &tileSize)	C_NE___;
		ND_ uint2 GetTileSizeGranularity (const VDevice &dev)						C_NE___;

		ND_ VkRenderPass				Handle ()									C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _renderPass; }
		ND_ VRenderPassID				CompatibpleRP ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _compatibleRP; }

		ND_ AttachmentMap_t const&		AttachmentMap ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _attachmentMap; }
		ND_ AttachmentStates_t const&	AttachmentStates ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _attStates; }
		ND_ SPNameToIdx_t const&		SubpassMap ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _subpassMap; }
		ND_ ArrayView<SubpassInfo>		Subpasses ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _subpasses; }

		DEBUG_ONLY(  ND_ StringView		GetDebugName ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		DEBUG_ONLY( ND_ SubpassName::Optimized_t  GetFirstSubpassName ()			C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _firstSPName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
