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
			RC<ShaderStructType>	st = ShaderStructType( "vs-fs.io" );
			st.Set( EStructLayout::InternalIO,
					"float3		texcoord;" +
					"float3		normal;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",	"UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex,	"un_Geometry",	"GeometrySBlock", EResourceState::ShaderStorage_Read );	// external
			ds.CombinedImage( EShaderStages::Fragment,	"un_CubeMap",	EImageType::Float_Cube,	 Sampler_LinearMipmapClamp );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl" );
			ppln.SetLayout( "pl" );
			ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
			ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "vs-fs.io" );

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
	#include "CubeMap.glsl"
	#include "Transform.glsl"

	void Main ()
	{
		const uint		idx		= gl.VertexIndex;
		const float2	coord	= un_Geometry.positions[idx].xy;
		const int		face	= int(un_Geometry.positions[idx].z);
		float3			uvw		= float3(0.0);

		switch ( iProj )
		{
			case 1 :	uvw = CM_IdentitySC_Forward( coord, face );		break;
			case 2 :	uvw = CM_TangentialSC_Forward( coord, face );	break;
			case 3 :	uvw = CM_EverittSC_Forward( coord, face );		break;
			case 4 :	uvw = CM_5thPolySC_Forward( coord, face );		break;
			case 5 :	uvw = CM_COBE_SC_Forward( coord, face );		break;
			case 6 :	uvw = CM_ArvoSC_Forward( coord, face );			break;
		}
		gl.Position		= LocalPosToClipSpace( uvw );
		Out.texcoord	= CM_IdentitySC_Forward( coord, face );
		Out.normal		= uvw;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "CodeTemplates.glsl"

	void Main ()
	{
		float3	norm1	= Normalize( gl.texture.SampleLod( un_CubeMap, In.texcoord, 0.0 ).rgb );
		float3	norm2	= Normalize( In.normal );
		float3	color	= float3(1.0);

		switch ( iCmp )
		{
			case 0 :
				out_Color = float4( norm1, 1.0 );
				break;

			case 1 :
				out_Color = float4( norm2, 1.0 );
				break;

			case 2 :
			{
				float	len = Length(norm1 - norm2) * iScale;
				out_Color = float2( len, 1.0 ).xxxy;

				if ( len >= 1.0 )
					out_Color = float4(1.0, 0.0, 0.0, 1.0);

				// wireframe
				{
					const float		thickness	= 1.0;	// pixels
					const float		falloff		= 1.0;	// pixels

					out_Color.rgb = Lerp( float3(0.0, 0.5, 1.0), out_Color.rgb, FSBarycentricWireframe( thickness, falloff ).x);
				}
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
