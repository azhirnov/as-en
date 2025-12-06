// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Simulate particle collisions on screen-space depth & normals.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicDim>	dim					= SurfaceSize();
		RC<Image>		rt					= Image( EPixelFormat::RGBA8_UNorm, dim );
		RC<Image>		norm				= Image( EPixelFormat::RGBA16_SNorm, dim );
		RC<Image>		ds					= Image( Supported_DepthFormat(), dim );

		RC<Scene>		scene				= Scene();
		RC<Scene>		scene_vfx			= Scene();

		RC<FPVCamera>	camera				= FPVCamera();
		RC<Buffer>		obj_buf				= Buffer();
		RC<Buffer>		particles			= Buffer();
		uint			inst_count			= 0;
		const uint		local_size			= 64;
		const uint		max_particle_count	= 100;
		RC<DynamicUInt>	p_count				= DynamicUInt();
		RC<DynamicUInt>	p2_count			= p_count.Mul( local_size );

		Slider( p_count,	"Particles",	1,	max_particle_count,		10 );

		particles.ArrayLayout(
			"Particle",
			"	float4	position_size;" +
			"	float4	velocity_color;" +
			"	float	startTime;" +
			"	uint	numCollisions;",
			max_particle_count * local_size );

		// setup camera
		{
			camera.ClipPlanes( 1.0f, 100.f );
			camera.FovY( 60.f );

			const float	s = 2.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Dimension( dim );
			scene.Set( camera );
			scene_vfx.Set( camera );
		}

		// particle geometry
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount = 3;
			cmd.InstanceCount( p2_count );
			geometry.Draw( cmd );
			geometry.ArgIn( "un_Particles", particles );
			scene_vfx.Add( geometry );
		}

		// AABBs
		{
			const array<float3>		position = {
				float3( 0.0, 101.0, 4.0),
				float3( 1.0, 1.5,  5.0),
				float3(-1.7, 1.2,  5.0)
			};
			const array<float3>		scale = {
				float3(100.0),
				float3(1.0),
				float3(0.6, 2.0, 0.6)
			};
			const array<float3>		color = {
				float3(0.3, 0.2, 0.3),
				float3(0.2, 0.4, 0.2),
				float3(0.2, 0.2, 0.6)
			};
			Assert( scale.size() == position.size() );
			Assert( color.size() == position.size() );
			inst_count = position.size();

			obj_buf.FloatArray( "position",		position );
			obj_buf.FloatArray( "scale",		scale );
			obj_buf.FloatArray( "color",		color );
			obj_buf.LayoutName( "ObjectTransform" );
		}

		// create scene with AABBs
		{
			array<float3>	positions, normals;
			array<uint>		indices;
			GetCube( OUT positions, OUT normals, OUT indices );

			RC<Buffer>		geom_data = Buffer();
			geom_data.FloatArray( "positions",	positions );
			geom_data.UIntArray(  "indices",	indices );
			geom_data.LayoutName( "GeometryData" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_Transform",	obj_buf );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= indices.size();
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.instanceCount = inst_count;
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		// render loop //
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw scene" );
			pass.AddPipeline( "samples/SS-Particles-Scene.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/SS-Particles-Scene.as)
			pass.Output(	"out_Color",		rt,			RGBA32f(0.0) );
			pass.Output(	"out_Normals",		norm,		RGBA32f(0.0) );
			pass.Output(						ds,			DepthStencil(1.0, 0) );
			pass.Constant(	"iLight",			Normalize(float3(0.4, -1.0, -0.5)) );
		}{
			RC<ComputePass>		pass = ComputePass( "", "SIMULATION" );
			pass.Set( camera );
			pass.ArgInOut(	"un_Particles",		particles );
			pass.ArgIn(		"un_Depth",			ds,			Sampler_NearestClamp );
			pass.ArgIn(		"un_Normals",		norm,		Sampler_NearestClamp );
			pass.Slider(	"iGravity",			1.0,					10.0,					5.0 );
			pass.Slider(	"iDepthThreshold",	0.0,					1.0,					0.1 );
			pass.Slider(	"iMaxCollisions",	0,						10,						4 );
			pass.Slider(	"iColorMode",		0,						4,						4 );
			pass.Slider(	"iMaxLifetime",		1.0,					100.0,					15.0 );
			pass.Slider(	"iEmitterPos",		float3(-3.0,-2.0,3.0),	float3(3.0,2.0,6.0),	float3(2.0, -0.2, 5.0) );
			pass.Slider(	"iCollision",		0,						1,						1 );
			pass.Constant(	"iResolution",		dim );
			pass.LocalSize( local_size );
			pass.DispatchGroups( p_count );
		}{
			RC<SceneGraphicsPass>	pass = scene_vfx.AddGraphicsPass( "particles" );
			pass.AddPipeline( "particles/Rays-i.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/particles/Rays-i.as)
			pass.Output(	"out_Color",		rt );
			pass.Output(						ds );
			pass.Slider(	"iSize",			0.5,		4.0,		1.0 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef __INTELLISENSE__
#	include <glsl.h>
#	define SIMULATION
#endif
//-----------------------------------------------------------------------------
#ifdef SIMULATION
	#define DISABLE_un_PerObject
	#include "Transform.glsl"

	#include "Hash.glsl"
	#include "Color.glsl"
	#include "Matrix.glsl"
	#include "InvocationID.glsl"
	#include "Particles-Simulation.glsl"


	void  RestartParticle (out Particle outParticle, const float globalTime)
	{
		float	index	= float(GetGlobalIndex());
		float	size	= float(GetGlobalIndexSize());
		float	vel		= 0.5;
		float	dt		= DHash12( float2( globalTime, index * 100.0 )) * 5.0;

		outParticle.position_size.xyz	= iEmitterPos;
		outParticle.position_size.w		= 8.0;
		outParticle.velocity_color.xyz	= ParticleEmitter_ConeVector( index, size, 1.0 ).zxy * -vel;
		outParticle.velocity_color.w	= uintBitsToFloat( 0x00000001 );
		outParticle.startTime			= globalTime + dt;
		outParticle.numCollisions		= 0;
	}


	void  CheckCollisions (inout Particle outParticle, const float3 prev_pos, const float dt)
	{
		const float4	p1_snorm	= LocalPosToNormClipSpace( outParticle.position_size.xyz );
		const float2	p1			= ToUNorm( p1_snorm.xy ) * iResolution.xy;
		const int2		coord		= int2(p1);

		if ( AnyLess( coord, int2(0) ) or AnyGreaterEqual( coord, iResolution.xy ) or p1_snorm.z < 0.0 )
			return;

		// depth in view space
	  #if 0
		const float		depth	= UnProjectNDC( MatInverse(un_PerPass.camera.proj), float3(p1_snorm, gl.texture.Fetch( un_Depth, coord, 0 ).r) ).z;
		const float		d0		= LocalPosToViewSpace( prev_pos ).z;
		const float		d1		= LocalPosToViewSpace( outParticle.position_size.xyz ).z;
	  #else
		const float		depth	= FastUnProjectZ( un_PerPass.camera.proj, gl.texture.Fetch( un_Depth, coord, 0 ).r );
		const float		d0		= FastViewSpaceZ( un_PerPass.camera.view, prev_pos - un_PerPass.camera.pos );
		const float		d1		= FastViewSpaceZ( un_PerPass.camera.view, outParticle.position_size.xyz - un_PerPass.camera.pos );
	  #endif

		bool	collision = false;
		collision = Abs( d1 - depth ) < iDepthThreshold;
		collision = collision or (d1 < depth and d0 > depth);

		if ( collision )
		{
			float3	norm = gl.texture.Fetch( un_Normals, coord, 0 ).rgb;  // world space
			outParticle.velocity_color.xyz = Reflect( outParticle.velocity_color.xyz, norm );

			// offset to avoid collision with same plane
			outParticle.position_size.xyz += outParticle.velocity_color.xyz * dt + norm * 0.01;

			++outParticle.numCollisions;
		}
		else
		if ( d1 > depth )
		{
			// restart if beyond the obstacle
			outParticle.velocity_color.w = 0.0;
		}
	}


	void  UpdateParticle (inout Particle outParticle, const float dt, const float globalTime)
	{
		float	start_time	= outParticle.startTime;

		if ( start_time > globalTime )
			return;

		float3	accel		= float3(0.0, iGravity*0.1, 0.0);
		float3	prev_pos	= outParticle.position_size.xyz;

		UniformlyAcceleratedMotion( INOUT outParticle.position_size.xyz, INOUT outParticle.velocity_color.xyz, accel, dt );

		if ( iCollision == 1 )
		{
			CheckCollisions( INOUT outParticle, prev_pos, dt );
		}

		if ( floatBitsToUint( outParticle.velocity_color.w ) == 0	or
			 globalTime - start_time > iMaxLifetime					or
			 outParticle.numCollisions > iMaxCollisions				)
		{
			RestartParticle( OUT outParticle, globalTime );
			return;
		}

		switch ( iColorMode )
		{
			case 0 :
				outParticle.velocity_color.w = uintBitsToFloat( ParticleColor_FromVelocityLength( outParticle.velocity_color.xyz ));
				break;

			case 1 :
				outParticle.velocity_color.w = uintBitsToFloat( ParticleColor_FromVelocity( outParticle.velocity_color.xyz ));
				break;

			case 2 :
				outParticle.velocity_color.w = uintBitsToFloat( ParticleColor_FromNormalizedVelocity( outParticle.velocity_color.xyz ));
				break;

			case 3 :
				outParticle.velocity_color.w = uintBitsToFloat( packUnorm4x8( Rainbow( float(outParticle.numCollisions) / float(iMaxCollisions) )));
				break;

			case 4 :
				outParticle.velocity_color.w = uintBitsToFloat( packUnorm4x8( Rainbow( (globalTime - start_time) / iMaxLifetime )));
				break;
		}
	}


	void  Main ()
	{
		UpdateParticle( un_Particles.elements[GetGlobalIndex()], Clamp( un_PerPass.timeDelta, 4.0e-3, 32.0e-3 ), un_PerPass.time );
	}

#endif
//-----------------------------------------------------------------------------
