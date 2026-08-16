// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/VDevice.h"

namespace AE::Graphics
{

	//
	// Autorelease Memory
	//

	struct VAutoreleaseMemory
	{
	private:
		VkDeviceMemory		_mem	= Default;
		VkBuffer			_buf	= Default;
		VDevice const&		_dev;

	public:
		explicit VAutoreleaseMemory (const VDevice &dev)	__NE___	: _dev{dev}
		{}

		~VAutoreleaseMemory ()								__NE___
		{
			if ( _mem != Default )
				_dev.vkFreeMemory( _dev.GetVkDevice(), _mem, null );

			if ( _buf != Default )
				_dev.vkDestroyBuffer( _dev.GetVkDevice(), _buf, null );
		}

		ND_ VkDeviceMemory		Release ()						__NE___
		{
			CHECK( _mem != Default );
			VkDeviceMemory	res = _mem;
			_mem = Default;
			return res;
		}

		ND_ VkDeviceMemory*		operator & ()				__NE___ { CHECK( _mem == Default );  return &_mem; }
		ND_ VkDeviceMemory		Get ()						C_NE___	{ return _mem; }
		ND_ VkDeviceMemory&		Ref ()						__NE___	{ return _mem; }

		ND_ VkBuffer&			BufferRef ()				__NE___	{ return _buf; }
		ND_ VkBuffer			ReleaseBuffer ()			__NE___
		{
			VkBuffer	res = _buf;
			_buf = Default;
			return res;
		}

	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
