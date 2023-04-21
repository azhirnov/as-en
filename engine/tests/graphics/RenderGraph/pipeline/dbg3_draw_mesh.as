#include <pipeline_compiler>

void ASmain ()
{
	if ( ! IsVulkan() )
		return;

	{
		RC<PipelineLayout>		pl = PipelineLayout( "dbg3_draw.pl" );
		pl.AddFeatureSet( "MinMeshShader" );
		pl.AddDebugDSLayout( EShaderOpt::Trace, EShaderStages::Mesh );
		pl.Define( "#define DEBUG" );
	}

	RC<MeshPipeline>	ppln = MeshPipeline( "dbg3_draw" );
	ppln.SetLayout( "dbg3_draw.pl" );
	
	{
		RC<Shader>	ms	= Shader();
		ms.file		= "draw_mesh1_ms.glsl";
		ms.options	= EShaderOpt::Trace;
		ms.version	= EShaderVersion::SPIRV_1_4;
		ms.SetMeshSpec1();
		ppln.SetMeshShader( ms );
	}
	{
		RC<Shader>	fs	= Shader();
		fs.file		= "draw_mesh1_fs.glsl";
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
