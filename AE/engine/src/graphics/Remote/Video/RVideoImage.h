// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/Video.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/IDs.h"
# include "graphics/Remote/RCommon.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Video Image immutable data
	//

	class RVideoImage final
	{
	// variables
	private:
		RmVideoImageID					_handle;
		RmImageID						_imageHandle;
		VideoImageDesc					_desc;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RVideoImage ()										__NE___	{}
		~RVideoImage ()										__NE___	{}

		ND_ bool  Create (RResourceManager &, const VideoImageDesc &, GfxMemAllocatorPtr, StringView)	__NE___	{ return false; }
			void  Destroy (RResourceManager &)															__NE___	{}

		ND_ RmVideoImageID				Handle ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _handle; }
		ND_ RmImageID					GetImageHandle ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _imageHandle; }
		ND_ ImageID						GetImageID ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  UNTESTED  return Default; }	// TODO
		ND_ VideoImageDesc const&		Description ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }

		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })


		ND_ static bool  IsSupported (const RResourceManager &, const VideoImageDesc &)	__NE___	{ return false; }
		ND_ static bool  Validate (const RDevice &, INOUT VideoImageDesc &)				__NE___	{ return false; }
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
