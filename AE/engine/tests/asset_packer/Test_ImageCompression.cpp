// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"
using namespace AE::AssetPacker;

namespace
{
	const bool		force_update	= true;
	PackAssetsFn_t	pack_assets		= null;


	static void  ImageCompression_Test1 ()
	{
		const PathParams	files[]			= { {TXT("img_comp.as")} };
		const Path			output			{"img_comp.bin"};
		const Path			temp_file		{"temp.bin"};
		CharType const*		res_folders[]	= { TXT("") };	// current dir

		FileSystem::DeleteFile( output );
		FileSystem::DeleteFile( temp_file );

		AssetInfo		info		= {};
		info.inFiles				= files;
		info.inFileCount			= CountOf(files);
		info.inResourceFolders		= res_folders;
		info.inResourceFolderCount	= CountOf(res_folders);
		info.outputArchive			= Cast<CharType>(output.c_str());
		info.tempFile				= Cast<CharType>(temp_file.c_str());

		TEST( pack_assets( &info ));

		auto	file = MakeRC<FileRStream>( output );
		TEST( file->IsOpen() );
		TEST_Eq( file->Size(), 644'124_b );
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
