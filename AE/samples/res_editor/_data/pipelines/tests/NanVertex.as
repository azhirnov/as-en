// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"mediump float2		uv;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl" );
		ppln.SetLayout( "pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "spec" );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			rs.inputAssembly.topology		= EPrimitive::TriangleList;
			rs.rasterization.cullMode		= ECullMode::None;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Math.glsl"

	const float2 g_Positions[] = {
		float2( 0.5, 0.0 ),
		float2( 0.0, 1.0 ),
		float2( 1.0, 1.0 )
	};

	void Main ()
	{
		float2	uv	= g_Positions[ gl.VertexIndex ];

		gl.Position = float4( ToSNorm(uv) * 0.25, 0.0, 1.0 );
		Out.uv		= uv;

		gl.Position.x += (float(gl.InstanceIndex & 3) - 1.5) * 0.5;
		gl.Position.y += (float(gl.InstanceIndex >> 2) - 0.5);

		if ( gl.VertexIndex == (gl.InstanceIndex&3) )
			gl.Position[(gl.InstanceIndex >> 2) & 1] = float_nan;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	void  Main ()
	{
		out_Color = float4(In.uv, 0.0, 1.0);
	}

#endif
//-----------------------------------------------------------------------------
