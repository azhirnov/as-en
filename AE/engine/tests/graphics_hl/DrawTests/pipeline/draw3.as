#include <pipeline_compiler.as>

void ASmain ()
{
	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "draw3" );
	ppln.AddFeatureSet( "MinDesktop" );
	ppln.SetVertexInput( "VB_Position_f2, VB_UVf2_Col8" );

	{
		RC<Shader>	vs	= Shader();
		vs.file		= "draw1_vs.glsl";
		vs.options	= EShaderOpt::Optimize;
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs	= Shader();
		fs.file		= "draw1_fs.glsl";
		fs.options	= EShaderOpt::Optimize;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "rect_trilist_lines" );
		spec.AddToRenderTech( "DesktopCanvasDrawTest", "Draw_1" );

		RenderState	rs;
		rs.rasterization.frontFaceCCW	= true;
		//rs.rasterization.cullMode		= ECullMode::Back;
		rs.rasterization.polygonMode	= EPolygonMode::Line;
		rs.inputAssembly.topology		= EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}
