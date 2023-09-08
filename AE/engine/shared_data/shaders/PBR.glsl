// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Physically based rendering functions.

    https://physicallybased.info/
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

// Lighting
ND_ float  LinearAttenuation (const float dist, const float radius);
ND_ float  QuadraticAttenuation (const float dist, const float radius);
ND_ float  Attenuation (const float3 attenFactor, const float dist);

ND_ float3  SpecularBRDF (const float3 albedo, const float3 lightColor, const float3 lightDir, const float3 viewDir,
                          const float3 surfNorm, const float metallic, const float roughness);

ND_ float  Fresnel (float eta, float cosThetaI);
//-----------------------------------------------------------------------------




/*
=================================================
    Attenuation
=================================================
*/
float  LinearAttenuation (const float dist, const float radius)
{
    return Saturate( 1.0 - (dist / radius) );
}

float  QuadraticAttenuation (const float dist, const float radius)
{
    float   f = dist / radius;
    return Saturate( 1.0 - f*f );
}

float  Attenuation (const float3 attenFactor, const float dist)
{
    return 1.0 / ( attenFactor.x + attenFactor.y * dist + attenFactor.z * dist * dist );
}

/*
=================================================
    Fresnel
=================================================
*/
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

/*
=================================================
    SpecularBRDF
=================================================
*/
float3  SpecularBRDF (const float3 albedo, const float3 lightColor, const float3 lightDir, const float3 viewDir,
                      const float3 surfNorm, const float metallic, const float roughness)
{
    float3  halfway_dir = Normalize( viewDir + lightDir );
    float   n_dot_v     = Saturate( Dot( surfNorm, viewDir ));
    float   n_dot_l     = Saturate( Dot( surfNorm, lightDir ));
//  float   l_dot_h     = Saturate( Dot( lightDir, halfway_dir ));
    float   n_dot_h     = Saturate( Dot( surfNorm, halfway_dir ));
    float3  color       = float3(0.0);

    if ( n_dot_l > 0.0 )
    {
        float  rough = Max( 0.05, roughness );

        float  D;
        {
            float   alpha   = rough * rough;
            float   alpha2  = alpha * alpha;
            float   denom   = n_dot_h * n_dot_h * (alpha2 - 1.0f) + 1.0f;
            D = (alpha2) / (Pi() * denom*denom); 
        }

        float  G;
        {
            float   r   = (rough + 1.0f);
            float   k   = (r*r) / 8.0f;
            float   GL  = n_dot_l / (n_dot_l * (1.0f - k) + k);
            float   GV  = n_dot_v / (n_dot_v * (1.0f - k) + k);
            G = GL * GV;
        }

        float3 F;
        {
            float3  F0 = Lerp( float3(0.04f), albedo, metallic );
            F = F0 + (1.0f - F0) * Pow( 1.0f - n_dot_v, 5.0f ); 
        }

        float3  spec = D * F * G / (4.0f * n_dot_l * n_dot_v);

        color += spec * n_dot_l * lightColor;
    }

    return color;
}
