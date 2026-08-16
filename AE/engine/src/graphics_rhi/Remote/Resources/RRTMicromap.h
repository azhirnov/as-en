// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Public/RayTracingDesc.h"
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Public/IDs.h"
# include "graphics_rhi/Remote/RQueue.h"

namespace AE::Graphics
{

	//
	// Remote Ray Tracing Micromap
	//

	class RRTMicromap final
	{
	// variables
	private:
		RmRTMicromapID					_micromap;
		RmMemoryID						_memoryId;
		RTMicromapDesc					_desc;
		GfxMemAllocatorPtr				_allocator;

		GFX_DBG_ONLY( DebugName_t		_debugName;	)


	// methods
	public:
		RRTMicromap ()																							__NE___	{}
		~RRTMicromap ()																							__NE___;

		ND_ bool  Create (ResourceManager &, const RTMicromapDesc &, GfxMemAllocatorPtr, StringView dbgName)	__NE___;
			void  Destroy (ResourceManager &)																	__NE___;

		ND_ RmRTMicromapID				Handle ()																C_NE___	{ return _micromap; }
		ND_ RmMemoryID					MemoryId ()																C_NE___	{ return _memoryId; }
		ND_ RTMicromapDesc const&		Description ()															C_NE___	{ return _desc; }
		ND_ bool						IsExclusiveSharing ()													C_NE___	{ return true; }

		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()															C_NE___	{ return _debugName; })

		ND_ static RTMicromapBuildSizes	GetBuildSizes (const ResourceManager &, const RTMicromapInfo &)			__NE___;

		ND_ static bool  IsSupported (const ResourceManager &, const RTMicromapDesc &)							__NE___;
		ND_ static bool  IsSupported (const ResourceManager &, const RTMicromapInfo &)							__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
