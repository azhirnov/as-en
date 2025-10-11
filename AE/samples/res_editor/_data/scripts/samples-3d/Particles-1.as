// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Simulate particles in gravimagnetic field and draw them using geometry shader.

	'Dots-tl' a bit slower than 'Dots-i' on mobile.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	const uint	Mode_GS			= 0;	// Supports_GeometryShader()
	const uint	Mode_Instancing	= 1;
	const uint	Mode_TriList	= 2;
//	const uint	Mode_MS			= 3;	// Supports_MeshShader()
	const uint	Mode_Count		= 3;


	void ASmain ()
	{
		RC<Image>			rt					= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );

		array<RC<Scene>>	scenes;
		RC<Buffer>			particles			= Buffer();
		const uint			max_particle_count	= IsDiscreteGPU() ? (1000 * 1000) : (100 * 1000);
		RC<DynamicUInt>		particle_count		= DynamicUInt();
		RC<DynamicUInt>		draw_mode			= DynamicUInt();
		RC<DynamicUInt>		sim_steps			= DynamicUInt();
		RC<DynamicFloat>	particle_size		= DynamicFloat();

		Slider( draw_mode,		"DrawMode",		0,		Mode_Count*2-1,	Mode_Instancing );
		Slider( sim_steps,		"Steps",		0,		10,				1 );
		Slider( particle_size,	"Size",			0.5f,	4.f,			1.f );

		for (uint i = 0; i < Mode_Count*2; ++i)
			scenes.push_back( Scene() );

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );
			camera.Position( float3( 0.f, 0.f, -5.f ));

			const float	s = 0.3f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			for (uint i = 0; i < scenes.size(); ++i)
				scenes[i].Set( camera );
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

			Slider( particle_count, "Particle count", max_particle_count/100, max_particle_count, max_particle_count/50 );
		}

		// how to draw geometry
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			UnifiedGeometry_Draw	cmd;
			cmd.VertexCount( particle_count );
			geometry.Draw( cmd );
			geometry.ArgIn( "un_Particles", particles );
			scenes[ Mode_GS ].Add( geometry );
			scenes[ Mode_GS + Mode_Count ].Add( geometry );
		}{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount = 3;
			cmd.InstanceCount( particle_count );
			geometry.Draw( cmd );
			geometry.ArgIn( "un_Particles", particles );
			scenes[ Mode_Instancing ].Add( geometry );
		}{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount = 4;
			cmd.InstanceCount( particle_count );
			geometry.Draw( cmd );
			geometry.ArgIn( "un_Particles", particles );
			scenes[ Mode_Instancing + Mode_Count ].Add( geometry );
		}{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			UnifiedGeometry_Draw	cmd;
			cmd.VertexCount( particle_count.Mul( 3 ));
			geometry.Draw( cmd );
			geometry.ArgIn( "un_Particles", particles );
			scenes[ Mode_TriList ].Add( geometry );
		}{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			UnifiedGeometry_Draw	cmd;
			cmd.VertexCount( particle_count.Mul( 6 ));
			geometry.Draw( cmd );
			geometry.ArgIn( "un_Particles", particles );
			scenes[ Mode_TriList + Mode_Count ].Add( geometry );
		}/*{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			UnifiedGeometry_DrawMeshTasks	cmd;
			cmd.TaskCount( particle_count.DivCeil( 32 ));
			geometry.Draw( cmd );
			geometry.ArgIn( "un_Particles", particles );
			scenes[ Mode_MS ].Add( geometry );
			scenes[ Mode_MS + Mode_Count ].Add( geometry );
		}*/

		// render loop
		{
			RC<ComputePass>			sim_pass = ComputePass();
			sim_pass.SetDebugLabel( "sim", RGBA8u(0, 255, 0, 255) );
			sim_pass.ArgInOut( "un_Particles", particles );
			sim_pass.LocalSize( 64 );
			sim_pass.DispatchThreads( particle_count );
			sim_pass.Constant( "iSteps",	sim_steps );
			sim_pass.Slider( "iTimeScale",	1.f,	4.f,	(IsRemoteGPU() ? 4.f : 1.f) );
			sim_pass.EnableIfGreater( sim_steps, 0 );
		}

		for (uint i = 0; i < scenes.size(); ++i)
		{
			//if ( (i % Mode_Count == Mode_MS) and not Supports_MeshShader() )
			//	continue;
			if ( (i % Mode_Count == Mode_GS) and not Supports_GeometryShader() )
				continue;

			RC<SceneGraphicsPass>	pass = scenes[i].AddGraphicsPass( "draw" );
			pass.EnableIfEqual( draw_mode, i );
			pass.Constant( "iSize", particle_size );
			switch ( i )
			{
				case Mode_GS :
					pass.SetDebugLabel( "Rays, GS", RGBA8u(200, 200, 0, 255) );
					pass.AddPipeline( "particles/Rays-gs.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/particles/Rays-gs.as)
					break;

				case Mode_GS + Mode_Count :
					pass.SetDebugLabel( "Dots, GS", RGBA8u(200, 200, 0, 255) );
					pass.AddPipeline( "particles/Dots-gs.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/particles/Dots-gs.as)
					break;

				case Mode_Instancing :
					pass.SetDebugLabel( "Rays, instancing", RGBA8u(200, 200, 0, 255) );
					pass.AddPipeline( "particles/Rays-i.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/particles/Rays-i.as)
					break;

				case Mode_Instancing + Mode_Count :
					pass.SetDebugLabel( "Dots, instancing", RGBA8u(200, 200, 0, 255) );
					pass.AddPipeline( "particles/Dots-i.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/particles/Dots-i.as)
					break;

				case Mode_TriList :
					pass.SetDebugLabel( "Rays, trilist", RGBA8u(200, 200, 0, 255) );
					pass.AddPipeline( "particles/Rays-tl.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/particles/Rays-tl.as)
					break;

				case Mode_TriList + Mode_Count :
					pass.SetDebugLabel( "Dots, trilist", RGBA8u(200, 200, 0, 255) );
					pass.AddPipeline( "particles/Dots-tl.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/particles/Dots-tl.as)
					break;
			}
			pass.Output( "out_Color", rt, RGBA32f(0.0) );
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
