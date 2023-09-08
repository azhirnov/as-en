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
    float   dist;
    int     mtrIndex;
};

ND_ DistAndMtr  DM_Create ();
ND_ DistAndMtr  DM_Create (const float dist, const int mtrIndex);
ND_ DistAndMtr  DM_Min (const DistAndMtr lhs, const DistAndMtr rhs);
ND_ DistAndMtr  DM_Min (const DistAndMtr lhs, const float rhsDist, const int rhsMtrIndex);
//-----------------------------------------------------------------------------


DistAndMtr  DM_Create ()
{
    DistAndMtr  dm;
    dm.dist     = 1.0e+10;
    dm.mtrIndex = -1;
    return dm;
}

DistAndMtr  DM_Create (const float dist, const int mtrIndex)
{
    DistAndMtr  dm;
    dm.dist     = dist;
    dm.mtrIndex = mtrIndex;
    return dm;
}

DistAndMtr  DM_Min (const DistAndMtr lhs, const DistAndMtr rhs)
{
    return lhs.dist < rhs.dist ? lhs : rhs;
}

DistAndMtr  DM_Min (const DistAndMtr lhs, const float rhsDist, const int rhsMtrIndex)
{
    return DM_Min( lhs, DM_Create( rhsDist, rhsMtrIndex ));
}
