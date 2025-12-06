// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#	define SPHERE
#	define QUAD
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  CreatePipeline (string name)
	{
		RC<GraphicsPipeline>	ppln = GraphicsPipeline( name+".t" );
		ppln.SetLayout( "pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		string	defines;
		if ( name == "DrawQuad" )	defines = "QUAD";
		if ( name == "DrawSphere" ) defines = "SPHERE";

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			vs.Define( defines );
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			fs.Define( defines );
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( name );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			if ( name == "DrawSphere" ) {
				rs.depth.test				= true;
				rs.depth.write				= true;

				rs.inputAssembly.topology	= EPrimitive::TriangleList;
			}

			if ( name == "DrawQuad" ) {
				rs.inputAssembly.topology	= EPrimitive::TriangleStrip;

				RenderState_ColorBuffer		cb;
				cb.SrcBlendFactor( EBlendFactor::SrcAlpha,			EBlendFactor::One				);
				cb.DstBlendFactor( EBlendFactor::OneMinusSrcAlpha,	EBlendFactor::OneMinusSrcAlpha	);
				cb.BlendOp( EBlendOp::Add );
				rs.color.SetColorBuffer( 0, cb );
			}

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"mediump float4		color;" +
					"mediump float3		worldPos;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex, "un_Geometry",  "GeometryData",			EResourceState::ShaderStorage_Read );	// external
			ds.StorageBuffer( EShaderStages::Vertex, "un_Transform", "ObjectTransform_Array",	EResourceState::ShaderStorage_Read );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		CreatePipeline( "DrawSphere" );
		CreatePipeline( "DrawQuad" );
	}

#endif
//=============================================================================
#ifdef SPHERE
# ifdef SH_VERT
	#include "Math.glsl"

	void Main ()
	{
		ObjectTransform	obj	= un_Transform.elements[ gl.InstanceIndex ];
		float3			pos = un_Geometry.positions[ gl.VertexIndex ];

		pos *= obj.scale;
		pos += obj.position;
		pos -= un_PerPass.camera.pos;

		gl.Position		= un_PerPass.camera.viewProj * float4(pos, 1.0);
		Out.color		= unpackUnorm4x8( obj.color );
		Out.worldPos	= pos;
	}

# endif
//-----------------------------------------------------------------------------
# ifdef SH_FRAG
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "Normal.glsl"
	#include "CodeTemplates.glsl"

	void Main ()
	{
		float3	norm	= ComputeNormalInWS_dxdy( In.worldPos );
		float	ndl		= Clamp( Dot( norm, Normalize(iLight) ), 0.1, 1.0 );

		out_Color = In.color * ndl;
	}

# endif
#endif // SPHERE
//=============================================================================
#ifdef QUAD
# ifdef SH_VERT
	#include "Sphere.glsl"

	void Main ()
	{
		ObjectTransform	obj = un_Transform.elements[ gl.InstanceIndex ];
		float3	view_space	= (un_PerPass.camera.view * float4(obj.position - un_PerPass.camera.pos, 0.0)).xyz;
		float4	aabb;

		if ( view_space.z - obj.scale > un_PerPass.camera.clipPlanes.x )
		{
			aabb = Sphere_FastProject( Sphere_Create( view_space, obj.scale ), un_PerPass.camera.proj[0][0], un_PerPass.camera.proj[1][1] );

			// padding for AA border
			const float	padding = 2.0;

			aabb.xy -= padding * un_PerPass.invResolution.xy;
			aabb.zw += padding * un_PerPass.invResolution.xy;

			switch ( gl.VertexIndex ){
				case 0 :	gl.Position = float4( aabb.x, aabb.y, 0.0, 1.0 );	break;
				case 1 :	gl.Position = float4( aabb.x, aabb.w, 0.0, 1.0 );	break;
				case 2 :	gl.Position = float4( aabb.z, aabb.y, 0.0, 1.0 );	break;
				case 3 :	gl.Position = float4( aabb.z, aabb.w, 0.0, 1.0 );	break;
			}
		}else
			gl.Position = float4( float_nan );

		Out.color		= unpackUnorm4x8( obj.color );
		Out.worldPos	= float3( (gl.Position.xy - aabb.xy) / (aabb.zw - aabb.xy), 0.0 );
	}

# endif
//-----------------------------------------------------------------------------
# ifdef SH_FRAG
	#include "SDF.glsl"

	void Main ()
	{
		float2	uv = In.worldPos.xy;
		float	a  = AA_Rect_dxdy( uv, float2(0.5, 2.0) );

		out_Color = In.color;
		out_Color.a *= a;
	}

# endif
#endif // QUAD
//=============================================================================
