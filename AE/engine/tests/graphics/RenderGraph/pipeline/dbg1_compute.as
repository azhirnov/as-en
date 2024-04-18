#include <pipeline_compiler.as>

void ASmain ()
{
	if ( ! IsVulkan() || ! IsShaderTraceSupported() )
		return;

	{
		RC<DescriptorSetLayout>		ds = DescriptorSetLayout( "dbg1_compute.ds1" );
		ds.StorageImage( EShaderStages::Compute, "un_OutImage", EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
	}
	{
		RC<PipelineLayout>		pl = PipelineLayout( "dbg1_compute.pl" );
		pl.DSLayout( 0, "dbg1_compute.ds1" );
		pl.AddDebugDSLayout( 1, EShaderOpt::Trace, EShaderStages::Compute );
	}


	RC<ComputePipeline>		ppln = ComputePipeline( "dbg1_compute" );
	ppln.SetLayout( "dbg1_compute.pl" );

	{
		RC<Shader>	cs	= Shader();
		cs.file		= "compute_1.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/compute_1.glsl)
		cs.version	= EShaderVersion::SPIRV_1_0;
		cs.options	= EShaderOpt::Trace;
		cs.ComputeSpecAndDefault( 8, 8 );
		ppln.SetShader( cs );
	}
}
