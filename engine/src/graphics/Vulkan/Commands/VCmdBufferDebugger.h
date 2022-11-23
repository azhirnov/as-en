// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/CommandBuffer.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"

namespace AE::Graphics
{

	//
	// Vulkan Command Buffer Debugger
	//

	template <typename BaseCmdBuf>
	class VCmdBufferDebugger : public BaseCmdBuf
	{
	// variables
	private:
		Array<String>		_markers;
		uint				_firstId		= 0;

		Strong<BufferID>	_dbgBuffer;					// buffer with host visible memory
		VkBuffer			_dbgVkBuffer	= Default;
		Bytes				_dbgBufferSize;
		Bytes				_dbgBufferOffset;


	// methods
	public:
		template <typename ...ArgTypes>
		explicit VCmdBufferDebugger (ArgTypes&& ...args) : BaseCmdBuf{ FwdArg<ArgTypes>(args)... }
		{
			_markers.reserve( 100 );
		}

		ND_ bool  InitDebugger (Strong<BufferID> bufId, Bytes offset, Bytes size, uint firstId = 0)
		{
			CHECK_ERR( bufId != Default );
			
			_dbgBuffer = RVRef(bufId);

			auto&	buf = this->_mngr.Get( _dbgBuffer );
			CHECK_ERR( offset < buf.Size() );
			CHECK_ERR( offset + size <= buf.Size() );

			_dbgVkBuffer		= buf.Handle();
			_dbgBufferSize		= size;
			_dbgBufferOffset	= offset;
			_firstId			= firstId;

			return true;
		}

		void  FreeDebugger ()
		{
			// TODO
		}

		void  AddMarker (StringView text)
		{
			CHECK_ERRV( _dbgBufferOffset < _dbgBufferSize );

			const uint	id = uint(_markers.size()) + _firstId;
			_markers.emplace_back( text );

			this->FillBuffer( _dbgVkBuffer, _dbgBufferOffset, SizeOf<uint>, id );
			_dbgBufferOffset += SizeOf<uint>;
		}
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
