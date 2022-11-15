
void main ()
{
	MeshPipeline@	ppln = MeshPipeline( "draw_mesh1" );
	ppln.AddFeatureSet( "MinMeshShader" );
	
	{
		Shader@	ms	= Shader();
		ms.file		= "draw_mesh1_ms.glsl";
		ms.options	= EShaderOpt::Optimize;
		ms.version	= EShaderVersion::SPIRV_1_4;
		ms.SetMeshSpec();
		ppln.SetMeshShader( ms );
	}
	{
		Shader@	fs	= Shader();
		fs.file		= "draw_mesh1_fs.glsl";
		fs.options	= EShaderOpt::Optimize;
		fs.version	= EShaderVersion::SPIRV_1_4;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		MeshPipelineSpec@	spec = ppln.AddSpecialization( "draw_mesh1" );
		spec.AddToRenderTech( "DrawMeshesTestRT", "DrawMeshes_1" );
		spec.SetViewportCount( 1 );
		spec.SetMeshGroupSize( 3 );

		RenderState	rs;
		spec.SetRenderState( rs );
	}
}
