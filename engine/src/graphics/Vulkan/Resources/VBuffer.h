// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/BufferDesc.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/IDs.h"
# include "graphics/Vulkan/VQueue.h"

namespace AE::Graphics
{

	//
	// Vulkan Buffer immutable data
	//

	class VBuffer final
	{
	// variables
	private:
		VkBuffer			_buffer			= Default;
		VDeviceAddress		_address		= Default;
		BufferDesc			_desc;

		Strong<VMemoryID>	_memoryId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VBuffer ()										__NE___	{}
		~VBuffer ()										__NE___;

		ND_ bool  Create (VResourceManager &, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)	__NE___;
		ND_ bool  Create (const VResourceManager &, const VulkanBufferDesc &desc, StringView dbgName)					__NE___;
			void  Destroy (VResourceManager &)																			__NE___;
		
		ND_ VulkanBufferDesc	GetNativeDescription () C_NE___;

		ND_ VkBuffer			Handle ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _buffer; }
		ND_ VDeviceAddress		GetDeviceAddress ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  ASSERT( HasDeviceAddress() );  return _address; }
		ND_ VMemoryID			MemoryID ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _memoryId; }

		ND_ BufferDesc const&	Description ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ Bytes				Size ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.size; }
		ND_ bool				IsExclusiveSharing ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.queues == Default; }
		ND_ bool				HasDeviceAddress ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return AllBits( _desc.usage, EBufferUsage::ShaderAddress ); }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		
		ND_ static bool	 IsSupported (const VResourceManager &, const BufferDesc &desc)		__NE___;
		ND_ bool		 IsSupported (const VResourceManager &, const BufferViewDesc &desc)	C_NE___;

		ND_ static Bytes GetMemoryAlignment (const VDevice &dev, const BufferDesc &desc)	__NE___;
		
		ND_ static bool  IsSupportedForVertex (const VResourceManager &, EVertexType type)	__NE___;
		ND_ static bool  IsSupportedForASVertex (const VResourceManager &, EVertexType type)__NE___;

	private:
		ND_ bool  _InitDeviceAddress (const VDevice &dev);
	};
	

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
