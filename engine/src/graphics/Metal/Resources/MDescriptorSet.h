// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Public/DescriptorSet.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Metal/Resources/MDescriptorSetLayout.h"

namespace AE::Graphics
{

	//
	// Metal Descriptor Set
	//

	class MDescriptorSet final
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
		MDescriptorSet () {}
		~MDescriptorSet ();

		ND_ bool  Create (MResourceManager &, DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator, StringView dbgName);
			void  Destroy (MResourceManager &);

		ND_ MetalBuffer				Handle ()			const	{ DRC_SHAREDLOCK( _drCheck );  return IsArgumentBuffer() ? _descrSet.handle : Default; }
		ND_ DescriptorSetLayoutID	LayoutID ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ bool					IsArgumentBuffer ()	const;	// TODO
		
		DEBUG_ONLY( ND_ StringView  GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};
	

} // AE::Graphics

#endif // AE_ENABLE_METAL
