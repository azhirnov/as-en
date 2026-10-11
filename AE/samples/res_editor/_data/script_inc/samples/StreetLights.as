// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PUT_OBJECTS
#	define PUT_LIGHTS
#endif
//-----------------------------------------------------------------------------
#if defined(PUT_OBJECTS) or defined(PUT_LIGHTS)

	#define GROUND_Y	2.0

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
			obj.position	= float3(0.0, GROUND_Y + 1.0, 0.0);
			obj.scale		= float2(1000.0, 1.0).xyx;
			obj.color		= packUnorm4x8( float4(0.2) );
			un_Objects.elements[idx] = obj;
			return;
		}

		if ( idx >= un_Objects.elements.length() )
			return;

		obj.position.x	= (ToSNorm( uv.x ) + 0.25) * 20.0;
		obj.position.y	= GROUND_Y;
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
#ifdef PUT_LIGHTS
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"
	#include "Transform.glsl"

	float  ConeHeightToFullyReachGround (const float3 lightPos, const float3 lightDir, const float halfAngle, const float groundY)
	{
		float	horizontal_factor		= Sqrt( Max( 0.0, 1.0 - lightDir.y * lightDir.y ));
		float	radiusY_per_height		= Tan( halfAngle ) * horizontal_factor;
		float	lowest_edgeY_per_height	= lightDir.y - radiusY_per_height;
		float	min_height				= 1.0;

		if ( lowest_edgeY_per_height <= 1e-5 )
			return min_height;

		return Max( min_height, (groundY - lightPos.y) / lowest_edgeY_per_height );
	}


	void  GenLight (out LightObject light, const uint idx)
	{
		const uint		street_cnt		= 4u * 2;	// 2 light lines per street
		const uint		street_idx		= idx % street_cnt;
		const uint		building_idx	= idx / street_cnt;
		const float2	uv				= float2( street_idx, building_idx ) / float2( street_cnt, 1.0 );

		light.position.x	= (ToSNorm( uv.x ) + 0.125) * 20.0 + ToSNorm( DHash12( uv.yx * 123.0 )) * 0.5;
		light.position.y	= -1.5 + DHash12( uv * 77.0 );
		light.position.z	= uv.y * 2.0 - 5.0;

		light.dir			= Normalize( float3(0.0, 1.0, 0.0) + float3( ToSNorm( DHash22( uv * 222.0 )), 0.0 ).xzy );
		light.angle			= float_Pi * 0.5 * (DHash12( uv * 67.2 ) * 0.5 + 0.5);

		light.color			= packUnorm4x8( Rainbow( DHash12( uv * 333.0 ) * 0.2 + 0.1 ));
		light.brightness	= 1.0 + DHash12( uv * 99.0 );
		light.atten			= float2( 0.027, 0.055 );

		light.height		= ConeHeightToFullyReachGround( light.position, light.dir, light.angle * 0.5, GROUND_Y );
		light.height		= Min( iMaxHeight, light.height );

		#ifdef FIELD_VISIBLE
			light.visible = 0;
		#endif

		un_Lights.elements[ idx ] = light;
	}


	void Main ()
	{
		const uint	idx = GetGlobalIndex();

		if ( idx >= un_Lights.elements.length() )
			return;

		LightObject light;
		GenLight( OUT light, idx );
	}

#endif
//-----------------------------------------------------------------------------
