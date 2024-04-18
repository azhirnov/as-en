// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"
using namespace AE::AssetPacker;

namespace
{
	const bool				force_update	= true;
	decltype(&PackAssets)	pack_assets		= null;


	static void  ImageAtlasPack_Test1 ()
	{
		const PathParams	files[]			= { {TXT("atlas.as")} };
		const Path			output			= TXT("atlas.bin");
		const Path			temp_file		= TXT("temp.bin");
		const Path			output_script	= TXT( AE_SHARED_DATA "/scripts/asset_packer.as" );

		FileSystem::Remove( output );
		FileSystem::Remove( temp_file );

		AssetInfo		info	= {};
		info.inFiles			= files;
		info.inFileCount		= CountOf(files);
		info.outputArchive		= Cast<CharType>(output.c_str());
		info.tempFile			= Cast<CharType>(temp_file.c_str());
		info.outputScriptFile	= Cast<CharType>(output_script.c_str());

		TEST( pack_assets( &info ));


		auto	file = MakeRC<FileRStream>( output );
		TEST( file->IsOpen() );
	}
}


extern void Test_ImageAtlasPack ()
{
#ifdef AE_ASSET_PACKER_LIBRARY
	{
		Library		lib;
		TEST( lib.Load( AE_ASSET_PACKER_LIBRARY ));
		TEST( lib.GetProcAddr( "PackAssets", OUT pack_assets ));

		TEST( FileSystem::SetCurrentPath( AE_CURRENT_DIR "/atlas_test" ));

		ImageAtlasPack_Test1();
	}
	TEST_PASSED();
#endif
}
