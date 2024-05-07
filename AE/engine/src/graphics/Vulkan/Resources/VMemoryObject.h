// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/VCommon.h"

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

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VMemoryObject ()									__NE___	{}
		~VMemoryObject ()									__NE___;

		ND_ bool  Create (VkBuffer, const BufferDesc &, GfxMemAllocatorPtr, StringView dbgName)	__NE___;
		ND_ bool  Create (VkImage, const ImageDesc &, GfxMemAllocatorPtr, StringView dbgName)	__NE___;
			void  Destroy (VResourceManager &)													__NE___;

		ND_ bool  GetMemoryInfo (OUT VulkanMemoryObjInfo &) C_NE___;

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
