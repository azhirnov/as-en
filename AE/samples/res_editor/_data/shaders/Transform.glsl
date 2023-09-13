// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Math.glsl"

/*
=================================================
    LocalPosToWorldSpace
=================================================
*/
ND_ float4  LocalPosToWorldSpace (const float4 pos)
{
    return (un_PerObject.transform * pos) - float4(un_PerPass.camera.pos, 0.0f);
}

ND_ float4  LocalPosToWorldSpace (const float3 pos)
{
    return LocalPosToWorldSpace( float4( pos, 1.0f ));
}

/*
=================================================
    LocalVecToWorldSpace
----
    can be used for normals, tangents, bitangents, light dirs
=================================================
*/
ND_ float3  LocalVecToWorldSpace (const float3 dir)
{
    return Normalize( float3x3(un_PerObject.transform) * dir );
}

/*
=================================================
    LocalPosToViewSpace
=================================================
*/
ND_ float4  LocalPosToViewSpace (const float4 pos)
{
    return un_PerPass.camera.view * LocalPosToWorldSpace( pos );
}

ND_ float4  LocalPosToViewSpace (const float3 pos)
{
    return LocalPosToViewSpace( float4( pos, 1.0f ));
}

/*
=================================================
    LocalPosToScreenSpace
----
    result in normalized coordinates before /w
    and without viewport scaling.
=================================================
*/
ND_ float4  LocalPosToScreenSpace (const float4 pos)
{
    return un_PerPass.camera.viewProj * LocalPosToWorldSpace( pos );
}

ND_ float4  LocalPosToScreenSpace (const float3 pos)
{
    return LocalPosToScreenSpace( float4( pos, 1.0f ));
}
