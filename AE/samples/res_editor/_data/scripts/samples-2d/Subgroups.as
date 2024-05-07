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
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
			pass.Output( "out_Color",	rt );
			pass.Slider( "iMode",		0, 3,	1 );
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

#ifdef AE_NV_shader_sm_builtins
	float4  SMID ()
	{
		return Rainbow( float(gl.NV.SMID) / gl.NV.SMCount );
	}

	float4  WarpID ()
	{
		return Rainbow( float(gl.NV.WarpID) / gl.NV.WarpsPerSM );
	}

#else
	float4  SMID ()		{ return float4(0.0); }
	float4  WarpID ()	{ return float4(0.0); }
#endif


	void  Main ()
	{
		out_Color = float4(0.0);
		switch ( iMode )
		{
			case 0 :	out_Color = QuadGroupId();		break;
			case 1 :	out_Color = SubgroupId();		break;
			case 2 :	out_Color = SMID();				break;
			case 3 :	out_Color = WarpID();			break;
		}
	}

#endif
//-----------------------------------------------------------------------------
