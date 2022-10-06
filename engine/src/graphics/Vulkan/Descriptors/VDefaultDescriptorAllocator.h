// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/DescriptorSet.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/Resources/VDescriptorSetLayout.h"

namespace AE::Graphics
{

	//
	// Vulkan Default Descriptor Allocator
	//

	class VDefaultDescriptorAllocator final : public IDescriptorAllocator
	{
	// types
	private:
		static constexpr uint	PoolsPerChunk			= 8;
		static constexpr uint	MaxDescriptorPoolSize	= 1u << 11;
		static constexpr uint	MaxDescriptorSets		= 1u << 10;
		static constexpr uint	MaxAllocFails			= 2;
		static constexpr uint	MaxChunks				= 1024;

		struct DSPool
		{
			SpinLock			guard;
			uint				allocFails	= 0;
			VkDescriptorPool	handle		= Default;
		};

		struct alignas(AE_CACHE_LINE) Chunk
		{
			StaticArray< DSPool, PoolsPerChunk >	pools;
			Atomic< Chunk *>						next	{null};
		};

		using PoolBits_t = BitSet< PoolsPerChunk >;


	// variables
	private:
		Chunk		_firstChunk;


	// methods
	public:
		VDefaultDescriptorAllocator () {}
		~VDefaultDescriptorAllocator () override;

		bool  Allocate (DescriptorSetLayoutID layoutId, OUT Storage &ds) override;
		void  Deallocate (DescriptorSetLayoutID layoutId, INOUT Storage &ds) override;

	private:
		static bool  _CreateDSPool (const VDevice &dev, uint descCount, uint maxDS, OUT VkDescriptorPool &dsPool);
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
