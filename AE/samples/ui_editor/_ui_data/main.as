// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <offline_packer.as>

void ASmain ()
{
	const string	output		= GetOutputDir();
	const string	output_temp	= output + "temp";

	array<string>	suffix;
	suffix.push_back( "vk" );
	//suffix.push_back( "mac" );

	// pipeline compiler
	for (uint i = 0; i < suffix.size(); ++i)
	{
		// ui pipelines
		{
			RC<PipelineCompiler>	ppln = PipelineCompiler();

			ppln.PipelineIncludeDir( GetSharedPipelinesPath() );
			ppln.AddPipelineFolderRecursive( GetSharedFeatureSetPath() );
			ppln.AddPipeline( "config_" + suffix[i] + ".as" );
			ppln.AddPipeline( GetCanvasVerticesPath() );
			ppln.AddPipelineFolder( "pipelines" );

			ppln.ShaderIncludeDir( GetSharedShadersPath() );
			ppln.ShaderIncludeDir( GetSharedPipelinesPath() );
			//ppln.AddShaderFolder( "shaders" );

			ppln.SetOutputCPPFile( "cpp/" + suffix[i] + "_types.h",  "cpp/" + suffix[i] + "_names.h",  EReflectionFlags::All );
			ppln.AddNameMapping();
			ppln.Compile( output + suffix[i] + "/ui_pipelines.bin" );
		}
	}

	// input actions
	{
		RC<InputActions>	iact = InputActions();
		iact.Add( "controls/glfw.as" );
		iact.Add( "controls/winapi.as" );
		iact.Include( GetUIBindingsPath() );

		iact.SetOutputCPPFile( "cpp/ia_names.h" );
		iact.Convert( output + "controls.bin" );
	}
}
