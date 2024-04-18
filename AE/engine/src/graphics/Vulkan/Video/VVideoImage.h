// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/Video.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/IDs.h"
# include "graphics/Vulkan/VQueue.h"

namespace AE::Graphics
{

	//
	// Vulkan Video Image immutable data
	//

	class VVideoImage final
	{
	// types
	private:
		using VideoStorageArr_t	= IGfxMemAllocator::VideoStorageArr_t;


	// variables
	private:
		VkImage						_image				= Default;
		VkImageView					_view				= Default;

		VideoImageDesc				_desc;
		ushort2						_pictureAccessGranularity;

		Strong<ImageID>				_imageId;
		Strong<ImageViewID>			_viewId;
		Strong<SamplerID>			_ycbcrSampler;

		GfxMemAllocatorPtr			_memAllocator;
		VideoStorageArr_t			_memStorages;

		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VVideoImage ()												__NE___	{}
		~VVideoImage ()												__NE___;

		ND_ bool  Create (VResourceManager &, const VideoImageDesc &,
						  GfxMemAllocatorPtr, StringView dbgName)	__NE___;
			void  Destroy (VResourceManager &)						__NE___;

		ND_ VkImage					GetImageHandle ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _image; }
		ND_ VkImageView				GetViewHandle ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _view; }

		ND_ ImageID					GetImageID ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _imageId; }
		ND_ ImageViewID				GetViewID ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _viewId; }

		ND_ VideoImageDesc const&	Description ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }

		DEBUG_ONLY(  ND_ StringView  GetDebugName ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })


		ND_ static bool  IsSupported (const VResourceManager &, const VideoImageDesc &desc)	__NE___;
		ND_ static bool  Validate (const VDevice &dev, INOUT VideoImageDesc &desc)			__NE___;
		ND_ static bool  Validate (const VDevice &dev, INOUT VideoImageDesc &desc,
								   OUT VkImageCreateInfo &, OUT VkImageViewCreateInfo &,
								   OUT ushort2 &pictureAccessGranularity)					__NE___;

	private:
		ND_ bool  _CreateForYcbcr (VResourceManager &, const VideoImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
		ND_ bool  _CreateForVideo (VResourceManager &, const VideoImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
