// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VImage.h"

namespace AE::Graphics
{

	//
	// Vulkan Image view immutable data
	//

	class VImageView final
	{
	// variables
	private:
		VkImageView					_imageView			= Default;
		ImageViewDesc				_desc;
		bool						_canBeDestroyed		= true;

		Strong<ImageID>				_imageId;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VImageView ()									__NE___	{}
		~VImageView ()									__NE___;

		ND_ bool  Create (VResourceManager &, const ImageViewDesc &, ImageID, StringView dbgName)		__NE___;
		ND_ bool  Create (VResourceManager &, const VulkanImageViewDesc &, ImageID, StringView dbgName)	__NE___;
			void  Destroy (VResourceManager &)															__NE___;

		ND_ VkImageView				Handle ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _imageView; }
		ND_ ImageViewDesc const&	Description ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ ImageID					ImageId ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _imageId; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
