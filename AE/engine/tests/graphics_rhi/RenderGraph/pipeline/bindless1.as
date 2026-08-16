// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#include <pipeline_compiler.as>

void ASmain ()
{
	{
		const uint	max_textures = 1000;	// upper bound, at runtime can set less value

		RC<DescriptorSetLayout>		ds = DescriptorSetLayout( "bindless1.ds" );
		ds.AddFeatureSet( "part.MinBindless" );
		ds.StorageImage( EShaderStages::Compute, "un_OutImage", EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
		ds.ImtblSampler( EShaderStages::Compute, "un_Sampler",  "LinearMipmapRepeat" );
		ds.SampledImage( EShaderStages::Compute, "un_Textures", ArraySize(max_textures), EImageType::Float_2D );

		ds.SetFlags( "un_Textures", EDescriptorFlags::VariableSize );
		ds.SetUsage( EDescSetUsage::ArgumentBuffer );	// required for bindless in Metal
	}{
		RC<ShaderStructType>	st = ShaderStructType( "PC_bindless1" );
		st.Set( "uint		texCount;" );
	}{
		RC<PipelineLayout>		pl = PipelineLayout( "bindless1.pl" );
		pl.DSLayout( 0, "bindless1.ds" );
		pl.PushConst( "pc", "PC_bindless1", EShader::Compute );
	}

	RC<ComputePipeline>		ppln = ComputePipeline( "bindless1" );
	ppln.SetLayout( "bindless1.pl" );

	{
		RC<Shader>	cs	= Shader();
		cs.file		= "bindless1.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics_rhi/RenderGraph/shaders/bindless1.glsl)
		cs.version	= EShaderVersion::SPIRV_1_4;
		cs.ComputeSpecAndDefault( 8, 8 );
		ppln.SetShader( cs );
	}

	// specialization
	{
		RC<ComputePipelineSpec>	spec = ppln.AddSpecialization( "bindless1.def" );

		spec.AddToRenderTech( "Bindless.RTech", "Compute_1" );
	}
}
