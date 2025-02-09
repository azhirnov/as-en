// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw spherical cube with cubemap.
	Used tessellation with constant detail level.
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
			RC<ShaderStructType>	st = ShaderStructType( "io.vs-tcs" );
			st.Set(	EStructLayout::InternalIO,
					"float4		position;" +
					"float3		uv;" );
		}{
			RC<ShaderStructType>	st = ShaderStructType( "io.tcs-tes" );
			st.Set( EStructLayout::InternalIO,
					"float4		position;" +
					"float3		uv;" );
		}{
			RC<ShaderStructType>	st = ShaderStructType( "io.tes-fs" );
			st.Set( EStructLayout::InternalIO,
					"float		noise;" +
					"float3		uv;" +
					"float3		worldPos;" +
					"float3		normal;" );
		}

		{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.CombinedImage( EShaderStages::Fragment,			"un_Palette",	EImageType::Float_2D,	Sampler_LinearClamp );
			ds.CombinedImage( EShaderStages::Fragment,			"un_NormalMap",	EImageType::Float_Cube, Sampler_LinearRepeat );
			ds.CombinedImage( EShaderStages::TessEvaluation,	"un_HeightMap",	EImageType::Float_Cube, Sampler_LinearRepeat );
			ds.UniformBuffer( EShaderStages::TessEvaluation,	"un_PerObject",	"SphericalCubeMaterialUB" );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl" );
			ppln.SetLayout( "pl" );
			ppln.SetVertexInput( "VB{SphericalCubeVertex}" );
			ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
			ppln.SetShaderIO( EShader::Vertex,			EShader::TessControl,	 "io.vs-tcs" );
			ppln.SetShaderIO( EShader::TessControl,		EShader::TessEvaluation, "io.tcs-tes" );
			ppln.SetShaderIO( EShader::TessEvaluation,	EShader::Fragment,		 "io.tes-fs" );

			{
				RC<Shader>	vs = Shader();
				vs.LoadSelf();
				ppln.SetVertexShader( vs );
			}{
				RC<Shader>	tcs = Shader();
				tcs.LoadSelf();
				tcs.TessPatchSize( 3 );
				ppln.SetTessControlShader( tcs );
			}{
				RC<Shader>	tes = Shader();
				tes.LoadSelf();
				tes.TessPatchMode( ETessPatch::Triangles, ETessSpacing::Equal, /*ccw*/false );
				ppln.SetTessEvalShader( tes );
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

				rs.inputAssembly.topology		= EPrimitive::Patch;

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
		Out.position	= float4(in_Position.xyz, 1.0);	// tangential projection
		Out.uv			= in_Texcoord.xyz;				// identity projection
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_TESS_CTRL

# ifndef iTessLevel
#	define iTessLevel	1.f
# endif
# define I	gl.InvocationID

	void Main ()
	{
		if ( I == 0 ) {
			gl.TessLevelInner[0] = iTessLevel;
			gl.TessLevelOuter[0] = iTessLevel;
			gl.TessLevelOuter[1] = iTessLevel;
			gl.TessLevelOuter[2] = iTessLevel;
		}
		Out[I].position = In[I].position;
		Out[I].uv		= In[I].uv;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_TESS_EVAL
	#include "Transform.glsl"

	#define Interpolate( _arr_, _field_ )	InterpolateTriangle( _arr_, _field_, gl.TessCoord )

	void Main ()
	{
		float3	uv		= Interpolate( In, .uv );
		float	noise	= gl.texture.Sample( un_HeightMap, uv ).r;
		float4	pos		= Interpolate( In, .position );
		float3	surf_n	= Normalize( pos.xyz );

		Out.noise	 = noise;
		Out.uv		 = uv;
		pos.xyz		 = surf_n * (1.0 + noise * iHeight);
		Out.worldPos = LocalPosToWorldSpace( pos.xyz ).xyz;
		Out.normal	 = LocalVecToWorldSpace( pos.xyz );
		gl.Position	 = LocalPosToClipSpace( pos );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Color.glsl"
	#include "Normal.glsl"
	#include "ColorSpace.glsl"

	void Main ()
	{
	#if 1
		float3	norm		= Normalize( gl.texture.Sample( un_NormalMap, In.uv ).rgb );
	#else
	//	float3	norm		= ComputeNormalInWS_dxdy( In.worldPos );
	#endif

		float	lighting	= Max( 0.05, Dot( norm, iLightDir ));
		float3	surf_norm	= Normalize( In.normal );

	#if 1
		float	x			= ToUNorm( In.noise ) * 1.3 - 0.2;
				x			+= Max( 0.0, Dot( norm, surf_norm )) * 0.2;	// for mountains
		float3	color		= gl.texture.Sample( un_Palette, float2(x, 0.5) ).rgb;
	#else
		float3	color		= Rainbow( ToUNorm( -In.noise )).rgb;
	#endif
				color		= RemoveSRGBCurve( color );
				color		*= lighting;
				color		= ApplySRGBCurve( color );

		out_Color = float4( color, 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
