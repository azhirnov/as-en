
// from https://blog.selfshadow.com/publications/blending-in-detail/

float3 blend_linear(float4 n1, float4 n2)
{
    float3 r = (n1 + n2)*2 - 2;
    return normalize(r);
}


float3 blend_overlay(float4 n1, float4 n2)
{
    n1 = n1*4 - 2;
    float4 a = n1 >= 0 ? -1 : 1;
    float4 b = n1 >= 0 ?  1 : 0;
    n1 =  2*a + n1;
    n2 = n2*a + b;
    float3 r = n1*n2 - a;
    return normalize(r);
}


float3 blend_pd(float4 n1, float4 n2)
{
    n1 = n1*2 - 1;
    n2 = n2.xyzz*float4(2, 2, 2, 0) + float4(-1, -1, -1, 0);
    float3 r = n1.xyz*n2.z + n2.xyw*n1.z;
    return normalize(r);
}


float3 blend_whiteout(float4 n1, float4 n2)
{
    n1 = n1*2 - 1;
    n2 = n2*2 - 1;
    float3 r = float3(n1.xy + n2.xy, n1.z*n2.z);
    return normalize(r);
}


float3 blend_udn(float4 n1, float4 n2)
{
    float3 c = float3(2, 1, 0);
    float3 r;
    r = n2*c.yyz + n1.xyz;
    r =  r*c.xxx -  c.xxy;
    return normalize(r);
}


float3 blend_rnm(float4 n1, float4 n2)
{
    float3 t = n1.xyz*float3( 2,  2, 2) + float3(-1, -1,  0);
    float3 u = n2.xyz*float3(-2, -2, 2) + float3( 1,  1, -1);
    float3 r = t*dot(t, u) - u*t.z;
    return normalize(r);
}


float3 blend_unity(float4 n1, float4 n2)
{
    n1 = n1.xyzz*float4(2, 2, 2, -2) + float4(-1, -1, -1, 1);
    n2 = n2*2 - 1;

    float3 r;
    r.x = dot(n1.zxx,  n2.xyz);
    r.y = dot(n1.yzy,  n2.xyz);
    r.z = dot(n1.xyw, -n2.xyz);
    return normalize(r);
}
