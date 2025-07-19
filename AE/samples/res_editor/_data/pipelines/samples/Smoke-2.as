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
					"float2		uv;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",		"UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex,	"un_InstBuffer",	"InstanceData_Array", EResourceState::ShaderStorage_Read );	// external
			ds.CombinedImage( EShaderStages::Fragment,	"un_Smoke", EImageType::Float_2D, Sampler_LinearMipmapClamp );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

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
			{
				RenderState_ColorBuffer	cb;
				cb.blend = true;
				cb.BlendOp( EBlendOp::Add );
				cb.SrcBlendFactor( EBlendFactor::One );
				cb.DstBlendFactor( EBlendFactor::OneMinusSrcAlpha );

				rs.color.SetColorBuffer( 0, cb );
			}

			rs.inputAssembly.topology		= EPrimitive::TriangleStrip;

			rs.rasterization.frontFaceCCW	= false;
			rs.rasterization.cullMode		= ECullMode::None;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "CodeTemplates.glsl"

	void Main ()
	{
		InstanceData	data	= un_InstBuffer.elements[ gl.InstanceIndex ];
		float2			uv		= ProceduralQuadUV();
		float2			pos		= data.m1 * float3( ToSNorm( uv ), 1.0 );

		if ( data.t < 0.0 )
			pos.x = float_qnan;

		gl.Position	= float4(pos, 0.0, 1.0);
		Out.uv		= uv;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "ColorSpace.glsl"

	void  Main ()
	{
		float	a = gl.texture.Sample( un_Smoke, In.uv ).r;
				a = RemoveSRGBCurve( a ) * iColor.a;
		float4	c = RemoveSRGBCurve( iColor );
		out_Color = c * a;
	}

#endif
//-----------------------------------------------------------------------------
