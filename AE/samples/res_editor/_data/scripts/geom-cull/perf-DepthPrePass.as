// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Compare DepthPrePass with Opaque and AlphaTest, LessEqual and Equal cmp mode.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
	  #if 0
		RC<DynamicDim>		dim					= SurfaceSize();
	  #else
		RC<DynamicUInt>		dim_scale			= DynamicUInt();
		RC<DynamicUInt2>	rt_dim				= DynamicUInt2( uint2(1920, 1080)/2 ).Mul( dim_scale.Exp2().XX() );
		RC<DynamicDim>		dim					= rt_dim.Dimension();

		Slider( dim_scale,	"DimensionScale",	0, 2, 1 );	// 1K, 2K, 4K
		Label(  rt_dim,		"Dimension" );
	  #endif

		RC<Image>			rt					= Image( EPixelFormat::RGBA8_UNorm, dim );	rt.Name( "RT" );
		RC<Image>			ds					= Image( Supported_DepthFormat(), dim );	ds.Name( "Depth" );
		RC<Image>			texture				= Image( EImageType::Float_2D, "shadertoy/GreyNoiseMedium.png" );
		RC<Scene>			scene				= Scene();
		RC<FPVCamera>		camera				= FPVCamera();
		RC<Buffer>			obj_buf				= Buffer();

		const uint3			local_size			= uint3( 4, 4, 2 );
		RC<DynamicUInt>		obj_count			= DynamicUInt();
		RC<DynamicUInt3>	count3d				= obj_count.XXX().Mul( local_size );
		RC<DynamicUInt>		count				= count3d.Volume();
		RC<DynamicUInt>		mode				= DynamicUInt();
		RC<DynamicUInt>		tris_count;
		RC<DynamicUInt>		repeat				= DynamicUInt();
		RC<DynamicFloat3>	light_dir			= DynamicFloat3( float3( 0.4, -1.0, -1.0 ));
		const RGBA32f		bg_color			= RGBA32f(1.0);

		RC<DynamicFloat>	time_cull			= DynamicFloat();
		RC<DynamicFloat>	time_draw			= DynamicFloat();
		RC<DynamicFloat>	total_time			= time_draw.Add( time_cull );

		uint				index_count;

		obj_buf.ArrayLayout(
			"ObjectTransform",
			"	float3	position;"
			"	float	scale;"
			"	uint	color;",
			count );

		array<RC<Image>>	images;
		for (uint i = 0; i < 4; ++i)
		{
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Abstract_1.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Abstract_3.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Wood.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Lichen.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Organic_1.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Organic_2.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Organic_3.jpg" ));
			images.push_back( Image( EImageType::Float_2D, "shadertoy/Organic_4.jpg" ));
		}

		// setup camera
		{
			camera.ClipPlanes( 1.0f, 200.f );
			camera.FovY( 57.f );

			const float	s = 2.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Dimension( dim );

			scene.Set( camera );
		}

		// create geometry
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position | EAttribute::Texcoord2D );
			mesh.AddSphere( 3 );

			index_count = mesh.IndexCount();
			@tris_count = count.Mul( index_count/3 );

			RC<Buffer>	geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometryData" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",		geom_data );
			geometry.ArgIn( "un_Transform",		obj_buf );
			geometry.ArgIn( "un_AlphaTexture",	texture,	Sampler_LinearMipmapClamp );
			geometry.ArgTex( "un_Textures",		images );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= index_count;
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.InstanceCount( count );
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		// render loop //

		// clear timers
		{
			array<RC<DynamicFloat>>		arr = { time_cull, time_draw };
			ResetUnusedTimers( arr );
		}

		// put spheres
		{
			RC<ComputePass>		pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut(	"un_Objects",		obj_buf );
			pass.Slider(	"iBackToFront",		0,		1 );
			pass.Slider(	"iRadius",			0.5,	2.0,	1.0 );
			pass.Constant(	"iIndexCount",		index_count );
			pass.Constant(	"iDimension",		dim );
			pass.LocalSize( local_size );
			pass.DispatchThreads( count3d );
		}

		// depth pre-pass
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "dpp opaque" );
			pass.AddPipeline( "perf/Culling/DPP-Opaque-p0.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/DPP-Opaque-p0.as)
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.EnableIfEqual( mode, 0 );
			pass.MeasureTime( time_cull );
			pass.Repeat( repeat );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "dpp alpha-test" );
			pass.AddPipeline( "perf/Culling/DPP-Alpha-p0.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/DPP-Alpha-p0.as)
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.EnableIfGreater( mode, 0 );
			pass.MeasureTime( time_cull );
			pass.Repeat( repeat );
		}

		// main pass
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque" );
			pass.AddPipeline( "perf/Culling/DPP-Opaque-p1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/DPP-Opaque-p1.as)
			pass.Output( "out_Color",	rt,		bg_color );
			pass.Output(				ds );
			pass.Constant( "iLight",	light_dir );
			pass.EnableIfEqual( mode, 0 );
			pass.MeasureTime( time_draw );
			pass.Repeat( repeat );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "alpha-test le" );
			pass.AddPipeline( "perf/Culling/DPP-Alpha-LE-p1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/DPP-Alpha-LE-p1.as)
			pass.Output( "out_Color",	rt,		bg_color );
			pass.Output(				ds );
			pass.Constant( "iLight",	light_dir );
			pass.EnableIfEqual( mode, 1 );
			pass.MeasureTime( time_draw );
			pass.Repeat( repeat );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "alpha-test eq" );
			pass.AddPipeline( "perf/Culling/DPP-Alpha-Eq-p1.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/DPP-Alpha-Eq-p1.as)
			pass.Output( "out_Color",	rt,		bg_color );
			pass.Output(				ds );
			pass.Constant( "iLight",	light_dir );
			pass.EnableIfEqual( mode, 2 );
			pass.MeasureTime( time_draw );
			pass.Repeat( repeat );
		}


		Slider( mode,		"Mode",			0,	2,	0 );
		Slider( obj_count,	"ObjCount",		1,	10 );
		Slider( repeat,		"Repeat",		1,	30 );

		Label( count,		"Sphere count" );
		Label( tris_count,	"Triangles" );

		Label( total_time.Mul( 1000.f ),	"Total (ms)" );
		Label( time_cull.Mul( 1000.f ),		"DPP (ms)" );
		Label( time_draw.Mul( 1000.f ),		"Draw (ms)" );

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef __INTELLISENSE__
#	include <glsl.h>
#	define PUT_OBJECTS
#endif
//-----------------------------------------------------------------------------
#ifdef PUT_OBJECTS
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "Quaternion.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		ObjectTransform		obj;
		const uint			idx				= GetGlobalIndex();
		const float3		inv_size		= 1.0 / float3(GetGlobalSize());
		const float			aspect_ratio	= float(iDimension.x) / float(iDimension.y);

		obj.scale = 1.0 + DHash11( idx * 11.0 );

		float3	unorm	= GetGlobalCoordUNorm();
		float3	seed	= unorm;

		if ( iBackToFront == 1 )
			unorm.xy = 1.0 - unorm.xy;

		unorm.z   = ToSNorm( unorm.z ) + ToSNorm( DHash13( 111.0 * seed )) * 2.0 * inv_size.z;
		unorm.xy += ToSNorm( DHash23( 222.0 * seed )) * 0.4 * inv_size.xy;

		unorm.xy *= aspect_ratio;
		unorm.z  *= Length( unorm.xy ) * 2.1 + 0.3;
		unorm.xy -= RemapClamp( float2(0.0, 4.0), float2(-0.1, 0.25), aspect_ratio );

		obj.position.xz = unorm.xy * 50.0;
		obj.position.y  = unorm.z * 10.0;

		// emulate LOD
		obj.scale *= Clamp( Length( unorm.xy ) * 1.1 + 0.1, 0.1, 4.0 );

		// change triangle density, required for low-end devices
		obj.scale *= iRadius;

		obj.position = QMul( QRotationY(ToRad(-45.0)), obj.position );

		obj.color = packUnorm4x8( RainbowWrap( float(idx) / 5.0 ));

	  #if 0
		un_Objects.elements[idx] = obj;
	  #else
		// fix for Metal
		un_Objects.elements[idx].position	= obj.position;
		un_Objects.elements[idx].scale		= obj.scale;
		un_Objects.elements[idx].color		= obj.color;
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
