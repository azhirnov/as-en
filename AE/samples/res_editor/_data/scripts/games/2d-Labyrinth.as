// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Escape from labyrinth.
	Map is procedurally generated.
	Use right mouse button to turn on flashlight.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GAME_LOGIC
#	define GEN_MAP
#	define DRAW_GAME
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			sdf_map	= Image( EPixelFormat::R16F, uint2(64) );				sdf_map.Name( "SDF map" );
		RC<Image>			rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<TopDownCamera>	camera	= TopDownCamera();
		RC<Buffer>			cbuf	= Buffer();

		// setup camera
		{
			camera.RotationScale( 0.f );

			float	s = 4.0f;
			camera.ForwardBackwardScale( s );
			camera.SideMovementScale( s );
			camera.Dimension( rt.Dimension() );
		}

		{
			cbuf.Int(	"generate",			1 );
			cbuf.Int(	"seed",				0 );
			cbuf.Float(	"flashLightPower",	0.f );

			cbuf.Float(	"playerPos",		float2(sdf_map.Dimension2()) * 0.5 );	// pixels
			cbuf.Float(	"playerRadius",		0.25f );								// pixels

			cbuf.Float(	"teleportPos",		float2(2.f, 2.f) );						// pixels
			cbuf.Float(	"teleportRadius",	1.0f );									// pixels

			cbuf.Float(	"flashLightDir",	float2() );
			cbuf.Float(	"prevCamPos",		float2() );

			cbuf.Float(	"uvToMap",			float2(sdf_map.Dimension2()) );
			cbuf.Float(	"mapToUV",			float2(1.f) / float2(sdf_map.Dimension2()) );
		}

		// render loop
		{
			RC<ComputePass>		gen_map = ComputePass( "", "GEN_MAP" );
			gen_map.ArgInOut( "un_SDFMap",	sdf_map );
			gen_map.ArgInOut( "un_CBuf",	cbuf );
			gen_map.LocalSize( 8, 8 );
			gen_map.DispatchThreads( sdf_map.Dimension2() );
		}{
			RC<ComputePass>		logic = ComputePass( "", "GAME_LOGIC" );
			logic.ArgIn(	"un_SDFMap",	sdf_map,	Sampler_LinearClamp );
			logic.ArgIn(	"un_RT",		rt,			Sampler_LinearClamp );
			logic.ArgInOut(	"un_CBuf",		cbuf );
			logic.Set(		camera );
			logic.LocalSize( 1 );
			logic.DispatchGroups( 1 );
		}{
			RC<Postprocess>		draw = Postprocess( EPostprocess::Shadertoy, "DRAW_GAME" );
			draw.ArgIn( "un_SDFMap",	sdf_map,	Sampler_LinearClamp );
			draw.ArgIn( "un_CBuf",		cbuf );
			draw.Output( rt );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE

	ND_ uint  CalcSeed ()
	{
		return (un_PerPass.seed & 0xFFFF) ^ (un_PerPass.seed >> 16) ^ un_CBuf.seed;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_MAP
	#include "InvocationID.glsl"
	#include "Hash.glsl"
	#include "SDF.glsl"

	ND_ float  SDF (const float2 coord, float seed)
	{
		float2	ipart	= Floor( coord );
		float2	fpart	= Fract( coord );
		float	md		= 2147483647.0;

		for (int y = -2; y <= 2; ++y)
		for (int x = -2; x <= 2; ++x)
		{
			float2	cur		= float2(x,y);
			float4	h		= DHash42( ipart + cur + seed );
			float2	hsize	= Lerp( float2(0.1), float2(1.1), h.xy );
			float2	pos		= h.zw + cur - fpart;
			float	d		= SDF2_Rect( pos, hsize );

			md = Min( md, d );
		}
		return md + 0.3;
	}

	void  Main ()
	{
		if ( un_CBuf.generate == 0 )
			return;

		const float		map_size		= 20.0;
		const float2	pos				= GetGlobalCoordUNorm().xy * map_size;
		const float2	player_pos		= (un_CBuf.playerPos * un_CBuf.mapToUV) * map_size;
		const float2	teleport_pos	= (un_CBuf.teleportPos * un_CBuf.mapToUV) * map_size;

		const uint		seed			= CalcSeed();
		const float		safe_r			= 1.0;

		float	d = SDF( pos, float(seed) / 0x1FFF );

		d = Max( d, -SDF2_Rect( SDF_Move( pos, player_pos ), float2(safe_r) ));
		d = Max( d, -SDF2_Rect( SDF_Move( pos, teleport_pos ), float2(safe_r) ));

		d = Min( d, -SDF2_Rect( SDF_Move( pos, map_size * 0.5 ), float2(map_size * 0.5 - 0.5) ));

		gl.image.Store( un_SDFMap, GetGlobalCoord().xy, float4(d) );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GAME_LOGIC
	#include "Math.glsl"
	#include "InvocationID.glsl"

	ND_ float  ReadSDF (const float2 pos)
	{
		return gl.texture.SampleLod( un_SDFMap, pos * un_CBuf.mapToUV, 0 ).r;
	}

	void  Main ()
	{
		un_CBuf.generate = 0;

		un_CBuf.flashLightPower = Saturate( un_CBuf.flashLightPower + 4.0 * (un_PerPass.mouse.z > 0.0 ? un_PerPass.timeDelta : -un_PerPass.timeDelta) );

		if ( un_PerPass.mouse.z > 0.0 )
		{
			float2	scr_size	= float2(gl.texture.GetSize( un_RT, 0 ));
			float2	mpos		= un_PerPass.mouse.xy * scr_size;
					mpos		= ToUNorm(MapPixCoordToSNormCorrected2( mpos, scr_size )) * un_CBuf.uvToMap;

			un_CBuf.flashLightDir = Normalize( mpos - un_CBuf.playerPos );
		}

		// player
		{
			float2	pos_delta = un_PerPass.camera.pos.xy - un_CBuf.prevCamPos;
			un_CBuf.prevCamPos = un_PerPass.camera.pos.xy;

			float d = ReadSDF( un_CBuf.playerPos + pos_delta );
			un_CBuf.playerPos += pos_delta * Saturate( d / (un_CBuf.playerRadius * 0.5) );
		}

		if ( Distance( un_CBuf.playerPos, un_CBuf.teleportPos ) < (un_CBuf.teleportRadius + un_CBuf.playerRadius) )
		{
			const uint	seed		= CalcSeed();

			// restart
			un_CBuf.generate		= 1;
			un_CBuf.seed			= un_CBuf.seed + 3;
			un_CBuf.playerPos		= un_CBuf.uvToMap * 0.5;
			un_CBuf.teleportPos.x	= ((seed & 4) == 0 ? 2.f : 4.f);
			un_CBuf.teleportPos.y	= ((seed & 8) == 0 ? 2.f : 4.f);
			un_CBuf.teleportPos.x	= ((seed & 1) == 0 ? un_CBuf.teleportPos.x : un_CBuf.uvToMap.x - un_CBuf.teleportPos.x);
			un_CBuf.teleportPos.y	= ((seed & 2) == 0 ? un_CBuf.teleportPos.y : un_CBuf.uvToMap.y - un_CBuf.teleportPos.y);
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef DRAW_GAME
	#include "InvocationID.glsl"
	#include "Waves.glsl"

	const float3	c_BackgroundColor		= float3(0.2, 0.2, 0.2);
	const float3	c_WallColor				= float3(0.2, 0.0, 1.0);
	const float3	c_PlayerColor			= float3(0.0, 2.0, 0.0);
	const float3	c_AmbientLightColor		= float3(0.3, 0.4, 0.5) * 2.2;
	const float3	c_FlashLightColor		= float3(1.0, 1.2, 1.0) * 0.8;
	const float3	c_AmbientLight			= float3(1.0, 1.0, 1.0) * 0.001;
	const float3	c_TeleportColor			= float3(0.4, 0.0, 0.5);
	const float3	c_TeleportWaveColor		= float3(0.5, 0.0, 0.7);
	const float		c_AmbientLightRadius	= 0.125;
	const float		c_FlashLightMaxDist		= 0.5;


	ND_ float  ReadSDF (const float2 pos)
	{
		const float  c_SDFScale = 0.75;
		return gl.texture.Sample( un_SDFMap, pos * un_CBuf.mapToUV, 0 ).r * c_SDFScale;
	}

	ND_ float  TraceRay (const float2 lightPos, const float2 origin, const bool insideWall)
	{
		const float		t_max		= Max( Distance( lightPos, origin ) - 0.001, 0.001 );
		const float		min_dist	= 0.00625;
		const int		max_iter	= 128;
		const float2	dir			= Normalize( lightPos - origin );
		float2			pos			= origin;
		float			t			= 0.0;
		int				i			= 0;

		if ( insideWall )
		{
			for (; i < max_iter; ++i)
			{
				float d = -ReadSDF( pos );

				if ( d < min_dist )
					break;

				t += d;
				pos = origin + dir * t;
			}

			t += 0.2;
			pos = origin + dir * t;
		}

		for (; i < max_iter; ++i)
		{
			float	d = ReadSDF( pos );

			if ( d < min_dist )
				break;

			t += d;
			pos = origin + dir * t;

			if ( t > t_max )
				break;
		}

		return t > t_max ? 1.0 : 0.0;
	}

	ND_ float3  ToneMap (const float3 hdr)
	{
		float3	x1 = hdr - 0.727;
		float3	y1 = (x1 / (x1 + 0.477)) * 0.388 + 0.723;
		float3	s  = Step( 0.7, hdr );
		return Lerp( hdr, Max( float3(0.0), y1 ), s );
	}

	void mainImage (out float4 fragColor, in float2 fragCoord)
	{
		const float2	pos_on_map				= ToUNorm( GetGlobalCoordSNormCorrected2() ) * un_CBuf.uvToMap;
		const float2	player_pos				= un_CBuf.playerPos;
		const float		dist_to_player			= Distance( pos_on_map, player_pos );
		const float2	dir_to_player			= Normalize( player_pos - pos_on_map );
		const float		dist_to_teleport		= Distance( un_CBuf.teleportPos, pos_on_map );
		float3			light_color				= float3(0.0, 0.0, 0.0);
		bool			trace_light				= false;
		const bool		inside_wall				= ReadSDF( pos_on_map ) < 0.0;

		fragColor		= float4(c_BackgroundColor, 1.0);
		fragColor.rgb	= Lerp( fragColor.rgb, c_WallColor, (inside_wall ? 1.0 : 0.0) );

		// teleport
		{
			float	factor	= Saturate( 1.0 - dist_to_teleport / un_CBuf.teleportRadius );
			fragColor.rgb	= Lerp( fragColor.rgb, c_TeleportColor, factor );
		}

		// flash light color
		if ( un_CBuf.flashLightPower > 0.0					and
			 Dot( un_CBuf.flashLightDir, -dir_to_player ) > 0.0 )
		{
			const float		tan_of_angle	= 0.5;
			const float2	light_dir		= un_CBuf.flashLightDir;

			float2	begin		= player_pos;
			float2	end			= begin + light_dir;
			float	dist_to_ray	= Abs( (-light_dir.y * pos_on_map.x) + (light_dir.x * pos_on_map.y) + (begin.x * end.y - end.x * begin.y) );
			float	cone_radius	= dist_to_player * tan_of_angle;

			float	atten		= dist_to_player / (c_FlashLightMaxDist * Max( un_CBuf.uvToMap.x, un_CBuf.uvToMap.y ));
					atten		= Saturate( 1.0 - atten * atten );
			float	factor		= Saturate( 1.0 - dist_to_ray / cone_radius ) * un_CBuf.flashLightPower * atten;
					light_color	= Lerp( light_color, c_FlashLightColor, factor );
					trace_light	= trace_light or factor > 0.0;
		}

		// ambient light
		{
			float	factor		= dist_to_player / (c_AmbientLightRadius * Max( un_CBuf.uvToMap.x, un_CBuf.uvToMap.y ));
					factor		= Saturate( 1.0 - Sqrt(factor) );
					light_color	= Lerp( light_color, c_AmbientLightColor, factor );
					trace_light	= trace_light or factor > 0.0;
		}

		if ( trace_light )
		{
			float	shading	= TraceRay( player_pos, pos_on_map, inside_wall ) * 0.5;
			float2	norm	= float2(-dir_to_player.y, dir_to_player.x);
			shading += TraceRay( player_pos, pos_on_map + norm * 0.125, inside_wall ) * 0.25;
			shading += TraceRay( player_pos, pos_on_map - norm * 0.125, inside_wall ) * 0.25;

			fragColor.rgb = (fragColor.rgb * light_color * shading) + (fragColor.rgb * c_AmbientLight);
		}
		else
		{
			fragColor.rgb *= c_AmbientLight;
		}

		// teleport wave
		{
			// use current color to detect if teleport is visible or shaded
			const float	is_invisible = 1.0 - SmoothStep( Dot( fragColor.rgb, fragColor.rgb ), 0.0, Dot( c_AmbientLight, c_AmbientLight ));

			const float	wave_width		= 0.2;
			const float	wave_r			= un_CBuf.teleportRadius * Fract( iTime );
			float		wave			= BumpStep( dist_to_teleport, wave_r, wave_r + wave_width );
						fragColor.rgb	= Lerp( fragColor.rgb, c_TeleportWaveColor, wave * is_invisible );
		}

		// player
		{
			float	factor			= Saturate( 1.0 - dist_to_player / un_CBuf.playerRadius );
					fragColor.rgb	= Lerp( fragColor.rgb, c_PlayerColor, factor );
		}

		fragColor.rgb = ToneMap( fragColor.rgb * 3.0 );
	}

#endif
//-----------------------------------------------------------------------------
