// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Metal/Resources/MPipelinePack.h"

namespace AE::Graphics
{
	
	//
	// Metal Ray Tracing Shader Binding Table
	//

	class MRTShaderBindingTable final
	{
	// types
	public:
		struct CreateInfo
		{
		};


	// variables
	private:
		RTShaderBindingDesc				_desc;

		DEBUG_ONLY(	DebugName_t			_debugName;	)
		DRC_ONLY(	RWDataRaceCheck		_drCheck;	)

	// methods
	public:
		MRTShaderBindingTable ()									__NE___	{}
		~MRTShaderBindingTable ()									__NE___ {}

		ND_ bool  Create (MResourceManager &, const CreateInfo &)	__NE___	{ return true; }
			void  Destroy (MResourceManager &)						__NE___	{}

		ND_ RTShaderBindingDesc const&	Description ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		
		DEBUG_ONLY(  ND_ StringView		GetDebugName ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
