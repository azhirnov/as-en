// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#	define TREE
#	define GROUND
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  TreePipe ()
	{
		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tree" );
		ppln.SetLayout( "pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			vs.Define( "TREE" );
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "Tree" );
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


	void  GroundPipe ()
	{
		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "ground" );
		ppln.SetLayout( "pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			vs.Define( "GROUND" );
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "Ground" );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			rs.depth.test					= true;
			rs.depth.write					= true;

			rs.inputAssembly.topology		= EPrimitive::TriangleStrip;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::None;

			spec.SetRenderState( rs );
		}
	}


	void  ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"float3		uv;"
					"float3		normal;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex, "un_Geometry",  "Vertex_Array", EResourceState::ShaderStorage_Read );		// external
			ds.CombinedImage( EShaderStages::Fragment, "un_Texture", EImageType::Float_2DArray, Sampler_LinearMipmapClamp );	// external
			ds.CombinedImage( EShaderStages::Fragment, "un_GroundTex", EImageType::Float_2D, Sampler_LinearMipmapRepeat );		// external
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		TreePipe();
		GroundPipe();
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Hash.glsl"
	#include "Transform.glsl"
	#include "CodeTemplates.glsl"

	void Main ()
	{
	#ifdef TREE
		const uint	idx		= gl.VertexIndex;
		Vertex		vtx		= un_Geometry.elements[idx];
		float2		seed	= (float2( gl.InstanceIndex % iGridSize, gl.InstanceIndex / iGridSize ) - (iGridSize * 0.5)) * 0.35;
		float		scale	= 0.5 + DHash12( seed * 222.0 ) * 0.5;
		float3		pos		= LocalPosToWorldSpace( vtx.inPos * scale ).xyz;

		pos.xz += seed + ToSNorm( DHash22( seed * 111.0 ));

		gl.Position		= WorldPosToClipSpace( pos );
		Out.uv			= float3( vtx.inUV, vtx.instanceTexIndex );
		Out.normal		= vtx.inNormal;
	#endif

	#ifdef GROUND
		int2	ipos	= GenGridWithInstancingTriStrip();
		float3	vpos	= float3( ToSNorm(float2(ipos) / 1.0), 0.0 ).xzy * 100.0;

		gl.Position		= LocalPosToClipSpace( vpos );
		Out.uv			= float3( vpos.xz, -1 );
		Out.normal		= float3(0.0, -1.0, 0.0);
	#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void  Main ()
	{
		const float3	light_dir	= Normalize(float3( 0.f, -1.f, 0.4f ));
		float3			norm		= Normalize( In.normal );
		float			n_dot_l		= Max( Dot( light_dir, norm ), 0.3f );

		if ( In.uv.z < 0 )
			out_Color = gl.texture.Sample( un_GroundTex, In.uv.xy );
		else
			out_Color = gl.texture.Sample( un_Texture, In.uv );

		if ( out_Color.a < 0.5 )
			gl.Discard;

		out_Color.rgb *= n_dot_l;
	}

#endif
//-----------------------------------------------------------------------------
