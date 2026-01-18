// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	using ESourceType = IDataSource::ESourceType;


	ND_ static Array<ubyte>  GenRandomArray (Bytes size)
	{
		Array<ubyte>	temp;
		temp.resize( usize(size) );

		Base::Random	rnd;
		for (usize i = 0; i < temp.size(); ++i)
		{
			temp[i] = rnd.Uniform<ubyte>() & 0xF;
		}
		return temp;
	}


#ifdef AE_ENABLE_BROTLI
	static void  BrotliStream_Test1 ()
	{
		const auto		uncompressed = GenRandomArray( 1_MiB );
		Array<ubyte>	file_data;

		// compress
		{
			BrotliWStream::Config	cfg;
			cfg.inBlockSize	= 1.0f;
			cfg.quality		= 1.0f;
			cfg.windowBits	= 1.0f;

			auto			stream = MakeRC<ArrayWStream>();
			BrotliWStream	encoder{ stream, cfg };

			TEST( encoder.IsOpen() );
			TEST( encoder.Write( ArrayView<ubyte>{uncompressed} ));
			encoder.Flush();

			file_data = stream->ReleaseData();
		}

		const Bytes	compressed_size {file_data.size()};
		TEST( compressed_size < uncompressed.size() );

		// uncompress
		{
			BrotliRStream	decoder{ MakeRC<ArrayRStream>( RVRef(file_data) )};
			Array<ubyte>	data2, data3;

			TEST( decoder.IsOpen() );
			TEST( decoder.Read( uncompressed.size() / 2, OUT data2 ));
			TEST( decoder.Read( uncompressed.size() - data2.size(), OUT data3 ));
			TEST_Eq( decoder.Position(), compressed_size );

			TEST_Eq( uncompressed.size(), (data2.size() + data3.size()) );
			TEST( ArrayView<ubyte>{uncompressed}.section( 0, data2.size() ) == data2 );
			TEST( ArrayView<ubyte>{uncompressed}.section( data2.size(), data3.size() ) == data3 );
		}
	}


	static void  BrotliStream_Test2 ()
	{
		const auto		uncompressed = GenRandomArray( 2_MiB );
		const Bytes		block_size	 = 1_KiB;
		Array<ubyte>	file_data;

		// compress
		{
			BrotliWStream::Config	cfg;
			cfg.inBlockSize	= 0.2f;
			cfg.quality		= 0.5f;
			cfg.windowBits	= 0.2f;

			auto			stream = MakeRC<ArrayWStream>();
			BrotliWStream	encoder{ stream, cfg };

			TEST( encoder.IsOpen() );
			for (Bytes pos, size = ArraySizeOf(uncompressed); pos < size;)
			{
				Bytes	wr_size	= Min( block_size, size - pos );
				Bytes	written = encoder.WriteSeq( uncompressed.data() + pos, wr_size );

				TEST( written > 0 );
				pos += written;
			}
			encoder.Flush();

			file_data = stream->ReleaseData();
		}

		const Bytes	compressed_size {file_data.size()};
		TEST( compressed_size < uncompressed.size() );

		// uncompress
		{
			BrotliRStream	decoder{ MakeRC<ArrayRStream>( RVRef(file_data) )};
			Array<ubyte>	data2, data3;

			TEST( decoder.IsOpen() );
			TEST( decoder.Read( uncompressed.size() / 2, OUT data2 ));
			TEST( decoder.Read( uncompressed.size() - data2.size(), OUT data3 ));
			TEST_Eq( decoder.Position(), compressed_size );

			TEST_Eq( uncompressed.size(), (data2.size() + data3.size()) );
			TEST( ArrayView<ubyte>{uncompressed}.section( 0, data2.size() ) == data2 );
			TEST( ArrayView<ubyte>{uncompressed}.section( data2.size(), data3.size() ) == data3 );
		}
	}


	static void  BrotliStream_Test3 ()
	{
		const auto		uncompressed = GenRandomArray( 2_MiB );
		Array<ubyte>	file_data;

		// compress
		{
			BrotliWStream::Config	cfg;
			cfg.inBlockSize	= 1.0f;
			cfg.quality		= 1.0f;
			cfg.windowBits	= 1.0f;

			auto			stream = MakeRC<ArrayWStream>();
			BrotliWStream	encoder{ stream, cfg };

			TEST( encoder.IsOpen() );
			TEST( encoder.Write( ArrayView<ubyte>{uncompressed} ));
			encoder.Flush();

			file_data = stream->ReleaseData();
		}

		const Bytes	compressed_size {file_data.size()};

		// uncompress
		{
			BrotliRStream	decoder{ MakeRC<ArrayRStream>( RVRef(file_data) )};
			TEST( decoder.IsOpen() );

			ArrayWStream	dst_mem;
			const Bytes		size = DataSourceUtils::BufferedCopy( dst_mem, decoder );

			TEST_Eq( size, ArraySizeOf(uncompressed) );
			TEST( dst_mem.GetData() == uncompressed );
		}
	}


	static void  BrotliStream_Test4 ()
	{
		const auto		uncompressed = GenRandomArray( 2_MiB );
		Array<ubyte>	file_data;

		// compress
		{
			file_data.resize( uncompressed.size() );

			Bytes	size = ArraySizeOf(file_data);
			TEST( BrotliUtils::Compress( OUT file_data.data(), INOUT size, uncompressed.data(), ArraySizeOf(uncompressed) ));

			file_data.resize( usize{size} );
		}

		const Bytes	compressed_size {file_data.size()};

		// decompress v1
		{
			Array<ubyte>	decoded;
			decoded.resize( uncompressed.size() + 1024 );

			Bytes	dec_size = ArraySizeOf(decoded);

			TEST( BrotliUtils::Decompress( OUT decoded.data(), INOUT dec_size, file_data.data(), ArraySizeOf(file_data) ));

			TEST_Eq( dec_size, ArraySizeOf(uncompressed) );
			TEST( ArrayView{decoded}.section( 0, usize{dec_size} ) == uncompressed );
		}

		// decompress v2
		{
			BrotliRStream		decoder{ MakeRC<ArrayRStream>( RVRef(file_data) )};
			TEST( decoder.IsOpen() );

			ArrayWStream	dst_mem;
			const Bytes		size = DataSourceUtils::BufferedCopy( dst_mem, decoder );

			TEST_Eq( size, ArraySizeOf(uncompressed) );
			TEST( dst_mem.GetData() == uncompressed );
		}
	}
#endif // AE_ENABLE_BROTLI


#ifdef AE_ENABLE_ZSTD
	static void  ZStdStream_Test1 ()
	{
		const auto		uncompressed = GenRandomArray( 1_MiB );
		Array<ubyte>	file_data;

		// compress
		{
			auto	stream = MakeRC<ArrayWStream>();
			{
				ZStdWStream::Config	cfg;
				ZStdWStream			encoder{ stream, cfg };

				TEST( encoder.IsOpen() );
				TEST( encoder.Write( ArrayView<ubyte>{uncompressed} ));
				encoder.Flush();
			}
			file_data = stream->ReleaseData();
		}

		const Bytes	compressed_size {file_data.size()};
		TEST( compressed_size < uncompressed.size() );

		// uncompress
		{
			ZStdRStream		decoder{ MakeRC<ArrayRStream>( RVRef(file_data) )};
			Array<ubyte>	data2, data3;

			TEST( decoder.IsOpen() );
			TEST( decoder.Read( uncompressed.size() / 2, OUT data2 ));
			TEST( decoder.Read( uncompressed.size() - data2.size(), OUT data3 ));
			TEST_Eq( decoder.Position(), compressed_size );

			TEST_Eq( uncompressed.size(), (data2.size() + data3.size()) );
			TEST( ArrayView<ubyte>{uncompressed}.section( 0, data2.size() ) == data2 );
			TEST( ArrayView<ubyte>{uncompressed}.section( data2.size(), data3.size() ) == data3 );
		}
	}


	static void  ZStdStream_Test2 ()
	{
		const auto		uncompressed = GenRandomArray( 2_MiB );
		const Bytes		block_size	 = 1_KiB;
		Array<ubyte>	file_data;

		// compress
		{
			ZStdWStream::Config	cfg;

			auto			stream = MakeRC<ArrayWStream>();
			ZStdWStream		encoder{ stream, cfg };

			TEST( encoder.IsOpen() );
			for (Bytes pos, size = ArraySizeOf(uncompressed); pos < size;)
			{
				Bytes	wr_size	= Min( block_size, size - pos );
				Bytes	written = encoder.WriteSeq( uncompressed.data() + pos, wr_size );

				TEST( written > 0 );
				pos += written;
			}
			encoder.Flush();

			file_data = stream->ReleaseData();
		}

		const Bytes	compressed_size {file_data.size()};
		TEST( compressed_size < uncompressed.size() );

		// uncompress
		{
			ZStdRStream		decoder{ MakeRC<ArrayRStream>( RVRef(file_data) )};
			Array<ubyte>	data2, data3;

			TEST( decoder.IsOpen() );
			TEST( decoder.Read( uncompressed.size() / 2, OUT data2 ));
			TEST( decoder.Read( uncompressed.size() - data2.size(), OUT data3 ));
			TEST_Eq( decoder.Position(), compressed_size );

			TEST_Eq( uncompressed.size(), (data2.size() + data3.size()) );
			TEST( ArrayView<ubyte>{uncompressed}.section( 0, data2.size() ) == data2 );
			TEST( ArrayView<ubyte>{uncompressed}.section( data2.size(), data3.size() ) == data3 );
		}
	}


	static void  ZStdStream_Test3 ()
	{
		const auto		uncompressed = GenRandomArray( 2_MiB );
		Array<ubyte>	file_data;

		// compress
		{
			auto	stream = MakeRC<ArrayWStream>();
			{
				ZStdWStream		encoder{ stream };

				TEST( encoder.IsOpen() );
				TEST( encoder.Write( ArrayView<ubyte>{uncompressed} ));
				encoder.Flush();
			}
			file_data = stream->ReleaseData();
		}

		const Bytes	compressed_size {file_data.size()};

		// uncompress
		{
			ZStdRStream		decoder{ MakeRC<ArrayRStream>( RVRef(file_data) )};
			TEST( decoder.IsOpen() );

			ArrayWStream	dst_mem;
			const Bytes		size = DataSourceUtils::BufferedCopy( dst_mem, decoder );

			TEST_Eq( size, ArraySizeOf(uncompressed) );
			TEST( dst_mem.GetData() == uncompressed );
		}
	}


	static void  ZStdStream_Test4 ()
	{
		const auto		uncompressed = GenRandomArray( 2_MiB );
		Array<ubyte>	file_data;

		// compress
		{
			file_data.resize( uncompressed.size() );

			Bytes	size = ArraySizeOf(file_data);
			TEST( ZStdUtils::Compress( OUT file_data.data(), INOUT size, uncompressed.data(), ArraySizeOf(uncompressed) ));

			file_data.resize( usize{size} );
		}

		const Bytes	compressed_size {file_data.size()};

		// decompress v1
		{
			Array<ubyte>	decoded;
			decoded.resize( uncompressed.size() + 1024 );

			Bytes	dec_size = ArraySizeOf(decoded);

			TEST( ZStdUtils::Decompress( OUT decoded.data(), INOUT dec_size, file_data.data(), ArraySizeOf(file_data) ));

			TEST_Eq( dec_size, ArraySizeOf(uncompressed) );
			TEST( ArrayView{decoded}.section( 0, usize{dec_size} ) == uncompressed );
		}

		// decompress v2
		{
			ZStdRStream		decoder{ MakeRC<ArrayRStream>( RVRef(file_data) )};
			TEST( decoder.IsOpen() );

			ArrayWStream	dst_mem;
			const Bytes		size = DataSourceUtils::BufferedCopy( dst_mem, decoder );

			TEST_Eq( size, ArraySizeOf(uncompressed) );
			TEST( dst_mem.GetData() == uncompressed );
		}
	}
#endif // AE_ENABLE_ZSTD


#ifdef AE_ENABLE_LZ4
	static void  Lz4Stream_Test1 ()
	{
		for (uint i = 0; i < 2; ++i)
		{
			const auto		uncompressed = GenRandomArray( 1_MiB );
			Array<ubyte>	file_data;

			// compress
			{
				auto	stream = MakeRC<ArrayWStream>();
				{
					Lz4WStream::Config	cfg;	cfg.hc = (i == 1);
					Lz4WStream			encoder{ stream, cfg };

					TEST( encoder.IsOpen() );
					TEST( encoder.Write( ArrayView<ubyte>{uncompressed} ));
				}
				file_data = stream->ReleaseData();
			}

			const Bytes	compressed_size {file_data.size()};
			TEST( compressed_size < uncompressed.size() );

			// uncompress
			{
				Lz4RStream		decoder{ MakeRC<ArrayRStream>( RVRef(file_data) )};
				Array<ubyte>	data2, data3;

				TEST( decoder.IsOpen() );
				TEST( decoder.Read( uncompressed.size() / 2, OUT data2 ));
				TEST( decoder.Read( uncompressed.size() - data2.size(), OUT data3 ));
				TEST_Eq( decoder.Position(), compressed_size );

				TEST_Eq( uncompressed.size(), (data2.size() + data3.size()) );
				TEST( ArrayView<ubyte>{uncompressed}.section( 0, data2.size() ) == data2 );
				TEST( ArrayView<ubyte>{uncompressed}.section( data2.size(), data3.size() ) == data3 );
			}
		}
	}


	static void  Lz4Stream_Test2 ()
	{
		const auto		uncompressed = GenRandomArray( 2_MiB );
		const Bytes		block_size	 = 64_KiB;
		Array<ubyte>	file_data;

		// compress
		{
			auto	stream = MakeRC<ArrayWStream>();
			{
				Lz4WStream::Config	cfg;
				Lz4WStream			encoder{ stream, cfg };

				TEST( encoder.IsOpen() );
				for (Bytes pos, size = ArraySizeOf(uncompressed); pos < size;)
				{
					Bytes	wr_size	= Min( block_size, size - pos );
					Bytes	written = encoder.WriteSeq( uncompressed.data() + pos, wr_size );

					TEST( written > 0 );
					pos += written;
				}
			}
			file_data = stream->ReleaseData();
		}

		const Bytes	compressed_size {file_data.size()};
		TEST( compressed_size < uncompressed.size() );

		// uncompress
		{
			Lz4RStream		decoder{ MakeRC<ArrayRStream>( RVRef(file_data) )};
			Array<ubyte>	data2, data3;

			TEST( decoder.IsOpen() );
			TEST( decoder.Read( uncompressed.size() / 2, OUT data2 ));
			TEST( decoder.Read( uncompressed.size() - data2.size(), OUT data3 ));
			TEST_Eq( decoder.Position(), compressed_size );

			TEST_Eq( uncompressed.size(), (data2.size() + data3.size()) );
			TEST( ArrayView<ubyte>{uncompressed}.section( 0, data2.size() ) == data2 );
			TEST( ArrayView<ubyte>{uncompressed}.section( data2.size(), data3.size() ) == data3 );
		}
	}


	static void  Lz4Stream_Test3 ()
	{
		const auto		uncompressed = GenRandomArray( 2_MiB );
		Array<ubyte>	file_data;

		// compress
		{
			auto	stream = MakeRC<ArrayWStream>();
			{
				Lz4WStream		encoder{ stream };

				TEST( encoder.IsOpen() );
				TEST( encoder.Write( ArrayView<ubyte>{uncompressed} ));
			}
			file_data = stream->ReleaseData();
		}

		const Bytes	compressed_size {file_data.size()};

		// uncompress
		{
			Lz4RStream		decoder{ MakeRC<ArrayRStream>( RVRef(file_data) )};
			TEST( decoder.IsOpen() );

			Array<char>		buffer;
			buffer.resize( usize{ 64_KiB });

			DataSourceUtils::TempBuffer	buf{ buffer.data(), ArraySizeOf(buffer) };

			ArrayWStream	dst_mem;
			const Bytes		size = DataSourceUtils::BufferedCopy( dst_mem, decoder, buf );

			TEST_Eq( size, ArraySizeOf(uncompressed) );
			TEST( dst_mem.GetData() == uncompressed );
		}
	}


	static void  Lz4Stream_Test4 ()
	{
		const auto		uncompressed = GenRandomArray( 2_MiB );
		Array<ubyte>	file_data;

		// compress
		{
			file_data.resize( usize{Lz4Utils::MaxCompressedSize( 2_MiB )} );

			Bytes	size = ArraySizeOf(file_data);
			TEST( Lz4Utils::CompressFrame( OUT file_data.data(), INOUT size, uncompressed.data(), ArraySizeOf(uncompressed) ));

			file_data.resize( usize{size} );
		}

		const Bytes	compressed_size {file_data.size()};

		// decompress v1
		{
			Array<ubyte>	decoded;
			decoded.resize( uncompressed.size() );

			Bytes	dec_size = ArraySizeOf(decoded);

			TEST( Lz4Utils::DecompressFrame( OUT decoded.data(), INOUT dec_size, file_data.data(), ArraySizeOf(file_data) ));

			TEST_Eq( dec_size, ArraySizeOf(uncompressed) );
			TEST( ArrayView{decoded}.section( 0, usize{dec_size} ) == uncompressed );
		}

		// decompress v2
		{
			Lz4RStream		decoder{ MakeRC<ArrayRStream>( RVRef(file_data) )};
			TEST( decoder.IsOpen() );

			ArrayWStream	dst_mem;
			const Bytes		size = DataSourceUtils::BufferedCopy( dst_mem, decoder );

			TEST_Eq( size, ArraySizeOf(uncompressed) );
			TEST( dst_mem.GetData() == uncompressed );
		}
	}


	static void  Lz4Stream_Test5 ()
	{
		const auto		uncompressed = GenRandomArray( 2_MiB );
		Array<ubyte>	file_data;

		// compress
		{
			file_data.resize( usize{Lz4Utils::MaxCompressedSize( 2_MiB )} );

			Bytes	size = ArraySizeOf(file_data);
			TEST( Lz4Utils::Compress( OUT file_data.data(), INOUT size, uncompressed.data(), ArraySizeOf(uncompressed) ));

			file_data.resize( usize{size} );
		}

		const Bytes	compressed_size {file_data.size()};

		// decompress v1
		{
			Array<ubyte>	decoded;
			decoded.resize( uncompressed.size() );

			Bytes	dec_size = ArraySizeOf(decoded);

			TEST( Lz4Utils::Decompress( OUT decoded.data(), INOUT dec_size, file_data.data(), ArraySizeOf(file_data) ));

			TEST_Eq( dec_size, ArraySizeOf(uncompressed) );
			TEST( ArrayView{decoded}.section( 0, usize{dec_size} ) == uncompressed );
		}

		// not compatible with Lz4RStream
	}
#endif // AE_ENABLE_LZ4


	static void  StdStream_Test1 ()
	{
		String			src_str = "4324356uytsdgfh1243rttrasfdg";
		RC<RStream>		mem = MakeRC<MemRefRStream>( src_str );

		StreambufWrap<char>	streambuf{ mem };
		std::istream		stream{ &streambuf };

		String				dst_str;
		dst_str.resize( src_str.length() );

		stream.read( dst_str.data(), dst_str.length() );
		TEST( src_str == dst_str );

		// read again
		dst_str.clear();
		dst_str.resize( src_str.length() );

		stream.seekg( 0, std::ios_base::beg );
		stream.read( dst_str.data(), dst_str.length() );
		TEST( src_str == dst_str );
	}


	static void  BufferedStream_Test1 ()
	{
		const uint	count	= 10'000;
		auto		stream1	= MakeRC<ArrayWStream>();
		auto		stream2	= MakeRC<BufferedWStream>( stream1, 128_b );

		for (uint i = 0; i < count; ++i)
			TEST( stream2->Write( i ));

		stream2->Flush();
		ArrayView<uint>	mem = stream1->GetData().Cast<uint>();

		for (uint i = 0; i < count; ++i)
			TEST( mem[i] == i );

		RC<MemRefRStream>	stream3 = MakeRC<MemRefRStream>( mem );
		RC<RStream>			stream4 = MakeRC<BufferedRStream>( stream3 );

		for (uint i = 0; i < count; ++i)
		{
			uint	j = 0;
			TEST( stream4->Read( OUT j ));
			TEST_Eq( i, j );
		}
	}


	static void  FastStream_Test1 ()
	{
		const uint	count	= 10'000;
		auto		stream1	= MakeRC<ArrayWStream>();

		for (uint i = 0; i < count; ++i)
			TEST( stream1->Write( i ));

		auto		stream2	= MakeRC<MemRefRStream>( stream1->GetData() );

		const void*	begin1	= null;
		const void*	end1	= null;
		stream2->UpdateFastStream( OUT begin1, OUT end1 );
		TEST( begin1 != null and end1 != null );
		TEST( (begin1 + 4_b * count) == end1 );

		for (uint i = 0; i < count; ++i)
			TEST( Cast<uint>(begin1)[i] == i );

		const void*	begin2	= begin1 + 8_b;
		const void*	end2	= end1;
		stream2->UpdateFastStream( OUT begin2, OUT end2 );
		TEST( begin2 != null and end2 != null );
		TEST( begin1 + 8_b == begin2 );
		TEST( end1 == end2 );
		TEST( (begin2 + 4_b * count - 8_b) == end2 );

		for (uint i = 0; i < count-2; ++i)
			TEST_Eq( Cast<uint>(begin2)[i], i+2 );
	}


	static void  FastStream_Test2 ()
	{
		const uint	count	= 10'000;
		auto		stream1	= MakeRC<ArrayWStream>();

		for (uint i = 0; i < count; ++i)
			TEST( stream1->Write( i ));

		auto		stream2	= MakeRC<MemRefRStream>( stream1->GetData() );
		auto		stream3 = MakeRC<BufferedRStream>( stream2, 128_b );

		const void*	begin1	= null;
		const void*	end1	= null;
		stream3->UpdateFastStream( OUT begin1, OUT end1 );
		TEST( begin1 != null and end1 != null );
		TEST( (begin1 + 128_b) == end1 );

		for (uint i = 0; i < 128/4; ++i)
			TEST_Eq( Cast<uint>(begin1)[i], i );

		const void*	begin2	= begin1 + 128_b;
		const void*	end2	= end1;
		stream3->UpdateFastStream( OUT begin2, OUT end2 );
		TEST( begin2 != null and end2 != null );
		TEST( (begin2 + 128_b) == end2 );
		TEST( begin1 == begin2 );
		TEST( end1 == end2 );

		for (uint i = 0; i < 128/4; ++i)
			TEST_Eq( Cast<uint>(begin2)[i], (i + 128/4) );
	}


	static void  FastStream_Test3 ()
	{
		auto		stream1	= MakeRC<ArrayWStream>();

		void*		begin1	= null;
		const void*	end1	= null;
		stream1->UpdateFastStream( OUT begin1, OUT end1, 128_b );
		TEST( begin1 != null and end1 != null );
		TEST( (begin1 + 128_b) == end1 );

		for (uint i = 0; i < 128/4; ++i)
			Cast<uint>(begin1)[i] = i;

		void*		begin2	= begin1 + 128_b;
		const void*	end2	= end1;
		stream1->UpdateFastStream( OUT begin2, OUT end2, 128_b );
		TEST( begin2 != null and end2 != null );
		TEST( (begin2 + 128_b) == end2 );
		TEST( end1 == begin2 );

		for (uint i = 0; i < 128/4; ++i)
			Cast<uint>(begin2)[i] = (i + 128/4);

		stream1->EndFastStream( begin2 + 128_b );

		ArrayView<uint>	arr = stream1->GetData().Cast<uint>();
		TEST_Eq( arr.size(), (128/4 * 2) );

		for (uint i = 0; i < uint(arr.size()); ++i)
			TEST_Eq( arr[i], i );
	}


	static void  FastStream_Test4 ()
	{
		auto		stream1	= MakeRC<ArrayWStream>();
		auto		stream2 = MakeRC<BufferedWStream>( stream1, 256_b );

		void*		begin1	= null;
		const void*	end1	= null;
		stream2->UpdateFastStream( OUT begin1, OUT end1, 128_b );
		TEST( begin1 != null and end1 != null );
		TEST( (begin1 + 128_b) == end1 );

		for (uint i = 0; i < 128/4; ++i)
			Cast<uint>(begin1)[i] = i;

		void*		begin2	= begin1 + 128_b;
		const void*	end2	= end1;
		stream2->UpdateFastStream( OUT begin2, OUT end2, 128_b );
		TEST( begin2 != null and end2 != null );
		TEST( (begin2 + 128_b) == end2 );
		TEST( begin1 == begin2 );
		TEST( end1 == end2 );

		for (uint i = 0; i < 128/4; ++i)
			Cast<uint>(begin2)[i] = (i + 128/4);

		stream2->EndFastStream( begin2 + 128_b );
		stream2 = null;

		ArrayView<uint>	arr = stream1->GetData().Cast<uint>();
		TEST_Eq( arr.size(), (128/4 * 2) );

		for (uint i = 0; i < uint(arr.size()); ++i)
			TEST_Eq( arr[i], i );
	}


	template <typename RStream, typename WStream>
	static void  Stream_Test1 ()
	{
		const ulong	file_size	= 128ull << 20;	// MiB
		const uint	buf_size	= 4u << 10;		// KiB
		StaticAssert( IsMultipleOf( file_size, buf_size ));

		const Path		fname {"stream1_data.bin"};
		{
			WStream		wfile {fname};
			TEST( wfile.IsOpen() );
			TEST( AllBits( wfile.GetSourceType(), ESourceType::SequentialAccess | ESourceType::WriteAccess ));

			ulong	buf [buf_size / sizeof(ulong)];
			ulong	pos = 0;

			while ( pos < file_size )
			{
				for (uint i = 0; i < CountOf(buf); ++i) {
					buf[i] = pos + i;
				}

				TEST_Eq( wfile.WriteSeq( buf, Sizeof(buf) ), buf_size );
				pos += buf_size;
			}

			TEST_Eq( wfile.Position(), file_size );
		}
		{
			RStream		rfile {fname};
			TEST( rfile.IsOpen() );
			TEST( AllBits( rfile.GetSourceType(), ESourceType::SequentialAccess | ESourceType::ReadAccess ));
			TEST_Eq( rfile.Size(), file_size );

			ulong	dst_buf [buf_size / sizeof(ulong)];
			ulong	ref_buf [buf_size / sizeof(ulong)];
			ulong	pos = 0;

			while ( pos < file_size )
			{
				for (uint i = 0; i < CountOf(ref_buf); ++i) {
					ref_buf[i] = pos + i;
				}

				TEST_Eq( rfile.ReadSeq( OUT dst_buf, Sizeof(dst_buf) ), buf_size );
				TEST( MemEqual( dst_buf, ref_buf ));

				pos += buf_size;
			}
		}
	}


	template <typename RStream, typename WStream>
	static void  Stream_Test2 ()
	{
		const uint	buf_size	= 4u << 10;		// Kb

		const Path	fname {"file2_data.bin"};
		{
			WStream		wfile { fname, WStream::EMode::OpenRewrite };
			TEST( wfile.IsOpen() );

			ulong	buf [buf_size / sizeof(ulong)];
			for (usize i = 0; i < CountOf(buf); ++i) {
				buf[i] = i;
			}
			TEST( wfile.Write( buf, Sizeof(buf) ));
		}

		// append
		{
			WStream		wfile { fname, WStream::EMode::OpenAppend };
			TEST( wfile.IsOpen() );

			ulong	buf [buf_size / sizeof(ulong)];
			for (usize i = 0; i < CountOf(buf); ++i) {
				buf[i] = i + CountOf(buf);
			}
			TEST( wfile.Write( buf, Sizeof(buf) ));
		}

		// read
		{
			RStream		rfile {fname};
			TEST( rfile.IsOpen() );
			TEST_Eq( rfile.Size(), Bytes{buf_size*2} );

			ulong	buf [buf_size / sizeof(ulong)];

			for (usize j = 0; j < CountOf(buf)*2;)
			{
				TEST( rfile.Read( OUT buf, Sizeof(buf) ));

				for (usize i = 0; i < CountOf(buf); ++i, ++j) {
					TEST_Eq( buf[i], j );
				}
			}
		}
	}


	template <typename RFile, typename WFile>
	static void  File_Test1 (bool reserve)
	{
		const ulong	file_size	= 128ull << 20;	// Mb
		const uint	buf_size	= 4u << 10;		// Kb
		StaticAssert( IsMultipleOf( file_size, buf_size ));

		const Path		fname {"file1_data.bin"};
		{
			WFile	wfile {fname};
			TEST( wfile.IsOpen() );
			TEST( AllBits( wfile.GetSourceType(), ESourceType::RandomAccess | ESourceType::WriteAccess ));

			if ( reserve )
			{
				TEST_Eq( wfile.Reserve( Bytes{file_size} ), file_size );
				TEST_Eq( wfile.Capacity(), file_size );
			}

			ulong	buf [buf_size / sizeof(ulong)];
			ulong	pos = 0;

			while ( pos < file_size )
			{
				for (uint i = 0; i < CountOf(buf); ++i) {
					buf[i] = pos + i;
				}

				TEST_Eq( wfile.WriteBlock( Bytes{pos}, buf, Sizeof(buf) ), buf_size );
				pos += buf_size;
			}

			TEST_Eq( wfile.Capacity(), file_size );
		}
		{
			RFile	rfile {fname};
			TEST( rfile.IsOpen() );
			TEST( AllBits( rfile.GetSourceType(), ESourceType::RandomAccess | ESourceType::ReadAccess ));
			TEST_Eq( rfile.Size(), file_size );

			ulong	dst_buf [buf_size / sizeof(ulong)];
			ulong	ref_buf [buf_size / sizeof(ulong)];
			ulong	pos = 0;

			while ( pos < file_size )
			{
				for (uint i = 0; i < CountOf(ref_buf); ++i) {
					ref_buf[i] = pos + i;
				}

				TEST_Eq( rfile.ReadBlock( Bytes{pos}, OUT dst_buf, Sizeof(dst_buf) ), buf_size );
				TEST( MemEqual( dst_buf, ref_buf ));

				pos += buf_size;
			}
		}
	}


	template <typename RFile, typename WFile>
	static void  File_Test2 (typename RFile::EMode readAccess, typename WFile::EMode writeAccess)
	{
		const ulong	file_size	= 128ull << 20;	// Mb
		const uint	buf_size	= 4u << 10;		// Kb
		StaticAssert( IsMultipleOf( file_size, buf_size ));

		const Path		fname {"file1_data.bin"};
		{
			WFile	wfile { fname, writeAccess };
			TEST( wfile.IsOpen() );
			TEST( AllBits( wfile.GetSourceType(), ESourceType::RandomAccess | ESourceType::WriteAccess ));

			const Bytes	align = Max( Bytes{wfile.OffsetAlign().ptrAlign}, AlignOf<ulong> );

			DynUntypedStorage	buf;
			TEST( buf.Alloc( Bytes{buf_size}, align, null ));

			usize	cnt = buf_size / sizeof(ulong);
			ulong*	ptr = buf.Ptr<ulong>();
			ulong	pos = 0;

			while ( pos < file_size )
			{
				for (uint i = 0; i < cnt; ++i) {
					ptr[i] = pos + i;
				}

				TEST_Eq( wfile.WriteBlock( Bytes{pos}, ptr, buf.Size() ), buf_size );
				pos += buf_size;
			}

			TEST_Eq( wfile.Capacity(), file_size );
		}
		{
			RFile	rfile { fname, readAccess };
			TEST( rfile.IsOpen() );
			TEST( AllBits( rfile.GetSourceType(), ESourceType::RandomAccess | ESourceType::ReadAccess ));
			TEST_Eq( rfile.Size(), file_size );

			const Bytes	align = Max( Bytes{rfile.OffsetAlign().ptrAlign}, AlignOf<ulong> );

			DynUntypedStorage	dst_buf;
			TEST( dst_buf.Alloc( Bytes{buf_size}, align, null ));

			ulong	ref_buf [buf_size / sizeof(ulong)];
			ulong	pos = 0;

			while ( pos < file_size )
			{
				for (uint i = 0; i < CountOf(ref_buf); ++i) {
					ref_buf[i] = pos + i;
				}

				TEST_Eq( rfile.ReadBlock( Bytes{pos}, OUT dst_buf.Data(), dst_buf.Size() ), buf_size );
				TEST( MemEqual( dst_buf.Data(), ref_buf, dst_buf.Size() ));

				pos += buf_size;
			}
		}
	}
}


extern void UnitTest_DataSource (const Path &curr)
{
	#ifdef AE_ENABLE_BROTLI
	BrotliStream_Test1();
	BrotliStream_Test2();
	BrotliStream_Test3();
	BrotliStream_Test4();
	#endif

	#ifdef AE_ENABLE_ZSTD
	ZStdStream_Test1();
	ZStdStream_Test2();
	ZStdStream_Test3();
	ZStdStream_Test4();
	#endif

	#ifdef AE_ENABLE_LZ4
	Lz4Stream_Test1();
	Lz4Stream_Test2();
	Lz4Stream_Test3();
	Lz4Stream_Test4();
	Lz4Stream_Test5();
	#endif

	StdStream_Test1();
	BufferedStream_Test1();

	FastStream_Test1();
	FastStream_Test2();
	FastStream_Test3();
	FastStream_Test4();


	const Path	folder = curr / "ds_test";

	FileSystem::DeleteDirectory( folder );
	FileSystem::CreateDirectories( folder );
	TEST( FileSystem::SetCurrentPath( folder ));

	Stream_Test1< StdFileRStream,		StdFileWStream		>();
	Stream_Test2< StdFileRStream,		StdFileWStream		>();
	File_Test1<   StdFileRDataSource,	StdFileWDataSource	>( false );

	#ifdef AE_PLATFORM_WINDOWS
		Stream_Test1< WinFileRStream,		WinFileWStream		>();
		Stream_Test1< StdFileRStream,		WinFileWStream		>();
		Stream_Test1< WinFileRStream,		StdFileWStream		>();

		Stream_Test2< WinFileRStream,		WinFileWStream		>();
		Stream_Test2< StdFileRStream,		WinFileWStream		>();
		Stream_Test2< WinFileRStream,		StdFileWStream		>();

		File_Test1<   WinFileRDataSource,	WinFileWDataSource	>( false );
		File_Test1<   WinFileRDataSource,	StdFileWDataSource	>( false );
		File_Test1<   StdFileRDataSource,	WinFileWDataSource	>( false );
		File_Test1<   WinFileRDataSource,	WinFileWDataSource	>( true );

		File_Test2<   WinFileRDataSource,	WinFileWDataSource	>( WinFileRDataSource::EMode::Direct,	WinFileWDataSource::EMode::Direct );
	#else

		Stream_Test1< UnixFileRStream,		UnixFileWStream		>();
		Stream_Test1< StdFileRStream,		UnixFileWStream		>();
		Stream_Test1< UnixFileRStream,		StdFileWStream		>();

		Stream_Test2< UnixFileRStream,		UnixFileWStream		>();
		Stream_Test2< StdFileRStream,		UnixFileWStream		>();
		Stream_Test2< UnixFileRStream,		StdFileWStream		>();

		File_Test1<   UnixFileRDataSource,	UnixFileWDataSource	>( false );
		File_Test1<   UnixFileRDataSource,	StdFileWDataSource	>( false );
		File_Test1<   StdFileRDataSource,	UnixFileWDataSource	>( false );

		File_Test2<   UnixFileRDataSource,	UnixFileWDataSource	>( UnixFileRDataSource::EMode::Direct,  UnixFileWDataSource::EMode::Direct );

	# ifndef AE_PLATFORM_ANDROID
		File_Test1<   UnixFileRDataSource,	UnixFileWDataSource	>( true );
	# endif
	#endif

	FileSystem::SetCurrentPath( curr );
	FileSystem::DeleteDirectory( folder );

	TEST_PASSED();
}
