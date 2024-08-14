// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"
using namespace AE::AssetPacker;

namespace
{
	const bool				force_update	= true;
	decltype(&PackAssets)	pack_assets		= null;


	static void  ImageCompression_Test1 ()
	{
		const PathParams	files[]			= { {TXT("img_comp.as")} };
		const Path			output			= TXT("img_comp.bin");
		const Path			temp_file		= TXT("temp.bin");

		FileSystem::DeleteFile( output );
		FileSystem::DeleteFile( temp_file );

		AssetInfo		info	= {};
		info.inFiles			= files;
		info.inFileCount		= CountOf(files);
		info.outputArchive		= Cast<CharType>(output.c_str());
		info.tempFile			= Cast<CharType>(temp_file.c_str());

		TEST( pack_assets( &info ));

		auto	file = MakeRC<FileRStream>( output );
		TEST( file->IsOpen() );
		TEST_Eq( file->Size(), 644'080_b );
	}
}


extern void Test_ImageCompression ()
{
#ifdef AE_ASSET_PACKER_LIBRARY
	{
		Library		lib;
		TEST( lib.Load( AE_ASSET_PACKER_LIBRARY ));
		TEST( lib.GetProcAddr( "PackAssets", OUT pack_assets ));

		TEST( FileSystem::SetCurrentPath( Path{AE_CURRENT_DIR} / "img_comp_test" ));

		ImageCompression_Test1();
	}
	TEST_PASSED();
#endif
}
