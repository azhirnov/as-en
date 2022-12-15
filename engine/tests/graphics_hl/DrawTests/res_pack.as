
void main ()
{
	const string	output = GetOutputDir();

	array<string>	suffix;
	suffix.push_back( "vk" );
	suffix.push_back( "mac" );
	
	// pipeline compiler
	for (uint i = 0; i < suffix.size(); ++i)
	{
		ScriptPipelineCompiler@		ppln = ScriptPipelineCompiler();
		ppln.AddPipelineFolder( GetSharedFeatureSetPath(), EPathParamsFlags::Recursive );
		ppln.AddPipeline( "config_" + suffix[i] + ".as" );
		ppln.AddPipelineFolder( "sampler" );
		ppln.AddPipeline( GetCanvasVerticesPath() );
		ppln.AddPipelineFolder( "rtech" );
		ppln.AddPipelineFolder( "pipeline" );
		ppln.AddShaderFolder( "shaders" );
		ppln.CompileWithNameMapping( output + suffix[i] + "/pipelines.bin", "cpp/" + suffix[i] + "_types.h" );
	}
}
