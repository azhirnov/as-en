// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Allocation on the GPU memory may be slow.
	Internal data is protected by the mutex for thread safety,
	so parallel usege is not recomended because may be too slow.
*/

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/Allocators/VGfxMemAllocatorUtils.h"

namespace AE::Graphics
{

	//
	// Vulkan Linear Memory Allocator
	//

	class VLinearMemAllocator final : public IGfxMemAllocator
	{
	// types
	private:
		struct alignas(AE_CACHE_LINE) Page
		{
			Atomic<int>			counter			{0};		// reference counter, for debugging
			VkDeviceMemory		memory			= Default;
			Bytes				capacity;
			Bytes				size;
			void*				mapped			= null;
			uint				memTypeIndex	= UMax;

			Page () {}

			Page (Page && other) :
				memory{ other.memory },	capacity{ other.capacity }, size{ other.size },
				mapped{ other.mapped }, memTypeIndex{ other.memTypeIndex }
			{}
		};
		
		struct Data
		{
			Page *		page;
			Bytes		offset;
			Bytes		size;
		};

		using Key		= VGfxMemAllocatorUtils::Key;
		using PageMap_t = FixedMap< Key, FixedArray< Page, 8 >, 4 >;

		static constexpr Bytes	_Align {4 << 10};


	// variables
	private:
		mutable SharedMutex		_pageGuard;
		Bytes					_pageSize;
		PageMap_t				_pages;


	// methods
	public:
		explicit VLinearMemAllocator (Bytes pageSize = 0_b);
		~VLinearMemAllocator () override;
		
		void  SetPageSize (Bytes size);
		void  Discard ();

	  // IGfxMemAllocator //
		bool  AllocForImage (VkImage image, const ImageDesc &desc, OUT Storage_t &data) override;
		bool  AllocForBuffer (VkBuffer buffer, const BufferDesc &desc, OUT Storage_t &data) override;

		bool  Dealloc (INOUT Storage_t &data) override;
			
		bool  GetInfo (const Storage_t &data, OUT VulkanMemoryObjInfo &info) const override;
		
		Bytes  MinAlignment ()		const override	{ return 1_b; }
		Bytes  MaxAllocationSize ()	const override	{ return _pageSize; }


	private:
		ND_ static Data &		_CastStorage (Storage_t &data)			{ return *data.Ptr<Data>(); }
		ND_ static Data const&	_CastStorage (const Storage_t &data)	{ return *data.Ptr<Data>(); }
		
		ND_ Bytes			_GetOffset (const Data &data)		const	{ return data.offset; }
		ND_ VkDeviceMemory	_GetMemory (const Data &data)		const	{ return data.page->memory; }

		ND_ bool  _IsValidPage (const Page* page) const;
		ND_ bool  _Allocate (const VkMemoryRequirements &memReq, EMemoryType memType, bool shaderAddress, bool isImage, OUT Data &);
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
