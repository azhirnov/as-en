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
					"float2		pos;" +
					"float2		center;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",	"UnifiedGeometryMaterialUB" );
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

			rs.rasterization.frontFaceCCW	= false;
			rs.rasterization.cullMode		= ECullMode::None;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

	void Main ()
	{
		gl.Position	= float4( un_VBuffer.positions[ gl.VertexIndex ] * iScale, 0.0, 1.0 );
		Out.pos		= un_VBuffer.positions[ gl.VertexIndex ];
		Out.center	= un_VBuffer.positions[ un_VBuffer.centerIdx[ gl.VertexIndex ]];
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "CodeTemplates.glsl"

	void  Main ()
	{
		float	d = Distance( In.pos, In.center ) * 2.0;
		d = Saturate( 1.0 - d );

		if ( iIsolines == 1 )
			out_Color = float4( SDF_Isolines( d * 10.0 ), 1.0 );
		else
			out_Color = float4( d*d );

	#ifdef AE_fragment_shader_barycentric
		if ( iWire == 1 )
			out_Color += float4(1.0, 1.0, 0.0, 0.0) * (1.0 - FSBarycentricWireframe( 1.0, 2.5 ).x);
	#endif
	}

#endif
//-----------------------------------------------------------------------------
