// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
//
// [shader](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/shaders/tools/VolumeNoise.glsl)
//
// functions:
//	void  AddNoise (float params [14]);
//	void  AddTurbulence (float params [12]);
//	void  AddSpline (float params [8]);

#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif

RC<Image>			rt;
RC<Image>			noise_tex;
RC<FPVCamera>		camera;

RC<DynamicUInt>		dyn_pass_id;
RC<DynamicFloat>	view_layer;
RC<DynamicUInt>		view_mode;

const uint			noise_dim		= 512;
const uint			tile_size		= 4;		// ? frames to whole update

uint				pass_id			= 1;
bool				has_dist_tex	= false;
uint				max_view_mode	= 4;		// can be overridden by user


void  AddNoise (const array<float> &params)
{
	RC<ComputePass>		pass = ComputePass( "tools/VolumeNoise.glsl", "P="+pass_id+";GEN_NOISE;vTILE="+tile_size );
	pass.ArgInOut( "un_Noise",		noise_tex );

	pass.Slider( "iNoise",			0,						17,					int(params[0]) );
	pass.Slider( "iOctaves",		1,						10,					int(params[1]) );
	pass.Slider( "iOp",				int2(0),				int2(4),			int2( int(params[2]), int(params[3]) ));	// neg, abs, >, <, [0,1], [-1,0], [-1,0]
																															// none, add, mul, mul unorm, min, max
	pass.Slider( "iPScale",			0.1f,					10.f,				params[4] );
	pass.Slider( "iPBias",			float3(-10.f),			float3(10.f),		float3( params[5],  params[6], params[7] ));
	pass.Slider( "iParams",			float4(-1.f),			float4(2.f),		float4( params[8],  params[9], params[10], params[11] ));
	pass.Slider( "iVScaleBias",		float2(0.01f, -2.f),	float2(4.f, 4.f),	float2( params[12], params[13] ));

	pass.Constant( "iIsDistortion",	has_dist_tex ? 1 : 0 );		has_dist_tex = false;

	pass.LocalSize( 8, 8 );
	pass.DispatchThreads( uint3(noise_dim) / tile_size );

	pass.EnableIfGreater( dyn_pass_id, pass_id-1 );
	++pass_id;
}


void  AddTurbulence (const array<float> &params)
{
	Assert( not has_dist_tex );

	RC<ComputePass>		pass = ComputePass( "tools/VolumeNoise.glsl", "P="+pass_id+";GEN_TURB;vTILE="+tile_size );
	pass.ArgInOut( "un_Noise",		noise_tex );

	pass.Slider( "iNoise",			0,				16,				int(params[0]) );
	pass.Slider( "iOctaves",		1,				10,				int(params[1]) );
	pass.Slider( "iOp",				0,				1,				int(params[2]) );
	pass.Slider( "iPScale",			0.1f,			10.f,			params[3] );
	pass.Slider( "iPBias",			float3(-10.f),	float3(10.f),	float3( params[4],  params[5], params[6] ));
	pass.Slider( "iParams",			float4(-1.f),	float4(2.f),	float4( params[7],  params[8], params[9], params[10] ));
	pass.Slider( "iDScale",			0.0f,			2.f,			params[11] );

	pass.LocalSize( 8, 8 );
	pass.DispatchThreads( uint3(noise_dim) / tile_size );

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
		RC<ComputePass>		pass = ComputePass( "tools/VolumeNoise.glsl", "P="+pass_id+";APPLY_SPLINE;vTILE="+tile_size );
		pass.ArgInOut(	"un_Noise",		noise_tex );
		pass.ArgOut(	"un_Params",	spline_params );

		const float		min	= -2.f;
		const float		max	= 2.f;
		pass.Slider( "iMode",	0,				2,				int(params[0]) );
		pass.Slider( "iA",		float4(min),	float4(max),	float4( params[1], params[2], params[3], params[4] ));
		pass.Slider( "iB",		float3(min),	float3(max),	float3( params[5], params[6], params[7] ));
		pass.Slider( "iScale",	1.0f,			10.f,			params[8] );
		pass.Slider( "iBias",	-2.f,			2.f,			params[9] );

		pass.LocalSize( 8, 8 );
		pass.DispatchThreads( uint3(noise_dim) / tile_size );

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


void  SetupVolumeNoise (SetupPasses_t @setupPasses)
{
	// initialize
	@rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
	@view_layer		= DynamicFloat();
	@view_mode		= DynamicUInt();
	@dyn_pass_id	= DynamicUInt();
	@noise_tex		= Image( EPixelFormat::RGBA16F, uint3(noise_dim) );	noise_tex.Name( "Noise" );
	@camera			= FPVCamera();

	// setup camera
	{
		camera.ClipPlanes( 0.02f, 12.f );
		camera.FovY( 60.f );
		camera.RotationScale( 1.f, 1.f );

		const float	s = 0.6f;
		camera.ForwardBackwardScale( s );
		camera.UpDownScale( s );
		camera.SideMovementScale( s );
	}

	// render loop
	{
		ClearImage( rt, RGBA32f(0.0) );

		// user-defined passes
		setupPasses();
	}

	Slider( view_mode,		"View",		0,		max_view_mode,	1 );
	Slider( dyn_pass_id,	"Pass",		0,		pass_id-1,		pass_id-1 );	// 0 - disable all passes (disable updates, keep current noise texture)
	Slider( view_layer,		"ZSlice",	0.f,	1.f,			0.f );

	{
		RC<Postprocess>		pass = Postprocess( "tools/VolumeNoise.glsl", "VIEW_2D" );
		pass.OutputBlend( "out_Color",	rt,			EBlendFactor::OneMinusDstAlpha, EBlendFactor::One, EBlendOp::Add );	// blend with spline editor
		pass.ArgIn(		  "un_Noise",	noise_tex,	Sampler_NearestClamp );
		pass.Constant(	  "iLayer",		view_layer );
		pass.EnableIfEqual( view_mode,	0 );
	}{
		RC<Postprocess>		pass = Postprocess( "tools/VolumeNoise.glsl", "TRACE_OPAQUE" );
		pass.Set( camera );
		pass.Output( "out_Color",		rt );
		pass.ArgIn(  "un_Volume",		noise_tex,	Sampler_LinearClamp );
		pass.Slider( "iMinValue",		0.f,		0.9f,	0.5f );
		pass.EnableIfEqual( view_mode,	1 );
	}{
		RC<Postprocess>		pass = Postprocess( "tools/VolumeNoise.glsl", "TRACE_CLOUD" );
		pass.Set( camera );
		pass.Output( "out_Color",		rt );
		pass.ArgIn(  "un_Volume",		noise_tex,	Sampler_LinearClamp );
		pass.Slider( "iDensity",		0.01f,		0.3f,	0.1f );
		pass.ColorSelector( "iLightColor", RGBA8u(255) );
		pass.EnableIfEqual( view_mode,	2 );
	}{
		RC<Postprocess>		pass = Postprocess( "tools/VolumeNoise.glsl", "TRACE_CLOUD2" );
		pass.Set( camera );
		pass.Output( "out_Color",		rt );
		pass.ArgIn(  "un_Volume",		noise_tex,		Sampler_LinearClamp );
		pass.Slider( "iDensity",		0.01f,			0.3f,			0.1f );
		pass.Slider( "iLightDir",		float3(-1.0),	float3(1.0),	float3(0.0, 1.0, 0.4) );
		pass.ColorSelector( "iLightColor", RGBA8u(255) );
		pass.EnableIfEqual( view_mode,	3 );
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
