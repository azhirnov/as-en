// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "geometry_tools/GeometryTools.pch.h"

namespace AE::GeometryTools
{

/*
=================================================
	CheckTBN
=================================================
*/
	inline void  CheckTBN (const float3 &normal, const float3 &tangent, const float3 &bitangent)
	{
		ASSERT( IsNormalized( normal ));
		ASSERT( IsNormalized( tangent ));
		ASSERT( IsNormalized( bitangent ));

		const float3	normal2		= Normalize( Cross( bitangent, tangent ));
		const float3	tangent2	= Normalize( Cross( normal, bitangent ));
		const float3	bitangent2	= Normalize( Cross( tangent, normal ));

		ASSERT( All(Equal( normal,		normal2,	1_pct )));
		ASSERT( All(Equal( tangent,		tangent2,	1_pct )));
		ASSERT( All(Equal( bitangent,	bitangent2,	1_pct )));
		Unused( normal2, tangent2, bitangent2 );
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

		//if ( Dot( Cross( tangent, bitangent ), normal ) < 0.0 )
		//	tangent = -tangent;
	}


} // AE::GeometryTools
