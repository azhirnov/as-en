
void main ()
{
	ShaderStructType@	st = ShaderStructType( "Vertex_draw2" );
	st.Set( "packed_float2		Position;" +
			"packed_ubyte_norm4	Color;" );

	VertexBufferInput@	vb = VertexBufferInput( "draw2.vb" );
	vb.Add( "vb", st );

	GraphicsPipeline@	ppln = GraphicsPipeline( "draw2" );
	ppln.AddFeatureSet( "MinimalFS" );
	ppln.SetVertexInput( vb );
	
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
		spec.AddToRenderTech( "DrawTestRT", "Draw_1" );
		spec.SetViewportCount( 1 );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}
