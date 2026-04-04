// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Public/IndirectCommandBuffer.h"
# include "graphics_rhi/Remote/RCommon.h"

namespace AE::Graphics
{

	//
	// Remote Indirect Execution Set immutable data
	//

	class RIndirectExecutionSet final
	{
	// types
	public:
		struct InitialState
		{
			// TODO
			EPipelineDynamicState	dynamicState	= Default;
		};

		struct CreateInfo
		{
			InitialState			initialState;
			StringView				dbgName;
		};


	// variables
	private:
		RmIndirectExecutionSetID		_id;
		InitialState					_initialState;
		uint							_pipelineCount	= 0;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RIndirectExecutionSet ()											__NE___	{}
		~RIndirectExecutionSet ()											__NE___;

		ND_ bool  Create (ResourceManager &, const CreateInfo &)	__NE___;
			void  Destroy (ResourceManager &)						__NE___;

		ND_ RmIndirectExecutionSetID		Handle ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _id; }
		ND_ InitialState					GetInitialState ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _initialState; }
		ND_ uint							PipelineCount ()		C_NE___	{ return _pipelineCount; }

		GFX_DBG_ONLY( ND_ StringView		GetDebugName ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
