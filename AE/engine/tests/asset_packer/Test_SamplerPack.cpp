// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"
using namespace AE::PipelineCompiler;

namespace
{
	const bool						force_update		= true;
	decltype(&CompilePipelines)		compile_pipelines	= null;


	static void  SamplerPack_Test1 ()
	{
		const PathParams	sampler_files[]	= { {TXT( AE_SHARED_DATA "/feature_set" ), 1, EPathParamsFlags::RecursiveFolder},
												{TXT("config_vk.as"), 0},
											    {TXT("samplers.as"), 2} };
		const Path			output_folder	= TXT("_output");

		FileSystem::DeleteDirectory( output_folder );
		TEST( FileSystem::CreateDirectories( output_folder ));

		const Path	output = output_folder / "samplers.bin";

		PipelinesInfo	info;
		info.inPipelines		= sampler_files;
		info.inPipelineCount	= CountOf( sampler_files );
		info.outputPackName		= Cast<CharType>(output.c_str());
		info.addNameMapping		= true;

		TEST( compile_pipelines( &info ));

		auto	file = MakeRC<FileRStream>( output );
		TEST( file->IsOpen() );

		HashToName	hash_to_name;
		auto		mem_stream = MakeRC<ArrayRStream>();
		{
			uint	name;
			TEST( file->Read( OUT name ));
			TEST_Eq( name, PackOffsets_Name );

			PipelinePackOffsets		offsets;
			TEST( file->Read( OUT offsets ));
			TEST_Lt( offsets.samplerOffset, ulong(file->Size()) );
			TEST_Lt( offsets.nameMappingOffset, ulong(file->Size()) );

			auto	mem_stream2 = MakeRC<ArrayRStream>();
			TEST( file->SeekSet( Bytes{offsets.nameMappingOffset} ));
			TEST( mem_stream2->LoadRemainingFrom( *file, Bytes{offsets.nameMappingDataSize} ));

			Serializing::Deserializer	des{ mem_stream2 };
			TEST( des( OUT name ));
			TEST_Eq( name, NameMapping_Name );
			TEST( hash_to_name.Deserialize( des ));

			TEST( file->SeekSet( Bytes{offsets.samplerOffset} ));
			TEST( mem_stream->LoadRemainingFrom( *file, Bytes{offsets.samplerDataSize} ));
		}

		RC<IAllocator>					alloc = MakeRC<LinearAlloc_t>();
		AE::Serializing::Deserializer	des{ mem_stream, alloc.get() };
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

		TEST_Eq( samp_names.size(), 10 );
		TEST_Eq( samplers.size(), 10 );

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
		Library		lib;
		TEST( lib.Load( AE_PIPELINE_COMPILER_LIBRARY ));
		TEST( lib.GetProcAddr( "CompilePipelines", OUT compile_pipelines ));

		TEST( FileSystem::SetCurrentPath( Path{AE_CURRENT_DIR} / "sampler_test" ));

		SamplerPack_Test1();
	}
	TEST_PASSED();
#endif
}
