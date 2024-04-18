// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <offline_packer.as>

void ASmain ()
{
	const string	output		= GetOutputDir();
	const string	output_temp	= output + "temp/";

	array<string>	suffix;
	suffix.push_back( "vk" );
	suffix.push_back( "mac" );

	Archive		archive;
	archive.SetTempFile( output_temp + "archive-1.tmp" );

	// pipeline compiler
	for (uint i = 0; i < suffix.size(); ++i)
	{
		// render passes
		{
			RC<PipelineCompiler>	ppln = PipelineCompiler();
			ppln.AddPipelineFolderRecursive( GetSharedFeatureSetPath() );
			ppln.AddPipeline( "config_" + suffix[i] + ".as" );
			ppln.AddPipeline( "rtech/render_passes.as" );
			ppln.AddPipelineFolder( "samplers" );

			const string  fname = output_temp + suffix[i] + "/render_passes.bin";
			ppln.CompileWithNameMapping( fname );
			archive.Add( suffix[i] + "/render_passes", fname );
		}

		// pipelines
		{
			RC<PipelineCompiler>	ppln = PipelineCompiler();
			ppln.AddPipelineFolderRecursive( GetSharedFeatureSetPath() );
			ppln.AddPipeline( "config_" + suffix[i] + ".as" );
			ppln.AddPipeline( "rtech/render_passes.as" );
			ppln.AddPipeline( "rtech/ren_tech.as" );
			ppln.AddPipelineFolder( "samplers" );
			ppln.AddPipeline( GetCanvasVerticesPath() );
			ppln.AddPipelineFolder( "layouts" );
			ppln.AddPipelineFolder( "pipelines" );
			ppln.AddShaderFolder( "shaders" );
			ppln.ShaderIncludeDir( GetSharedShadersPath() );
			ppln.SetOutputCPPFile( "cpp/" + suffix[i] + "_types.h",  "cpp/" + suffix[i] + "_names.h",  EReflectionFlags::All );

			const string  fname = output_temp + suffix[i] + "/pipelines.bin";
			ppln.CompileWithNameMapping( fname );
			archive.Add( suffix[i] + "/pipelines", fname );
		}
	}

	// input actions
	{
		RC<InputActions>		iact = InputActions();
		iact.Add( "controls/android.as" );
		iact.Add( "controls/glfw.as" );
		iact.Add( "controls/winapi.as" );
		//iact.Add( "controls/openvr.as" );
		//iact.Add( "controls/openxr.as" );
		iact.SetOutputCPPFile( "cpp/ia_names.h" );

		const string  fname = output_temp + "controls.bin";
		iact.Convert( fname );
		archive.Add( "controls", fname );
	}

	// asset packer
	{
		RC<AssetPacker>		apack = AssetPacker();

		apack.SetTempFile( output_temp + "archive-2.tmp" );

		apack.AddFolder( "images" );
		apack.AddFolder( "fonts" );

		const string  fname = output_temp + "archive-3.tmp";
		apack.ToArchive( fname );
		archive.AddArchive( fname );
	}

	// VFS
	{
        archive.Add( "Sound.ogg", "audio/Sound.ogg" );
	}

	archive.Store( output + "resources.bin" );

	DeleteFolder( output_temp );
}
