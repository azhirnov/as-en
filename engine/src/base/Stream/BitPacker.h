// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if 0
#pragma once

#include "base/Stream/FastStream.h"

namespace AE::Base
{

	//
	// Bit Packer
	//

	class BitPacker
	{
	// variables
	private:
		uint			_bits		= 0;
		uint			_bitCount	= 0;

		FastWStream		_stream;


	// methods
	public:
		BitPacker (FastWStream &&) = default;

		explicit BitPacker (RC<WStream> stream) : FastWStream{RVRef(stream)} {}
		
		ND_ bool  Empty ()	const	{ return _stream.Empty(); }
		
		template <typename T>
		ND_ bool  Write (const T& value);

		template <typename T>
		bool  WriteBits (const T value, uint bitCount);

	private:
		bool  _FlushBits ();
	};

	

	//
	// Bit Unpacker
	//

	class BitUnpacker
	{
	// variables
	private:
		uint			_bits		= 0;
		uint			_bitCount	= 0;

		FastRStream		_stream;


	// methods
	public:
		BitUnpacker (BitPacker &&) = default;

		explicit BitUnpacker (RC<RStream> stream) : FastRStream{RVRef(stream)} {}

		ND_ bool  Empty ()	const	{ return _stream.Empty(); }
		
		template <typename T>
		ND_ bool  Read (OUT T& value);

		template <typename T>
		ND_ T  ReadBits (uint bitCount);
	};
//-----------------------------------------------------------------------------

	

/*
=================================================
	Write
=================================================
*/
	template <typename T>
	forceinline bool  BitPacker::Write (const T& value)
	{
	}
		
/*
=================================================
	WriteBits
=================================================
*/
	template <typename T>
	forceinline bool  BitPacker::WriteBits (const T value, uint bitCount)
	{
	}
//-----------------------------------------------------------------------------


	
/*
=================================================
	Read
=================================================
*/
	template <typename T>
	forceinline bool  BitUnpacker::Read (OUT T& value)
	{
	}
	
/*
=================================================
	ReadBits
=================================================
*/
	template <typename T>
	forceinline T  BitUnpacker::ReadBits (uint bitCount)
	{
	}


} // AE::Base

#endif
