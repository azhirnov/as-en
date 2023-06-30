/*
    Blur functions
*/

ND_ float4  Blur5 (gl::CombinedTex2D<float> image, float2 uv, float2 invResolution, float2 direction);
ND_ float4  Blur9 (gl::CombinedTex2D<float> image, float2 uv, float2 invResolution, float2 direction);
ND_ float4  Blur13 (gl::CombinedTex2D<float> image, float2 uv, float2 invResolution, float2 direction);
//-----------------------------------------------------------------------------


float4  Blur5 (gl::CombinedTex2D<float> image, float2 uv, float2 invResolution, float2 direction)
{
    // from https://github.com/Jam3/glsl-fast-gaussian-blur
    // MIT license
    float4 color = float4(0.0);
    float2 off1  = float2(1.3333333333333333) * direction;
    color += gl.texture.Sample( image, uv ) * 0.29411764705882354;
    color += gl.texture.Sample( image, uv + (off1 * invResolution) ) * 0.35294117647058826;
    color += gl.texture.Sample( image, uv - (off1 * invResolution) ) * 0.35294117647058826;
    return color; 
}

float4  Blur9 (gl::CombinedTex2D<float> image, float2 uv, float2 invResolution, float2 direction)
{
    // from https://github.com/Jam3/glsl-fast-gaussian-blur
    // MIT license
    float4 color = float4(0.0);
    float2 off1  = float2(1.3846153846) * direction;
    float2 off2  = float2(3.2307692308) * direction;
    color += gl.texture.Sample( image, uv ) * 0.2270270270;
    color += gl.texture.Sample( image, uv + (off1 * invResolution) ) * 0.3162162162;
    color += gl.texture.Sample( image, uv - (off1 * invResolution) ) * 0.3162162162;
    color += gl.texture.Sample( image, uv + (off2 * invResolution) ) * 0.0702702703;
    color += gl.texture.Sample( image, uv - (off2 * invResolution) ) * 0.0702702703;
    return color;
}

float4  Blur13 (gl::CombinedTex2D<float> image, float2 uv, float2 invResolution, float2 direction)
{
    // from https://github.com/Jam3/glsl-fast-gaussian-blur
    // MIT license
    float4 color = float4(0.0);
    float2 off1  = float2(1.411764705882353) * direction;
    float2 off2  = float2(3.2941176470588234) * direction;
    float2 off3  = float2(5.176470588235294) * direction;
    color += gl.texture.Sample( image, uv ) * 0.1964825501511404;
    color += gl.texture.Sample( image, uv + (off1 * invResolution) ) * 0.2969069646728344;
    color += gl.texture.Sample( image, uv - (off1 * invResolution) ) * 0.2969069646728344;
    color += gl.texture.Sample( image, uv + (off2 * invResolution) ) * 0.09447039785044732;
    color += gl.texture.Sample( image, uv - (off2 * invResolution) ) * 0.09447039785044732;
    color += gl.texture.Sample( image, uv + (off3 * invResolution) ) * 0.010381362401148057;
    color += gl.texture.Sample( image, uv - (off3 * invResolution) ) * 0.010381362401148057;
    return color;
}

float4  Blur5v2 (gl::CombinedTex2D<float> image, float2 uv, float2 invResolution, float2 direction)
{
    const float     weights [5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
    const float2    tex_offset  = direction / invResolution;
    float4          color       = gl.texture.Sample( image, uv ) * weights[0];

    for (int i = 1; i < 5; ++i) {
        color += gl.texture.Sample( image, uv + tex_offset * i ) * weights[i];
        color += gl.texture.Sample( image, uv - tex_offset * i ) * weights[i];
    }
    return color;
}
