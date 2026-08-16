// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Vulkan/VCommon.h"

namespace AE::Graphics
{

	//
	// Vulkan Memory Object
	//

	class VMemoryObject final
	{
	// variables
	private:
		GfxMemAllocatorPtr				_memAllocator;
		IGfxMemAllocator::Storage_t		_storage;

		GFX_DBG_ONLY( DebugName_t		_debugName;	)


	// methods
	public:
		VMemoryObject ()									__NE___	{}
		~VMemoryObject ()									__NE___;

		ND_ bool  Create (VkBuffer, const BufferDesc &, GfxMemAllocatorPtr, StringView dbgName)	__NE___;
		ND_ bool  Create (VkImage, const ImageDesc &, GfxMemAllocatorPtr, StringView dbgName)	__NE___;
		ND_ bool  Create (Bytes, VkBufferUsageFlagBits2, GfxMemAllocatorPtr, StringView dbgName)__NE___;
			void  Destroy (ResourceManager &)													__NE___;

		ND_ bool  GetMemoryInfo (OUT VulkanMemoryObjInfo &) C_NE___;

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()		C_NE___	{ return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
