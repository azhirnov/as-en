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
		RC<Image>	rt			= Image( EPixelFormat::sRGB8_A8, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>	smoke		= Image( EImageType::Float_2D, "res/tex/smoke-2.jpg" );
		RC<Scene>	scene		= Scene();
		RC<Buffer>	inst_buf	= Buffer();

		// create geometry
		{
			const uint	inst_count = 38;
			inst_buf.ArrayLayout(
				"InstanceData",
				"float3x2	m1;" +
				"float		r;" +
				"float		t;" +
				"float		f;",
				inst_count
			);

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();

			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount		= 4;
			cmd.instanceCount	= inst_count;
			geometry.Draw( cmd );

			geometry.ArgIn(	"un_InstBuffer",	inst_buf );
			geometry.ArgIn(	"un_Smoke",			smoke,		Sampler_LinearMipmapClamp );

			scene.Add( geometry );
		}

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgInOut( "un_InstBuffer",	inst_buf );
			pass.LocalSize( 1 );
			pass.DispatchThreads( 1 );
		}{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "draw" );
			draw.SetDebugLabel( "draw", RGBA32f(1.f, 0.f, 0.f, 1.f) );
			draw.AddPipeline( "samples/Smoke-2.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Smoke-2.as)
			draw.Output( "out_Color",	rt, RGBA32f(0.2f, 0.3f, 0.7f, 1.f) );
		//	draw.Output( "out_Color",	rt, RGBA32f(0.f) );
			draw.ColorSelector( "iColor" );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "Matrix.glsl"
	#include "Easing.glsl"
	#include "Geometry.glsl"


	float3x2  MakeMatrix (float r, float t, float offset)
	{
		float		s = Lerp( 0.02, 0.5, t );
		float3x2	m = float3x2( f2x2_Rotate( float_Pi * r ) *
								  f2x2_Scale( s ));
		m[2] = Lerp( float2(-0.9,0.9), float2(1.0,-1.0), QuadraticEaseIn( t ));
		m[2] += float2(s) * Sin( float_Pi * (t*2.5 + offset) ) * 0.3;
		return m;
	}

	void  Init (out InstanceData data, uint i)
	{
		const uint	count = un_InstBuffer.elements.length();

		data.r = float(i) * 0.73;
		data.t = float(i) / float(count) - 1.0;
		data.f = Fract( float(i) * 2.376 );
	}


	void  Main ()
	{
		float	dt	= un_PerPass.timeDelta * 0.1;

		// initialize
		if ( un_PerPass.frame == 0 )
		{
			for (uint i = 0; i < un_InstBuffer.elements.length(); ++i)
			{
				InstanceData	data;
				Init( OUT data, i );
				un_InstBuffer.elements[i] = data;
			}
			dt = 0.0;
		}

		for (uint i = 0; i < un_InstBuffer.elements.length(); ++i)
		{
			InstanceData	data = un_InstBuffer.elements[i];

			data.m1 = MakeMatrix( data.r, data.t, data.f );
			data.r += dt;
			data.t += dt;

			if ( data.t > 1.1 )
			{
				data.r = Fract( data.r );
				data.t = Fract( data.t );
				data.f += 0.87;
			}

			un_InstBuffer.elements[i] = data;
		}
	}

#endif
//-----------------------------------------------------------------------------

