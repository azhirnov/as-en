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
					"float3		normal;" +
					"int		face;" );
		}{
			RC<ShaderStructType>	st = ShaderStructType( "gs-fs.io" );
			st.Set( EStructLayout::InternalIO,
					"float3		normal;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",	"UnifiedGeometryMaterialUB" );
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
			ppln.SetShaderIO( EShader::Vertex,   EShader::Geometry, "vs-gs.io" );
			ppln.SetShaderIO( EShader::Geometry, EShader::Fragment, "gs-fs.io" );

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
	#include "CubeMap.glsl"

	void Main ()
	{
		const uint		idx		= gl.VertexIndex;
		const float2	coord	= un_Geometry.positions[idx].xy;
		const int		face	= int(un_Geometry.positions[idx].z);

		if ( iProjInFS == 0 )
		{
			switch ( iProj )
			{
				case 1 :	Out.normal = CM_IdentitySC_Forward( coord, face );		break;
				case 2 :	Out.normal = CM_TangentialSC_Forward( coord, face );	break;
				case 3 :	Out.normal = CM_EverittSC_Forward( coord, face );		break;
				case 4 :	Out.normal = CM_5thPolySC_Forward( coord, face );		break;
				case 5 :	Out.normal = CM_COBE_SC_Forward( coord, face );			break;
				case 6 :	Out.normal = CM_ArvoSC_Forward( coord, face );			break;
			}
		}
		else
			Out.normal = float3( coord, face );

		gl.Position	= float4(coord, 0.0, 1.0);
		Out.face	= face;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_GEOM

	layout (triangles) in;
	layout (triangle_strip, max_vertices = 3) out;

	void Main ()
	{
		for (uint i = 0; i < 3; ++i)
		{
			gl.Position		= gl_in[i].gl_Position;
			gl.Layer		= In[0].face;
			Out.normal		= In[i].normal;
			gl.EmitVertex();
		}
		gl.EndPrimitive();
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "CubeMap.glsl"

	float3  SphereNormal (float2 coord, ECubeFace face)
	{
		switch ( iProj )
		{
			case 0 :	return CM_CubeSC_Forward( coord, face );
			case 1 :	return CM_IdentitySC_Forward( coord, face );
			case 2 :	return CM_TangentialSC_Forward( coord, face );
			case 3 :	return CM_EverittSC_Forward( coord, face );
			case 4 :	return CM_5thPolySC_Forward( coord, face );
			case 5 :	return CM_COBE_SC_Forward( coord, face );
			case 6 :	return CM_ArvoSC_Forward( coord, face );
		}
	}

	void Main ()
	{
		if ( iProjInFS == 0 )
			out_Color = float4(Normalize(In.normal), 1.0);
		else
			out_Color = float4(SphereNormal( In.normal.xy, ECubeFace(In.normal.z) ), 1.0);
	}

#endif
//-----------------------------------------------------------------------------
