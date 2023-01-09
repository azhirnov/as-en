#include <pipeline_compiler>

#if SCRIPT
void main ()
{
	RC<ComputePipeline>	ppln = ComputePipeline( "compute_1" );
	ppln.AddFeatureSet( "MinimalFS" );

	RC<Shader>	cs = Shader();
	cs.SetComputeSpecAndDefault( 8, 8 );

	if ( IsVulkan() )
	{
		ppln.SetLayout( "Compute_PL_1_Dbg" );

		cs.options	= EShaderOpt::Trace;
		cs.file		= "compute_1.glsl";
		cs.version	= EShaderVersion::SPIRV_1_0;
		ppln.SetShader( cs );
	}

	if ( IsMetal() )
	{
		ppln.SetLayout( "Compute_PL_1" );

		cs.options	= EShaderOpt::Optimize;
		cs.file		= "compute_1.msl";
		cs.version	= EShaderVersion::Metal_2_0;
		ppln.SetShader( cs );
	}

	// specialization
	{
		RC<ComputePipelineSpec>		spec = ppln.AddSpecialization( "compute_1.def" );

		spec.AddToRenderTech( "MinForward", "Compute1" );
	}
}
#endif
