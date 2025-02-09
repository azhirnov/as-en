// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"
#include "graphics_rhi/Private/EnumUtils.h"
#include "graphics_hl/UI/StyleCollection.h"
#include "vfs/VirtualFileSystem.h"
using namespace AE::AssetPacker;

namespace
{
#	include "Packer/ImagePacker.cpp.h"
#	include "Packer/ImageAtlasPacker.cpp.h"
#	include "Packer/RasterFontPacker.cpp.h"

	const bool				force_update	= true;
	decltype(&PackAssets)	pack_assets		= null;


	static void  DeserializeUIStyle (Serializing::Deserializer &des)
	{
		Serializing::SerializedID	id;

		for (;;)
		{
			TEST( des( OUT id ));

			switch ( uint{id} )
			{
				case uint{ImagePacker::SerID} :
				{
					CachedResourceName	name;
					TEST( des( OUT name ) and name.IsDefined() );

					ImagePacker::FileHeader		file_hdr;
					TEST( ImagePacker_Deserialize( des, OUT file_hdr ));

					TEST( file_hdr.fileName == VFS::FileName{"ui_image_r"} or
						  file_hdr.fileName == VFS::FileName{"ui_image_rgba"} );
					break;
				}

				case uint{ImageAtlasPacker::SerID} :
				{
					CachedResourceName	name;
					TEST( des( OUT name ) and name.IsDefined() );

					ImageAtlasPacker	unpacker;
					TEST( ImageAtlasPacker_Deserialize( OUT unpacker, des ));
					break;
				}

				case uint{RasterFontPacker::SerID} :
				{
					CachedResourceName	name;
					TEST( des( OUT name ) and name.IsDefined() );

					RasterFontPacker	unpacker;
					TEST( RasterFontPacker_Deserialize( OUT unpacker, des ));
					break;
				}

				case uint{UI::StyleCollection::SerID} :
				{
					PipelineName	dbg_ppln_name;
					uint			style_count		= 0;

					TEST( des( OUT dbg_ppln_name, OUT style_count ));
					TEST_Eq( style_count, 4 );
					return;
				}
			}
		}
	}


	static void  DeserializeUIWidget (Serializing::Deserializer &des)
	{
	}


	static void  UIPack_Test1 ()
	{
		const PathParams	files[]			= { {TXT("ui_atlas_font.as")}, {TXT("ui_style.as")} };
		const Path			output			= TXT("ui_style.bin");
		const Path			temp_file		= TXT("temp.bin");
		const Path			output_script	= TXT( AE_SHARED_DATA "/scripts/asset_packer.as" );

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
		TEST( storage->Open( OUT file, VFS::FileName{"ui-style"} ));

		Serializing::Deserializer	des {file};
		DeserializeUIStyle( des );
	}


	static void  UIPack_Test2 ()
	{
		const PathParams	files[]			= { {TXT("ui_atlas_font.as")}, {TXT("ui_style.as")}, {TXT("ui_widgets.as")} };
		const Path			output			= TXT("ui_widgets.bin");
		const Path			temp_file		= TXT("temp.bin");
		const Path			output_script	= TXT( AE_SHARED_DATA "/scripts/asset_packer.as" );

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
		{
			RC<RStream>	file;
			TEST( storage->Open( OUT file, VFS::FileName{"ui-style"} ));

			Serializing::Deserializer	des {file};
			DeserializeUIStyle( des );
		}{
			RC<RStream>	file;
			TEST( storage->Open( OUT file, VFS::FileName{"ui-widget"} ));

			Serializing::Deserializer	des {file};
			DeserializeUIWidget( des );
		}
	}
}


extern void Test_UI ()
{
#ifdef AE_ASSET_PACKER_LIBRARY
	{
		Library		lib;
		TEST( lib.Load( AE_ASSET_PACKER_LIBRARY ));
		TEST( lib.GetProcAddr( "PackAssets", OUT pack_assets ));

		TEST( FileSystem::SetCurrentPath( Path{AE_CURRENT_DIR} / "ui_test" ));

		LocalTaskScheduler	scheduler {IOThreadCount(1)};

		UIPack_Test1();
		UIPack_Test2();
	}
	TEST_PASSED();
#endif
}
