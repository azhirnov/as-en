#include <pipeline_compiler.as>

void ASmain ()
{
	{
		RC<DescriptorSetLayout>		ds = DescriptorSetLayout( "compute2.ds1" );
		ds.StorageImage( EShaderStages::Compute, "un_Image", EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_RW );
	}
	{
		RC<PipelineLayout>		pl = PipelineLayout( "compute2.pl" );
		pl.DSLayout( 0, "compute2.ds1" );
	}


	RC<ComputePipeline>		ppln = ComputePipeline( "compute2" );
	ppln.SetLayout( "compute2.pl" );

	{
		RC<Shader>	cs	= Shader();
		cs.file		= "compute_2.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/compute_2.glsl)
		cs.version	= EShaderVersion::SPIRV_1_0;
		cs.options	= EShaderOpt::Optimize;
		cs.ComputeLocalSize( 4, 4 );
		ppln.SetShader( cs );
	}

	// specialization
	{
		RC<ComputePipelineSpec>		spec = ppln.AddSpecialization( "async_comp1.compute" );
		spec.AddToRenderTech( "AsyncCompTestRT", "Compute_1" );
	}
}
