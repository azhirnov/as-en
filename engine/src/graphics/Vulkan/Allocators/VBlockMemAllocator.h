// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Allocation on the GPU memory may be slow.
	Internal data is protected by the mutex for thread safety,
	so parallel usage is not recommended because may be too slow.
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/Allocators/VGfxMemAllocatorUtils.h"

namespace AE::Graphics
{

	//
	// Vulkan Fixed Block Memory Allocator
	//

	class VBlockMemAllocator final : public IGfxMemAllocator
	{
	// types
	private:
		static constexpr uint	_BitsPerPageGranularity = 4;
		static constexpr uint	_PageCount				= 8;	// must be < 16
		static constexpr uint	_HighWaitCount			= 2;

		struct Page
		{
			VkDeviceMemory	memory			= Default;
			void*			mapped			= null;
			uint			memTypeIndex	= UMax;
		};
		
		struct PageArr
		{
		  alignas(AE_CACHE_LINE)
			Atomic<uint>	hiLevel;				// 0 - empty page bit, 1 - completely full bit  |  1 - page allocated bit
			SpinLockRelaxed	guard;					// only for 'hiLevel' modification

		  alignas(AE_CACHE_LINE)
			Atomic<uint>	lowLevel [_PageCount];	// 0 - empty block bit, 1 - occupied block bit

			Page			pages [_PageCount];		// write access protected by 'allocated' bit in 'hiLevel'

			PageArr ();
			PageArr (const PageArr &other) : PageArr{} { ASSERT( other.hiLevel.load() == 0 );  Unused( other ); }
		};

		struct Data
		{
			ushort			pageIndex	= UMax;
			ushort			blockIndex	= UMax;
			PageArr *		page		= null;
		};
		
		using Key		= VGfxMemAllocatorUtils::Key;
		using PageMap_t = FixedMap< Key, PageArr, 4 >;


	// variables
	private:
		mutable SharedMutex		_pageMapGuard;
		
		const Bytes				_blockSize;
		const uint				_bitsPerPage;

		PageMap_t				_pageMap;


	// methods
	public:
		VBlockMemAllocator (Bytes blockSize, Bytes pageSize)									__NE___;
		~VBlockMemAllocator ()																	__NE_OV;
		
	  // IGfxMemAllocator //
		bool  AllocForImage (VkImage image, const ImageDesc &desc, OUT Storage_t &data)			__NE_OV;
		bool  AllocForBuffer (VkBuffer buffer, const BufferDesc &desc, OUT Storage_t &data)		__NE_OV;
		bool  AllocForVideoSession (VkVideoSessionKHR, EMemoryType, OUT VideoStorageArr_t &data)__NE_OV;
		bool  AllocForVideoImage (VkImage, const VideoImageDesc &, OUT VideoStorageArr_t &data)	__NE_OV;

		bool  Dealloc (INOUT Storage_t &data)													__NE_OV;
			
		bool  GetInfo (const Storage_t &data, OUT VulkanMemoryObjInfo &info)					C_NE_OV;
		
		Bytes  MinAlignment ()																	C_NE_OV	{ return _blockSize; }
		Bytes  MaxAllocationSize ()																C_NE_OV	{ return _blockSize; }


	private:
		ND_ static Data &		_CastStorage (Storage_t &data)			{ return *data.Ptr<Data>(); }
		ND_ static Data const&	_CastStorage (const Storage_t &data)	{ return *data.Ptr<Data>(); }
		
		ND_ Bytes				_PageSize ()					const	{ return _blockSize * _bitsPerPage; }
		
		ND_ Bytes				_GetOffset (const Data &data)	const	{ return data.blockIndex * _blockSize; }
		ND_ VkDeviceMemory		_GetMemory (const Data &data)	const	{ return data.page->pages[data.pageIndex].memory; }
		
		ND_ bool  _IsValidPage (const PageArr* page) const;
		ND_ bool  _Allocate (const VkMemoryRequirements &memReq, EMemoryType memType, bool shaderAddress, bool isImage, OUT Data &);
		ND_ bool  _AllocInPage (PageArr &pageArr, OUT Data &outData) const;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
