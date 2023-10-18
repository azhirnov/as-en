// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    local_pos * model_mat   = world_pos (world space)
    world_pos * view_mat    = view_pos (view space)
    view_pos  * proj_mat    = clip_pos (clip space)
    clip_pos  * viewport_tr = scr_pos (screen space)
*/

#include "Math.glsl"


#ifndef DISABLE_un_PerPass
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

#else

/*
=================================================
    LocalPosToWorldSpace
=================================================
*/
ND_ float4  LocalPosToWorldSpace (const float4 pos)
{
    return pos - float4(un_PerPass.camera.pos, 0.0f);
}

#endif
//-----------------------------------------------------------------------------


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
    WorldPosToViewSpace
=================================================
*/
ND_ float4  WorldPosToViewSpace (const float4 pos)
{
    return un_PerPass.camera.view * pos;
}

/*
=================================================
    LocalPosToClipSpace
----
    result in normalized coordinates before /w
    and without viewport scaling.
=================================================
*/
ND_ float4  LocalPosToClipSpace (const float4 pos)
{
    return un_PerPass.camera.viewProj * LocalPosToWorldSpace( pos );
}

ND_ float4  LocalPosToClipSpace (const float3 pos)
{
    return LocalPosToClipSpace( float4( pos, 1.0f ));
}

/*
=================================================
    WorldPosToClipSpace
=================================================
*/
ND_ float4  WorldPosToClipSpace (const float4 pos)
{
    return un_PerPass.camera.viewProj * pos;
}

