// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RImage.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Image view immutable data
	//

	class RImageView final
	{
	// variables
	private:
		RmImageViewID		_viewId;
		ImageViewDesc		_desc;
		bool				_releaseRef	= false;
		Strong<ImageID>		_imageId;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RImageView ()									__NE___	{}
		~RImageView ()									__NE___;

		ND_ bool  Create (RResourceManager &, const ImageViewDesc &, ImageID, StringView dbgName)	__NE___;
		ND_ bool  Create (RResourceManager &, const RemoteImageViewDesc &, ImageID, StringView)		__NE___;
			void  Destroy (RResourceManager &)														__NE___;

		ND_ RmImageViewID			Handle ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _viewId; }

		ND_ ImageViewDesc const&	Description ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ ImageID					ImageId ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _imageId; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
