// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/BufferDesc.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/IDs.h"
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics
{

	//
	// Metal Video Session immutable data
	//

	class MVideoSession final
	{
	// variables
	private:
		VideoSessionDesc				_desc;
		
		DEBUG_ONLY(	DebugName_t			_debugName;	)
		DRC_ONLY(	RWDataRaceCheck		_drCheck;	)


	// methods
	public:
		MVideoSession ()								__NE___	{}
		~MVideoSession ()								__NE___;

		ND_ bool  Create (MResourceManager &, const VideoSessionDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)	__NE___;
			void  Destroy (MResourceManager &)																					__NE___;

	//	ND_ VkVideoSessionKHR			Session ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _session; }
		ND_ VideoSessionDesc const&		Description ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })


		ND_ static bool  Validate (const MDevice &dev, INOUT VideoSessionDesc &desc)			__NE___;
		ND_ static bool  IsSupported (const MResourceManager &, const VideoSessionDesc &desc)	__NE___;
	};
	

} // AE::Graphics

#endif // AE_ENABLE_METAL
