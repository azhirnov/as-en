// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/VCommon.h"

VK_DEFINE_HANDLE(VmaAllocation)
VK_DEFINE_HANDLE(VmaAllocator)

namespace AE::Graphics
{

	//
	// Vulkan Universal Memory Allocator
	//

	class VUniMemAllocator final : public IGfxMemAllocator
	{
	// types
	private:
		struct Data
		{
			VmaAllocation	allocation;
		};


	// variables
	private:
		mutable SharedMutex		_guard;
		VDevice const&			_device;
		VmaAllocator			_allocator;


	// methods
	public:
		VUniMemAllocator ();
		~VUniMemAllocator () override;
		
		bool  AllocForImage (VkImage image, const ImageDesc &desc, OUT Storage_t &data) override;
		bool  AllocForBuffer (VkBuffer buffer, const BufferDesc &desc, OUT Storage_t &data) override;

		bool  Dealloc (INOUT Storage_t &data) override;
			
		bool  GetInfo (const Storage_t &data, OUT VulkanMemoryObjInfo &info) const override;
		
		Bytes  MinAlignment ()		const override	{ return 1_b; }
		Bytes  MaxAllocationSize ()	const override;

	private:
		bool _CreateAllocator (OUT VmaAllocator &alloc) const;

		ND_ static Data &		_CastStorage (Storage_t &data)			{ return *data.Ptr<Data>(); }
		ND_ static Data const&	_CastStorage (const Storage_t &data)	{ return *data.Ptr<Data>(); }
	};


}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
