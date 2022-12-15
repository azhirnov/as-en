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


	// methods
	public:
		MRTShaderBindingTable ()									__NE___	{}
		~MRTShaderBindingTable ()									__NE___ {}

		ND_ bool  Create (MResourceManager &, const CreateInfo &)	__NE___	{ return true; }
			void  Destroy (MResourceManager &)						__NE___	{}

		DEBUG_ONLY(  ND_ StringView		GetDebugName ()				C_NE___	{ return Default; })
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
