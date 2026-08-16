// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Public/Video.h"
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Public/IDs.h"
# include "graphics_rhi/Vulkan/VQueue.h"

namespace AE::Graphics
{

	//
	// Vulkan Video Image immutable data
	//

	class VVideoImage final : private VulkanInstanceFn
	{
	// types
	public:
		struct ValidationParams
		{
			ushort2		pictureAccessGranularity;
		};

	private:
		using VideoStorageArr_t	= IGfxMemAllocator::VideoStorageArr_t;


	// variables
	private:
		VkImage						_image				= Default;		// cached '_imageId' handle
		VkImageView					_view				= Default;		// cached '_viewId' handle

		VideoImageDesc				_desc;
		ushort2						_pictureAccessGranularity;

		VkImageAspectFlagBits		_aspectMask			= Zero;

		Strong<ImageID>				_imageId;
		Strong<ImageViewID>			_viewId;
		Strong<SamplerID>			_ycbcrSampler;

		GfxMemAllocatorPtr			_memAllocator;
		VideoStorageArr_t			_memStorages;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VVideoImage ()												__NE___	{}
		~VVideoImage ()												__NE___;

		ND_ bool  Create (ResourceManager &, const VideoImageDesc &,
						  GfxMemAllocatorPtr, StringView dbgName)	__NE___;
			void  Destroy (ResourceManager &)						__NE___;

		ND_ VkImage					GetImageHandle ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _image; }
		ND_ VkImageView				GetViewHandle ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _view; }

		ND_ ImageID					GetImageID ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _imageId; }
		ND_ ImageViewID				GetViewID ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _viewId; }

		ND_ VideoImageDesc const&	Description ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ VkImageAspectFlags		AspectMask ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _aspectMask; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })


		ND_ static bool  IsSupported (const ResourceManager &, const VideoImageDesc &desc)	__NE___;
		ND_ static bool  Validate (const VDevice &dev, INOUT VideoImageDesc &desc)			__NE___;
		ND_ static bool  Validate (const VDevice &dev, INOUT VideoImageDesc &desc,
								   OUT VkImageViewCreateInfo &, OUT ValidationParams &)		__NE___;

	private:
		ND_ bool  _CreateForYcbcr (ResourceManager &, const VideoImageDesc &, GfxMemAllocatorPtr, StringView dbgName) __NE___;
		ND_ bool  _CreateForVideo (ResourceManager &, const VideoImageDesc &, GfxMemAllocatorPtr, StringView dbgName) __NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
