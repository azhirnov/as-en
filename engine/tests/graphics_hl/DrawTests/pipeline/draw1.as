
void main ()
{
	GraphicsPipeline@	ppln = GraphicsPipeline( "draw1" );
	ppln.AddFeatureSet( "MinimalFS" );
	ppln.SetVertexInput( "VB_Position_f2, VB_UVf2_Col" );
	
	{
		Shader@	vs	= Shader();
		vs.file		= "draw1_vs.glsl";
		vs.options	= EShaderOpt::Optimize;
		ppln.SetVertexShader( vs );
	}
	{
		Shader@	fs	= Shader();
		fs.file		= "draw1_fs.glsl";
		fs.options	= EShaderOpt::Optimize;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		GraphicsPipelineSpec@	spec = ppln.AddSpecialization( "rect_tristrip" );
		spec.AddToRenderTech( "CanvasDrawTest", "Draw_1" );
		spec.SetViewportCount( 1 );

		RenderState	rs;
		rs.rasterization.frontFaceCCW	= true;
		rs.rasterization.cullMode		= ECullMode::Back;
		rs.inputAssembly.topology		= EPrimitive::TriangleStrip;

		spec.SetRenderState( rs );
	}
	{
		GraphicsPipelineSpec@	spec = ppln.AddSpecialization( "rect_trilist" );
		spec.AddToRenderTech( "CanvasDrawTest", "Draw_1" );
		spec.SetViewportCount( 1 );

		RenderState	rs;
		rs.rasterization.frontFaceCCW	= true;
		rs.rasterization.cullMode		= ECullMode::Back;
		rs.inputAssembly.topology		= EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
	{
		GraphicsPipelineSpec@	spec = ppln.AddSpecialization( "rect_trilist_lines" );
		spec.AddToRenderTech( "CanvasDrawTest", "Draw_1" );
		spec.SetViewportCount( 1 );

		RenderState	rs;
		rs.rasterization.frontFaceCCW	= true;
		//rs.rasterization.cullMode		= ECullMode::Back;
		rs.rasterization.polygonMode	= EPolygonMode::Line;
		rs.inputAssembly.topology		= EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}