// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VBufferView.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VEnumCast.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	VBufferView::~VBufferView () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( _bufferView == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VBufferView::Create (VResourceManager &resMngr, const BufferViewDesc &desc, BufferID bufferId, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _bufferView == Default );

		const VBuffer*	buffer = resMngr.GetResource( bufferId, True{"incRef"} );
		CHECK_ERR( buffer != null );

		_bufferId = Strong<BufferID>{bufferId};
		_canBeDestroyed = true;

		_desc = desc;
		_desc.Validate( buffer->Description() );
		GRES_CHECK( buffer->IsSupported( resMngr, _desc ));

		VkBufferViewCreateInfo	info = {};
		info.sType		= VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
		info.flags		= 0;
		info.buffer		= buffer->Handle();
		info.format		= VEnumCast( _desc.format );
		info.offset		= VkDeviceSize( _desc.offset );
		info.range		= VkDeviceSize( _desc.size );

		auto&	dev = resMngr.GetDevice();
		VK_CHECK_ERR( dev.vkCreateBufferView( dev.GetVkDevice(), &info, null, OUT &_bufferView ));

		dev.SetObjectName( _bufferView, dbgName, VK_OBJECT_TYPE_BUFFER_VIEW );

		DEBUG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool  VBufferView::Create (VResourceManager &resMngr, const VulkanBufferViewDesc &desc, BufferID bufferId, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _bufferView == Default );
		CHECK_ERR( desc.view != Default );

		const VBuffer*	buffer = resMngr.GetResource( bufferId, True{"incRef"} );
		CHECK_ERR( buffer != null );

		_bufferId		= Strong<BufferID>{bufferId};

		_desc.format	= AEEnumCast( desc.format );
		_desc.offset	= desc.offset;
		_desc.size		= desc.range;

		_bufferView		= desc.view;
		_canBeDestroyed	= desc.canBeDestroyed;

		auto&	dev = resMngr.GetDevice();
		dev.SetObjectName( _bufferView, dbgName, VK_OBJECT_TYPE_BUFFER_VIEW );

		DEBUG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VBufferView::Destroy (VResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		auto&	dev = resMngr.GetDevice();

		if ( _canBeDestroyed and _bufferView != Default )
			dev.vkDestroyBufferView( dev.GetVkDevice(), _bufferView, null );

		resMngr.ImmediatelyRelease( INOUT _bufferId );

		_bufferId	= Default;
		_bufferView	= Default;
		_desc		= Default;

		DEBUG_ONLY( _debugName.clear(); )
	}

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
