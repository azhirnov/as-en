// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Check fast sphere projection.

	paper: [2D Polyhedral Bounds of a Clipped, Perspective-Projected 3D Sphere](https://jcgt.org/published/0002/02/05/paper.pdf)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PUT_OBJECTS
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicDim>		dim				= SurfaceSize();
		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, dim );				rt.Name( "RT" );
		RC<Image>			ds				= Image( Supported_DepthFormat(), dim );				ds.Name( "Depth" );
		RC<Image>			pyramid			= Image( EPixelFormat::R32F, dim/2, MipmapLevel(~0) );	pyramid.Name( "Depth pyramid" );
		RC<Scene>			scene			= Scene();
		RC<Scene>			scene2			= Scene();
		RC<FPVCamera>		camera			= FPVCamera();
		RC<Buffer>			obj_buf			= Buffer();

		const uint3			local_size		= uint3( 4, 4, 2 );
		const uint3			global_size		= local_size * 2;
		const uint			obj_count		= global_size.x * global_size.y * global_size.z;
		RC<DynamicFloat3>	light_dir		= DynamicFloat3();
		RC<DynamicUInt>		quads			= DynamicUInt();

		obj_buf.ArrayLayout(
			"ObjectTransform",
			"	float3	position;" +
			"	float	scale;" +
			"	uint	color;",
			obj_count );

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 300.f );
			camera.FovY( 60.f );

			const float	s = 4.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			camera.Dimension( dim );
			scene.Set( camera );
			scene2.Set( camera );
		}

		// create geometry
		{
			array<float3>	positions;
			array<uint>		indices;
			GetSphere( 8, OUT positions, OUT indices );

			RC<Buffer>		geom_data = Buffer();
			geom_data.FloatArray( "positions",	positions );
			geom_data.UIntArray(  "indices",	indices );
			geom_data.LayoutName( "GeometryData" );

			{
				UnifiedGeometry_DrawIndexed	cmd;
				cmd.indexCount		= indices.size();
				cmd.instanceCount	= obj_count;
				cmd.IndexBuffer( geom_data, "indices" );
				cmd.PipelineHint( "DrawSphere" );

				RC<UnifiedGeometry>		geometry = UnifiedGeometry();
				geometry.ArgIn( "un_Geometry",	geom_data );
				geometry.ArgIn( "un_Transform",	obj_buf );
				geometry.Draw( cmd );
				scene.Add( geometry );
			}{
				UnifiedGeometry_Draw	cmd;
				cmd.vertexCount		= 4;
				cmd.instanceCount	= obj_count;
				cmd.PipelineHint( "DrawQuad" );

				RC<UnifiedGeometry>		geometry = UnifiedGeometry();
				geometry.ArgIn( "un_Geometry",	geom_data );
				geometry.ArgIn( "un_Transform",	obj_buf );
				geometry.Draw( cmd );
				scene2.Add( geometry );
			}
		}

		Slider( quads,		"ShowQuads",	0,	1,	1 );
		Slider( light_dir,	"LightDir",		float3(-1.0),	float3(1.0),	float3(0.4, -1.0, -1.0) );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut( "un_Objects",	obj_buf );
			pass.LocalSize( local_size );
			pass.DispatchThreads( global_size );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "spheres" );
			pass.AddPipeline( "tests/ProjectSphere.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/ProjectSphere.as)
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.Constant( "iLight",	light_dir );
		}{
			RC<SceneGraphicsPass>	pass = scene2.AddGraphicsPass( "quads" );
			pass.AddPipeline( "tests/ProjectSphere.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/ProjectSphere.as)
			pass.Output( "out_Color",	rt );
			pass.Output(				ds );
			pass.Constant( "iLight",	light_dir );
			pass.EnableIfEqual( quads, 1 );
		}

		Present( rt );
	}

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
		const uint			idx			= GetGlobalIndex();
		const float3		inv_size	= 1.0 / float3(GetGlobalSize());

		obj.scale = 1.0 + DHash11( idx * 11.0 );

		float3	unorm = GetGlobalCoordUNorm();

		unorm.z   = ToSNorm( unorm.z ) + ToSNorm( DHash12( 111.0 * unorm.xy )) * inv_size.z;
		unorm.xy += ToSNorm( DHash22( 222.0 * unorm.xy )) * 0.1 * inv_size.xy - 0.2;

		obj.position.xz = unorm.xy * 100.0;
		obj.position.y  = unorm.z * 30.0;

		obj.scale *= Saturate( Length( unorm.xy ) * 2.0 + 0.1 );

		obj.position = QMul( QRotationY(ToRad(-45.0)), obj.position );

		obj.color = packUnorm4x8( RainbowWrap( float(idx) / 5.0 ));

		un_Objects.elements[idx] = obj;
	}

#endif
//-----------------------------------------------------------------------------
