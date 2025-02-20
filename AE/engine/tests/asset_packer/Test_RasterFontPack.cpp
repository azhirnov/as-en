// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"
#include "graphics_rhi/Private/EnumUtils.h"
#include "vfs/VirtualFileSystem.h"
using namespace AE::AssetPacker;

namespace
{
#	include "Packer/ImagePacker.cpp.h"
#	include "Packer/RasterFontPacker.cpp.h"

	const bool				force_update	= true;
	decltype(&PackAssets)	pack_assets		= null;


	static void  RasterFontPack_Test1 ()
	{
		const PathParams	files[]			= { {TXT("font1.as")} };
		const Path			output			{"font1.bin"};
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
		TEST( storage->Open( OUT file, VFS::FileName{"raster-font"} ));

		RasterFontPacker	unpacker;
		{
			Serializing::Deserializer	des{ MakeRC<BufferedRStream>( file )};
			TEST( RasterFontPacker_Deserialize( OUT unpacker, des ));
		}
		TEST_Eq( unpacker.glyphMap.size(), 163 );
		TEST( unpacker._header.flags == RasterFontPacker::EFileFlags::HasImage );
		TEST(All( unpacker._imageHeader.dimension == ImageDim_t{1024, 273, 1} ));
	}


	static void  RasterFontPack_Test2 ()
	{
		const PathParams	files[]			= { {TXT("font2.as")} };
		const Path			output			{"font2.bin"};
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
		TEST( storage->Open( OUT file, VFS::FileName{"sdf-font"} ));

		RasterFontPacker	unpacker;
		{
			Serializing::Deserializer	des{ MakeRC<BufferedRStream>( file )};
			TEST( RasterFontPacker_Deserialize( OUT unpacker, des ));
		}
		TEST_Eq( unpacker.glyphMap.size(), 163 );
		TEST( unpacker._header.flags == RasterFontPacker::EFileFlags::HasImage );
		TEST(All( unpacker._imageHeader.dimension == ImageDim_t{512, 140, 1} ));
	}


	static void  RasterFontPack_Test3 ()
	{
		const PathParams	files[]			= { {TXT("font3.as")} };
		const Path			output			{"font3.bin"};
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
		TEST( storage->Open( OUT file, VFS::FileName{"msdf-font"} ));

		RasterFontPacker	unpacker;
		{
			Serializing::Deserializer	des{ MakeRC<BufferedRStream>( file )};
			TEST( RasterFontPacker_Deserialize( OUT unpacker, des ));
		}
		TEST_Eq( unpacker.glyphMap.size(), 163 );
		TEST( unpacker._header.flags == RasterFontPacker::EFileFlags::HasImage );
		TEST(All( unpacker._imageHeader.dimension == ImageDim_t{512, 140, 1} ));
	}


	static void  RasterFontPack_Test4 ()
	{
		const PathParams	files[]			= { {TXT("font4.as")} };
		const Path			output			{"font4.bin"};
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
		TEST( storage->Open( OUT file, VFS::FileName{"font_meta"} ));

		RasterFontPacker	unpacker;
		{
			Serializing::Deserializer	des{ MakeRC<BufferedRStream>( file )};
			Serializing::SerializedID	ser1, ser2;
			CachedResourceName			name1, name2;

			TEST( des( OUT ser1, OUT name1 ) and
				  ser1 == ImagePacker::SerID and
				  name1 == CachedResourceName{"font-image"} );

			ImagePacker::FileHeader		img_file_hdr;
			TEST( ImagePacker_Deserialize( des, OUT img_file_hdr ));
			TEST( img_file_hdr.fileName == VFS::FileName{"font_image"} );

			TEST( des( OUT ser2, OUT name2 ) and
				  ser2 == RasterFontPacker::SerID and
				  name2 == CachedResourceName{"raster-font"} );

			TEST( RasterFontPacker_Deserialize( OUT unpacker, des ));
		}
		TEST_Eq( unpacker.glyphMap.size(), 163 );
		TEST( unpacker._header.flags == RasterFontPacker::EFileFlags::HasResName );
	}
}


extern void Test_RasterFontPack ()
{
#ifdef AE_ASSET_PACKER_LIBRARY
	{
		Library		lib;
		TEST( lib.Load( AE_ASSET_PACKER_LIBRARY ));
		TEST( lib.GetProcAddr( "PackAssets", OUT pack_assets ));

		TEST( FileSystem::SetCurrentPath( Path{AE_CURRENT_DIR} / "font_test" ));

		LocalTaskScheduler	scheduler {IOThreadCount(1)};

		RasterFontPack_Test1();
		RasterFontPack_Test2();
		RasterFontPack_Test3();
		RasterFontPack_Test4();
	}
	TEST_PASSED();
#endif
}
