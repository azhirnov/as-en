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
	// Metal Video Buffer immutable data
	//

	class MVideoBuffer final
	{
	// variables
	private:

		VideoBufferDesc		_desc;
		POTBytes			_minOffsetAlign;	// for bitstream
		POTBytes			_minSizeAlign;

		Strong<BufferID>	_bufferId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MVideoBuffer ()													__NE___	{}
		~MVideoBuffer ()												__NE___;

		ND_ bool  Create (MResourceManager &, const VideoBufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)__NE___;
			void  Destroy (MResourceManager &)																				__NE___;

	//	ND_ VkBuffer				Handle ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _buffer; }
		ND_ BufferID				BufferId ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _bufferId; }

		ND_ VideoBufferDesc const&	Description ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ POTBytes				MinBitstreamBufferOffsetAlign ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _minOffsetAlign; }
		ND_ POTBytes				MinBitstreamBufferSizeAlign ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _minSizeAlign; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		ND_ static bool  IsSupported (const MResourceManager &, const VideoBufferDesc &desc) __NE___;
	};
	

} // AE::Graphics

#endif // AE_ENABLE_METAL
