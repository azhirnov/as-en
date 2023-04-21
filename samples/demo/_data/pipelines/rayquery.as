#include <pipeline_compiler>

void ASmain ()
{
	if ( ! IsVulkan() )	// TODO
		return;

	{
		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "rquery.ds0" );
		ds.AddFeatureSet( "MinInlineRayTracing" );
		ds.StorageImage( EShaderStages::Compute, "un_OutImage", ArraySize(1), EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
		ds.RayTracingScene( EShaderStages::Compute, "un_RtScene", ArraySize(1) );
	}
	{
		RC<PipelineLayout>		pl = PipelineLayout( "rquery.pl" );
		pl.DSLayout( 0, "rquery.ds0" );
	}


	RC<ComputePipeline>		ppln = ComputePipeline( "rquery" );
	ppln.SetLayout( "rquery.pl" );

	{
		RC<Shader>	cs = Shader();
		cs.file		= "rquery.glsl";
		cs.version	= EShaderVersion::SPIRV_1_4;
		cs.options	= EShaderOpt::Optimize;
		cs.SetComputeSpecAndDefault( 8, 8 );
		ppln.SetShader( cs );
	}

	// specialization
	{
		RC<ComputePipelineSpec>		spec = ppln.AddSpecialization( "rquery.def" );
		spec.AddToRenderTech( "RayQuery.RTech", "RayTrace" );
	}
}
