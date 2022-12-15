
void main ()
{
	{
		DescriptorSetLayout@	ds = DescriptorSetLayout( "compute1.ds1" );
		ds.StorageImage( EShaderStages::Compute, "un_OutImage", ArraySize(1), EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
	}
	{
		PipelineLayout@		pl = PipelineLayout( "compute1.pl" );
		pl.DSLayout( 0, "compute1.ds1" );
	}


	ComputePipeline@	ppln = ComputePipeline( "compute1" );
	ppln.SetLayout( "compute1.pl" );

	{
		Shader@	cs	= Shader();
		cs.file		= "compute_1.glsl";
		cs.version	= EShaderVersion::SPIRV_1_0;
		cs.options	= EShaderOpt::Optimize;
		cs.SetComputeSpecAndDefault( 8, 8 );
		ppln.SetShader( cs );
	}
}
