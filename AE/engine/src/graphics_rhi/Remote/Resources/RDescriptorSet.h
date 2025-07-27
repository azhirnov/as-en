// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS

# include "graphics_rhi/Public/DescriptorSet.h"
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Remote/Resources/RDescriptorSetLayout.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Descriptor Set
	//

	class RDescriptorSet final
	{
	// variables
	private:
		RmDescriptorSetID				_descSetId;
		Strong<DescriptorSetLayoutID>	_layoutId;

		DescriptorAllocatorPtr			_allocator;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RDescriptorSet ()									__NE___	{}
		~RDescriptorSet ()									__NE___;

		ND_ bool  Create (ResourceManager &, RmDescriptorSetID, DescriptorSetLayoutID, DescriptorAllocatorPtr, StringView dbgName)	__NE___;
			void  Destroy (ResourceManager &)																							__NE___;

		ND_ DescriptorSetLayoutID		LayoutId ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ RmDescriptorSetID			Handle ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _descSetId; }

		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
