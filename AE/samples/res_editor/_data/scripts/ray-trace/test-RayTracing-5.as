// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Draw animated grid using ray query in compute shader.
	Grid vertices updated every frame in compute shader.
	BLAS and TLAS are being rebuild at every frame using indirect command.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	define AE_ray_query
#	include <glsl.h>
#	define GEN_MESH
#	define RAYTRACE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<RTScene>		scene		= RTScene();
		RC<RTGeometry>	geom		= RTGeometry();
		RC<Buffer>		triangles	= Buffer();
		const uint		grid_size	= 64;
		RC<FPVCamera>	camera		= FPVCamera();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 10.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
		}

		// create geometry
		{
			array<float3>	positions;
			array<uint>		indices;
			GetGrid( grid_size, OUT positions, OUT indices );

			for (uint i = 0; i < positions.size(); ++i) {
				positions[i].x = ToSNorm( positions[i].x );
				positions[i].y = ToSNorm( positions[i].y );
			}

			triangles.Uint(			"gridSize",		grid_size );
			triangles.Float(		"invGridSize",	1.f / float(grid_size) );
			triangles.FloatArray(	"positions",	positions );
			triangles.FloatArray(	"normals",		positions );
			triangles.UIntArray(	"indices",		indices );

			geom.AddIndexedTriangles( triangles, triangles );

			scene.AddInstance( geom, float3(0.f, 0.f, 2.f), RTInstanceCustomIndex(0) );
		}

		// render loop
		{
			RC<ComputePass>		upd_verts = ComputePass( "", "GEN_MESH" );
			upd_verts.ArgInOut( "un_Triangles",		triangles );
			upd_verts.ArgInOut( "un_IndirectCmd",	geom.IndirectBuffer() );
			upd_verts.LocalSize( 8, 8 );
			upd_verts.DispatchThreads( grid_size, grid_size );
		}{
			BuildRTGeometryIndirect( geom );
			BuildRTScene( scene );
		}{
			RC<ComputePass>		rtrace = ComputePass( "", "RAYTRACE" );
			rtrace.Set(		camera );
			rtrace.ArgIn(	"un_Triangles",	triangles );
			rtrace.ArgOut(	"un_OutImage",	rt );
			rtrace.ArgIn(	"un_RtScene",	scene );
			rtrace.LocalSize( 8, 8 );
			rtrace.DispatchThreads( rt.Dimension() );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_MESH
	#include "InvocationID.glsl"
	#include "Normal.glsl"
	#include "Waves.glsl"

	ND_ float  WaveAnim (const float2 snorm, const float timeBias)
	{
		const float	wave_width	= 0.2;
		float		wave_r		= 1.5 * Fract( (un_PerPass.time + timeBias) * 0.5 );
		float		wave		= BumpStep( Length(snorm), wave_r, wave_r + wave_width );
		return wave * 0.1;
	}

	ND_ float3  GetPosition (const float2 snorm) {
		return float3( snorm, Max( WaveAnim( snorm, 0.f ), WaveAnim( snorm, 0.5f )) );
	}

	ND_ float3  GetPosition (const float2 snorm, const float2 offset) {
		return GetPosition( snorm + offset );
	}

	ND_ float3  GetNormal (const float2 snorm)
	{
		float3	norm;
		SmoothNormal3x3f( OUT norm, GetPosition, snorm, un_Triangles.invGridSize );
		return norm;
	}

	void  Main ()
	{
		const int	idx		= GetGlobalIndex();
		float2		snorm	= GetGlobalCoordSNorm().xy;

		un_Triangles.positions[idx]	= GetPosition( snorm );
		un_Triangles.normals[idx]	= GetNormal( snorm );

		if ( idx == 0 )
		{
			const int	prim_count = un_Triangles.indices.length() / 3;

			un_IndirectCmd.elements[0].primitiveCount =
				Clamp( int(Abs(Sin(un_PerPass.time * 0.5)) * prim_count + 0.5), 1, prim_count );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RAYTRACE
	#include "InvocationID.glsl"
	#include "HWRayTracing.glsl"

	ND_ float4  MissShader ()
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	ND_ float4  HitShader (const float2 barycentric, const uint primitiveId)
	{
		float3	n0		= un_Triangles.normals[ un_Triangles.indices[ primitiveId * 3 + 0 ]];
		float3	n1		= un_Triangles.normals[ un_Triangles.indices[ primitiveId * 3 + 1 ]];
		float3	n2		= un_Triangles.normals[ un_Triangles.indices[ primitiveId * 3 + 2 ]];
		float3	norm	= BaryLerp( n0, n1, n2, barycentric );
		return float4( ToUNorm(norm), 1.f );
	}

	void  Main ()
	{
		const Ray		ray		= Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );
		const HWRay		hwray	= HWRay_Create( ray, un_PerPass.camera.clipPlanes.y );
		gl::RayQuery	ray_query;
		float4			color;

		RayQuery_Init( ray_query, un_RtScene, hwray );

		while ( gl.rayQuery.Proceed( ray_query ))
		{
			if ( GetCandidateIntersectionType( ray_query ) == gl::RayQueryCandidateIntersection::Triangle )
				gl.rayQuery.ConfirmIntersection( ray_query );
		}

		if ( GetCommittedIntersectionType( ray_query ) == gl::RayQueryCommittedIntersection::None )
		{
			color = MissShader();
		}
		else
		{
			color = HitShader( GetCommittedIntersectionBarycentrics( ray_query ),
							   GetCommittedIntersectionPrimitiveIndex( ray_query ));
		}

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, color );
	}

#endif
//-----------------------------------------------------------------------------
