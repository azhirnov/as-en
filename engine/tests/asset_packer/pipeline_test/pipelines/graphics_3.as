
void main ()
{
	GraphicsPipeline@	ppln = GraphicsPipeline( "graphics_3" );
	ppln.AddFeatureSet( "MinimalFS" );
	ppln.SetLayout( "Graphics_PL_3" );
	ppln.SetVertexInput( "VBInput2" );
	
	{
		Shader@	vs = Shader();
		vs.file		= "vertex_2.glsl";
		vs.options	= EShaderOpt::Optimize;
		vs.version	= EShaderVersion::SPIRV_1_0;
		ppln.SetVertexShader( vs );
	}
	{
		Shader@	fs = Shader();
		fs.file		= "fragment_2.glsl";
		fs.options	= EShaderOpt::Optimize;
		fs.version	= EShaderVersion::SPIRV_1_0;
		ppln.SetFragmentShader( fs );
	}

	ppln.TestRenderPass( "Simple", /*subpass*/"Main" );
}