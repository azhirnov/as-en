// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/DescriptorSet.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/Resources/VDescriptorSetLayout.h"

namespace AE::Graphics
{

	//
	// Vulkan Cached Descriptor Allocator
	//

	class VCachedDescriptorAllocator final : public IDescriptorAllocator
	{
	// types
	private:
		struct DSLayoutCache
		{
			Array< VkDescriptorSet >	available;
		};

		using DSLayoutMap_t	= FlatHashMap< DescriptorSetLayoutID, DSLayoutCache >;


	// variables
	private:


	// methods
	public:
		VCachedDescriptorAllocator () {}
		~VCachedDescriptorAllocator () override;

		bool  Preallocate (ArrayView<Pair< DescriptorSetLayoutID, uint >> info);

		bool  Allocate (DescriptorSetLayoutID layoutId, OUT Storage &ds) override;
		void  Deallocate (DescriptorSetLayoutID layoutId, INOUT Storage &ds) override;

	private:
		static bool  _CreateDSPool (const VDevice &dev, uint descCount, uint maxDS, OUT VkDescriptorPool &dsPool);
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
