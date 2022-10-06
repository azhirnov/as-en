// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Metal/Resources/MDescriptorSetLayout.h"

namespace AE::Graphics
{

	//
	// Metal Pipeline Layout
	//

	class MPipelineLayout final
	{
	// types
	public:
		struct DescSetLayout
		{
			DescriptorSetLayoutID		layoutId;
			//VkDescriptorSetLayout		layout		= Default;	// TODO: remove?
			uint						index		= 0;
		};

		using DescriptorSets_t	= FixedMap< DescriptorSetName::Optimized_t, DescSetLayout, GraphicsConfig::MaxDescriptorSets >;
		using PushConst			= PipelineCompiler::PushConstants::PushConst;
		using PushConstants_t	= PipelineCompiler::PushConstants::PushConstMap_t;
		

	// variables
	private:
		DescriptorSets_t			_descriptorSets;
		uint						_firstDescSet	= UMax;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)

		
	// methods
	public:
		MPipelineLayout () {}
		~MPipelineLayout () {}
		
		ND_ bool  Create (MResourceManager &resMngr, const DescriptorSets_t &descSetLayouts, const PushConstants_t &pushConstants, StringView dbgName);
			void  Destroy (MResourceManager &);
		
		ND_ bool  GetDescriptorSetLayout (const DescriptorSetName &id, OUT DescriptorSetLayoutID &layout, OUT uint &binding) const;
		
		ND_ uint					GetFirstDescriptorSet ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _firstDescSet; }
		ND_ DescriptorSets_t const&	GetDescriptorSets ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _descriptorSets; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

}	// AE::Graphics

#endif	// AE_ENABLE_METAL
