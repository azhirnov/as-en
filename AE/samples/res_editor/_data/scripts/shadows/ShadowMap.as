// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Simple shadow mapping.
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
		RC<Image>				rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>				ds			= Image( Supported_DepthFormat(), SurfaceSize() );		ds.Name( "RT-Depth" );
		RC<Image>				sm			= Image( Supported_DepthFormat(), uint2(1024) );		sm.Name( "ShadowMap" );

		RC<FPVCamera>			camera		= FPVCamera();
		RC<Scene>				scene		= Scene();
		RC<Scene>				shadow_scene= Scene();

		const float3			light_dir	= Normalize(float3( 0.f, -1.f, 0.f ));
		const float				sm_size		= 10.f;
		const float4x4			sm_vp		= float4x4().Ortho( RectF(-sm_size, -sm_size, sm_size, sm_size), float2(-10.f, 10.f) ) *
											  float4x4().RotateX( ToRad(90.f) );
	//	const float4x4			sm_vp		= float4x4().Ortho( RectF(-sm_size, -sm_size, sm_size, sm_size), float2(-10.f, 10.f) ) *
	//										  float4x4().LookAt( -light_dir, float3(0.0), float3(0.0, -1.0, 0.0) );
		const float4x4			sm_bias_mat	= float4x4(	float4( 0.5, 0.0, 0.0, 0.0 ),
														float4( 0.0, 0.5, 0.0, 0.0 ),
														float4( 0.0, 0.0, 1.0, 0.0 ),
														float4( 0.5, 0.5, 0.0, 1.0 ));

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
		}

		// create geometry
		{
			RC<Buffer>		geom_data	= Buffer();
			RC<Buffer>		light_data	= Buffer();

			array<float3>	positions;
			array<uint>		indices;
			GetSphere( 3, OUT positions, OUT indices );

			array<float3>	normals = positions;

			// move sphere
			for (uint i = 0; i < positions.size(); ++i) {
				positions[i] += float3(0.f, -1.0f, 4.f);
			}

			// add ground
			{
				array<float2>	plane_pos;
				array<uint>		plane_indices;
				GetGrid( 2, OUT plane_pos, OUT plane_indices );

				MergeMesh( indices, positions.size(), plane_indices );
				for (uint i = 0; i < plane_pos.size(); ++i)
				{
					positions.push_back( float3( ToSNorm(plane_pos[i].x) * sm_size, 1.f, ToSNorm(plane_pos[i].y) * sm_size ));
					normals.push_back( float3( 0.f, -1.f, 0.f ));	// up
				}
			}

			geom_data.FloatArray(	"positions",	positions );
			geom_data.FloatArray(	"normals",		normals );
			geom_data.UIntArray(	"indices",		indices );
			geom_data.LayoutName( "GeometrySBlock" );

			light_data.Float(		"lightDir",		light_dir );
			light_data.Float(		"shadowVP",		sm_vp );
			light_data.Float(		"viewToShadow",	sm_bias_mat * sm_vp );
			light_data.Float(		"invShadowDim",	float2(1.f) / float2(sm.Dimension2()) );
			light_data.LayoutName( "LightsSBlock" );


			RC<UnifiedGeometry>		geometry1	= UnifiedGeometry();

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = indices.size();
			cmd.IndexBuffer( geom_data, "indices" );

			geometry1.Draw( cmd );
			geometry1.ArgIn(	"un_Geometry",	geom_data );
			geometry1.ArgIn(	"un_Lights",	light_data );

			shadow_scene.Add( geometry1 );


			RC<UnifiedGeometry>		geometry2	= geometry1.Clone();

			geometry2.ArgIn( "un_ShadowMap",	sm,		Sampler_NearestClamp );	// linear sampler may be unsupported

			scene.Add( geometry2 );
		}

		// render loop
		{
			RC<SceneGraphicsPass>	draw = shadow_scene.AddGraphicsPass( "shadow map pass" );
			draw.AddPipeline( "samples/ShadowMap-pass1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/ShadowMap-pass1.as)
			draw.Output( sm, DepthStencil( 1.f, 0 ));
		}
		{
			RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "main pass" );
			draw.AddPipeline( "samples/ShadowMap-pass2.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/ShadowMap-pass2.as)
			draw.Set( camera );
			draw.Output( "out_Color", rt, RGBA32f( 0.3, 0.5, 1.0, 1.0 ));
			draw.Output( ds, DepthStencil( 1.f, 0 ));
			draw.Slider( "iPCF", 0, 1 );
		}

		DbgView( sm, DbgViewFlags::LinearDepth );
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
