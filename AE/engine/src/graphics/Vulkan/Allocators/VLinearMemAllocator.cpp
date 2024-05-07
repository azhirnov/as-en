// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Allocators/VLinearMemAllocator.h"
# include "graphics/Vulkan/Allocators/VAutoreleaseMemory.h"
# include "graphics/Vulkan/Resources/VBuffer.h"
# include "graphics/Vulkan/Resources/VImage.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"

# define VGFXALLOC	VLinearMemAllocator
# include "graphics/Vulkan/Allocators/VGfxMemAllocatorUtils.cpp.h"

namespace AE::Graphics
{
namespace
{
	static constexpr Bytes	c_PageAlign			{4 << 10};
	static constexpr Bytes	c_DefaultPageSize	{64 << 20};

	ND_ static Bytes  ValidatePageSize (Bytes pageSize)
	{
		pageSize = (pageSize == 0 ? c_DefaultPageSize : pageSize);
		pageSize = AlignUp( pageSize, c_PageAlign );

		auto&	dev = GraphicsScheduler().GetDevice();
		if ( dev.GetVExtensions().maintenance3 )
		{
			pageSize = Min( pageSize, Bytes{dev.GetVProperties().maintenance3Props.maxMemoryAllocationSize} );
			ASSERT( pageSize > 0 );
		}

		return pageSize;
	}
}

/*
=================================================
	constructor
=================================================
*/
	VLinearMemAllocator::VLinearMemAllocator (Bytes pageSize) __NE___ :
		_pageSize{ ValidatePageSize( pageSize )}
	{}

/*
=================================================
	destructor
=================================================
*/
	VLinearMemAllocator::~VLinearMemAllocator () __NE___
	{
		EXLOCK( _pageGuard );

		auto&	dev = GraphicsScheduler().GetDevice();

		for (auto [key, pages] : _pages)
		{
			for (auto& page : pages)
			{
				CHECK( page.dbgCounter.exchange( 0 ) == 0 );

				if ( page.mapped != null )
					dev.vkUnmapMemory( dev.GetVkDevice(), page.memory );

				ASSERT( page.memory != Default );
				dev.vkFreeMemory( dev.GetVkDevice(), page.memory, null );
			}
		}
	}

/*
=================================================
	Discard
=================================================
*/
	void  VLinearMemAllocator::Discard () __NE___
	{
		EXLOCK( _pageGuard );

		for (auto [key, pages] : _pages)
		{
			for (auto& page : pages)
			{
				CHECK( page.dbgCounter.exchange( 0 ) == 0 );

				page.size = 0_b;
			}
		}
	}

/*
=================================================
	_Allocate
=================================================
*/
	inline bool  VLinearMemAllocator::_Allocate (VDevice const& dev, const Bytes memSize, const Bytes memAlign, const uint memBits,
												 const Bool shaderAddress, const Bool isImage, const Bool mapMem, OUT Data &outData) __NE___
	{
		outData = Default;

		// try to allocate in page
		{
			EXLOCK( _pageGuard );

			for (uint type_idx : BitIndexIterate( memBits ))
			{
				const Key	key{ type_idx, shaderAddress, isImage };

				auto	iter = _pages.find( key );
				if ( iter == _pages.end() )
					continue;

				for (auto& page : iter->second)
				{
					Bytes	offset = AlignUp( page.size, memAlign );

					if_unlikely( offset + memSize <= page.capacity )
					{
						page.size = offset + memSize;
						page.dbgCounter.fetch_add( 1 );

						outData.page	= &page;
						outData.offset	= offset;
						outData.size	= memSize;
						return true;
					}
				}
			}
		}

		// create new page
		VkMemoryAllocateInfo		mem_alloc	= {};
		VkMemoryAllocateFlagsInfo	mem_flag	= {};
		VAutoreleaseMemory			memory		{dev};

		mem_alloc.sType			 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mem_alloc.pNext			 = shaderAddress ? &mem_flag : null;
		mem_alloc.allocationSize = VkDeviceSize( Max( AlignUp( memSize*2, c_PageAlign ), _pageSize ));

		mem_flag.sType			 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
		mem_flag.flags			 = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

		for (uint type_idx : BitIndexIterate( memBits ))
		{
			mem_alloc.memoryTypeIndex = type_idx;

			if_likely( dev.AllocateMemory( mem_alloc, OUT memory.Ref() ) == VK_SUCCESS )
				break;
		}
		CHECK_ERR( memory.Get() != Default );


		// map memory
		void*	mapped_ptr = null;
		if ( mapMem )
			VK_CHECK_ERR( dev.vkMapMemory( dev.GetVkDevice(), memory.Get(), 0, mem_alloc.allocationSize, 0, OUT &mapped_ptr ));


		EXLOCK( _pageGuard );

		const Key	key{ mem_alloc.memoryTypeIndex, shaderAddress, isImage };

		auto&	page_arr = _pages( key );
		CHECK_ERR( page_arr.size() < page_arr.capacity() );

		auto&	page		= page_arr.emplace_back();
		auto&	mem_props	= dev.GetVProperties().memoryProperties;

		page.dbgCounter.fetch_add( 1 );
		page.memory			= memory.Release();
		page.capacity		= Bytes{mem_alloc.allocationSize};
		page.size			= memSize;
		page.mapped			= mapped_ptr;
		page.propertyFlags	= VkMemoryPropertyFlagBits(mem_props.memoryTypes[ mem_alloc.memoryTypeIndex ].propertyFlags);

		outData.page		= &page;
		outData.offset		= 0_b;
		outData.size		= memSize;

		return true;
	}

/*
=================================================
	Dealloc
=================================================
*/
	bool  VLinearMemAllocator::Dealloc (INOUT Storage_t &data) __NE___
	{
		auto&	mem_data = _CastStorage( data );

		if_unlikely( mem_data.page == null )
			return false;

		EXLOCK( _pageGuard );
		ASSERT( _IsValidPage( mem_data.page ));

		CHECK( mem_data.page->dbgCounter.fetch_sub( 1 ) > 0 );
		return true;
	}

/*
=================================================
	GetInfo
=================================================
*/
	bool  VLinearMemAllocator::GetInfo (const Storage_t &data, OUT VulkanMemoryObjInfo &info) C_NE___
	{
		auto&	mem_data = _CastStorage( data );
		CHECK_ERR( mem_data.page != null );

		SHAREDLOCK( _pageGuard );
		ASSERT( _IsValidPage( mem_data.page ));

		info.memory		= mem_data.page->memory;
		info.flags		= mem_data.page->propertyFlags;
		info.offset		= mem_data.offset;
		info.size		= mem_data.size;
		info.mappedPtr	= mem_data.page->mapped + mem_data.offset;

		return true;
	}

/*
=================================================
	_IsValidPage
=================================================
*/
	bool  VLinearMemAllocator::_IsValidPage (const Page* pagePtr) C_NE___
	{
		//SHAREDLOCK( _pageGuard );	// required before function call

		for (auto [key, pages] : _pages)
		{
			for (auto& page : pages)
			{
				if ( &page == pagePtr )
					return true;
			}
		}
		return false;
	}

/*
=================================================
	GetStatistic
=================================================
*/
	VLinearMemAllocator::Statistic  VLinearMemAllocator::GetStatistic () C_NE___
	{
		SHAREDLOCK( _pageGuard );

		Statistic	result;
		for (auto [key, pages] : _pages)
		{
			for (auto& page : pages)
			{
				result.totalAllocated	+= page.capacity;
				result.totalUsed		+= page.size;
				result.pageCount		++;
				result.refCount			+= uint(Max( 0, page.dbgCounter.load() ));
			}
		}
		return result;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
