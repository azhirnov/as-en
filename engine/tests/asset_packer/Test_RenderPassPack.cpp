// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"


namespace
{
	const bool						force_update		= true;
	decltype(&CompilePipelines)		compile_pipelines	= null;


	static void  RenderPassPack_Test (bool isVk, StringView refName)
	{
		const PathParams	fs_folder[]		= { {TXT( AE_SHARED_DATA "/feature_set" ), 1, EPathParamsFlags::Recursive} };
		const PathParams	rpass_dir[]		= { {isVk ? TXT("config_vk.as") : TXT("config_mac.as"), 0}, {TXT("rpass.as"), 2} };
		const Path			output_folder	= TXT("_output");
		const Path			ref_dump_fname	= FileSystem::ToAbsolute( refName );
		
		FileSystem::RemoveAll( output_folder );
		TEST( FileSystem::CreateDirectories( output_folder ));
		
		const Path	output = output_folder / "passes.bin";

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
			TEST( file->Read( OUT name ) and name == PackOffsets_Name );

			PipelinePackOffsets		offsets;
			TEST( file->Read( OUT offsets ));
			TEST( offsets.renderPassOffset < ulong(file->Size()) );
			
			auto	mem_stream2 = MakeRC<MemRStream>();
			TEST( file->SeekSet( Bytes{offsets.nameMappingOffset} ));
			TEST( mem_stream2->Load( *file, Bytes{offsets.nameMappingDataSize} ));

			Serializing::Deserializer	des{ mem_stream2 };
			TEST( des( OUT name ) and name == NameMapping_Name );
			TEST( hash_to_name.Deserialize( des ));

			TEST( file->SeekSet( Bytes{offsets.renderPassOffset} ));
			TEST( mem_stream->Load( *file, Bytes{offsets.renderPassDataSize} ));
		}
		
		AE::Serializing::Deserializer	des{ mem_stream, MakeRC<LinearAlloc_t>() };
		{
			uint	version = 0;
			uint	name	= 0;
			TEST( des( OUT name, OUT version ));
			TEST( name == RenderPassPack_Name );
			TEST( version == RenderPassPack_Version );
		}

		uint	compat_rp_count = 0;
		TEST( des( OUT compat_rp_count ));
		
		String	ser_str;
		for (uint i = 0; i < compat_rp_count; ++i)
		{
			SerializableRenderPassInfo	rp_info;
			TEST( rp_info.Deserialize( des ));
			ser_str += rp_info.ToString( hash_to_name );
			
			#ifdef AE_ENABLE_VULKAN
			if ( isVk )
			{
				uint	vk_ver		= 0;
				uint	spec_count	= 0;
				TEST( des( vk_ver, spec_count ));
				TEST( vk_ver == RenderPassPack_VkRpBlock );
				
				SerializableVkRenderPass	vk_compat;
				TEST( vk_compat.Deserialize( des ));
				ser_str += vk_compat.ToString( hash_to_name );
				
				for (uint j = 0; j < spec_count; ++j)
				{
					SerializableVkRenderPass	vk_rp;
					TEST( vk_rp.Deserialize( des ));
					ser_str += vk_rp.ToString( hash_to_name );
				}
			}
			#endif

			// Metal
			if ( not isVk )
			{
				uint	mtl_ver		= 0;
				uint	spec_count	= 0;
				TEST( des( mtl_ver, spec_count ));
				TEST( mtl_ver == RenderPassPack_MtlRpBlock );
				
				SerializableMtlRenderPass	mtl_compat;
				TEST( mtl_compat.Deserialize( des ));
				ser_str += mtl_compat.ToString( hash_to_name );
				
				for (uint j = 0; j < spec_count; ++j)
				{
					SerializableMtlRenderPass	mtl_rp;
					TEST( mtl_rp.Deserialize( des ));
					ser_str += mtl_rp.ToString( hash_to_name );
				}
			}
		}
		
		TEST( des.IsEnd() );
		TEST( CompareWithDump( ser_str, ref_dump_fname, force_update ));
	}


	static void  RenderPassPack_Test1 ()
	{
		RenderPassPack_Test( true, "test1_ref.txt" );
	}
	
	static void  RenderPassPack_Test2 ()
	{
		RenderPassPack_Test( false, "test2_ref.txt" );
	}
}


extern void Test_RenderPassPack ()
{
	{
		Path	dll_path{ AE_PIPELINE_COMPILER_LIBRARY };
		dll_path.append( AE_RESPACK_BUILD_TYPE "/PipelineCompiler-shared.dll" );

		Library		lib;
		TEST( lib.Load( dll_path ));
		TEST( lib.GetProcAddr( "CompilePipelines", OUT compile_pipelines ));
		
		TEST( FileSystem::SetCurrentPath( AE_CURRENT_DIR "/rp_test" ));

		RenderPassPack_Test1();
		RenderPassPack_Test2();
	}
	TEST_PASSED();
}
