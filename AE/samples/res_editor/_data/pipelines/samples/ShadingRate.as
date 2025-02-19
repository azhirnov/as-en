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
			RC<ShaderStructType>	st = ShaderStructType( "io.vs-fs" );
			st.Set( EStructLayout::InternalIO,
					"float3		worldPos;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::AllGraphics,	"un_PerObject",	"SphericalCubeMaterialUB" );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl" );
			ppln.SetLayout( "pl" );
			ppln.SetVertexInput( "VB{SphericalCubeVertex}" );
			ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
			ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io.vs-fs" );

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
				spec.SetDynamicState( EPipelineDynamicState::FragmentShadingRate );

				RenderState	rs;

				rs.depth.test					= true;
				rs.depth.write					= true;

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
	#include "Transform.glsl"

	void Main ()
	{
		float4	pos		= LocalPosToWorldSpace( in_Position.xyz );

		gl.Position		= WorldPosToClipSpace( pos );
		Out.worldPos	= pos.xyz;

	  #ifdef iPrimRate
		gl.PrimitiveShadingRate = (iPrimRate.y&3) | ((iPrimRate.x&3) << 2);
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Color.glsl"
	#include "Hash.glsl"

	void  Main ()
	{
		uint	y	= (gl.ShadingRate & 3) + 1;				// 1..3
		uint	x	= ((gl.ShadingRate >> 2) & 3) + 1;		// 1..3
		float	h	= DHash13( In.worldPos * 100.0 );

		out_Color = Rainbow( (x * y - 1) / 9.0 );

		out_Color.rgb *= h;
	}

#endif
//-----------------------------------------------------------------------------
