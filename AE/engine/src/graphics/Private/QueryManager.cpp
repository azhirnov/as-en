// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/QueryManager.h"

namespace AE::Graphics
{

/*
=================================================
	Initialize
=================================================
*/
	bool  IQueryManager::_Initialize (uint maxFrames) __NE___
	{
		_packedIdx.store( PackedIdx{ 0, 1 });
		_maxFrames		= maxFrames+1;

		CHECK_ERR( _maxFrames <= _FrameHistorySize );
		return true;
	}

/*
=================================================
	_NextFrame
=================================================
*/
	auto  IQueryManager::_NextFrame (const FrameUID frameId) __NE___ -> Tuple<uint, PackedIdx>
	{
		const uint			i	= frameId.Remap( uint(_maxFrames-1), _maxFrames );
		const PackedIdx		p	{ WriteIndex( frameId ), ReadIndex( frameId )};
		_packedIdx.store( p );

		return Tuple{ i, p };
	}

/*
=================================================
	WriteIndex / ReadIndex
=================================================
*/
	uint  IQueryManager::WriteIndex (const FrameUID frameId) C_NE___
	{
		return frameId.Remap( 0u, _maxFrames );
	}

	uint  IQueryManager::ReadIndex (const FrameUID frameId) C_NE___
	{
		return frameId.Remap( 1u, _maxFrames );
	}


} // AE::Graphics
