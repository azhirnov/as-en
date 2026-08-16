// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		{
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
			rs.rasterization.cullMode		= ECullMode::None;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Geometry2D.glsl"

	#ifndef iExtrudePx
	#	define iExtrudePx	0.f
	#endif

	void Main ()
	{
		float2	pos;
		float2	norm;

		switch ( gl.VertexIndex )
		{
			case 0 :	pos = iP0;	norm = Normalize( iP0 - iP1 ) + Normalize( iP0 - iP2 );	break;
			case 1 :	pos = iP1;	norm = Normalize( iP1 - iP0 ) + Normalize( iP1 - iP2 );	break;
			case 2 :	pos = iP2;	norm = Normalize( iP2 - iP0 ) + Normalize( iP2 - iP1 );	break;
		}

		pos += Normalize( norm ) * un_PerPass.invResolution * iExtrudePx;

		gl.Position = float4( pos, 0.0, 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	void  Main ()
	{
		out_Color = float4(1.0);
	}

#endif
//-----------------------------------------------------------------------------
