// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw scene with shadows.
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
					"float3		worldPos;" +
					"float3		normal;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",	"UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex,	"un_Geometry",	"GeometrySBlock",	EResourceState::ShaderStorage_Read );	// external
			ds.StorageBuffer( EShaderStages::Fragment,	"un_Lights",	"LightsSBlock",		EResourceState::ShaderStorage_Read );	// external
			ds.CombinedImage( EShaderStages::Fragment,	"un_ShadowMap",	EImageType::Float_2D, Sampler_NearestClamp );
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
		const uint		idx	= gl.VertexIndex;
		const float4	pos	= LocalPosToWorldSpace( un_Geometry.positions[idx] );

		gl.Position		= WorldPosToClipSpace( pos );
		Out.normal		= LocalVecToWorldSpace( un_Geometry.normals[idx] );
		Out.worldPos	= pos.xyz;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	const float		c_Ambient = 0.2;

	float  SampleShadow (in float4 shadowCoord, in float2 off)
	{
		float shadow = 1.0;
		if ( All2( shadowCoord.z > -1.0, shadowCoord.z < 1.0 ))
		{
			float dist = gl.texture.Sample( un_ShadowMap, shadowCoord.xy + off ).r;
			if ( All2( shadowCoord.w > 0.0, dist < shadowCoord.z ))
			{
				shadow = c_Ambient;
			}
		}
		return shadow;
	}

	float  SampleShadowPCF (float4 sc)
	{
		float	accum = 0.0;
		for (int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				accum += SampleShadow( sc, un_Lights.invShadowDim * 1.5 * float2(x, y) );
			}
		}
		return accum / 9.0;
	}

	void  Main ()
	{
		float3	view_pos	= In.worldPos + un_PerPass.camera.pos;
		float4	sm_coord	= (un_Lights.viewToShadow) * float4(view_pos, 1.0);  sm_coord /= sm_coord.w;
		float	shadow		= iPCF == 0 ? SampleShadow( sm_coord, float2(0.0) ) : SampleShadowPCF( sm_coord );
		float3	normal		= Normalize( In.normal );
		float3	light_dir	= un_Lights.lightDir;
		float	n_dot_l		= Dot( normal, light_dir );
		float3	diffuse		= float3(Max( n_dot_l, c_Ambient ));

		out_Color = float4( diffuse * shadow, 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
