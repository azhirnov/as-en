// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Resources/VBufferView.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	VBufferView::~VBufferView () __NE___
	{
		ASSERT( _bufferView == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VBufferView::Create (ResourceManager &resMngr, const BufferViewDesc &desc, BufferID bufferId, StringView dbgName) __NE___
	{
		CHECK_ERR( _bufferView == Default );

		const VBuffer*	buffer = resMngr.GetResource( bufferId, True{"incRef"} );
		CHECK_ERR( buffer != null );

		_bufferId = Strong<BufferID>{bufferId};
		_canBeDestroyed = true;

		const auto	buf_desc = buffer->Description();

		_desc = desc;
		_desc.Validate( buf_desc );
		GRES_CHECK( VBuffer::IsSupported( resMngr, buf_desc, _desc ));

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

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool  VBufferView::Create (ResourceManager &resMngr, const VulkanBufferViewDesc &desc, BufferID bufferId, StringView dbgName) __NE___
	{
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

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VBufferView::Destroy (ResourceManager &resMngr) __NE___
	{
		auto&	dev = resMngr.GetDevice();

		if ( _canBeDestroyed and _bufferView != Default )
			dev.vkDestroyBufferView( dev.GetVkDevice(), _bufferView, null );

		resMngr.ImmediatelyRelease( INOUT _bufferId );

		_bufferId	= Default;
		_bufferView	= Default;
		_desc		= Default;

		GFX_DBG_ONLY( _debugName.clear() );
	}

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
