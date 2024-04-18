// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void ASmain ()
{
	{
		RC<Sampler>		samp = Sampler( "NearestClamp|ycbcr|G8_B8R8_420_UNorm" );
		samp.AddFeatureSet( "part.samplerYcbcrConversion" );

		samp.Filter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
		samp.AddressMode( EAddressMode::Clamp, EAddressMode::Clamp, EAddressMode::Clamp );

		samp.Ycbcr_Format( EPixelFormat::G8_B8R8_420_UNorm );
		samp.Ycbcr_Model( ESamplerYcbcrModelConversion::Ycbcr_709 );
		samp.Ycbcr_Range( ESamplerYcbcrRange::ITU_Full );
		samp.Ycbcr_XChromaOffset( ESamplerChromaLocation::CositedEven );
		samp.Ycbcr_YChromaOffset( ESamplerChromaLocation::CositedEven );
		samp.Ycbcr_ChromaFilter( EFilter::Nearest );
	}

	{
		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "ycbcr.ds0" );
		ds.CombinedImage( EShaderStages::Fragment, "un_YcbcrTexture", EImageType::FImage2D, "NearestClamp|ycbcr|G8_B8R8_420_UNorm" );
	}{
		RC<PipelineLayout>		pl = PipelineLayout( "ycbcr.pl" );
		pl.DSLayout( 0, "ycbcr.ds0" );
	}

	{
		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "ycbcr.draw1" );
		ppln.SetLayout( "ycbcr.pl" );

		{
			RC<Shader>	vs = Shader();
			vs.file		= "ycbcr.glsl";	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/ycbcr.glsl)
			vs.options	= EShaderOpt::Optimize;
			ppln.SetVertexShader( vs );
		}
		{
			RC<Shader>	fs = Shader();
			fs.file		= "ycbcr.glsl";	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/ycbcr.glsl)
			fs.options	= EShaderOpt::Optimize;
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "ycbcr.draw1" );
			spec.AddToRenderTech( "Ycbcr.RTech", "Main" );

			RenderState	rs;

			rs.inputAssembly.topology	= EPrimitive::TriangleList;

			spec.SetRenderState( rs );
		}
	}
}
