#include <pipeline_compiler.as>

void ASmain ()
{
	{
		RC<DescriptorSetLayout>		ds = DescriptorSetLayout( "compute1.ds1" );
		ds.StorageImage( EShaderStages::Compute, "un_OutImage", EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
	}
	{
		RC<PipelineLayout>		pl = PipelineLayout( "compute1.pl" );
		pl.DSLayout( 0, "compute1.ds1" );
	}


	RC<ComputePipeline>		ppln = ComputePipeline( "compute1" );
	ppln.SetLayout( "compute1.pl" );

	{
		RC<Shader>	cs	= Shader();
		cs.file		= "compute_1.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/compute_1.glsl)
		cs.version	= EShaderVersion::SPIRV_1_0;
		cs.options	= EShaderOpt::Optimize;
		cs.ComputeSpecAndDefault( 8, 8 );
		ppln.SetShader( cs );
	}
}
