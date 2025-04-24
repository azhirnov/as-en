// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
//
// [shader](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/shaders/tools/TerrainNoise.glsl)
//
// functions:
//	void  AddNoise (float params [14]);
//	void  AddTurbulence (float params [12]);
//	void  AddSpline (float params [8]);

#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif

RC<Image>			rt;
RC<Image>			ds;
RC<Image>			noise_tex;
RC<Image>			palette_tex;
RC<Image>			normal_map;
RC<Scene>			scene;

RC<DynamicUInt>		dyn_pass_id;
RC<DynamicFloat>	dyn_ter_height;
RC<DynamicFloat>	dyn_ter_size;
RC<DynamicUInt>		view_mode;

const uint2			noise_dim		= uint2(1024);
const uint			tile_size		= 8;		// 64 frames to whole update
const uint			grid_tess		= 4;		// tessellation factor

uint				pass_id			= 1;
bool				has_dist_tex	= false;
float				terrain_height	= 1.f;
uint				max_view_mode	= 3;		// can be overridden by user


void  _AddPass (RC<ComputePass> pass)
{
	pass.LocalSize( 8, 8 );
	pass.DispatchThreads( noise_dim / tile_size );

	pass.EnableIfGreater( dyn_pass_id, pass_id-1 );
	++pass_id;
}


void  AddNoise (const array<float> &params)
{
	RC<ComputePass>		pass = ComputePass( "tools/TerrainNoise.glsl", "P="+pass_id+";GEN_NOISE;tTILE="+tile_size );
	pass.ArgInOut( "un_Noise",		noise_tex );

	pass.Slider( "iNoise",			0,						22,					int(params[0]) );
	pass.Slider( "iOctaves",		1,						10,					int(params[1]) );
	pass.Slider( "iOp",				int2(0),				int2(7,5),			int2( int(params[2]), int(params[3]) ));	// neg, abs, >, <, [0,1], [-1,0], [-1,0]
																															// none, add, mul, mul unorm, min, max
	pass.Slider( "iPScale",			0.1f,					100.f,				params[4] );
	pass.Slider( "iPBias",			float3(-10.f),			float3(10.f),		float3( params[5],  params[6], params[7] ));
	pass.Slider( "iParams",			float4(-1.f),			float4(2.f),		float4( params[8],  params[9], params[10], params[11] ));
	pass.Slider( "iVScaleBias",		float2(0.01f, -2.f),	float2(4.f, 4.f),	float2( params[12], params[13] ));

	pass.Constant( "iIsDistortion",	has_dist_tex ? 1 : 0 );		has_dist_tex = false;

	_AddPass( pass );
}


void  AddTurbulence (const array<float> &params)
{
	Assert( not has_dist_tex );

	RC<ComputePass>		pass = ComputePass( "tools/TerrainNoise.glsl", "P="+pass_id+";GEN_TURB;tTILE="+tile_size );
	pass.ArgInOut( "un_Noise",		noise_tex );

	pass.Slider( "iNoise",			0,				16,				int(params[0]) );
	pass.Slider( "iOctaves",		1,				10,				int(params[1]) );
	pass.Slider( "iOp",				0,				1,				int(params[2]) );
	pass.Slider( "iPScale",			0.1f,			100.f,			params[3] );
	pass.Slider( "iPBias",			float3(-10.f),	float3(10.f),	float3( params[4],  params[5], params[6] ));
	pass.Slider( "iParams",			float4(-1.f),	float4(2.f),	float4( params[7],  params[8], params[9], params[10] ));
	pass.Slider( "iDScale",			0.0f,			2.f,			params[11] );
	pass.Slider( "iDOffset",		-1.0f,			1.f,			params[12] );

	_AddPass( pass );

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
		RC<ComputePass>		pass = ComputePass( "tools/TerrainNoise.glsl", "P="+pass_id+";APPLY_SPLINE;tTILE="+tile_size );
		pass.ArgInOut(	"un_Noise",		noise_tex );
		pass.ArgOut(	"un_Params",	spline_params );

		const float		min	= -2.f;
		const float		max	= 2.f;
		pass.Slider( "iMode",	0,				2,				int(params[0]) );
		pass.Slider( "iA",		float4(min),	float4(max),	float4( params[1], params[2], params[3], params[4] ));
		pass.Slider( "iB",		float3(min),	float3(max),	float3( params[5], params[6], params[7] ));
		pass.Slider( "iScale",	0.5f,			10.f,			params[8] );
		pass.Slider( "iBias",	-2.f,			2.f,			params[9] );

		_AddPass( pass );
	}{
		RC<Postprocess>		pass = Postprocess( "tools/TerrainNoise.glsl", "VIEW_SPLINE;P="+(pass_id-1) );
		pass.Output( "out_Color",	rt );
		pass.ArgIn(  "un_Params",	spline_params );
		pass.EnableIfEqual( dyn_pass_id, (pass_id-1) );
	}
}


funcdef void  SetupPasses_t ();


void  SetupTerrainNoise (SetupPasses_t @setupPasses)
{
	// initialize
	@rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
	@ds				= Image( Supported_DepthFormat(), SurfaceSize() );
	@dyn_ter_height	= DynamicFloat();
	@dyn_ter_size	= DynamicFloat();
	@view_mode		= DynamicUInt();
	@dyn_pass_id	= DynamicUInt();
	@scene			= Scene();
	@noise_tex		= Image( EPixelFormat::RGBA16F, noise_dim );	noise_tex.Name( "Noise (height) texture" );
	@normal_map		= Image( EPixelFormat::RGBA16F, noise_dim );	normal_map.Name( "Normal map" );
	@palette_tex	= Image( EImageType::Float_2D, "res/tex/default-gradient.aeimg" );

	// setup camera
	{
		RC<FPVCamera>	camera = FPVCamera();

		camera.ClipPlanes( 0.02f, 20.f );
		camera.FovY( 60.f );
		camera.RotationScale( 1.f, 1.f );

		const float	s = 0.6f;
		camera.ForwardBackwardScale( s );
		camera.UpDownScale( s );
		camera.SideMovementScale( s );

		camera.Position( float3( 0.f, -1.5f, -5.f ));

		scene.Set( camera );
	}

	// render loop
	{
		ClearImage( rt, RGBA32f(0.0) );

		// user-defined passes
		setupPasses();
	}

	Assert( @palette_tex != null );

	// create terrain
	{
		RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

		UnifiedGeometry_Draw	cmd;
		cmd.vertexCount = Area( noise_dim * grid_tess ) * 2 - 4;

		geometry.Draw( cmd );
		geometry.ArgIn( "un_HeightMap", noise_tex,		Sampler_LinearClamp );
		geometry.ArgIn( "un_NormalMap", normal_map,		Sampler_LinearClamp );
		geometry.ArgIn( "un_Palette",	palette_tex,	Sampler_LinearClamp );

		scene.Add( geometry );
	}

	Slider( view_mode,		"View",		0,		max_view_mode,	2 );
	Slider( dyn_pass_id,	"Pass",		0,		pass_id-1,		pass_id-1 );	// 0 - disable all passes (disable updates, keep current noise texture)
	Slider( dyn_ter_height,	"Height",	-2.f,	2.f,			terrain_height );
	Slider( dyn_ter_size,	"Size",		8.f,	32.f,			8.f );

	{
		RC<ComputePass>		pass = ComputePass( "tools/TerrainNoise.glsl", "GEN_NORMAL;tTILE="+tile_size );
		pass.ArgIn(		"un_HeightMap",	noise_tex );
		pass.ArgInOut(	"un_NormalMap",	normal_map );
		pass.Constant(	"iHeight",		dyn_ter_height );
		pass.Constant(	"iSize",		dyn_ter_size );

		pass.LocalSize( 8, 8 );
		pass.DispatchThreads( noise_dim / tile_size );

		pass.EnableIfGreater( dyn_pass_id, 0 );
	}{
		RC<Postprocess>		pass = Postprocess( "tools/TerrainNoise.glsl", "VIEW_2D" );
		pass.OutputBlend( "out_Color",	rt,			EBlendFactor::OneMinusDstAlpha, EBlendFactor::One, EBlendOp::Add );	// blend with spline editor
		pass.ArgIn(		  "un_Noise",	noise_tex,	Sampler_NearestClamp );
		pass.EnableIfEqual( view_mode, 0 );
	}{
		RC<Postprocess>		pass = Postprocess( "tools/TerrainNoise.glsl", "VIEW_2D_VEC3" );
		pass.OutputBlend( "out_Color",	rt,			EBlendFactor::OneMinusDstAlpha, EBlendFactor::One, EBlendOp::Add );	// blend with spline editor
		pass.ArgIn(		  "un_Noise",	noise_tex,	Sampler_NearestClamp );
		pass.EnableIfEqual( view_mode, 1 );
	}{
		RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "terrain" );
		pass.AddPipeline( "tools/Terrain.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tools/Terrain.as)
		pass.Output(	"out_Color",	rt,		RGBA32f(0.3, 0.5, 1.0, 1.0) );
		pass.Output(					ds,		DepthStencil(1.f, 0) );
		pass.Constant(	"iSize",		dyn_ter_size );
		pass.Constant(	"iHeight",		dyn_ter_height );
		pass.Constant(	"iTerrainSize",	noise_dim * grid_tess );
		pass.EnableIfEqual( view_mode, 2 );
	}{
		RC<Postprocess>		pass = Postprocess( "tools/TerrainNoise.glsl", "VIEW_1D" );
		pass.OutputBlend( "out_Color",	rt,			EBlendFactor::OneMinusDstAlpha, EBlendFactor::One, EBlendOp::Add );	// blend with spline editor
		pass.ArgIn(  "un_Noise",		noise_tex,	Sampler_LinearClamp );
		pass.Slider( "iYOffset",		0.f,		1.f,		0.5f );
		pass.Slider( "iTilePos",		int2(0),	int2(4),	int2(0) );
		pass.EnableIfEqual( view_mode, 3 );
	}
}
