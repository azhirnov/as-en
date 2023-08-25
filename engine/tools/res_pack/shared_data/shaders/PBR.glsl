/*
    Physically based rendering functions.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


ND_ float  Fresnel (float eta, float cosThetaI);
//-----------------------------------------------------------------------------


float  Fresnel (float eta, float cosThetaI)
{
    cosThetaI = Clamp( cosThetaI, -1.0f, 1.0f );

    if ( cosThetaI < 0.0f )
    {
        eta         = 1.0f / eta;
        cosThetaI   = -cosThetaI;
    }

    float   sin_theta_tsq = eta * eta * (1.0f - cosThetaI * cosThetaI);

    if ( sin_theta_tsq > 1.0f )
        return 1.0f;

    float   cos_theta_t = Sqrt( 1.0f - sin_theta_tsq );
    float   Rs          = (eta * cosThetaI   - cos_theta_t) / (eta * cosThetaI   + cos_theta_t);
    float   Rp          = (eta * cos_theta_t - cosThetaI)   / (eta * cos_theta_t + cosThetaI);

    return 0.5f * (Rs * Rs + Rp * Rp);
}
