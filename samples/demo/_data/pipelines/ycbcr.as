#include <pipeline_compiler>

void ASmain ()
{
	{
		RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "ycbcr.ds0" );
		ds.CombinedImage( EShaderStages::Fragment, "un_YcbcrTexture", EImageType::FImage2D, "NearestClamp|ycbcr|G8_B8R8_420_UNorm" );
	}{
		RC<PipelineLayout>		pl = PipelineLayout( "ycbcr.pl" );
		pl.DSLayout( 0, "ycbcr.ds0" );
	}

	{
		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "ycbcr.draw1" );
		ppln.AddFeatureSet( "MinimalFS" );
		ppln.SetLayout( "ycbcr.pl" );
	
		{
			RC<Shader>	vs = Shader();
			vs.file		= "ycbcr_image.glsl";
			vs.options	= EShaderOpt::Optimize;
			ppln.SetVertexShader( vs );
		}
		{
			RC<Shader>	fs = Shader();
			fs.file		= "ycbcr_image.glsl";
			fs.options	= EShaderOpt::Optimize;
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "ycbcr.draw1" );
			spec.AddToRenderTech( "Ycbcr.RTech", "Main" );
			spec.SetViewportCount( 1 );

			RenderState	rs;

			rs.inputAssembly.topology	= EPrimitive::TriangleList;

			spec.SetRenderState( rs );
		}
	}
}
