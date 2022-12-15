// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/ImageDesc.h"
# include "graphics/Public/EResourceState.h"
# include "graphics/Public/IDs.h"
# include "graphics/Vulkan/VQueue.h"
# include "graphics/Public/ResourceManager.h"

namespace AE::Graphics
{

	//
	// Vulkan Image immutable data
	//

	class VImage final
	{
	// variables
	private:
		VkImage					_image			= Default;
		ImageDesc				_desc;
		
		VkImageAspectFlagBits	_aspectMask		= Zero;
		
		Strong<VMemoryID>		_memoryId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VImage ()											__NE___	{}
		~VImage ()											__NE___;

		ND_ bool  Create (VResourceManager &, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)	__NE___;
		ND_ bool  Create (const VResourceManager &dev, const VulkanImageDesc &desc, StringView dbgName)					__NE___;
			void  Destroy (VResourceManager &)																			__NE___;

		ND_ VulkanImageDesc		GetNativeDescription ()		C_NE___;

		ND_ VkImage				Handle ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _image; }
		ND_ VMemoryID			MemoryID ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _memoryId; }
		ND_ ImageDesc const&	Description ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ VkImageAspectFlags	AspectMask ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _aspectMask; }

		ND_ uint3 const			Dimension ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension; }
		ND_ uint const			Width ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.x; }
		ND_ uint const			Height ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.y; }
		ND_ uint const			Depth ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.z; }
		ND_ uint const			ArrayLayers ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.arrayLayers.Get(); }
		ND_ uint const			MipmapLevels ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.maxLevel.Get(); }
		ND_ EPixelFormat		PixelFormat ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.format; }
		ND_ uint const			Samples ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.samples.Get(); }

		ND_ bool				IsExclusiveSharing ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc.queues == Default; }
		
		DEBUG_ONLY( ND_ StringView  GetDebugName ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })


		ND_ static bool	 IsSupported (const VResourceManager &, const ImageDesc &desc)		__NE___;
		ND_ bool		 IsSupported (const VResourceManager &, const ImageViewDesc &desc)	C_NE___;
		
		ND_ static Bytes  GetMemoryAlignment (const VDevice &dev, const ImageDesc &desc)	__NE___;
	};
	

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
