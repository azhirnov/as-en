// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Warning: you can not simultaneously use 'sub-stream' which created from 'stream' and 'stream',
			 because 'sub-stream' modifies offset in 'stream'.
*/

#pragma once

#include "base/Stream/Stream.h"

namespace AE::Base
{

	//
	// Read-only sub stream
	//

	class RSubStream final : public RStream
	{
	// variables
	private:
		RC<RStream>		_stream;
		Bytes			_pos;
		const Bytes		_offset;
		const Bytes		_size;


	// methods
	public:
		RSubStream (const RC<RStream> &stream, Bytes offset, Bytes size);
		
		bool	IsOpen ()	const override;
		Bytes	Position ()	const override	{ return _pos; }
		Bytes	Size ()		const override	{ return _size; }

		bool	SeekSet (Bytes pos) override;
		Bytes	Read2 (OUT void *buffer, Bytes size) override;
	};


}	// AE::Base
