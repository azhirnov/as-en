// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
//
// [shader](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/shaders/tools/SphereNoise.glsl)
//
// functions:
//	void  AddNoise (float params [14]);
//	void  AddTurbulence (float params [12]);
//	void  AddSpline (float params [8]);

#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif


RC<DynamicUInt>		dyn_pass_id;
RC<DynamicFloat>	dyn_height;
RC<DynamicUInt>		view_mode;
RC<DynamicFloat3>	dyn_light_dir;
RC<DynamicUInt>		view_layer;

RC<Image>			rt;
RC<Image>			ds;
RC<Image>			noise_tex;
RC<Image>			palette_tex;
RC<Image>			height_map_view;
RC<Image>			normal_map;
RC<Image>			normal_map_view;
RC<Scene>			scene;

const uint			noise_dim		= 1024;
const uint			tile_size		= 8;		// ? frames to whole update

uint				pass_id			= 1;
bool				has_dist_tex	= false;
uint				max_view_mode	= 4;		// can be overridden by user
float				displacement	= 0.07f;


void  AddNoise (const array<float> &params)
{
	RC<ComputePass>		pass = ComputePass( "tools/SphereNoise.glsl", "P="+pass_id+";GEN_NOISE;sTILE="+tile_size );
	pass.ArgInOut( "un_Noise",		noise_tex );

	pass.Slider( "iNoise",			0,						17,					int(params[0]) );
	pass.Slider( "iOctaves",		1,						10,					int(params[1]) );
	pass.Slider( "iOp",				int2(0),				int2(7,5),			int2( int(params[2]), int(params[3]) ));	// neg, abs, >, <, [0,1], [-1,0], [-1,0]
																															// none, add, mul, mul unorm, min, max
	pass.Slider( "iPScale",			0.1f,					100.f,				params[4] );
	pass.Slider( "iPBias",			float3(-10.f),			float3(10.f),		float3( params[5],  params[6], params[7] ));
	pass.Slider( "iParams",			float4(-1.f),			float4(2.f),		float4( params[8],  params[9], params[10], params[11] ));
	pass.Slider( "iVScaleBias",		float2(0.01f, -2.f),	float2(4.f, 4.f),	float2( params[12], params[13] ));

	pass.Constant( "iIsDistortion",	has_dist_tex ? 1 : 0 );		has_dist_tex = false;

	pass.LocalSize( 8, 8 );
	pass.DispatchThreads( noise_dim / tile_size, noise_dim / tile_size, noise_tex.ArrayLayers() );

	pass.EnableIfGreater( dyn_pass_id, pass_id-1 );
	++pass_id;
}


void  AddTurbulence (const array<float> &params)
{
	Assert( not has_dist_tex );

	RC<ComputePass>		pass = ComputePass( "tools/SphereNoise.glsl", "P="+pass_id+";GEN_TURB;sTILE="+tile_size );
	pass.ArgInOut( "un_Noise",		noise_tex );

	pass.Slider( "iNoise",			0,				14,				int(params[0]) );
	pass.Slider( "iOctaves",		1,				10,				int(params[1]) );
	pass.Slider( "iOp",				0,				1,				int(params[2]) );
	pass.Slider( "iPScale",			0.1f,			100.f,			params[3] );
	pass.Slider( "iPBias",			float3(-10.f),	float3(10.f),	float3( params[4],  params[5], params[6] ));
	pass.Slider( "iParams",			float4(-1.f),	float4(2.f),	float4( params[7],  params[8], params[9], params[10] ));
	pass.Slider( "iDScale",			0.0f,			2.f,			params[11] );

	pass.LocalSize( 8, 8 );
	pass.DispatchThreads( noise_dim / tile_size, noise_dim / tile_size, noise_tex.ArrayLayers() );

	pass.EnableIfGreater( dyn_pass_id, pass_id-1 );

	++pass_id;
	has_dist_tex = true;
}


void  AddSpline (const array<float> &params)
{
	Assert( not has_dist_tex );

	RC<Buffer>	spline_params = Buffer();
	spline_params.UseLayout(
		"SplineParams",
		"float4	A;" +
		"float3	B;" +
		"int	Mode;"
	);
	{
		RC<ComputePass>		pass = ComputePass( "tools/SphereNoise.glsl", "P="+pass_id+";APPLY_SPLINE;sTILE="+tile_size );
		pass.ArgInOut(	"un_Noise",		noise_tex );
		pass.ArgOut(	"un_Params",	spline_params );

		const float		min	= -2.f;
		const float		max	= 2.f;
		pass.Slider( "iMode",	0,				2,				int(params[0]) );
		pass.Slider( "iA",		float4(min),	float4(max),	float4( params[1], params[2], params[3], params[4] ));
		pass.Slider( "iB",		float3(min),	float3(max),	float3( params[5], params[6], params[7] ));
		pass.Slider( "iScale",	0.5f,			10.f,			params[8] );
		pass.Slider( "iBias",	-2.f,			2.f,			params[9] );

		pass.LocalSize( 8, 8 );
		pass.DispatchThreads( noise_dim / tile_size, noise_dim / tile_size, noise_tex.ArrayLayers() );

		pass.EnableIfGreater( dyn_pass_id, pass_id-1 );
	}{
		RC<Postprocess>		pass = Postprocess( "tools/TerrainNoise.glsl", "VIEW_SPLINE;P="+pass_id );
		pass.Output( "out_Color",	rt );
		pass.ArgIn(  "un_Params",	spline_params );
		pass.EnableIfEqual( dyn_pass_id, pass_id );
	}
	++pass_id;
}


funcdef void  SetupPasses_t ();


void  SetupSphereNoise (SetupPasses_t @setupPasses)
{
	// initialize
	@rt					= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
	@ds					= Image( Supported_DepthFormat(), SurfaceSize() );
	@dyn_height			= DynamicFloat();
	@view_layer			= DynamicUInt();
	@view_mode			= DynamicUInt();
	@dyn_light_dir		= DynamicFloat3( Normalize(float3( 0.f, -1.f, -0.5f )));
	@dyn_pass_id		= DynamicUInt();
	@scene				= Scene();
	@noise_tex			= Image( EPixelFormat::RGBA16F, uint2(noise_dim), ImageLayer(6) );	noise_tex.Name( "Noise (height) texture" );
	@height_map_view	= noise_tex.CreateView( EImage::Cube );
	@normal_map			= Image( EPixelFormat::RGBA16F, uint2(noise_dim), ImageLayer(6) );	normal_map.Name( "Normal map" );
	@normal_map_view	= normal_map.CreateView( EImage::Cube );
	@palette_tex		= Image( EImageType::Float_2D, "res/tex/default-gradient.aeimg" );

	// setup camera
	{
		RC<OrbitalCamera>	camera = OrbitalCamera();

		camera.ClipPlanes( 0.1f, 100.f );
		camera.FovY( 60.f );
		camera.Offset( 2.f );
		camera.OffsetScale( -10.0f );

		scene.Set( camera );
	}

	// create sphere
	{
		RC<SphericalCube>	sphere = SphericalCube();

		sphere.ArgIn( "un_HeightMap",	height_map_view,	Sampler_LinearRepeat );
		sphere.ArgIn( "un_NormalMap",	normal_map_view,	Sampler_LinearRepeat );
		sphere.ArgIn( "un_Palette",		palette_tex,		Sampler_LinearClamp );
		sphere.DetailLevel( 9 );

		scene.Add( sphere );
	}

	// render loop
	{
		ClearImage( rt, RGBA32f(0.0) );

		// user-defined passes
		setupPasses();
	}

	Slider( view_mode,		"View",		0,		max_view_mode,	2 );
	Slider( dyn_pass_id,	"Pass",		0,		pass_id-1,		pass_id-1 );	// 0 - disable all passes (disable updates, keep current noise texture)
	Slider( view_layer,		"Face",		0,		5,				0 );
	Slider( dyn_height,		"Height",	-0.2f,	0.2f,			displacement );

	{
		RC<ComputePass>		pass = ComputePass( "tools/SphereNoise.glsl", "GEN_NORMAL;sTILE="+tile_size );
		pass.ArgIn(		"un_HeightMap",	height_map_view,	Sampler_LinearRepeat );
		pass.ArgInOut(	"un_NormalMap",	normal_map );
		pass.Constant(	"iHeight",		dyn_height );

		pass.LocalSize( 8, 8 );
		pass.DispatchThreads( noise_dim / tile_size, noise_dim / tile_size, noise_tex.ArrayLayers() );

		pass.EnableIfGreater( dyn_pass_id, 0 );
	}{
		RC<Postprocess>		pass = Postprocess( "tools/TerrainNoise.glsl", "VIEW_2D" );
		pass.OutputBlend( "out_Color",	rt,			EBlendFactor::OneMinusDstAlpha, EBlendFactor::One, EBlendOp::Add );	// blend with spline editor
		pass.ArgIn(		  "un_Noise",	noise_tex,	Sampler_NearestClamp );
		pass.Constant(	  "iLayer",		view_layer );
		pass.EnableIfEqual( view_mode,	0 );
	}{
		RC<Postprocess>		pass = Postprocess( "tools/TerrainNoise.glsl", "VIEW_2D_VEC3" );
		pass.OutputBlend( "out_Color",	rt,			EBlendFactor::OneMinusDstAlpha, EBlendFactor::One, EBlendOp::Add );	// blend with spline editor
		pass.ArgIn(		  "un_Noise",	noise_tex,	Sampler_NearestClamp );
		pass.Constant(	  "iLayer",		view_layer );
		pass.EnableIfEqual( view_mode,	1 );
	}{
		RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "parallax" );
		draw.AddPipeline( "tools/SphereNoise-Parlx.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tools/SphereNoise-Parlx.as)
		draw.Output(	"out_Color",	rt,		RGBA32f(0.0) );
		draw.Output(					ds,		DepthStencil(1.f, 0) );
		draw.Constant(	"iHeight",		dyn_height );
		draw.Constant(	"iLightDir",	dyn_light_dir );
		draw.EnableIfEqual( view_mode,	2 );
	}
	if ( Supports_TessellationShader() )
	{
		RC<SceneGraphicsPass>	draw = scene.AddGraphicsPass( "tessellation" );
		draw.AddPipeline( "tools/SphereNoise-Tess.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tools/SphereNoise-Tess.as)
		draw.Output(	"out_Color",	rt,		RGBA32f(0.0) );
		draw.Output(					ds,		DepthStencil(1.f, 0) );
		draw.Slider(	"iTessLevel",	1.f,	64.f,	16.f );
		draw.Constant(	"iHeight",		dyn_height );
		draw.Constant(	"iLightDir",	dyn_light_dir );
		draw.EnableIfEqual( view_mode,	3 );
	}{
		RC<Postprocess>		pass = Postprocess( "tools/TerrainNoise.glsl", "VIEW_1D" );
		pass.OutputBlend( "out_Color",	rt,			EBlendFactor::OneMinusDstAlpha, EBlendFactor::One, EBlendOp::Add );	// blend with spline editor
		pass.ArgIn(		"un_Noise",		noise_tex,	Sampler_LinearClamp );
		pass.Slider(	"iYOffset",		0.f,		1.f,		0.5f );
		pass.Slider(	"iTilePos",		int2(0),	int2(4),	int2(0) );
		pass.Constant(	"iLayer",		view_layer );
		pass.EnableIfEqual( view_mode,	4 );
	}
}
