// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Metal/Resources/MBuffer.h"

namespace AE::Graphics
{

	//
	// Metal Buffer view immutable data
	//

	class MBufferView final
	{
	// variables
	private:
		MetalImageRC			_bufferView;
		BufferViewDesc			_desc;
		Strong<BufferID>		_bufferId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		MBufferView () {}
		~MBufferView ();

		ND_ bool  Create (MResourceManager &, const BufferViewDesc &desc, BufferID bufferId, StringView dbgName);
			void  Destroy (MResourceManager &);
		
		ND_ MetalImage				Handle ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _bufferView; }
		ND_ BufferViewDesc const&	Description ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ BufferID				Buffer ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _bufferId; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};
	

}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
