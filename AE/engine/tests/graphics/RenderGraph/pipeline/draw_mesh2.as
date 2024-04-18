#include <pipeline_compiler.as>

void ASmain ()
{
	if ( !IsVulkan() )
		return;

	{
		RC<ShaderStructType>	st = ShaderStructType( "tasktomesh2.io" );
		st.Set( "float	scale;" );
	}{
		RC<ShaderStructType>	st = ShaderStructType( "meshtofrag2.io" );
		st.Set( "float4	color;" );
	}

	RC<MeshPipeline>	ppln = MeshPipeline( "draw_mesh2" );
	ppln.AddFeatureSet( "MinMeshShader" );
	ppln.SetShaderIO( EShader::MeshTask, EShader::Mesh,		"tasktomesh2.io" );
	ppln.SetShaderIO( EShader::Mesh,	 EShader::Fragment,	"meshtofrag2.io" );

	{
		RC<Shader>	ts	= Shader();
		ts.file		= "draw_mesh2.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/draw_mesh2.glsl)
		ts.options	= EShaderOpt::Optimize;
		ts.version	= EShaderVersion::SPIRV_1_4;
		ts.MeshSpec1();
		ppln.SetTaskShader( ts );
	}{
		RC<Shader>	ms	= Shader();
		ms.file		= "draw_mesh2.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/draw_mesh2.glsl)
		ms.options	= EShaderOpt::Optimize;
		ms.version	= EShaderVersion::SPIRV_1_4;
		ms.MeshSpec1();
		ms.MeshOutput( 3, 1, EPrimitive::TriangleList );
		ppln.SetMeshShader( ms );
	}{
		RC<Shader>	fs	= Shader();
		fs.file		= "draw_mesh2.glsl";		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics/RenderGraph/shaders/draw_mesh2.glsl)
		fs.options	= EShaderOpt::Optimize;
		fs.version	= EShaderVersion::SPIRV_1_4;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<MeshPipelineSpec>	spec = ppln.AddSpecialization( "draw_mesh2" );
		spec.AddToRenderTech( "DrawMeshesTestRT", "DrawMeshes_1" );

		spec.SetTaskLocalSize( 1 );
		spec.SetMeshLocalSize( 3 );

		RenderState	rs;
		spec.SetRenderState( rs );
	}
}
