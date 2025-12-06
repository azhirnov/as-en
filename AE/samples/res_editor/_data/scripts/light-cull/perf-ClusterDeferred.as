// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	related:
	* [project lights to clusters](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/light-cull/test-LightVolToClusters.as)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PUT_OBJECTS
#	define CLUSTER_LIGHT_PASS
#	define PUT_LIGHTS_TO_CLUSTERS
#	define RESOLVE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicDim>		dim				= SurfaceSize();
		RC<Image>			rt				= Image( EPixelFormat::RGBA16F, dim );			rt.Name( "Main-RT" );
		RC<Image>			rt_col			= Image( EPixelFormat::RGBA16F, dim );			rt_col.Name( "Albedo" );
		RC<Image>			rt_norm			= Image( EPixelFormat::RGB10_A2_UNorm, dim );	rt_norm.Name( "Normals" );
		RC<Image>			ds				= Image( Supported_DepthFormat(), dim );		ds.Name( "Depth" );
		RC<Image>			rt_light		= Image( EPixelFormat::RGBA16F, dim );			rt_light.Name( "LightBuf" );
		RC<Buffer>			obj_buf			= Buffer();
		RC<Buffer>			clusters_buf	= Buffer();
		RC<Buffer>			lights_buf		= Buffer();
		RC<Scene>			scene			= Scene();

		RC<DynamicUInt>		obj_count		= DynamicUInt();
		const int2			tile_size		= int2( 16 );
		RC<DynamicUInt>		depth_slices	= DynamicUInt();
		RC<DynamicUInt>		cluster_count	= dim.DivCeil( tile_size ).Area().Mul( depth_slices );
		RC<DynamicUInt>		light_count		= DynamicUInt();
		RC<DynamicFloat>	max_depth		= DynamicFloat();

		obj_buf.ArrayLayout(
			"ObjectTransform",
			"	float3	position;"s +
			"	float3	scale;" +
			"	uint	color;",
			obj_count );

		lights_buf.ArrayLayout(
			"LightObject",
			"	float3	position;"s +
			"	float	radius;" +
			"	uint	color;" +
			"	float	brightness;",
			light_count );

		clusters_buf.ArrayLayout(
			"Cluster",
			"	uint	count;"s +		// atomic
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
			geometry.ArgIn( "un_LightObjs",	lights_buf );

			{
				UnifiedGeometry_DrawIndexed	cmd;
				cmd.indexCount	= indices.size();
				cmd.IndexBuffer( geom_data, "indices" );
				cmd.InstanceCount( obj_count );
				cmd.PipelineHint( "opaque" );
				cmd.layer = ERenderLayer::Opaque;
				geometry.Draw( cmd );
			}{
				UnifiedGeometry_Draw	cmd;
				cmd.vertexCount = 4;
				cmd.InstanceCount( light_count );
				cmd.PipelineHint( "lights" );
				cmd.layer = ERenderLayer::Translucent;
				geometry.Draw( cmd );
			}
			scene.Add( geometry );
		}

		Slider( obj_count,		"ObjCount",		100,	400,		100 );
		Slider( light_count,	"LightCount",	100,	1000,		200 );
		Slider( depth_slices,	"DepthSlices",	1,		100,		10 );
		Slider( max_depth,		"MaxDepth",		1.0,	1000.0,		100.0 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "PUT_OBJECTS" );
			pass.ArgInOut( "un_Objects",	obj_buf );
			pass.LocalSize( 64 );
			pass.DispatchThreads( obj_count );
		}{
			ClearBuffer( clusters_buf, 0 );

			RC<ComputePass>		pass = ComputePass( "", "PUT_LIGHTS_TO_CLUSTERS" );
			pass.ArgInOut( "un_Lights",		lights_buf );
			pass.ArgInOut( "un_Clusters",	clusters_buf );
			pass.LocalSize( 64 );
			pass.DispatchThreads( light_count );
			pass.Constant( "iMaxDepth",		max_depth );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "opaque" );
			pass.Layer( ERenderLayer::Opaque );
			pass.AddPipeline( "*-opaque.ppln" );
			pass.Output( "out_Color",		rt_col,		RGBA32f(0.0) );
			pass.Output( "out_Normal",		rt_norm,	RGBA32f(0.0) );
			pass.Output(					ds,			DepthStencil(0.0, 0) );
		}{
			RC<Postprocess>		pass = Postprocess( "", "CLUSTER_LIGHT_PASS" );
			pass.Output( "out_Color",		rt_light );
			pass.ArgIn( "un_Albedo",		rt_col,		Sampler_LinearClamp );
			pass.ArgIn( "un_Normal",		rt_norm,	Sampler_LinearClamp );
			pass.ArgIn( "un_Depth",			ds,			Sampler_LinearClamp );
			pass.ArgIn( "un_Lights",		lights_buf );
			pass.ArgIn( "un_Clusters",		clusters_buf );
			pass.Constant( "iLight",		float3(0.4, -1.0, -1.0) );
			pass.Constant( "iMaxDepth",		max_depth );
			pass.Constant( "iDepthSlices",	depth_slices );
			pass.Constant( "iTileSize",		tile_size );
		}{
			RC<SceneGraphicsPass>	pass = scene.AddGraphicsPass( "translucent" );
			pass.Layer( ERenderLayer::Translucent );
			pass.AddPipeline( "*-lights.ppln" );
			pass.Output( "out_Color",		rt_col );
			pass.OutputLS(					ds,			EAttachmentLoadOp::Load, EAttachmentStoreOp::None );
			pass.Slider( "iLightSize",		0.01,	1.0,	1.0 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "RESOLVE" );
			pass.Output( "out_Color",		rt );
			pass.ArgIn( "un_Albedo",		rt_col,		Sampler_LinearClamp );
			pass.ArgIn( "un_LightBuf",		rt_light,	Sampler_LinearClamp );
		}

		Present( rt );
	}

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
			obj.position	= float3(0.0, 3.0, 0.0);
			obj.scale		= float2(1000.0, 1.0).xyx;
			obj.color		= packUnorm4x8( float4(0.2) );
			un_Objects.elements[idx] = obj;
			return;
		}

		obj.position.x	= (ToSNorm( uv.x ) + 0.25) * 20.0;
		obj.position.y	= 2.0;
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
#ifdef PUT_LIGHTS_TO_CLUSTERS
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

		light.radius		= 2.0 + DHash12( uv * 55.0 ) * 3.0;
		light.color			= packUnorm4x8( Rainbow( DHash12( uv * 333.0 ) * 0.2 + 0.1 ));
		light.brightness	= 1.0 + DHash12( uv * 99.0 );

		un_Lights.elements[ idx ] = light;
	}


	void  PutLightToCluster (const LightObject light, const uint lightIdx)
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
	}


	void Main ()
	{
		const uint	idx = GetGlobalIndex();

		LightObject light
		GenLight( OUT light, idx );

		PutLightToCluster( light, idx );
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
	#include "ToneMapping.glsl"

	void Main ()
	{
		float3	light	= gl.texture.Fetch( un_LightBuf, int2(gl.FragCoord.xy), 0 ).rgb;
		float3	albedo	= gl.texture.Fetch( un_Albedo, int2(gl.FragCoord.xy), 0 ).rgb;

		out_Color = float4( light * albedo, 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
