/*
	3party tileable noise functions.
*/
#ifdef __cplusplus
# pragma once
#endif

#define _UHash22( _a_ )			DHash22( _a_ )
#define _UHash33( _a_ )			DHash33( _a_ )
//-----------------------------------------------------------------------------

/*
=================================================
	TileableGradientNoise
----
	range ?

	from https://www.shadertoy.com/view/3dVXDc
	license CC BY-NC-SA 3.0 (shadertoy default)
=================================================
*/
#ifdef AE_LICENSE_CC_BY_NC_SA_3

float  TileableGradientNoise (const float3 x, const float freq)
{
	// grid
	float3 p = floor(x);
	float3 w = fract(x);

	// quintic interpolant
	float3 u = w * w * w * (w * (w * 6. - 15.) + 10.);

	// gradients
	float3 ga = _UHash33(Mod(p + float3(0., 0., 0.), freq));
	float3 gb = _UHash33(Mod(p + float3(1., 0., 0.), freq));
	float3 gc = _UHash33(Mod(p + float3(0., 1., 0.), freq));
	float3 gd = _UHash33(Mod(p + float3(1., 1., 0.), freq));
	float3 ge = _UHash33(Mod(p + float3(0., 0., 1.), freq));
	float3 gf = _UHash33(Mod(p + float3(1., 0., 1.), freq));
	float3 gg = _UHash33(Mod(p + float3(0., 1., 1.), freq));
	float3 gh = _UHash33(Mod(p + float3(1., 1., 1.), freq));

	// projections
	float va = Dot(ga, w - float3(0., 0., 0.));
	float vb = Dot(gb, w - float3(1., 0., 0.));
	float vc = Dot(gc, w - float3(0., 1., 0.));
	float vd = Dot(gd, w - float3(1., 1., 0.));
	float ve = Dot(ge, w - float3(0., 0., 1.));
	float vf = Dot(gf, w - float3(1., 0., 1.));
	float vg = Dot(gg, w - float3(0., 1., 1.));
	float vh = Dot(gh, w - float3(1., 1., 1.));

	// interpolation
	return va +
		   u.x * (vb - va) +
		   u.y * (vc - va) +
		   u.z * (ve - va) +
		   u.x * u.y * (va - vb - vc + vd) +
		   u.y * u.z * (va - vc - ve + vg) +
		   u.z * u.x * (va - vb - ve + vf) +
		   u.x * u.y * u.z * (-va + vb + vc - vd + ve - vf - vg + vh);
}

#endif
//-----------------------------------------------------------------------------


#undef _UHash22
#undef _UHash33
