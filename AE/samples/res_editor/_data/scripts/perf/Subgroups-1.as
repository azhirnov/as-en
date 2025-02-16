// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize()/4 );		rt.Name( "RT" );
		RC<DynamicUInt>	cs		= DynamicUInt();
		RC<DynamicUInt>	mode	= DynamicUInt();
		RC<DynamicUInt>	fix_id	= DynamicUInt();

		Slider( cs,		"CS",		0,	1 );
		Slider( mode,	"Mode",		0,	4,	1 );
		Slider( fix_id,	"FixOrder",	0,	1 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.Constant( "iMode",		mode );
			pass.EnableIfEqual( cs, 0 );
		}{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgOut( "un_Image",	rt );
			pass.Constant( "iMode",		mode );
			pass.Constant( "iFixOrder",	fix_id );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
			pass.EnableIfEqual( cs, 1 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(SH_FRAG) or defined(SH_COMPUTE)
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"

	float4  QuadGroupId ()
	{
		const float4	colors [] = {
			float4( 1.0, 0.0, 0.0, 1.0 ),	// red			(0,0)
			float4( 1.0, 1.0, 0.0, 1.0 ),	// yellow		(1,0)
			float4( 0.0, 0.0, 1.0, 1.0 ),	// blue			(0,1)
			float4( 1.0, 0.0, 1.0, 1.0 )	// pink			(1,1)
		};
		return colors[ gl.subgroup.Index & 3 ];
	}

	float4  SubgroupId ()
	{
		return Rainbow( float(gl.subgroup.Index) / float(gl.subgroup.Size-1) );
	}

	float4  UniqueSubgroup ()
	{
		float2	sum = gl.subgroup.Add( GetGlobalCoordUNorm().xy );
		return Rainbow( DHash12( sum ));
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

  #elif defined(AE_ARM_shader_core_builtins)
	float4  SMID ()
	{
		return Rainbow( float(gl.ARM.CoreID) / gl.ARM.CoreMaxID );
	}

	float4  WarpID ()
	{
		return Rainbow( float(gl.ARM.WarpID) / gl.ARM.WarpMaxID );
	}

  #else
	float4  SMID ()		{ return float4(0.0); }
	float4  WarpID ()	{ return float4(0.0); }
  #endif

	float4  GetColor ()
	{
		float4	col = float4(0.0);
		switch ( iMode )
		{
			case 0 :	col = QuadGroupId();	break;
			case 1 :	col = SubgroupId();		break;
			case 2 :	col = UniqueSubgroup();	break;
			case 3 :	col = SMID();			break;
			case 4 :	col = WarpID();			break;
		}
		return col;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG

	void  Main ()
	{
		out_Color = GetColor();
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE

	void  Main ()
	{
		int2	pos;

		if ( iFixOrder == 1 )
		{
			// same subgroup coords as in FS on NVidia
			pos	= GetGlobalCoordQuadCorrected().xy;
		}
		else
		{
			pos = GetGlobalCoord().xy;
		}

		gl.image.Store( un_Image, pos, GetColor() );
	}

#endif
//-----------------------------------------------------------------------------
