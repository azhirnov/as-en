// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Use SDF to draw scene and calculate collisions.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define CALC_COLLISION
#	define TRACE_RAYS
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<FPSCamera>	camera	= FPSCamera();
		RC<Buffer>		cam_pos	= Buffer();

		cam_pos.Float(	"actualPos",	float3() );
		cam_pos.Float(	"prevPos",		float3() );

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s*2.f, s );
			camera.UpDownScale( s*6.f, s );
			camera.SideMovementScale( s );

			camera.Dimension( rt.Dimension() );
		}

		// render loop
		{
			RC<ComputePass>		collision	= ComputePass( "", "CALC_COLLISION" );
			collision.Set(		camera );
			collision.ArgInOut(	"un_CollisionData",	cam_pos );
			collision.LocalSize( 1 );
			collision.DispatchGroups( 1 );
		}{
			RC<Postprocess>		draw		= Postprocess( "", "TRACE_RAYS" );
			draw.Set(	 camera );
			draw.ArgIn(	 "un_CollisionData",	cam_pos );
			draw.Output( "out_Color",			rt );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(SH_COMPUTE) or defined(SH_FRAG)
	#include "SDF.glsl"
	#include "DistAndMtr.glsl"

	const float	GroundY			= 1.f;
	const float	CameraHeight	= 0.75f;
	const float	CameraRadius	= 0.125f;
	const float	Gravity			= 1.f;

	const int	MTR_Ground		= 0;
	const int	MTR_Column		= 1;
	const int	MTR_Sky			= -1;

	ND_ DistAndMtr  SDFGround (const float3 pos)
	{
		DistAndMtr	dm = DM_Create();
		dm.mtrIndex = MTR_Ground;
		dm.dist		= SDF_Plane( pos, float3(0.0, -GroundY, 0.0), 0.5 );
		return dm;
	}

	ND_ DistAndMtr  SDFColumns2 (const float3 pos)
	{
		DistAndMtr	dm = DM_Create();
		dm.mtrIndex = MTR_Column;
		dm.dist		= SDF_Cylinder( pos, float2(0.2, 1.0) );
		return dm;
	}

	ND_ DistAndMtr  SDFColumns (float3 pos)
	{
		pos = SDF_Move( pos, float3(-0.5f, 0.f, -0.5f));
		return SDF_Repetition( pos, 2.f, float3(100.f, 1.f, 100.f), SDFColumns2 );
	}

	ND_ DistAndMtr  SDFScene (const float3 pos)
	{
		DistAndMtr	dm = DM_Create( 99.0, MTR_Sky );
		dm = DM_Unite( dm, SDFColumns( pos ));
		dm = DM_Unite( dm, SDFGround( pos ));
		return dm;
	}

	ND_ float  SDFScene2 (const float3 pos) {
		return SDFScene( pos ).dist;
	}

	GEN_SDF_NORMAL_6sp_FN( SDFNormal, SDFScene2 )

#endif
//-----------------------------------------------------------------------------
#ifdef CALC_COLLISION
	#include "Geometry.glsl"

	void  Main ()
	{
		const float3	delta		= un_PerPass.camera.pos - un_CollisionData.prevPos;
		const float3	prev_pos	= un_CollisionData.actualPos;
			  float3	pos			= prev_pos + delta;
		const float		ground		= GroundY - CameraHeight;
		const float		dist		= SDFScene( pos ).dist;

		if ( dist < CameraRadius )
		{
			const float3	left	= prev_pos + LeftVectorXZ( delta );
			const float3	right	= prev_pos + RightVectorXZ( delta );
			const float		l_dist	= SDFScene( left ).dist;
			const float		r_dist	= SDFScene( right ).dist;

			pos = prev_pos;

			if ( l_dist > r_dist ){
				if ( l_dist >= CameraRadius )	pos = left;
			}else{
				if ( r_dist >= CameraRadius )	pos = right;
			}
		}

		pos.y += Gravity * un_PerPass.timeDelta;

		if ( pos.y > ground )
			pos.y = ground;

		un_CollisionData.actualPos	= pos;
		un_CollisionData.prevPos	= un_PerPass.camera.pos;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef TRACE_RAYS
	#include "InvocationID.glsl"
	#include "Ray.glsl"
	#include "Fog.glsl"

	void  Main ()
	{
		Ray			ray			= Ray_Perspective( un_PerPass.camera.invViewProj, un_CollisionData.actualPos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );

		const uint	max_iter	= 256;
		const float	min_dist	= 0.00625;
		const float	max_dist	= 100.0;
		int			mtr_index	= -1;

		// ray marching
		for (uint i = 0; i < max_iter; ++i)
		{
			DistAndMtr	dm = SDFScene( ray.pos );

			mtr_index = dm.mtrIndex;

			Ray_Move( INOUT ray, dm.dist );

			if ( Abs(dm.dist) < min_dist )
				break;

			if ( ray.t > max_dist )
			{
				mtr_index = MTR_Sky;
				break;
			}
		}

		const float3	light_dir	= Normalize( float3( 0.f, -1.0f, -0.7f ));
		const float3	normal		= SDFNormal( ray.pos );
		const float		light		= Saturate( Dot( normal, light_dir )) + 0.25f;
		const float		fog			= FogFactorExp( ray.t / max_dist, 16.0f );
		const float3	fog_color	= float3( 0.4f, 0.4f, 0.5f );

		float3	color;
		switch ( mtr_index )
		{
			case MTR_Ground :	color = Lerp( float3( 0.2f, 0.2f, 0.2f ) * light, fog_color, fog );	break;
			case MTR_Column :	color = Lerp( float3( 1.0f, 0.0f, 0.0f ) * light, fog_color, fog );	break;
			case MTR_Sky :
			default :			color = float3( 0.0f, 1.0f, 1.0f );			break;
		}

		out_Color = float4(color, 1.f);
	}

#endif
//-----------------------------------------------------------------------------

