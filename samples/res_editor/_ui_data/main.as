// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <offline_packer>

void ASmain ()
{
	const string	output		= GetOutputDir();
	const string	output_temp	= output + "temp";

	array<string>	suffix;
	suffix.push_back( "vk" );
	suffix.push_back( "mac" );

	// pipeline compiler
	for (uint i = 0; i < suffix.size(); ++i)
	{
		// ui pipelines
		{
			RC<PipelineCompiler>	ppln = PipelineCompiler();
			ppln.AddPipelineFolder( GetSharedFeatureSetPath(), EPathParamsFlags::Recursive );
			ppln.AddPipeline( "config_" + suffix[i] + ".as" );
			ppln.AddPipeline( GetCanvasVerticesPath() );
			ppln.AddPipeline( "../_data/pipelines/VertexInput.as" );
			ppln.AddPipelineFolder( "pipelines" );
			ppln.AddShaderFolder( "shaders" );
			ppln.IncludeDir( GetSharedShadersPath() );
			ppln.CompileWithNameMapping( output + suffix[i] + "/ui_pipelines.bin", "cpp/" + suffix[i] + "_types.h" );
		}
	}

	// input actions
	{
		RC<InputActions>	iact = InputActions();
		iact.Add( "controls/glfw.as" );
		iact.Add( "controls/winapi.as" );
		iact.Convert( output + "controls.bin" );
	}
}
