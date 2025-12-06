// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Spectral ray tracing with dispersion in multiple prisms.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	define AE_ray_query
#	define SH_RAY_GEN
#	define AE_HAS_ATOMICS
#	include <glsl.h>
#	define TRACE_GEOMETRY
#	define TONEMAPPING
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		hdr					= Image( EPixelFormat::RGBA16F, SurfaceSize() );		hdr.Name( "HDR" );
		RC<Image>		rt					= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );

		RC<RTScene>		rtrace_scene		= RTScene();
		RC<Buffer>		rtrace_cbuf			= Buffer();
		RC<Buffer>		rtrace_raybuf		= Buffer();

		RC<Scene>		raster_scene		= Scene();
		RC<Buffer>		geom_data			= Buffer();

		const uint		max_initial_rays	= 8;
		const uint		max_ray_depth		= 8;
		const uint		max_recursion		= 28;
		const uint		ray_storage_size	= 256 << 10;

		array<float2>	ior_per_obj;		// min/max index of refraction
		array<ulong>	position_addr;
		array<ulong>	indices_addr;

		// create geometry for ray tracing
		{
			RC<RTGeometry>	geom		= RTGeometry();
			RC<Buffer>		triangles	= Buffer();
			array<float2>	contour		= { float2(-0.25f, -0.25f), float2(-0.25f, 0.25f), float2(0.25f, 0.25f), float2(0.25f, -0.25f) };	// rectangle
			array<float3>	positions;
			array<uint>		indices;
			TriangulateAndExtrude( contour, 0.1f, OUT positions, OUT indices );

			uint	pos_off	= triangles.FloatArray(	"positions",	positions );
			uint	idx_off	= triangles.UIntArray(	"indices",		indices );

			geom.AddIndexedTriangles( triangles, triangles );

			rtrace_scene.AddInstance( geom, RTInstanceTransform( float3(0.f, 0.f, 0.f), float3(ToRad(-90.f), 0.f, 0.f) ));
			ior_per_obj.push_back(float2( 1.5f, 1.4f ));

			position_addr	.push_back( triangles.DeviceAddress() + pos_off );
			indices_addr	.push_back( triangles.DeviceAddress() + idx_off );
			geom_data.AddReference( triangles );
		}

		// create geometry for ray tracing
		{
			RC<RTGeometry>	geom		= RTGeometry();
			RC<Buffer>		triangles	= Buffer();
			array<float2>	contour		= { float2(-0.25f, -0.25f), float2(-0.25f, 0.25f), float2(-0.5f, 0.25f) };	// triangle
			array<float3>	positions;
			array<uint>		indices;
			TriangulateAndExtrude( contour, 0.1f, OUT positions, OUT indices );

			uint	pos_off	= triangles.FloatArray(	"positions",	positions );
			uint	idx_off	= triangles.UIntArray(	"indices",		indices );

			geom.AddIndexedTriangles( triangles, triangles );

			rtrace_scene.AddInstance( geom, RTInstanceTransform( float3(0.f, 0.f, 0.f), float3(ToRad(-90.f), 0.f, 0.f) ));
			ior_per_obj.push_back(float2( 1.7f, 1.6f ));

			position_addr	.push_back( triangles.DeviceAddress() + pos_off );
			indices_addr	.push_back( triangles.DeviceAddress() + idx_off );
			geom_data.AddReference( triangles );
		}

		// create geometry data
		{
			Assert( position_addr.size() == indices_addr.size() );

			geom_data.ULongArray(	"positions",	position_addr );
			geom_data.ULongArray(	"indices",		indices_addr );
		}

		// setup ray tracing data
		{
			rtrace_cbuf.Float(	"light_energy",		1.f );
			rtrace_cbuf.Float(	"light_coneAngle",	ToRad(30.f) * 0.5f );
			rtrace_cbuf.Float(	"light_areaSize",	0.1f );
			rtrace_cbuf.Float(	"map_scale",		1.0f );

			// materials
			rtrace_cbuf.FloatArray( "mtr_ior",		ior_per_obj );
			Assert( ior_per_obj.size() == rtrace_scene.InstanceCount() );

			array<float4>	wl_to_rgb;
			WhiteColorSpectrumStep50nm( OUT wl_to_rgb, /*normalized*/true );
			//wl_to_rgb.push_back( float4( 550.f, 1.f, 1.f, 1.f ));	// for debugging

			rtrace_cbuf.FloatArray( "wavelengthToRGB",	wl_to_rgb );
			rtrace_cbuf.LayoutName( "RTConstants" );

			rtrace_raybuf.ArrayLayout(
				// dynamic part:
				//   LightCone elements []
				"LightCone",
				"	float2	origin0;" +
				"	float2	origin1;" +
				"	float2	dir0;" +
				"	float2	dir1;" +
				"	float2	energy;" +
				"	float	wavelength;" +
				"	float	ior;",
				// static part:
				"uint	coneCount;" +	// atomic
				"uint	rayCount;" +	// atomic
				"uint2	rayToCone [" + (ray_storage_size / max_ray_depth) + "];",	// [0] - index in 'elements', [1] - count
				ray_storage_size );

			RC<UnifiedGeometry>		geometry	= UnifiedGeometry();

			UnifiedGeometry_Draw	cmd;
			cmd.vertexCount		= max_ray_depth;
			cmd.instanceCount	= ray_storage_size / max_ray_depth;

			geometry.Draw( cmd );
			geometry.ArgIn(	"un_RayStorage",	rtrace_raybuf );
			geometry.ArgIn(	"un_Constants",		rtrace_cbuf );

			raster_scene.Add( geometry );
		}

		RC<DynamicUInt>	dbg_mode = DynamicUInt();
		Slider( dbg_mode, "DbgMode", 0, 1, 0 );

		// render loop
		{
			ClearBuffer( rtrace_raybuf, 0, 8, 0 );	// reset counter

			RC<RayTracingPass>		pass = RayTracingPass();
			pass.ArgIn(		"un_RtScene",		rtrace_scene );
			pass.ArgIn(		"un_Constants",		rtrace_cbuf );
			pass.ArgOut(	"un_RayStorage",	rtrace_raybuf );
			pass.ArgIn(		"un_Geometry",		geom_data );
			pass.ArgIn(		"un_Surface",		rt );
			pass.Slider(	"iLightPos",		float2(-1.f),	float2(1.f),		float2(0.75f, 0.1f) );
			pass.Slider(	"iLightDir",		0.f,			360.f,				162.f );
			pass.Slider(	"iLightShape",		0,				2,					1 );
			pass.Slider(	"iRayDepth",		1,				max_ray_depth-1,	max_ray_depth/2 );
			pass.Slider(	"iRecursionDepth",	1,				max_recursion,		max_recursion/4 );
			pass.Dispatch( max_initial_rays );

			// setup SBT
			pass.RayGen( RTShader("") );
			pass.Callable( CallableIndex(0), RTShader("", "MAX_LOCAL_STORAGE=" + max_ray_depth) );

			pass.MaxCallableRecursion( max_recursion );
		}{
			RC<ComputePass>		pass = ComputePass( "", "TRACE_GEOMETRY" );
			pass.ArgIn(		"un_RtScene",		rtrace_scene );
			pass.ArgIn(		"un_Constants",		rtrace_cbuf );
			pass.ArgOut(	"un_Surface",		rt );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
			pass.EnableIfEqual( dbg_mode, 1 );
		}{
			RC<SceneGraphicsPass>	draw = raster_scene.AddGraphicsPass( "draw area" );
			draw.AddPipeline( "samples/Dispersion2D-area.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Dispersion2D-area.as)
			draw.Output( "out_Color", hdr, RGBA32f().OpaqueBlack() );
			draw.EnableIfEqual( dbg_mode, 0 );
		}{
			RC<Postprocess>		pass = Postprocess( EPostprocess::None, "TONEMAPPING" );
			pass.ArgIn(	 "un_HDR",			hdr,	Sampler_LinearClamp );
			pass.Output( "out_Color",		rt );
			pass.Slider( "iTonemapping",	0,		2 );
			pass.Slider( "iScale",			0.2,	100.0,	1.0 );
			pass.EnableIfEqual( dbg_mode, 0 );
		}{
			RC<SceneGraphicsPass>	draw = raster_scene.AddGraphicsPass( "draw rays" );
			draw.AddPipeline( "samples/Dispersion2D-rays.as" );	// [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/samples/Dispersion2D-rays.as)
			draw.Output( "out_Color", rt );
			draw.EnableIfEqual( dbg_mode, 1 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef __INTELLISENSE__
	struct LightCone
	{
		float2	origin0;
		float2	origin1;
		float2	dir0;
		float2	dir1;
		float2	energy;			// for each ray
		float	wavelength;
		float	ior;
	};
#endif
//-----------------------------------------------------------------------------
#if defined(SH_RAY_GEN) or defined(SH_RAY_CALL)
	#include "Math.glsl"

	struct Payload
	{
		LightCone	cone;
		uint		recursion;
	};

	const float		c_SmallOffset	= 0.0001;
	const float		c_MaxRayLen		= 10.0;
	const float		c_VacuumIOR		= 1.f;

	float3  ToV3 (const float2 v)			{ return float3(v.x, v.y, 0.f); }
	float2  ToV2 (const float3 v)			{ return v.xy; }

	float	WavelengthToUNorm (float wl)	{ return Saturate( (wl - 400.f) / (700.f - 400.f) ); }
	float	MaterialIOR (uint id, float wl)	{ return Lerp( un_Constants.mtr_ior[ id ].x, un_Constants.mtr_ior[ id ].y, WavelengthToUNorm( wl )); }
#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_GEN
	#include "InvocationID.glsl"
	#include "Matrix.glsl"
	#include "Geometry.glsl"
	#include "HWRayTracing.glsl"

	layout(location=0) gl::CallableData Payload  payload;


	int  GetCurrentMtr (const float2 origin, const float2 dir)
	{
		gl::RayQuery	ray_query;
		gl.rayQuery.Initialize( ray_query, un_RtScene, gl::RayFlags::Opaque,
							    /*cullMask*/0xFF, ToV3(origin),
							    /*Tmin*/0.0f, ToV3(dir), /*Tmax*/c_MaxRayLen );

		while ( gl.rayQuery.Proceed( ray_query ))
		{
			if ( GetCandidateIntersectionType( ray_query ) == gl::RayQueryCandidateIntersection::Triangle )
				gl.rayQuery.ConfirmIntersection( ray_query );
		}

		// if intersected with back face
		if ( GetCommittedIntersectionType( ray_query ) != gl::RayQueryCommittedIntersection::None  and
			 ! GetCommittedIntersectionFrontFace( ray_query ))
		{
			return GetCommittedIntersectionInstanceId( ray_query );
		}
		return -1;
	}


	void  InitLightSource (out LightCone cone)
	{
		const float2	map_scale	= float2(un_Constants.map_scale);
		const float		snorm_pos0	= ToSNorm( float(GetGlobalCoord().x+0) / float(GetGlobalSize().x) ) * 0.5;
		const float		snorm_pos1	= ToSNorm( float(GetGlobalCoord().x+1) / float(GetGlobalSize().x) ) * 0.5;

		switch ( iLightShape )
		{
			// cone
			case 0 :
				cone.dir0		= GetDirection2D( ToRad( iLightDir ) + un_Constants.light_coneAngle * snorm_pos0 );
				cone.dir1		= GetDirection2D( ToRad( iLightDir ) + un_Constants.light_coneAngle * snorm_pos1 );
				cone.origin0	= iLightPos * map_scale;
				cone.origin1	= cone.origin0;
				break;

			// area
			case 1 :
				cone.dir0		= GetDirection2D( ToRad( iLightDir ));
				cone.dir1		= cone.dir0;
				cone.origin0	= (iLightPos + LeftVector( cone.dir0 ) * un_Constants.light_areaSize * snorm_pos0) * map_scale;
				cone.origin1	= (iLightPos + LeftVector( cone.dir1 ) * un_Constants.light_areaSize * snorm_pos1) * map_scale;
				break;

			// sphere
			case 2 :
				cone.dir0		= GetDirection2D( snorm_pos0 * float_Pi2 );
				cone.dir1		= GetDirection2D( snorm_pos1 * float_Pi2 );
				cone.origin0	= iLightPos * map_scale;
				cone.origin1	= cone.origin0;
				break;
		}

		cone.origin0	+= cone.dir0 * c_SmallOffset;
		cone.origin1	+= cone.dir1 * c_SmallOffset;
		cone.energy		= float2(1.f);
	}


	void Main ()
	{
		LightCone	cone;
		InitLightSource( OUT cone );

		int	mtr_id = GetCurrentMtr( cone.origin0, cone.dir0 );

		for (uint i = 0; i < un_Constants.wavelengthToRGB.length(); ++i)
		{
			cone.wavelength		= un_Constants.wavelengthToRGB[i].x;

			if ( mtr_id >= 0 )
				cone.ior = MaterialIOR( mtr_id, cone.wavelength );
			else
				cone.ior = c_VacuumIOR;

			payload.cone		= cone;
			payload.recursion	= 0;

			gl.ExecuteCallable( 0, 0 );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_RAY_CALL
	#include "Intersectors.glsl"
	#include "Normal.glsl"
	#include "HWRayTracing.glsl"
	#include "PBR.glsl"

	layout(location=0) gl::CallableDataIn Payload  payload;

	layout(std430, buffer_reference) buffer readonly PositionsRef	{ float3	positions []; };
	layout(std430, buffer_reference) buffer readonly IndicesRef		{ uint		indices	[]; };

	struct HitParams
	{
		float	t;
		int		mtrId;
		bool	frontFace;
		float2	normal;
	};

	LightCone	s_LocalStorage [MAX_LOCAL_STORAGE+1];
	uint		s_RayIndex		= 0;
	uint		c_Recursion;


	bool  IsValid (const LightCone cone)
	{
		return All(bool4( IsNotZero( cone.energy ), IsNormalized( cone.dir0 ), IsNormalized( cone.dir1 )));
	}


	bool  RayClosestHit (const float2 origin, const float2 dir, out HitParams hit)
	{
					hit.mtrId	= -1;
					hit.t		= c_MaxRayLen;
		int			prim_id		= -1;
		float3x3	normal_mat;
		{
			gl::RayQuery	ray_query;

			// Find front face.
			// For layered material we need the second layer material.
			gl.rayQuery.Initialize( ray_query, un_RtScene, gl::RayFlags::Opaque | gl::RayFlags::CullBackFacingTriangles,
									/*cullMask*/0xFF, ToV3(origin),
									/*Tmin*/0.0f, ToV3(dir), /*Tmax*/c_MaxRayLen );

			while ( gl.rayQuery.Proceed( ray_query ))
			{
				if ( GetCandidateIntersectionType( ray_query ) == gl::RayQueryCandidateIntersection::Triangle )
					gl.rayQuery.ConfirmIntersection( ray_query );
			}

			if ( GetCommittedIntersectionType( ray_query ) != gl::RayQueryCommittedIntersection::None )
			{
				hit.mtrId		= GetCommittedIntersectionInstanceId( ray_query );
				prim_id			= GetCommittedIntersectionPrimitiveIndex( ray_query );
				normal_mat		= float3x3( GetCommittedIntersectionObjectToWorld3x4( ray_query ));
				hit.t			= GetCommittedIntersectionT( ray_query );
				hit.frontFace	= true;
			}

			// Find back face.
			//
			gl.rayQuery.Initialize( ray_query, un_RtScene, gl::RayFlags::Opaque | gl::RayFlags::CullFrontFacingTriangles,
									/*cullMask*/0xFF, ToV3(origin),
									/*Tmin*/0.0f, ToV3(dir), /*Tmax*/c_MaxRayLen );

			while ( gl.rayQuery.Proceed( ray_query ))
			{
				if ( GetCandidateIntersectionType( ray_query ) == gl::RayQueryCandidateIntersection::Triangle )
					gl.rayQuery.ConfirmIntersection( ray_query );
			}

			if ( GetCommittedIntersectionType( ray_query ) != gl::RayQueryCommittedIntersection::None  and
				 hit.t > GetCommittedIntersectionT( ray_query ))
			{
				hit.mtrId		= GetCommittedIntersectionInstanceId( ray_query );
				prim_id			= GetCommittedIntersectionPrimitiveIndex( ray_query );
				normal_mat		= float3x3( GetCommittedIntersectionObjectToWorld3x4( ray_query ));
				hit.t			= GetCommittedIntersectionT( ray_query );
				hit.frontFace	= false;
			}
		}

		if ( hit.mtrId >= 0 )
		{
			PositionsRef	pos_addr	= PositionsRef( un_Geometry.positions[ hit.mtrId ]);
			IndicesRef		idx_addr	= IndicesRef( un_Geometry.indices[ hit.mtrId ]);
			const uint		idx			= prim_id * 3;
			float2			obj_norm	= ToV2( ComputeNormal( pos_addr.positions[ idx_addr.indices[ idx+0 ]],
															   pos_addr.positions[ idx_addr.indices[ idx+1 ]],
															   pos_addr.positions[ idx_addr.indices[ idx+2 ]] )
												* normal_mat );
			hit.normal		= Normalize( hit.frontFace ? obj_norm : -obj_norm );
			return true;
		}
		return false;
	}


	bool  ConeClosestHit ()
	{
		LightCone		cone	= s_LocalStorage[s_RayIndex];
		HitParams		r0_params;
		HitParams		r1_params;
		bool			r0_hit	= RayClosestHit( cone.origin0, cone.dir0, OUT r0_params );
		bool			r1_hit	= RayClosestHit( cone.origin1, cone.dir1, OUT r1_params );

		if ( All2( r0_hit, r1_hit )							and
			 (r0_params.frontFace == r1_params.frontFace)	and
			 (r0_params.mtrId == r1_params.mtrId)			)
		{
			const float2	begin0	= cone.origin0;
			const float2	begin1	= cone.origin1;
			const float2	end0	= begin0 + cone.dir0 * r0_params.t;
			const float2	end1	= begin1 + cone.dir1 * r1_params.t;
			float			eta_i,	eta_t;	// incident, transmitted

			if ( r0_params.frontFace ) {
				eta_i	= cone.ior;
				eta_t	= MaterialIOR( r0_params.mtrId, cone.wavelength );
			}else{
				eta_i	= cone.ior;
				eta_t	= c_VacuumIOR;
			}

			const float		eta		= eta_i / eta_t;
			const float2	fresnel	= float2( FresnelDielectric( Dot( cone.dir0, -r0_params.normal ), eta ),
											  FresnelDielectric( Dot( cone.dir1, -r1_params.normal ), eta ));

			// cone rays must not intersects
			const bool		cone_valid	= ! Line_Line_Intersects( begin0, end0, begin1, end1 );

			// both rays must have 0s or 1s or non of them
			bool	fr_valid	= All(bool4( Greater( fresnel, float2(0.0) ), Less( fresnel, float2(1.0) )));
			fr_valid = fr_valid or All(IsZero( fresnel ));
			fr_valid = fr_valid or All(IsZero( fresnel - 1.0 ));

			// reflection
			if ( cone_valid and fr_valid and c_Recursion < iRecursionDepth )
			{
				payload.cone			= cone;
				payload.cone.energy		*= fresnel;
				payload.recursion		= c_Recursion;

				payload.cone.dir0		= Normalize( Reflect( cone.dir0, r0_params.normal ));
				payload.cone.dir1		= Normalize( Reflect( cone.dir1, r1_params.normal ));
				payload.cone.origin0	= end0 + r0_params.normal * c_SmallOffset;
				payload.cone.origin1	= end1 + r1_params.normal * c_SmallOffset;
				payload.cone.ior		= eta_i;

				gl.ExecuteCallable( 0, 0 );
			}

			// refraction
			if ( cone_valid and fr_valid )
			{
				cone.dir0		= Normalize( Refract( cone.dir0, r0_params.normal, eta ));
				cone.dir1		= Normalize( Refract( cone.dir1, r1_params.normal, eta ));
				cone.origin0	= end0 - r0_params.normal * c_SmallOffset;
				cone.origin1	= end1 - r1_params.normal * c_SmallOffset;
				cone.energy		*= (1.0 - fresnel);
				cone.ior		= eta_t;

				s_LocalStorage[++s_RayIndex] = cone;

				if ( ! IsValid( cone ))
					return false;	// total internal reflection

				return true;
			}

			// div cone on 2 parts if rays are intersects.
		}

		// reset
		cone = s_LocalStorage[s_RayIndex];

		// div cone by 2 and continue
		if ( Any(bool2( r0_hit, r1_hit )) and c_Recursion < iRecursionDepth )
		{
			const float	e_scale = 0.5;

			// left
			payload.cone.origin0	= cone.origin0;
			payload.cone.origin1	= Lerp( cone.origin0, cone.origin1, 0.5 );
			payload.cone.dir0		= cone.dir0;
			payload.cone.dir1		= Normalize( Lerp( cone.dir0, cone.dir1, 0.5 ));
			payload.cone.energy.x	= cone.energy.x * e_scale;
			payload.cone.energy.y	= Lerp( cone.energy.x, cone.energy.y, 0.5 ) * e_scale;
			payload.cone.wavelength	= cone.wavelength;
			payload.cone.ior		= cone.ior;
			payload.recursion		= c_Recursion;

			gl.ExecuteCallable( 0, 0 );

			// right
			payload.cone.origin0	= Lerp( cone.origin0, cone.origin1, 0.5 );
			payload.cone.origin1	= cone.origin1;
			payload.cone.dir0		= Normalize( Lerp( cone.dir0, cone.dir1, 0.5 ));
			payload.cone.dir1		= cone.dir1;
			payload.cone.energy.x	= Lerp( cone.energy.x, cone.energy.y, 0.5 ) * e_scale;
			payload.cone.energy.y	= cone.energy.y * e_scale;
			payload.cone.wavelength	= cone.wavelength;
			payload.cone.ior		= cone.ior;
			payload.recursion		= c_Recursion;

			gl.ExecuteCallable( 0, 0 );
		}

		// no intersections, continue cone to the end of viewport
		if ( All2( !r0_hit, !r1_hit ))
		{
			float2		t0_min_max, t1_min_max;

			if ( Rect_Ray_Intersect( float2(un_Constants.map_scale), cone.dir0, cone.origin0, OUT t0_min_max ) and
				 Rect_Ray_Intersect( float2(un_Constants.map_scale), cone.dir1, cone.origin1, OUT t1_min_max ))
			{
				cone.origin0 += Max( t0_min_max.x, t0_min_max.y ) * cone.dir0;
				cone.origin1 += Max( t1_min_max.x, t1_min_max.y ) * cone.dir1;

				s_LocalStorage[++s_RayIndex] = cone;
			}
		}
		return false;
	}


	void Main ()
	{
		if ( ! IsValid( payload.cone ))
			return;

		s_LocalStorage[s_RayIndex]	= payload.cone;
		c_Recursion					= payload.recursion + 1;

		const uint	ray_depth = Min( iRayDepth, s_LocalStorage.length()-1 );

		for (; s_RayIndex < ray_depth and ConeClosestHit();) {}

		++s_RayIndex;	// now it is number of rays
		if ( s_RayIndex <= 1 )
			return;

		// copy to global storage
		uint	cone_pos	= gl.AtomicAdd( INOUT un_RayStorage.coneCount, s_RayIndex );
		uint	ray_pos		= gl.AtomicAdd( INOUT un_RayStorage.rayCount, 1 );

		if ( cone_pos + s_RayIndex <= un_RayStorage.elements.length() and
			 ray_pos < un_RayStorage.rayToCone.length() )
		{
			un_RayStorage.rayToCone[ ray_pos ] = uint2( cone_pos, s_RayIndex );

			float2	inv_map_scale	= float2(gl.image.GetSize( un_Surface ));
					inv_map_scale	= float2(1.0, inv_map_scale.x / inv_map_scale.y) / un_Constants.map_scale;

			for (uint i = 0; i < s_RayIndex; ++i)
			{
				s_LocalStorage[i].origin0	*= inv_map_scale;
				s_LocalStorage[i].origin1	*= inv_map_scale;
				s_LocalStorage[i].dir0		*= inv_map_scale;
				s_LocalStorage[i].dir1		*= inv_map_scale;

				un_RayStorage.elements[ cone_pos+i ] = s_LocalStorage[i];
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef TRACE_GEOMETRY
	#include "InvocationID.glsl"
	#include "HWRayTracing.glsl"
	#include "Color.glsl"

	void  Main ()
	{
		float2	origin	= GetGlobalCoordSNormCorrected().xy;

		gl::RayQuery	ray_query;
		gl.rayQuery.Initialize( ray_query, un_RtScene, gl::RayFlags::Opaque,
							    /*cullMask*/0xFF, float3(origin.x, origin.y, -5.f),
							    /*Tmin*/0.0f, float3(0.f, 0.f, 1.f), /*Tmax*/10.0f );

		while ( gl.rayQuery.Proceed( ray_query ))
		{
			if ( GetCandidateIntersectionType( ray_query ) == gl::RayQueryCandidateIntersection::Triangle )
				gl.rayQuery.ConfirmIntersection( ray_query );
		}

		float3	color = float3(0.1f);

		if ( GetCommittedIntersectionType( ray_query ) != gl::RayQueryCommittedIntersection::None )
		{
			color = float3(0.4f);
		}

		gl.image.Store( un_Surface, GetGlobalCoord().xy, float4(color, 1.0) );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef TONEMAPPING
	#include "ToneMapping.glsl"
	#include "ColorSpace.glsl"

	void  Main ()
	{
		float4	hdr = gl.texture.Fetch( un_HDR, int2(gl.FragCoord.xy), 0 ) / iScale;	// linear space

		switch ( iTonemapping )
		{
			case 1 :	out_Color = float4(ToneMap_Unreal( hdr.rgb ), 1.0);	break;
			case 2 :	out_Color = float4(Tonemap_Lottes( hdr.rgb ), 1.0);	break;
			default :	out_Color = ApplySRGBCurve( hdr );					break;
		}
	}

#endif
//-----------------------------------------------------------------------------
