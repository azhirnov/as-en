// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/BufferDesc.h"
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Public/IDs.h"
# include "graphics_rhi/Vulkan/VQueue.h"

namespace AE::Graphics
{

	//
	// Vulkan Ray Tracing Micromap
	//

	class VRTMicromap final
	{
	// variables
	private:
		VkMicromapEXT				_micromap		= Default;
		VkBuffer					_storage		= Default;
		Bytes						_offset;
		RTMicromapDesc				_desc;
		Strong<MemoryID>			_memoryId;

		GFX_DBG_ONLY( DebugName_t	_debugName;)


	// methods
	public:
		VRTMicromap ()																							__NE___	{}
		~VRTMicromap ()																							__NE___;

		ND_ bool  Create (ResourceManager &, const RTMicromapDesc &, GfxMemAllocatorPtr, StringView dbgName)	__NE___;
			void  Destroy (ResourceManager &)																	__NE___;

		ND_ VkMicromapEXT				Handle ()																C_NE___	{ return _micromap; }
		ND_ MemoryID					MemoryId ()																C_NE___	{ return _memoryId; }
		ND_ RTMicromapDesc const&		Description ()															C_NE___	{ return _desc; }
		ND_ bool						IsExclusiveSharing ()													C_NE___	{ return true; }
		ND_ BufferSubRange				GetBufferStorage ()														C_NE___;

		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()															C_NE___	{ return _debugName; })

		ND_ static RTMicromapBuildSizes	GetBuildSizes (const ResourceManager &, const RTMicromapInfo &)			__NE___;

		ND_ static bool  ConvertBuildInfo (const ResourceManager &, const RTMicromapInfo &,
										   OUT VkMicromapBuildInfoEXT &)										__NE___;
		ND_ static bool  ConvertBuildInfo (const ResourceManager &, const RTMicromapBuild &,
										   OUT VkMicromapBuildInfoEXT &)										__NE___;

		ND_ static bool  IsSupported (const ResourceManager &, const RTMicromapDesc &)							__NE___;
		ND_ static bool  IsSupported (const ResourceManager &, const RTMicromapInfo &)							__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
