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
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex, "un_VBuffer",   "VBuffer", EResourceState::ShaderStorage_Read );	// external
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		{
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

				rs.rasterization.frontFaceCCW	= false;
				rs.rasterization.cullMode		= ECullMode::None;

				spec.SetRenderState( rs );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

	void Main ()
	{
		float2	pos = un_VBuffer.vertices[gl.VertexIndex];

		pos *= float(1u << iScale);

		gl.Position	= float4(pos, 0.0, 1.0);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"
	#include "CodeTemplates.glsl"

	float4  QuadGroupId ()
	{
		const float4	colors [] = {
			float4( 1.0, 0.0, 0.0, 1.0 ),	// red			(0,0)
			float4( 1.0, 1.0, 0.0, 1.0 ),	// yellow		(1,0)
			float4( 0.0, 0.0, 1.0, 1.0 ),	// blue			(0,1)
			float4( 1.0, 0.0, 1.0, 1.0 )	// pink			(1,1)
		};
		return colors[ gl.subgroup.Index & 3 ];
	}

	float4  SubgroupId ()
	{
		return Rainbow( float(gl.subgroup.Index) / float(gl.subgroup.Size-1) );
	}

	float4  UniqueSubgroup ()
	{
		float2	sum = gl.subgroup.Add( gl.FragCoord.xy );
		float4	col = Rainbow( DHash12( sum * iHash ));

		// uniform
		if ( un_PerPass.mouse.z > 0.0 )
		{
			float2	pos1 = Floor( un_PerPass.mouse.xy * un_PerPass.resolution.xy );
			float2	pos2 = Floor( gl.FragCoord.xy );
			float	d	 = Distance( pos1, pos2 );
			float	min  = gl.subgroup.Min( d );

			if ( min < 0.01 )
				col = float4(1.0);
		}
		return col;
	}

	float4  HelperInvocationCount ()
	{
		uint i = HelperInvocationCountPerQuad();
		return Rainbow( float(i) / 4.0 );
	}

	float4  FullQuad ()
	{
		float	val	=	DHash12( (Floor( gl.FragCoord.xy / 4.0 ) * 10.0 + 10.0) * iHash );
		float	sum	=	gl.quadGroup.Broadcast( val, 0 ) +
						gl.quadGroup.Broadcast( val, 1 ) +
						gl.quadGroup.Broadcast( val, 2 ) +
						gl.quadGroup.Broadcast( val, 3 );
		return Rainbow( 1.0 - sum / (val * 4.0) );
	}

	float4  FullSubgroup ()
	{
		float	val = 1.0;
		float	sum = gl.subgroup.Add( val );
		return Rainbow( 1.0 - sum / gl.subgroup.Size );
	}

  #ifdef AE_NV_shader_sm_builtins
	float4  SMID ()
	{
		return Rainbow( float(gl.NV.SMID) / gl.NV.SMCount );
	}

	float4  WarpID ()
	{
		return Rainbow( float(gl.NV.WarpID) / gl.NV.WarpsPerSM );
	}

  #elif defined(AE_ARM_shader_core_builtins)
	float4  SMID ()
	{
		return Rainbow( float(gl.ARM.CoreID) / gl.ARM.CoreMaxID );
	}

	float4  WarpID ()
	{
		return Rainbow( float(gl.ARM.WarpID) / gl.ARM.WarpMaxID );
	}

  #else
	float4  SMID ()		{ return float4(0.0); }
	float4  WarpID ()	{ return float4(0.0); }
  #endif

	float4  Wireframe ()
	{
	#ifdef AE_fragment_shader_barycentric
		return float4( Lerp( float3(1.0, 0.0, 0.0), float3(0.0, 0.0, 1.0), FSBarycentricWireframe( 0.5, 1.0 ).x ), 1.0 );
	#else
		return float4(0.0);
	#endif
	}


	void  Main ()
	{
		switch ( iMode )
		{
			case 0 :	out_Color = QuadGroupId();				break;
			case 1 :	out_Color = SubgroupId();				break;
			case 2 :	out_Color = UniqueSubgroup();			break;
			case 3 :	out_Color = FullSubgroup();				break;
			case 4 :	out_Color = HelperInvocationCount();	break;
			case 5 :	out_Color = FullQuad();					break;
			case 6 :	out_Color = SMID();						break;
			case 7 :	out_Color = WarpID();					break;
			case 8 :	out_Color = Wireframe();				break;
		}
	}

#endif
//-----------------------------------------------------------------------------
