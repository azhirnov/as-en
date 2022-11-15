// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"


namespace
{
	const bool						force_update		= true;
	decltype(&CompilePipelines)		compile_pipelines	= null;


	static void  FeatureSetPack_Test1 ()
	{
		const PathParams	features[]	= { {TXT("config_vk.as"), 0} };
		const PathParams	fs_folder[]	= { {TXT( AE_SHARED_DATA "/feature_set" ), 1, EPathParamsFlags::Recursive} };
		Path				output		= TXT("_output");
		
		FileSystem::RemoveAll( output );
		TEST( FileSystem::CreateDirectories( output ));

		output.append( "features.bin" );

		PipelinesInfo	info = {};
		info.inPipelines		= features;
		info.inPipelineCount	= CountOf( features );
		info.pipelineFolders	= fs_folder;
		info.pipelineFolderCount= CountOf( fs_folder );
		info.outputPackName		= output.c_str();

		TEST( compile_pipelines( &info ));
		

		auto	file = MakeRC<FileRStream>( output );
		TEST( file->IsOpen() );
		
		auto	mem_stream = MakeRC<MemRStream>();
		{
			uint	name;
			TEST( file->Read( OUT name ));
			TEST_EQ( name, PackOffsets_Name );

			PipelinePackOffsets		offsets;
			TEST( file->Read( OUT offsets ));
			TEST_L( offsets.featureSetOffset, ulong(file->Size()) );

			TEST( file->SeekSet( Bytes{offsets.featureSetOffset} ));
			TEST( mem_stream->LoadRemaining( *file, Bytes{offsets.featureSetDataSize} ));
		}

		AE::Serializing::Deserializer	des{ mem_stream };
		{
			uint	version = 0;
			uint	name	= 0;
			TEST( des( OUT name, OUT version ));
			TEST_EQ( name, FeatureSetPack_Name );
			TEST_EQ( version, FeatureSetPack_Version );
		}

		uint	fs_size = 0;
		TEST( des( OUT fs_size ));
		TEST_EQ( fs_size, sizeof(FeatureSet) );

		uint	count = 0;
		TEST( des( OUT count ));
		TEST_EQ( count, 34 );
		
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
	{
		Path	dll_path{ AE_PIPELINE_COMPILER_LIBRARY };
		dll_path.append( AE_RESPACK_BUILD_TYPE "/PipelineCompiler-shared.dll" );

		Library		lib;
		TEST( lib.Load( dll_path ));
		TEST( lib.GetProcAddr( "CompilePipelines", OUT compile_pipelines ));
		
		TEST( FileSystem::SetCurrentPath( AE_CURRENT_DIR "/featset_test" ));

		FeatureSetPack_Test1();
	}
	TEST_PASSED();
}
