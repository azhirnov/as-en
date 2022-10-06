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
		MImageView () {}
		~MImageView ();

		ND_ bool  Create (MResourceManager &, const ImageViewDesc &desc, ImageID imageId, StringView dbgName);
			void  Destroy (MResourceManager &);
		
		ND_ MetalImage				Handle ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _imageView; }
		ND_ ImageViewDesc const&	Description ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ ImageID					Image ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _imageId; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};
	

}	// AE::Graphics

#endif	// AE_ENABLE_METAL
