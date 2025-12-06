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
			RC<ShaderStructType>	st = ShaderStructType( "gs-fs.io" );
			st.Set( EStructLayout::InternalIO,
					"float		area;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Geometry,		"un_PerObject",	"UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::AllGraphics,	"un_Geometry",	"GeometrySBlock", EResourceState::ShaderStorage_Read );	// external
			ds.CombinedImage( EShaderStages::Fragment,		"un_CubeMap",	EImageType::Float_Cube,	 Sampler_LinearMipmapClamp );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl" );
			ppln.SetLayout( "pl" );
			ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
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
	#include "CubeMap.glsl"

	void Main ()
	{
		const uint		idx		= gl.VertexIndex;
		const float2	coord	= un_Geometry.positions[idx].xy;
		const int		face	= int(un_Geometry.positions[idx].z);
		float3			uvw		= float3(0.0);

		switch ( iProj )
		{
			case 0 :	uvw = CM_CubeSC_Forward( coord, face );			break;
			case 1 :	uvw = CM_IdentitySC_Forward( coord, face );		break;
			case 2 :	uvw = CM_TangentialSC_Forward( coord, face );	break;
			case 3 :	uvw = CM_EverittSC_Forward( coord, face );		break;
			case 4 :	uvw = CM_5thPolySC_Forward( coord, face );		break;
			case 5 :	uvw = CM_COBE_SC_Forward( coord, face );		break;
			case 6 :	uvw = CM_ArvoSC_Forward( coord, face );			break;
		}
		gl.Position = float4(uvw, 1.0);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_GEOM
	#include "Transform.glsl"
	#include "Geometry3D.glsl"

	layout (triangles) in;
	layout (triangle_strip, max_vertices = 3) out;

	#define P0		gl_in[0].gl_Position.xyz
	#define P1		gl_in[1].gl_Position.xyz
	#define P2		gl_in[2].gl_Position.xyz


	void Main ()
	{
		float	area = Triangle_Area( Triangle_Create( P0, P1, P2 ));

		for (uint i = 0; i < 3; ++i)
		{
			gl.Position	= LocalPosToClipSpace( gl_in[i].gl_Position );
			Out.area	= area;
			gl.EmitVertex();
		}
		gl.EndPrimitive();
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "CodeTemplates.glsl"
	#include "Color.glsl"
	#include "SDF.glsl"
	#include "InvocationID.glsl"

	int3  GetGlobalSize () {
		return int3(un_PerPass.resolution.xy, 0);
	}

	void Main ()
	{
		// triangle area as color
		{
			float	x = In.area * iAreaScale;
			x = x*x;

			out_Color = Rainbow( 1.0 - Saturate(x) );
		}

		// debug output
		out_Color.a = In.area * iAreaScale;

		// wireframe
		{
			const float		thickness	= 1.0;	// pixels
			const float		falloff		= 1.0;	// pixels

			out_Color.rgb *= FSBarycentricWireframe( thickness, falloff ).x;
		}

		// screen space quad/circle
		{
			float2	uv = GetGlobalCoordSNormCorrected2();
		  #if 0
			float	d  = SDF2_Rect( uv, float2(0.23 * iRadius) );
					d  = SDF_OpSub( d, SDF2_Rect( uv, float2(0.2 * iRadius) ));
		  #else
			float	d  = SDF2_Circle( uv, 0.23 * iRadius );
					d  = SDF_OpSub( d, SDF2_Circle( uv, 0.2 * iRadius ));
		  #endif

			float	a  = SmoothStep( -d*4.0/iRadius, 0.02, 0.025 );
			float	b  = SmoothStep( -d*2.5/iRadius, 0.02, 0.025 );

			out_Color.rgb *= (1.0 - a);
			out_Color.rgb = Lerp( out_Color.rgb, float3(0.8, 0.0, 1.0), b );
		}
	}

#endif
//-----------------------------------------------------------------------------
