// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/BufferDesc.h"
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Public/IDs.h"
# include "graphics_rhi/Vulkan/VQueue.h"

namespace AE::Graphics
{

	//
	// Vulkan Buffer immutable data
	//

	class VBuffer final
	{
	// variables
	private:
		VkBuffer					_buffer			= Default;
		DeviceAddress				_address		= Default;
		BufferDesc					_desc;

		Strong<MemoryID>			_memoryId;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VBuffer ()										__NE___	{}
		~VBuffer ()										__NE___;

		ND_ bool  Create (ResourceManager &, const BufferDesc &, GfxMemAllocatorPtr, StringView dbgName)		__NE___;
		ND_ bool  Create (ResourceManager &, const VulkanBufferDesc &, GfxMemAllocatorPtr, StringView dbgName)	__NE___;
			void  Destroy (ResourceManager &)																	__NE___;

		ND_ VulkanBufferDesc	GetNativeDescription () C_NE___;

		ND_ VkBuffer			Handle ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _buffer; }
		ND_ DeviceAddress		GetDeviceAddress ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  ASSERT( _address != Default );  return _address; }
		ND_ MemoryID			MemoryId ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _memoryId; }

		ND_ BufferDesc const&	Description ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ Bytes				Size ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.size; }
		ND_ bool				IsExclusiveSharing ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.queues == Default; }
		ND_ bool				HasDeviceAddress ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _address != Default; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })


		ND_ static bool	 IsSupported (const ResourceManager &, const BufferDesc &desc)		__NE___;
		ND_ static bool	 IsSupported (const ResourceManager &, const BufferDesc &, const BufferViewDesc &)	__NE___;

		ND_ static Bytes GetMemoryAlignment (const VDevice &dev, const BufferDesc &desc)	__NE___;

		ND_ static bool  IsSupportedForVertex (const ResourceManager &, EVertexType type)	__NE___;
		ND_ static bool  IsSupportedForASVertex (const ResourceManager &, EVertexType type)__NE___;

	private:
		ND_ bool  _InitDeviceAddress (const VDevice &dev) __NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
