// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RDescriptorSetLayout.h"
# include "graphics/Remote/Resources/RPipelinePack.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Pipeline Layout
	//

	class RPipelineLayout final
	{
	// types
	public:
		using DescriptorSets_t	= RPipelinePack::DSLMap_t;
		using PushConstants_t	= PipelineCompiler::PushConstants::PushConstMap_t;

		struct CreateInfo
		{
			DescriptorSets_t const&		ds;
			PushConstants_t const&		pc;
			StringView					dbgName;
		};

	private:
		struct PushConst
		{
			ShaderStructName	typeName;
			Bytes16u			size;
			PushConstantIndex	idx;
		};
		using PushConstMap_t = FixedMap< PushConstantName::Optimized_t, PushConst, GraphicsConfig::MaxPushConstants >;


	// variables
	private:
		RmPipelineLayoutID		_plId;
		DescriptorSets_t		_descriptorSets;
		PushConstMap_t			_pushConstants;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RPipelineLayout ()												__NE___	{}
		~RPipelineLayout ()												__NE___;

		ND_ bool  Create (RResourceManager &, const CreateInfo &ci)		__NE___;
			void  Destroy (RResourceManager &)							__NE___;

		ND_ bool  GetDescriptorSetLayout (DescriptorSetName::Ref,
										  OUT DescriptorSetLayoutID &,
										  OUT DescSetBinding &)			C_NE___;

		ND_ RmPipelineLayoutID			Handle ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _plId; }
		ND_ DescriptorSets_t const&		GetDescriptorSets ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _descriptorSets; }
		ND_ PushConstMap_t const&		GetPushConstants ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _pushConstants; }

		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
