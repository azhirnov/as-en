// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VPipelinePack.h"

namespace AE::Graphics
{

	//
	// Vulkan Compute Pipeline
	//

	class VComputePipeline final
	{
	// types
	public:
		struct CreateInfo
		{
			PipelineCompiler::SerializableComputePipeline const&	templCI;
			ComputePipelineDesc const&								specCI;
			PipelineLayoutID										layoutId;
			VPipelinePack::ShaderModuleRef							shader;
			PipelineCacheID											cacheId;
		};


	// variables
	private:
		VkPipeline					_handle				= Default;
		VkPipelineLayout			_layout				= Default;

		ushort3						_localSize;
		EPipelineOpt				_options			= Default;

		Strong<PipelineLayoutID>	_layoutId;

		ShaderTracePtr				_dbgTrace;			// allocated by pipeline pack linear allocator

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VComputePipeline ()													__NE___	{}
		~VComputePipeline ()												__NE___;

		ND_ bool  Create (VResourceManager &, const CreateInfo &ci)			__NE___;
			void  Destroy (VResourceManager &)								__NE___;

		ND_ bool  ParseShaderTrace (const void*					ptr,
									Bytes						maxSize,
									ShaderDebugger::ELogFormat	logFmt,
									OUT Array<String>			&result)	C_NE___;

		ND_ VkPipeline				Handle ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _handle; }
		ND_ VkPipelineLayout		Layout ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layout; }
		ND_ VkPipelineBindPoint		BindPoint ()							C_NE___	{ return VK_PIPELINE_BIND_POINT_COMPUTE; }
		ND_ PipelineLayoutID		LayoutId ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ uint3					LocalSize ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return uint3{_localSize}; }
		ND_ EPipelineDynamicState	DynamicState ()							C_NE___	{ return Default; }
		ND_ EPipelineOpt			Options ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _options; }
		ND_ bool					AllowDispatchBase ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return AllBits( _options, EPipelineOpt::CS_DispatchBase ); }

		ND_ ShaderTracePtr			GetShaderTrace ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _dbgTrace; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
