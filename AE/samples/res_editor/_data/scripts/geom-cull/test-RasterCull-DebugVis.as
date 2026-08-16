// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Visualization for Raster Occlusion culling.
	See [OpenGL Scene-Rendering Techniques (Siggraph 2014)](https://web.archive.org/web/20160314160241/http://on-demand.gputechconf.com/siggraph/2014/presentation/SG4117-OpenGL-Scene-Rendering-Techniques.pdf)

	Use 'iShpereId' to show Raster Occlusion culling result (highlighted area - object is visible).
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicDim>		dim				= SurfaceSize();
		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, dim );		rt.Name( "RT" );
		RC<Image>			ds				= Image( Supported_DepthFormat(), dim );		ds.Name( "Depth" );
		RC<Scene>			scene			= Scene();	// indirect draw
		RC<Scene>			scene_aabb		= Scene();
		RC<FPVCamera>		camera			= FPVCamera();
		RC<Buffer>			obj_buf			= Buffer();
		RC<Buffer>			indirect_buf	= Buffer();
		RC<Buffer>			remap_idx		= Buffer();
		RC<Buffer>			vis_flags		= Buffer();

		const uint3			local_size		= uint3( 4, 4, 2 );
		RC<DynamicUInt>		obj_count		= DynamicUInt();
		RC<DynamicUInt3>	count3d			= obj_count.XXX().Mul( local_size );
		RC<DynamicUInt>		count			= count3d.Volume();
		RC<DynamicUInt>		tris_count;
		uint				index_count;

		obj_buf.ArrayLayout(
			"ObjectTransform",
			"	float3	position;"
			"	float	scale;"
			"	uint	color;",
			count );

		remap_idx.ArrayLayout(
			"ObjectIdxRemapping",
			"	uint	newIndex;",
			count );

		vis_flags.ArrayLayout(
			"ObjectIsVisible",
			"	uint	visible;",
			count );

		indirect_buf.UseLayout(
			"DrawCmd",
			"	DrawIndexedIndirectCommand	cmd;" );

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
			scene_aabb.Set( camera );
		}

		// create geometry
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position | EAttribute::Texcoord2D );
			mesh.AddSphere( 8 );

			index_count = mesh.IndexCount();
			@tris_count = count.Mul( index_count/3 );

			RC<Buffer>	geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometryData" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_Transform",	obj_buf );

			UnifiedGeometry_DrawIndexedIndirect		cmd;
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.IndirectBuffer( indirect_buf );
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		// create AABB
		{
			RC<Mesh>	mesh = Mesh();
			mesh.SetAttributes( EAttribute::Position );
			mesh.AddCube();

			RC<Buffer>		geom_data = mesh.ToBuffer();
			geom_data.LayoutName( "GeometryData" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_Transform",	obj_buf );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= mesh.IndexCount();
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.InstanceCount( count );
			geometry.Draw( cmd );

			scene_aabb.Add( geometry );
		}

		Slider( obj_count,	"ObjCount",		1,	10 );

		Label( count,		"Sphere count" );
		Label( tris_count,	"Triangles" );

		// render loop //

		{
			RC<ComputePass>		pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut(	"un_Objects",		obj_buf );
			pass.ArgOut(	"un_IndirectCmd",	indirect_buf );
			pass.ArgOut(	"un_VisFlags",		vis_flags );		// set zero
			pass.Slider(	"iRadius",			0.5,	2.0,	1.0 );
			pass.Constant(	"iIndexCount",		index_count );
			pass.Constant(	"iDimension",		dim );
			pass.LocalSize( local_size );
			pass.DispatchThreads( count3d );
		}

		{
			RC<SceneGraphicsPass>	pass = scene_aabb.AddGraphicsPass( "raster cull" );
			pass.AddPipeline( "perf/Culling/RasterCull.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/RasterCull.as)
			pass.OutputLS(					ds,		EAttachmentLoadOp::Load,	EAttachmentStoreOp::None );
			pass.ArgInOut( "un_VisFlags",	vis_flags );
		}{
			RC<ComputePass>		pass = ComputePass( "", "CHECK_VIS_FLAGS" );
			pass.ArgIn(		"un_VisFlags",		vis_flags );
			pass.ArgOut(	"un_RemapIdx",		remap_idx );		// override
			pass.ArgInOut(	"un_IndirectCmd",	indirect_buf );		// atomic
			pass.LocalSize( local_size );
			pass.DispatchThreads( count3d );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw" );
			pass.AddPipeline( "perf/Culling/1-DepthTest.as" );		// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/1-DepthTest.as)
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.ArgIn(  "un_RemapIdx",	remap_idx );
			pass.Constant( "iLight",	float3(0.4, -1.0, -1.0) );
			pass.Constant( "iRemapIdx",	1 );
		}

		// Raster Occlusion debug visualization
		{
			RC<Postprocess>		pass = Postprocess( "", "DBG_CULL_OBJECTS" );
			pass.Set( camera );
		//	pass.Output(	"out_Color",		rt,		 RGBA32f(0.0) );
			pass.OutputBlend( "out_Color",		rt,			EBlendFactor::One, EBlendFactor::One, EBlendOp::Add );
			pass.ArgIn(		"un_Objects",		obj_buf );
			pass.ArgInOut(  "un_VisFlags",		vis_flags );		// read and set zero
			pass.Slider(	"iSphereId",		0,			100 );
			pass.Slider(	"iSphereIdRange",	0,			100 );	// max 10'000
		}

		RC<DynamicUInt>		vis_objects = DynamicUInt();
		ReadBuffer( vis_objects, indirect_buf, "cmd.instanceCount" );
		Label( vis_objects,	"Visible spheres" );

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef __INTELLISENSE__
#	include <glsl.h>
#	define PUT_OBJECTS
#	define CHECK_VIS_FLAGS
#	define DBG_CULL_OBJECTS
#endif
//-----------------------------------------------------------------------------
#ifdef PUT_OBJECTS
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "Quaternion.glsl"
	#include "IndirectCmd.glsl"
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

		un_Objects.elements[idx] = obj;
		un_VisFlags.elements[idx].visible = 0;

		if ( idx == 0 )
		{
			un_IndirectCmd.cmd = DrawIndexedIndirectCommand_Create( iIndexCount );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CHECK_VIS_FLAGS
	#include "IndirectCmd.glsl"
	#include "InvocationID.glsl"
	#include "Atomic.glsl"

	void  Main ()
	{
		const int	idx			= GetGlobalIndex();
		const bool	is_visible	= (un_VisFlags.elements[ idx ].visible != 0);

		if ( is_visible )
		{
			uint	dst_idx;
			SubgroupAtomicInc( dst_idx, un_IndirectCmd.cmd.instanceCount );

			un_RemapIdx.elements[dst_idx].newIndex = idx;
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef DBG_CULL_OBJECTS
	#include "SDF.glsl"
	#include "Sphere.glsl"
	#include "Matrix.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		const uint	idx = iSphereId + 100*iSphereIdRange;

		if ( idx >= un_Objects.elements.length() )
		{
			out_Color = float4(-1.0);
			return;
		}

		const bool			is_visible		= (un_VisFlags.elements[ idx ].visible != 0);
		ObjectTransform		obj				= un_Objects.elements[ idx ];
		const float3		sphere_center	= (un_PerPass.camera.view * float4(obj.position - un_PerPass.camera.pos, 0.0)).xyz;		// view space
		const float			sphere_radius	= obj.scale;
		const float2		uv				= GetGlobalCoordUNorm().xy;

		Rect	aabb	= Sphere_FastProject( Sphere_Create( sphere_center, sphere_radius ), un_PerPass.camera.proj[0][0], un_PerPass.camera.proj[1][1] );
				aabb	= ToUNorm( aabb );	// uv space

		float2	size	= Rect_Size( aabb );
		float2	center	= Rect_Center( aabb );

		// draw rect
		{
			float2	md	= AA_Helper_fwidth( uv );
			float	sd	= SDF2_Rect( uv - center, size * 0.5 );
			float	x	= MinOf( SmoothStep( float2(Abs(sd)), md*0.5, md*2.0 ));
			float4	bc	= unpackUnorm4x8( obj.color );

			if ( sd < -0.001 and is_visible )
			{
				// object is visible
				out_Color = float4(0.5);
				return;
			}
			out_Color = bc * (1.0 - x);
		}
	}

#endif
//-----------------------------------------------------------------------------
