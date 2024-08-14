// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Distance & Material (for SDF)
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


struct DistAndMtr
{
	float	dist;
	int		mtrIndex;
};

ND_ DistAndMtr  DM_Create ();
ND_ DistAndMtr  DM_Create (const float dist, const int mtrIndex);

ND_ DistAndMtr  DM_Unite (const DistAndMtr lhs, const DistAndMtr rhs);
ND_ DistAndMtr  DM_Unite (const DistAndMtr lhs, const float rhsDist, const int rhsMtrIndex);

ND_ DistAndMtr  DM_SmoothUnite (const DistAndMtr lhs, const DistAndMtr rhs, const float smoothFactor);
//-----------------------------------------------------------------------------


DistAndMtr  DM_Create ()
{
	DistAndMtr	dm;
	dm.dist		= float_max;
	dm.mtrIndex	= -1;
	return dm;
}

DistAndMtr  DM_Create (const float dist, const int mtrIndex)
{
	DistAndMtr	dm;
	dm.dist		= dist;
	dm.mtrIndex	= mtrIndex;
	return dm;
}

DistAndMtr  DM_Unite (const DistAndMtr lhs, const DistAndMtr rhs)
{
	return lhs.dist < rhs.dist ? lhs : rhs;
}

DistAndMtr  DM_Unite (const DistAndMtr lhs, const float rhsDist, const int rhsMtrIndex)
{
	return DM_Unite( lhs, DM_Create( rhsDist, rhsMtrIndex ));
}


// in SDF.glsl
ND_ float  SDF_OpUnite (const float d1, const float d2, const float smoothFactor);

DistAndMtr  DM_SmoothUnite (const DistAndMtr lhs, const DistAndMtr rhs, const float smoothFactor)
{
	int	mtr = lhs.dist < rhs.dist ? lhs.mtrIndex : rhs.mtrIndex;
	return DM_Create( SDF_OpUnite( lhs.dist, rhs.dist, smoothFactor ), mtr );
}
