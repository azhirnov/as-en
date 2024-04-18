#include <pipeline_compiler.as>

void ASmain ()
{
	if ( ! IsVulkan() || ! IsShaderTraceSupported() )
		return;

	{
		RC<PipelineLayout>		pl = PipelineLayout( "dbg2_draw.pl" );
		pl.AddFeatureSet( "MinDesktop" );
		pl.AddDebugDSLayout( EShaderOpt::Trace, EShaderStages::Fragment | EShaderStages::Vertex );
		pl.Define( "DEBUG" );
	}

	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "dbg2_draw" );
	ppln.SetLayout( "dbg2_draw.pl" );

	{
		RC<Shader>	vs	= Shader();
		vs.file		= "draw1_vs.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/draw1_vs.glsl)
		vs.options	= EShaderOpt::Trace;
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs	= Shader();
		fs.file		= "draw1_fs.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/draw1_fs.glsl)
		fs.options	= EShaderOpt::Trace;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "dbg2_draw" );
		spec.AddToRenderTech( "DebugDrawTestRT", "Draw_1" );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}
