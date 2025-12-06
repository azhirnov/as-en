// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*

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
		RC<Image>			rt				= Image( EPixelFormat::RGBA8_UNorm, dim );		rt.Name( "Color" );
		RC<Image>			rt2				= Image( EPixelFormat::RGBA8_UNorm, dim );
		RC<Image>			ds				= Image( Supported_DepthFormat(), dim );		ds.Name( "Depth" );
		RC<Image>			wp				= Image( EPixelFormat::RGBA32F, dim );			wp.Name( "WorldPos" );

		RC<FPVCamera>		camera			= FPVCamera();
		RC<Scene>			scene			= Scene();
		RC<Buffer>			obj_buf			= Buffer();

		const uint2			local_size		= uint2( 8, 8 );
		RC<DynamicUInt>		obj_count		= DynamicUInt();
		RC<DynamicUInt2>	count2d			= obj_count.XX().Mul( local_size );
		RC<DynamicUInt>		count			= count2d.Area();

		obj_buf.ArrayLayout(
			"ObjectTransform",
			"	float3	position;" +
			"	float2	scale;" +
			"	uint	color;",
			count );

		// setup camera
		{
			#if 1
				// better precision
				camera.ReverseZ( true );
				camera.ClipPlanes( 0.5f );	// infinite projection
			#else
				camera.ClipPlanes( 1.0f, 100.f );
			#endif

			camera.FovY( 60.f );

			const float	s = 5.0f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );

			scene.Set( camera );
		}

		// create scene with AABBs
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
			cmd.InstanceCount( count );
			geometry.Draw( cmd );

			scene.Add( geometry );
		}

		Slider( obj_count,	"ObjCount",		1,	10,	3 );

		// render loop //
		{
			RC<ComputePass>		pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut( "un_Objects",	obj_buf );
			pass.LocalSize( local_size );
			pass.DispatchThreads( count2d );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "draw scene" );
			pass.AddPipeline( "tests/WorldPosReconstruction.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/tests/WorldPosReconstruction.as)
			pass.Output( "out_Color",		rt,			RGBA32f(0.0) );
			pass.Output( "out_WorldPos",	wp,			RGBA32f(0.0) );
			pass.Output(					ds,			DepthStencil(1.0, 0) );
			pass.Constant( "iLight",		float3(0.4, -1.0, -1.0) );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( camera );
			pass.Output( "out_Color",		rt2 );
			pass.ArgIn(	 "un_Color",		rt,		Sampler_NearestClamp );
			pass.ArgIn(	 "un_Depth",		ds,		Sampler_NearestClamp );
			pass.ArgIn(	 "un_WorldPos",		wp,		Sampler_NearestClamp );
			pass.Slider( "iCmp",			0,	3,		1 );
			pass.Slider( "iCmpDiff",		0,	10,		4 );
		}

		Present( rt2 );
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
	#include "InvocationID.glsl"

	void  Main ()
	{
		ObjectTransform		obj;
		const uint			idx	= GetGlobalIndex();
		const float2		uv	= GetGlobalCoordUNorm().xy;

		obj.position.xy  = ToSNorm( uv ) * 20.0;
		obj.position.xy += ToSNorm( DHash22( uv * 111.0 )) * 2.2;
		obj.position.z   = 10.0 + ToSNorm( DHash12( uv * 222.0 )) * 3.0;

		obj.scale.x = 1.0 + ToSNorm( DHash12( uv * 444.0 )) * 0.2;
		obj.scale.y = 0.6 + DHash12( uv * 234.0 ) * 2.0;

		obj.color = packUnorm4x8( float4( DHash32( uv * 333.0 ), 1.0 ));

		un_Objects.elements[idx] = obj;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Matrix.glsl"

	void  Main ()
	{
		float4	color			= gl.texture.Fetch( un_Color, int2(gl.FragCoord.xy), 0 );
		float	depth			= gl.texture.Fetch( un_Depth, int2(gl.FragCoord.xy), 0 ).r;
		float3	ref_world_pos	= gl.texture.Fetch( un_WorldPos, int2(gl.FragCoord.xy), 0 ).rgb;
		float3	world_pos		= UnProject( un_PerPass.camera.invViewProj, float3( gl.FragCoord.xy, depth ), un_PerPass.invResolution.xy );

		// camera offset is not included to view matrix
		world_pos += un_PerPass.camera.pos;

		if ( depth >= 0.999999 )
			world_pos = float3(0.0);

		switch ( iCmp )
		{
			case 0 :	out_Color = color;  break;
			case 1 :	out_Color = float4( Abs( ref_world_pos - world_pos ) * Exp10( float(iCmpDiff) ), 1.0 );  break;
			case 2 :	out_Color = float4( ref_world_pos, 1.0 ) * 0.1;  break;
			case 3 :	out_Color = float4( world_pos, 1.0 ) * 0.1;  break;
		}
	}

#endif
//-----------------------------------------------------------------------------
