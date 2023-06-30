// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/StdStream.h"
#include "base/DataSource/MemStream.h"
#include "base/DataSource/BrotliStream.h"
#include "base/DataSource/BufferedStream.h"
#include "base/DataSource/FastStream.h"
#include "base/DataSource/FileStream.h"
#include "base/DataSource/WindowsFile.h"
#include "base/Math/Random.h"
#include "UnitTest_Common.h"


namespace
{
    using ESourceType = IDataSource::ESourceType;


    ND_ static Array<ubyte>  GenRandomArray (Bytes size)
    {
        Array<ubyte>    temp;
        temp.resize( usize(size) );

        Math::Random    rnd;
        for (usize i = 0; i < temp.size(); ++i)
        {
            temp[i] = rnd.Uniform<ubyte>() & 0xF;
        }
        return temp;
    }


#ifdef AE_ENABLE_BROTLI
    static void  BrotliStream_Test1 ()
    {
        const auto      uncompressed = GenRandomArray( 1_Mb );

        Array<ubyte>    file_data;

        // compress
        {
            BrotliWStream::Config   cfg;
            cfg.inBlockSize = 1.0f;
            cfg.quality     = 1.0f;
            cfg.windowBits  = 1.0f;

            auto            stream = MakeRC<MemWStream>();
            BrotliWStream   encoder{ stream, cfg };

            TEST( encoder.IsOpen() );
            TEST( encoder.Write( ArrayView<ubyte>{uncompressed} ));
            encoder.Flush();

            file_data = stream->ReleaseData();
        }

        const Bytes compressed_size {file_data.size()};
        TEST( compressed_size < uncompressed.size() );

        // uncompress
        {
            BrotliRStream   decoder{ MakeRC<MemRStream>( RVRef(file_data) )};
            Array<ubyte>    data2, data3;

            TEST( decoder.IsOpen() );
            TEST( decoder.Read( uncompressed.size() / 2, OUT data2 ));
            TEST( decoder.Read( uncompressed.size() - data2.size(), OUT data3 ));
            TEST( decoder.Position() == compressed_size );

            TEST( uncompressed.size() == (data2.size() + data3.size()) );
            TEST( ArrayView<ubyte>{uncompressed}.section( 0, data2.size() ) == data2 );
            TEST( ArrayView<ubyte>{uncompressed}.section( data2.size(), data3.size() ) == data3 );
        }
    }


    static void  BrotliStream_Test2 ()
    {
        const auto      uncompressed = GenRandomArray( 2_Mb );
        const Bytes     block_size   = 1_Kb;

        Array<ubyte>    file_data;

        // compress
        {
            BrotliWStream::Config   cfg;
            cfg.inBlockSize = 0.2f;
            cfg.quality     = 0.5f;
            cfg.windowBits  = 0.2f;

            auto            stream = MakeRC<MemWStream>();
            BrotliWStream   encoder{ stream, cfg };

            TEST( encoder.IsOpen() );
            for (Bytes pos, size = ArraySizeOf(uncompressed); pos < size;)
            {
                Bytes   wr_size = Min( block_size, size - pos );
                Bytes   written = encoder.WriteSeq( uncompressed.data() + pos, wr_size );

                TEST( written > 0 );
                pos += written;
            }
            encoder.Flush();

            file_data = stream->ReleaseData();
        }

        const Bytes compressed_size {file_data.size()};
        TEST( compressed_size < uncompressed.size() );

        // uncompress
        {
            BrotliRStream   decoder{ MakeRC<MemRStream>( RVRef(file_data) )};
            Array<ubyte>    data2, data3;

            TEST( decoder.IsOpen() );
            TEST( decoder.Read( uncompressed.size() / 2, OUT data2 ));
            TEST( decoder.Read( uncompressed.size() - data2.size(), OUT data3 ));
            TEST( decoder.Position() == compressed_size );

            TEST( uncompressed.size() == (data2.size() + data3.size()) );
            TEST( ArrayView<ubyte>{uncompressed}.section( 0, data2.size() ) == data2 );
            TEST( ArrayView<ubyte>{uncompressed}.section( data2.size(), data3.size() ) == data3 );
        }
    }


    static void  BrotliStream_Test3 ()
    {
        const auto      uncompressed = GenRandomArray( 2_Mb );

        Array<ubyte>    file_data;

        // compress
        {
            BrotliWStream::Config   cfg;
            cfg.inBlockSize = 1.0f;
            cfg.quality     = 1.0f;
            cfg.windowBits  = 1.0f;

            auto            stream = MakeRC<MemWStream>();
            BrotliWStream   encoder{ stream, cfg };

            TEST( encoder.IsOpen() );
            TEST( encoder.Write( ArrayView<ubyte>{uncompressed} ));
            encoder.Flush();

            file_data = stream->ReleaseData();
        }

        const Bytes compressed_size {file_data.size()};

        // uncompress
        {
            BrotliRStream   decoder{ MakeRC<MemRStream>( RVRef(file_data) )};
            TEST( decoder.IsOpen() );

            MemWStream      dst_mem;
            const Bytes     size = DataSourceUtils::BufferedCopy( dst_mem, decoder );

            TEST( size == ArraySizeOf(uncompressed) );
            TEST( dst_mem.GetData() == uncompressed );
        }
    }
#endif // AE_ENABLE_BROTLI


    static void  StdStream_Test1 ()
    {
        String          src_str = "4324356uytsdgfh1243rttrasfdg";
        RC<RStream>     mem = MakeRC<MemRefRStream>( src_str );

        StreambufWrap<char> streambuf{ mem };
        std::istream        stream{ &streambuf };

        String              dst_str;
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
        const uint      count   = 10'000;
        RC<MemWStream>  stream1 = MakeRC<MemWStream>();
        RC<WStream>     stream2 = MakeRC<BufferedWStream>( stream1, 128_b );

        for (uint i = 0; i < count; ++i)
            stream2->Write( i );

        stream2->Flush();
        ArrayView<uint> mem = stream1->GetData().Cast<uint>();

        for (uint i = 0; i < count; ++i)
            TEST( mem[i] == i );

        RC<MemRefRStream>   stream3 = MakeRC<MemRefRStream>( mem );
        RC<RStream>         stream4 = MakeRC<BufferedRStream>( stream3 );

        for (uint i = 0; i < count; ++i)
        {
            uint    j = 0;
            stream4->Read( OUT j );
            TEST( i == j );
        }
    }


    static void  FastStream_Test1 ()
    {
        const uint      count   = 10'000;
        RC<MemWStream>  stream1 = MakeRC<MemWStream>();

        for (uint i = 0; i < count; ++i)
            stream1->Write( i );

        RC<MemRefRStream>   stream2 = stream1->ToRStream();

        const void* begin1  = null;
        const void* end1    = null;
        stream2->UpdateFastStream( OUT begin1, OUT end1 );
        TEST( begin1 != null and end1 != null );
        TEST( (begin1 + 4_b * count) == end1 );

        for (uint i = 0; i < count; ++i)
            TEST( Cast<uint>(begin1)[i] == i );

        const void* begin2  = begin1 + 8_b;
        const void* end2    = end1;
        stream2->UpdateFastStream( OUT begin2, OUT end2 );
        TEST( begin2 != null and end2 != null );
        TEST( begin1 + 8_b == begin2 );
        TEST( end1 == end2 );
        TEST( (begin2 + 4_b * count - 8_b) == end2 );

        for (uint i = 0; i < count-2; ++i)
            TEST( Cast<uint>(begin2)[i] == i+2 );
    }


    static void  FastStream_Test2 ()
    {
        const uint      count   = 10'000;
        RC<MemWStream>  stream1 = MakeRC<MemWStream>();

        for (uint i = 0; i < count; ++i)
            stream1->Write( i );

        RC<MemRefRStream>   stream2 = stream1->ToRStream();
        RC<BufferedRStream> stream3 = MakeRC<BufferedRStream>( stream2, 128_b );

        const void* begin1  = null;
        const void* end1    = null;
        stream3->UpdateFastStream( OUT begin1, OUT end1 );
        TEST( begin1 != null and end1 != null );
        TEST( (begin1 + 128_b) == end1 );

        for (uint i = 0; i < 128/4; ++i)
            TEST( Cast<uint>(begin1)[i] == i );

        const void* begin2  = begin1 + 128_b;
        const void* end2    = end1;
        stream3->UpdateFastStream( OUT begin2, OUT end2 );
        TEST( begin2 != null and end2 != null );
        TEST( (begin2 + 128_b) == end2 );
        TEST( begin1 == begin2 );
        TEST( end1 == end2 );

        for (uint i = 0; i < 128/4; ++i)
            TEST( Cast<uint>(begin2)[i] == (i + 128/4) );
    }


    static void  FastStream_Test3 ()
    {
        RC<MemWStream>  stream1 = MakeRC<MemWStream>();

        void*       begin1  = null;
        const void* end1    = null;
        stream1->UpdateFastStream( OUT begin1, OUT end1, 128_b );
        TEST( begin1 != null and end1 != null );
        TEST( (begin1 + 128_b) == end1 );

        for (uint i = 0; i < 128/4; ++i)
            Cast<uint>(begin1)[i] = i;

        void*       begin2  = begin1 + 128_b;
        const void* end2    = end1;
        stream1->UpdateFastStream( OUT begin2, OUT end2, 128_b );
        TEST( begin2 != null and end2 != null );
        TEST( (begin2 + 128_b) == end2 );
        TEST( end1 == begin2 );

        for (uint i = 0; i < 128/4; ++i)
            Cast<uint>(begin2)[i] = (i + 128/4);

        stream1->EndFastStream( begin2 + 128_b );

        ArrayView<uint> arr = stream1->GetData().Cast<uint>();
        TEST( arr.size() == (128/4 * 2) );

        for (uint i = 0; i < uint(arr.size()); ++i)
            TEST( arr[i] == i );
    }


    static void  FastStream_Test4 ()
    {
        RC<MemWStream>      stream1 = MakeRC<MemWStream>();
        RC<BufferedWStream> stream2 = MakeRC<BufferedWStream>( stream1, 256_b );

        void*       begin1  = null;
        const void* end1    = null;
        stream2->UpdateFastStream( OUT begin1, OUT end1, 128_b );
        TEST( begin1 != null and end1 != null );
        TEST( (begin1 + 128_b) == end1 );

        for (uint i = 0; i < 128/4; ++i)
            Cast<uint>(begin1)[i] = i;

        void*       begin2  = begin1 + 128_b;
        const void* end2    = end1;
        stream2->UpdateFastStream( OUT begin2, OUT end2, 128_b );
        TEST( begin2 != null and end2 != null );
        TEST( (begin2 + 128_b) == end2 );
        TEST( begin1 == begin2 );
        TEST( end1 == end2 );

        for (uint i = 0; i < 128/4; ++i)
            Cast<uint>(begin2)[i] = (i + 128/4);

        stream2->EndFastStream( begin2 + 128_b );
        stream2 = null;

        ArrayView<uint> arr = stream1->GetData().Cast<uint>();
        TEST( arr.size() == (128/4 * 2) );

        for (uint i = 0; i < uint(arr.size()); ++i)
            TEST( arr[i] == i );
    }


    template <typename RStream, typename WStream>
    static void  Stream_Test1 ()
    {
        const ulong file_size   = 128ull << 20; // Mb
        const uint  buf_size    = 4u << 10;     // Kb
        STATIC_ASSERT( file_size % buf_size == 0 );

        const Path      fname {"stream1_data.txt"};
        {
            WStream     wfile {fname};
            TEST( wfile.IsOpen() );
            TEST( AllBits( wfile.GetSourceType(), ESourceType::SequentialAccess | ESourceType::WriteAccess ));

            ulong   buf [buf_size / sizeof(ulong)];
            ulong   pos = 0;

            while ( pos < file_size )
            {
                for (uint i = 0; i < CountOf(buf); ++i) {
                    buf[i] = pos + i;
                }

                TEST( wfile.WriteSeq( buf, Sizeof(buf) ) == buf_size );
                pos += buf_size;
            }

            TEST( wfile.Position() == file_size );
        }
        {
            RStream     rfile {fname};
            TEST( rfile.IsOpen() );
            TEST( AllBits( rfile.GetSourceType(), ESourceType::SequentialAccess | ESourceType::ReadAccess ));
            TEST( rfile.Size() == file_size );

            ulong   dst_buf [buf_size / sizeof(ulong)];
            ulong   ref_buf [buf_size / sizeof(ulong)];
            ulong   pos = 0;

            while ( pos < file_size )
            {
                for (uint i = 0; i < CountOf(ref_buf); ++i) {
                    ref_buf[i] = pos + i;
                }

                TEST( rfile.ReadSeq( OUT dst_buf, Sizeof(dst_buf) ) == buf_size );
                TEST( MemEqual( dst_buf, ref_buf ));

                pos += buf_size;
            }
        }
    }


    template <typename RFile, typename WFile>
    static void  File_Test1 (bool reserve)
    {
        const ulong file_size   = 128ull << 20; // Mb
        const uint  buf_size    = 4u << 10;     // Kb
        STATIC_ASSERT( file_size % buf_size == 0 );

        const Path      fname {"file1_data.txt"};
        {
            WFile   wfile {fname};
            TEST( wfile.IsOpen() );
            TEST( AllBits( wfile.GetSourceType(), ESourceType::RandomAccess | ESourceType::WriteAccess ));

            if ( reserve )
            {
                TEST( wfile.Reserve( Bytes{file_size} ) == file_size );
                TEST( wfile.Capacity() == file_size );
            }

            ulong   buf [buf_size / sizeof(ulong)];
            ulong   pos = 0;

            while ( pos < file_size )
            {
                for (uint i = 0; i < CountOf(buf); ++i) {
                    buf[i] = pos + i;
                }

                TEST( wfile.WriteBlock( Bytes{pos}, buf, Sizeof(buf) ) == buf_size );
                pos += buf_size;
            }

            TEST( wfile.Capacity() == file_size );
        }
        {
            RFile   rfile {fname};
            TEST( rfile.IsOpen() );
            TEST( AllBits( rfile.GetSourceType(), ESourceType::RandomAccess | ESourceType::ReadAccess ));
            TEST( rfile.Size() == file_size );

            ulong   dst_buf [buf_size / sizeof(ulong)];
            ulong   ref_buf [buf_size / sizeof(ulong)];
            ulong   pos = 0;

            while ( pos < file_size )
            {
                for (uint i = 0; i < CountOf(ref_buf); ++i) {
                    ref_buf[i] = pos + i;
                }

                TEST( rfile.ReadBlock( Bytes{pos}, OUT dst_buf, Sizeof(dst_buf) ) == buf_size );
                TEST( MemEqual( dst_buf, ref_buf ));

                pos += buf_size;
            }
        }
    }
}


extern void UnitTest_DataSource ()
{
    const Path  curr    = FileSystem::CurrentPath();
    const Path  folder  {AE_CURRENT_DIR "/ds_test"};

    FileSystem::RemoveAll( folder );
    FileSystem::CreateDirectories( folder );
    TEST( FileSystem::SetCurrentPath( folder ));

    #ifdef AE_ENABLE_BROTLI
    BrotliStream_Test1();
    BrotliStream_Test2();
    BrotliStream_Test3();
    #endif

    StdStream_Test1();
    BufferedStream_Test1();

    FastStream_Test1();
    FastStream_Test2();
    FastStream_Test3();
    FastStream_Test4();

    // minimize disk usage
#if 0
    Stream_Test1< FileRStream,      FileWStream >();
    File_Test1<   FileRDataSource,  FileWDataSource >( false );

    #ifdef AE_PLATFORM_WINDOWS
        Stream_Test1< WinRFileStream,       WinWFileStream      >();
        Stream_Test1< FileRStream,          WinWFileStream      >();
        Stream_Test1< WinRFileStream,       FileWStream         >();
        File_Test1<   WinRFileDataSource,   WinWFileDataSource  >( false );
        File_Test1<   WinRFileDataSource,   FileWDataSource     >( false );
        File_Test1<   FileRDataSource,      WinWFileDataSource  >( false );
        File_Test1<   WinRFileDataSource,   WinWFileDataSource  >( true );
    #endif
#endif

    FileSystem::SetCurrentPath( curr );

    TEST_PASSED();
}
