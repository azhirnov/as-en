
void main ()
{
	if ( ! IsVulkan() )
		return;

	{
		PipelineLayout@		pl = PipelineLayout( "dbg2_draw.pl" );
		pl.AddDebugDSLayout( EShaderOpt::Trace, EShaderStages::Fragment | EShaderStages::Vertex );
		pl.Define( "#define DEBUG" );
	}

	GraphicsPipeline@	ppln = GraphicsPipeline( "dbg2_draw" );
	ppln.SetLayout( "dbg2_draw.pl" );
	
	{
		Shader@	vs	= Shader();
		vs.file		= "draw1_vs.glsl";
		vs.options	= EShaderOpt::Trace;
		ppln.SetVertexShader( vs );
	}
	{
		Shader@	fs	= Shader();
		fs.file		= "draw1_fs.glsl";
		fs.options	= EShaderOpt::Trace;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		GraphicsPipelineSpec@	spec = ppln.AddSpecialization( "dbg2_draw" );
		spec.AddToRenderTech( "DrawTestRT", "Draw_1" );
		spec.SetViewportCount( 1 );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}
