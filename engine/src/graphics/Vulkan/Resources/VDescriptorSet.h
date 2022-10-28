// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/DescriptorSet.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/Resources/VDescriptorSetLayout.h"

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
		
		DEBUG_ONLY(	DebugName_t			_debugName;	)
		DRC_ONLY(	RWDataRaceCheck		_drCheck;	)


	// methods
	public:
		VDescriptorSet () {}
		~VDescriptorSet ();

		ND_ bool  Create (VResourceManager &, DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator, StringView dbgName);
			void  Destroy (VResourceManager &);

		ND_ VkDescriptorSet			Handle ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _descrSet.handle; }
		ND_ DescriptorSetLayoutID	LayoutID ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		
		DEBUG_ONLY( ND_ StringView  GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};
	

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
