// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*

*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PUT_OBJECTS
#	define SETUP_SM
#	define VIEW_TO_SM
#	define RESOLVE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const EPixelFormat	hdr_fmt			= EPixelFormat::RGBA16F;
		RC<DynamicDim>		dim				= SurfaceSize();
		RC<DynamicUInt>		sm_pot			= DynamicUInt();
		RC<DynamicDim>		sm_dim			= sm_pot.Add( 9 ).Exp2().Dimension2();
		RC<Image>			rt				= Image( hdr_fmt, dim );							rt.Name( "Main-RT" );
		RC<Image>			rt_col			= Image( EPixelFormat::RGB10_A2_UNorm, dim );		rt_col.Name( "Albedo" );
		RC<Image>			rt_norm			= Image( EPixelFormat::RGB10_A2_UNorm, dim );		rt_norm.Name( "Normals" );
		RC<Image>			ds				= Image( Supported_DepthStencilFormat(), dim );		ds.Name( "Depth" );
		RC<Image>			sm				= Image( Supported_DepthFormat(), sm_dim );			sm.Name( "ShadowMap" );
		RC<Image>			sm_moments32	= Image( EPixelFormat::RGBA32F, sm_dim );			sm.Name( "ShadowMoments-32" );
		RC<Image>			sm_moments16	= Image( EPixelFormat::RGBA16F, sm_dim );			sm.Name( "ShadowMoments-16" );
		RC<Image>			sm_col			= Image( EPixelFormat::RGBA8_UNorm, sm_dim );
		RC<Buffer>			obj_buf			= Buffer();
		RC<Buffer>			cbuf			= Buffer();
		RC<Scene>			scene			= Scene();
		RC<FPVCamera>		camera			= FPVCamera();
		RC<DynamicFloat2>	exp				= DynamicFloat2();
		RC<DynamicFloat>	depth_bias		= DynamicFloat();
		RC<DynamicUInt>		mode			= DynamicUInt();
		const uint			obj_count		= 32 * 32;

		obj_buf.ArrayLayout(
			"ObjectTransform",
			"	float3	position;"
			"	float	rotation;"
			"	float3	scale;"
			"	uint	color;",
			obj_count );

		cbuf.UseLayout(
			"ParamsBuffer",
			"	float4x4	shadowVP;"
			"	float3		lightDir;"
			"	float3		cameraPos;"
			"	float		shadowSizeWS;"
			"	float3		cornerPoints[8];"
		);

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 60.f );

			const float	s = 2.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Dimension( rt.Dimension() );

			scene.Set( camera );
		}

		// add ground
		{
			// XY plane
			const float				size = 1.f;
			const array<float3>		positions = {
				float3(-size,  0.0, -size),
				float3(-size,  0.0,  size),
				float3( size,  0.0, -size),
				float3( size,  0.0,  size),
				float3(), float3() // make compatible with other 'GeometryData' type
			};
			const array<uint>		indices = {
				0, 3, 1,
				0, 2, 3
			};

			RC<Buffer>		geom_data = Buffer();
			geom_data.FloatArray( "position",	positions );
			geom_data.UIntArray(  "indices",	indices );
			geom_data.LayoutName( "GeometryData" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_Transform",	obj_buf );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.PipelineHint( "opaque" );
			cmd.indexCount	= indices.size();
			cmd.IndexBuffer( geom_data, "indices" );
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		// add trees
		{
			// simplified tree with 2 planes
			const array<float3>		positions = {
				float3(-1.0,  1.0,  0.0),
				float3( 1.0,  1.0,  0.0),
				float3( 0.0, -1.0,  0.0),

				float3( 0.0,  1.0, -1.0),
				float3( 0.0,  1.0,  1.0),
				float3( 0.0, -1.0,  0.0)
			};
			const array<uint>		indices = {
				0, 1, 2,
				3, 4, 5
			};

			RC<Buffer>		geom_data = Buffer();
			geom_data.FloatArray( "position",	positions );
			geom_data.UIntArray(  "indices",	indices );
			geom_data.LayoutName( "GeometryData" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_Transform",	obj_buf );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.PipelineHint( "opaque" );
			cmd.indexCount	= indices.size();
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.firstInstance = 1;
			cmd.instanceCount = obj_count-1;
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		Slider( mode,		"Mode",			0,				4 );									// ESM, VSM, EVSM, OriginMSM, MSM
		Slider( sm_pot,		"ShadowDim",	0,				3,				2 );
		Slider( exp,		"Exp",			float2(5.0),	float2(80.0),	float2(40.0, 5.0) );	// 1 - ESM, 2 - EVSM
		Slider( depth_bias,	"DepthBias",	0.0,			10.0,			0.0 );

		Label( sm_dim.X(),	"ShadowMap dim" );

		// render loop
		{
			RC<ComputePass>			pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut(	"un_Objects",	obj_buf );
			pass.LocalSize( 64 );
			pass.DispatchThreads( obj_count );
		}{
			RC<ComputePass>			pass = ComputePass( "", "SETUP_SM" );
			pass.Set( camera );
			pass.ArgInOut(	"un_Params",	cbuf );
			pass.Slider(	"iLightDir",	float3(-1.0),	float3(1.0),	float3(0.4, 0.0, -0.35) );
			pass.Slider(	"iShadowDist",	1.0,			100.0,			15.0 );
			pass.Slider(	"iShadowZ",		0.0,			100.0,			25.0 );		// or set 'depthClamp=true' in pipeline
			pass.Constant(	"iShadowDim",	sm_dim );
			pass.LocalSize( 1 );
			pass.DispatchThreads( 1 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque" );
			pass.AddPipeline( "samples/LowPolyTrees/Opaque.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/LowPolyTrees/Opaque.as)
			pass.Output( "out_Color",		rt_col,			RGBA32f(0.0) );
			pass.Output( "out_Normal",		rt_norm,		RGBA32f(0.0) );
			pass.Output(					ds,				DepthStencil(1.0, 0) );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "ESM" );
			pass.AddPipeline( "samples/LowPolyTrees/ExpSM.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/LowPolyTrees/ExpSM.as)
			pass.Output( "out_Color",		sm_col,			RGBA32f(0.0) );
			pass.Output( "out_Exp",			sm_moments32,	RGBA32f(0.0) );
			pass.Output(					sm,				DepthStencil(1.0, 0) );
			pass.ArgIn(  "un_Params",		cbuf );
			pass.Constant( "iExp",			exp );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "VSM" );
			pass.AddPipeline( "samples/LowPolyTrees/VarSM.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/LowPolyTrees/VarSM.as)
			pass.Output( "out_Color",		sm_col,			RGBA32f(0.0) );
			pass.Output( "out_Moments",		sm_moments32,	RGBA32f(0.0) );
			pass.Output(					sm,				DepthStencil(1.0, 0) );
			pass.ArgIn(  "un_Params",		cbuf );
			pass.Constant( "iBias",			depth_bias );
			pass.EnableIfEqual( mode, 1 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "EVSM" );
			pass.AddPipeline( "samples/LowPolyTrees/EVSM.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/LowPolyTrees/EVSM.as)
			pass.Output( "out_Color",		sm_col,			RGBA32f(0.0) );
			pass.Output( "out_Moments",		sm_moments32,	RGBA32f(0.0) );
			pass.Output(					sm,				DepthStencil(1.0, 0) );
			pass.ArgIn(  "un_Params",		cbuf );
			pass.Constant( "iExp",			exp );
			pass.Constant( "iBias",			depth_bias );
			pass.EnableIfEqual( mode, 2 );
		}{
			// setup: DepthBias = 2.1, iNormBiasScale = 0.8

			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "MSM" );
			pass.AddPipeline( "samples/LowPolyTrees/MSM.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/LowPolyTrees/MSM.as)
			pass.Output( "out_Color",		sm_col,			RGBA32f(0.0) );
			pass.Output( "out_Moments",		sm_moments32,	RGBA32f(0.0) );
			pass.Output(					sm,				DepthStencil(1.0, 0) );
			pass.ArgIn(  "un_Params",		cbuf );
			pass.Constant( "iBias",			depth_bias );
			pass.EnableIfGreater( mode, 2 );
		}{
			BlitImage( sm_moments32, sm_moments16 );
		}{
			RC<Postprocess>			pass = Postprocess( "", "RESOLVE" );
			pass.Set( camera );
			pass.Output( "out_Color",		rt,				RGBA32f(0.0) );
			pass.ArgIn( "un_Albedo",		rt_col,			Sampler_NearestClamp );
			pass.ArgIn( "un_Normal",		rt_norm,		Sampler_NearestClamp );
			pass.ArgIn( "un_Depth",			ds,				Sampler_NearestClamp );
			pass.ArgIn( "un_ShadowMap",		sm,				Sampler_NearestClamp );
			pass.ArgIn( "un_Moments32",		sm_moments32,	Sampler_LinearClamp );
			pass.ArgIn( "un_Moments16",		sm_moments16,	Sampler_LinearClamp );
			pass.ArgIn( "un_SMColor",		sm_col,			Sampler_NearestClamp );
			pass.ArgIn( "un_Params",		cbuf );
			pass.Slider( "iView",			0,		3,		0 );
			pass.Slider( "iHighPrecision",	0,		1,		1 );
			pass.Slider( "iScale",			1.0,	10.0,	3.0 );
			pass.Slider( "iNormBiasScale",	0.0,	2.0,	0.2 );
			pass.Constant( "iShadowDim",	sm_dim );
			pass.Constant( "iExp",			exp );
			pass.Constant( "iMode",			mode );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PUT_OBJECTS
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"

	#define GROUND_Y	2.0

	void  Main ()
	{
		ObjectTransform		obj;
		const uint			idx		= GetGlobalIndex();
		const uint			size	= uint( Sqrt( float(un_Objects.elements.length()) ) + 0.5 );
		float2				uv		= float2( idx / size, idx % size );
							uv		+= ToSNorm( DHash22( uv * 222.0 )) * 0.7;
							uv		= uv / float(size);

		if ( idx == 0 )
		{
			// floor
			obj.position	= float3(0.0, GROUND_Y, 0.0);
			obj.rotation	= 0.0;
			obj.scale		= float2(1000.0, 1.0).xyx;
			obj.color		= packUnorm4x8( float4(0.2) );
			un_Objects.elements[idx] = obj;
			return;
		}

		if ( idx >= un_Objects.elements.length() )
			return;

		obj.position.xz	= ToSNorm( uv ) * 40.0;
		obj.position.y	= GROUND_Y;

		obj.rotation	= ToSNorm( DHash12( uv.xy * 111.0 )) * float_Pi;

		obj.scale.x		= 0.5 + DHash12( uv.yx * 111.0 ) * 1.0;
		obj.scale.y		= 0.8 + DHash12( uv * 444.0 ) * 5.0;
		obj.scale.z		= 2.0;

		obj.position.y	-= obj.scale.y;
		obj.position.z	*= obj.scale.z * 2.0;

		obj.color		= packUnorm4x8( float4( DHash32( uv * 333.0 ), 1.0 ));

		un_Objects.elements[idx] = obj;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SETUP_SM
	#include "AABB.glsl"
	#include "Sphere.glsl"
	#include "Matrix.glsl"
	#include "Frustum.glsl"


	// input and output in light space
	float2  SnapToTexel (float2 centerLS, float2 sizeLS)
	{
		float2	units_per_texel	= sizeLS / float2(iShadowDim);

		float2	min_ls	= centerLS - sizeLS * 0.5;
				min_ls	= Floor( min_ls / units_per_texel ) * units_per_texel;

		return	min_ls + sizeLS * 0.5;
	}


	AABB  CalcShadowBBox (float3x3 view)
	{
		float4x4	inv_vp		= un_PerPass.camera.invViewProj;
		float		z			= FastProjectZ( un_PerPass.camera.proj, iShadowDist );

		// 'UnProjectNDC' returns world space, camera at (0,0,0)
		// 'view * wp' returns light space

		float3		near		= float3(0.0);	// same as camera pos

		float3		far0		= view * UnProjectNDC( inv_vp, float3(-1.0,  1.0, z) );
		float3		far1		= view * UnProjectNDC( inv_vp, float3( 1.0,  1.0, z) );
		float3		far2		= view * UnProjectNDC( inv_vp, float3(-1.0, -1.0, z) );
		float3		far3		= view * UnProjectNDC( inv_vp, float3( 1.0, -1.0, z) );

		float3		min			= Min( near, Min( Min( far0, far1 ), Min( far2, far3 )) );
		float3		max			= Max( near, Max( Max( far0, far1 ), Max( far2, far3 )) );

		return	AABB_Create( min, max );
	}


	void Main ()
	{
		float3		light_ang	= iLightDir * float_HalfPi + float3(0.5, 1.0, 0.0) * float_Pi;
		float3x3	light_view	= f3x3_RotateX( light_ang.x ) * f3x3_RotateY( light_ang.y ) * f3x3_RotateZ( light_ang.z );

		float		fov			= MaxOf( un_PerPass.camera.fov );	// 'fov.x' for FPS camera, 'MaxOf(fov.xy)' for flight camera
		float		tan_hfov	= Tan( fov * 0.5 );

		AABB		aabb		= CalcShadowBBox( light_view );

		float3		view_pos	= light_view * un_PerPass.camera.pos;
		float2		vp_size		= AABB_Size( aabb ).xy;
		float2		z_range		= float2( aabb.min.z - iShadowZ, aabb.max.z + iShadowZ ) + view_pos.z;
		float		max_size	= iShadowDist * Max( tan_hfov, 1.0 ) * 2.0;
		float		shadow_dist = iShadowDist / Cos( fov * 0.5 );

		ASSERT( AllLess( vp_size, float2(max_size * 1.01) ));

		float2		size		= float2( max_size );
		float2		center		= AABB_Center( aabb ).xy;
					center		= SnapToTexel( view_pos.xy + center, size );

		un_Params.shadowVP = f4x4_Ortho( Rect_FromCenterSize( center, size ), z_range ) * float4x4(light_view) ;
		un_Params.lightDir = Normalize( -GetAxisZ( light_view ));

		un_Params.cameraPos = un_PerPass.camera.pos;
		Frustum_ToCornerPoints( Frustum_Create(un_PerPass.camera.frustum), OUT un_Params.cornerPoints );

		un_Params.shadowSizeWS = max_size;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RESOLVE
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "Matrix.glsl"
	#include "Shadow.glsl"
	#include "ColorSpace.glsl"
	#include "InvocationID.glsl"
	#include "TexSampling.glsl"

	struct ShadowResult
	{
		float	value;
		float2	pxCoord;
	};

	ShadowResult  Shadow ()
	{
		float	depth		= gl.texture.Fetch( un_Depth, int2(gl.FragCoord.xy), 0 ).r;					// non-linear
		float3	normal		= ToSNorm( gl.texture.Fetch( un_Normal, int2(gl.FragCoord.xy), 0 ).rgb );	// world space

		float3	world_pos	= UnProject( un_PerPass.camera.invViewProj, float3( gl.FragCoord.xy, depth ), un_PerPass.invResolution );

		float	norm_bias	= iNormBiasScale * un_Params.shadowSizeWS / float(iShadowDim);
				world_pos	= world_pos + normal * norm_bias;

		float4			sc	= ProjectShadow( un_Params.shadowVP, world_pos + un_PerPass.camera.pos );
		ShadowResult	res;

		if ( ! SampleShadow_IsValidCoord( sc ))
		{
			res.value	= 2.0;
			res.pxCoord	= float2(0.0);
			return res;
		}

		float	shadow		= 0.0;
		float4	moments32	= gl.texture.SampleLod( un_Moments32, sc.xy, 0.0 );
		float4	moments16	= gl.texture.SampleLod( un_Moments16, sc.xy, 0.0 );
		float4	moments		= (iHighPrecision == 1 ? moments32 : moments16);

		switch ( iMode )
		{
			case 0 :	shadow = ExponentialSM( sc.z, iExp.x, moments.r );		break;
			case 1 :	shadow = VarianceSM( sc.z, moments.rg );				break;
			case 2 :	shadow = ExponentialVarianceSM( sc.z, iExp, moments );	break;
			case 3 :	shadow = OriginMomentSM( sc.z, moments );				break;
			case 4 :	shadow = MomentSM( sc.z, moments, 0.0 );				break;
		}

		float	ambient	= 0.2;
		float	n_dot_l	= Max( Dot( normal, un_Params.lightDir ), 0.0 );

		res.value		= Max( shadow * n_dot_l, ambient );
		res.pxCoord		= Floor( sc.xy * float2(iShadowDim) );
		return res;
	}


	void Main ()
	{
		float3			albedo	= gl.texture.Fetch( un_Albedo, int2(gl.FragCoord.xy), 0 ).rgb;
		ShadowResult	shadow	= Shadow();

		float2	uv2 = MapPixCoordToUNormCorrected(
							gl.FragCoord.xy,	// with subpixel offset
							un_PerPass.resolution.xy,
							float2(iShadowDim)
						);

		// shadow map is flipped
		uv2.x = 1.0 - uv2.x;

		out_Color = float4(0.2);

		switch ( iView )
		{
			case 0 :	// color + shadow
				out_Color = float4( albedo * shadow.value * iScale, 1.0 );
				break;

			case 1 :	// shadow only
				out_Color = float2( shadow.value * iScale, 1.0 ).rrrg;
				break;

			case 2 :	// shadow map color
				if ( IsUNorm( uv2 ))
					out_Color = float4( gl.texture.Sample( un_SMColor, uv2 ).rgb * iScale, 1.0 );
				break;

			case 3 :	// shadow map unique pixels
				out_Color = float4( Saturate( DHash32( shadow.pxCoord ) - shadow.value * (iScale - 1.0) ), 1.0 );
				break;
		}
	}

#endif
//-----------------------------------------------------------------------------
