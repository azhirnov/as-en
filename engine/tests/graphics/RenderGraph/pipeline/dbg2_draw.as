#include <pipeline_compiler>

void main ()
{
	if ( ! IsVulkan() )
		return;

	{
		RC<PipelineLayout>		pl = PipelineLayout( "dbg2_draw.pl" );
		pl.AddDebugDSLayout( EShaderOpt::Trace, EShaderStages::Fragment | EShaderStages::Vertex );
		pl.Define( "#define DEBUG" );
	}

	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "dbg2_draw" );
	ppln.SetLayout( "dbg2_draw.pl" );
	
	{
		RC<Shader>	vs	= Shader();
		vs.file		= "draw1_vs.glsl";
		vs.options	= EShaderOpt::Trace;
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs	= Shader();
		fs.file		= "draw1_fs.glsl";
		fs.options	= EShaderOpt::Trace;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "dbg2_draw" );
		spec.AddToRenderTech( "DrawTestRT", "Draw_1" );
		spec.SetViewportCount( 1 );

		RenderState	rs;
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}
