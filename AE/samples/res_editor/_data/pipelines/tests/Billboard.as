// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"mediump float3		color;"
					"mediump float2		uv;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
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

			rs.inputAssembly.topology		= EPrimitive::TriangleStrip;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Matrix.glsl"
	#include "CodeTemplates.glsl"

	void Main ()
	{
		float2	uv		= ToSNorm( ProceduralQuadUV() );
		float2	zw		= FastProjectZW( un_PerPass.camera.proj, iCoordZ );
		float	min_size = (float(iMinSizePx) * zw[1]) * un_PerPass.invResolution.x;	// in world space
		float	size	= Max( iWorldSize * 0.5, min_size );
		float2	pos		= uv * size;

		// aspect correction
		pos.y *= un_PerPass.resolution.x * un_PerPass.invResolution.y;

		gl.Position	= float4( pos, zw );
		Out.uv		= uv;
		Out.color	= float3(1.0);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "Normal.glsl"

	void Main ()
	{
		out_Color.rgb = In.color * Saturate( 1.0 - Length(In.uv) );
		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
