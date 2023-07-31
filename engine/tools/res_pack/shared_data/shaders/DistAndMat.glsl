// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Distance & Material (for SDF)
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


struct DistAndMat
{
    float   dist;
    int     mtrIndex;
};

ND_ DistAndMat  DM_Create ();
ND_ DistAndMat  DM_Create (float d, int mtr);
ND_ DistAndMat  DM_Min (const DistAndMat x, const DistAndMat y);
//-----------------------------------------------------------------------------


DistAndMat  DM_Create ()
{
    DistAndMat  dm;
    dm.dist     = 1.0e+10;
    dm.mtrIndex = -1;
    return dm;
}

DistAndMat  DM_Create (float d, int mtr)
{
    DistAndMat  dm;
    dm.dist     = d;
    dm.mtrIndex = mtr;
    return dm;
}

DistAndMat  DM_Min (const DistAndMat x, const DistAndMat y)
{
    return x.dist < y.dist ? x : y;
}
