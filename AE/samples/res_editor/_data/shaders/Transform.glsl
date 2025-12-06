// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	local_pos * model_mat	= world_pos (world space)
	world_pos * view_mat	= view_pos  (view space)
	view_pos  * proj_mat	= clip_pos  (clip space)			- before /W
	norm( clip_pos )		= ndc_pos	(NDC, norm clip space)	- after /W:  XY range [-1,+1], Z range [0, +1]
	norm( ndc_pos )			= unorm_scr	(unorm screen pos)		- range [0,1]
	unorm_scr * viewport	= scr_pos   (screen space in px)	- happens before FS
*/

#include "Math.glsl"
#include "Matrix.glsl"


#ifdef DESCRIPTOR_UniformBuffer_un_PerObject
/*
=================================================
	LocalPosToWorldSpace
----
	world space where camera in center
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
//-----------------------------------------------------------------------------
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

ND_ float4  LocalPosToWorldSpace (const float3 pos)
{
	return LocalPosToWorldSpace( float4( pos, 1.0f ));
}

#endif // DESCRIPTOR_UniformBuffer_un_PerObject
//-----------------------------------------------------------------------------


/*
=================================================
	WorldPosToViewSpace / LocalPosToViewSpace
=================================================
*/
ND_ float4  WorldPosToViewSpace (const float4 pos)
{
	return un_PerPass.camera.view * pos;
}

ND_ float4  WorldPosToViewSpace (const float3 pos)
{
	return WorldPosToViewSpace( float4( pos, 1.0f ));
}

ND_ float4  LocalPosToViewSpace (const float4 pos)
{
	return WorldPosToViewSpace( LocalPosToWorldSpace( pos ));
}

ND_ float4  LocalPosToViewSpace (const float3 pos)
{
	return WorldPosToViewSpace( LocalPosToWorldSpace( pos ));
}

/*
=================================================
	WorldPosToClipSpace / LocalPosToClipSpace
----
	result in coordinates before /w
=================================================
*/
ND_ float4  WorldPosToClipSpace (const float4 pos)
{
	return un_PerPass.camera.viewProj * pos;
}

ND_ float4  LocalPosToClipSpace (const float4 pos)
{
	return WorldPosToClipSpace( LocalPosToWorldSpace( pos ));
}

ND_ float4  LocalPosToClipSpace (const float3 pos)
{
	return LocalPosToClipSpace( float4( pos, 1.0f ));
}

/*
=================================================
	WorldPosToNormClipSpace / LocalPosToNormClipSpace
----
	result in normalized coordinates after /w.
	W component contains 1/w as in 'gl.FragCoord.w'.
=================================================
*/
ND_ float4  WorldPosToNormClipSpace (const float4 pos)
{
	return ProjectToNormClipSpace( un_PerPass.camera.viewProj, pos );
}

ND_ float4  LocalPosToNormClipSpace (const float4 pos)
{
	return WorldPosToNormClipSpace( LocalPosToWorldSpace( pos ));
}

ND_ float4  LocalPosToNormClipSpace (const float3 pos)
{
	return LocalPosToNormClipSpace( float4( pos, 1.0f ));
}

/*
=================================================
	WorldPosToScreenSpace / LocalPosToScreenSpace / ViewPosToScreenSpace
----
	XY in screen coordinates (pixels).
	Z - non-linear depth in range [0, 1].
	W - contains 1/w as in 'gl.FragCoord.w'.
=================================================
*/
ND_ float4  WorldPosToScreenSpace (const float4 pos, float4 viewport)
{
	return ProjectToScreenSpace( un_PerPass.camera.viewProj, pos, viewport );
}

ND_ float4  LocalPosToScreenSpace (const float4 pos, float4 viewport)
{
	return WorldPosToScreenSpace( LocalPosToWorldSpace( pos ), viewport );
}

ND_ float4  LocalPosToScreenSpace (const float3 pos, float4 viewport)
{
	return LocalPosToScreenSpace( float4( pos, 1.0f ), viewport );
}

ND_ float4  ViewPosToScreenSpace (const float4 pos, float4 viewport)
{
	return ProjectToScreenSpace( un_PerPass.camera.proj, pos, viewport );
}
