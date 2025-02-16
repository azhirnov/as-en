// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	define SH_COMPUTE
#	include <glsl.h>

#	define UNIFORM_CONTROL_FLOW
#	define MAXIMAL_RECONVERGENCE
#	define QUAD_CONTROL

#	define AE_subgroup_uniform_control_flow
#	define AE_maximal_reconvergence
#	define AE_shader_quad_control
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "UNIFORM_CONTROL_FLOW" );
			pass.ArgInOut( "un_OutImage", rt );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
		}{
			RC<ComputePass>		pass = ComputePass( "", "MAXIMAL_RECONVERGENCE" );
			pass.ArgInOut( "un_OutImage", rt );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
		}{
			RC<ComputePass>		pass = ComputePass( "", "QUAD_CONTROL" );
			pass.ArgInOut( "un_OutImage", rt );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef UNIFORM_CONTROL_FLOW
	#include "InvocationID.glsl"

	#ifdef AE_subgroup_uniform_qualifier

		void  Main ()
		{
			if ( gl::SubgroupUniform( gl.GlobalInvocationID.z == 0 ))
			{
				float2	uv = GetGlobalCoordUNormCorrected();

				float3	col = 0.5 + 0.5 * Cos( un_PerPass.time + uv.xyx + float3(0,2,4) );

				gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(col, 1.0) );
			}
		}

	#elif defined(AE_subgroup_uniform_control_flow)

		void  Main () [[subgroup_uniform_control_flow]]
		{
			if ( gl.GlobalInvocationID.z == 0 )
			{
				float2	uv = GetGlobalCoordUNormCorrected();

				float3	col = 0.5 + 0.5 * Cos( un_PerPass.time + uv.xyx + float3(0,2,4) );

				gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(col, 1.0) );
			}
		}

	#else
	#	error 'subgroup_uniform_qualifier' and 'subgroup_uniform_control_flow' are not supported
	#endif

#endif
//-----------------------------------------------------------------------------
#ifdef MAXIMAL_RECONVERGENCE
	#include "InvocationID.glsl"

	#ifdef AE_maximal_reconvergence

		void  Main () [[maximally_reconverges]]
		{
			if ( gl.GlobalInvocationID.z == 0 )
			{
				float2	uv = GetGlobalCoordUNormCorrected();

				float3	col = 0.5 + 0.5 * Cos( un_PerPass.time + uv.xyx + float3(0,2,4) );

				gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(col, 1.0) );
			}
		}

	#else
	#	error 'maximal_reconvergence' is not supported
	#endif

#endif
//-----------------------------------------------------------------------------
#ifdef QUAD_CONTROL
	#include "InvocationID.glsl"

	#ifdef AE_shader_quad_control

	//	layout (full_quads) in;		// fragment only
		layout (quad_derivatives) in;

		void  Main ()
		{
			if ( gl.quadGroup.All( gl.GlobalInvocationID.z == 0 ))
			{
			}

			if ( gl.quadGroup.Any( gl.GlobalInvocationID.z > 0 ))
			{
			}
		}

	#else
	#	error 'shader_quad_control' is not supported
	#endif

#endif
//-----------------------------------------------------------------------------
