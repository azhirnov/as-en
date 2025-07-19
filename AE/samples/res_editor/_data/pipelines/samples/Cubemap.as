// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw cube with cubemap.
*/
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
					"float3		normal;" +
					"float3		texcoord;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.CombinedImage( EShaderStages::Fragment,	"un_CubeMap",	EImageType::Float_Cube,	 Sampler_LinearMipmapRepeat );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",	"SphericalCubeMaterialUB" );
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

				rs.rasterization.frontFaceCCW	= true;
				rs.rasterization.cullMode		= ECullMode::Front;

				spec.SetRenderState( rs );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Transform.glsl"

	#ifndef iUVMode
	# define iUVMode	1
	#endif

	void Main ()
	{
		Out.texcoord = in_Texcoord.xyz;

		if ( iUVMode == 1 ) Out.texcoord = in_Position.xyz;
		Out.texcoord.y = -Out.texcoord.y;

		gl.Position	= LocalPosToClipSpace( in_Position.xyz );
		Out.normal	= in_Position.xyz;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void Main ()
	{
		out_Color = gl.texture.Sample( un_CubeMap, In.texcoord );
	}

#endif
//-----------------------------------------------------------------------------
