// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*

*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize()/4 );		rt.Name( "RT" );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );
			pass.Slider( "iMode",		0, 3 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "GlobalIndex.glsl"
	#include "Color.glsl"

	float4  QuadGroupId ()
	{
		const float4	colors [] = {
			float4( 1.0, 0.0, 0.0, 1.0 ),
			float4( 1.0, 1.0, 0.0, 1.0 ),
			float4( 0.0, 0.0, 1.0, 1.0 ),
			float4( 1.0, 0.0, 1.0, 1.0 )
		};
		return colors[ gl.subgroup.Index & 3 ];
	}


	float4  SubgroupId ()
	{
		return Rainbow( float(gl.subgroup.Index) / float(gl.subgroup.Size-1) );
	}


	void  Main ()
	{
		out_Color = float4(0.0);
		switch ( iMode )
		{
			case 0 :	out_Color = QuadGroupId();		break;
			case 1 :	out_Color = SubgroupId();		break;
		//	case 2 :	out_Color = SubgroupGroupId();	break;
		}
	}

#endif
//-----------------------------------------------------------------------------
