// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_ZSTD
# include "base/DataSource/DataStream.h"

namespace AE::Base
{

	//
	// Read-only ZStd Decompression Stream
	//

	class ZStdRStream final : public RStream
	{
	// variables
	private:
		RC<RStream>		_stream;
		void *			_context	= null;		// ZSTD_DCtx
		Bytes			_position;				// uncompressed size

		static constexpr usize	_BufferSize	= 4u << 10;


	// methods
	public:
		explicit ZStdRStream (RC<RStream> stream)							__NE___;
		~ZStdRStream ()														__NE_OV;

	// RStream //
		bool		IsOpen ()												C_NE_OV	{ return _context != null and _stream and _stream->IsOpen(); }
		PosAndSize	PositionAndSize ()										C_NE_OV	{ return { _position, UMax }; }

		ESourceType	GetSourceType ()										C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::ReadAccess; }

		bool		SeekFwd (Bytes offset)									__NE_OV;
		Bytes		ReadSeq (OUT void* buffer, Bytes size)					__NE_OV;
	};



	//
	// Write-only ZStd Compression Stream
	//

	class ZStdWStream final : public WStream
	{
	// types
	public:
		struct Config
		{
			float	level	= 0.2f;		// 0..1

			Config () __NE___ {}
		};


	// variables
	private:
		RC<WStream>		_stream;
		void *			_context	= null;		// ZSTD_CStream
		Bytes			_position;				// uncompressed size

		static constexpr usize	_BufferSize	= 4u << 10;


	// methods
	public:
		explicit ZStdWStream (RC<WStream>		stream,
							  const Config		&cfg	= Default)			__NE___;
		~ZStdWStream ()														__NE_OV;

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
	// ZStd Utils
	//

	class ZStdUtils final : public Noninstanceable
	{
	public:
		ND_ static bool  Compress (OUT void* dstData, INOUT Bytes &dstSize,
								   const void* srcData, Bytes srcSize,
								   const ZStdWStream::Config &cfg = Default)	__NE___;

		ND_ static bool  Decompress (OUT void* dstData, INOUT Bytes &dstSize,
									 const void* srcData, Bytes srcSize)		__NE___;
	};


} // AE::Base

#endif // AE_ENABLE_ZSTD
