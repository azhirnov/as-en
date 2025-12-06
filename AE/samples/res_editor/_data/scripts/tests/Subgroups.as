// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Visualize difference between subgroup operations:
		Add / InclusiveAdd / ExclusiveAdd
		Ballot
		ClusteredAdd
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize()/16 );		rt.Name( "RT" );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgOut( "un_OutImage", rt );
			pass.LocalSize( 32 );
			pass.DispatchThreads( rt.Dimension() );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "Color.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2		uv		= GetGlobalCoordUNorm().xy;
		float		x		= float(gl.subgroup.Index + 1) / (gl.subgroup.Size * gl.subgroup.Size / 2);
		float		y		= 0.0;
		const float	y_max	= 5.0;
		int			i		= int(uv.y * y_max);

		switch ( i )
		{
		  #ifdef AE_shader_subgroup_arithmetic
			case 0 :	y = gl.subgroup.Add( x );			break;
			case 1 :	y = gl.subgroup.InclusiveAdd( x );	break;
			case 2 :	y = gl.subgroup.ExclusiveAdd( x );	break;
		  #endif
		  #ifdef AE_shader_subgroup_ballot
			case 3 :	y = HasBit( gl.subgroup.Ballot( (gl.subgroup.Index & 1) == 1 ).x, gl.subgroup.Index ) ? 0.1 : 0.5;	break;
		  #endif
		  #ifdef AE_shader_subgroup_clustered
			case 4 :	y = gl.subgroup.ClusteredAdd( x, 4 ) * 4.0;	break;
		  #endif
		}

		float4	c = Rainbow( y );
		c.a = y;

		if ( gl.subgroup.Index+1 == gl.subgroup.Size or
			 i != int(GetGlobalCoordUNorm(int3(1)).y * y_max) )
		{
			c = float4(0.0);
		}

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, c );
	}

#endif
//-----------------------------------------------------------------------------
