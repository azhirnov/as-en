// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		const string	pass = "pass-2";

		{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr-1.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl1" );
			pl.DSLayout( "pass",	 0, "subpass-2.ds" );
			pl.DSLayout( "material", 1, "mtr-1.ds" );
		}

		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl-1" );
			ppln.SetLayout( "pl1" );
			ppln.SetFragmentOutputFromRenderTech( "rtech", pass );

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
				RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "spec-1" );
				spec.AddToRenderTech( "rtech", pass );  // in ScriptSceneGraphicsPass

				RenderState	rs;

				rs.depth.test					= false;
				rs.depth.write					= false;

				rs.inputAssembly.topology		= EPrimitive::TriangleList;

				rs.rasterization.frontFaceCCW	= true;
				rs.rasterization.cullMode		= ECullMode::Back;

				spec.SetRenderState( rs );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "CodeTemplates.glsl"

	void Main ()
	{
		gl.Position = FullscreenTrianglePos();
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	void  Main ()
	{
		float2	uv = gl.FragCoord.xy * un_PerPass.invResolution;

		switch ( iMode )
		{
			case 0 :	out_Color1 = gl.subpass.Load( in_Color1 );				break;
			case 1 :	out_Color1 = gl.subpass.Load( in_Color2 );				break;
			case 2 :	out_Color1 = float4(gl.subpass.Load( in_Depth1 ).r);	break;
			case 3 :	out_Color1 = float4(gl.subpass.Load( in_Depth2 ).r);	break;
		}
	}

#endif
//-----------------------------------------------------------------------------
