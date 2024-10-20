#include <offline_packer.as>

void ASmain ()
{
	const string	output = GetOutputDir();
	array<string>	suffix;

	if ( IsGLSLCompilerSupported() )
		suffix.push_back( "vk" );

	if ( IsMetalCompilerSupported() )
		suffix.push_back( "mtl" );

	Assert( ! suffix.empty() );

	// pipeline compiler
	for (uint i = 0; i < suffix.size(); ++i)
	{
		RC<PipelineCompiler>	ppln = PipelineCompiler();
		ppln.AddPipelineFolderRecursive( GetSharedFeatureSetPath() );
		ppln.AddPipeline( "config_" + suffix[i] + ".as" );
		ppln.AddPipelineFolder( "sampler" );
		ppln.AddPipelineFolder( "rtech" );
		ppln.AddPipelineFolder( "pipeline" );
		ppln.AddShaderFolder( "shaders" );
		ppln.SetOutputCPPFile( "cpp/" + suffix[i] + "_types.h",  "cpp/" + suffix[i] + "_names.h",  EReflectionFlags::All );
		ppln.CompileWithNameMapping( output + "Tests.Graphics." + suffix[i] + "Pipelines.bin" );
	}
}
