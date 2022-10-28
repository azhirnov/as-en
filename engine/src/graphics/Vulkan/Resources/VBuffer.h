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
		VBuffer () {}
		~VBuffer ();

		ND_ bool  Create (VResourceManager &, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
		ND_ bool  Create (const VDevice &, const VulkanBufferDesc &desc, StringView dbgName);
			void  Destroy (VResourceManager &);
		
		ND_ VulkanBufferDesc	GetNativeDescription () const;

		ND_ VkBuffer			Handle ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _buffer; }
		ND_ VDeviceAddress		GetDeviceAddress ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _address; }
		ND_ VMemoryID			MemoryID ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _memoryId; }

		ND_ BufferDesc const&	Description ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ Bytes				Size ()					const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.size; }
		ND_ bool				IsExclusiveSharing ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.queues == Default; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		
		ND_ static bool	 IsSupported (const VDevice &dev, const BufferDesc &desc);
		ND_ bool		 IsSupported (const VDevice &dev, const BufferViewDesc &desc) const;

		ND_ static Bytes  GetMemoryAlignment (const VDevice &dev, const BufferDesc &desc);
		
		ND_ static bool  IsSupportedForVertex (const VDevice &dev, EVertexType type);
		ND_ static bool  IsSupportedForASVertex (const VDevice &dev, EVertexType type);
	};
	

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
