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
		VkImageView			_imageView			= Default;
		ImageViewDesc		_desc;
		
		Strong<ImageID>		_imageId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VImageView () {}
		~VImageView ();

		ND_ bool  Create (VResourceManager &, const ImageViewDesc &desc, ImageID imageId, StringView dbgName);
			void  Destroy (VResourceManager &);
		
		ND_ VkImageView				Handle ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _imageView; }
		ND_ ImageViewDesc const&	Description ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ ImageID					Image ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _imageId; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};
	

}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
