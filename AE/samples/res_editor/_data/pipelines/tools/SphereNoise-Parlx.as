// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw geometry with parallax mapping.
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
					"float3		texcoord;" +
					"float3		worldPos;" +
					"float3		normal;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::AllGraphics,	"un_PerObject",	"SphericalCubeMaterialUB" );
			ds.CombinedImage( EShaderStages::Fragment,		"un_HeightMap",	EImageType::Float_Cube, Sampler_LinearRepeat );
			ds.CombinedImage( EShaderStages::Fragment,		"un_NormalMap",	EImageType::Float_Cube, Sampler_LinearRepeat );
			ds.CombinedImage( EShaderStages::Fragment,		"un_Palette",	EImageType::Float_2D,	Sampler_LinearClamp );
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

				rs.depth.test					= true;
				rs.depth.write					= true;

				rs.inputAssembly.topology		= EPrimitive::TriangleList;

				rs.rasterization.frontFaceCCW	= true;
				rs.rasterization.cullMode		= ECullMode::None;

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
		const float4	pos	= LocalPosToWorldSpace( in_Position.xyz );

		gl.Position		= WorldPosToClipSpace( pos );
		Out.texcoord	= in_Texcoord.xyz;
		Out.worldPos	= pos.xyz;
		Out.normal		= LocalVecToWorldSpace( in_Position.xyz );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#define ExtractDepth( _heightMap_, _uv_ )	ToUNorm( -gl.texture.Sample( _heightMap_, _uv_ ).r * Sign(iHeight) )

	#include "Color.glsl"
	#include "Normal.glsl"
	#include "Matrix.glsl"
	#include "ReliefMapping.glsl"
	#include "ColorSpace.glsl"


	ND_ float3  ReliefMapping (const float3 uv, const float3 viewDir, const float3 worldPos, const float3 lightDir)
	{
		int		num_layers		= 64;
		float3	distorted_uv	= ParallaxOcclusionMapping( un_HeightMap, uv, viewDir, num_layers, Abs(iHeight) );

	#if 1
		float3	normal		= Normalize( gl.texture.Sample( un_NormalMap, distorted_uv ).rgb );
	#else
		float3	normal		= ComputeNormalInWS_dxdy( In.worldPos );
	#endif

		float	lighting	= Max( 0.05, Dot( normal, lightDir ));
		float	noise		= gl.texture.Sample( un_HeightMap, distorted_uv ).r;
		float3	surf_norm	= Normalize( In.normal );

	#if 1
		float	x			= ToUNorm( noise ) * 1.3 - 0.2;
				x			+= Max( 0.0, Dot( normal, surf_norm )) * 0.2;	// for mountains
		float3	color		= gl.texture.Sample( un_Palette, float2(x, 0.5) ).rgb;
	#else
		float3	color		= Rainbow( ToUNorm( -In.noise )).rgb;
	#endif
				color		= RemoveSRGBCurve( color );
				color		*= lighting;
				color		= ApplySRGBCurve( color );
		return color;
	}

	void  Main ()
	{
		float3	view_dir = -ViewDir( un_PerPass.camera.invViewProj, gl.FragCoord.xy * un_PerPass.invResolution );

		out_Color = float4( ReliefMapping( In.texcoord, view_dir, In.worldPos, iLightDir ), 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
