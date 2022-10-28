// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{

	//
	// Vulkan Large Memory Allocator
	//

	class VLargeMemAllocator final : public IGfxMemAllocator
	{
	// types
	private:
		struct Data
		{
		};


	// variables
	private:
		VDevice const&		_device;


	// methods
	public:
		explicit VLargeMemAllocator (const VDevice &dev);
		~VLargeMemAllocator () override;
		
	  // IGfxMemAllocator //
		bool  AllocForImage (ImageVk_t image, const ImageDesc &desc, OUT Storage_t &data) override;
		bool  AllocForBuffer (BufferVk_t buffer, const BufferDesc &desc, OUT Storage_t &data) override;

		bool  Dealloc (INOUT Storage_t &data) override;
			
		bool  GetInfo (const Storage_t &data, OUT VulkanMemoryObjInfo &info) const override;
		
		Bytes  MinAlignment ()		const override;
		Bytes  MaxAllocationSize ()	const override;

	private:
		ND_ static Data &		_CastStorage (Storage_t &data);
		ND_ static Data const&	_CastStorage (const Storage_t &data);
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
