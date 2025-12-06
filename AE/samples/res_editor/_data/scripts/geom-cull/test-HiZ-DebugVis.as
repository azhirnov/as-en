// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Visualization for HiZ culling.

	Set 'DbgVis=1' and use 'iShpereId' to show AABB and HiZ culling result (red area - culled).
	Increase number of spheres by 'ObjCount' slider to fill whole screen, it increases efficiency of HiZ.

	Use 'iMip' slider to compare 'iMip.x' with 'iMip.y', if mipmap builded incorrectly it will have red areas.
	Use 'iBeginEnd' to setup contrast of depth value.
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
		RC<DynamicDim>		pyramid_dim		= dim.FloorPOT();

		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, dim );						rt.Name( "RT" );
		RC<Image>			ds				= Image( Supported_DepthFormat(), dim );						ds.Name( "Depth" );
		RC<Image>			pyramid			= Image( EPixelFormat::R32F, pyramid_dim, MipmapLevel(~0) );	pyramid.Name( "Depth pyramid" );
		RC<Scene>			scene			= Scene();	// indirect draw
		RC<FPVCamera>		camera			= FPVCamera();
		RC<Buffer>			obj_buf			= Buffer();
		RC<Buffer>			indirect_buf	= Buffer();
		RC<Buffer>			remap_idx		= Buffer();

		const uint3			local_size		= uint3( 4, 4, 2 );
		RC<DynamicUInt>		obj_count		= DynamicUInt();
		RC<DynamicUInt3>	count3d			= obj_count.XXX().Mul( local_size );
		RC<DynamicUInt>		count			= count3d.Volume();
		RC<DynamicUInt>		mode			= DynamicUInt();
		RC<DynamicUInt>		tris_count;
		uint				index_count;
		const bool			has_minmax_sampler = GetFeatureSet().hasSamplerFilterMinmax();

		obj_buf.ArrayLayout(
			"ObjectTransform",
			"	float3	position;" +
			"	float	scale;" +
			"	uint	color;",
			count );

		remap_idx.ArrayLayout(
			"ObjectIdxRemapping",
			"	uint	newIndex;",
			count );

		indirect_buf.UseLayout( "DrawIndexedIndirectCommand" );

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
			array<float3>	positions;
			array<uint>		indices;
			GetSphere( 8, OUT positions, OUT indices );
			index_count = indices.size();

			@tris_count = count.Mul( index_count/3 );

			RC<Buffer>		geom_data = Buffer();
			geom_data.FloatArray( "positions",	positions );
			geom_data.UIntArray(  "indices",	indices );
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

		Slider( mode,		"DbgVis",		0,	1,	1 );
		Slider( obj_count,	"ObjCount",		1,	10 );

		Label( count,		"Sphere count" );
		Label( tris_count,	"Triangles" );

		// render loop //

		{
			RC<ComputePass>		pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut(	"un_Objects",	obj_buf );
			pass.Constant(	"iDimension",	dim );
			pass.Slider(	"iRadius",		0.5,	2.0,	1.0 );
			pass.LocalSize( local_size );
			pass.DispatchThreads( count3d );
		}

		// HiZ Culling
		{
			ClearBuffer( indirect_buf, 0 );

			RC<ComputePass>		pass = ComputePass( "", "CULL_OBJECTS;USE_REDUCTION="+has_minmax_sampler );
			pass.ArgIn(		"un_Objects",		obj_buf );
			pass.ArgInOut(	"un_IndirectCmd",	indirect_buf );
			pass.ArgInOut(	"un_RemapIdx",		remap_idx );
			pass.ArgIn(		"un_DepthPyramid",	pyramid,	(has_minmax_sampler ? Sampler_MaxLinearClamp : Sampler_NearestClamp) );
			pass.Constant(	"iIndexCount",		index_count );
			pass.Constant(	"iPyramidDim",		dim.ToFloat2() );
			pass.Set( camera );
			pass.LocalSize( local_size );
			pass.DispatchThreads( count3d );
		}

		// HiZ
		{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "HiZ" );
			pass.AddPipeline( "perf/Culling/1-DepthTest.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/perf/Culling/1-DepthTest.as)
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.Output(				ds,		DepthStencil(1.0, 0) );
			pass.ArgIn(  "un_RemapIdx",	remap_idx );
			pass.Constant( "iLight",	float3(0.4, -1.0, -1.0) );
			pass.Constant( "iRemapIdx",	1 );
		}

		// HiZ debug visualization
		{
			RC<Postprocess>		pass = Postprocess( "", "DBG_CULL_OBJECTS" );
			pass.Output( "out_Color",			rt,			RGBA32f(0.0) );
			pass.ArgIn(	 "un_Objects",			obj_buf );
			pass.ArgIn(	 "un_DepthPyramid",		pyramid,	Sampler_MaxLinearClamp );
			pass.ArgIn(	 "un_DepthPyramid2",	pyramid,	Sampler_NearestClamp );
			pass.Slider( "iSphereId",			0,				100 );
			pass.Slider( "iSphereIdRange",		0,				100 );	// max 10'000
			pass.Slider( "iBeginEnd",			float2(0.0),	float2(1.0),	float2(0.0, 1.0) );
			pass.Slider( "iMip",				int2(-1),		int2(10),		int2(-1) );
			pass.Constant( "iPyramidDim",		dim.ToFloat2() );
			pass.Set( camera );
			pass.EnableIfEqual( mode, 1 );
		}

		{
			// see [GenHiZ-1](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/GenHiZ-1.as) and [GenHiZ-2](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/GenHiZ-2.as)
			// for simplification reprojection from previous frame is not used

			RC<Postprocess>		pass = Postprocess( "", "MIPMAP_0" );	// non-POT to POT image
			pass.Output( "out_Color",	pyramid );
			pass.ArgIn(  "un_Depth",	ds,		Sampler_NearestClamp );
		}{
			RC<ComputeMip>		pass = ComputeMip( "", "GEN_MIPMAP;USE_REDUCTION="+has_minmax_sampler );
			pass.Variable( "un_InImage",	"un_OutImage",	pyramid,	(has_minmax_sampler ? Sampler_MaxLinearClamp : Sampler_NearestClamp) );
		}

		RC<DynamicUInt>		vis_objects = DynamicUInt();
		ReadBuffer( vis_objects, indirect_buf, "instanceCount" );
		Label( vis_objects,	"Visible spheres" );

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef __INTELLISENSE__
#	include <glsl.h>
#	define PUT_OBJECTS
#	define CULL_OBJECTS
#	define DBG_CULL_OBJECTS
#	define MIPMAP_0
#	define GEN_MIPMAP
#	define USE_REDUCTION	1
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

		un_Objects.elements[idx] = obj;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CULL_OBJECTS
	#include "IndirectCmd.glsl"
	#include "InvocationID.glsl"
	#include "Frustum.glsl"
	#include "Matrix.glsl"
	#include "Sphere.glsl"


	bool  IsVisible (uint idx)
	{
		ObjectTransform		obj				= un_Objects.elements[idx];
		const float3		sphere_center	= (un_PerPass.camera.view * float4(obj.position - un_PerPass.camera.pos, 0.0)).xyz;
		const float			sphere_radius	= obj.scale;
		const float			znear			= un_PerPass.camera.clipPlanes.x;

		// frustum culling
		if ( ! Frustum_IsVisible( un_PerPass.camera.frustum, obj.position - un_PerPass.camera.pos, sphere_radius ))
			return false;

		// see [ProjectSphere test](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/tests/ProjectSphere.as)
		// and [Quad shader](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/ProjectSphere.as)
		if ( sphere_center.z - sphere_radius < znear )
			return true;  // too close to camera

		float4	aabb = Sphere_FastProject( Sphere_Create( sphere_center, sphere_radius ), un_PerPass.camera.proj[0][0], un_PerPass.camera.proj[1][1] );
				aabb = ToUNorm( aabb );	// to uv space

		// see [DepthPyramidCulling test](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/tests/DepthPyramidCulling.as)
		float2	size		= float2( aabb.z - aabb.x, aabb.w - aabb.y ) * iPyramidDim;
		float2	center		= (aabb.xy + aabb.zw) * 0.5;
		float	level		= Ceil( Log2( MaxOf( size )));

	  #if USE_REDUCTION
		float	max_depth	= gl.texture.SampleLod( un_DepthPyramid, center, level ).r;		// normalized clip space
	  #else
		float2	c			= center - 0.5 / float2(Max( int2(iPyramidDim) >> int(level), 1 ));
		float	max_depth	=				  gl.texture.SampleLodOffset( un_DepthPyramid, c, level, int2(0,0) ).r;
				max_depth	= Max( max_depth, gl.texture.SampleLodOffset( un_DepthPyramid, c, level, int2(1,0) ).r );
				max_depth	= Max( max_depth, gl.texture.SampleLodOffset( un_DepthPyramid, c, level, int2(0,1) ).r );
				max_depth	= Max( max_depth, gl.texture.SampleLodOffset( un_DepthPyramid, c, level, int2(1,1) ).r );
	  #endif

		float	sphere_depth = FastProjectZ( un_PerPass.camera.proj, sphere_center.z - sphere_radius );	// normalized clip space
		bool	visible		= sphere_depth <= max_depth;

		return visible;
	}


	void  Main ()
	{
		const uint	idx		= GetGlobalIndex();
		const uint	count	= GetGlobalIndexSize();

		if ( idx == 0 )
		{
			un_IndirectCmd.indexCount = iIndexCount;
		}

		bool	is_visible		= IsVisible( idx );
		uint4	visible_mask	= gl.subgroup.Ballot( is_visible );
		uint	visible_count	= gl.subgroup.BallotBitCount( visible_mask );
		uint	dst_idx			= 0;

		if ( gl.subgroup.Index == 0 )
		{
			dst_idx = gl.AtomicAdd( INOUT un_IndirectCmd.instanceCount, visible_count );
		}
		gl.subgroup.ExecutionBarrier();

		dst_idx = gl.subgroup.Broadcast( dst_idx, 0 );
		dst_idx += gl.subgroup.BallotExclusiveBitCount( visible_mask );

		if ( is_visible )
			un_RemapIdx.elements[dst_idx].newIndex = idx;
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
		if ( iMip.x >= 0 )
		{
			float2	uv			= GetGlobalCoordUNorm().xy;
			float	depth		= gl.texture.SampleLod( un_DepthPyramid2, uv, iMip.x ).r;
			float	ref_depth	= gl.texture.SampleLod( un_DepthPyramid2, uv, iMip.y ).r;

			out_Color.rgb = float3( LinearStep( depth, iBeginEnd.x, iBeginEnd.y ));
			out_Color.a = 1.0;

			if ( iMip.y >= 0 and iMip.y < iMip.x and depth < ref_depth )
			{
				out_Color.r = 1.0;
				out_Color.g = -out_Color.g;
				out_Color.b = -LinearStep( ref_depth, iBeginEnd.x, iBeginEnd.y );
			}
			return;
		}

		const uint	obj_id = iSphereId + 100*iSphereIdRange;

		if ( obj_id >= un_Objects.elements.length() )
		{
			out_Color = float4(-1.0);
			return;
		}

		ObjectTransform		obj				= un_Objects.elements[ obj_id ];
		const float3		sphere_center	= (un_PerPass.camera.view * float4(obj.position - un_PerPass.camera.pos, 0.0)).xyz;		// view space
		const float			sphere_radius	= obj.scale;
		const float			znear			= un_PerPass.camera.clipPlanes.x;
		const float2		uv				= GetGlobalCoordUNorm().xy;

		if ( sphere_center.z + sphere_radius < znear )
		{
			out_Color = float4(-2.0);
			return;
		}

		float4	aabb = Sphere_FastProject( Sphere_Create( sphere_center, sphere_radius ), un_PerPass.camera.proj[0][0], un_PerPass.camera.proj[1][1] );
				aabb = ToUNorm( aabb );	// uv space

		float2	size		= float2( aabb.z - aabb.x, aabb.w - aabb.y );
		float2	center		= (aabb.xy + aabb.zw) * 0.5;
		float	level		= Ceil( Log2( MaxOf( size * iPyramidDim )));

		float	depth		= gl.texture.SampleLod( un_DepthPyramid2, uv, level ).r;

	  #if USE_REDUCTION
		float	max_depth	= gl.texture.SampleLod( un_DepthPyramid, center, level ).r;		// normalized clip space
	  #else
		float2	c			= center - 0.5 / float2(Max( int2(iPyramidDim) >> int(level), 1 ));
		float	max_depth	=				  gl.texture.SampleLodOffset( un_DepthPyramid, c, level, int2(0,0) ).r;
				max_depth	= Max( max_depth, gl.texture.SampleLodOffset( un_DepthPyramid, c, level, int2(1,0) ).r );
				max_depth	= Max( max_depth, gl.texture.SampleLodOffset( un_DepthPyramid, c, level, int2(0,1) ).r );
				max_depth	= Max( max_depth, gl.texture.SampleLodOffset( un_DepthPyramid, c, level, int2(1,1) ).r );
	  #endif

		float3	col			= float3( LinearStep( depth, iBeginEnd.x, iBeginEnd.y ));
		float	sphere_depth = FastProjectZ( un_PerPass.camera.proj, sphere_center.z - sphere_radius );	// normalized clip space

		// draw rect
		{
			float2	md	= AA_Helper_fwidth( uv );
			float	sd	= SDF2_Rect( uv - center, size * 0.5 );
			float	x	= MinOf( SmoothStep( float2(Abs(sd)), md*0.5, md*2.0 ));
			float4	bc	= unpackUnorm4x8( obj.color );

			if ( sd < -0.001 and sphere_depth > max_depth )
			{
				// object is culled
				out_Color = float4(0.5, -sphere_depth, -max_depth, 0.0);
				return;
			}
			col = Lerp( bc.rgb, col, x );
		}

		out_Color.rgb = col;
		out_Color.a   = depth;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MIPMAP_0
	#include "InvocationID.glsl"

	void  Main ()
	{
		int2	dim		= gl.texture.GetSize( un_Depth, 0 );

		float2	uv0		= (GetGlobalCoordUF().xy * un_PerPass.invResolution.xy) * dim;
		float2	uv1		= ((GetGlobalCoordUF().xy + 1.0) * un_PerPass.invResolution.xy) * dim;

		float	d		= -1.0;

		int2	c0		= Max( int2(uv0), int2(0) );
		int2	c1		= Min( int2(uv1), dim-1 );

		for (int y = c0.y; y <= c1.y; ++y)
		for (int x = c0.x; x <= c1.x; ++x)
		{
			d = Max( d, gl.texture.Fetch( un_Depth, int2(x,y), 0 ).r );
		}

		out_Color = float4(d);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_MIPMAP
	#include "InvocationID.glsl"

	void  Main ()
	{
		float4	c;
		#if USE_REDUCTION
			float2	uv = (float2(GetGlobalCoord().xy) + 0.5) / float2(gl.image.GetSize( un_OutImage ));
			c  = gl.texture.SampleLod( un_InImage, uv, 0.0 );
		#else
			int2	p = GetGlobalCoord().xy * 2;
			c = gl.texture.Fetch( un_InImage, p, 0 );
			c = Max( c, gl.texture.Fetch( un_InImage, p + int2(1,0), 0 ));
			c = Max( c, gl.texture.Fetch( un_InImage, p + int2(0,1), 0 ));
			c = Max( c, gl.texture.Fetch( un_InImage, p + int2(1,1), 0 ));
		#endif
		gl.image.Store( un_OutImage, GetGlobalCoord().xy, c );
	}

#endif
//-----------------------------------------------------------------------------
