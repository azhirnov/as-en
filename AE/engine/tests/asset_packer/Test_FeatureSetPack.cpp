// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"
using namespace AE::PipelineCompiler;

namespace
{
	const bool						force_update		= true;
	decltype(&CompilePipelines)		compile_pipelines	= null;


	static void  FeatureSetPack_Test1 ()
	{
		const PathParams	features[]	= { {TXT("config_vk.as"), 0},
											{TXT( AE_SHARED_DATA "/feature_set" ), 1, EPathParamsFlags::RecursiveFolder} };
		Path				output		= TXT("_output");

		FileSystem::DeleteDirectory( output );
		TEST( FileSystem::CreateDirectories( output ));

		output.append( "features.bin" );

		PipelinesInfo	info = {};
		info.inPipelines		= features;
		info.inPipelineCount	= CountOf( features );
		info.outputPackName		= Cast<CharType>(output.c_str());

		TEST( compile_pipelines( &info ));


		auto	file = MakeRC<FileRStream>( output );
		TEST( file->IsOpen() );

		auto	mem_stream = MakeRC<ArrayRStream>();
		{
			uint	name;
			TEST( file->Read( OUT name ));
			TEST_Eq( name, PackOffsets_Name );

			PipelinePackOffsets		offsets;
			TEST( file->Read( OUT offsets ));
			TEST_Lt( offsets.featureSetOffset, ulong(file->Size()) );

			TEST( file->SeekSet( Bytes{offsets.featureSetOffset} ));
			TEST( mem_stream->LoadRemainingFrom( *file, Bytes{offsets.featureSetDataSize} ));
		}

		AE::Serializing::Deserializer	des{ mem_stream };
		{
			uint	version = 0;
			uint	name	= 0;
			TEST( des( OUT name, OUT version ));
			TEST_Eq( name, FeatureSetPack_Name );
			TEST_Eq( version, FeatureSetPack_Version );
		}

		ulong	fs_hash = 0;
		TEST( des( OUT fs_hash ));
		TEST_Eq( fs_hash, ulong(FeatureSet::GetHashOfFS_Precalculated()) );

		uint	count = 0;
		TEST( des( OUT count ));
		TEST_Eq( count, 48 );

		for (uint i = 0; i < count; ++i)
		{
			FeatureSetName::Optimized_t	name;
			FeatureSetSerializer		fs;

			TEST( des( OUT name, OUT fs ));
			TEST( name.IsDefined() );
			TEST( fs.Get().IsValid() );
		}

		//TEST( des.IsEnd() );
		//TEST( CompareWithDump( ser_str, "test1_ref.txt", force_update ));
	}
}


extern void Test_FeatureSetPack ()
{
#ifdef AE_PIPELINE_COMPILER_LIBRARY
	{
		Library		lib;
		TEST( lib.Load( AE_PIPELINE_COMPILER_LIBRARY ));
		TEST( lib.GetProcAddr( "CompilePipelines", OUT compile_pipelines ));

		TEST( FileSystem::SetCurrentPath( Path{AE_CURRENT_DIR} / "featset_test" ));

		FeatureSetPack_Test1();
	}
	TEST_PASSED();
#endif
}
