
void main ()
{
	if ( !IsVulkan() )
		return;

	RayTracingPipeline@	ppln = RayTracingPipeline( "raytracing_1" );
	ppln.AddFeatureSet( "MinRecursiveRayTracing" );
	ppln.SetLayout( "RayTracing_PL_1" );

	// general
	{
		Shader@	rg	= Shader();
		rg.file		= "rtrace1_rg.glsl";
		rg.type		= EShader::RayGen;
		rg.options	= EShaderOpt::Optimize;
		rg.version	= EShaderVersion::SPIRV_1_4;

		ppln.AddGeneralShader( "Main", rg );
	}
	{
		Shader@	rm	= Shader();
		rm.file		= "rtrace1_miss.glsl";
		rm.type		= EShader::RayMiss;
		rm.options	= EShaderOpt::Optimize;
		rm.version	= EShaderVersion::SPIRV_1_4;

		ppln.AddGeneralShader( "Miss", rm );
	}

	// triangle hit groups
	{
		Shader@	rch = Shader();
		rch.file	= "rtrace1_tri_chit.glsl";
		rch.type	= EShader::RayClosestHit;
		rch.options	= EShaderOpt::Optimize;
		rch.version	= EShaderVersion::SPIRV_1_4;

		ppln.AddTriangleHitGroup( "TriHit1", rch, null );
	
		Shader@	rah = Shader();
		rah.file	= "rtrace1_tri_ahit.glsl";
		rah.type	= EShader::RayAnyHit;
		rah.options	= EShaderOpt::Optimize;
		rah.version	= EShaderVersion::SPIRV_1_4;

		ppln.AddTriangleHitGroup( "TriHit2", rch, rah );
	}

	// procedural hit group
	{
		Shader@	ri	= Shader();
		ri.file		= "rtrace1_proc_intsec.glsl";
		ri.type		= EShader::RayIntersection;
		ri.options	= EShaderOpt::Optimize;
		ri.version	= EShaderVersion::SPIRV_1_4;

		ppln.AddProceduralHitGroup( "ProcHit1", ri, null, null );
	}

	// specialization
	{
		RayTracingPipelineSpec@	spec = ppln.AddSpecialization( "raytracing_1.def" );

		spec.AddToRenderTech( "RayTracing", "Trace1" );
	}

	// shader binding table
	{
		//RayTracingShaderBinding@	sbt = RayTracingShaderBinding( ppln, "raytracing_1.sbt0" );

		// TODO
	}
}
