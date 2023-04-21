#include <pipeline_compiler>

void ASmain ()
{
	RC<ComputePipeline>		ppln = ComputePipeline( "compute_3" );
	ppln.AddFeatureSet( "MinRecursiveRayTracing" );
	ppln.SetLayout( "Compute_PL_3" );

	RC<Shader>	cs = Shader();
	cs.SetComputeSpecAndDefault( 8, 8 );
	cs.file		= "compute_3.glsl";
	cs.version	= EShaderVersion::SPIRV_1_5;
	ppln.SetShader( cs );

	// specialization
	{
		RC<ComputePipelineSpec>		spec = ppln.AddSpecialization( "compute_3.def" );
		
		spec.AddToRenderTech( "MinForward", "Compute1" );
	}
}
