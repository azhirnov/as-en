// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	const string	prefix = "opaque";


	void  CreatePipe (bool dbgVS, bool dbgFS)
	{
		string	suffix = "";
		if ( dbgVS ) suffix += ".dbgVS";
		if ( dbgFS ) suffix += ".dbgFS";

		{
			RC<PipelineLayout>		pl = PipelineLayout( prefix+".pl"+suffix );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, prefix+".mtr.ds" );

			if ( dbgVS )	pl.AddDebugDSLayout( EShaderOpt::Trace, EShaderStages::Vertex );
			if ( dbgFS )	pl.AddDebugDSLayout( EShaderOpt::Trace, EShaderStages::Fragment );
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( prefix+".t"+suffix );
		ppln.SetLayout( prefix+".pl"+suffix );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, prefix+".io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			if ( dbgVS ) vs.options = EShaderOpt::Trace;
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			if ( dbgFS ) fs.options = EShaderOpt::Trace;
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( prefix+suffix );
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
			rs.depth.compareOp				= ECompareOp::GEqual;	// reverseZ

			rs.inputAssembly.topology		= EPrimitive::TriangleStrip;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}


	void  ASmain ()
	{
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
			ds.StorageBuffer( EShaderStages::Vertex, "un_LightObjs", "LightObject_Array",		EResourceState::ShaderStorage_Read );	// external
		}

		CreatePipe( false, false );		// default
		CreatePipe( true,  false );
		CreatePipe( false, true  );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Cone.glsl"
	#include "Matrix.glsl"
	#include "Frustum.glsl"
	#include "Transform.glsl"
	#include "CodeTemplates.glsl"

	void Main ()
	{
		LightObject		obj			= un_LightObjs.elements[ gl.InstanceIndex ];
		const float4	wpos		= LocalPosToWorldSpace( obj.position );

		Cone			cone		= Cone_Create( wpos.xyz, obj.dir, obj.angle, obj.height );	// world space
		bool			is_visible	= Frustum_IsVisible( Frustum_Create(un_PerPass.camera.frustum), cone );

		Cone_Rotate( INOUT cone, float3x3(un_PerPass.camera.view) );							// to view space

		const float3	vpos		= cone.origin;
		const Sphere	sp			= Cone_ToBoundingSphere( cone );							// view space
		const float2	zw			= FastProjectZW( un_PerPass.camera.proj, vpos.z - sp.radius );
		const float2	uv			= ProceduralQuadUV();
		Rect			aabb		= Sphere_FastProject( sp, un_PerPass.camera.proj[0][0], un_PerPass.camera.proj[1][1] );	// ndc

		if ( vpos.z - sp.radius < un_PerPass.camera.clipPlanes.x or NotAllFinite( aabb.v ) )
			aabb.v = float2(-1.0, 1.0).xxyy;	// fullscreen

		aabb.v = Clamp( aabb.v, -1.0, 1.0 );

		gl.Position		= float4( Rect_Lerp( aabb, uv ), zw );
		gl.Position.xy	*= zw[1];

		Out.color		= unpackUnorm4x8( obj.color ).rgb * obj.brightness;
		Out.lightPos	= wpos.xyz;			// world space
		Out.lightDir	= -obj.dir;			// world space
		Out.outerCos	= Cos( cone.halfAngle );
		Out.atten		= obj.atten;
		Out.range		= Cone_SlantLength( cone );

		if ( vpos.z + sp.radius < un_PerPass.camera.clipPlanes.x or !is_visible )
			gl.Position.x = float_nan;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "PBR.glsl"
	#include "Matrix.glsl"
	#include "CodeTemplates.glsl"

	void Main ()
	{
	#ifdef OVERDRAW
		out_Overdraw.r = 1.0 + float(HelperInvocationCountPerQuad()) / 4.0;
	#else

		float		depth		= gl.texture.Fetch( un_Depth, int2(gl.FragCoord.xy), 0 ).r;	// non-linear

		if ( depth < 0.0001 )
			gl.Discard;

		float3		albedo		= gl.texture.Fetch( un_Albedo, int2(gl.FragCoord.xy), 0 ).rgb;
		float3		norm		= ToSNorm( gl.texture.Fetch( un_Normal, int2(gl.FragCoord.xy), 0 ).rgb );	// world space

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

	#endif
	}

#endif
//-----------------------------------------------------------------------------
