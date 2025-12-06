// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Simple flight simulation with collision detection.
	Has 2 projection types:
		1. from projection matrix
		2. used distortion correction

	Map is ray marched (skyline shader from shadertoy).
	SDF used to calculate collision with camera (player).
	Camera rotation used to calculate lift force.
	To restart press '1' in 3D mode (press 'Esc' to switch between UI and 3D mode).
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
		RC<Image>			rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<FlightCamera>	camera	= FlightCamera();
		RC<Buffer>			cbuf	= Buffer();

		// setup camera
		{
			camera.ClipPlanes( 0.05f, 10.f );
			camera.FovY( 70.f );
			camera.RotationScale( 1.f, 1.f, 1.f );
			camera.Dimension( rt.Dimension() );
			camera.Position( float3( 0.f, 1.f, 0.f ));
		}

		{
			cbuf.Float(	"actualPos",		float3() );
			cbuf.Float(	"prevPos",			float3() );
			cbuf.Float(	"velocity",			float3() );
			cbuf.Float(	"airplaneWidth",	0.1f );
			cbuf.Float(	"liftScale",		0.5f );
			cbuf.Int(	"crashed",			0 );
		}

		// render loop
		{
			RC<ComputePass>		logic = ComputePass();
			logic.ArgInOut(	"un_CBuf",		cbuf );
			logic.Set(		camera );
			logic.Slider( "iGravity",	0,	1,	1 );
			logic.LocalSize( 1 );
			logic.DispatchGroups( 1 );
		}{
			RC<Postprocess>		draw = Postprocess( EPostprocess::Shadertoy );
			draw.ArgIn(	"un_CBuf",	cbuf );
			draw.Slider( "iProj",	0,		2 );
			draw.Slider( "iFov",	60.0,	250.0,	70.0 );
			draw.Set(	camera );
			draw.Output( rt );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "Geometry.glsl"

	// >>>> 3rd party code
	#include "Skyline.glsl"

	ND_ float  RayTrace (const Ray ray, const float maxDepth, const int maxIter)
	{
		float2			distAndMat;
		float			t			= ray.t;
		float3			pos			= float3(0.0);
		const float		smallVal	= 0.000625;
		const float3	rayVec		= ray.dir;

		for (int i = 0; i < maxIter; i++)
		{
			marchCount+=1.0;
			pos = (ray.origin + rayVec * t);
			distAndMat = DistanceToObject(pos);

			float walk = distAndMat.x;
			float dx = -fract(pos.x);
			if (rayVec.x > 0.0) dx = fract(-pos.x);
			float dz = -fract(pos.z);
			if (rayVec.z > 0.0) dz = fract(-pos.z);
			float nearestVoxel = min(fract(dx/rayVec.x), fract(dz/rayVec.z))+voxelPad;
			nearestVoxel = max(voxelPad, nearestVoxel);
			walk = min(walk, nearestVoxel);

			t += walk;
			if ((t > maxDepth) || (abs(distAndMat.x) < smallVal))
				break;
		}
		return t;
	}
	// <<<< 3rd party code

	// Lift force minus gravity force
	ND_ float  LiftMinusGravity ()
	{
		float3	v0	= (un_PerPass.camera.view * float4(-1.f, 0.f, 0.f, 0.f)).xyz;	v0.y = 0.f;
		float3	v1	= (un_PerPass.camera.view * float4( 1.f, 0.f, 0.f, 0.f)).xyz;	v1.y = 0.f;
		float3	v2	= (un_PerPass.camera.view * float4( 0.f, 0.f, 1.f, 0.f)).xyz;	v2.y = 0.f;
		float	lift = Triangle_Area( Triangle_Create( v0, v1, v2 ));	// area of triangle in XZ plane

		return Pow( Saturate( 1.f - lift ), 2.0 ) * un_PerPass.timeDelta * un_CBuf.liftScale;
	}

	ND_ bool  HasCollision (const float3 pos, const float radius)
	{
		const Ray	ray	= Ray_Perspective( un_PerPass.camera.invViewProj, pos, un_PerPass.camera.clipPlanes.x, float2(0.5) );
		const float	d	= RayTrace( ray, 5.0, 50 );
		return d < radius;
	}

	void  Main ()
	{
		const float3	delta		= un_PerPass.camera.pos - un_CBuf.prevPos;
		const float3	prev_pos	= un_CBuf.actualPos;
			  float3	pos			= prev_pos + delta;
		const float		ground		= 0.f;

		if ( iGravity == 1 )
			pos.y -= LiftMinusGravity();

		if ( pos.y < ground )
			pos.y = ground;

		if ( HasCollision( pos, un_CBuf.airplaneWidth ))
		{
			un_CBuf.crashed = 1;
		}

		un_CBuf.actualPos	= pos;
		un_CBuf.prevPos		= un_PerPass.camera.pos;

		// restart
		if ( un_PerPass.customKeys.x == 1.0 )
		{
			un_CBuf.crashed		= 0;
			un_CBuf.actualPos	= float3(0.f, 1.f, 0.f);
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	// 3rd party code:
	#include "Skyline.glsl"

	void mainVR (out vec4 fragColor, in vec2 fragCoord, in vec3 fragRayOri, in vec3 fragRayDir)
	{
		if ( un_CBuf.crashed != 0 ) {
			fragColor = float4(1.0, 0.0, 0.0, 1.0);
			return;
		}
		Ray	ray = Ray_Create( fragRayOri, fragRayDir, un_PerPass.camera.clipPlanes.x );
		fragColor = Trace( ray, fragCoord );
	}

	void mainImage (out vec4 fragColor, in vec2 fragCoord)
	{
		if ( un_CBuf.crashed != 0 ) {
			fragColor = float4(1.0, 0.0, 0.0, 1.0);
			return;
		}

		Ray	ray;
		switch ( iProj )
		{
			case 0 : {
				float	fov		= ToRad(Min( iFov, 140.0 ));
				float	ratio	= un_PerPass.resolution.x / un_PerPass.resolution.y;
				ray = Ray_Perspective( un_CBuf.actualPos, fov, ratio, un_PerPass.camera.clipPlanes.x, ToSNorm( fragCoord / iResolution.xy ));
				break;
			}

			case 1 : {
				float	fov = ToRad(Min( iFov, 179.0 ));
				ray = Ray_PaniniProjection( fov, un_CBuf.actualPos, un_PerPass.camera.clipPlanes.x, fragCoord, iResolution.xy );
				break;
			}

			case 2 : {
				float2	fov = float2(ToRad(iFov));
				fov.y *= un_PerPass.resolution.y / un_PerPass.resolution.x;
				ray = Ray_PlaneToSphere( fov, un_CBuf.actualPos, un_PerPass.camera.clipPlanes.x, ToSNorm( fragCoord / iResolution.xy ));
				break;
			}
		}

		Ray_Rotate( INOUT ray, MatTranspose(float3x3(un_PerPass.camera.view)) );

		fragColor = Trace( ray, fragCoord );
	}

#endif
//-----------------------------------------------------------------------------
