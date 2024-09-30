// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"float		instanceId;" +
					"float2		uv;" );
		}{
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
	#include "Math.glsl"

	void Main ()
	{
		float2	pos = un_VBuffer.vertices[gl.VertexIndex];
				pos = gl.InstanceIndex == 0 ? pos : -pos + (iWithOffset == 1 ? 0.01 : 0.0);

		pos *= float(1u << iScale);

		gl.Position		= float4(pos, 0.0, 1.0);
		Out.instanceId	= float(gl.InstanceIndex);
		Out.uv			= ToUNorm( pos );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "GlobalIndex.glsl"
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
		float3	sum = gl.subgroup.Add(float3( gl.FragCoord.xy, In.instanceId ));
		return Rainbow( DHash13( sum * iHash ));
	}

	float4  HelperInvocationCount ()
	{
		uint i = HelperInvocationCountPerQuad();
		return Rainbow( float(i) / 3.0 );
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


	void  Main ()
	{
		out_Color = gl.texture.Sample( un_Texture, In.uv ) * 0.001;

		switch ( iMode )
		{
			case 0 :	out_Color += QuadGroupId();					break;
			case 1 :	out_Color += SubgroupId();					break;
			case 2 :	out_Color += UniqueSubgroup();				break;
			case 3 :	out_Color += FullSubgroup();				break;
			case 4 :	out_Color += HelperInvocationCount();		break;
			case 5 :	out_Color += FullQuad();					break;
		}
	}

#endif
//-----------------------------------------------------------------------------
