// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  CreatePipeline (string name, string define)
	{
		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl"+name );
		ppln.SetLayout( "pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			vs.Define( define );
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			fs.Define( define );
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( name );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			rs.inputAssembly.topology		= EPrimitive::TriangleStrip;

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
					"float2		uv;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject", "UnifiedGeometryMaterialUB" );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		CreatePipeline( "WithInstancing",		"INSTANCING=1" );
		CreatePipeline( "WithoutInstancing",	"INSTANCING=0" );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Transform.glsl"
	#include "CodeTemplates.glsl"

	void Main ()
	{
	#if INSTANCING
		int2	ipos	= GenGridWithInstancingTriStrip();
	#else
		int2	ipos	= GenGridTriStrip( iGridSize );
	#endif

		float3	vpos	= float3( ToSNorm(float2(ipos) / float2(iGridSize)), 0.f );
	#if INSTANCING == 0
		if ( ipos.x > iGridSize )
			vpos.x = float_qnan;
	#endif

		gl.Position		= LocalPosToClipSpace( vpos );
		Out.uv			= float2(ipos);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "CodeTemplates.glsl"

	void Main ()
	{
		float	wireframe = 1.f;

		switch ( iWireframe )
		{
			case 0 :
				wireframe = Sqrt( AA_QuadGrid_dxdy( In.uv, float2(0.0, 2.75) ).x );
				break;

			case 1 :
			  #ifdef AE_fragment_shader_barycentric
				wireframe = FSBarycentricWireframe( 0.0, 1.0 ).x;
			  #endif
				break;
		}

	#if INSTANCING
		out_Color.rgb = float3(1.0, 0.4, 0.3) * wireframe;
	#else
		out_Color.rgb = float3(0.2, 1.0, 0.3) * wireframe;
	#endif
		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
