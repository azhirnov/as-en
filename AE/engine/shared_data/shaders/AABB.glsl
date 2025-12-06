// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Axis aligned bounding box.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

struct AABB
{
	float3	min;
	float3	max;
};

ND_ AABB	AABB_Create (float3 min, float3 max);

ND_ bool    AABB_IsInside (const AABB box, const float3 globalPos);
ND_ float3  AABB_ToLocal (const AABB box, const float3 globalPos);
ND_ float3  AABB_ToLocalSNorm (const AABB box, const float3 globalPos);
ND_ float3  AABB_ToLocalUNorm (const AABB box, const float3 globalPos);

ND_ float3  AABB_ToGlobal (const AABB box, const float3 localPos);
ND_ float3  AABB_SNormToGlobal (const AABB box, const float3 snormPos);
ND_ float3  AABB_UNormToGlobal (const AABB box, const float3 unormPos);

ND_ float3  AABB_Center (const AABB box);
ND_ float3  AABB_Size (const AABB box);
ND_ float3  AABB_HalfSize (const AABB box);
ND_ float	AABB_OuterRadius (const AABB box);

// for particles
//ND_ float3  AABB_Wrap (const AABB box, const float3 globalPos);
//ND_ float3  AABB_Clamp (const AABB box, const float3 globalPos);
//ND_ bool    AABB_Rebound (const AABB box, inout float3 globalPos, inout float3 vel);
//-----------------------------------------------------------------------------



/*
=================================================
	AABB_Create
=================================================
*/
AABB  AABB_Create (float3 min, float3 max)
{
	AABB	res;
	res.min = min;
	res.max = max;
	return res;
}

/*
=================================================
	AABB_IsInside
=================================================
*/
bool  AABB_IsInside (const AABB box, const float3 globalPos)
{
	return AllGreaterEqual( globalPos, box.min ) and AllLessEqual( globalPos, box.max );
}

/*
=================================================
	AABB_Center
=================================================
*/
float3  AABB_Center (const AABB box)
{
	return (box.min + box.max) * 0.5;
}

float  AABB_OuterRadius (const AABB box)
{
	return MaxOf( Abs( box.max - box.min ));
}

/*
=================================================
	AABB_Size
=================================================
*/
float3  AABB_Size (const AABB box)
{
	return (box.max - box.min);
}

float3  AABB_HalfSize (const AABB box)
{
	return (box.max - box.min) * 0.5;
}

/*
=================================================
	AABB_ToLocal
----
	converts position from global space to AABB local space
=================================================
*/
float3  AABB_ToLocal (const AABB box, const float3 globalPos)
{
	return globalPos - AABB_Center( box );
}

float3  AABB_ToLocalSNorm (const AABB box, const float3 globalPos)
{
	return (globalPos - AABB_Center( box )) / AABB_HalfSize( box );
}

float3  AABB_ToLocalUNorm (const AABB box, const float3 globalPos)
{
	return ToUNorm( AABB_ToLocalSNorm( box, globalPos ));
}

/*
=================================================
	AABB_ToGlobal
----
	converts position from AABB local space to global space
=================================================
*/
float3  AABB_ToGlobal (const AABB box, const float3 localPos)
{
	return localPos + AABB_Center( box );
}

float3  AABB_SNormToGlobal (const AABB box, const float3 snormPos)
{
	return snormPos * AABB_HalfSize( box ) + AABB_Center( box );
}

float3  AABB_UNormToGlobal (const AABB box, const float3 unormPos)
{
	return AABB_SNormToGlobal( box, ToSNorm(unormPos) );
}

/*
=================================================
	AABB_Wrap
=================================================
*
float3  AABB_Wrap (const AABB box, const float3 globalPos)
{
	return Wrap( globalPos, box.min, box.max );
}

/*
=================================================
	AABB_Clamp
=================================================
*
float3  AABB_Clamp (const AABB box, const float3 globalPos)
{
	return Clamp( globalPos, box.min, box.max );
}

/*
=================================================
	AABB_Rebound
=================================================
*
bool  AABB_Rebound (const AABB box, inout float3 globalPos, inout float3 vel)
{
	if ( AABB_IsInside( box, globalPos ))
		return false;

	globalPos	= AABB_Clamp( box, globalPos );
	vel			= -vel;
	return true;
}
*/
