
void main ()
{
	GraphicsPipeline@	ppln = GraphicsPipeline( "draw1" );
	ppln.AddFeatureSet( "MinimalFS" );
	
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
		GraphicsPipelineSpec@	spec = ppln.AddSpecialization( "draw1" );
		spec.AddToRenderTech( "DrawTestRT", "Draw_1" );
		spec.SetViewportCount( 1 );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}

	// specialization
	{
		GraphicsPipelineSpec@	spec = ppln.AddSpecialization( "async_comp1.graphics" );
		spec.AddToRenderTech( "AsyncCompTestRT", "Draw_1" );
		spec.SetViewportCount( 1 );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}
