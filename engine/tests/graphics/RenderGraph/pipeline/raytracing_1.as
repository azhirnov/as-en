
void main ()
{
	if ( !IsVulkan() )
		return;

	DescriptorSetLayout@	ds = DescriptorSetLayout( "rtrace1.ds1" );
	ds.AddFeatureSet( "MinRecursiveRayTracing" );
	ds.StorageImage( EShaderStages::RayGen, "un_OutImage", ArraySize(1), EImageType::2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
	ds.RayTracingScene( EShaderStages::RayGen, "un_RtScene", ArraySize(1) );
	
	PipelineLayout@		pl = PipelineLayout( "rtrace1.pl" );
	pl.DSLayout( 0, "rtrace1.ds1" );
	

	RayTracingPipeline@	ppln = RayTracingPipeline( "rtrace1" );
	ppln.SetLayout( pl );

	// general
	{
		Shader@	rg	= Shader();
		rg.file		= "rtrace1.glsl";
		rg.type		= EShader::RayGen;
		rg.options	= EShaderOpt::Optimize;
		rg.version	= EShaderVersion::SPIRV_1_4;
		
		rg.AddSpec( EValueType::UInt32, "HitGroupStride" );

		ppln.AddGeneralShader( "Main", rg );
	}
	{
		Shader@	rm	= Shader();
		rm.file		= "rtrace1.glsl";
		rm.type		= EShader::RayMiss;
		rm.options	= EShaderOpt::Optimize;
		rm.version	= EShaderVersion::SPIRV_1_4;

		ppln.AddGeneralShader( "Miss", rm );
	}

	// triangle hit groups
	{
		Shader@	rch = Shader();
		rch.file	= "rtrace1.glsl";
		rch.type	= EShader::RayClosestHit;
		rch.options	= EShaderOpt::Optimize;
		rch.version	= EShaderVersion::SPIRV_1_4;

		ppln.AddTriangleHitGroup( "TriHit", rch, null );
	}

	// specialization
	{
		const uint	hit_group_stride = 2;
		
		RayTracingPipelineSpec@	spec = ppln.AddSpecialization( "rtrace1.def" );

		spec.SetSpecValue( "HitGroupStride", hit_group_stride );
		spec.AddToRenderTech( "RayTracingTestRT", "RayTrace_1" );

		// shader binding table
		{
			RayTracingShaderBinding@	sbt = RayTracingShaderBinding( spec, "rtrace1.sbt0" );

			sbt.BindRayGen( "Main" );

			sbt.HitGroupStride( hit_group_stride );

			sbt.BindMiss( "Miss", MissIndex(0) );
			sbt.BindMiss( "Miss", MissIndex(1) );
			
			sbt.BindHitGroup( "TriHit",	InstanceIndex(1),	RayIndex(0) );
		}
	}
}
