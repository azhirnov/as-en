// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"


namespace
{
	const bool				force_update	= true;
	decltype(&PackAssets)	pack_assets		= null;


	static void  ImageAtlasPack_Test1 ()
	{
		const CharType*		files[]		= { TXT("atlas.as") };
		const Path			output		= TXT("atlas.bin");
		const Path			temp_file	= TXT("temp.bin");
		
		FileSystem::Remove( output );
		FileSystem::Remove( temp_file );

		AssetInfo	info = {};
		info.inFiles		= files;
		info.inFileCount	= CountOf(files);
		info.outputArchive	= output.c_str();
		info.tempFile		= temp_file.c_str();

		TEST( pack_assets( &info ));
		

		auto	file = MakeRC<FileRStream>( output );
		TEST( file->IsOpen() );
	}
}


extern void Test_ImageAtlasPack ()
{
	{
		Path	dll_path{ AE_ASSET_PACKER_LIBRARY };
		dll_path.append( AE_RESPACK_BUILD_TYPE "/AssetPacker-shared.dll" );

		Library		lib;
		TEST( lib.Load( dll_path ));
		TEST( lib.GetProcAddr( "PackAssets", OUT pack_assets ));
		
		TEST( FileSystem::SetCurrentPath( AE_CURRENT_DIR "/atlas_test" ));

		ImageAtlasPack_Test1();
	}
	TEST_PASSED();
}
