// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  ASmain ()
	{
		const string prefix = "opaque";

		{
			RC<ShaderStructType>	st = ShaderStructType( prefix+".io" );
			st.Set( EStructLayout::InternalIO,
					"float3					lightPos;"
					"flat float				range;"
					"flat mediump float3	color;"
					"flat mediump float3	lightDir;"
					"flat mediump float		outerCos;"
					"flat mediump float2	atten;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( prefix+".mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex, "un_Geometry",  "GeometryData",			EResourceState::ShaderStorage_Read );	// external
			ds.StorageBuffer( EShaderStages::Vertex, "un_LightObjs", "LightObject_Array",		EResourceState::ShaderStorage_Read );	// external
		}{
			RC<PipelineLayout>		pl = PipelineLayout( prefix+".pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, prefix+".mtr.ds" );
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( prefix+".t" );
		ppln.SetLayout( prefix+".pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, prefix+".io" );

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
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;
			{
				RenderState_ColorBuffer		cb;
				cb.SrcBlendFactor( EBlendFactor::One );
				cb.DstBlendFactor( EBlendFactor::One );
				cb.BlendOp( EBlendOp::Add );
				rs.color.SetColorBuffer( 0, cb );
			}
			rs.depth.test					= true;
			rs.depth.write					= false;
			rs.depth.compareOp				= ECompareOp::GEqual;

			// 0 - sky
			rs.stencil.enabled				= true;
			rs.stencil.CompareOp( ECompareOp::NotEqual );
			rs.stencil.CompareMask( 0xFF );
			rs.stencil.Reference( 0 );
			rs.stencil.WriteMask( 0 );

			rs.stencil.StencilFailOp( EStencilOp::Keep );
			rs.stencil.DepthFailOp	( EStencilOp::Keep );
			rs.stencil.PassOp		( EStencilOp::Keep );

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Transform.glsl"
	#include "Quaternion.glsl"
	#include "Cone.glsl"

	void Main ()
	{
		LightObject		obj		= un_LightObjs.elements[ gl.InstanceIndex ];
		const Cone		cone	= Cone_Create( obj.position - un_PerPass.camera.pos, obj.dir, obj.angle, obj.height );
		const float		radius	= Cone_BaseRadius( cone ) * 1.1;
		const Quat		rot		= QFrom2Normals( float3(0.0, 1.0, 0.0), // origin direction
												 obj.dir );				// new direction

		const float3	vpos	= QMul( rot, (un_Geometry.positions[ gl.VertexIndex ] * float3(radius, radius, obj.height)).xzy );
		const float3	wpos	= cone.origin + vpos;

		gl.Position		= WorldPosToClipSpace( wpos );
		Out.color		= unpackUnorm4x8( obj.color ).rgb * obj.brightness;
		Out.lightPos	= cone.origin;
		Out.lightDir	= -cone.dir;		// world space
		Out.outerCos	= Cos( cone.halfAngle );
		Out.atten		= obj.atten;
		Out.range		= Cone_SlantLength( cone );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "PBR.glsl"
	#include "Matrix.glsl"

	void Main ()
	{
		float3		albedo		= gl.texture.Fetch( un_Albedo, int2(gl.FragCoord.xy), 0 ).rgb;
		float3		norm		= ToSNorm( gl.texture.Fetch( un_Normal, int2(gl.FragCoord.xy), 0 ).rgb );	// world space
		float		depth		= gl.texture.Fetch( un_Depth, int2(gl.FragCoord.xy), 0 ).r;					// non-linear

		#if 0
			// if not used stencil test
			if ( depth < 0.0001 )
				gl.Discard;
		#endif

		float2		uv			= gl.FragCoord.xy * un_PerPass.invResolution;
		float3		scene_wpos	= UnProjectNDC( un_PerPass.camera.invViewProj, float3( ToSNorm(uv), depth ));

		float3		dir_to_l	= Normalize( In.lightPos - scene_wpos );
		float		cos_theta	= Dot( dir_to_l, In.lightDir );
		float		dist		= Distance( In.lightPos, scene_wpos );

		if ( dist > In.range )
			gl.Discard;

		if ( cos_theta < In.outerCos )
			gl.Discard;

		float		atten		= SpotAttenuation( float3(1.0, In.atten), cos_theta, dist, float2(1.0, In.outerCos) );

		float		n_dot_l		= Max( Dot( norm, In.lightDir ), 0.0 );
		float3		view_dir	= ViewDir( un_PerPass.camera.invViewProj, uv );

		LightingResult	res		= CookTorrance( albedo, albedo, In.lightDir, view_dir, norm, 0.5, 0.8 );

		out_Color.rgb = (res.diffuse + res.specular) * In.color * atten;
		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
