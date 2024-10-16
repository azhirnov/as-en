// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_Common.h"
using namespace AE::PipelineCompiler;

namespace
{
	const bool						force_update		= true;
	decltype(&CompilePipelines)		compile_pipelines	= null;

	using EMarker = PipelineStorage::EMarker;

	static void  PipelinePack_Test (bool isVk, StringView refName)
	{
		TEST( FileSystem::SetCurrentPath( Path{AE_CURRENT_DIR} / "pipeline_test" ));

		const PathParams	pipelines[]			= { {isVk ? TXT("config_vk.as") : TXT("config_mac.as"), 1},
													{TXT("../sampler_test/samplers.as"), 2},
													{TXT("../rp_test/rpass.as"), 3},
													{TXT("pipelines"), 10, EPathParamsFlags::Folder},
												    {TXT( AE_SHARED_DATA "/feature_set" ), 0, EPathParamsFlags::RecursiveFolder},
													{TXT("rtech"), 5, EPathParamsFlags::Folder},
												    {TXT("layouts"), 4, EPathParamsFlags::Folder} };
		const CharType *	shader_folder[]		= { TXT("shaders_glsl"), TXT("shaders_msl") };
		const CharType *	include_dir[]		= { TXT("shaders_glsl/include"), TXT("shaders_msl/include") };
		const Path			output_folder		= TXT("_output");
		const Path			output_script		= TXT( AE_SHARED_DATA "/scripts/pipeline_compiler.as" );
		const Path			ref_dump_fname		= FileSystem::ToAbsolute( refName );

		FileSystem::DeleteDirectory( output_folder );
		TEST( FileSystem::CreateDirectories( output_folder ));

		const Path	output			= FileSystem::ToAbsolute( output_folder / "pipelines.bin" );
		const Path	output_cpp		= FileSystem::ToAbsolute( output_folder / ".." / (isVk ? "vk_types.h" : "mtl_types.h" ));
		const Path	output_names	= FileSystem::ToAbsolute( output_folder / ".." / (isVk ? "vk_names.h" : "mtl_names.h" ));

		PipelinesInfo	info		= {};
		info.inPipelines			= pipelines;
		info.inPipelineCount		= CountOf( pipelines );
		info.shaderIncludeDirs		= include_dir;
		info.shaderIncludeDirCount	= CountOf( include_dir );
		info.shaderFolders			= shader_folder;
		info.shaderFolderCount		= CountOf( shader_folder );
		info.outputPackName			= Cast<CharType>(output.c_str());
		info.outputCppStructsFile	= Cast<CharType>(output_cpp.c_str());
		info.outputCppNamesFile		= Cast<CharType>(output_names.c_str());
		info.outputScriptFile		= Cast<CharType>(output_script.c_str());
		info.cppReflectionFlags		= EReflectionFlags::All;
		info.addNameMapping			= true;

		TEST( compile_pipelines( &info ));

		auto	file = MakeRC<FileRStream>( output );
		TEST( file->IsOpen() );

		auto					mem_stream = MakeRC<ArrayRStream>();
		PipelinePackOffsets		offsets;
		{
			uint	name;
			TEST( file->Read( OUT name ));
			TEST_Eq( name, PackOffsets_Name );

			TEST( file->Read( OUT offsets ));
			TEST( offsets.pipelineOffset < ulong(file->Size()) );

			TEST( file->SeekSet( Bytes{offsets.pipelineOffset} ));
			TEST( mem_stream->LoadRemainingFrom( *file, Bytes{offsets.pipelineDataSize} ));
		}

		HashToName	hash_to_name;
		{
			auto	mem_stream2 = MakeRC<ArrayRStream>();
			TEST( file->SeekSet( Bytes{offsets.nameMappingOffset} ));
			TEST( mem_stream2->LoadRemainingFrom( *file, Bytes{offsets.nameMappingDataSize} ));

			Serializing::Deserializer	des{ mem_stream2 };

			uint	name;
			TEST( des( OUT name ));
			TEST_Eq( name, NameMapping_Name );
			TEST( hash_to_name.Deserialize( des ));
		}

		PipelineStorage::BlockOffsets_t		block_offsets;
		{
			DefaultPackHeader	hdr;
			TEST( mem_stream->Read( OUT hdr ));

			TEST( hdr.name == PipelinePack_Name );
			TEST( hdr.ver == PipelinePack_Version );

			TEST( mem_stream->Read( OUT block_offsets ));
		}

		RC<IAllocator>		alloc = MakeRC<LinearAlloc_t>();
		String				ser_str;
		EnumSet< EMarker >	unique;
		unique.insert( EMarker::Unknown );

		for (usize i = 0; i < block_offsets.size(); ++i)
		{
			const auto	base_off = block_offsets[i];
			if ( base_off == UMax )
				continue;

			EMarker						marker;
			Serializing::Deserializer	des{ mem_stream->ToSubStream( base_off ), alloc.get() };

			TEST( des( OUT marker ));
			TEST_Eq( usize(marker), i );

			TEST_Lt( uint(marker), unique.size() );
			TEST( not unique.contains( marker ));

			unique.insert( marker );

			switch_enum( marker )
			{
				case EMarker::RenderStates :
				{
					Array< SerializableRenderState >	rstates;
					TEST( des( OUT rstates ));
					ser_str << ArrayToString( "RenderStates", rstates, hash_to_name );
					break;
				}

				case EMarker::DepthStencilStates :
				{
					Array< SerializableDepthStencilState >	ds_states;
					TEST( des( OUT ds_states ));
					ser_str << ArrayToString( "DepthStencilStates", ds_states, hash_to_name );
					break;
				}

				case EMarker::RenderTechniques :
				{
					Array< SerializableRenderTechnique >	rtech;
					TEST( des( OUT rtech ));
					ser_str << ArrayToString( "RenderTechniques", rtech, hash_to_name );
					break;
				}

				case EMarker::DescrSetLayouts :
				{
					Array< DescriptorSetLayoutDesc >	ds_layouts;
					TEST( des( OUT ds_layouts ));
					ser_str << ArrayToString( "DescrSetLayouts", ds_layouts, hash_to_name );
					break;
				}

				case EMarker::PipelineLayouts :
				{
					Array< PipelineLayoutDesc >		ppln_layouts;
					TEST( des( OUT ppln_layouts ));
					ser_str << ArrayToString( "PipelineLayouts", ppln_layouts, hash_to_name );
					break;
				}

				case EMarker::PipelineTemplNames :
				{
					Array<Pair< PipelineTmplName, PipelineTemplUID >>	ppln_names;
					TEST( des( OUT ppln_names ));

					ser_str << "\nPipelineTemplNames {";
					for (usize j = 0; j < ppln_names.size(); ++j) {
						ser_str << "\n  [" << ToString(j) << "]  '" << hash_to_name( ppln_names[j].first ) << "', " << ToString<16>(usize(ppln_names[j].second));
					}
					ser_str << "\n}\n";
					break;
				}

				case EMarker::GraphicsPipelineTempl :
				{
					Array< SerializableGraphicsPipeline >	gpipelines;
					TEST( des( OUT gpipelines ));
					ser_str << ArrayToString( "GraphicsPipelineTempl", gpipelines, hash_to_name );
					break;
				}

				case EMarker::MeshPipelineTempl :
				{
					Array< SerializableMeshPipeline >		mpipelines;
					TEST( des( OUT mpipelines ));
					ser_str << ArrayToString( "MeshPipelineTempl", mpipelines, hash_to_name );
					break;
				}

				case EMarker::ComputePipelineTempl :
				{
					Array< SerializableComputePipeline >	cpipelines;
					TEST( des( OUT cpipelines ));
					ser_str << ArrayToString( "ComputePipelineTempl", cpipelines, hash_to_name );
					break;
				}

				case EMarker::TilePipelineTempl :
				{
					Array< SerializableTilePipeline >	tpipelines;
					TEST( des( OUT tpipelines ));
					ser_str << ArrayToString( "TilePipelineTempl", tpipelines, hash_to_name );
					break;
				}

				case EMarker::RayTracingPipelineTempl :
				{
					Array< SerializableRayTracingPipeline >	rtpipelines;
					TEST( des( OUT rtpipelines ));
					ser_str << ArrayToString( "RayTracingPipelineTempl", rtpipelines, hash_to_name );
					break;
				}

				case EMarker::GraphicsPipelineSpec :
				{
					Array< SerializableGraphicsPipelineSpec >	gpipeline_spec;
					TEST( des( OUT gpipeline_spec ));
					ser_str << ArrayToString( "GraphicsPipelineSpec", gpipeline_spec, hash_to_name );
					break;
				}

				case EMarker::MeshPipelineSpec :
				{
					Array< SerializableMeshPipelineSpec >		mpipeline_spec;
					TEST( des( OUT mpipeline_spec ));
					ser_str << ArrayToString( "MeshPipelineSpec", mpipeline_spec, hash_to_name );
					break;
				}

				case EMarker::ComputePipelineSpec :
				{
					Array< SerializableComputePipelineSpec >	cpipeline_spec;
					TEST( des( OUT cpipeline_spec ));
					ser_str << ArrayToString( "ComputePipelineSpec", cpipeline_spec, hash_to_name );
					break;
				}

				case EMarker::TilePipelineSpec :
				{
					Array< SerializableTilePipelineSpec >		tpipeline_spec;
					TEST( des( OUT tpipeline_spec ));
					ser_str << ArrayToString( "TilePipelineSpec", tpipeline_spec, hash_to_name );
					break;
				}

				case EMarker::RayTracingPipelineSpec :
				{
					Array< SerializableRayTracingPipelineSpec >	rtpipeline_spec;
					TEST( des( OUT rtpipeline_spec ));
					ser_str << ArrayToString( "RayTracingPipelineSpec", rtpipeline_spec, hash_to_name );
					break;
				}

				case EMarker::SpirvShaders :
				{
					Array< ShaderBytecode >	spirv_shaders;
					TEST( des( OUT spirv_shaders ));

					ser_str << ArrayToString( "SpirvShaders", spirv_shaders, hash_to_name );
					{
						TEST_Lt( offsets.shaderOffset, ulong(file->Size()) );
						TEST( file->SeekSet( Bytes{offsets.shaderOffset} ));

						uint	version = 0;
						uint	name	= 0;
						TEST( file->Read( OUT name ) and file->Read( OUT version ));
						TEST_Eq( name, ShaderPack_Name );
						TEST_Eq( version, ShaderPack_Version );
					}

					// read shader data
					{
						offsets.shaderOffset += 8;	// skip name + version

						ser_str << "\nSpirvShaderData {";

						uint	idx = 0;
						for (auto& sh : spirv_shaders)
						{
							sh.offset += offsets.shaderOffset;
							TEST( sh.ReadData( *file ));

							ser_str << "\n  [" << ToString(idx++) << "]" << sh.ToString2( hash_to_name );
						}
						ser_str << "\n}";
					}
					break;
				}

				case EMarker::MetaliOSShaders :
				{
					Array< ShaderBytecode >	metal_shaders;
					TEST( des( OUT metal_shaders ));

					ser_str << ArrayToString( "MetaliOSShaders", metal_shaders, hash_to_name );
					{
						TEST_Lt( offsets.shaderOffset, ulong(file->Size()) );
						TEST( file->SeekSet( Bytes{offsets.shaderOffset} ));

						uint	version = 0;
						uint	name	= 0;
						TEST( file->Read( OUT name ) and file->Read( OUT version ));
						TEST_Eq( name, ShaderPack_Name );
						TEST_Eq( version, ShaderPack_Version );
					}

					// read shader data
					{
						offsets.shaderOffset += 8;	// skip name + version

						ser_str << "\nMetaliOSShaderData {";

						uint	idx = 0;
						for (auto& sh : metal_shaders)
						{
							sh.offset += offsets.shaderOffset;
							TEST( sh.ReadData( *file ));

							ser_str << "\n  [" << ToString(idx++) << "]" << sh.ToString2( hash_to_name );
						}
						ser_str << "\n}";
					}
					break;
				}

				case EMarker::MetalMacShaders :
				{
					Array< ShaderBytecode >	metal_shaders;
					TEST( des( OUT metal_shaders ));

					ser_str << ArrayToString( "MetalMacShaders", metal_shaders, hash_to_name );
					{
						TEST_Lt( offsets.shaderOffset, ulong(file->Size()) );
						TEST( file->SeekSet( Bytes{offsets.shaderOffset} ));

						uint	version = 0;
						uint	name	= 0;
						TEST( file->Read( OUT name ) and file->Read( OUT version ));
						TEST_Eq( name, ShaderPack_Name );
						TEST_Eq( version, ShaderPack_Version );
					}

					// read shader data
					{
						offsets.shaderOffset += 8;	// skip name + version

						ser_str << "\nMetalMacShaderData {";

						uint	idx = 0;
						for (auto& sh : metal_shaders)
						{
							sh.offset += offsets.shaderOffset;
							TEST( sh.ReadData( *file ));

							ser_str << "\n  [" << ToString(idx++) << "]" << sh.ToString2( hash_to_name );
						}
						ser_str << "\n}";
					}
					break;
				}

				case EMarker::RTShaderBindingTable :
				{
					Array< SerializableRTShaderBindingTable >	sbt;
					TEST( des( OUT sbt ));
					ser_str << ArrayToString( "RTShaderBindingTable", sbt, hash_to_name );
					break;
				}

				case EMarker::Unknown :
				case EMarker::_Count :
				default :				DBG_WARNING( "unknown marker" );
			}
			switch_end
		}

		TEST( CompareWithDump( ser_str, ref_dump_fname, force_update ));
	}


	static void  PipelinePack_Test1 ()
	{
		PipelinePack_Test( true, "test1_ref.txt" );
	}

	static void  PipelinePack_Test2 ()
	{
	#ifdef AE_METAL_TOOLS
		PipelinePack_Test( false, "test2_ref.txt" );
	#endif
	}
}


extern void Test_PipelinePack ()
{
#ifdef AE_PIPELINE_COMPILER_LIBRARY
	{
		Library		lib;
		TEST( lib.Load( AE_PIPELINE_COMPILER_LIBRARY ));
		TEST( lib.GetProcAddr( "CompilePipelines", OUT compile_pipelines ));

		PipelinePack_Test1();
		PipelinePack_Test2();
	}
	TEST_PASSED();
#endif
}


#include "base/Math/Vec.h"
#include "base/Math/Matrix.h"
#include "base/Math/MatrixStorage.h"
using namespace AE::Math;

namespace VkTypes {
# include "pipeline_test/vk_types.h"
}
namespace MtlTypes {
# include "pipeline_test/mtl_types.h"
}
