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
		VImage () {}
		~VImage ();

		ND_ bool  Create (VResourceManager &, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
		ND_ bool  Create (const VDevice &dev, const VulkanImageDesc &desc, StringView dbgName);
			void  Destroy (VResourceManager &);

		ND_ VulkanImageDesc		GetNativeDescription () const;

		ND_ VkImage				Handle ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _image; }
		ND_ VMemoryID			MemoryID ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _memoryId; }
		ND_ ImageDesc const&	Description ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ VkImageAspectFlags	AspectMask ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _aspectMask; }

		ND_ uint3 const			Dimension ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension; }
		ND_ uint const			Width ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.x; }
		ND_ uint const			Height ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.y; }
		ND_ uint const			Depth ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.dimension.z; }
		ND_ uint const			ArrayLayers ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.arrayLayers.Get(); }
		ND_ uint const			MipmapLevels ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.maxLevel.Get(); }
		ND_ EPixelFormat		PixelFormat ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.format; }
		ND_ uint const			Samples ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.samples.Get(); }

		ND_ bool				IsExclusiveSharing ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _desc.queues == Default; }
		
		DEBUG_ONLY( ND_ StringView  GetDebugName ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })


		ND_ static bool	 IsSupported (const VDevice &dev, const ImageDesc &desc);
		ND_ bool		 IsSupported (const VDevice &dev, const ImageViewDesc &desc) const;
		
		ND_ static Bytes  GetMemoryAlignment (const VDevice &dev, const ImageDesc &desc);
	};
	

}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
