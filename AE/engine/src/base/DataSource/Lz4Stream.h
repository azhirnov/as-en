// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_LZ4
# include "base/DataSource/DataStream.h"

namespace AE::Base
{

	//
	// Read-only Lz4 Decompression Stream
	//

	class Lz4RStream final : public RStream
	{
	// variables
	private:
		RC<RStream>		_stream;
		void *			_context	= null;		//
		Bytes			_position;				// uncompressed size


	// methods
	public:
		explicit Lz4RStream (RC<RStream> stream)							__NE___;
		~Lz4RStream ()														__NE_OV;

	// RStream //
		bool		IsOpen ()												C_NE_OV	{ return _context != null and _stream and _stream->IsOpen(); }
		PosAndSize	PositionAndSize ()										C_NE_OV	{ return { _position, UMax }; }

		ESourceType	GetSourceType ()										C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::ReadAccess; }

		bool		SeekFwd (Bytes offset)									__NE_OV;
		Bytes		ReadSeq (OUT void* buffer, Bytes size)					__NE_OV;
	};



	//
	// Write-only Lz4 Compression Stream
	//

	class Lz4WStream final : public WStream
	{
	// types
	public:
		struct Config
		{
			bool	hc		= false;	// slow compression
			float	hcLevel	= 0.2f;		// 0..1

			Config () __NE___ {}
		};


	// variables
	private:
		RC<WStream>		_stream;
		void *			_context	= null;		//
		Bytes			_position;				// uncompressed size


	// methods
	public:
		explicit Lz4WStream (RC<WStream>		stream,
							  const Config		&cfg	= Default)			__NE___;
		~Lz4WStream ()														__NE_OV;

		void		SetTotalSize (Bytes size)								__NE___;


	// WStream //
		bool		IsOpen ()												C_NE_OV	{ return _context != null and _stream and _stream->IsOpen(); }
		Bytes		Position ()												C_NE_OV	{ return _position; }
		ESourceType	GetSourceType ()										C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::WriteAccess; }

		Bytes		Reserve (Bytes additionalSize)							__NE_OV;

		bool		SeekFwd (Bytes)											__NE_OV	{ return false; }
		void		Flush ()												__NE_OV	{ _Flush(); }

		Bytes		WriteSeq (const void* buffer, Bytes size)				__NE_OV;

	private:
		bool		_Flush ()												__NE___;
		bool		_End ()													__NE___;
	};



	//
	// LZ4 Utils
	//

	class Lz4Utils final : public Noninstanceable
	{
	public:
		ND_ static bool  Compress (OUT void* dstData, INOUT Bytes &dstSize,
								   const void* srcData, Bytes srcSize,
								   const Lz4WStream::Config &cfg = Default)		__NE___;

		ND_ static bool  Decompress (OUT void* dstData, INOUT Bytes &dstSize,
									 const void* srcData, Bytes srcSize)		__NE___;
	};


} // AE::Base

#endif // AE_ENABLE_LZ4
