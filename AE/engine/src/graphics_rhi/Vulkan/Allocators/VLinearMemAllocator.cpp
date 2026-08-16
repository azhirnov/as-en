// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Allocators/VLinearMemAllocator.h"
# include "graphics_rhi/Vulkan/Allocators/VAutoreleaseMemory.h"
# include "graphics_rhi/Vulkan/Resources/VBuffer.h"
# include "graphics_rhi/Vulkan/Resources/VImage.h"
# include "graphics_rhi/Vulkan/VRenderTaskScheduler.h"

# define VGFXALLOC	VLinearMemAllocator
# include "graphics_rhi/Vulkan/Allocators/VGfxMemAllocatorUtils.cpp.h"

namespace AE::Graphics
{
namespace
{
	static constexpr Bytes	c_PageAlign			= 4_KiB;
	static constexpr Bytes	c_DefaultPageSize	= 64_MiB;

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
	VLinearMemAllocator::VLinearMemAllocator (Bytes pageSize, Bytes padding) __NE___ :
		_pageSize{ ValidatePageSize( pageSize )},
		_padding{ padding }
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

		DEBUG_ONLY(
			if ( dev._EnableAllocatorStats() )
				_PrintStats();
		)

		for (auto [key, pages] : _pages)
		{
			for (auto& page : pages)
			{
				CHECK( page.dbgCounter.exchange( 0 ) == 0 );

				if ( page.buffer != Default )
					dev.vkDestroyBuffer( dev.GetVkDevice(), page.buffer, null );

				if ( page.mapped != null )
					dev.vkUnmapMemory( dev.GetVkDevice(), page.memory );

				ASSERT( page.memory != Default );
				dev.vkFreeMemory( dev.GetVkDevice(), page.memory, null );
			}
		}
	}

/*
=================================================
	_PrintStats
=================================================
*/
	inline void  VLinearMemAllocator::_PrintStats () C_NE___
	{
		Bytes	capacity, used;
		usize	page_count	= 0;

		for (auto [key, pages] : _pages)
		{
			for (auto& page : pages)
			{
				capacity += page.capacity;
				used	 += page.size;
			}
			page_count += pages.size();
		}

		AE_LOGI( "Vulkan linear allocator stats:\nallocated: "s << ToString(capacity) <<
				 "\nused:      " << ToString(used) << " (" <<
				 ToString(uint(double(ulong{used})*100.0 / double(ulong{capacity}) + 0.5)) << "%)" <<
				 "\npages:     " << ToString(page_count) <<	// - number of allocations
				 "\npageSize:  " << ToString(_pageSize)
				);
	}

/*
=================================================
	Discard
=================================================
*/
	void  VLinearMemAllocator::Discard () __NE___
	{
		EXLOCK( _pageGuard );

		DEBUG_ONLY(
			if ( GraphicsScheduler().GetDevice()._EnableAllocatorStats() )
				_PrintStats();
		)

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
	bool  VLinearMemAllocator::_Allocate (VDevice const& dev, const Bytes memSize, const Bytes memAlign, const uint memBits,
										  const EFlags flags, OUT Data &outData) __NE___
	{
		StaticAssert( uint(EFlags::All) == 0x1F );

		outData = Default;

		// try to allocate in page
		{
			EXLOCK( _pageGuard );

			for (uint type_idx : BitIndexIterate( memBits ))
			{
				const Key	key{ type_idx, flags };

				auto	iter = _pages.find( key );
				if ( iter == _pages.end() )
					continue;

				for (auto& page : iter->second)
				{
					Bytes	offset = AlignUp( page.size + _padding, memAlign );

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
		mem_alloc.pNext			 = AllBits( flags, EFlags::ShaderAddress ) ? &mem_flag : null;
		mem_alloc.allocationSize = VkDeviceSize{ Max( AlignUp( memSize*2, c_PageAlign ), _pageSize )};

		mem_flag.sType			 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
		mem_flag.flags			 = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

		for (uint type_idx : BitIndexIterate( memBits ))
		{
			mem_alloc.memoryTypeIndex = type_idx;

			if_likely( dev.AllocateMemory( mem_alloc, OUT memory.Ref() ) == VK_SUCCESS )
				break;
		}
		CHECK_ERR_MSG( memory.Get() != Default,
			"Failed to allocate memory: "s << ToString(Bytes{mem_alloc.allocationSize}) << ", bits: " << ToString<2>(memBits) );


		// map memory
		void*	mapped_ptr = null;
		if ( AllBits( flags, EFlags::MapMemory ))
		{
			VK_CHECK_ERR( dev.vkMapMemory( dev.GetVkDevice(), memory.Get(), 0, mem_alloc.allocationSize, 0, OUT &mapped_ptr ));
			CHECK_ERR( mapped_ptr != null );
		}

		// create buffer
		if ( AllBits( flags, EFlags::CreateBuffer ))
		{
			CHECK_ERR( VGfxMemAllocatorUtils::CreateStorageBuffer( dev, Bytes{mem_alloc.allocationSize}, memory.Get(), OUT memory.BufferRef() ));
		}

		EXLOCK( _pageGuard );

		const Key	key{ mem_alloc.memoryTypeIndex, flags };

		auto	[it, inserted] = _pages.emplace( key, PageArr_t{} );
		CHECK_ERR_MSG( it, "overflow!" );

		auto&	page_arr	= it->second;
		auto&	page		= page_arr.emplace_back();
		auto&	mem_props	= dev.GetVProperties().memoryProperties;

		page.dbgCounter.fetch_add( 1 );
		page.memory			= memory.Release();
		page.buffer			= memory.ReleaseBuffer();
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
		info.buffer		= mem_data.page->buffer;
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

#undef VGFXALLOC
#endif // AE_ENABLE_VULKAN
