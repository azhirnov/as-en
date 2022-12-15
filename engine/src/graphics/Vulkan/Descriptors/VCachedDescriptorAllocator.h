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
		VCachedDescriptorAllocator ()											__NE___	{}
		~VCachedDescriptorAllocator ()											__NE_OV;

		bool  Preallocate (ArrayView<Pair< DescriptorSetLayoutID, uint >> info)	__NE___;

		bool  Allocate (DescriptorSetLayoutID layoutId, OUT Storage &ds)		__NE_OV;
		void  Deallocate (DescriptorSetLayoutID layoutId, INOUT Storage &ds)	__NE_OV;

	private:
		static bool  _CreateDSPool (const VDevice &dev, uint descCount, uint maxDS, OUT VkDescriptorPool &dsPool);
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
