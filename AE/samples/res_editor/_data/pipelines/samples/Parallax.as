// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw geometry with parallax mapping.
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
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"float2		texcoord;" +
					"float3		worldPos;" +
					"float3		normal;" +
					"float3		tangent;" +
					"float3		bitangent;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",			"UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::AllGraphics,"un_Geometry",			"GeometrySBlock", EResourceState::ShaderStorage_Read );	// external
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

				rs.depth.test					= true;
				rs.depth.write					= true;

				rs.inputAssembly.topology		= EPrimitive::TriangleList;

				rs.rasterization.frontFaceCCW	= true;
				rs.rasterization.cullMode		= ECullMode::Back;

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
		const uint		idx	= gl.VertexIndex;
		const float4	pos	= LocalPosToWorldSpace( un_Geometry.positions[idx] );

		gl.Position		= WorldPosToClipSpace( pos );
		Out.texcoord	= un_Geometry.texcoords[idx];
		Out.worldPos	= pos.xyz;
		Out.normal		= LocalVecToWorldSpace( un_Geometry.normals[idx] );
		Out.tangent		= LocalVecToWorldSpace( un_Geometry.tangents[idx] );
		Out.bitangent	= LocalVecToWorldSpace( un_Geometry.bitangents[idx] );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Matrix.glsl"
	#include "Normal.glsl"
	#include "ReliefMapping.glsl"

	// sliders:
	#ifndef iCalcNormal
	# define iCalcNormal		0
	#endif
	#ifndef iScreenSpaceTBN
	# define iScreenSpaceTBN	0
	#endif
	#ifndef iShowNormals
	# define iShowNormals		0
	#endif
	#ifndef iMode
	# define iMode				3
	#endif
	#ifndef iHeightScale
	# define iHeightScale		0.1
	#endif

	#define ExtractNormal( _normalMap_, _uv_ )		(ToSNorm(gl.texture.Sample( _normalMap_, _uv_ ).yzw) * float3(1.f, -1.f, 1.f))

	#define GetPosition( _uv_, _off_ )\
		float3( _uv_ + float2(_off_) * inv_size, gl.texture.SampleLodOffset( un_HeightNormalMap, _uv_, lod, _off_ ).r * iHeightScale )


	ND_ float3  CalcNormal (const float2 uv)
	{
		if ( iCalcNormal == 0 )
		{
			return ExtractNormal( un_HeightNormalMap, uv );
		}
		else
		{
			float3	norm;
			float2	lod2		= gl.texture.QueryLod( un_HeightNormalMap, uv );
			float	lod			= Max( 0.0, Floor(Min( lod2.x, lod2.y )));
			float2	inv_size	= 1.0 / gl.texture.GetSize( un_HeightNormalMap, int(lod) );

			SmoothNormal3x3i( norm, GetPosition, uv );
			return norm;
		}
	}


	ND_ float3  ReliefMapping (const float2 uv, const float3 viewDir, const float3 worldPos, const float3 lightDir)
	{
		float3x3		TBN;
		if ( iScreenSpaceTBN == 0 ){
			TBN	= float3x3( Normalize( In.tangent ),
							Normalize( In.bitangent ),
							Normalize( In.normal ));
		}else{
			TBN = ComputeTBNinWS_dxdy( uv, worldPos );
		}
		const float3	tan_view_dir	= Normalize( MatTranspose(TBN) * viewDir );
			  float2	distorted_uv	= uv;
		const int		num_layers		= 64;

		switch ( iMode )
		{
			case 0 :	break; // normal mapping
			case 1 :	distorted_uv = ParallaxMapping( un_HeightNormalMap, uv, tan_view_dir, iHeightScale, -0.02f );				break;
			case 2 :	distorted_uv = SteepParallaxMapping( un_HeightNormalMap, uv, tan_view_dir, num_layers, iHeightScale );		break;
			case 3:		distorted_uv = ParallaxOcclusionMapping( un_HeightNormalMap, uv, tan_view_dir, num_layers, iHeightScale );	break;
		}

		const float3	normal			= Normalize( TBN * CalcNormal( distorted_uv ));
		const float3	tex_color		= gl.texture.Sample( un_ColorMap, distorted_uv ).rgb;
		const bool		left_src		= gl.FragCoord.x < un_PerPass.resolution.x*0.5;

		const float3	halfway_dir		= Normalize( lightDir + viewDir );
		const float3	ambient			= 0.2 * tex_color;
		const float3	diffuse			= Max( Dot(lightDir, normal), 0.f ) * tex_color;
		const float3	specular		= float3(0.2) * Pow( Max( Dot(normal, halfway_dir), 0.f ), 32.f );
		const float		shadow			= 1.0;
		const float3	color			= ambient + (diffuse * shadow) + (specular * shadow);

		return Lerp( color, (left_src ? normal : -normal), float(iShowNormals) );
	}


	void  Main ()
	{
		const float3	view_dir = ViewDir( un_PerPass.camera.invViewProj, gl.FragCoord.xy * un_PerPass.invResolution );

		out_Color = float4( ReliefMapping( In.texcoord, view_dir, In.worldPos, un_Geometry.lightDir ), 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
