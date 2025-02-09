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
					"float		noise;" +
					"float2		uv;" +
					"float3		worldPos;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.CombinedImage( EShaderStages::Vertex,	"un_HeightMap", EImageType::Float_2D, Sampler_LinearClamp );	// external
			ds.CombinedImage( EShaderStages::Fragment,	"un_NormalMap", EImageType::Float_2D, Sampler_LinearClamp );	// external
			ds.CombinedImage( EShaderStages::Fragment,	"un_Palette",	EImageType::Float_2D, Sampler_LinearClamp );	// external
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		{
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

				rs.depth.test					= true;
				rs.depth.write					= true;

				rs.inputAssembly.topology		= EPrimitive::TriangleStrip;

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
	#include "CodeTemplates.glsl"

	void Main ()
	{
		int2	ipos		= GenGridTriStrip( iTerrainSize.x );
		float2	fpos		= float2(ipos) / float2(iTerrainSize-1);

		float	noise		= gl.texture.SampleLod( un_HeightMap, fpos, 0.0 ).r;
		float3	vert_pos	= float3( ToSNorm(fpos) * iSize, -noise * iHeight ).xzy;	// -Y is up

		if ( ipos.x > iTerrainSize.x )
			vert_pos.x = float_qnan;

		gl.Position		= LocalPosToClipSpace( vert_pos );
		Out.worldPos	= LocalPosToWorldSpace( vert_pos ).xyz;
		Out.noise		= noise;
		Out.uv			= (float2(ipos) + 0.5) / float2(iTerrainSize);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Color.glsl"
	#include "Normal.glsl"
	#include "ColorSpace.glsl"

	void Main ()
	{
	#if 1
		float3	norm		= Normalize( gl.texture.Sample( un_NormalMap, In.uv ).rgb );
	#else
		float3	norm		= ComputeNormalInWS_dxdy( In.worldPos );
	#endif

		float3	light_dir	= Normalize(float3( 0.f, -1.f, 0.5f ));
		float	lighting	= Max( 0.25, Dot( norm, light_dir ));

	#if 1
		float	x			= ToUNorm( In.noise ) * 1.3 - 0.2;
				x			+= Max( 0.0, Dot( norm, float3(0.f, -1.f, 0.f) )) * 0.2;	// for mountains
		float3	color		= gl.texture.Sample( un_Palette, float2(x, 0.5) ).rgb;
	#else
		float3	color		= Rainbow( ToUNorm( -In.noise )).rgb;
	#endif
				color		= RemoveSRGBCurve( color );
				color		*= lighting;
				color		= ApplySRGBCurve( color );

		out_Color = float4( color, 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
