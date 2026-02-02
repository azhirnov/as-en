// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Apply shadow map with deferred shading.
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
		RC<Image>			sm_col			= Image( EPixelFormat::RGBA8_UNorm, sm_dim );
		RC<Image>			sm_col2			= Image( EPixelFormat::RGBA8_UNorm, sm_dim );
		RC<Buffer>			obj_buf			= Buffer();
		RC<Buffer>			cbuf			= Buffer();
		RC<Scene>			scene			= Scene();
		RC<Scene>			scene_sm		= Scene();
		RC<Scene>			scene_fr		= Scene();
		RC<FPVCamera>		camera			= FPVCamera();
		RC<DynamicUInt>		obj_count		= DynamicUInt();

		obj_buf.ArrayLayout(
			"ObjectTransform",
			"	float3	position;"
			"	float3	scale;"
			"	uint	color;",
			obj_count );

		cbuf.UseLayout(
			"ParamsBuffer",
			"	float4x4	shadowVP;"
			"	float3		lightDir;"
			"	float3		cameraPos;"
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

		// create scene with buildings
		{
			array<float3>	positions, normals;
			array<uint>		indices;
			GetCube( OUT positions, OUT normals, OUT indices );

			RC<Buffer>		geom_data = Buffer();
			geom_data.FloatArray( "positions",	positions );
			geom_data.UIntArray(  "indices",	indices );
			geom_data.LayoutName( "GeometryData" );

			{
				RC<UnifiedGeometry>		geometry = UnifiedGeometry();
				geometry.ArgIn( "un_Geometry",	geom_data );
				geometry.ArgIn( "un_Transform",	obj_buf );

				UnifiedGeometry_DrawIndexed	cmd;
				cmd.indexCount	= indices.size();
				cmd.IndexBuffer( geom_data, "indices" );
				cmd.InstanceCount( obj_count );
				cmd.PipelineHint( "opaque.LEqual" );
				geometry.Draw( cmd );

				scene.Add( geometry );
			}{
				RC<UnifiedGeometry>		geometry = UnifiedGeometry();
				geometry.ArgIn( "un_Geometry",	geom_data );
				geometry.ArgIn( "un_Transform",	obj_buf );
				geometry.ArgIn( "un_Params",	cbuf );

				UnifiedGeometry_DrawIndexed	cmd;
				cmd.indexCount	= indices.size();
				cmd.IndexBuffer( geom_data, "indices" );
				cmd.InstanceCount( obj_count );
				geometry.Draw( cmd );

				scene_sm.Add( geometry );
			}
		}

		// create frustum
		{
			RC<Buffer>				geom_data	= Buffer();
			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();
			array<uint>				indices;
			GetFrustumIndices( OUT indices );

			geom_data.UIntArray(  "indices",		indices );
			geom_data.LayoutName( "GeometrySBlock" );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount = indices.size();
			cmd.IndexBuffer( geom_data, "indices" );

			geometry.Draw( cmd );
			geometry.ArgIn( "un_Params",	cbuf );

			scene_fr.Add( geometry );
		}

		Slider( obj_count,	"ObjCount",		100,	400,	100 );
		Slider( sm_pot,		"ShadowDim",	0,		3,		1 );

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
			pass.Slider(	"iLightDir",	float3(-1.0),	float3(1.0),	float3(0.3, 0.0, -0.35) );
			pass.Slider(	"iShadowDist",	1.0,			100.0,			30.0 );
			pass.Slider(	"iShadowZ",		0.0,			100.0,			25.0 );		// or set 'depthClamp=true' in pipeline
			pass.Slider(	"iSnapToTexel",	0,				1 );
			pass.Constant(	"iShadowDim",	sm_dim );
			pass.LocalSize( 1 );
			pass.DispatchThreads( 1 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque" );
			pass.AddPipeline( "samples/StreetLights-Opaque.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights-Opaque.as)
			pass.Output( "out_Color",		rt_col,		RGBA32f(0.0) );
			pass.Output( "out_Normal",		rt_norm,	RGBA32f(0.0) );
			pass.Output(					ds,			DepthStencil(1.0, 0) );
		}{
			RC<SceneGraphicsPass>	pass = scene_sm.AddGraphicsPass( "shadows" );
			pass.AddPipeline( "samples/StreetLights-SM.as" );			// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights-SM.as)
			pass.Output( "out_Color",		sm_col,		RGBA32f(0.0) );
			pass.Output(					sm,			DepthStencil(1.0, 0) );
		//	pass.Slider( "iConstBias",		0.0,	1.0,	0.1 );
		//	pass.Slider( "iSlopeBias",		0.0,	1.0,	0.5 );
		//	pass.Slider( "iBiasClamp",		0.0,	2.0,	0.0 );
		}{
			CopyImage( sm_col, sm_col2 );

			RC<SceneGraphicsPass>	pass = scene_fr.AddGraphicsPass( "dbg frustum" );
			pass.AddPipeline( "samples/DeferredSM-DbgFrustum.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/DeferredSM-DbgFrustum.as)
			pass.Output( "out_Color",		sm_col2 );
			pass.Output(					sm );
		}{
			RC<Postprocess>			pass = Postprocess( "", "RESOLVE" );
			pass.Set( camera );
			pass.Output( "out_Color",		rt,			RGBA32f(0.0) );
			pass.ArgIn( "un_Albedo",		rt_col,		Sampler_NearestClamp );
			pass.ArgIn( "un_Normal",		rt_norm,	Sampler_NearestClamp );
			pass.ArgIn( "un_Depth",			ds,			Sampler_NearestClamp );
			pass.ArgIn( "un_ShadowMap",		sm,			Sampler_NearestClamp );
			pass.ArgIn( "un_SMColor",		sm_col,		Sampler_NearestClamp );
			pass.ArgIn( "un_SMColor2",		sm_col2,	Sampler_NearestClamp );
			pass.ArgIn( "un_Params",		cbuf );
			pass.Slider( "iView",			0,		5,				0 );
			pass.Slider( "iScale",			1.0,	10.0,			1.0 );
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
		const uint			idx				= GetGlobalIndex();
		const uint			street_cnt		= 4;
		const uint			street_idx		= idx % street_cnt;
		const uint			building_idx	= idx / street_cnt;
		const float2		uv				= float2( street_idx, building_idx ) / float2( street_cnt, 1 );

		if ( idx == 0 )
		{
			// floor
			obj.position	= float3(0.0, GROUND_Y + 1.0, 0.0);
			obj.scale		= float2(1000.0, 1.0).xyx;
			obj.color		= packUnorm4x8( float4(0.2) );
			un_Objects.elements[idx] = obj;
			return;
		}

		obj.position.x	= (ToSNorm( uv.x ) + 0.25) * 20.0;
		obj.position.y	= GROUND_Y;
		obj.position.z	= uv.y - 5.0;

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


	float2  SnapToTexel (float2 size)
	{
		const float2	inv_texel_size	= float2(iShadowDim);
		const float2	texel_size		= Rcp( inv_texel_size );

		return Ceil( size * inv_texel_size ) * texel_size;
	}


	AABB  CalcShadowBBox (float3x3 view)
	{
		float4x4	inv_vp		= un_PerPass.camera.invViewProj;
		float		z			= FastProjectZ( un_PerPass.camera.proj, iShadowDist );

		// 'UnProjectNDC' returns world space, camera at (0,0,0)
		// 'view * wp' returns light space

		float3		near		= float3(0.0);

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
		float3x3	view		= f3x3_RotateX( light_ang.x ) * f3x3_RotateY( light_ang.y ) * f3x3_RotateZ( light_ang.z );

		float4x4	tr_mat		= f4x4_Translate( -un_PerPass.camera.pos );

		AABB		aabb		= CalcShadowBBox( view );

		float2		center		= AABB_Center( aabb ).xy;
		float2		size		= float2(MaxOf( AABB_Size( aabb )));

		if ( iSnapToTexel == 1 )
			center = SnapToTexel( center );

		float2		z_range		= float2( aabb.min.z - iShadowZ, aabb.max.z );

		un_Params.shadowVP = f4x4_Ortho( Rect_FromCenterSize( center, size ), z_range ) * float4x4(view) * tr_mat;
		un_Params.lightDir = Normalize( -GetAxisZ( view ));

		un_Params.cameraPos = un_PerPass.camera.pos;
		Frustum_ToCornerPoints( Frustum_Create(un_PerPass.camera.frustum), OUT un_Params.cornerPoints );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW_TO_SM
	#include "Matrix.glsl"
	#include "TexSampling.glsl"

	void Main ()
	{
		float	depth		= gl.texture.Fetch( un_Depth, int2(gl.FragCoord.xy), 0 ).r;			// non-linear
		float3	world_pos	= UnProject( un_PerPass.camera.invViewProj, float3( gl.FragCoord.xy, depth ), un_PerPass.invResolution );
		float4	sc			= ProjectShadow( un_Params.shadowVP, world_pos + un_PerPass.camera.pos );
		float	sm_depth	= gl.texture.Fetch( un_ShadowMap, int2(gl.FragCoord.xy), 0 ).r;		// non-linear

		out_Color = gl.texture.Fetch( un_SMColor, int2(gl.FragCoord.xy), 0 );

		if ( ! SampleShadow_IsValidCoord( sc ))
		{
			out_Color.rgb *= 0.2;
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RESOLVE
	#include "Matrix.glsl"
	#include "ColorSpace.glsl"
	#include "InvocationID.glsl"
	#include "TexSampling.glsl"


	float  Shadow ()
	{
		float	depth		= gl.texture.Fetch( un_Depth, int2(gl.FragCoord.xy), 0 ).r;					// non-linear
		float3	normal		= ToSNorm( gl.texture.Fetch( un_Normal, int2(gl.FragCoord.xy), 0 ).rgb );	// world space

		float3	world_pos	= UnProject( un_PerPass.camera.invViewProj, float3( gl.FragCoord.xy, depth ), un_PerPass.invResolution );
		float4	sc			= ProjectShadow( un_Params.shadowVP, world_pos + un_PerPass.camera.pos );

		if ( ! SampleShadow_IsValidCoord( sc ))
			return 2.0;

		float	shadow		= SampleShadow( un_ShadowMap, sc );
		float	ambient		= 0.2;
		float	n_dot_l		= Max( Dot( normal, un_Params.lightDir ), 0.0 );

		return	Max( shadow * n_dot_l, ambient );
	}


	void Main ()
	{
		float3	albedo	= gl.texture.Fetch( un_Albedo, int2(gl.FragCoord.xy), 0 ).rgb;
		float	shadow	= Shadow();

		float2	uv2 = MapPixCoordToUNormCorrected(
							gl.FragCoord.xy,	// with subpixel offset
							un_PerPass.resolution.xy,
							float2(gl.texture.GetSize( un_SMColor, 0 ))
						);

		// shadow map is filpped
		uv2.x = 1.0 - uv2.x;

		out_Color = float4(0.2);

		switch ( iView )
		{
			case 0 :	// color + shadow
				out_Color = float4( albedo * shadow * iScale, 1.0 );  break;

			case 1 :	// shadow only
				out_Color = float2( shadow * iScale, 1.0 ).rrrg;  break;

			case 2 :	// color only
				out_Color = float4( albedo * iScale, 1.0 );  break;

			case 3 :	// shadow map color
			{
				if ( IsUNorm( uv2 ))
					out_Color = float4( gl.texture.Sample( un_SMColor, uv2 ).rgb * iScale, 1.0 );
				break;
			}

			case 4 :	// shadow map depth
			{
				if ( IsUNorm( uv2 ))
					out_Color = float4( gl.texture.Sample( un_ShadowMap, uv2 ).rrr * iScale, 1.0 );
				break;
			}

			case 5 :	// shadow map color with reprojected view space depth
			{
				if ( IsUNorm( uv2 ))
					out_Color = float4( gl.texture.Sample( un_SMColor2, uv2 ).rgb * iScale, 1.0 );
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
