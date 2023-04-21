// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Resources/MImage.h"

namespace AE::Graphics
{

	//
	// Metal Image view immutable data
	//

	class MImageView final
	{
	// variables
	private:
		MetalImageRC		_imageView;
		ImageViewDesc		_desc;
		
		Strong<ImageID>		_imageId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MImageView ()									__NE___	{}
		~MImageView ()									__NE___;

		ND_ bool  Create (MResourceManager &, const ImageViewDesc &desc, ImageID imageId, StringView dbgName) __NE___;
			void  Destroy (MResourceManager &)			__NE___;
		
		ND_ MetalImage				Handle ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _imageView; }
		ND_ ImageViewDesc const&	Description ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ ImageID					Image ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _imageId; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};
	

} // AE::Graphics

#endif // AE_ENABLE_METAL
