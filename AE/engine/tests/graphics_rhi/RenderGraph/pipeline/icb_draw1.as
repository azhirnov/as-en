// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void CreatePipelines()
{
	RC<GraphicsPipeline>	ppln = GraphicsPipeline( "icb.draw1" );

	{
		RC<Shader>	vs	= Shader();
		vs.file		= "draw1_vs.glsl";			// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics_rhi/RenderGraph/shaders/draw1_vs.glsl)
		vs.options	= EShaderOpt::Optimize;
		vs.Define( "PER_INSTANCE_OFFSET" );
		ppln.SetVertexShader( vs );
	}
	{
		RC<Shader>	fs	= Shader();
		fs.file		= "draw1_fs.glsl";			// [src](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tests/graphics_rhi/RenderGraph/shaders/draw1_fs.glsl)
		fs.options	= EShaderOpt::Optimize;
		ppln.SetFragmentShader( fs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "icb.draw1.v1" );
		spec.AddToRenderTech( "IndirectCmds.RTech", "Main" );
		spec.SetOptions( EPipelineOpt::IndirectBindable );

		RenderState	rs;
		{
			RenderState_ColorBuffer		cb;
			cb.colorMask.setRGBA( false );
			cb.colorMask.setR( true );
			rs.color.SetColorBuffer( 0, cb );
		}
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "icb.draw1.v2" );
		spec.AddToRenderTech( "IndirectCmds.RTech", "Main" );
		spec.SetOptions( EPipelineOpt::IndirectBindable );

		RenderState	rs;
		{
			RenderState_ColorBuffer		cb;
			cb.colorMask.setRGBA( false );
			cb.colorMask.setG( true );
			rs.color.SetColorBuffer( 0, cb );
		}
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}

	// specialization
	{
		RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "icb.draw1.v3" );
		spec.AddToRenderTech( "IndirectCmds.RTech", "Main" );
		spec.SetOptions( EPipelineOpt::IndirectBindable );

		RenderState	rs;
		{
			RenderState_ColorBuffer		cb;
			cb.colorMask.setRGBA( false );
			cb.colorMask.setB( true );
			rs.color.SetColorBuffer( 0, cb );
		}
		rs.inputAssembly.topology = EPrimitive::TriangleList;

		spec.SetRenderState( rs );
	}
}


void ASmain ()
{
	CreatePipelines();

	RC<IndirectExecutionSet>	exec_set = IndirectExecutionSet( "icb.set1" );
	exec_set.AddToRenderTech( "IndirectCmds.RTech", "Main" );

	exec_set.AddPipeline( "icb.draw1.v1" );
	exec_set.AddPipeline( "icb.draw1.v2" );
	exec_set.AddPipeline( "icb.draw1.v3" );
}
