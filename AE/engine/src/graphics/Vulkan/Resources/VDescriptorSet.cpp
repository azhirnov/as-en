// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VDescriptorSet.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VEnumCast.h"

namespace AE::Graphics
{
/*
=================================================
	destructor
=================================================
*/
	VDescriptorSet::~VDescriptorSet () __NE___
	{
		CHECK( _descrSet.handle == Default );
		CHECK( _allocator == null );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VDescriptorSet::Create (VResourceManager &resMngr, DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );

		CHECK_ERR( allocator != null );
		CHECK_ERR( layoutId != Default );

		CHECK_ERR( _descrSet.handle == Default );
		CHECK_ERR( _allocator == null );

		_layoutId = resMngr.AcquireResource( layoutId );
		CHECK_ERR( _layoutId );

		if_unlikely( not allocator->Allocate( _layoutId, OUT _descrSet ))
		{
			resMngr.ImmediatelyRelease( _layoutId );
			RETURN_ERR( "descriptor set allocation failed" );
		}

		resMngr.GetDevice().SetObjectName( _descrSet.handle, dbgName, VK_OBJECT_TYPE_DESCRIPTOR_SET );

		_allocator = RVRef(allocator);

		DEBUG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VDescriptorSet::Destroy (VResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( (_allocator != null) and (_descrSet.handle != Default) )
		{
			_allocator->Deallocate( _layoutId, INOUT _descrSet );
		}

		resMngr.ImmediatelyRelease( INOUT _layoutId );

		_allocator	= null;
		_layoutId	= Default;

		CHECK( _descrSet.handle == Default );

		DEBUG_ONLY( _debugName.clear(); )
	}

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
