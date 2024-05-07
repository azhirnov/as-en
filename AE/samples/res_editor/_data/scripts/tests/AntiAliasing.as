// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( EPostprocess::None );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
			pass.Output( "out_Color",	rt );
			pass.Slider( "iType",	0,				2,				0 );
			pass.Slider( "iScale",	1.0,			1000.0,			1.0 );
			pass.Slider( "iOffset",	float2(-1.0),	float2(1.0),	float2(0.0) );
			pass.Slider( "iTick",	1.0,			3.0,			1.5 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"

	void Main ()
	{
		switch ( iType )
		{
			// grid + offset
			case 0 :
			{
			/*	float	time	= Abs(Sin( un_PerPass.time*0.1 ));
				float2	pos;
				pos.x = RemapClamp( float2(0.0, 0.5), float2(0.0, 8.0), time );
				pos.y = RemapClamp( float2(0.5, 1.0), float2(0.0, 8.0), time );
				pos += Floor( gl.FragCoord.xy ) + iOffset * 8.0;*/

				float2	pos = Floor( gl.FragCoord.xy );
				pos.x += un_PerPass.time*0.5;

				out_Color = float4(AA_QuadGrid( pos, float2(1.0/100.0), iTick ));
				break;
			}

			// point
			case 1 :
			{
				float2	pos	= Floor( gl.FragCoord.xy ) + iOffset * 8.0;
				float2	p	= TriangleWave( pos*0.01 );

				out_Color = float4(1.0 - Dot( p, p ) * iScale);
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
