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
		VDedicatedMemAllocator ()															__NE___;
		~VDedicatedMemAllocator ()															__NE_OV;
		
	  // IGfxMemAllocator //
		bool  AllocForImage (VkImage image, const ImageDesc &desc, OUT Storage_t &data)		__NE_OV;
		bool  AllocForBuffer (VkBuffer buffer, const BufferDesc &desc, OUT Storage_t &data) __NE_OV;

		bool  Dealloc (INOUT Storage_t &data)												__NE_OV;
			
		bool  GetInfo (const Storage_t &data, OUT VulkanMemoryObjInfo &info)				C_NE_OV;
		
		Bytes  MinAlignment ()																C_NE_OV	{ return 1_b; }
		Bytes  MaxAllocationSize ()															C_NE_OV;


	private:
		ND_ static Data &		_CastStorage (Storage_t &data)			{ return *data.Ptr<Data>(); }
		ND_ static Data const&	_CastStorage (const Storage_t &data)	{ return *data.Ptr<Data>(); }
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
