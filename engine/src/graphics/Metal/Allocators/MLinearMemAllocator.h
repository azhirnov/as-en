// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/ResourceManager.h"

#ifdef AE_ENABLE_METAL

# include "graphics/Metal/Allocators/MGfxMemAllocatorUtils.h"

namespace AE::Graphics
{

	//
	// Metal Linear Memory Allocator
	//

	class MLinearMemAllocator final : public IGfxMemAllocator
	{
	// types
	private:
		struct alignas(AE_CACHE_LINE) Page
		{
			Atomic<int>			counter		{0};		// reference counter, for debugging
			MetalMemoryRC		memory;
			Bytes				capacity;
			Bytes				size;

			Page () {}
			Page (Page && other) : memory{ RVRef(other.memory) }, capacity{ other.capacity }, size{ other.size } {}
		};

		struct Data
		{
			Page *		page;
			Bytes		offset;
			Bytes		size;
		};

		using Key		= MGfxMemAllocatorUtils::Key;
		using PageMap_t = FixedMap< Key, FixedArray< Page, 8 >, 6 >;
		
		static constexpr Bytes	_Align {4 << 10};


	// variables
	private:
		mutable SharedMutex		_guard;
		PageMap_t				_pages;
		MDevice const&			_device;
		Bytes					_pageSize;


	// methods
	public:
		explicit MLinearMemAllocator (Bytes pageSize = 0_b);
		~MLinearMemAllocator () override;
		
		void  SetPageSize (Bytes size);
		void  Discard ();

	  // IGfxMemAllocator //
		MetalImageRC   AllocForImage  (const ImageDesc &desc, OUT Storage_t &data) override;
		MetalBufferRC  AllocForBuffer (const BufferDesc &desc, OUT Storage_t &data) override;
		
		MetalAccelStructRC  AllocForAccelStruct (const RTGeometryDesc &desc, OUT Storage_t &data) override;
		MetalAccelStructRC  AllocForAccelStruct (const RTSceneDesc &desc, OUT Storage_t &data) override;

		bool  Dealloc (INOUT Storage_t &data) override;

		bool  GetInfo (const Storage_t &data, OUT MetalMemoryObjInfo &info) const override;
		
		Bytes  MinAlignment ()		const override	{ return 1_b; }
		Bytes  MaxAllocationSize ()	const override	{ return _pageSize; }

	private:
		ND_ static Data &		_CastStorage (Storage_t &data)			{ return *data.Ptr<Data>(); }
		ND_ static Data const&	_CastStorage (const Storage_t &data)	{ return *data.Ptr<Data>(); }
		
		ND_ bool  _IsValidPage (const Page* page) const;
		ND_ bool  _Allocate (Bytes alignment, Bytes size, const Key &key, OUT Data &outData);
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
