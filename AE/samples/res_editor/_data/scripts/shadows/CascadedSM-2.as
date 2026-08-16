// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Cascaded Shadow Maps.
	Added shadow depth mask to optimize rasterization.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PUT_OBJECTS
#	define SETUP_CSM
#	define RESOLVE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const EPixelFormat	hdr_fmt			= EPixelFormat::RGBA16F;
		RC<DynamicDim>		dim				= SurfaceSize();
		RC<DynamicUInt>		csm_pot			= DynamicUInt();
		RC<DynamicDim>		csm_dim			= csm_pot.Add( 9 ).Exp2().Dimension2();
		const uint			max_cascades	= 8;
		RC<Image>			rt				= Image( hdr_fmt, dim );							rt.Name( "Main-RT" );
		RC<Image>			rt_col			= Image( EPixelFormat::RGB10_A2_UNorm, dim );		rt_col.Name( "Albedo" );
		RC<Image>			rt_norm			= Image( EPixelFormat::RGB10_A2_UNorm, dim );		rt_norm.Name( "Normals" );
		RC<Image>			ds				= Image( Supported_DepthFormat(), dim );			ds.Name( "Depth" );
		RC<Image>			csm				= Image( Supported_DepthFormat(), csm_dim, ImageLayer(max_cascades) );		csm.Name( "CSM" );
		RC<Image>			csm_col			= Image( EPixelFormat::RGBA8_UNorm, csm_dim, ImageLayer(max_cascades) );
		RC<Image>			csm_col2		= Image( EPixelFormat::RGBA8_UNorm, csm_dim, ImageLayer(max_cascades) );
		RC<Buffer>			obj_buf			= Buffer();
		RC<Buffer>			cbuf			= Buffer();
		RC<Scene>			scene			= Scene();
		RC<Scene>			scene_csm		= Scene();
		RC<Scene>			scene_fr		= Scene();
		RC<FPVCamera>		camera			= FPVCamera();
		RC<DynamicUInt>		obj_count		= DynamicUInt();
		RC<DynamicUInt>		num_cascades	= DynamicUInt();

		obj_buf.ArrayLayout(
			"ObjectTransform",
			"	float3	position;"
			"	float3	scale;"
			"	uint	color;",
			obj_count );

		cbuf.UseLayout(
			"ParamsBuffer",
			"	float3		lightDir;"
			"	float3		cameraPos;"
			"	float4x4	cascadesVP		["+max_cascades+"];"
			"	float		splits			["+max_cascades+"];"	// non-linear, clip space
			"	float		blendStart		["+max_cascades+"];"	// non-linear, clip space
			"	float3		cornerPoints	[8];"					// frustum
			"	float3		cascadeFrustum	["+max_cascades+"][8];"
			"	float2		cascadeSize		["+max_cascades+"];"	// world space
			"	int			cascadeCount;"
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
				cmd.InstanceCount( obj_count.Mul( num_cascades ));
				geometry.Draw( cmd );

				scene_csm.Add( geometry );
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
			cmd.InstanceCount( num_cascades );

			geometry.Draw( cmd );
			geometry.ArgIn( "un_Params",	cbuf );

			scene_fr.Add( geometry );
		}

		Slider( obj_count,		"ObjCount",		100,	400,				200 );
		Slider( csm_pot,		"ShadowDim",	0,		3,					1 );
		Slider( num_cascades,	"NumCascades",	1,		max_cascades-1,		3 );

		Label( csm_dim.X(),		"ShadowMap dim" );

		// render loop
		{
			RC<ComputePass>			pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut(	"un_Objects",	obj_buf );
			pass.LocalSize( 64 );
			pass.DispatchThreads( obj_count );
		}{
			RC<ComputePass>			pass = ComputePass( "", "SETUP_CSM" );
			pass.Set( camera );
			pass.ArgInOut(	"un_Params",	cbuf );
			pass.Slider(	"iLightDir",	float3(-1.0),	float3(1.0),	float3(0.3, 0.0, -0.35) );
			pass.Slider(	"iSliceLambda",	0.0,			1.0,			0.5 );
			pass.Slider(	"iBlend",		0.0,			0.5,			0.2 );
			pass.Slider(	"iShadowZ",		0.0,			100.0,			25.0 );		// or set 'depthClamp=true' in pipeline
			pass.Constant(	"iNumCascades",	num_cascades );
			pass.Constant(	"iShadowDim",	csm_dim );
			pass.LocalSize( 1 );
			pass.DispatchThreads( 1 );
			pass.AddFlag( EPassFlags::Enable_ShaderAsserts );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque" );
			pass.AddPipeline( "samples/StreetLights/Opaque.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/Opaque.as)
			pass.Output( "out_Color",		rt_col,		RGBA32f(0.0) );
			pass.Output( "out_Normal",		rt_norm,	RGBA32f(0.0) );
			pass.Output(					ds,			DepthStencil(1.0, 0) );
		}{
			// initialize depth buffer to 0 to cull all pixels.
			// draw frustum to create depth mask with 1 to pass all pixels.
			RC<SceneGraphicsPass>	pass = scene_fr.AddGraphicsPass( "shadow mask" );
			pass.AddPipeline( "samples/StreetLights/CSM-mask.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/CSM-mask.as)
			pass.Output(					csm,		DepthStencil(0.0, 0) );

		}{
			RC<SceneGraphicsPass>	pass = scene_csm.AddGraphicsPass( "shadows" );
			pass.AddPipeline( "samples/StreetLights/CSM.as" );			// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights/CSM.as)
			pass.Output( "out_Color",		csm_col,	RGBA32f(0.0) );
			pass.Output(					csm );
		}{
			CopyImage( csm_col, csm_col2 );

			RC<SceneGraphicsPass>	pass = scene_fr.AddGraphicsPass( "dbg frustum" );
			pass.AddPipeline( "samples/CascadedSM-DbgFrustum.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/CascadedSM-DbgFrustum.as)
			pass.Output( "out_Color",		csm_col2 );
			pass.Output(					csm );
		}{
			RC<Postprocess>			pass = Postprocess( "", "RESOLVE" );
			pass.Set( camera );
			pass.Output( "out_Color",		rt,			RGBA32f(0.0) );
			pass.ArgIn( "un_Albedo",		rt_col,		Sampler_NearestClamp );
			pass.ArgIn( "un_Normal",		rt_norm,	Sampler_NearestClamp );
			pass.ArgIn( "un_Depth",			ds,			Sampler_NearestClamp );
			pass.ArgIn( "un_CSM",			csm,		Sampler_NearestClamp );
			pass.ArgIn( "un_CSMColor",		csm_col,	Sampler_NearestClamp );
			pass.ArgIn( "un_CSMColor2",		csm_col2,	Sampler_NearestClamp );
			pass.ArgIn( "un_Params",		cbuf );
			pass.Slider( "iView",			0,		7,				0 );
			pass.Slider( "iCascade",		0,		max_cascades-1,	0 );
			pass.Slider( "iScale",			1.0,	10.0,			1.0 );
			pass.Constant( "iShadowDim",	csm_dim );
		}

		Present( rt );

		DbgExport( cbuf, "cascaded-sm-params-.h" );
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
#ifdef SETUP_CSM
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


	struct ViewProjZ
	{
		float	viewZ;
		float	projZ;
	};
	ViewProjZ  DepthSlice (float4x4 proj, int cascade)
	{
		// linear + logarithmic
		// also see [ProjCSM](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/shadows/test-ProjCSM.as)
		float2		zrange		= ExtractClipPlanes( proj );
		float		linear_z	= Lerp( zrange.x, zrange.y, float(cascade) / float(iNumCascades) );
		float		log_z		= zrange.x * Pow( zrange.y / zrange.x,  float(cascade) / float(iNumCascades) );
		ViewProjZ	res;
		res.viewZ	= Lerp( linear_z, log_z, iSliceLambda );
		res.projZ	= FastProjectZ( proj, res.viewZ );
		return res;
	}


	AABB  CalcShadowBBox (float3x3 lightView, float2 zRange)
	{
		float4x4	inv_vp		= un_PerPass.camera.invViewProj;

		// 'UnProjectNDC' returns world space, camera at (0,0,0)
		// 'lightView * wp' returns light space

		float3		near0		= lightView * UnProjectNDC( inv_vp, float3(-1.0,  1.0, zRange.x) );
		float3		near1		= lightView * UnProjectNDC( inv_vp, float3( 1.0,  1.0, zRange.x) );
		float3		near2		= lightView * UnProjectNDC( inv_vp, float3(-1.0, -1.0, zRange.x) );
		float3		near3		= lightView * UnProjectNDC( inv_vp, float3( 1.0, -1.0, zRange.x) );

		float3		far0		= lightView * UnProjectNDC( inv_vp, float3(-1.0,  1.0, zRange.y) );
		float3		far1		= lightView * UnProjectNDC( inv_vp, float3( 1.0,  1.0, zRange.y) );
		float3		far2		= lightView * UnProjectNDC( inv_vp, float3(-1.0, -1.0, zRange.y) );
		float3		far3		= lightView * UnProjectNDC( inv_vp, float3( 1.0, -1.0, zRange.y) );

		float3		min			= Min(	Min( Min( near0, near1 ), Min( near2, near3 )),
										Min( Min( far0,  far1  ), Min( far2,  far3  )) );
		float3		max			= Max(	Max( Max( near0, near1 ), Max( near2, near3 )),
										Max( Max( far0,  far1  ), Max( far2,  far3  )) );

		return	AABB_Create( min, max );
	}


	void Main ()
	{
		float3		light_ang	= iLightDir * float_HalfPi + float3(0.5, 1.0, 0.0) * float_Pi;
		float3x3	light_view	= f3x3_RotateX( light_ang.x ) * f3x3_RotateY( light_ang.y ) * f3x3_RotateZ( light_ang.z );
		float4x4	cam_proj	= un_PerPass.camera.proj;
		float		tan_hfov	= Tan( MaxOf( un_PerPass.camera.fov ) * 0.5 );	// 'fov.x' for FPS camera, 'MaxOf(fov.xy)' for flight camera
		Frustum		camera_fr	= Frustum_Create( un_PerPass.camera.frustum );

		// divide on layers (cascades)
		ViewProjZ	prev_vp;
		prev_vp.viewZ = 0.0;
		prev_vp.projZ = 0.0;

		for (int c = 0; c < iNumCascades; ++c)
		{
			ViewProjZ	vp			= DepthSlice( cam_proj, c+1 );
			float		max_size	= vp.viewZ * Max( tan_hfov, 1.0 ) * 2.0;

			AABB		aabb		= CalcShadowBBox( light_view, float2(prev_vp.projZ, vp.projZ) );

			// More detail about stable SM in [DeferredSM](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/shadows/DeferredSM.as)
			// and in [StableCSM](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/shadows/test-StableCSM.as).

			float2		center		= AABB_Center( aabb ).xy;
			float2		size		= float2(max_size);
			float2		vp_size		= AABB_Size( aabb ).xy;		// can be used for scissor

			ASSERT( AllLess( vp_size, size * 1.01 ));

			float3		view_pos	= light_view * un_PerPass.camera.pos;
						center		= SnapToTexel( view_pos.xy + center, size );
			float2		z_range		= float2( aabb.min.z - iShadowZ, aabb.max.z + iShadowZ ) + view_pos.z;

			float		blend_start_vs	= Lerp( prev_vp.viewZ, vp.viewZ, 1.0 - iBlend );

			un_Params.cascadesVP[c]		= f4x4_Ortho( Rect_FromCenterSize( center, size ), z_range ) * float4x4(light_view);
			un_Params.splits[c]			= vp.projZ;
			un_Params.blendStart[c]		= FastProjectZ( cam_proj, blend_start_vs );
			un_Params.cascadeSize[c]	= size;

			ASSERT( un_Params.blendStart[c] <= un_Params.splits[c] );

			Frustum		cascade_fr	= Frustum_ZSlice( camera_fr, 0.0, vp.viewZ );
			Frustum_ToCornerPoints( cascade_fr, OUT un_Params.cascadeFrustum[c] );

			prev_vp = vp;
		}

		un_Params.cascadeCount	= iNumCascades;
		un_Params.lightDir		= Normalize( -GetAxisZ( light_view ));
		un_Params.cameraPos		= un_PerPass.camera.pos;
		Frustum_ToCornerPoints( camera_fr, OUT un_Params.cornerPoints );
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


	int  CascadeIndex (float projZ)
	{
		for (int i = 0; i < un_Params.cascadeCount; ++i)
		{
			if ( projZ < un_Params.splits[i] )
				return i;
		}
		return un_Params.cascadeCount-1;
	}

	struct ShadowResult
	{
		float	value;
		float	cascade;
		float2	pxCoord;
	};

	ShadowResult  Shadow ()
	{
		float	depth		= gl.texture.Fetch( un_Depth, int2(gl.FragCoord.xy), 0 ).r;					// non-linear
		float3	normal		= ToSNorm( gl.texture.Fetch( un_Normal, int2(gl.FragCoord.xy), 0 ).rgb );	// world space
		int		cascade		= CascadeIndex( depth );

		float3	world_pos	= UnProject( un_PerPass.camera.invViewProj, float3( gl.FragCoord.xy, depth ), un_PerPass.invResolution ) + un_PerPass.camera.pos;
		float4	sc			= ProjectShadow( un_Params.cascadesVP[cascade], world_pos );

		ShadowResult	res;

		if ( ! SampleShadow_IsValidCoord( sc ) or depth >= 1.0 )
		{
			res.value	= 2.0;
			res.cascade	= 1.0;
			res.pxCoord	= float2(0.0);
			return res;
		}

		float	shadow		= SampleShadow( un_CSM, sc, float(cascade) );
		float	f_cascade	= float(cascade);

		if ( depth > un_Params.blendStart[cascade] and cascade+1 < un_Params.cascadeCount )
		{
			int		cascade2	= cascade + 1;
			float4	sc2			= ProjectShadow( un_Params.cascadesVP[cascade2], world_pos );
			float	shadow2		= SampleShadow( un_CSM, sc2, float(cascade2) );
			float	blend		= InvLerp( un_Params.blendStart[cascade], un_Params.splits[cascade], depth );

			shadow		= Lerp( shadow, shadow2, blend );
			f_cascade	+= blend;
		}

		float	ambient		= 0.2;
		float	n_dot_l		= Max( Dot( normal, un_Params.lightDir ), 0.0 );

		res.value		= Max( shadow * n_dot_l, ambient );
		res.cascade		= f_cascade / float(un_Params.cascadeCount);
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

		float3	uvw = float3( uv2, Min( iCascade, un_Params.cascadeCount-1 ));

		out_Color = float4(0.2);

		switch ( iView )
		{
			case 0 :	// color + shadow
				out_Color = float4( albedo * shadow.value * iScale, 1.0 );  break;

			case 1 :	// shadow only
				out_Color = Saturate( (1.0 - shadow.value) * iScale ) * Rainbow2( shadow.cascade );  break;

			case 2 :	// color only
				out_Color = float4( albedo * iScale, 1.0 );  break;

			case 3 :	// shadow map color
			{
				if ( IsUNorm( uv2 ))
					out_Color = float4( gl.texture.Sample( un_CSMColor, uvw ).rgb * iScale, 1.0 );
				break;
			}

			case 4 :	// shadow map depth
			{
				if ( IsUNorm( uv2 ))
					out_Color = float4( gl.texture.Sample( un_CSM, uvw ).rrr * iScale, 1.0 );
				break;
			}

			case 5 :	// shadow map color with frustum
			{
				if ( IsUNorm( uv2 ))
					out_Color = float4( gl.texture.Sample( un_CSMColor2, uvw ).rgb * iScale, 1.0 );
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
