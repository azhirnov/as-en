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
			RC<ShaderStructType>	st = ShaderStructType( "vs-gs.io" );
			st.Set( EStructLayout::InternalIO,
					"uint		coneIdx;" +
					"uint		rayIdx;" );
		}{
			RC<ShaderStructType>	st = ShaderStructType( "gs-fs.io" );
			st.Set( EStructLayout::InternalIO,
					"float4		color;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Geometry,	"un_PerObject",  "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Geometry,	"un_RayStorage", "LightCone_Array2",	EResourceState::ShaderStorage_Read );	// external
			ds.StorageBuffer( EShaderStages::Geometry,	"un_Constants",  "RTConstants",			EResourceState::ShaderStorage_Read );	// external
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl" );
			ppln.SetLayout( "pl" );
			ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
			ppln.SetShaderIO( EShader::Vertex,		EShader::Geometry, "vs-gs.io" );
			ppln.SetShaderIO( EShader::Geometry,	EShader::Fragment, "gs-fs.io" );

			{
				RC<Shader>	vs = Shader();
				vs.LoadSelf();
				ppln.SetVertexShader( vs );
			}{
				RC<Shader>	gs = Shader();
				gs.LoadSelf();
				ppln.SetGeometryShader( gs );
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
					RenderState_ColorBuffer		cb;
					cb.SrcBlendFactor( EBlendFactor::One );
					cb.DstBlendFactor( EBlendFactor::One );
					cb.BlendOp( EBlendOp::Add );
					rs.color.SetColorBuffer( 0, cb );
				}
				rs.depth.test					= false;
				rs.depth.write					= false;

				rs.inputAssembly.topology		= EPrimitive::LineStrip;

				rs.rasterization.frontFaceCCW	= true;
				rs.rasterization.cullMode		= ECullMode::Back;

				spec.SetRenderState( rs );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

	void Main ()
	{
		Out.coneIdx	= gl.VertexIndex;
		Out.rayIdx	= gl.InstanceIndex;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_GEOM
	#include "Color.glsl"
	#include "Normal.glsl"

	layout (lines) in;
	layout (triangle_strip, max_vertices = 4) out;

	void Main ()
	{
		if ( In[0].rayIdx >= un_RayStorage.rayCount )
			return;

		//           /
		//  /
		//  \ 
		//  cone0    \
		//          cone1

		const uint	cone0_idx	= un_RayStorage.rayToCone[ In[0].rayIdx ].x + In[0].coneIdx;
		const uint	cone1_idx	= un_RayStorage.rayToCone[ In[0].rayIdx ].x + In[1].coneIdx;
		const uint	cont_cnt	= Min( un_RayStorage.rayToCone[ In[0].rayIdx ].y, 16 );

		if ( In[0].coneIdx >= cont_cnt or
			 In[1].coneIdx >= cont_cnt )
			return;

		const float	wl	= un_RayStorage.elements[ cone0_idx ].wavelength;
		float3		col = float3(0.0);

		for (uint i = 0; i < un_Constants.wavelengthToRGB.length(); ++i)
		{
			if ( wl == un_Constants.wavelengthToRGB[i].x )
				col = un_Constants.wavelengthToRGB[i].yzw;	// linear space
		}

		const float3	v [] = {
			float3( un_RayStorage.elements[ cone0_idx ].origin0, 0.f ),
			float3( un_RayStorage.elements[ cone1_idx ].origin0, 0.f ),
			float3( un_RayStorage.elements[ cone0_idx ].origin1, 0.f ),
			float3( un_RayStorage.elements[ cone1_idx ].origin1, 0.f )
		};

		float	s0 = 1.0 / Max( 0.001, Distance( v[0], v[2] ));
		float	s1 = 1.0 / Max( 0.001, Distance( v[1], v[3] ));

		const float		e [] = {
			un_RayStorage.elements[ cone0_idx ].energy[0] * s0,
			un_RayStorage.elements[ cone0_idx ].energy[0] * s1,
			un_RayStorage.elements[ cone0_idx ].energy[1] * s0,
			un_RayStorage.elements[ cone0_idx ].energy[1] * s1
		};
		const float3	n	= ComputeNormal( v[3].xyz, v[1].xyz, v[2].xyz );
		const uint4		i	= n.z < 0.0 ? uint4(3, 1, 2, 0) : uint4(3, 2, 1, 0);

		gl.Position		= float4( v[i[0]], 1.f );
		Out.color.rgb	= col * e[i[0]];
		Out.color.a		= 1.0;
		gl.EmitVertex();

		gl.Position		= float4( v[i[1]], 1.f );
		Out.color.rgb	= col * e[i[1]];
		Out.color.a		= 1.0;
		gl.EmitVertex();

		gl.Position		= float4( v[i[2]], 1.f );
		Out.color.rgb	= col * e[i[2]];
		Out.color.a		= 1.0;
		gl.EmitVertex();

		gl.Position		= float4( v[i[3]], 1.f );
		Out.color.rgb	= col * e[i[3]];
		Out.color.a		= 1.0;
		gl.EmitVertex();

		gl.EndPrimitive();
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void  Main ()
	{
		out_Color = In.color;
	}

#endif
//-----------------------------------------------------------------------------
