#include <pipeline_compiler>

void ASmain ()
{
	if ( !IsVulkan() )	// TODO
		return;

	RC<RayTracingPipeline>	ppln = RayTracingPipeline( "raytracing_1" );
	ppln.AddFeatureSet( "MinRecursiveRayTracing" );
	ppln.SetLayout( "RayTracing_PL_1" );

	// general
	{
		RC<Shader>	rg	= Shader();
		rg.file		= "rtrace1_rg.glsl";
		rg.type		= EShader::RayGen;
		rg.options	= EShaderOpt::Optimize;
		rg.version	= EShaderVersion::SPIRV_1_4;

		rg.AddSpec( EValueType::UInt32, "sbtRecordStride" );

		ppln.AddGeneralShader( "Main", rg );
	}
	{
		RC<Shader>	rm	= Shader();
		rm.file		= "rtrace1_miss.glsl";
		rm.type		= EShader::RayMiss;
		rm.options	= EShaderOpt::Optimize;
		rm.version	= EShaderVersion::SPIRV_1_4;

		ppln.AddGeneralShader( "Miss", rm );
	}

	// triangle hit groups
	{
		RC<Shader>	rch = Shader();
		rch.file	= "rtrace1_tri_chit.glsl";
		rch.type	= EShader::RayClosestHit;
		rch.options	= EShaderOpt::Optimize;
		rch.version	= EShaderVersion::SPIRV_1_4;

		ppln.AddTriangleHitGroup( "TriHit1", rch, null );
	
		RC<Shader>	rah = Shader();
		rah.file	= "rtrace1_tri_ahit.glsl";
		rah.type	= EShader::RayAnyHit;
		rah.options	= EShaderOpt::Optimize;
		rah.version	= EShaderVersion::SPIRV_1_4;

		ppln.AddTriangleHitGroup( "TriHit2", rch, rah );
	}

	// procedural hit group
	{
		RC<Shader>	ri	= Shader();
		ri.file		= "rtrace1_proc_intsec.glsl";
		ri.type		= EShader::RayIntersection;
		ri.options	= EShaderOpt::Optimize;
		ri.version	= EShaderVersion::SPIRV_1_4;

		ppln.AddProceduralHitGroup( "ProcHit1", ri, null, null );
	}

	// specialization
	{
		const uint	hit_group_stride = 2;

		RC<RayTracingPipelineSpec>	spec = ppln.AddSpecialization( "raytracing_1.def" );

		spec.SetSpecValue( "sbtRecordStride", hit_group_stride );
		spec.AddToRenderTech( "RayTracing", "Trace1" );

		// shader binding table
		{
			RC<RayTracingShaderBinding>		sbt = RayTracingShaderBinding( spec, "raytracing_1.sbt0" );

			sbt.BindRayGen( "Main" );

			sbt.HitGroupStride( hit_group_stride );

			sbt.BindMiss( "Miss", MissIndex(0) );
			sbt.BindMiss( "Miss", MissIndex(1) );
			
			sbt.BindHitGroup( "TriHit1",	InstanceIndex(0),	RayIndex(0) );
			sbt.BindHitGroup( "TriHit2",	InstanceIndex(1),	RayIndex(0) );
			sbt.BindHitGroup( "ProcHit1",	InstanceIndex(2),	RayIndex(0) );
		}
	}
}
