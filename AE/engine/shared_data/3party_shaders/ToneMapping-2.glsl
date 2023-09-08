
//
// ACES
//

// from https://www.shadertoy.com/view/XsGfWV
float3  ToneMap_ACES_v2 (const float3 color)
{   
    const float3x3  m1 = float3x3(
        0.59719, 0.07600, 0.02840,
        0.35458, 0.90834, 0.13383,
        0.04823, 0.01566, 0.83777
    );
    const float3x3  m2 = float3x3(
         1.60475, -0.10208, -0.00327,
        -0.53108,  1.10813, -0.07276,
        -0.07367, -0.00605,  1.07602
    );
    float3 v = m1 * color;    
    float3 a = v * (v + 0.0245786) - 0.000090537;
    float3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return clamp( m2 * (a / b), 0.0, 1.0 );
}

float3  ToneMap_ACES_v3 (const float3 color)
{   
    const float3x3  m1 = float3x3(
        0.59719, 0.35458, 0.04823,
        0.07600, 0.90834, 0.01566,
        0.02840, 0.13383, 0.83777
    );
    const float3x3  m2 = float3x3(
         1.60475, -0.53108, -0.07367,
        -0.10208,  1.10813, -0.00605,
        -0.00327, -0.07276,  1.07602
    );
    float3 v = m1 * color;    
    float3 a = v * (v + 0.0245786) - 0.000090537;
    float3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return clamp( m2 * (a / b), 0.0, 1.0 );
}


//-----------------------------------------------------------------------------
// from https://www.shadertoy.com/view/WdjSW3


//
// Reinhard2
// 

float3  ToneMap_Reinhard2 (const float3 hdr)
{
    const float L_white = 4.0;
    return (hdr * (1.0 + hdr / (L_white * L_white))) / (1.0 + hdr);
}


//
// Unreal
// 

// Unreal 3, Documentation: "Color Grading"
// Adapted to be close to Tonemap_ACES, with similar range
// Gamma 2.2 correction is baked in, don't use with sRGB conversion!

float3  ToneMap_Unreal (const float3 hdr)
{
    return hdr / (hdr + 0.155) * 1.019;
}


//
// Uchimura
// 

// Uchimura 2017, "HDR theory and practice"
// Math: https://www.desmos.com/calculator/gslcdxvipg
// Source: https://www.slideshare.net/nikuque/hdr-theory-and-practicce-jp

float  ToneMap_Uchimura (const float x, const float P, const float a, const float m, const float l, const float c, const float b)
{
    float l0 = ((P - m) * l) / a;
    float L0 = m - m / a;
    float L1 = m + (1.0 - m) / a;
    float S0 = m + l0;
    float S1 = m + a * l0;
    float C2 = (a * P) / (P - S1);
    float CP = -C2 / P;

    float w0 = 1.0 - smoothstep(0.0, m, x);
    float w2 = step(m + l0, x);
    float w1 = 1.0 - w0 - w2;

    float T = m * pow(x / m, c) + b;
    float S = P - (P - S1) * exp(CP * (x - S0));
    float L = m + a * (x - m);

    return T * w0 + L * w1 + S * w2;
}

float  ToneMap_Uchimura (const float hdr)
{
    const float P = 1.0;  // max display brightness
    const float a = 1.0;  // contrast
    const float m = 0.22; // linear section start
    const float l = 0.4;  // linear section length
    const float c = 1.33; // black
    const float b = 0.0;  // pedestal
    return ToneMap_Uchimura( hdr, P, a, m, l, c, b );
}

float3  ToneMap_Uchimura (const float3 hdr) {
    return float3( ToneMap_Uchimura(hdr.r), ToneMap_Uchimura(hdr.g), ToneMap_Uchimura(hdr.b) );
}



//
// Lottes
// 

// Lottes 2016, "Advanced Techniques and Optimization of HDR Color Pipelines"

float  Tonemap_Lottes (const float x)
{
    const float a       = 1.6;
    const float d       = 0.977;
    const float hdrMax  = 8.0;
    const float midIn   = 0.18;
    const float midOut  = 0.267;

    // Can be precomputed
    const float b =
        (-pow(midIn, a) + pow(hdrMax, a) * midOut) /
        ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

    const float c =
        (pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
        ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

    return pow(x, a) / (pow(x, a * d) * b + c);
}

float3  Tonemap_Lottes (const float3 hdr) {
    return float3( Tonemap_Lottes(hdr.r), Tonemap_Lottes(hdr.g), Tonemap_Lottes(hdr.b) );
}
//-----------------------------------------------------------------------------



// from https://www.shadertoy.com/view/tl3GR8
float3  ToneMap_whitePreservingLumaBasedReinhardToneMapping (float3 color)
{
    float white = 2.;
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float toneMappedLuma = luma * (1. + luma / (white*white)) / (1. + luma);
    color *= toneMappedLuma / luma;
    return color;
}
//-----------------------------------------------------------------------------
