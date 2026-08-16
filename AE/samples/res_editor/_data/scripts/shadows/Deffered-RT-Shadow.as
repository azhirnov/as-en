// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Draw scene to GBuffer and then trace shadows.
*/
#ifdef __INTELLISENSE__
#	define AE_RTAS_BUILD
# 	include <res_editor.as>
#	include <glsl.h>
#	define PUT_OBJECTS
#	define SETUP_LIGHT
#	define RT_PASS
#	define RESOLVE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicDim>		dim				= SurfaceSize();
		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, dim );			rt.Name( "Main-RT" );
		RC<Image>			rt_col			= Image( EPixelFormat::RGB10_A2_UNorm, dim );		rt_col.Name( "Albedo" );
		RC<Image>			rt_norm			= Image( EPixelFormat::RGB10_A2_UNorm, dim );		rt_norm.Name( "Normals" );
		RC<Image>			ds				= Image( Supported_DepthFormat(), dim );			ds.Name( "Depth" );
		RC<Image>			sm				= Image( EPixelFormat::R32F, dim );					sm.Name( "ShadowMap" );
		RC<Buffer>			obj_buf			= Buffer();
		RC<Buffer>			cbuf			= Buffer();
		RC<Scene>			scene			= Scene();
		RC<RTScene>			rt_scene		= RTScene();
		RC<FPVCamera>		camera			= FPVCamera();
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
			"	float3	lightDir;"
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
			camera.Position(float3( 0.0, -13.0, 0.0 ));

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

			RC<Buffer>	geom_data	= Buffer();
			uint		pos_off		= geom_data.FloatArray( "position",		positions );
			uint		idx_off		= geom_data.UIntArray(  "indices",		indices );
			geom_data.LayoutName( "GeometryData" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_Transform",	obj_buf );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= indices.size();
			cmd.IndexBuffer( geom_data, "indices" );
			geometry.Draw( cmd );

			scene.Add( geometry );

			RC<RTGeometry>	rt_geom	= RTGeometry();
			rt_geom.AddIndexedTriangles( geom_data, geom_data );
			rt_scene.AddInstance( rt_geom, float3(0.f, 2.0f, 0.f) );
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

			RC<Buffer>	geom_data	= Buffer();
			uint		pos_off		= geom_data.FloatArray( "position",		positions );
			uint		idx_off		= geom_data.UIntArray(  "indices",		indices );
			geom_data.LayoutName( "GeometryData" );

			RC<UnifiedGeometry>		geometry = UnifiedGeometry();
			geometry.ArgIn( "un_Geometry",	geom_data );
			geometry.ArgIn( "un_Transform",	obj_buf );

			UnifiedGeometry_DrawIndexed	cmd;
			cmd.indexCount	= indices.size();
			cmd.IndexBuffer( geom_data, "indices" );
			cmd.firstInstance = 1;
			cmd.instanceCount = obj_count-1;
			geometry.Draw( cmd );

			scene.Add( geometry );

			RC<RTGeometry>	rt_geom	= RTGeometry();
			rt_geom.AddIndexedTriangles( geom_data, geom_data );

			for (uint i = 1; i < obj_count; ++i) {
				rt_scene.AddInstance( rt_geom );
			}
		}
		Assert( rt_scene.InstanceCount() == obj_count );

		// render loop
		{
			RC<ComputePass>			pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut(	"un_Objects",		obj_buf );
			pass.ArgInOut(	"un_RtInstances",	rt_scene.InstanceBuffer() );
			pass.LocalSize( 64 );
			pass.DispatchThreads( obj_count );
		}{
			RC<ComputePass>			pass = ComputePass( "", "SETUP_LIGHT" );
			pass.Set( camera );
			pass.ArgInOut(	"un_Params",	cbuf );
			pass.Slider(	"iLightDir",	float3(-1.0),	float3(1.0),	float3(0.4, 0.0, -0.35) );
			pass.LocalSize( 1 );
			pass.DispatchThreads( 1 );
		}{
			BuildRTScene( rt_scene );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque" );
			pass.AddPipeline( "samples/LowPolyTrees/Opaque.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/LowPolyTrees/Opaque.as)
			pass.Output( "out_Color",		rt_col,		RGBA32f(0.0) );
			pass.Output( "out_Normal",		rt_norm,	RGBA32f(0.0) );
			pass.Output(					ds,			DepthStencil(1.0, 0) );
		}{
			RC<Postprocess>		pass = Postprocess( "", "RT_PASS" );
			pass.Set( camera );
			pass.Output( "out_Shading",		sm );
			pass.ArgIn(  "un_RtScene",		rt_scene );
			pass.ArgIn(  "un_Normal",		rt_norm,	Sampler_NearestClamp );
			pass.ArgIn(  "un_Depth",		ds,			Sampler_NearestClamp );
			pass.ArgIn(  "un_Params",		cbuf );
			pass.Slider( "iNormOffset",		0.0,		1.0,	0.2 );
			pass.Slider( "iLightOffset",	0.0,		1.0,	0.1 );
			pass.Slider( "iDistScale",		0.0,		1.0,	0.2 );
		}{
			RC<Postprocess>			pass = Postprocess( "", "RESOLVE" );
			pass.Set( camera );
			pass.Output( "out_Color",		rt,			RGBA32f(0.0) );
			pass.ArgIn(  "un_Albedo",		rt_col,		Sampler_NearestClamp );
			pass.ArgIn(  "un_Normal",		rt_norm,	Sampler_NearestClamp );
			pass.ArgIn(  "un_Depth",		ds,			Sampler_NearestClamp );
			pass.ArgIn(  "un_Shading",		sm,			Sampler_NearestClamp );
			pass.ArgIn(  "un_Params",		cbuf );
			pass.Slider( "iView",			0,		2,				0 );
			pass.Slider( "iScale",			1.0,	10.0,			3.0 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PUT_OBJECTS
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "Matrix.glsl"
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

			float4x3	tr = float4x3( f3x3_Scale( obj.scale ));
			tr[3] = obj.position;

			un_RtInstances.elements[idx].transform = MatTranspose( tr );
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


		float4x3	tr = float4x3( f3x3_Scale( obj.scale ) * f3x3_RotateY( obj.rotation ));
		tr[3] = obj.position;

		un_RtInstances.elements[idx].transform = MatTranspose( tr );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SETUP_LIGHT
	#include "Matrix.glsl"

	void Main ()
	{
		float3		light_ang	= iLightDir * float_HalfPi + float3(0.5, 1.0, 0.0) * float_Pi;
		float3x3	light_view	= f3x3_RotateX( light_ang.x ) * f3x3_RotateY( light_ang.y ) * f3x3_RotateZ( light_ang.z );

		un_Params.lightDir = Normalize( -GetAxisZ( light_view ));
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RT_PASS
	#include "Matrix.glsl"
	#include "HWRayTracing.glsl"

	ND_ float  CastShadow (const float3 origin, const float3 dir, float tmin, float tmax)
	{
		HWRay	hwray	= HWRay_Create();
		hwray.rayFlags	= gl::RayFlags::Opaque | gl::RayFlags::TerminateOnFirstHit;
		hwray.rayOrigin	= origin;
		hwray.rayDir	= dir;
		hwray.tMin		= tmin;
		hwray.tMax		= tmax;

		gl::RayQuery	ray_query;
		RayQuery_Init( ray_query, un_RtScene, hwray );

		while ( gl.rayQuery.Proceed( ray_query ))
		{}

		return GetCommittedIntersectionType( ray_query ) != gl::RayQueryCommittedIntersection::None ? 0.0 : 1.0;
	}

	void Main ()
	{
		float	depth		= gl.texture.Fetch( un_Depth, int2(gl.FragCoord.xy), 0 ).r;					// non-linear
		float3	world_pos	= UnProject( un_PerPass.camera.invViewProj, float3( gl.FragCoord.xy, depth ), un_PerPass.invResolution );
		float3	normal		= ToSNorm( gl.texture.Fetch( un_Normal, int2(gl.FragCoord.xy), 0 ).rgb );	// world space
		float	view_z		= FastUnProjectZ( un_PerPass.camera.proj, depth );
		float	unorm_z		= view_z / un_PerPass.camera.clipPlanes.y;

		float	norm_bias	= iNormOffset * 1.0e-2 +	// constant part
							  unorm_z * iDistScale;
		float	ray_bias	= iLightOffset * 1.0e-3 +	// constant part
							  unorm_z * iDistScale;

		world_pos += un_PerPass.camera.pos;
		world_pos += normal * norm_bias;

		out_Shading.r = CastShadow( world_pos, un_Params.lightDir, ray_bias, 100.0 );
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


	void Main ()
	{
		float3	albedo		= gl.texture.Fetch( un_Albedo, int2(gl.FragCoord.xy), 0 ).rgb;
		float3	normal		= ToSNorm( gl.texture.Fetch( un_Normal, int2(gl.FragCoord.xy), 0 ).rgb );	// world space
		float	ambient		= 0.2;
		float	n_dot_l		= Max( Dot( normal, un_Params.lightDir ), 0.0 );
		float	shadow		= gl.texture.Fetch( un_Shading, int2(gl.FragCoord.xy), 0 ).r;
		float	shading		= Max( shadow * n_dot_l, ambient );

		out_Color = float4(0.2);

		switch ( iView )
		{
			case 0 :	// color + shadow
				out_Color = float4( albedo * shading * iScale, 1.0 );  break;

			case 1 :	// shadow only
				out_Color = float2( shadow * iScale, 1.0 ).rrrg;  break;

			case 2 :	// color only
				out_Color = float4( albedo * iScale, 1.0 );  break;
		}
	}

#endif
//-----------------------------------------------------------------------------
