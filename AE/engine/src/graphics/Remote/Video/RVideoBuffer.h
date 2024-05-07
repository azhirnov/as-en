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
	// Remote Graphics Video Buffer immutable data
	//

	class RVideoBuffer final
	{
	// variables
	private:
		VideoBufferDesc		_desc;
		POTBytes			_minOffsetAlign;	// for bitstream
		POTBytes			_minSizeAlign;

		Strong<BufferID>	_bufferId;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RVideoBuffer ()													__NE___	{}
		~RVideoBuffer ()												__NE___	{}

		ND_ bool  Create (RResourceManager &, const VideoBufferDesc &, GfxMemAllocatorPtr, StringView)	__NE___	{ return false; }
			void  Destroy (RResourceManager &)															__NE___	{}

		ND_ BufferID				BufferId ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _bufferId; }

		ND_ VideoBufferDesc const&	Description ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ POTBytes				MinBitstreamBufferOffsetAlign ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _minOffsetAlign; }
		ND_ POTBytes				MinBitstreamBufferSizeAlign ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _minSizeAlign; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		ND_ static bool  IsSupported (const RResourceManager &, const VideoBufferDesc &) __NE___ { return false; }
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
