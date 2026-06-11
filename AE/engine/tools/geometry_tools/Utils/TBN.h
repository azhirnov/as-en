// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "geometry_tools/Common.h"

namespace AE::GeometryTools
{

/*
=================================================
	CheckTBN
=================================================
*/
	inline void  CheckTBN (const float3 &normal, const float3 &tangent, const float3 &bitangent)
	{
	#ifdef AE_DEBUG
		CHECK( IsNormalized( normal ));
		CHECK( IsNormalized( tangent ));
		CHECK( IsNormalized( bitangent ));

		const float		handedness	= Dot( Cross( tangent, bitangent ), normal );

		// must be -1 for left-handed matrix
		CHECK( IsZero( handedness + 1.f ));

		const float3	normal2		= Normalize( Cross( bitangent, tangent ));
		const float3	tangent2	= Normalize( Cross( normal, bitangent ));
		const float3	bitangent2	= Normalize( Cross( tangent, normal ));

		CHECK( All(Equal( normal,		normal2,	1_pct )));
		CHECK( All(Equal( tangent,		tangent2,	1_pct )));
		CHECK( All(Equal( bitangent,	bitangent2,	1_pct )));
	#else
		Unused( normal, tangent, bitangent );
	#endif
	}

/*
=================================================
	ComputeTBN
=================================================
*/
	inline void  ComputeTBN (/*vert0*/  const float3 position0, const float2 texcoord0,
							 /*vert1*/  const float3 position1, const float2 texcoord1,
							 /*vert2*/  const float3 position2, const float2 texcoord2,
							 /*output*/ OUT float3 &normal, OUT float3 &tangent, OUT float3 &bitangent)
	{
		float3	e0 = float3( position1.x - position0.x,
							 texcoord1.x - texcoord0.x,
							 texcoord1.y - texcoord0.y );
		float3	e1 = float3( position2.x - position0.x,
							 texcoord2.x - texcoord0.x,
							 texcoord2.y - texcoord0.y );
		float3	cp = Cross( e0, e1 );

		normal = Normalize( Cross( position1 - position0, position2 - position0 ));

		if ( IsNotZero( cp.x )) {
			tangent.x	= -cp.y / cp.x;
			bitangent.x	= -cp.z / cp.x;
		}else{
			tangent.x   = 0.0;
			bitangent.x = 0.0;
		}

		e0.x = position1.y - position0.y;
		e1.x = position2.y - position0.y;
		cp   = Cross( e0, e1 );

		if ( IsNotZero( cp.x )) {
			tangent.y   = -cp.y / cp.x;
			bitangent.y = -cp.z / cp.x;
		}else{
			tangent.y   = 0.0;
			bitangent.y = 0.0;
		}

		e0.x = position1.z - position0.z;
		e1.x = position2.z - position0.z;
		cp   = Cross( e0, e1 );

		if ( IsNotZero( cp.x )) {
			tangent.z   = -cp.y / cp.x;
			bitangent.z = -cp.z / cp.x;
		}else{
			tangent.z   = 0.0;
			bitangent.z = 0.0;
		}

		tangent		= Normalize( tangent );
		bitangent	= Normalize( bitangent );

		if ( Dot( Cross( tangent, bitangent ), normal ) > 0.0 )
			tangent = -tangent;

		CheckTBN( normal, tangent, bitangent );
	}


} // AE::GeometryTools
