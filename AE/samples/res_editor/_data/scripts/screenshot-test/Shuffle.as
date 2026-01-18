// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	for screenshot test
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
		RC<Image>		rt	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );
			pass.Slider( "iSeed",		0,	32 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Hash.glsl"
	#include "InvocationID.glsl"


	void  Main ()
	{
		float4		col		= float4(0.2);
		const float	x_max	= 2.0;
		const float	y_max	= 32.0;
		const int2	pos		= int2(GetGlobalCoordUNorm().xy * float2(x_max,y_max));
		const uint	seed	= ToBitMask( iSeed );

		const uint	src		= ToBitMask( pos.y ) | 1;
		const uint	src_bc	= BitCount( src );
		uint		dst		= 0;

		switch ( pos.x )
		{
			case 0 :	dst = ShuffleBits( src, seed );  break;
			case 1 :	dst = FastShuffleBits( src, seed );  break;
		}

		uint	dst_bc	= BitCount( dst );

		if ( src == dst )
			col = float4(1.0);
		else
		if ( src_bc != dst_bc )
			col = float4(1.0, 0.0, 0.0, 1.0);

		if ( AllNotEqual( pos, int2(GetGlobalCoordUNorm().xy * float2(x_max,y_max)) ))
		{
			out_Color = float4(0.0);
			return;
		}

		out_Color = col;
	}

#endif
//-----------------------------------------------------------------------------
