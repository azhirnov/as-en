// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		if ( not HasDescriptorSetLayout( "mtr-0.ds" ))
		{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr-0.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
		}

		string	name;
		string	pass;

		if ( not HasPipelineLayout( "pl-0" ))
		{
			RC<PipelineLayout>		pl = PipelineLayout( "pl-0" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr-0.ds" );

			name = "ppln-0";
			pass = "pass-0";
		}
		else
		{
			name = "ppln-1";
			pass = "pass-1";
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( name );
		ppln.SetLayout( "pl-0" );
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
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( name );
			spec.AddToRenderTech( "rtech", pass );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			rs.depth.test					= true;
			rs.depth.write					= true;
			rs.depth.compareOp				= ECompareOp::Always;

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
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
	#include "Hash.glsl"

	void  Main ()
	{
		float2	uv = gl.FragCoord.xy * un_PerPass.invResolution;

		#ifdef iSubpass1

			out_Color2.rgb	= DHash32( uv * 111.0 );
			out_Color2.a	= 1.0;
			gl.FragDepth	= DHash12( uv * 222.0 );

		#else

			out_Color1.rgb	= DHash32( uv * 123.0 );
			out_Color1.a	= 1.0;
			gl.FragDepth	= DHash12( uv * 321.0 );

		#endif
	}

#endif
//-----------------------------------------------------------------------------
