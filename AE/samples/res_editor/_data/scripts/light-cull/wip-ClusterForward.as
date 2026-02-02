// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Compare techniques for light pass in deferred shading:
	 * light volume
	 * light AABB / billboard
	 * tiled without z-test
	 * tiled with z-test, zbins

	related:
	* [project lights to clusters](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/light-cull/test-LightVolToClusters.as)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PUT_OBJECTS
#	define PUT_LIGHTS
#	define CLUSTER_LIGHT_PASS
#	define RESOLVE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const EPixelFormat	hdr_fmt			= EPixelFormat::RGBA16F;
		RC<DynamicDim>		dim				= SurfaceSize();
		RC<Image>			rt				= Image( hdr_fmt, dim );						rt.Name( "Main-RT" );
		RC<Image>			rt_col			= Image( EPixelFormat::RGB10_A2_UNorm, dim );	rt_col.Name( "Albedo" );
		RC<Image>			rt_norm			= Image( EPixelFormat::RGB10_A2_UNorm, dim );	rt_norm.Name( "Normals" );
		RC<Image>			ds				= Image( Supported_DepthStencilFormat(), dim );	ds.Name( "Depth" );
		RC<Image>			rt_light		= Image( hdr_fmt, dim );						rt_light.Name( "LightBuf" );
		RC<Buffer>			obj_buf			= Buffer();
		RC<Buffer>			clusters_buf	= Buffer();
		RC<Buffer>			lights_buf		= Buffer();
		RC<Scene>			scene			= Scene();
		RC<Scene>			scene_lights	= Scene();
		RC<Scene>			scene_defer_lights1 = Scene();
		RC<Scene>			scene_defer_lights2 = Scene();

		RC<DynamicUInt>		obj_count		= DynamicUInt();
		const int2			tile_size		= int2( 16 );
		RC<DynamicUInt>		depth_slices	= DynamicUInt();
		RC<DynamicUInt>		cluster_count	= dim.DivCeil( tile_size ).Area().Mul( depth_slices );
		RC<DynamicUInt>		light_count		= DynamicUInt();
		RC<DynamicUInt>		mode			= DynamicUInt();

		obj_buf.ArrayLayout(
			"ObjectTransform",
			"	float3	position;"
			"	float3	scale;"
			"	uint	color;",
			obj_count );

		lights_buf.ArrayLayout(
			"LightObject",
			"	float3	position;"
			"	float	angle;"
			"	float3	dir;"
			"	float	height;"
			"	float2	atten;"		// linear, quadratic
			"	uint	color;"
			"	float	brightness;",
			light_count );

		clusters_buf.ArrayLayout(
			"Cluster",
			"	uint	count;"			// atomic
			"	uint	indices [16];",
			cluster_count );

		// setup camera
		{
			RC<FPVCamera>	camera = FPVCamera();

			camera.ClipPlanes( 1.0f );
			camera.FovY( 60.f );
			camera.ReverseZ( true );

			const float	s = 2.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
			scene_lights.Set( camera );
			scene_defer_lights1.Set( camera );
			scene_defer_lights2.Set( camera );
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

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_Transform",	obj_buf );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= indices.size();
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.InstanceCount( obj_count );
			cmd.PipelineHint( "opaque.GEqual|Stencil" );
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		// create scene with lights
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_LightObjs",	lights_buf );
			{
				UnifiedGeometry_Draw	cmd;
				cmd.vertexCount = 4;
				cmd.InstanceCount( light_count );
				cmd.PipelineHint( "light_bulb" );
				geometry.Draw( cmd );
			}{
				UnifiedGeometry_Draw	cmd;
				cmd.vertexCount = 5;
				cmd.InstanceCount( light_count );
				cmd.PipelineHint( "light_cone" );
				geometry.Draw( cmd );
			}
			scene_lights.Add( geometry );
		}

		// create scene with light cones for deferred shading
		{
			array<float3>	positions, normals;
			array<uint>		indices;
			GetCone( 12, 1.0, 1.0, OUT positions, OUT indices );

			RC<Buffer>		geom_data = Buffer();
			geom_data.FloatArray( "positions",	positions );
			geom_data.UIntArray(  "indices",	indices );
			geom_data.LayoutName( "GeometryData" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_LightObjs",	lights_buf );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= indices.size();
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.InstanceCount( light_count );
			geometry.Draw( cmd );

			scene_defer_lights1.Add( geometry );
		}

		// create scene with light billboards for deferred shading
		{
			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_LightObjs",	lights_buf );

			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount = 4;
			cmd.InstanceCount( light_count );
			geometry.Draw( cmd );

			scene_defer_lights2.Add( geometry );
		}

		Slider( obj_count,		"ObjCount",		100,	400,		100 );
		Slider( light_count,	"LightCount",	100,	1000,		200 );
		Slider( depth_slices,	"DepthSlices",	1,		100,		10 );
		Slider( mode,			"Mode",			0,		2 );	// light volumes, light billboards, tiled, tiled depth cull, tiled zbin

		// render loop
		{
			RC<ComputePass>			pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut( "un_Objects",	obj_buf );
			pass.LocalSize( 64 );
			pass.DispatchThreads( obj_count );
		}{
			ClearBuffer( clusters_buf, 0 );

			RC<ComputePass>			pass = ComputePass( "", "PUT_LIGHTS" );
			pass.ArgInOut( "un_Lights",		lights_buf );
			pass.ArgInOut( "un_Clusters",	clusters_buf );
			pass.LocalSize( 64 );
			pass.DispatchThreads( light_count );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque" );
			pass.AddPipeline( "samples/StreetLights-Opaque.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights-Opaque.as)
			pass.Output( "out_Color",		rt_col,		RGBA32f(0.0) );
			pass.Output( "out_Normal",		rt_norm,	RGBA32f(0.0) );
			pass.Output(					ds,			DepthStencil(0.0, 0) );
		}

		// classic deferred
		{
			// not handled case when camera is inside light volume
			RC<SceneGraphicsPass>	pass = scene_defer_lights1.AddGraphicsPass( "light volumes" );
			pass.AddPipeline( "samples/DeferredShading-Volume.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/DeferredShading-Volume.as)
			pass.Output( "out_Color",		rt_light,	RGBA32f(0.0) );
			pass.Output(					ds );
			pass.ArgIn( "un_Albedo",		rt_col,		Sampler_NearestClamp );
			pass.ArgIn( "un_Normal",		rt_norm,	Sampler_NearestClamp );
			pass.ArgIn( "un_Depth",			ds,			Sampler_NearestClamp );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<SceneGraphicsPass>	pass = scene_defer_lights2.AddGraphicsPass( "light quads" );
			pass.AddPipeline( "samples/DeferredShading-Billboard.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/DeferredShading-Billboard.as)
			pass.Output( "out_Color",		rt_light,	RGBA32f(0.0) );
			pass.Output(					ds );
			pass.ArgIn( "un_Albedo",		rt_col,		Sampler_NearestClamp );
			pass.ArgIn( "un_Normal",		rt_norm,	Sampler_NearestClamp );
			pass.ArgIn( "un_Depth",			ds,			Sampler_NearestClamp );
			pass.EnableIfEqual( mode, 1 );
		}

		{
			RC<Postprocess>			pass = Postprocess( "", "RESOLVE" );
			pass.Output( "out_Color",		rt );
			pass.ArgIn( "un_Albedo",		rt_col,		Sampler_NearestClamp );
			pass.ArgIn( "un_LightBuf",		rt_light,	Sampler_NearestClamp );
			pass.Slider( "iView",			0,		2,		0 );	// 0 - combined, 1 - color, 2 - light
			pass.Slider( "iLightScale",		1.0,	100.0,	10.0 );
			pass.ColorSelector( "iAmbient",	RGBA8u(54, 61, 75, 255) );
		}{
			RC<SceneGraphicsPass>	pass = scene_lights.AddGraphicsPass( "translucent" );
			pass.AddPipeline( "samples/StreetLights-LightBulb.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights-LightBulb.as)
			pass.AddPipeline( "samples/StreetLights-LightCone.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/StreetLights-LightCone.as)
			pass.Output( "out_Color",		rt );
			pass.OutputLS(					ds,			EAttachmentLoadOp::Load, EAttachmentStoreOp::None );
			pass.Slider( "iLightSize",		0.01,	1.0,	1.0 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(PUT_OBJECTS) or defined(PUT_LIGHTS)

	#define GROUND_Y	2.0

#endif
//-----------------------------------------------------------------------------
#ifdef PUT_OBJECTS
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"

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
#ifdef PUT_LIGHTS
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"
	#include "Transform.glsl"

	void  GenLight (out LightObject light, const uint idx)
	{
		const uint		street_cnt		= 4u * 2;	// 2 light lines per street
		const uint		street_idx		= idx % street_cnt;
		const uint		building_idx	= idx / street_cnt;
		const float2	uv				= float2( street_idx, building_idx ) / float2( street_cnt, 1.0 );

		light.position.x	= (ToSNorm( uv.x ) + 0.125) * 20.0 + ToSNorm( DHash12( uv.yx * 123.0 )) * 0.5;
		light.position.y	= -1.5 + DHash12( uv * 77.0 );
		light.position.z	= uv.y * 2.0 - 5.0;

		light.dir			= Normalize( float3(0.0, 1.0, 0.0) + float3( ToSNorm( DHash22( uv * 222.0 )), 0.0 ).xzy );

		light.height		= Abs( (GROUND_Y + 2.0 - light.position.y) / light.dir.y );
		light.angle			= float_Pi * 0.5 * (DHash12( uv * 67.2 ) * 0.5 + 0.5);

		light.color			= packUnorm4x8( Rainbow( DHash12( uv * 333.0 ) * 0.2 + 0.1 ));
		light.brightness	= 1.0 + DHash12( uv * 99.0 );
		light.atten			= float2( 0.027, 0.055 );

		un_Lights.elements[ idx ] = light;
	}


	/*void  PutLightToCluster (const LightObject light, const uint lightIdx)
	{
		const float4	view_pos	= WorldPosToViewSpace( light.position );
		const float4	scr_pos		= ViewPosToScreenSpace( view_pos, float4( 0.0, 0.0, un_PerPass.resolution.xy ));
		const uint		z_index		= uint(Saturate( view_pos.z / iMaxDepth ) * iDepthSlices);

		const uint2		tile_idx	= uint2(scr_pos.xy) / iTileSize;
		const uint2		cluster_dim	= DivCeil( uint2(un_PerPass.resolution.xy), iTileSize );

		const uint		cluster_idx	= z_index * cluster_dim.x * cluster_dim.y +
									  tile_idx.y * cluster_dim.x +
									  tile_idx.x;

		if ( cluster_idx >= un_Clusters.elements.length() )
			return;		// TODO: error ?

		const uint		idx = gl.AtomicAdd( INOUT un_Clusters.elements[ cluster_idx ].count, 1u );

		if ( idx >= un_Clusters.elements[ cluster_idx ].indices.length() )
			return;		// TODO: error ?

		un_Clusters.elements[ cluster_idx ].indices[ idx ] = lightIdx;
	}*/


	void Main ()
	{
		const uint	idx = GetGlobalIndex();

		LightObject light;
		GenLight( OUT light, idx );

	//	PutLightToCluster( light, idx );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CLUSTER_LIGHT_PASS
	#include "Matrix.glsl"

	float4  ProcessLights (float3 albedo, float3 surfNormal)
	{
		// TODO
		return float4( 1.0 );
	}


	void  Main ()
	{
		float4		albedo		= gl.texture.Fetch( un_Albedo, int2(gl.FragCoord.xy), 0 );
		float3		norm		= Normalize( ToSNorm( gl.texture.Fetch( un_Normal, int2(gl.FragCoord.xy), 0 ).rgb ));
		float		depth		= gl.texture.Fetch( un_Depth, int2(gl.FragCoord.xy), 0 ).r;  // non-linear
		float		view_depth	= FastUnProjectZ( un_PerPass.camera.proj, depth );

		if ( view_depth < iMaxDepth )
		{
			out_Color = ProcessLights( albedo.rgb, norm );
		}
		else
		{
			float	ndl = Max( Dot( norm, Normalize(iLight) ), 0.0 );
			out_Color = float4( ndl );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RESOLVE
	#include "ColorSpace.glsl"
	#include "ToneMapping.glsl"

	void Main ()
	{
		float3	light	= gl.texture.Fetch( un_LightBuf, int2(gl.FragCoord.xy), 0 ).rgb * iLightScale;
		float3	albedo	= gl.texture.Fetch( un_Albedo, int2(gl.FragCoord.xy), 0 ).rgb;
		float3	ambient	= RemoveSRGBCurve( iAmbient.rgb );

		switch ( iView )
		{
			case 0 :
				out_Color = ApplySRGBCurve( float4( albedo * (light + ambient), 1.0 ));
				break;

			case 1 :
				out_Color = float4( albedo, 1.0 );
				break;

			case 2 :
				out_Color = float4( light, 1.0 );
				if ( Any( IsNaN( light )))
					out_Color = float4(1.0, 0.0, 1.0, 1.0);
				break;
		}
	}

#endif
//-----------------------------------------------------------------------------
