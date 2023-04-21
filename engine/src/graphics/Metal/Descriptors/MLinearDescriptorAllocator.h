// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/DescriptorSet.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Metal/Resources/MDescriptorSetLayout.h"

namespace AE::Graphics
{

	//
	// Metal Linear Descriptor Allocator
	//

	class MLinearDescriptorAllocator final : public IDescriptorAllocator
	{
	// methods
	public:
		MLinearDescriptorAllocator ()										__NE___ {}
		~MLinearDescriptorAllocator ()										__NE_OV {}

		bool  Allocate (DescriptorSetLayoutID layoutId, OUT Storage &ds)	__NE_OV;
		void  Deallocate (DescriptorSetLayoutID layoutId, INOUT Storage &ds)__NE_OV;
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
