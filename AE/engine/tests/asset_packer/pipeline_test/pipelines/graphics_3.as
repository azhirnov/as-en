#include <pipeline_compiler.as>

void ASmain ()
{
	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "graphics_3" );
	ppln.SetLayout( "Graphics_PL_3" );
	ppln.SetVertexInput( "vb_input2" );
	ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "graphics_1.io" );

	{
		RC<Shader>	vs = Shader();
		vs.file		= "vertex_2.glsl";
		vs.options	= EShaderOpt::Optimize;
		vs.version	= EShaderVersion::SPIRV_1_0;
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs = Shader();
		fs.file		= "fragment_2.glsl";
		fs.options	= EShaderOpt::Optimize;
		fs.version	= EShaderVersion::SPIRV_1_0;
		ppln.SetFragmentShader( fs );
	}

	ppln.TestRenderPass( "Simple", /*subpass*/"Main" );
}
