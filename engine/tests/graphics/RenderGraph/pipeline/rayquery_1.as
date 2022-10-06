
void main ()
{
	{
		DescriptorSetLayout@	ds = DescriptorSetLayout( "rquery1.ds1" );
		ds.StorageImage( EShaderStages::Compute, "un_OutImage", ArraySize(1), EImageType::FImage2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
		ds.RayTracingScene( EShaderStages::Compute, "un_RtScene", ArraySize(1) );
	}
	{
		PipelineLayout@		pl = PipelineLayout( "rquery1.pl" );
		pl.DSLayout( 0, "rquery1.ds1" );
	}


	ComputePipeline@	ppln = ComputePipeline( "rquery1" );
	ppln.SetLayout( "rquery1.pl" );
	ppln.AddFeatureSet( "MinInlineRayTracing" );

	{
		Shader@	cs	= Shader();
		cs.file		= "rquery1.glsl";
		cs.version	= EShaderVersion::SPIRV_1_4;
		cs.options	= EShaderOpt::Optimize;
		cs.SetComputeSpecAndDefault( 8, 8 );
		ppln.SetShader( cs );
	}

	// specialization
	{
		ComputePipelineSpec@	spec = ppln.AddSpecialization( "rquery1.def" );
		spec.AddToRenderTech( "RayQueryTestRT", "RayTrace_1" );
	}
}
