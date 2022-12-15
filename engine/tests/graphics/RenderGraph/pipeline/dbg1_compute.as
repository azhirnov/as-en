
void main ()
{
	if ( ! IsVulkan() )
		return;

	{
		DescriptorSetLayout@	ds = DescriptorSetLayout( "dbg1_compute.ds1" );
		ds.StorageImage( EShaderStages::Compute, "un_OutImage", ArraySize(1), EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
	}
	{
		PipelineLayout@		pl = PipelineLayout( "dbg1_compute.pl" );
		pl.DSLayout( 0, "dbg1_compute.ds1" );
		pl.AddDebugDSLayout( 1, EShaderOpt::Trace, EShaderStages::Compute );
	}


	ComputePipeline@	ppln = ComputePipeline( "dbg1_compute" );
	ppln.SetLayout( "dbg1_compute.pl" );

	{
		Shader@	cs	= Shader();
		cs.file		= "compute_1.glsl";
		cs.version	= EShaderVersion::SPIRV_1_0;
		cs.options	= EShaderOpt::Trace;
		cs.SetComputeSpecAndDefault( 8, 8 );
		ppln.SetShader( cs );
	}
}
