#include <pipeline_compiler>

void main ()
{
	RC<ComputePipeline>		ppln = ComputePipeline( "compute_2" );
	ppln.AddFeatureSet( "MinDesktop" );
	ppln.SetLayout( "Compute_PL_2" );

	RC<Shader>	cs = Shader();
	cs.SetComputeSpecAndDefault( 8, 8 );
	cs.file		= "compute_2.glsl";
	cs.version	= EShaderVersion::SPIRV_1_0;
	ppln.SetShader( cs );

	// specialization
	{
		RC<ComputePipelineSpec>		spec = ppln.AddSpecialization( "compute_2.def" );
		
		spec.AddToRenderTech( "MinForward", "Compute1" );
	}
}
