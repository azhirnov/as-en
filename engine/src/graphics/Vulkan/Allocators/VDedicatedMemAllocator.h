// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{

	//
	// Vulkan Dedicated Memory Allocator
	//

	class VDedicatedMemAllocator final : public IGfxMemAllocator
	{
	// types
	private:
		struct Data
		{
			VkDeviceMemory	mem;
			Bytes			size;
			void*			mapped;
			uint			index;
		};


	// variables
	private:
		alignas(AE_CACHE_LINE)
		  Atomic<int>			_counter {0};

		VDevice const&			_device;
		const bool				_supportDedicated;


	// methods
	public:
		VDedicatedMemAllocator ();
		~VDedicatedMemAllocator () override;
		
		bool  AllocForImage (VkImage image, const ImageDesc &desc, OUT Storage_t &data) override;
		bool  AllocForBuffer (VkBuffer buffer, const BufferDesc &desc, OUT Storage_t &data) override;

		bool  Dealloc (INOUT Storage_t &data) override;
			
		bool  GetInfo (const Storage_t &data, OUT VulkanMemoryObjInfo &info) const override;
		
		Bytes  MinAlignment ()		const override	{ return 1_b; }
		Bytes  MaxAllocationSize ()	const override;

	private:
		ND_ static Data &		_CastStorage (Storage_t &data)			{ return *data.Ptr<Data>(); }
		ND_ static Data const&	_CastStorage (const Storage_t &data)	{ return *data.Ptr<Data>(); }
	};


}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
