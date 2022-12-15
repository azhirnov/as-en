
void main ()
{
	{
		DescriptorSetLayout@	ds = DescriptorSetLayout( "compute2.ds1" );
		ds.StorageImage( EShaderStages::Compute, "un_Image", ArraySize(1), EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_RW );
	}
	{
		PipelineLayout@		pl = PipelineLayout( "compute2.pl" );
		pl.DSLayout( 0, "compute2.ds1" );
	}


	ComputePipeline@	ppln = ComputePipeline( "compute2" );
	ppln.SetLayout( "compute2.pl" );

	{
		Shader@	cs	= Shader();
		cs.file		= "compute_2.glsl";
		cs.version	= EShaderVersion::SPIRV_1_0;
		cs.options	= EShaderOpt::Optimize;
		cs.SetComputeLocalSize( 4, 4 );
		ppln.SetShader( cs );
	}
	
	// specialization
	{
		ComputePipelineSpec@	spec = ppln.AddSpecialization( "async_comp1.compute" );
		spec.AddToRenderTech( "AsyncCompTestRT", "Compute_1" );
	}
}
