#include <pipeline_compiler>

void ASmain ()
{
	if ( ! IsVulkan() )
		return;

	{
		RC<PipelineLayout>		pl = PipelineLayout( "dbg3_draw.pl" );
		pl.AddFeatureSet( "MinMeshShader" );
		pl.AddDebugDSLayout( EShaderOpt::Trace, EShaderStages::Mesh );
		pl.Define( "DEBUG" );
	}

	RC<MeshPipeline>	ppln = MeshPipeline( "dbg3_draw" );
	ppln.SetLayout( "dbg3_draw.pl" );
	
	{
		RC<Shader>	ms	= Shader();
		ms.file		= "draw_mesh1.glsl";
		ms.options	= EShaderOpt::Trace;
		ms.version	= EShaderVersion::SPIRV_1_4;
		ms.MeshSpec1();
		ppln.SetMeshShader( ms );
	}
	{
		RC<Shader>	fs	= Shader();
		fs.file		= "draw_mesh1.glsl";
		fs.version	= EShaderVersion::SPIRV_1_4;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<MeshPipelineSpec>	spec = ppln.AddSpecialization( "dbg3_draw" );
		spec.AddToRenderTech( "DrawTestRT", "Draw_1" );
		spec.SetViewportCount( 1 );
		spec.SetMeshGroupSize( 3 );

		RenderState	rs;
		spec.SetRenderState( rs );
	}
}