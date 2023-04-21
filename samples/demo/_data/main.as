#include <offline_packer>

void ASmain ()
{
	const string	output		= GetOutputDir();
	const string	output_temp	= output + "temp/";

	array<string>	suffix;
	suffix.push_back( "vk" );
	suffix.push_back( "mac" );

	Archive		archive;
	archive.SetTempFile( output_temp + "archive1.tmp" );

	// pipeline compiler
	for (uint i = 0; i < suffix.size(); ++i)
	{
		// render passes
		{
			RC<PipelineCompiler>	ppln = PipelineCompiler();
			ppln.AddPipelineFolder( GetSharedFeatureSetPath(), EPathParamsFlags::Recursive );
			ppln.AddPipeline( "config_" + suffix[i] + ".as" );
			ppln.AddPipeline( "rtech/render_passes.as" );
			ppln.AddPipelineFolder( "samplers" );
			ppln.CompileWithNameMapping( output_temp + suffix[i] + "/render_passes.bin" );
			archive.Add( suffix[i] + "/render_passes", output_temp + suffix[i] + "/render_passes.bin" );
		}

		// pipelines
		{
			RC<PipelineCompiler>	ppln = PipelineCompiler();
			ppln.AddPipelineFolder( GetSharedFeatureSetPath(), EPathParamsFlags::Recursive );
			ppln.AddPipeline( "config_" + suffix[i] + ".as" );
			ppln.AddPipeline( "rtech/render_passes.as" );
			ppln.AddPipeline( "rtech/ren_tech.as" );
			ppln.AddPipelineFolder( "samplers" );
			ppln.AddPipeline( GetCanvasVerticesPath() );
			ppln.AddPipelineFolder( "layouts" );
			ppln.AddPipelineFolder( "pipelines" );
			ppln.AddShaderFolder( "shaders" );
			ppln.IncludeDir( GetSharedShadersPath() );
			ppln.CompileWithNameMapping( output_temp + suffix[i] + "/pipelines.bin", "cpp/" + suffix[i] + "_types.h" );
			archive.Add( suffix[i] + "/pipelines", output_temp + suffix[i] + "/pipelines.bin" );
		}
	}

	// input actions
	{
		RC<InputActions>		iact = InputActions();
		iact.Add( "controls/android.as" );
		iact.Add( "controls/glfw.as" );
		iact.Add( "controls/winapi.as" );
		iact.Add( "controls/openvr.as" );
		//iact.Add( "controls/openxr.as" );
		iact.Convert( output_temp + "controls.bin" );
		archive.Add( "controls", output_temp + "controls.bin" );
	}
	
	// asset packer
	{
		RC<AssetPacker>		apack = AssetPacker();

		apack.SetTempFile( output_temp + "archive2.tmp" );

		apack.AddFolder( "images" );
		apack.AddFolder( "fonts" );

		apack.ToArchive( output_temp + "archive3.tmp" );
		archive.AddArchive( output_temp + "archive3.tmp" );
	}

	archive.Store( output + "resources.bin" );

	DeleteFolder( output_temp );
}
