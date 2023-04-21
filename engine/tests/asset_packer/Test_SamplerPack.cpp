// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"


namespace
{
	const bool						force_update		= true;
	decltype(&CompilePipelines)		compile_pipelines	= null;


	static void  SamplerPack_Test1 ()
	{
		const PathParams	fs_folder[]		= { {TXT( AE_SHARED_DATA "/feature_set" ), 1, EPathParamsFlags::Recursive} };
		const PathParams	sampler_files[]	= { {TXT("config_vk.as"), 0}, {TXT("samplers.as"), 2} };
		const Path			output_folder	= TXT("_output");
		
		FileSystem::RemoveAll( output_folder );
		TEST( FileSystem::CreateDirectories( output_folder ));
		
		const Path	output = output_folder / "samplers.bin";

		PipelinesInfo	info;
		info.pipelineFolders	= fs_folder;
		info.pipelineFolderCount= CountOf( fs_folder );
		info.inPipelines		= sampler_files;
		info.inPipelineCount	= CountOf( sampler_files );
		info.outputPackName		= output.c_str();
		info.addNameMapping		= true;

		TEST( compile_pipelines( &info ));
		
		auto	file = MakeRC<FileRStream>( output );
		TEST( file->IsOpen() );
		
		HashToName	hash_to_name;
		auto		mem_stream = MakeRC<MemRStream>();
		{
			uint	name;
			TEST( file->Read( OUT name ));
			TEST_Eq( name, PackOffsets_Name );

			PipelinePackOffsets		offsets;
			TEST( file->Read( OUT offsets ));
			TEST_Lt( offsets.samplerOffset, ulong(file->Size()) );
			TEST_Lt( offsets.nameMappingOffset, ulong(file->Size()) );
			
			auto	mem_stream2 = MakeRC<MemRStream>();
			TEST( file->SeekSet( Bytes{offsets.nameMappingOffset} ));
			TEST( mem_stream2->LoadRemaining( *file, Bytes{offsets.nameMappingDataSize} ));

			Serializing::Deserializer	des{ mem_stream2 };
			TEST( des( OUT name ));
			TEST_Eq( name, NameMapping_Name );
			TEST( hash_to_name.Deserialize( des ));

			TEST( file->SeekSet( Bytes{offsets.samplerOffset} ));
			TEST( mem_stream->LoadRemaining( *file, Bytes{offsets.samplerDataSize} ));
		}
		
		AE::Serializing::Deserializer	des{ mem_stream, MakeRC<LinearAlloc_t>() };
		{
			uint	version = 0;
			uint	name	= 0;
			TEST( des( OUT name, OUT version ));
			TEST_Eq( name, SamplerPack_Name );
			TEST_Eq( version, SamplerPack_Version );
		}

		Array<Pair< SamplerName, uint >>	samp_names;
		TEST( des( OUT samp_names ));

		Array<SamplerSerializer>	samplers;
		TEST( des( OUT samplers ));
		
		TEST_Eq( samp_names.size(), 8 );
		TEST_Eq( samplers.size(), 8 );
		
		String	ser_str;

		for (auto& [name, idx] : samp_names)
		{
			TEST_Lt( idx, samplers.size() );

			auto&	samp = samplers [idx];

			ser_str << "name:  '" << hash_to_name( name ) << "'\n";
			ser_str << samp.ToString( hash_to_name );
			ser_str << "--------------\n";
		}
		
		TEST( des.IsEnd() );
		TEST( CompareWithDump( ser_str, "test1_ref.txt", force_update ));
	}
}


extern void Test_SamplerPack ()
{
#ifdef AE_PIPELINE_COMPILER_LIBRARY
	{
		Path	dll_path{ AE_PIPELINE_COMPILER_LIBRARY };
		dll_path.append( CMAKE_INTDIR "/PipelineCompiler-shared.dll" );

		Library		lib;
		TEST( lib.Load( dll_path ));
		TEST( lib.GetProcAddr( "CompilePipelines", OUT compile_pipelines ));
		
		TEST( FileSystem::SetCurrentPath( AE_CURRENT_DIR "/sampler_test" ));

		SamplerPack_Test1();
	}
	TEST_PASSED();
#endif
}
