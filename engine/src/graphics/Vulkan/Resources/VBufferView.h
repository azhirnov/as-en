// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Vulkan/Resources/VBuffer.h"

namespace AE::Graphics
{

	//
	// Vulkan Buffer view immutable data
	//

	class VBufferView final
	{
	// variables
	private:
		VkBufferView			_bufferView			= Default;
		BufferViewDesc			_desc;
		Strong<BufferID>		_bufferId;
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VBufferView () {}
		~VBufferView ();

		ND_ bool  Create (VResourceManager &, const BufferViewDesc &desc, BufferID bufferId, StringView dbgName);
			void  Destroy (VResourceManager &);
		
		ND_ VkBufferView			Handle ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _bufferView; }
		ND_ BufferViewDesc const&	Description ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }
		ND_ BufferID				Buffer ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _bufferId; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};
	

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
