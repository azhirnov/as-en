// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw TBN vectors for debugging.
	'DbgTBN' can be:
		0 - draw nothing
		1 - per triangle TBN & UV, triangle border
		2 - per vertex TBN
		3 - triangle border

	Line colors:
		Normal:		red
		Tangent:	green
		Bitangent:	blue
		U:			yellow
		V:			violet
*/
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "vs_gs.io" );
			st.Set( EStructLayout::InternalIO,
					"float3		normal;" +
					"float3		tangent;" +
					"float3		bitangent;" +
					"float2		texcoord;" );
		}{
			RC<ShaderStructType>	st = ShaderStructType( "gs_fs.io" );
			st.Set( EStructLayout::InternalIO,
					"float3		color;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",			"UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex,	"un_Geometry",			"GeometrySBlock", EResourceState::ShaderStorage_Read );	// external
			ds.CombinedImage( EShaderStages::Fragment,	"un_ColorMap",			EImageType::Float_2D, Sampler_LinearMipmapRepeat );
			ds.CombinedImage( EShaderStages::Fragment,	"un_HeightNormalMap",	EImageType::Float_2D, Sampler_LinearMipmapRepeat );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl" );
			ppln.SetLayout( "pl" );
			ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
			ppln.SetShaderIO( EShader::Vertex,   EShader::Geometry, "vs_gs.io" );
			ppln.SetShaderIO( EShader::Geometry, EShader::Fragment, "gs_fs.io" );

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

				rs.inputAssembly.topology		= EPrimitive::TriangleList;		// input - triangles, output - lines

				rs.rasterization.frontFaceCCW	= true;
				rs.rasterization.cullMode		= ECullMode::None;
				rs.rasterization.lineWidth		= 4;

				spec.SetRenderState( rs );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Transform.glsl"

	void Main ()
	{
		const uint		idx		 = gl.VertexIndex;
		const float4	pos		 = LocalPosToWorldSpace( un_Geometry.positions[idx] );

		gl.Position		= pos;
		Out.texcoord	= un_Geometry.texcoords[idx];
		Out.normal		= LocalVecToWorldSpace( un_Geometry.normals[idx] );
		Out.tangent		= LocalVecToWorldSpace( un_Geometry.tangents[idx] );
		Out.bitangent	= LocalVecToWorldSpace( un_Geometry.bitangents[idx] );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_GEOM

	#ifndef DbgTBN
	#define DbgTBN	0
	#endif

	layout (triangles) in;
	layout (line_strip, max_vertices = 18) out;

	#include "Math.glsl"

	#define P0		gl_in[0].gl_Position.xyz
	#define P1		gl_in[1].gl_Position.xyz
	#define P2		gl_in[2].gl_Position.xyz

	const float	c_Length	= 0.25;
	const float	c_Offset	= 0.025;


	// 6 verts, 3 prims
	void  DrawPerTriangleTBN ()
	{
		const float3	pos			= (P0 + P1 + P2) / 3.0f;
		const float3	normal		= Normalize( In[0].normal + In[1].normal + In[2].normal );
		const float3	tangent		= Normalize( In[0].tangent + In[1].tangent + In[2].tangent );
		const float3	bitangent	= Normalize( In[0].bitangent + In[1].bitangent + In[2].bitangent );
		{
			gl.Position	= un_PerPass.camera.viewProj * float4(pos, 1.0f);
			Out.color	= float3(0.2, 0.0, 0.0);
			gl.EmitVertex();

			gl.Position	= un_PerPass.camera.viewProj * float4(pos + normal * c_Length, 1.0f);
			Out.color	= float3(1.0, 0.0, 0.0);
			gl.EmitVertex();

			gl.EndPrimitive();
		}{
			gl.Position	= un_PerPass.camera.viewProj * float4(pos, 1.0f);
			Out.color	= float3(0.0, 0.2, 0.0);
			gl.EmitVertex();

			gl.Position	= un_PerPass.camera.viewProj * float4(pos + tangent * c_Length, 1.0f);
			Out.color	= float3(0.0, 1.0, 0.0);
			gl.EmitVertex();

			gl.EndPrimitive();
		}{
			gl.Position	= un_PerPass.camera.viewProj * float4(pos, 1.0f);
			Out.color	= float3(0.0, 0.0, 0.2);
			gl.EmitVertex();

			gl.Position	= un_PerPass.camera.viewProj * float4(pos + bitangent * c_Length, 1.0f);
			Out.color	= float3(0.0, 0.0, 1.0);
			gl.EmitVertex();

			gl.EndPrimitive();
		}
	}


	// 4 verts, 2 prims
	void  DrawPerTriangleUV ()
	{
		const float3	v10		= Normalize( P1 - P0 );
		const float3	v20		= Normalize( P2 - P0 );
		const float2	duv0	= Normalize( In[1].texcoord - In[0].texcoord );
		const float2	duv1	= Normalize( In[2].texcoord - In[0].texcoord );
		const float3	u_dir	= v10 * duv0.x + v20 * duv0.y;
		const float3	v_dir	= v10 * duv1.x + v20 * duv1.y;
		const float3	center	= (P0 + P1 + P2) / 3.0f;
		const float3	pos		= center + (v_dir + u_dir) * c_Offset;
		{
			gl.Position	= un_PerPass.camera.viewProj * float4(pos, 1.0f);
			Out.color	= float3(0.2, 0.2, 0.2);
			gl.EmitVertex();

			gl.Position	= un_PerPass.camera.viewProj * float4(pos + u_dir * c_Length, 1.0f);
			Out.color	= float3(1.0, 1.0, 0.0);
			gl.EmitVertex();

			gl.EndPrimitive();
		}{
			gl.Position	= un_PerPass.camera.viewProj * float4(pos, 1.0f);
			Out.color	= float3(0.2, 0.2, 0.2);
			gl.EmitVertex();

			gl.Position	= un_PerPass.camera.viewProj * float4(pos + v_dir * c_Length, 1.0f);
			Out.color	= float3(1.0, 0.0, 1.0);
			gl.EmitVertex();

			gl.EndPrimitive();
		}
	}


	// 18 verts, 9 prims
	void  DrawPerVertexTBN ()
	{
		for (int i = 0; i < 3; ++i)
		{
			float3	pos			= gl_in[i].gl_Position.xyz;
			float3	normal		= In[i].normal;
			float3	tangent		= In[i].tangent;
			float3	bitangent	= In[i].bitangent;

			{
				gl.Position	= un_PerPass.camera.viewProj * float4(pos, 1.0f);
				Out.color	= float3(0.2, 0.0, 0.0);
				gl.EmitVertex();

				gl.Position	= un_PerPass.camera.viewProj * float4(pos + normal * c_Length, 1.0f);
				Out.color	= float3(1.0, 0.0, 0.0);
				gl.EmitVertex();

				gl.EndPrimitive();
			}{
				gl.Position	= un_PerPass.camera.viewProj * float4(pos, 1.0f);
				Out.color	= float3(0.0, 0.2, 0.0);
				gl.EmitVertex();

				gl.Position	= un_PerPass.camera.viewProj * float4(pos + tangent * c_Length, 1.0f);
				Out.color	= float3(0.0, 1.0, 0.0);
				gl.EmitVertex();

				gl.EndPrimitive();
			}{
				gl.Position	= un_PerPass.camera.viewProj * float4(pos, 1.0f);
				Out.color	= float3(0.0, 0.0, 0.2);
				gl.EmitVertex();

				gl.Position	= un_PerPass.camera.viewProj * float4(pos + bitangent * c_Length, 1.0f);
				Out.color	= float3(0.0, 0.0, 1.0);
				gl.EmitVertex();

				gl.EndPrimitive();
			}
		}
	}


	// 4 verts, 1 prim
	void  DrawTriangleBorder ()
	{
		for (int i = 0; i < 3; ++i)
		{
			gl.Position	= un_PerPass.camera.viewProj * gl_in[i].gl_Position;
			Out.color	= float3(1.0);
			gl.EmitVertex();
		}

		gl.Position	= un_PerPass.camera.viewProj * gl_in[0].gl_Position;
		Out.color	= float3(1.0);
		gl.EmitVertex();

		gl.EndPrimitive();
	}


	void Main ()
	{
		// per triangle (14 verts)
		if ( DbgTBN == 1 )
		{
			DrawPerTriangleTBN();
			DrawPerTriangleUV();
			DrawTriangleBorder();
		}
		else
		// per vertex (18 verts)
		if ( DbgTBN == 2 )
		{
			DrawPerVertexTBN();
		}
		else
		// face border (4 verts)
		if ( DbgTBN == 3 )
		{
			DrawTriangleBorder();
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void Main ()
	{
		out_Color = float4(In.color, 1.f);
	}

#endif
//-----------------------------------------------------------------------------
