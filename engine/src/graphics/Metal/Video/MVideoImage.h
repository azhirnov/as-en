// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/Video.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/IDs.h"
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics
{

	//
	// Metal Video Image immutable data
	//

	class MVideoImage final
	{
	// variables
	private:
		VideoImageDesc				_desc;
		ushort2						_pictureAccessGranularity;

		Strong<ImageID>				_imageId;
		Strong<ImageViewID>			_viewId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MVideoImage ()										__NE___	{}
		~MVideoImage ()										__NE___;

		ND_ bool  Create (MResourceManager &, const VideoImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)	__NE___;
			void  Destroy (MResourceManager &)																				__NE___;

	//	ND_ VkImage					GetImage ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _image; }
	//	ND_ VkImageView				GetView ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _view; }

		ND_ ImageID					GetImageID ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _imageId; }
		ND_ ImageViewID				GetViewID ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _viewId; }

		ND_ VideoImageDesc const&	Description ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

			
		ND_ static bool  IsSupported (const MResourceManager &, const VideoImageDesc &desc)	__NE___;
		ND_ static bool  Validate (const MDevice &dev, INOUT VideoImageDesc &desc)			__NE___;
	};
	

} // AE::Graphics

#endif // AE_ENABLE_METAL
