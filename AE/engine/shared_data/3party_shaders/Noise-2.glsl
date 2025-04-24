/*
	3party noise-like functions.
*/
#ifdef __cplusplus
# pragma once
#endif

/*
=================================================
	HilbertCurve
----
	from https://www.shadertoy.com/view/llGcDm
	Created by inigo quilez - iq/2022
	license CC BY-NC-SA 3.0 (default)

	adapted from https://en.wikipedia.org/wiki/Hilbert_curve
=================================================
*/
#ifdef AE_LICENSE_CC_BY_NC_SA_3

int  HilbertCurve (int2 p, const int level)
{
	int	d = 0;
	UNROLL for (int k = 0; k < level; ++k)
	{
		int		n = level-k-1;
		int2	r = (p>>n)&1;
				d += ((3*r.x)^r.y) << (2*n);

		if (r.y == 0) {
			if (r.x == 1) {
				p = (1<<n)-1-p;
			}
			p = p.yx;
		}
	}
	return d;
}

int2  InvHilbertCurve (int i, const int level)
{
	int2	p = int2(0);
	UNROLL for (int k = 0; k < level; ++k)
	{
		int2 r = int2( i>>1, i^(i>>1) ) & 1;

		if ( r.y == 0 ) {
			if ( r.x == 1 ) {
				p = (1<<k) - 1 - p;
			}
			p = p.yx;
		}

		p += r << k;
		i >>= 2;
	}
	return p;
}

#endif // AE_LICENSE_CC_BY_NC_SA_3
