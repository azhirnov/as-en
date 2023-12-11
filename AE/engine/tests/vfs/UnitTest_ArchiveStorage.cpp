// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"
#include "vfs/Archive/ArchiveStaticStorage.h"
#include "vfs/Archive/ArchivePacker.h"

namespace
{
    using EFileType = ArchivePacker::EFileType;


    ND_ static bool  CreateRandomFile (const Path &path, Bytes size)
    {
        Array<ubyte>    temp;
        temp.resize( usize(size) );

        Math::Random    rnd;
        for (usize i = 0; i < temp.size(); ++i)
        {
            temp[i] = rnd.Uniform<ubyte>() & 0x3F;
        }

        FileWStream file {path};
        CHECK_ERR( file.IsOpen() );
        CHECK_ERR( file.Write( ArrayView<ubyte>{ temp }));

        return true;
    }


    ND_ static bool  CompareFiles (RStream &lhsStream, RStream &rhsStream, Bytes dataSize)
    {
        auto    res = DataSourceUtils::Compare( lhsStream, rhsStream );
        return (res.diff == 0) and (res.processed == dataSize);
    }

    ND_ static bool  CompareFiles (const Path &lhsPath, RStream &rhsStream, Bytes dataSize)
    {
        FileRStream file {lhsPath};
        return file.IsOpen() and CompareFiles( file, rhsStream, dataSize );
    }

    ND_ static bool  CompareFiles (const Path &lhsPath, RDataSource &rhsDS, Bytes dataSize)
    {
        FileRStream                         file        {lhsPath};
        RDataSourceAsStream< RDataSource *> rhs_stream  {&rhsDS};

        return  file.IsOpen()       and
                rhs_stream.IsOpen() and
                CompareFiles( file, rhs_stream, dataSize );
    }


    static void  Archive_Test1 ()
    {
        const Path  file1 {"temp/file1.bin"};
        const Path  file2 {"temp/file2.bin"};
        const Path  file3 {"temp/file3.bin"};
        const Path  file4 {"temp/file4.bin"};
        const Path  arch  {"archive.bin"};

        const Bytes file1_size  = 1_Mb;
        const Bytes file2_size  = 512_Kb;
        const Bytes file3_size  = 782_Kb;
        const Bytes file4_size  = 55_Kb;

        const FileName::WithString_t    name1 {"file1"};
        const FileName::WithString_t    name2 {"file2"};
        const FileName::WithString_t    name3 {"file3"};
        const FileName::WithString_t    name4 {"file4"};

        FileSystem::CreateDirectories( "temp" );

        // create files
        {
            TEST( CreateRandomFile( file1, file1_size ));
            TEST( CreateRandomFile( file2, file2_size ));
            TEST( CreateRandomFile( file3, file3_size ));
            TEST( CreateRandomFile( file4, file4_size ));
        }

        // create archive
        {
            ArchivePacker   packer;
            TEST( packer.Create( "temp/archive.tmp" ));

            TEST( packer.Add( name1, file1, EFileType::Brotli ));
            TEST( packer.Add( name2, file2, EFileType::Raw ));
            TEST( packer.Add( name3, file3, EFileType::Brotli ));
            TEST( packer.Add( name4, file4, EFileType::BrotliInMemory ));

            TEST( packer.Store( arch ));
        }

        // read archive
        {
            auto    storage = VirtualFileStorageFactory::CreateStaticArchive( arch );
            TEST( storage );

            {
                RC<RStream>     stream;
                TEST( storage->Open( OUT stream, name1 ));
                TEST( CompareFiles( file1, *stream, file1_size ));
            }{
                RC<RStream>     stream;
                TEST( storage->Open( OUT stream, name2 ));
                TEST( CompareFiles( file2, *stream, file2_size ));
            }{
                RC<RStream>     stream;
                TEST( storage->Open( OUT stream, name3 ));
                TEST( CompareFiles( file3, *stream, file3_size ));
            }{
                RC<RDataSource> ds;
                TEST( storage->Open( OUT ds, name4 ));
                TEST( CompareFiles( file4, *ds, file4_size ));
            }
        }
    }
}

extern void UnitTest_ArchiveStorage ()
{
    const Path  curr    = FileSystem::CurrentPath();
    const Path  folder  {AE_CURRENT_DIR "/vfs_test1"};

    FileSystem::RemoveAll( folder );
    FileSystem::CreateDirectories( folder );
    TEST( FileSystem::SetCurrentPath( folder ));

    Archive_Test1();

    FileSystem::SetCurrentPath( curr );
    TEST_PASSED();
}
