// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/BufferDesc.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/IDs.h"
# include "graphics/Remote/RCommon.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Video Session immutable data
	//

	class RVideoSession final
	{
	// variables
	private:
		VideoSessionDesc				_desc;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RVideoSession ()								__NE___	{}
		~RVideoSession ()								__NE___	{}

		ND_ bool  Create (RResourceManager &, const VideoSessionDesc &, GfxMemAllocatorPtr, StringView)	__NE___	{ return false; }
			void  Destroy (RResourceManager &)															__NE___	{}

		ND_ VideoSessionDesc const&		Description ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }

		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })


		ND_ static bool  Validate (const RDevice &, INOUT VideoSessionDesc &)				__NE___	{ return false; }
		ND_ static bool  IsSupported (const RResourceManager &, const VideoSessionDesc &)	__NE___	{ return false; }
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
