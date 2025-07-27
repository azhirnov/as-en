// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"
#include "graphics_rhi/Private/EnumUtils.h"
#include "vfs/VirtualFileSystem.h"
using namespace AE::AssetPacker;

namespace
{
#	include "Packer/ImagePacker.cpp.h"
#	include "Packer/ImageAtlasPacker.cpp.h"

	const bool		force_update	= true;
	PackAssetsFn_t	pack_assets		= null;


	static void  ImageAtlasPack_Test1 ()
	{
		const PathParams	files[]			= { {TXT("atlas1.as")} };
		const Path			output			{"atlas1.bin"};
		const Path			temp_file		{"temp.bin"};
		const Path			output_script	{ AE_SHARED_DATA "/scripts/asset_packer.as" };

		FileSystem::DeleteFile( output );
		FileSystem::DeleteFile( temp_file );

		AssetInfo		info	= {};
		info.inFiles			= files;
		info.inFileCount		= CountOf(files);
		info.outputArchive		= Cast<CharType>(output.c_str());
		info.tempFile			= Cast<CharType>(temp_file.c_str());
		info.outputScriptFile	= Cast<CharType>(output_script.c_str());

		TEST( pack_assets( &info ));


		auto	storage = VFS::VirtualFileStorageFactory::CreateStaticArchive( output );
		TEST( storage );

		RC<RStream>	file;
		TEST( storage->Open( OUT file, VFS::FileName{"atlas"} ));

		ImageAtlasPacker	unpacker;
		{
			Serializing::Deserializer	des{ MakeRC<BufferedRStream>( file )};
			TEST( ImageAtlasPacker_Deserialize( OUT unpacker, des ));
		}
		TEST_Eq( unpacker.rects.size(), 5 );
		TEST_Eq( unpacker.map.size(), 6 );
		TEST( unpacker._header.flags == ImageAtlasPacker::EFileFlags::HasImage );
		TEST(All( unpacker._imageHeader.dimension == ImageDim_t{128, 68, 1} ));
	}


	static void  ImageAtlasPack_Test2 ()
	{
		const PathParams	files[]			= { {TXT("atlas2.as")} };
		const Path			output			{"atlas2.bin"};
		const Path			temp_file		{"temp.bin"};
		const Path			output_script	{ AE_SHARED_DATA "/scripts/asset_packer.as" };

		FileSystem::DeleteFile( output );
		FileSystem::DeleteFile( temp_file );

		AssetInfo		info	= {};
		info.inFiles			= files;
		info.inFileCount		= CountOf(files);
		info.outputArchive		= Cast<CharType>(output.c_str());
		info.tempFile			= Cast<CharType>(temp_file.c_str());
		info.outputScriptFile	= Cast<CharType>(output_script.c_str());

		TEST( pack_assets( &info ));


		auto	storage = VFS::VirtualFileStorageFactory::CreateStaticArchive( output );
		TEST( storage );

		RC<RStream>	file;
		TEST( storage->Open( OUT file, VFS::FileName{"atlas_meta"} ));

		ImageAtlasPacker	unpacker;
		{
			Serializing::Deserializer	des{ MakeRC<BufferedRStream>( file )};
			Serializing::SerializedID	ser1, ser2;
			CachedResourceName			name1, name2;

			TEST( des( OUT ser1, OUT name1 ) and
				  ser1 == ImagePacker::SerID and
				  name1 == CachedResourceName{"atlas-image"} );

			ImagePacker::FileHeader		img_file_hdr;
			TEST( ImagePacker_Deserialize( des, OUT img_file_hdr ));
			TEST( img_file_hdr.fileName == VFS::FileName{"atlas_image"} );

			TEST( des( OUT ser2, OUT name2 ) and
				  ser2 == ImageAtlasPacker::SerID and
				  name2 == CachedResourceName{"atlas"} );

			TEST( ImageAtlasPacker_Deserialize( OUT unpacker, des ));
		}
		TEST_Eq( unpacker.rects.size(), 5 );
		TEST_Eq( unpacker.map.size(), 6 );
		TEST( unpacker._header.flags == ImageAtlasPacker::EFileFlags::HasResName );
	}
}


extern void Test_ImageAtlasPack ()
{
#ifdef AE_ASSET_PACKER_LIBRARY
	{
		Library		lib;
		TEST( lib.Load( AE_ASSET_PACKER_LIBRARY ));
		TEST( lib.GetProcAddr( "PackAssets", OUT pack_assets ));

		TEST( FileSystem::SetCurrentPath( Path{AE_CURRENT_DIR} / "atlas_test" ));

		LocalTaskScheduler	scheduler {IOThreadCount(1)};

		ImageAtlasPack_Test1();
		ImageAtlasPack_Test2();
	}
	TEST_PASSED();
#endif
}
