// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"


namespace
{
	const bool						force_update		= true;
	decltype(&CompilePipelines)		compile_pipelines	= null;


	static void  MaterialPack_Test1 (bool isVk, StringView refName)
	{
		const PathParams	fs_folder[]		= { {TXT( AE_SHARED_DATA "/feature_set" ), 1, EPathParamsFlags::Recursive} };
		const PathParams	rpass_dir[]		= { {isVk ? TXT("config_vk.as") : TXT("config_mac.as"), 0},
											    {TXT("rpass.as"), 2} };
		const Path			output_folder	= TXT("_output");
		const Path			ref_dump_fname	= FileSystem::ToAbsolute( refName );
		
		FileSystem::RemoveAll( output_folder );
		TEST( FileSystem::CreateDirectories( output_folder ));
		
		const Path	output		= output_folder / "materials.bin";
		const Path	output_cpp	= FileSystem::ToAbsolute( output_folder / ".." / (isVk ? "vk_types.h" : "mtl_types.h" ));

		PipelinesInfo	info;
		info.pipelineFolders	= fs_folder;
		info.pipelineFolderCount= CountOf( fs_folder );
		info.inPipelines		= rpass_dir;
		info.inPipelineCount	= CountOf( rpass_dir );
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
			TEST_Lt( offsets.renderPassOffset, ulong(file->Size()) );
			
			auto	mem_stream2 = MakeRC<MemRStream>();
			TEST( file->SeekSet( Bytes{offsets.nameMappingOffset} ));
			TEST( mem_stream2->LoadRemaining( *file, Bytes{offsets.nameMappingDataSize} ));

			Serializing::Deserializer	des{ mem_stream2 };
			TEST( des( OUT name ));
			TEST_Eq( name, NameMapping_Name );
			TEST( hash_to_name.Deserialize( des ));

			TEST( file->SeekSet( Bytes{offsets.renderPassOffset} ));
			TEST( mem_stream->LoadRemaining( *file, Bytes{offsets.renderPassDataSize} ));
		}
		
		AE::Serializing::Deserializer	des{ mem_stream, MakeRC<LinearAlloc_t>() };
		{
			uint	version = 0;
			uint	name	= 0;
			TEST( des( OUT name, OUT version ));
			TEST_Eq( name, RenderPassPack_Name );
			TEST_Eq( version, RenderPassPack_Version );
		}
		
		TEST( des.IsEnd() );
		//TEST( CompareWithDump( ser_str, ref_dump_fname, force_update ));
	}
}


extern void Test_MaterialPack ()
{
#ifdef AE_PIPELINE_COMPILER_LIBRARY
	{
		Path	dll_path{ AE_PIPELINE_COMPILER_LIBRARY };
		dll_path.append( CMAKE_INTDIR "/PipelineCompiler-shared.dll" );

		Library		lib;
		TEST( lib.Load( dll_path ));
		TEST( lib.GetProcAddr( "CompilePipelines", OUT compile_pipelines ));
		
		TEST( FileSystem::SetCurrentPath( AE_CURRENT_DIR "/material_test" ));

		//MaterialPack_Test1();
	}
	TEST_PASSED();
#endif
}
