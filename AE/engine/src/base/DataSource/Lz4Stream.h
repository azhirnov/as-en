// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

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
		void *			_context	= null;		// LZ4F_dctx*
		Bytes			_position;				// uncompressed size

		static constexpr usize	c_ReadBuffer	= 4u << 10;


	// methods
	public:
		explicit Lz4RStream (RC<RStream> stream)							__NE___;
		~Lz4RStream ()														__NE_OV;

	// RStream //
		bool		IsOpen ()												C_NE_OV;
		PosAndSize	PositionAndSize ()										C_NE_OV	{ return { _position, UMax }; }

		ESourceType	GetSourceType ()										C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::ReadOnly; }

		bool		SeekFwd (Bytes offset)									__NE_OV;
		Bytes		ReadSeq (OUT void* buffer, Bytes size)					__NE_OV;

		ReqAlign	DirectAccessAlign ()									C_NE_OV	{ return Default; }
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
			float	level	= 0.2f;		// 0..1

			Config () __NE___ {}
		};


	// variables
	private:
		RC<WStream>		_stream;
		void *			_context			= null;		// LZ4F_cctx*
		Bytes			_position;						// uncompressed size
		int				_compressionLevel	= 0;

		static constexpr usize	c_BufferSize = 4u << 10;


	// methods
	public:
		explicit Lz4WStream (RC<WStream>		stream,
							  const Config		&cfg	= Default)			__NE___;
		~Lz4WStream ()														__NE_OV;

		void		SetTotalSize (Bytes size)								__NE___;


	// WStream //
		bool		IsOpen ()												C_NE_OV	{ return _context != null and _stream and _stream->IsOpen(); }
		Bytes		Position ()												C_NE_OV	{ return _position; }
		ESourceType	GetSourceType ()										C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::WriteOnly; }

		Bytes		Reserve (Bytes additionalSize)							__NE_OV;

		bool		SeekFwd (Bytes)											__NE_OV	{ return false; }
		bool		UpdateAt (Bytes)										__NE_OV	{ return false; }
		void		Flush ()												__NE_OV	{ _Flush(); }

		Bytes		WriteSeq (const void* buffer, Bytes size)				__NE_OV;

		ReqAlign	DirectAccessAlign ()									C_NE_OV	{ return Default; }

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
		ND_ static Bytes  MaxCompressedSize (Bytes nonCompressedSize)				__NE___;

		ND_ static bool  Compress (OUT void* dstData, INOUT Bytes &dstSize,
								   const void* srcData, Bytes srcSize,
								   const Lz4WStream::Config &cfg = Default)			__NE___;

		ND_ static bool  Decompress (OUT void* dstData, INOUT Bytes &dstSize,
									 const void* srcData, Bytes srcSize)			__NE___;

		ND_ static bool  CompressFrame (OUT void* dstData, INOUT Bytes &dstSize,
										const void* srcData, Bytes srcSize,
										const Lz4WStream::Config &cfg = Default)	__NE___;

		ND_ static bool  DecompressFrame (OUT void* dstData, INOUT Bytes &dstSize,
										  const void* srcData, Bytes srcSize)		__NE___;
	};



	//
	// LZ4 Buffered Write Stream
	//

	class Lz4BufferedWStream
	{
	// types
	private:
		using Allocator_t	= UntypedAllocator;

		static constexpr Bytes	c_BlockSize {64_KiB};


	// variables
	private:
		void *		_stream				= null;		// LZ4_stream_t*
		void *		_decompressedBlock	= null;		// temporary memory
		void *		_compressed			= null;
		int			_compressedSize		= 0;
		int			_decompressedSize	= 0;
		int			_capacity			= 0;
		int			_blockSize			= 0;


	// methods
	public:
		Lz4BufferedWStream ()									__NE___ {}
		~Lz4BufferedWStream ()									__NE___	{ Deallocate(); }

		ND_ bool  Allocate (Bytes blockSize = c_BlockSize)		__NE___;
			void  Deallocate ()									__NE___;

		ND_ bool  Write (const void* data, Bytes dataSize)		__NE___;
		ND_ bool  WriteBlock (const void* data, Bytes dataSize)	__NE___;

			void  Flush ()										__NE___;

	//	ND_ ConstMemView  Compressed ()							C_NE___	{ return ConstMemView{ _compressed, _compressedSize }; }
	};


} // AE::Base

#endif // AE_ENABLE_LZ4
