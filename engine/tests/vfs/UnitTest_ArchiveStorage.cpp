// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "../shared/UnitTest_Shared.h"
#include "base/Math/Random.h"
#include "base/Stream/FileStream.h"
#include "vfs/Archive/ArchiveStaticStorage.h"
#include "vfs/Archive/ArchivePacker.h"
using namespace AE::VFS;

namespace
{
	using EFileType = ArchivePacker::EFileType;


	ND_ static bool  CreateRandomFile (const Path &path, Bytes size)
	{
		Array<ubyte>	temp;
		temp.resize( usize(size) );

		Math::Random	rnd;
		for (usize i = 0; i < temp.size(); ++i)
		{
			temp[i] = rnd.Uniform<ubyte>() & 0x3F;
		}

		FileWStream	file {path};
		CHECK_ERR( file.IsOpen() );
		CHECK_ERR( file.Write( ArrayView<ubyte>{ temp }));

		return true;
	}


	ND_ static bool  CompareFiles (RStream &lhsStream, RStream &rhsStream) {
		return bool(StreamUtils::Compare( lhsStream, rhsStream ));
	}
	
	ND_ static bool  CompareFiles (const Path &lhsPath, RStream &rhsStream)
	{
		FileRStream	file {lhsPath};
		return file.IsOpen() and CompareFiles( file, rhsStream );
	}


	static void  Archive_Test1 ()
	{
		const Path	file1 {"temp/file1.bin"};
		const Path	file2 {"temp/file2.bin"};
		const Path	file3 {"temp/file3.bin"};
		const Path	arch  {"archive.bin"};
		
		const FileName	name1 {"file1"};
		const FileName	name2 {"file2"};
		const FileName	name3 {"file3"};

		FileSystem::CreateDirectories( "temp" );

		// create files
		{
			TEST( CreateRandomFile( file1, 1_Mb ));
			TEST( CreateRandomFile( file2, 512_Kb ));
			TEST( CreateRandomFile( file3, 782_Kb ));
		}

		// create archive
		{
			ArchivePacker	packer;
			TEST( packer.Create( "temp/archive.tmp" ));

			TEST( packer.Add( name1, file1, EFileType::Brotli ));
			TEST( packer.Add( name2, file2, EFileType::Raw ));
			TEST( packer.Add( name3, file3, EFileType::Brotli ));

			TEST( packer.Store( arch ));
		}

		// read archive
		{
			ArchiveStaticStorage	storage;
			TEST( storage.Create( arch ));

			{
				auto	stream = storage.Open( name1 );
				TEST( stream );
				TEST( CompareFiles( file1, *stream ));
			}{
				auto	stream = storage.Open( name2 );
				TEST( stream );
				TEST( CompareFiles( file2, *stream ));
			}{
				auto	stream = storage.Open( name3 );
				TEST( stream );
				TEST( CompareFiles( file3, *stream ));
			}
		}
	}
}

extern void UnitTest_ArchiveStorage ()
{
	const Path	curr	= FileSystem::CurrentPath();
	const Path	folder	{AE_CURRENT_DIR "/vfs_test"};
	
	FileSystem::CreateDirectories( folder );
	TEST( FileSystem::SetCurrentPath( folder ));

	Archive_Test1();
	
	FileSystem::SetCurrentPath( curr );
	TEST_PASSED();
}
