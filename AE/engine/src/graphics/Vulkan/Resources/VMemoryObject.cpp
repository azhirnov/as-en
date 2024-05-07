// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VMemoryObject.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	VMemoryObject::~VMemoryObject () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( _memAllocator == null );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VMemoryObject::Create (VkBuffer buffer, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _memAllocator == null );
		CHECK_ERR( allocator != null );

		CHECK_ERR( allocator->AllocForBuffer( buffer, desc, OUT _storage ));

		_memAllocator = RVRef(allocator);

		GFX_DBG_ONLY( _debugName = dbgName );
		Unused( dbgName );

		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool  VMemoryObject::Create (VkImage image, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _memAllocator == null );
		CHECK_ERR( allocator != null );

		CHECK_ERR( allocator->AllocForImage( image, desc, OUT _storage ));

		_memAllocator = RVRef(allocator);

		GFX_DBG_ONLY( _debugName = dbgName );
		Unused( dbgName );

		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VMemoryObject::Destroy (VResourceManager &) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _memAllocator )
			CHECK( _memAllocator->Dealloc( INOUT _storage ));

		_memAllocator = null;

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	GetMemoryInfo
=================================================
*/
	bool  VMemoryObject::GetMemoryInfo (OUT VulkanMemoryObjInfo &info) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		CHECK_ERR( _memAllocator != null );
		return _memAllocator->GetInfo( _storage, OUT info );
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
