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
					"float3		normal;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Geometry,	"un_PerObject",	"UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex,	"un_Geometry",	"GeometrySBlock", EResourceState::ShaderStorage_Read );	// external
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl" );
			ppln.SetLayout( "pl" );
			ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
			ppln.SetShaderIO( EShader::Geometry, EShader::Fragment, "io" );

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

				rs.depth.test					= true;
				rs.depth.write					= true;

				rs.inputAssembly.topology		= EPrimitive::TriangleList;

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
		const uint	idx	= gl.VertexIndex;
		gl.Position		= float4( un_Geometry.positions[idx], 1.f );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_GEOM
	#include "Transform.glsl"

	layout (triangles) in;
	layout (triangle_strip, max_vertices = 3) out;

	void Main ()
	{
		float3	edge1	= (gl_in[1].gl_Position - gl_in[0].gl_Position).xyz;
		float3	edge2	= (gl_in[2].gl_Position - gl_in[0].gl_Position).xyz;
		float3	norm	= Normalize( Cross( edge1, edge2 ));

		gl.Position	= LocalPosToClipSpace( gl_in[0].gl_Position );
		Out.normal	= norm;
		gl.EmitVertex();

		gl.Position	= LocalPosToClipSpace( gl_in[1].gl_Position );
		Out.normal	= norm;
		gl.EmitVertex();

		gl.Position	= LocalPosToClipSpace( gl_in[2].gl_Position );
		Out.normal	= norm;
		gl.EmitVertex();

		gl.EndPrimitive();
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Normal.glsl"
	#include "CodeTemplates.glsl"

	void  Main ()
	{
		out_Color = (gl.FrontFacing ?
						float4(0.0, 1.0, 0.0, 1.0) :
						float4(1.0, 0.0, 0.0, 1.0));

		// wireframe
		{
			const float		thickness	= 1.5;	// pixels
			const float		falloff		= 6.0;	// pixels

			out_Color *= FSBarycentricWireframe( thickness, falloff ).x;
		}

		// normals
		if ( iDbgNormals == 1 )
		{
			float3	normal = Normalize( In.normal );// ComputeNormalInWS_dxdy( In.worldPos );

			out_Color = gl.FragCoord.x > un_PerPass.resolution.x*0.5 ?
							float4(normal, 1.0f) :
							float4(-normal, 1.0f);
		}
	}

#endif
//-----------------------------------------------------------------------------
