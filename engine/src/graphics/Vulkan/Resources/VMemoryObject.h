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
		GfxMemAllocatorPtr			_memAllocator;
		IGfxMemAllocator::Storage_t	_storage;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VMemoryObject () {}
		~VMemoryObject ();

		ND_ bool  Create (VkBuffer buffer, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
		ND_ bool  Create (VkImage image, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
			void  Destroy (VResourceManager &);
		
		ND_ bool  GetMemoryInfo (OUT VulkanMemoryObjInfo &) const;
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName () const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};
	

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
