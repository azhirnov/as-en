// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::ResEditor
{

	//
	// Content Version
	//

	struct ContentVersion
	{
	// variables
	private:
		Atomic<ulong>	_ver [2] = {};


	// methods
	public:
		ContentVersion () {}

		ND_ ulong	Get (uint frameId)		const	{ return _ver[frameId].load(); }
		ND_ ulong	Get (FrameUID frameId)	const	{ return Get( frameId.Remap2() ); }

			void	Update (FrameUID frameId);
	};


	inline void  ContentVersion::Update (FrameUID frameId)
	{
		const uint	i0	= frameId.Remap2( 0 );
		const uint	i1	= frameId.Remap2( 1 );

		_ver[i1].store( Max( _ver[i0].load(), _ver[i1].load() ) + 1 );
	}


} // AE::ResEditor
