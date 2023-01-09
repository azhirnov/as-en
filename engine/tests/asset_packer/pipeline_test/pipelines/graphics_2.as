#include <pipeline_compiler>

void main ()
{
	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "graphics_2" );
	ppln.AddFeatureSet( "MinimalFS" );
	ppln.SetLayout( "Graphics_PL_1" );
	ppln.SetVertexInput( "vb_input1" );
	ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "graphics_1.io" );
	
	{
		RC<Shader>	vs = Shader();
		vs.file		= "vertex_1.glsl";
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
