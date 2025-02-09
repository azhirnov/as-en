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

				rs.inputAssembly.topology		= EPrimitive::LineStrip;

				rs.rasterization.frontFaceCCW	= true;
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
		Out.coneIdx	= gl.VertexIndex;
		Out.rayIdx	= gl.InstanceIndex;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_GEOM
	#include "Color.glsl"

	layout (lines) in;
	layout (line_strip, max_vertices = 4) out;

	void Main ()
	{
		if ( In[0].rayIdx >= un_RayStorage.rayCount )
			return;

		const uint	cone0_idx	= un_RayStorage.rayToCone[ In[0].rayIdx ].x + In[0].coneIdx;
		const uint	cone1_idx	= un_RayStorage.rayToCone[ In[0].rayIdx ].x + In[1].coneIdx;
		const uint	cont_cnt	= Min( un_RayStorage.rayToCone[ In[0].rayIdx ].y, 16 );

		if ( In[0].coneIdx >= cont_cnt or
			 In[1].coneIdx >= cont_cnt )
			return;

		float4	col = Rainbow( float(In[0].coneIdx) / cont_cnt );

		// left ray
		{
			gl.Position	= float4( un_RayStorage.elements[ cone0_idx ].origin0, 0.f, 1.f );
			Out.color	= col;
			gl.EmitVertex();

			gl.Position	= float4( un_RayStorage.elements[ cone1_idx ].origin0, 0.f, 1.f );
			Out.color	= col;
			gl.EmitVertex();

			gl.EndPrimitive();
		}

		// right ray
		{
			gl.Position	= float4( un_RayStorage.elements[ cone0_idx ].origin1, 0.f, 1.f );
			Out.color	= col;
			gl.EmitVertex();

			gl.Position	= float4( un_RayStorage.elements[ cone1_idx ].origin1, 0.f, 1.f );
			Out.color	= col;
			gl.EmitVertex();

			gl.EndPrimitive();
		}
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
