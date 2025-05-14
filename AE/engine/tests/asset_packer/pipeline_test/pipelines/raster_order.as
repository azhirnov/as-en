#include <pipeline_compiler.as>

void ASmain ()
{
	if ( not IsVulkan() )
		return;

	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "raster_order" );
	ppln.AddFeatureSet( "part.RasterOrderAttachment" );
	ppln.SetLayout( "RasterOrder_PL" );
	ppln.SetVertexInput( "vb_layout1" );

	ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "graphics_1.io" );
	ppln.SetFragmentOutputFromRenderTech( "RasterOrderAttachment.RTech", "Graphics0" );

	{
		RC<Shader>	vs = Shader();
		vs.file		= "vertex_3.glsl";
		vs.options	= EShaderOpt::Optimize;
		vs.version	= EShaderVersion::SPIRV_1_0;
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs = Shader();
		fs.file		= "raster_order_fs.glsl";
		fs.options	= EShaderOpt::Optimize;
		fs.version	= EShaderVersion::SPIRV_1_0;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "raster_order" );
		spec.AddToRenderTech( "RasterOrderAttachment.RTech", "Graphics0" );
		spec.SetViewportCount( 1 );
		spec.SetVertexInput( "vb_input3" );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		rs.rasterOrderAccess.color = true;

		spec.SetRenderState( rs );
	}
}
