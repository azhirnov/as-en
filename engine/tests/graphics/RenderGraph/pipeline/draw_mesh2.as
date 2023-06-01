#include <pipeline_compiler>

void ASmain ()
{
	if ( !IsVulkan() )
		return;
	
	{
		RC<ShaderStructType>	st = ShaderStructType( "tasktomesh.io" );
		st.Set( "float	scale;" );
	}{
		RC<ShaderStructType>	st = ShaderStructType( "meshtofrag.io" );
		st.Set( "float4	color;" );
	}

	RC<MeshPipeline>	ppln = MeshPipeline( "draw_mesh2" );
	ppln.AddFeatureSet( "MinMeshShader" );
	ppln.SetShaderIO( EShader::MeshTask, EShader::Mesh,		"tasktomesh.io" );
	ppln.SetShaderIO( EShader::Mesh,	 EShader::Fragment,	"meshtofrag.io" );
	
	{
		RC<Shader>	ts	= Shader();
		ts.file		= "draw_mesh2.glsl";
		ts.options	= EShaderOpt::Optimize;
		ts.version	= EShaderVersion::SPIRV_1_4;
		ts.MeshSpec1();
		ppln.SetTaskShader( ts );
	}{
		RC<Shader>	ms	= Shader();
		ms.file		= "draw_mesh2.glsl";
		ms.options	= EShaderOpt::Optimize;
		ms.version	= EShaderVersion::SPIRV_1_4;
		ms.MeshSpec1();
		ppln.SetMeshShader( ms );
	}{
		RC<Shader>	fs	= Shader();
		fs.file		= "draw_mesh2.glsl";
		fs.options	= EShaderOpt::Optimize;
		fs.version	= EShaderVersion::SPIRV_1_4;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<MeshPipelineSpec>	spec = ppln.AddSpecialization( "draw_mesh2" );
		spec.AddToRenderTech( "DrawMeshesTestRT", "DrawMeshes_1" );
		spec.SetViewportCount( 1 );
		spec.SetTaskGroupSize( 1 );
		spec.SetMeshGroupSize( 3 );

		RenderState	rs;
		spec.SetRenderState( rs );
	}
}
