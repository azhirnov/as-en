// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Find difference between texture array and array of texture.

	results in [Bindless paper](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/Bindless-ru.md)
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

		Slider( dim_scale,	"DimensionScale",	0, 3, 1 );	// 1K, 2K, 4K, 8K
		Label(  rt_dim,		"Dimension" );
	  #endif

		RC<Image>			rt					= Image( EPixelFormat::RGBA8_UNorm, dim );	rt.Name( "RT" );
		RC<Image>			ds					= Image( Supported_DepthFormat(), dim );	ds.Name( "Depth" );

		RC<Scene>			scene0				= Scene();
		RC<Scene>			scene1				= Scene();
		RC<Scene>			scene2				= Scene();
		RC<FPVCamera>		camera				= FPVCamera();
		RC<Buffer>			obj_buf				= Buffer();

		const uint3			local_size			= uint3( 4, 4, 2 );
		RC<DynamicUInt>		obj_count			= DynamicUInt();
		RC<DynamicUInt3>	count3d				= obj_count.XXX().Mul( local_size );
		RC<DynamicUInt>		count				= count3d.Volume();
		RC<DynamicUInt>		mode1				= DynamicUInt();
		RC<DynamicUInt>		mode2				= DynamicUInt();
		RC<DynamicUInt>		mode				= mode1.Add( mode2.Mul(3) );
		RC<DynamicUInt>		tris_count;
		RC<DynamicUInt>		repeat				= DynamicUInt();
		RC<DynamicFloat>	tex_bias			= DynamicFloat();
		const bool			has_minmax_sampler = GetFeatureSet().hasSamplerFilterMinmax();
		bool				low_detail			= true;

		switch ( GPUVendor() )
		{
			case EGPUVendor::NVidia :
			case EGPUVendor::AMD :
				low_detail = false;		break;
		}

		const uint2			img_dim		= uint2(1024);
		const uint			img_count	= 32;
		array<RC<Image>>	images;
		RC<Image>			img_arr		= Image( EPixelFormat::RGBA8_UNorm, img_dim, ImageLayer(img_count), MipmapLevel(~0) );

		{
			for (uint i = 0; i < img_count; ++i){
				images.push_back( Image( EPixelFormat::RGBA8_UNorm, img_dim, MipmapLevel(~0) ));
			}

			RC<Collection>	args = Collection();

			args.Add( "array",	images );
			args.Add( "layers",	img_arr );

			RunScript( "NonUniform-GenImages.as", ScriptFlags::RunOnce, args );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/callable/NonUniform-GenImages.as)
		}

		obj_buf.ArrayLayout(
			"ObjectTransform",
			"	float3	position;" +
			"	float	scale;" +
			"	uint	color;" +
			"	uint	texId;",
			count );

		// setup camera
		{
			camera.ClipPlanes( 1.0f, 200.f );
			camera.FovY( 57.f );

			const float	s = 2.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Dimension( dim );
			scene0.Set( camera );
			scene1.Set( camera );
			scene2.Set( camera );
		}

		// create geometry
		{
			array<float3>	positions;
			array<float2>	uvs;
			array<uint>		indices;
			GetSphere( (low_detail ? 3 : 6), OUT positions, OUT uvs, OUT indices );

			@tris_count = count.Mul( indices.size()/3 );

			RC<Buffer>		geom_data = Buffer();
			geom_data.FloatArray( "positions",	positions );
			geom_data.FloatArray( "uvs",		uvs );
			geom_data.UIntArray(  "indices",	indices );
			geom_data.LayoutName( "GeometryData" );

			{
				RC<UnifiedGeometry>		geometry = UnifiedGeometry();
				geometry.ArgIn( "un_Geometry",		geom_data );
				geometry.ArgIn( "un_Transform",		obj_buf );
				geometry.ArgIn( "un_TextureArr",	img_arr,	Sampler_LinearRepeat );

				UnifiedGeometry_DrawIndexed	cmd;
				cmd.indexCount	= indices.size();
				cmd.IndexBuffer( geom_data, "indices" );
				cmd.InstanceCount( count );
				geometry.Draw( cmd );

				scene0.Add( geometry );
			}{
				RC<UnifiedGeometry>		geometry = UnifiedGeometry();
				geometry.ArgIn( "un_Geometry",		geom_data );
				geometry.ArgIn( "un_Transform",		obj_buf );
				geometry.ArgIn( "un_Textures",		images,		Sampler_LinearRepeat );

				UnifiedGeometry_DrawIndexed	cmd;
				cmd.indexCount	= indices.size();
				cmd.IndexBuffer( geom_data, "indices" );
				cmd.InstanceCount( count );
				geometry.Draw( cmd );

				scene1.Add( geometry );
			}{
				RC<UnifiedGeometry>		geometry = UnifiedGeometry();
				geometry.ArgIn( "un_Geometry",		geom_data );
				geometry.ArgIn( "un_Transform",		obj_buf );
				geometry.ArgTex( "un_Textures",		images );

				UnifiedGeometry_DrawIndexed	cmd;
				cmd.indexCount	= indices.size();
				cmd.IndexBuffer( geom_data, "indices" );
				cmd.InstanceCount( count );
				geometry.Draw( cmd );

				scene2.Add( geometry );
			}
		}

		// render loop //
		{
			RC<ComputePass>		pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut(	"un_Objects",	obj_buf );
			pass.Slider(	"iRadius",		0.5,	3.0,	2.2 );
			pass.Constant(	"iDimension",	dim );
			pass.Constant(  "iTexCount",	img_count );
			pass.LocalSize( local_size );
			pass.DispatchThreads( count3d );
		}

		uint	mode_id = 0;
		{
			RC<SceneGraphicsPass>	pass = scene0.AddGraphicsPass( "tex array" );
			pass.AddPipeline( "perf/NonUniform-Stress/Tex1.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/NonUniform-Stress/Tex1.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iTexBias",	tex_bias );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene1.AddGraphicsPass( "tex bindless" );
			pass.AddPipeline( "perf/NonUniform-Stress/Tex2.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/NonUniform-Stress/Tex2.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iTexBias",	tex_bias );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene2.AddGraphicsPass( "sampler bindless" );
			pass.AddPipeline( "perf/NonUniform-Stress/Samp.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/NonUniform-Stress/Samp.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iTexBias",	tex_bias );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene0.AddGraphicsPass( "per warp tex array" );
			pass.AddPipeline( "perf/NonUniform-Stress/Tex1-PerWarp.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/NonUniform-Stress/Tex1-PerWarp.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iTexBias",	tex_bias );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene1.AddGraphicsPass( "per warp tex bindless" );
			pass.AddPipeline( "perf/NonUniform-Stress/Tex2-PerWarp.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/NonUniform-Stress/Tex2-PerWarp.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iTexBias",	tex_bias );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene2.AddGraphicsPass( "per warp sampler bindless" );
			pass.AddPipeline( "perf/NonUniform-Stress/Samp-PerWarp.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/NonUniform-Stress/Samp-PerWarp.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iTexBias",	tex_bias );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene0.AddGraphicsPass( "per quad tex array" );
			pass.AddPipeline( "perf/NonUniform-Stress/Tex1-PerQuad.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/NonUniform-Stress/Tex1-PerQuad.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iTexBias",	tex_bias );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene1.AddGraphicsPass( "per quad tex bindless" );
			pass.AddPipeline( "perf/NonUniform-Stress/Tex2-PerQuad.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/NonUniform-Stress/Tex2-PerQuad.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iTexBias",	tex_bias );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene2.AddGraphicsPass( "per quad sampler bindless" );
			pass.AddPipeline( "perf/NonUniform-Stress/Samp-PerQuad.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/NonUniform-Stress/Samp-PerQuad.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iTexBias",	tex_bias );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene0.AddGraphicsPass( "per pixel tex array" );
			pass.AddPipeline( "perf/NonUniform-Stress/Tex1-PerPix.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/NonUniform-Stress/Tex1-PerPix.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iTexBias",	tex_bias );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene1.AddGraphicsPass( "per pixel tex bindless" );
			pass.AddPipeline( "perf/NonUniform-Stress/Tex2-PerPix.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/NonUniform-Stress/Tex2-PerPix.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iTexBias",	tex_bias );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}
		++mode_id;
		{
			RC<SceneGraphicsPass>	pass = scene2.AddGraphicsPass( "per pixel sampler bindless" );
			pass.AddPipeline( "perf/NonUniform-Stress/Samp-PerPix.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/NonUniform-Stress/Samp-PerPix.as)
			pass.Output( "out_Color",	rt,		RGBA32f(1.0, 0.0, 0.0, 0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iTexBias",	tex_bias );
			pass.EnableIfEqual( mode, mode_id );
			pass.Repeat( repeat );
		}
		Assert( mode_id == 3*4-1, "mode_id = "+mode_id );

		Slider( mode1,		"BindMode",		0,		2 );	// tex array, bindless tex, bindless tex & sampler
		Slider( mode2,		"GranMode",		0,		3 );	// granularity: per object, per warp, per quad, per pixel
		Slider( obj_count,	"ObjCount",		1,		10,		3 );
		Slider( repeat,		"Repeat",		1,		30 );
		Slider( tex_bias,	"TexBias",		-2.f,	4.f,	0.f );

		Label( count,		"Sphere count" );
		Label( tris_count,	"Triangles" );

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
		obj.texId = idx % iTexCount;

	  #if 0
		un_Objects.elements[idx] = obj;
	  #else
		// fix for Metal
		un_Objects.elements[idx].position	= obj.position;
		un_Objects.elements[idx].color 		= obj.color;
		un_Objects.elements[idx].scale 		= obj.scale;
		un_Objects.elements[idx].texId 		= obj.texId;
	  #endif
	}

#endif
//-----------------------------------------------------------------------------
