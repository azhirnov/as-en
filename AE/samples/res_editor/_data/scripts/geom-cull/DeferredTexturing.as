// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	First pass:
		* draw scene to the G-Buffer (RGB10A2 + RG16 + depth)

	Second pass:
		* draw fullscreen triangle
		* unpack G-Buffer
		* calculate worldPos from depth
		* apply material, used material ID, texture UV and UV derivatives or LOD
		* apply lighting, used worldPos and normal

	Results:
		* Texture LOD can be stored in 8 bits (unorm)
		* dUV can be stored in 4x 8bits
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>			gbuf1		= Image( EPixelFormat::RGB10_A2_UNorm, SurfaceSize() );	gbuf1.Name( "GBuf-Normal" );
		RC<Image>			gbuf2		= Image( EPixelFormat::RG16F, SurfaceSize() );			gbuf2.Name( "GBuf-UV" );
		RC<Image>			gbuf3		= Image( EPixelFormat::RGBA32F, SurfaceSize() );		gbuf3.Name( "GBuf-UVdxdy" );	// reference
		RC<Image>			gbuf4		= Image( EPixelFormat::RGBA32U, SurfaceSize() );		gbuf3.Name( "GBuf-Packed" );	// matrial ID, dUV, lod
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT-Color" );
		RC<Image>			ds			= Image( EPixelFormat::Depth32F, SurfaceSize() );		ds.Name( "RT-Depth" );
		RC<Scene>			scene		= Scene();
		RC<DynamicUInt>		duv_packing	= DynamicUInt();
		RC<DynamicUInt>		lod_packing	= DynamicUInt();

		// setup camera
		{
			RC<FPVCamera>	camera	= FPVCamera();

			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );

			const float	s = 1.0f;
			camera.ForwardBackwardScale( s*2.0f, s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
		}

		// setup model
		{
			RC<Model>	model	= Model( "res/models/Sponza/Sponza.gltf" );

			model.InitialTransform( float3(0.f, -1.f, 0.f), float3(0.f, ToRad(90.f), ToRad(180.f)), 100.f );

			model.AddOmniLight( float3(0.f, -5.f, 0.f), float3(0.f, 0.f, 0.05f), RGBA32f(1.f) );

			scene.Add( model );
		}

		Slider( lod_packing,	"LOD pack",		0,	3 );	// 8bit, 12bit, 16bit, 32bit
		Slider( duv_packing,	"dUV pack",		0,	2 );	// 8bit, 12bit, 16bit

		// render loop
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque" );
			pass.AddPipeline( "*-pass1.ppln" );
			pass.Output( "out_Normals",		gbuf1,	RGBA32f(0.0) );
			pass.Output( "out_UV",			gbuf2,	RGBA32f(0.0) );
			pass.Output( "out_RefDUV",		gbuf3,	RGBA32f(0.0) );
			pass.Output( "out_Packed",		gbuf4,	RGBA32u(~0) );
			pass.Output(					ds,		DepthStencil(1.f, 0) );
			pass.Layer( ERenderLayer::Opaque );
			pass.Constant( "iDUVPack",		duv_packing );
			pass.Constant( "iLodPack",		lod_packing );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "postprocess" );
			pass.AddPipeline( "*-pass2.ppln" );
			pass.Output( "out_Color",		rt );
			pass.ArgIn(  "un_GBufNormal",	gbuf1,	Sampler_NearestClamp );
			pass.ArgIn(  "un_GBufUV",		gbuf2,	Sampler_NearestClamp );
			pass.ArgIn(  "un_GBufRefUV",	gbuf3,	Sampler_NearestClamp );
			pass.ArgIn(  "un_GBufPacked",	gbuf4,	Sampler_NearestClamp );
			pass.ArgIn(  "un_Depth",		ds,		Sampler_NearestClamp );
			pass.Layer( ERenderLayer::PostProcess );
			pass.Constant( "iDUVPack",		duv_packing );
			pass.Constant( "iLodPack",		lod_packing );
			pass.Slider( "iCmp",			0,		6,		0 );	// 0 - reference, 1 - texture grad, 2 - texture lod, 3/4 - diff grad, 5/6 - diff lod
			pass.Slider( "iCmpScale",		0,		10,		3 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
