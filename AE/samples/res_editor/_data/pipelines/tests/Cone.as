// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#	define CONE_VS
#	define BOX_VS
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ConePipeline ()
	{
		const string	prefix = "cone";

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( prefix+".tmpl" );
		ppln.SetLayout( "pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.Define( "CONE_VS" );
			vs.LoadSelf();
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			rs.depth.test					= true;
			rs.depth.write					= true;
			rs.depth.compareOp				= ECompareOp::LEqual;

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}


	void BoxPipeline ()
	{
		const string	prefix = "box";

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( prefix+".tmpl" );
		ppln.SetLayout( "pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.Define( "BOX_VS" );
			vs.LoadSelf();
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;
			{
				RenderState_ColorBuffer	cb;
				cb.blend = true;
				cb.BlendOp( EBlendOp::Add );
				cb.SrcBlendFactor( EBlendFactor::SrcAlpha );
				cb.DstBlendFactor( EBlendFactor::OneMinusSrcAlpha );
				rs.color.SetColorBuffer( 0, cb );
			}

			rs.depth.test					= true;
			rs.depth.write					= true;
			rs.depth.compareOp				= ECompareOp::LEqual;

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}


	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"float4		color;"
					"float4		worldPos;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",	"UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex,	"un_Geometry",	"GeometrySBlock",	EResourceState::ShaderStorage_Read );	// external
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		ConePipeline();
		BoxPipeline();
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CONE_VS
	#include "Transform.glsl"
	#include "Quaternion.glsl"
	#include "Cone.glsl"

	void Main ()
	{
		const float		height		= un_Params.coneHeight;
		const float3	cone_pos	= un_Params.coneOrigin;
		const float3	dir			= un_Params.coneDir;
		const float		angle		= un_Params.coneAngle;
		const Cone		cone		= Cone_Create( cone_pos - un_PerPass.camera.pos, dir, angle, height );
		const float		radius		= Cone_BaseRadius( cone );

		Quat			rot			= QFrom2Normals( float3(0.0, 1.0, 0.0), // origin direction
													 dir );					// new direction

		const uint		idx			= gl.VertexIndex;
		const float3	vert_pos	= cone.origin + QMul( rot, (un_Geometry.position[idx] * float3(radius, radius, height)).xzy );
		const float4	world_pos	= float4( vert_pos, 1.0 );
		float4			color		= float4(0.0, 0.9, 0.0, 1.0);

		gl.Position		= WorldPosToClipSpace( world_pos );
		Out.color		= color;
		Out.worldPos	= world_pos;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef BOX_VS
	#include "Transform.glsl"

	void Main ()
	{
		const float3	center		= un_Params.boxCenter;
		const float3	hsize		= un_Params.boxHalfSize;

		const float3	vert_pos	= un_Geometry.position[ gl.VertexIndex ] * hsize + center;
		const float4	world_pos	= float4( vert_pos - un_PerPass.camera.pos, 1.0 );
		float4			color		= float4(0.2, 0.3, 0.9, un_Params.boxAlpha);

		gl.Position		= WorldPosToClipSpace( world_pos );
		Out.color		= color;
		Out.worldPos	= world_pos;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Normal.glsl"

	void Main ()
	{
		float3	norm		= ComputeNormalInWS_dxdy( In.worldPos.xyz );
		float3	light_dir	= Normalize(float3( 0.f, -1.f, 0.5f ));
		float	lighting	= Max( Dot( norm, light_dir ), 0.0f ) + 0.2f;

		out_Color.rgb	= In.color.rgb * lighting;
		out_Color.a		= In.color.a;
	}

#endif
//-----------------------------------------------------------------------------
