// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Allocators/VBlockMemAllocator.h"
# include "graphics/Vulkan/Allocators/VAutoreleaseMemory.h"
# include "graphics/Vulkan/Resources/VBuffer.h"
# include "graphics/Vulkan/Resources/VImage.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"

# define VGFXALLOC	VBlockMemAllocator
# include "graphics/Vulkan/Allocators/VGfxMemAllocatorUtils.cpp.h"

namespace AE::Graphics
{
/*
=================================================
	PageArr ctor
=================================================
*/
	VBlockMemAllocator::PageArr::PageArr () __NE___
	{
		hiLevel.store( 0 );	// set empty & unallocated page bit

		for (auto& lvl : lowLevel)
			lvl.store( 0 );	// set empty block bit
	}

/*
=================================================
	constructor
=================================================
*/
	VBlockMemAllocator::VBlockMemAllocator (Bytes blockSize, Bytes pageSize) __NE___ :
		_blockSize{ blockSize },
		_bitsPerPage{ AlignUp( Max( DivCeil( pageSize, blockSize ), 1 ), _BitsPerPageGranularity )}
	{
		CHECK( pageSize >= blockSize );
		CHECK( _bitsPerPage > 0 );
	}

/*
=================================================
	destructor
=================================================
*/
	VBlockMemAllocator::~VBlockMemAllocator () __NE___
	{
		EXLOCK( _pageMapGuard );

		auto&				dev			= GraphicsScheduler().GetDevice();
		const		uint	low_mask	= ToBitMask<uint>( _bitsPerPage );
		constexpr	uint	hi_mask		= ToBitMask<uint>( _PageCount );

		for (auto [key, page_arr] : _pageMap)
		{
			CHECK_MSG( (page_arr.hiLevel.load() & hi_mask) == 0,
					   "one of the pages is completely in use" );

			for (auto& lvl : page_arr.lowLevel)
			{
				CHECK_MSG( (lvl.load() & low_mask) == 0,
						   "one of the blocks is in use" );
			}

			for (auto& page : page_arr.pages)
			{
				if ( page.memory == Default )
					continue;

				if ( page.mapped != null )
					dev.vkUnmapMemory( dev.GetVkDevice(), page.memory );

				ASSERT( page.memory != Default );
				dev.vkFreeMemory( dev.GetVkDevice(), page.memory, null );
			}
		}
	}

/*
=================================================
	_AllocInPage
=================================================
*/
	inline bool  VBlockMemAllocator::_AllocInPage (PageArr &pageArr, OUT Data &outData) C_NE___
	{
		const		uint	low_mask	= ToBitMask<uint>( _bitsPerPage );
		constexpr	uint	hi_mask		= ToBitMask<uint>( _PageCount );

		for (uint try_count = 0; try_count < _HighWaitCount; ++try_count)
		{
			const uint		hi_lvl			= pageArr.hiLevel.load();
			const uint		alloc_page_bits	= (hi_lvl >> 16) & hi_mask;					// 1 - allocated bit
			const uint		empty_page_bits	= (hi_lvl & hi_mask);						// 0 - empty page bit
			Bitfield<uint>	hi_available	{ (~empty_page_bits) & alloc_page_bits };	// 1 - empty & allocated
			int				hi_lvl_idx		= hi_available.ExtractBitIndex();			// first 1 bit

			for (; hi_lvl_idx >= 0 and hi_lvl_idx < int(_PageCount);)
			{
				auto&	low_level		= pageArr.lowLevel [hi_lvl_idx];
				uint	low_available	= low_level.load();								// 0 - empty block bit
				int		low_lvl_idx		= BitScanForward( ~low_available & low_mask );	// first 0 bit

				for (; low_lvl_idx >= 0 and low_lvl_idx < int(_bitsPerPage);)
				{
					const uint	low_lvl_bit = (1u << low_lvl_idx);

					if_likely( low_level.CAS( INOUT low_available, low_available | low_lvl_bit ))	// 0 -> 1
					{
						// update high level
						if_unlikely( low_available == (~low_lvl_bit & low_mask) )
						{
							EXLOCK( pageArr.guard );

							// low level value may be changed at any time so check it inside spinlock
							if ( (low_level.load() & low_mask) == low_mask )
								pageArr.hiLevel.fetch_or( 1u << hi_lvl_idx );	// 0 -> 1
						}

						outData.page		= &pageArr;
						outData.pageIndex	= CheckCast<ushort>(hi_lvl_idx);
						outData.blockIndex	= CheckCast<ushort>(low_lvl_idx);
						return true;
					}

					low_lvl_idx = BitScanForward( ~low_available & low_mask );	// first 0 bit
					ThreadUtils::Pause();
				}

				hi_lvl_idx = hi_available.ExtractBitIndex();		// first 1 bit
			}
			ThreadUtils::Pause();
		}
		return false;
	}

/*
=================================================
	_Allocate
=================================================
*/
	inline bool  VBlockMemAllocator::_Allocate (VDevice const& dev, const Bytes memSize, const Bytes memAlign, const uint memBits,
												const Bool shaderAddress, const Bool isImage, const Bool mapMem, OUT Data &outData) __NE___
	{
		outData = Default;

		CHECK_ERR( memAlign <= _blockSize and memSize <= _blockSize );

		// try to allocate in page
		{
			SHAREDLOCK( _pageMapGuard );

			for (uint type_idx : BitIndexIterate( memBits ))
			{
				const Key	key{ type_idx, shaderAddress, isImage };

				auto	iter = _pageMap.find( key );
				if ( iter == _pageMap.end() )
					continue;

				if ( _AllocInPage( iter->second, OUT outData ))
					return true;
			}
		}

		// create new page
		VkMemoryAllocateInfo		mem_alloc	= {};
		VkMemoryAllocateFlagsInfo	mem_flag	= {};
		VAutoreleaseMemory			memory		{dev};

		mem_alloc.sType			 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mem_alloc.pNext			 = shaderAddress ? &mem_flag : null;
		mem_alloc.allocationSize = VkDeviceSize(_PageSize());

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


		const Key	key{ mem_alloc.memoryTypeIndex, shaderAddress, isImage };
		PageArr*	page_arr;
		{
			EXLOCK( _pageMapGuard );
			page_arr = &_pageMap( key );

			const uint	alloc_page_bits	= page_arr->hiLevel.load() >> 16;		// 1 - allocated
			const int	idx				= BitScanForward( ~alloc_page_bits );	// first 0 bit
			CHECK_ERR( idx >= 0 and idx < int(_PageCount) );

			page_arr->hiLevel.fetch_or( 1u << (idx+16) );	// 0 -> 1 -- allocated page bit

			auto&		page	= page_arr->pages[idx];
			page.memory			= memory.Release();
			page.mapped			= mapped_ptr;
			page.memTypeIndex	= mem_alloc.memoryTypeIndex;
		}

		return _AllocInPage( *page_arr, OUT outData );
	}

/*
=================================================
	Dealloc
=================================================
*/
	bool  VBlockMemAllocator::Dealloc (INOUT Storage_t &data) __NE___
	{
		auto&	mem_data = _CastStorage( data );

		if_unlikely( mem_data.page == null )
			return false;

		SHAREDLOCK( _pageMapGuard );
		ASSERT( _IsValidPage( mem_data.page ));

		auto&			page_arr	= *mem_data.page;
		const uint		hi_lvl_idx	= mem_data.pageIndex;
		const uint		low_lvl_idx	= mem_data.blockIndex;
		auto&			low_level	= page_arr.lowLevel[hi_lvl_idx];

		CHECK_ERR( hi_lvl_idx < _PageCount );
		CHECK_ERR( low_lvl_idx < _bitsPerPage );

		const uint		low_mask	= ToBitMask<uint>( _bitsPerPage );
		const uint		low_bit		= 1u << low_lvl_idx;
		const uint		old_bits	= low_level.fetch_and( ~low_bit );	// 1 -> 0

		if_unlikely( not (old_bits & low_bit) )
		{
			AE_LOGE( "failed to deallocate memory: block is already deallocated" );
			return false;
		}

		// update high level bits
		if_unlikely( old_bits == low_mask )
		{
			EXLOCK( page_arr.guard );

			// low level value may be changed at any time so check it inside spinlock
			if ( low_level.load() != low_mask )
				page_arr.hiLevel.fetch_and( ~(1u << hi_lvl_idx) );	// 1 -> 0
		}
		return true;
	}

/*
=================================================
	_IsValidPage
=================================================
*/
	bool  VBlockMemAllocator::_IsValidPage (const PageArr* pagePtr) C_NE___
	{
		//SHAREDLOCK( _pageMapGuard );	// required before function call

		for (auto [key, page_arr] : _pageMap)
		{
			if ( &page_arr == pagePtr )
				return true;
		}
		return false;
	}

/*
=================================================
	GetInfo
=================================================
*/
	bool  VBlockMemAllocator::GetInfo (const Storage_t &data, OUT VulkanMemoryObjInfo &info) C_NE___
	{
		auto&	dev			= GraphicsScheduler().GetDevice();
		auto&	mem_data	= _CastStorage( data );
		auto&	mem_props	= dev.GetVProperties().memoryProperties;
		CHECK_ERR( mem_data.page != null );

		SHAREDLOCK( _pageMapGuard );
		ASSERT( _IsValidPage( mem_data.page ));

		CHECK_ERR( mem_data.pageIndex < _PageCount );
		CHECK_ERR( mem_data.blockIndex < _bitsPerPage );

		const auto&		page_arr	= *mem_data.page;
		const auto&		page		= page_arr.pages[ mem_data.pageIndex ];

		CHECK_ERR( page.memTypeIndex < mem_props.memoryTypeCount );
		auto&	type = mem_props.memoryTypes[ page.memTypeIndex ];

		info.memory		= page.memory;
		info.flags		= VkMemoryPropertyFlagBits(type.propertyFlags);
		info.offset		= _blockSize * mem_data.blockIndex;
		info.size		= _blockSize;
		info.mappedPtr	= page.mapped + info.offset;

		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
