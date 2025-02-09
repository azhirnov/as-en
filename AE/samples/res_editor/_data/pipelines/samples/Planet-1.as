// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw spherical cube with cubemap.
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
			RC<ShaderStructType>	st = ShaderStructType( "io.vs-fs" );
			st.Set( EStructLayout::InternalIO,
					"float3		texcoord;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.CombinedImage( EShaderStages::Fragment,	"un_HeightMap",		EImageType::Float_Cube,	 Sampler_LinearMipmapRepeat );
			ds.CombinedImage( EShaderStages::Fragment,	"un_NormalMap",		EImageType::Float_Cube,	 Sampler_LinearMipmapRepeat );
			ds.CombinedImage( EShaderStages::Fragment,	"un_AlbedoMap",		EImageType::Float_Cube,	 Sampler_LinearMipmapRepeat );
			ds.CombinedImage( EShaderStages::Fragment,	"un_EmissionMap",	EImageType::Float_Cube,	 Sampler_LinearMipmapRepeat );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",		"SphericalCubeMaterialUB" );
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
		gl.Position		= LocalPosToClipSpace( in_Position.xyz );
		Out.texcoord	= in_Texcoord.xyz;
		//Out.normal	= LocalVecToWorldSpace( in_Position.xyz );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void Main ()
	{
		const float3	light_dir	= float3( 0.f, 0.f, 1.f );
		const float3	norm		= Normalize( gl.texture.Sample( un_NormalMap, In.texcoord ).xyz );
		const float3	color		= gl.texture.Sample( un_AlbedoMap, In.texcoord ).xyz;
		const float		light		= Dot( norm, light_dir );

		out_Color = float4( color * light, 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
