// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef __cplusplus
# pragma once
#endif

#include "Geometry2D.glsl"


ND_ RectI	NdcToTile (Rect ndc, const float2 tileCount);

ND_ float2	Cluster_ZProjection_Log     (const float clusterZ, const float clusterCount, const float2 clipPlanes);
ND_ int		Cluster_InvZProjection_Log  (const float viewZ,    const float clusterCount, const float2 clipPlanes);
ND_ float2	Cluster_ZProjectionLerp_Log (const float clusterZ, const float clusterCount, const float2 clipPlanes);

// optimized
ND_ float2	Cluster_ZProjection2_Log    (const float clusterZ, const float clusterCount, const float zNear,    const float zFarDivNear);
ND_ int		Cluster_InvZProjection2_Log (const float viewZ,    const float clusterCount, const float invZNear, const float lnZFarDivNear);
//-----------------------------------------------------------------------------




/*
=================================================
	NdcToTile
=================================================
*/
RectI  NdcToTile (Rect ndc, const float2 tileCount)
{
	ndc = Rect_ToUNorm( ndc );
	ndc.v *= tileCount;

	return Rect_Create( int2(		Rect_Min( ndc )),
						int2( Ceil(	Rect_Max( ndc ))) );
}

/*
=================================================
	Cluster_ZProjection_Log
----
	log view-space
	returns cluster near/far Z in view space
=================================================
*/
float2  Cluster_ZProjection_Log (const float clusterZ, const float clusterCount, const float2 clipPlanes)
{
	float	z_near	= clipPlanes.x;
	float	z_far	= clipPlanes.y;
	return Cluster_ZProjection2_Log( clusterZ, clusterCount, z_near, z_far / z_near );
}

float2  Cluster_ZProjection2_Log (const float clusterZ, const float clusterCount, const float zNear, const float zFarDivNear)
{
	float	cl_near_z	= zNear * Pow( zFarDivNear,  clusterZ       / clusterCount );
	float	cl_far_z	= zNear * Pow( zFarDivNear,  (clusterZ+1.0) / clusterCount );
	return	float2( cl_near_z, cl_far_z );
}

/*
=================================================
	Cluster_ZProjectionLerp_Log
----
	returns lerp factor for cluster near/far Z
	result may be used in Frustum_ToCluster()
=================================================
*/
float2  Cluster_ZProjectionLerp_Log (const float clusterZ, const float clusterCount, const float2 clipPlanes)
{
	float2	cl_near_far	= Cluster_ZProjection_Log( clusterZ, clusterCount, clipPlanes );
	float2	factor		= (cl_near_far - clipPlanes.x) / (clipPlanes.y - clipPlanes.x);
	return	factor;
}

/*
=================================================
	Cluster_InvZProjection_Log
----
	depth to cluster
=================================================
*/
int	Cluster_InvZProjection_Log (const float viewZ, const float clusterCount, const float2 clipPlanes)
{
	float	z_near	= clipPlanes.x;
	float	z_far	= clipPlanes.y;
	return clusterCount * Ln( viewZ / z_near ) / Ln( z_far / z_near );
}

int  Cluster_InvZProjection2_Log (const float viewZ, const float clusterCount, const float invZNear, const float lnZFarDivNear)
{
	return clusterCount * Ln( viewZ * invZNear ) / lnZFarDivNear;
}

