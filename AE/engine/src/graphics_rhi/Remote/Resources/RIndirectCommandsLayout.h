// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Public/IndirectCommandBuffer.h"
# include "graphics_rhi/Remote/RCommon.h"

namespace AE::Graphics
{

	//
	// Vulkan Indirect Commands Layout immutable data
	//

	class RIndirectCommandsLayout final
	{
	// variables
	private:
		RmIndirectCommandsLayoutID		_id;
		EIndirectCommandsLayoutUsage	_usage			= Default;
		EShaderStages					_stages			= Default;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RIndirectCommandsLayout ()								__NE___	{}
		~RIndirectCommandsLayout ()								__NE___;

		ND_ bool  Create (ResourceManager &, const IndirectCommandsLayoutDesc &, StringView dbgName)	__NE___;
			void  Destroy (ResourceManager &)															__NE___;

		ND_ RmIndirectCommandsLayoutID		Handle ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _id; }

		GFX_DBG_ONLY( ND_ StringView		GetDebugName ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
