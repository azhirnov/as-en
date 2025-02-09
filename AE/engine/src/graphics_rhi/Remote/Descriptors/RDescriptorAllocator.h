// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Public/DescriptorSet.h"
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Remote/Resources/RDescriptorSetLayout.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Descriptor Allocator
	//

	class RDescriptorAllocator final : public IDescriptorAllocator
	{
	// variables
	private:
		RmDescriptorAllocatorID		_dsAllocId;


	// methods
	public:
		RDescriptorAllocator ()										__NE___ {}
		explicit RDescriptorAllocator (RmDescriptorAllocatorID id)	__NE___ : _dsAllocId{id} {}
		~RDescriptorAllocator ()									__NE_OV;

		ND_ RmDescriptorAllocatorID  Handle ()						C_NE___	{ return _dsAllocId; }

		// IDescriptorAllocator //
		bool  Allocate (DescriptorSetLayoutID, OUT Storage &)		__NE_OV	{ return false; }
		void  Deallocate (DescriptorSetLayoutID, INOUT Storage &)	__NE_OV	{}
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
