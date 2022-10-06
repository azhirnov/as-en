
void main ()
{
	GraphicsPipeline@	ppln = GraphicsPipeline( "draw2" );
	ppln.AddFeatureSet( "MinimalFS" );
	ppln.SetVertexInput( "VB_Position_f2, VB_Color" );
	
	{
		Shader@	vs	= Shader();
		vs.file		= "draw2_vs.glsl";
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
		GraphicsPipelineSpec@	spec = ppln.AddSpecialization( "draw2" );
		spec.AddToRenderTech( "CanvasDrawTest", "Draw_1" );
		spec.SetViewportCount( 1 );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}
