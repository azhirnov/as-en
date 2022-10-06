// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Public/DescriptorSet.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Metal/Resources/MDescriptorSetLayout.h"

namespace AE::Graphics
{

	//
	// Metal Default Descriptor Allocator
	//

	class MDefaultDescriptorAllocator final : public IDescriptorAllocator
	{
	// methods
	public:
		MDefaultDescriptorAllocator () {}
		~MDefaultDescriptorAllocator () override {}

		bool  Allocate (DescriptorSetLayoutID layoutId, OUT Storage &ds) override;
		void  Deallocate (DescriptorSetLayoutID layoutId, INOUT Storage &ds) override;
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
