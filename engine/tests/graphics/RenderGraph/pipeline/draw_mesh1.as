#include <pipeline_compiler>

void ASmain ()
{
	if ( !IsVulkan() )
		return;

	{
		RC<ShaderStructType>	st = ShaderStructType( "meshtofrag1.io" );
		st.Set( "float4	color;" );
	}

	RC<MeshPipeline>	ppln = MeshPipeline( "draw_mesh1" );
	ppln.AddFeatureSet( "MinMeshShader" );
	ppln.SetShaderIO( EShader::Mesh, EShader::Fragment,	"meshtofrag1.io" );
	
	{
		RC<Shader>	ms	= Shader();
		ms.file		= "draw_mesh1.glsl";
		ms.options	= EShaderOpt::Optimize;
		ms.version	= EShaderVersion::SPIRV_1_4;
		ms.MeshSpec1();
		ms.MeshOutput( 3, 1, EPrimitive::TriangleList );
		ppln.SetMeshShader( ms );
	}
	{
		RC<Shader>	fs	= Shader();
		fs.file		= "draw_mesh1.glsl";
		fs.options	= EShaderOpt::Optimize;
		fs.version	= EShaderVersion::SPIRV_1_4;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<MeshPipelineSpec>	spec = ppln.AddSpecialization( "draw_mesh1" );
		spec.AddToRenderTech( "DrawMeshesTestRT", "DrawMeshes_1" );
		spec.SetViewportCount( 1 );
		spec.SetMeshGroupSize( 3 );

		RenderState	rs;
		spec.SetRenderState( rs );
	}
}
