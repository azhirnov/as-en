// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Simulate particles in gravimagnetic field and draw them using geometry shader.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>			rt					= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>			ds					= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );

		RC<Scene>			scene				= Scene();
		RC<Buffer>			particles			= Buffer();
		const bool			use_rays			= true;
		const uint			max_particle_count	= 1000 * 1000;
		RC<DynamicUInt>		particle_count		= DynamicUInt();

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );

			const float	s = 0.3f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
		}

		// setup particles
		{
			particles.ArrayLayout(
				"Particle",
				"	float4	position_size;" +
				"	float4	velocity_color;" +
				"	float2	fParams;" +
				"	uint2	iParams;",
				max_particle_count );

			Slider( particle_count, "Particle count", max_particle_count/100, max_particle_count );
		}

		// how to draw geometry
		{
			UnifiedGeometry_Draw	cmd;
			cmd.VertexCount( particle_count );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();

			geometry.Draw( cmd );
			geometry.ArgIn( "un_Particles", particles );

			scene.Add( geometry, float3( 0.f, 0.f, 5.f ));
		}

		// render loop
		{
			RC<ComputePass>			sim_pass = ComputePass();
			sim_pass.ArgInOut( "un_Particles", particles );
			sim_pass.LocalSize( 64 );
			sim_pass.DispatchThreads( particle_count );
			sim_pass.Slider( "iSteps",		1,		10 );
			sim_pass.Slider( "iTimeScale",	1.f,	4.f );
		}{
			RC<SceneGraphicsPass>	draw_pass = scene.AddGraphicsPass( "draw" );
			draw_pass.AddPipeline( use_rays ? "particles/Rays.as" :		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/particles/Rays.as)
											  "particles/Dots.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/particles/Dots.as)
			draw_pass.Output( "out_Color", rt, RGBA32f(0.0) );
			draw_pass.Output( ds, DepthStencil(1.f, 0) );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "Particles-Simulation.glsl"


	const GravityObject		g_GravityObjects[1] = {
		{ float3( 0.0 ), 0.1, 0.1 }
	};

	const MagneticObject	g_MagneticObjects[1] = {
		{ float3( 0.0, 0.0, 0.1 ), float3( 0.0, 0.0, -0.1 ), 0.5 }
	};

	const AABB				g_BoundingBox	= { float3(-10.0), float3(10.0) };


	void  RestartParticle (out Particle outParticle, const float globalTime)
	{
		float	index	= float(GetGlobalIndex());
		float	size	= float(GetGlobalIndexSize());
		float	vel		= 0.5;

		outParticle.position_size.xyz	= AABB_SNormToGlobal( g_BoundingBox, float3( 0.1, 0.0, 0.0 ));
		outParticle.position_size.w		= 8.0;
		outParticle.velocity_color.xyz	= ParticleEmitter_ConeVector( index, size, 1.0 ).zxy * -vel;
		outParticle.velocity_color.w	= uintBitsToFloat( 0xFFFFFFFF );
		outParticle.fParams.x			= Sign(ToSNorm( DHash12(float2( globalTime, index / size )) ));	// sign
		outParticle.iParams.x			= 1;
	}


	void  UpdateParticle (inout Particle outParticle, const float stepTime, const uint steps, const float globalTime)
	{
		for (uint t = 0; t < steps; ++t)
		{
			float3	accel		= float3(0.0);
			int		destroyed	= 0;
			float	sign		= outParticle.fParams.x;
			bool	restart		= outParticle.iParams.x == 0;

			for (int i = 0; i < g_GravityObjects.length(); ++i)
			{
				accel		+= GravityAccel( outParticle.position_size.xyz, g_GravityObjects[i].position, g_GravityObjects[i].gravity );
				destroyed	+= int( Distance( outParticle.position_size.xyz, g_GravityObjects[i].position ) < g_GravityObjects[i].radius );
			}

			for (int i = 0; i < g_MagneticObjects.length(); ++i)
			{
				accel += SphericalMagneticFieldAccel( outParticle.velocity_color.xyz, outParticle.position_size.xyz,
													  g_MagneticObjects[i].north, g_MagneticObjects[i].south,
													  g_MagneticObjects[i].induction ) * sign;
			}

			UniformlyAcceleratedMotion( INOUT outParticle.position_size.xyz, INOUT outParticle.velocity_color.xyz, accel, stepTime );

			if ( ! AABB_IsInside( g_BoundingBox, outParticle.position_size.xyz ) or destroyed > 0 )
			{
				RestartParticle( OUT outParticle, globalTime );
			}
		}

		outParticle.velocity_color.w = uintBitsToFloat( ParticleColor_FromVelocityLength( outParticle.velocity_color.xyz ));
	}


	void Main ()
	{
		UpdateParticle( un_Particles.elements[GetGlobalIndex()], 0.004 * iTimeScale, iSteps, un_PerPass.time );
	}

#endif
//-----------------------------------------------------------------------------
