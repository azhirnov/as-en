// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Resources/VDescriptorSetLayout.h"

namespace AE::Graphics
{

	//
	// Vulkan Pipeline Layout
	//

	class VPipelineLayout final
	{
	// types
	public:
		struct DescSetLayout
		{
			DescriptorSetLayoutID		layoutId;	// strong reference is not needed - PipelineLayout and DSL are managed by PipelinePack
			DescSetBinding				index;
		};

		using DescriptorSets_t		= FixedMap< DescriptorSetName::Optimized_t, DescSetLayout, GraphicsConfig::MaxDescriptorSets >;
		using PushConst				= PipelineCompiler::PushConstants::PushConst;
		using PushConstants_t		= PipelineCompiler::PushConstants::PushConstMap_t;

		struct CreateInfo
		{
		// variables
			DescriptorSets_t const&		descSetLayouts;
			PushConstants_t const&		pushConstants;
			VkDescriptorSetLayout		emptyLayout		= Default;
			IAllocator *				allocator		= null;		// only for descriptor heap
			StringView					dbgName;

		// methods
			CreateInfo (DescriptorSets_t const&		descSetLayouts,
						PushConstants_t const&		pushConstants,
						VkDescriptorSetLayout		emptyLayout,
						IAllocator *				allocator,
						StringView					dbgName			= Default) __NE___ :
				descSetLayouts{descSetLayouts}, pushConstants{pushConstants},
				emptyLayout{emptyLayout}, allocator{allocator}, dbgName{dbgName}
			{}
		};

		using LayoutOrHeapBindings_t = Union< NullUnion, VkPipelineLayout, VkShaderDescriptorSetAndBindingMappingInfoEXT const* >;


	// variables
	private:
		LayoutOrHeapBindings_t		_layout;
		PushConstants_t				_pushConstants;
		DescriptorSets_t			_descriptorSets;
		uint						_firstDescSet	= UMax;

		GFX_DBG_ONLY( DebugName_t	_debugName;	)


	// methods
	public:
		VPipelineLayout ()													__NE___	{}
		~VPipelineLayout ()													__NE___;

		ND_ bool  Create (ResourceManager &, const CreateInfo &ci)			__NE___;
			void  Destroy (ResourceManager &)								__NE___;

		ND_ bool  GetDescriptorSetLayout (DescriptorSetName::Ref,
										  OUT DescriptorSetLayoutID &,
										  OUT DescSetBinding &)				C_NE___;


		ND_ VkPipelineLayout		Handle ()								C_NE___;
		ND_ auto 					GetHeapMapping ()						C_NE___ -> VkShaderDescriptorSetAndBindingMappingInfoEXT const*;

		ND_ uint					GetFirstDescriptorSet ()				C_NE___	{ return _firstDescSet; }
		ND_ DescriptorSets_t const&	GetDescriptorSets ()					C_NE___	{ return _descriptorSets; }
		ND_ PushConstants_t const&	GetPushConstants ()						C_NE___	{ return _pushConstants; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()						C_NE___	{ return _debugName; })

	private:
		ND_ bool  _CreateLayout (ResourceManager &, const CreateInfo &)		__NE___;
		ND_ bool  _CreateHeapMapping (const ResourceManager &, const CreateInfo &)__NE___;
	};

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
