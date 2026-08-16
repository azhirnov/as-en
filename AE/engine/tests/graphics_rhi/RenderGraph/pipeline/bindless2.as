// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#include <pipeline_compiler.as>

void ASmain ()
{
/*	{
		RC<DescriptorSetLayout>		ds = DescriptorSetLayout( "bindless2.ds" );
		ds.AddFeatureSet( "part.MinBindless" );
		ds.StorageImage( EShaderStages::Compute, "un_OutImage", EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
		ds.Sampler(		 EShaderStages::Compute, "un_Samplers" );
		ds.SampledImage( EShaderStages::Compute, "un_Textures", EImageType::Float_2D );

		// now all bindings are aliased unsized arrays
		ds.SetUsage( EDescSetUsage::DescriptorHeap );
	}{
		RC<ShaderStructType>	st = ShaderStructType( "PC_bindless2" );
		st.Set( "uint		OutImage_first;"
				"uint		Textures_first;"
				"uint		Textures_count;" );
	}{
		RC<PipelineLayout>		pl = PipelineLayout( "bindless2.pl" );
		pl.DSLayout( 0, "bindless2.ds" );
		pl.PushConst( "pc", "PC_bindless2", EShader::Compute );
	}

	RC<ComputePipeline>		ppln = ComputePipeline( "bindless2" );

	// pipeline layout used only to generate resource bindings
	ppln.SetLayout( "bindless2.pl" );

	{
		RC<Shader>	cs	= Shader();
		cs.file		= "bindless2.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics_rhi/RenderGraph/shaders/bindless2.glsl)
		cs.version	= EShaderVersion::SPIRV_1_6;
		cs.ComputeSpecAndDefault( 8, 8 );
		ppln.SetShader( cs );
	}

	// specialization
	{
		RC<ComputePipelineSpec>	spec = ppln.AddSpecialization( "bindless2.def" );

		spec.AddToRenderTech( "Bindless.RTech", "Compute_1" );
	}*/
}
