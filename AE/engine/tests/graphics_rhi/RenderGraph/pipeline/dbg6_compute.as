// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void ASmain ()
{
	if ( ! IsVulkan() or ! IsShaderTraceSupported() )
		return;

	{
		RC<DescriptorSetLayout>		ds = DescriptorSetLayout( "dbg6_compute.ds1" );
		ds.StorageImage( EShaderStages::Compute, "un_OutImage", EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
	}
	{
		RC<PipelineLayout>		pl = PipelineLayout( "dbg6_compute.pl" );
		pl.DSLayout( 0, "dbg6_compute.ds1" );
		pl.AddDebugDSLayout( 1, EShaderOpt::Asserts, EShaderStages::Compute );
	}


	RC<ComputePipeline>		ppln = ComputePipeline( "dbg6_compute" );
	ppln.SetLayout( "dbg6_compute.pl" );

	{
		RC<Shader>	cs	= Shader();
		cs.file		= "compute_3.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics_rhi/RenderGraph/shaders/compute_3.glsl)
		cs.version	= EShaderVersion::SPIRV_1_0;
		cs.options	= EShaderOpt::Asserts;
		cs.ComputeSpecAndDefault( 8, 8 );
		ppln.SetShader( cs );
	}
}
