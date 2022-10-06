
void main ()
{
	GraphicsPipeline@	ppln = GraphicsPipeline( "graphics_1" );
	ppln.AddFeatureSet( "MinimalFS" );
	ppln.SetLayout( "Graphics_PL_1" );
	ppln.SetVertexInput( "VBInput1" );
	ppln.SetFragmentOutputFromRenderPass( "Simple", /*subpass*/"Main" );

	{
		Shader@	vs = Shader();
		vs.file		= "vertex_1.glsl";
		vs.options	= EShaderOpt::Optimize;
		ppln.SetVertexShader( vs );
	}
	{
		Shader@	fs = Shader();
		fs.file		= "fragment_1.glsl";
		fs.options	= EShaderOpt::Optimize;
		ppln.SetFragmentShader( fs );
	}

	ppln.TestRenderPass( "Simple", /*subpass*/"Main" );

	// specialization
	{
		GraphicsPipelineSpec@	spec = ppln.AddSpecialization( "graphics_1 rp:Simple" );
		spec.AddToRenderTech( "MinForward", "Graphics1" );
		spec.SetViewportCount( 1 );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}
