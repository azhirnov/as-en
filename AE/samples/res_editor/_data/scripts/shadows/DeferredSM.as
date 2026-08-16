// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Apply shadow map with deferred shading.

	Used 'depthBiasConstFactor' and 'depthBiasSlopeFactor' to fix self shadowing.
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
		RC<Image>			ds				= Image( Supported_DepthFormat(), dim );			ds.Name( "Depth" );
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
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddCube();

			RC<Buffer>		geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometryData" );

			{
				RC<UnifiedGeometry>		geometry = UnifiedGeometry();
				geometry.ArgIn( "un_Geometry",	geom_data );
				geometry.ArgIn( "un_Transform",	obj_buf );

				UnifiedGeometry_DrawIndexed	cmd;
				cmd.indexCount	= mesh.IndexCount();
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
				cmd.indexCount	= mesh.IndexCount();
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
			pass.Slider(	"iLightDir",	float3(-1.0),	float3(1.0),	float3(0.4, 0.0, -0.35) );
			pass.Slider(	"iShadowDist",	1.0,			100.0,			30.0 );
			pass.Slider(	"iShadowZ",		0.0,			100.0,			25.0 );		// or set 'depthClamp=true' in pipeline
			pass.Slider(	"iStable",		0,				3,				3 );
			pass.Constant(	"iShadowDim",	sm_dim );
			pass.LocalSize( 1 );
			pass.DispatchThreads( 1 );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque" );
			pass.AddPipeline( "samples/StreetLights/Opaque.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/Opaque.as)
			pass.Output( "out_Color",		rt_col,		RGBA32f(0.0) );
			pass.Output( "out_Normal",		rt_norm,	RGBA32f(0.0) );
			pass.Output(					ds,			DepthStencil(1.0, 0) );
		}{
			RC<SceneGraphicsPass>	pass = scene_sm.AddGraphicsPass( "shadows" );
			pass.AddPipeline( "samples/StreetLights/SM.as" );			// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/SM.as)
			pass.Output( "out_Color",		sm_col,		RGBA32f(0.0) );
			pass.Output(					sm,			DepthStencil(1.0, 0) );
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
			pass.Slider( "iView",			0,		7,				0 );
			pass.Slider( "iScale",			1.0,	10.0,			1.0 );
			pass.Constant( "iShadowDim",	sm_dim );
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

		if ( idx >= un_Objects.elements.length() )
			return;

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

		float4x4	tr_mat		= f4x4_Translate( -un_PerPass.camera.pos );
		float		fov			= MaxOf( un_PerPass.camera.fov );	// 'fov.x' for FPS camera, 'MaxOf(fov.xy)' for flight camera
		float		tan_hfov	= Tan( fov * 0.5 );

		AABB		aabb		= CalcShadowBBox( light_view );

		float2		center		= AABB_Center( aabb ).xy;
		float2		size		= AABB_Size( aabb ).xy;
		float2		z_range		= float2( aabb.min.z - iShadowZ, aabb.max.z + iShadowZ );
		float		max_size	= iShadowDist * Max( tan_hfov, 1.0 ) * 2.0;
		float		shadow_dist = iShadowDist / Cos( fov * 0.5 );

		// Virtual shadow map: rectangle with sides 'shadow_dist*2' and camera in center (snaped to texel center).
		// This make pixels stable when rotating or moving camera.
		// Physical shadow map will use part of this virtual texture to save memory.
		// Max size of physical texture is 'max_size'.
		//
		switch ( iStable )
		{
			case 1 :	// stable size
				size = float2( max_size );
				break;

			case 2 :	// stable size and snap local center to texel
				size   = float2( max_size );
				center = SnapToTexel( center, size );
				break;

			case 3 :	// stable size and snap global center to texel
			{
				ASSERT( AllLess( size, float2(max_size * 1.01) ));

				float3 view_pos = light_view * un_PerPass.camera.pos;
				size   = float2( max_size );
				center = SnapToTexel( view_pos.xy + center, size );
				tr_mat = f4x4_Identity();
				z_range += view_pos.z;
				break;
			}
		}

		un_Params.shadowVP = f4x4_Ortho( Rect_FromCenterSize( center, size ), z_range ) * float4x4(light_view) * tr_mat;
		un_Params.lightDir = Normalize( -GetAxisZ( light_view ));

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
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "Matrix.glsl"
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
		float4	sc			= ProjectShadow( un_Params.shadowVP, world_pos + un_PerPass.camera.pos );

		ShadowResult	res;

		if ( ! SampleShadow_IsValidCoord( sc ))
		{
			res.value	= 2.0;
			res.pxCoord	= float2(0.0);
			return res;
		}

		float	shadow		= SampleShadow( un_ShadowMap, sc );
		float	ambient		= 0.2;
		float	n_dot_l		= Max( Dot( normal, un_Params.lightDir ), 0.0 );

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
				out_Color = float4( albedo * shadow.value * iScale, 1.0 );  break;

			case 1 :	// shadow only
				out_Color = float2( shadow.value * iScale, 1.0 ).rrrg;  break;

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

			case 5 :	// shadow map color with frustum
			{
				if ( IsUNorm( uv2 ))
					out_Color = float4( gl.texture.Sample( un_SMColor2, uv2 ).rgb * iScale, 1.0 );
				break;
			}

			case 6 :	// shadow map unique pixels
			{
				out_Color = float4( Saturate( DHash32( shadow.pxCoord ) - shadow.value * (iScale - 1.0) ), 1.0 );
				break;
			}

			case 7 :	// shadow map LOD (pixel density)
			{
				float2	dx		= gl.dFdx( shadow.pxCoord );
				float2	dy		= gl.dFdy( shadow.pxCoord );
				float	Pmax	= Max( Length(dx), Length(dy) );
				float	level	= Log2( Pmax );

				out_Color = Rainbow2( 1.0 - level * iScale / 8.0 );
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
