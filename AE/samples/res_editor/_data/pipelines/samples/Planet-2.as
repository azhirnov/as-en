// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw spherical cube with cubemap.
	Used tessellation with constant detail level.
*/
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#	define USE_QUADS	0
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io.vs-tcs" );
			st.Set(	EStructLayout::InternalIO,
					"float4		position;" +
					"float3		texcoord;" );
		}{
			RC<ShaderStructType>	st = ShaderStructType( "io.tcs-tes" );
			st.Set( EStructLayout::InternalIO,
					"float4		position;" +
					"float3		texcoord;" );
		}{
			RC<ShaderStructType>	st = ShaderStructType( "io.tes-fs" );
			st.Set( EStructLayout::InternalIO,
					"float3		texcoord;" );
		}

		{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.CombinedImage( EShaderStages::TessEvaluation, "un_HeightMap",	EImageType::Float_Cube,	 Sampler_LinearMipmapRepeat );
			ds.CombinedImage( EShaderStages::Fragment,		 "un_NormalMap",	EImageType::Float_Cube,	 Sampler_LinearMipmapRepeat );
			ds.CombinedImage( EShaderStages::Fragment,		 "un_AlbedoMap",	EImageType::Float_Cube,	 Sampler_LinearMipmapRepeat );
			ds.CombinedImage( EShaderStages::Fragment,		 "un_EmissionMap",	EImageType::Float_Cube,	 Sampler_LinearMipmapRepeat );
			ds.UniformBuffer( EShaderStages::AllGraphics,	 "un_PerObject",	"SphericalCubeMaterialUB" );
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

			const bool	use_quads	= false;

			{
				RC<Shader>	vs = Shader();
				vs.LoadSelf();
				ppln.SetVertexShader( vs );
			}{
				RC<Shader>	tcs = Shader();
				tcs.LoadSelf();
				tcs.Define( use_quads ? "USE_QUADS=1" : "USE_QUADS=0" );
				tcs.TessPatchSize( use_quads ? 4 : 3 );
				ppln.SetTessControlShader( tcs );
			}{
				RC<Shader>	tes = Shader();
				tes.LoadSelf();
				tes.Define( use_quads ? "USE_QUADS=1" : "USE_QUADS=0" );
				tes.TessPatchMode( (use_quads ? ETessPatch::Quads : ETessPatch::Triangles), ETessSpacing::Equal, /*ccw*/false );
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
		Out.position	= float4(in_Position.xyz, 1.0);
		Out.texcoord	= in_Texcoord.xyz;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_TESS_CTRL

# ifndef iTessLevel
#	define iTessLevel	1.f
# endif
# define I	gl.InvocationID

# if USE_QUADS

	void Main ()
	{
		if ( I == 0 ) {
			gl.TessLevelInner[0] = iTessLevel;
			gl.TessLevelInner[1] = iTessLevel;
			gl.TessLevelOuter[0] = iTessLevel;
			gl.TessLevelOuter[1] = iTessLevel;
			gl.TessLevelOuter[2] = iTessLevel;
			gl.TessLevelOuter[3] = iTessLevel;
		}
		Out[I].position = In[I].position;
		Out[I].texcoord = In[I].texcoord;
	}

# else

	void Main ()
	{
		if ( I == 0 ) {
			gl.TessLevelInner[0] = iTessLevel;
			gl.TessLevelOuter[0] = iTessLevel;
			gl.TessLevelOuter[1] = iTessLevel;
			gl.TessLevelOuter[2] = iTessLevel;
		}
		Out[I].position = In[I].position;
		Out[I].texcoord = In[I].texcoord;
	}

# endif // not USE_QUADS
#endif
//-----------------------------------------------------------------------------
#ifdef SH_TESS_EVAL
	#include "Transform.glsl"

	#if USE_QUADS
	#	define Interpolate( _arr_, _field_ )	InterpolateQuad( _arr_, _field_, gl.TessCoord )
	#else
	#	define Interpolate( _arr_, _field_ )	InterpolateTriangle( _arr_, _field_, gl.TessCoord )
	#endif

	void Main ()
	{
		float3	texc	= Interpolate( In, .texcoord );
		float	height	= gl.texture.Sample( un_HeightMap, texc ).r;
		float4	pos		= Interpolate( In, .position );
		float3	surf_n	= Normalize( pos.xyz );

		Out.texcoord = texc;
		pos.xyz		 = surf_n * (1.0 + height);
		gl.Position	 = LocalPosToClipSpace( pos );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void Main ()
	{
		const float3	light_dir	= float3( 0.f, 0.f, 1.f );
		const float3	norm		= Normalize( gl.texture.Sample( un_NormalMap, In.texcoord ).xyz );
		const float3	color		= gl.texture.Sample( un_AlbedoMap, In.texcoord ).rgb;
		const float		light		= Dot( norm, light_dir );

		out_Color = float4( color * light, 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
