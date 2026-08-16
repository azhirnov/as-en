// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/DescriptorSet.h"
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Vulkan/Resources/VDescriptorSetLayout.h"

namespace AE::Graphics
{

	//
	// Vulkan Descriptor Set
	//

	class VDescriptorSet final
	{
	// types
	private:
		using DescrSetStorage_t	= IDescriptorAllocator::Storage;


	// variables
	private:
		DescrSetStorage_t				_descrSet;
		Strong<DescriptorSetLayoutID>	_layoutId;

		DescriptorAllocatorPtr			_allocator;

		GFX_DBG_ONLY( DebugName_t		_debugName;	)


	// methods
	public:
		VDescriptorSet ()												__NE___	{}
		~VDescriptorSet ()												__NE___;

		ND_ bool  Create (ResourceManager &, DescriptorSetLayoutID,
						  const DescSetParams* params,
						  DescriptorAllocatorPtr, StringView dbgName)	__NE___;
			void  Destroy (ResourceManager &)							__NE___;

		ND_ VkDescriptorSet				Handle ()						C_NE___	{ return _descrSet.handle; }
		ND_ DescriptorSetLayoutID		LayoutId ()						C_NE___	{ return _layoutId; }

		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()					C_NE___	{ return _debugName; })

	private:
		bool  _Validate (ResourceManager &, const DescSetParams* params);
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
