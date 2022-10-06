
void main ()
{
	if ( !IsVulkan() )
		return;

	MeshPipeline@	ppln = MeshPipeline( "mesh_2" );
	ppln.SetLayout( "Graphics_PL_2" );
	ppln.AddFeatureSet( "MinMeshShader" );
	
	{
		Shader@	ms = Shader();
		ms.file		= "mesh_2.glsl";
		ms.options	= EShaderOpt::Optimize;
		ms.version	= EShaderVersion::SPIRV_1_3;
		ms.SetMeshSpec();
		ppln.SetMeshShader( ms );
	}
	{
		Shader@	fs = Shader();
		fs.file		= "fragment_2.glsl";
		fs.options	= EShaderOpt(EShaderOpt::Optimize | EShaderOpt::OptimizeSize);
		fs.version	= EShaderVersion::SPIRV_1_3;
		ppln.SetFragmentShader( fs );
	}

	ppln.TestRenderPass( "Simple", /*subpass*/"Main" );

	// specialization
	{
		MeshPipelineSpec@	spec = ppln.AddSpecialization( "mesh_2 rp:Simple" );
		spec.AddToRenderTech( "MeshForward", "Graphics1" );
		spec.SetViewportCount( 1 );
		spec.SetMeshGroupSize( 3 );
	}
}
