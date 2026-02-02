// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  ASmain ()
	{
		const string prefix = "light_cone";

		{
			RC<ShaderStructType>	st = ShaderStructType( prefix+".io" );
			st.Set( EStructLayout::InternalIO,
					"mediump float4		color;"s +
					"mediump float2		uv;" +
					"flat uint			lightId;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( prefix+".mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex, "un_LightObjs", "LightObject_Array", EResourceState::ShaderStorage_Read );	// external
		}{
			RC<PipelineLayout>		pl = PipelineLayout( prefix+".pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, prefix+".mtr.ds" );
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( prefix+".t" );
		ppln.SetLayout( prefix+".pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, prefix+".io" );

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
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;
			{
				RenderState_ColorBuffer		cb;
				cb.SrcBlendFactor( EBlendFactor::One );
				cb.DstBlendFactor( EBlendFactor::One );
				cb.BlendOp( EBlendOp::Add );
				rs.color.SetColorBuffer( 0, cb );
			}
			rs.depth.test					= true;
			rs.depth.write					= false;
			rs.depth.compareOp				= ECompareOp::GEqual;

			rs.inputAssembly.topology		= EPrimitive::TriangleStrip;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::None;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Cone.glsl"
	#include "Transform.glsl"

	const float2	c_UV [] = {
		float2(0.0, 1.0),
		float2(0.0, 0.0),
		float2(0.5, 1.0),
		float2(1.0, 0.0),
		float2(1.0, 1.0)
	};

	void Main ()
	{
		LightObject		obj			= un_LightObjs.elements[ gl.InstanceIndex ];
		Cone			cone		= Cone_Create( obj.position, obj.dir, obj.angle, obj.height );

		float4			apex_pos	= LocalPosToViewSpace( cone.origin );
		float4			base_pos	= LocalPosToViewSpace( Cone_BaseCenter( cone ));
		float			top_size	= 0.025;
		float			bottom_size	= 0.3;
		float2			uv			= c_UV[gl.VertexIndex];

		float4			view_pos	= Lerp( apex_pos, base_pos, uv.y );
						view_pos.x	+= Lerp( top_size, bottom_size, uv.y ) * ToSNorm( uv.x );

		float			atten		= Saturate( 4.0 / view_pos.z );

		gl.Position		= un_PerPass.camera.proj * view_pos;
		Out.uv			= float2( ToSNorm( uv.x ), uv.y );
		Out.color		= unpackUnorm4x8( obj.color ) * obj.brightness * atten;
		Out.lightId		= gl.InstanceIndex;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "Normal.glsl"

	void Main ()
	{
		out_Color = In.color * Max( 0.0, 1.0 - Square(In.uv.x) * 2.0 );
	}

#endif
//-----------------------------------------------------------------------------
