#include <pipeline_compiler.as>

void ASmain ()
{
	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "draw1" );

	{
		RC<Shader>	vs	= Shader();
		vs.file		= "draw1_vs.glsl";			// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/draw1_vs.glsl)
		vs.options	= EShaderOpt::Optimize;
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs	= Shader();
		fs.file		= "draw1_fs.glsl";			// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/draw1_fs.glsl)
		fs.options	= EShaderOpt::Optimize;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "draw1" );
		spec.AddToRenderTech( "DrawTestRT", "Draw_1" );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "async_comp1.graphics" );
		spec.AddToRenderTech( "AsyncCompTestRT", "Draw_1" );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}
