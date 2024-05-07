// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RBuffer.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Buffer view immutable data
	//

	class RBufferView final
	{
	// variables
	private:
		RmBufferViewID			_viewId;
		BufferViewDesc			_desc;
		bool					_releaseRef	= false;
		Strong<BufferID>		_bufferId;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		RBufferView ()									__NE___	{}
		~RBufferView ()									__NE___;

		ND_ bool  Create (RResourceManager &, const BufferViewDesc &, BufferID, StringView dbgName)	__NE___;
		ND_ bool  Create (RResourceManager &, const RemoteBufferViewDesc &, BufferID, StringView)	__NE___;
			void  Destroy (RResourceManager &)														__NE___;

		ND_ RmBufferViewID			Handle ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _viewId; }

		ND_ BufferViewDesc const&	Description ()		C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ BufferID				BufferId ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _bufferId; }

		GFX_DBG_ONLY( ND_ StringView  GetDebugName ()	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
