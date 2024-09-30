#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


#ifdef AE_ENABLE_UNKNOWN_LICENSE

ND_ uint  PCG11 (uint n)
{
	// from https://www.reedbeta.com/blog/hash-functions-for-gpu-rendering/

	uint state = n * 747796405u + 2891336453u;
	uint word  = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

ND_ uint2  PCG22 (uint2 v)
{
	// from https://rene.ruhr/gfx/gpuhash/, http://www.jcgt.org/published/0009/03/02/

	v = v * 1664525u + 1013904223u;

	v.x += v.y * 1664525u;
	v.y += v.x * 1664525u;

	v = v ^ (v >> 16u);

	v.x += v.y * 1664525u;
	v.y += v.x * 1664525u;

	v = v ^ (v >> 16u);

	return v;
}

ND_ uint3  PCG33 (uint3 v)
{
	// from https://rene.ruhr/gfx/gpuhash/, http://www.jcgt.org/published/0009/03/02/

	v = v * 1664525u + 1013904223u;

	v.x += v.y * v.z;
	v.y += v.z * v.x;
	v.z += v.x * v.y;

	v ^= v >> 16u;

	v.x += v.y * v.z;
	v.y += v.z * v.x;
	v.z += v.x * v.y;

	return v;
}

ND_ uint4  PCG44 (uint4 v)
{
	// from http://www.jcgt.org/published/0009/03/02/

	v = v * 1664525u + 1013904223u;

	v.x += v.y*v.w;
	v.y += v.z*v.x;
	v.z += v.x*v.y;
	v.w += v.y*v.z;

	v ^= v >> 16u;

	v.x += v.y*v.w;
	v.y += v.z*v.x;
	v.z += v.x*v.y;
	v.w += v.y*v.z;

	return v;
}

#endif // AE_ENABLE_UNKNOWN_LICENSE
//-----------------------------------------------------------------------------



#ifdef AE_LICENSE_CC_BY_NC_SA_3

// Helpers
// from https://www.shadertoy.com/view/XlGcRh
// license CC BY-NC-SA 3.0 (shadertoy default)

// commonly used constants
#define c1 0xcc9e2d51u
#define c2 0x1b873593u

ND_ uint  _rotl (uint x, uint r)
{
	return (x << r) | (x >> (32u - r));
}

ND_ uint  _rotr (uint x, uint r)
{
	return (x >> r) | (x << (32u - r));
}

ND_ uint  _fmix (uint h)
{
	h ^= h >> 16;
	h *= 0x85ebca6bu;
	h ^= h >> 13;
	h *= 0xc2b2ae35u;
	h ^= h >> 16;
	return h;
}

ND_ uint  _mur (uint a, uint h)
{
	// Helper from Murmur3 for combining two 32-bit values.
	a *= c1;
	a = _rotr(a, 17u);
	a *= c2;
	h ^= a;
	h = _rotr(h, 19u);
	return h * 5u + 0xe6546b64u;
}

ND_ uint  _bswap32 (uint x)
{
	return (((x & 0x000000ffu) << 24) |
			((x & 0x0000ff00u) <<  8) |
			((x & 0x00ff0000u) >>  8) |
			((x & 0xff000000u) >> 24));
}

ND_ uint  _taus (uint z, int s1, int s2, int s3, uint m)
{
	uint b = (((z << s1) ^ z) >> s2);
	return (((z & m) << s3) ^ b);
}
//-----------------------------------------------------------------------------



// CityHash32, adapted from Hash32Len0to4 in https://github.com/google/cityhash (MIT license)
// from https://www.jcgt.org/published/0009/03/02/, https://www.shadertoy.com/view/XlGcRh
// license: Creative Commons CC BY-ND 3.0 (shadertoy default)

ND_ uint  CityHash11 (uint s)
{
	uint len = 4u;
	uint b = 0u;
	uint c = 9u;

	for (uint i = 0u; i < len; i++) {
		uint v = (s >> (i * 8u)) & 0xffu;
		b = b * c1 + v;
		c ^= b;
	}

	return _fmix(_mur(b, _mur(len, c)));
}

ND_ uint  CityHash12 (uint2 s)
{
	uint len = 8u;
	uint a = len, b = len * 5u, c = 9u, d = b;

	a += _bswap32(s.x);
	b += _bswap32(s.y);
	c += _bswap32(s.y);

	return _fmix(_mur(c, _mur(b, _mur(a, d))));
}

ND_ uint  CityHash13 (uint3 s)
{
	uint len = 12u;
	uint a = len, b = len * 5u, c = 9u, d = b;

	a += _bswap32(s.x);
	b += _bswap32(s.z);
	c += _bswap32(s.y);

	return _fmix(_mur(c, _mur(b, _mur(a, d))));
}

ND_ uint  CityHash14 (uint4 s)
{
	uint len = 16u;
	uint a = _bswap32(s.w);
	uint b = _bswap32(s.y);
	uint c = _bswap32(s.z);
	uint d = _bswap32(s.z);
	uint e = _bswap32(s.x);
	uint f = _bswap32(s.w);
	uint h = len;

	return _fmix(_mur(f, _mur(e, _mur(d, _mur(c, _mur(b, _mur(a, h)))))));
}
//-----------------------------------------------------------------------------



// Schechter and Bridson hash
// https://www.cs.ubc.ca/~rbridson/docs/schechter-sca08-turbulence.pdf
ND_ uint  EsgtsaHash11 (uint s)
{
	s = (s ^ 2747636419u) * 2654435769u;// % 4294967296u;
	s = (s ^ (s >> 16u)) * 2654435769u;// % 4294967296u;
	s = (s ^ (s >> 16u)) * 2654435769u;// % 4294967296u;
	return s;
}



// UE4's RandFast function
// https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Shaders/Private/Random.ush
ND_ float  UEFastHash12 (float2 v)
{
	v = float(1./4320.) * v + float2(0.25, 0.);
	float state = fract( dot( v * v, float2(3571)));
	return fract( state * state * float(3571. * 2.));
}


// Hybrid Taus
// https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch37.html
ND_ uint  HybridTaus14 (uint4 z)
{
	z.x = _taus(z.x, 13, 19, 12, 0xfffffffeu);
	z.y = _taus(z.y,  2, 25,  4, 0xfffffff8u);
	z.z = _taus(z.z,  3, 11, 17, 0xfffffff0u);
	z.w = z.w * 1664525u + 1013904223u;

	return z.x ^ z.y ^ z.z ^ z.w;
}

// Interleaved Gradient Noise
//  - Jimenez, Next Generation Post Processing in Call of Duty: Advanced Warfare
//    Advances in Real-time Rendering, SIGGRAPH 2014
ND_ float  InterleavedGradientNoise12 (float2 v)
{
	float3 magic = float3(0.06711056, 0.00583715, 52.9829189);
	return fract(magic.z * fract(dot(v, magic.xy)));
}


ND_ uint  JKiss12 (uint2 p)
{
	uint x = p.x;//123456789;
	uint y = p.y;//234567891;

	uint z=345678912u,w=456789123u,c=0u;
	int t;
	y ^= (y<<5); y ^= (y>>7); y ^= (y<<22);
	t = int(z+w+c); z = w; c = uint(t < 0); w = uint(t&2147483647);
	x += 1411392427u;
	return x + y + w;
}

// UE4's PseudoRandom function
// https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Shaders/Private/Random.ush
ND_ float  PseudoHash12 (float2 v)
{
	v = fract(v / float(128.)) * float(128.) + float2(-64.340622, -72.465622);
	return fract(dot(v.xyx * v.xyy, float3(20.390625, 60.703125, 2.4281209)));
}



// Numerical Recipies 3rd Edition
ND_ uint  Ranlim11 (uint j)
{
	uint u, v, w1, w2, x, y;

	v = 2244614371U;
	w1 = 521288629U;
	w2 = 362436069U;

	u = j ^ v;

	u = u * 2891336453U + 1640531513U;
	v ^= v >> 13; v ^= v << 17; v ^= v >> 5;
	w1 = 33378u * (w1 & 0xffffu) + (w1 >> 16);
	w2 = 57225u * (w2 & 0xffffu) + (w2 >> 16);

	v = u;

	u = u * 2891336453U + 1640531513U;
	v ^= v >> 13; v ^= v << 17; v ^= v >> 5;
	w1 = 33378u * (w1 & 0xffffu) + (w1 >> 16);
	w2 = 57225u * (w2 & 0xffffu) + (w2 >> 16);

	x = u ^ (u << 9); x ^= x >> 17; x ^= x << 6;
	y = w1 ^ (w1 << 17); y ^= y >> 15; y ^= y << 5;

	return (x + v) ^ (y + w2);
}

// Tiny Encryption Algorithm
//  - Zafar et al., GPU random numbers via the tiny encryption algorithm, HPG 2010
ND_ uint2  TeaHash22 (int tea, uint2 p)
{
	uint s = 0u;

	for( int i = 0; i < tea; i++) {
		s += 0x9E3779B9u;
		p.x += (p.y<<4u)^(p.y+s)^(p.y>>5u);
		p.y += (p.x<<4u)^(p.x+s)^(p.x>>5u);
	}
	return p.xy;
}

// Wang hash, described on http://burtleburtle.net/bob/hash/integer.html
// original page by Thomas Wang 404
ND_ uint  WangHash11 (uint v)
{
	v = (v ^ 61u) ^ (v >> 16u);
	v *= 9u;
	v ^= v >> 4u;
	v *= 0x27d4eb2du;
	v ^= v >> 15u;
	return v;
}

// 128-bit xorshift
//  - Marsaglia, Xorshift RNGs, Journal of Statistical Software, v8n14, 2003
ND_ uint  XorShift14 (uint4 v)
{
	v.w ^= v.w << 11u;
	v.w ^= v.w >> 8u;
	v = v.wxyz;
	v.x ^= v.y;
	v.x ^= v.y >> 19u;
	return v.x;
}

// 32-bit xorshift
//  - Marsaglia, Xorshift RNGs, Journal of Statistical Software, v8n14, 2003
ND_ uint  XorShift11 (uint v)
{
	v ^= v << 13u;
	v ^= v >> 17u;
	v ^= v << 5u;
	return v;
}
//-----------------------------------------------------------------------------



// Adapted from MurmurHash3_x86_32 from https://github.com/aappleby/smhasher

ND_ uint  Murmur11 (uint seed)
{
	uint h = 0u;
	uint k = seed;

	k *= c1;
	k = _rotl(k,15u);
	k *= c2;

	h ^= k;
	h = _rotl(h,13u);
	h = h*5u+0xe6546b64u;

	h ^= 4u;

	return _fmix(h);
}

ND_ uint  Murmur12 (uint2 seed)
{
	uint h = 0u;
	uint k = seed.x;

	k *= c1;
	k = _rotl(k,15u);
	k *= c2;

	h ^= k;
	h = _rotl(h,13u);
	h = h*5u+0xe6546b64u;

	k = seed.y;

	k *= c1;
	k = _rotl(k,15u);
	k *= c2;

	h ^= k;
	h = _rotl(h,13u);
	h = h*5u+0xe6546b64u;

	h ^= 8u;

	return _fmix(h);
}

ND_ uint  Murmur13 (uint3 seed)
{
	uint h = 0u;
	uint k = seed.x;

	k *= c1;
	k = _rotl(k,15u);
	k *= c2;

	h ^= k;
	h = _rotl(h,13u);
	h = h*5u+0xe6546b64u;

	k = seed.y;

	k *= c1;
	k = _rotl(k,15u);
	k *= c2;

	h ^= k;
	h = _rotl(h,13u);
	h = h*5u+0xe6546b64u;

	k = seed.z;

	k *= c1;
	k = _rotl(k,15u);
	k *= c2;

	h ^= k;
	h = _rotl(h,13u);
	h = h*5u+0xe6546b64u;

	h ^= 12u;

	return _fmix(h);
}

ND_ uint  Murmur14 (uint4 seed)
{
	uint h = 0u;
	uint k = seed.x;

	k *= c1;
	k = _rotl(k,15u);
	k *= c2;

	h ^= k;
	h = _rotl(h,13u);
	h = h*5u+0xe6546b64u;

	k = seed.y;

	k *= c1;
	k = _rotl(k,15u);
	k *= c2;

	h ^= k;
	h = _rotl(h,13u);
	h = h*5u+0xe6546b64u;

	k = seed.z;

	k *= c1;
	k = _rotl(k,15u);
	k *= c2;

	h ^= k;
	h = _rotl(h,13u);
	h = h*5u+0xe6546b64u;

	k = seed.w;

	k *= c1;
	k = _rotl(k,15u);
	k *= c2;

	h ^= k;
	h = _rotl(h,13u);
	h = h*5u+0xe6546b64u;

	h ^= 16u;

	return _fmix(h);
}

#undef c1
#undef c2
//-----------------------------------------------------------------------------


// SuperFastHash, adapated from http://www.azillionmonkeys.com/qed/hash.html

ND_ uint  SuperFastHash11 (uint data)
{
	uint hash = 4u, tmp;

	hash += data & 0xffffu;
	tmp = (((data >> 16) & 0xffffu) << 11) ^ hash;
	hash = (hash << 16) ^ tmp;
	hash += hash >> 11;

	// Force "avalanching" of final 127 bits
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

ND_ uint  SuperFastHash12 (uint2 data)
{
	uint hash = 8u, tmp;

	hash += data.x & 0xffffu;
	tmp = (((data.x >> 16) & 0xffffu) << 11) ^ hash;
	hash = (hash << 16) ^ tmp;
	hash += hash >> 11;

	hash += data.y & 0xffffu;
	tmp = (((data.y >> 16) & 0xffffu) << 11) ^ hash;
	hash = (hash << 16) ^ tmp;
	hash += hash >> 11;

	// Force "avalanching" of final 127 bits
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

ND_ uint  SuperFastHash13 (uint3 data)
{
	uint hash = 8u, tmp;

	hash += data.x & 0xffffu;
	tmp = (((data.x >> 16) & 0xffffu) << 11) ^ hash;
	hash = (hash << 16) ^ tmp;
	hash += hash >> 11;

	hash += data.y & 0xffffu;
	tmp = (((data.y >> 16) & 0xffffu) << 11) ^ hash;
	hash = (hash << 16) ^ tmp;
	hash += hash >> 11;

	hash += data.z & 0xffffu;
	tmp = (((data.z >> 16) & 0xffffu) << 11) ^ hash;
	hash = (hash << 16) ^ tmp;
	hash += hash >> 11;

	// Force "avalanching" of final 127 bits
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

ND_ uint  SuperFastHash14 (uint4 data)
{
	uint hash = 8u, tmp;

	hash += data.x & 0xffffu;
	tmp = (((data.x >> 16) & 0xffffu) << 11) ^ hash;
	hash = (hash << 16) ^ tmp;
	hash += hash >> 11;

	hash += data.y & 0xffffu;
	tmp = (((data.y >> 16) & 0xffffu) << 11) ^ hash;
	hash = (hash << 16) ^ tmp;
	hash += hash >> 11;

	hash += data.z & 0xffffu;
	tmp = (((data.z >> 16) & 0xffffu) << 11) ^ hash;
	hash = (hash << 16) ^ tmp;
	hash += hash >> 11;

	hash += data.w & 0xffffu;
	tmp = (((data.w >> 16) & 0xffffu) << 11) ^ hash;
	hash = (hash << 16) ^ tmp;
	hash += hash >> 11;

	// Force "avalanching" of final 127 bits
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

#endif // AE_LICENSE_CC_BY_NC_SA_3
//-----------------------------------------------------------------------------


#ifdef AE_LICENSE_MIT

// xxhash (https://github.com/Cyan4973/xxHash)
//   From https://www.shadertoy.com/view/Xt3cDn
// by nimitz 2018
// The MIT License

ND_ uint  XXHash11 (uint p)
{
	const uint PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
	const uint PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
	uint h32 = p + PRIME32_5;
	h32 = PRIME32_4*((h32 << 17) | (h32 >> (32 - 17)));
	h32 = PRIME32_2*(h32^(h32 >> 15));
	h32 = PRIME32_3*(h32^(h32 >> 13));
	return h32^(h32 >> 16);
}

ND_ uint  XXHash12 (uint2 p)
{
	const uint PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
	const uint PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
	uint h32 = p.y + PRIME32_5 + p.x*PRIME32_3;
	h32 = PRIME32_4*((h32 << 17) | (h32 >> (32 - 17)));
	h32 = PRIME32_2*(h32^(h32 >> 15));
	h32 = PRIME32_3*(h32^(h32 >> 13));
	return h32^(h32 >> 16);
}

ND_ uint  XXHash13 (uint3 p)
{
	const uint PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
	const uint PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
	uint h32 =  p.z + PRIME32_5 + p.x*PRIME32_3;
	h32 = PRIME32_4*((h32 << 17) | (h32 >> (32 - 17)));
	h32 += p.y * PRIME32_3;
	h32 = PRIME32_4*((h32 << 17) | (h32 >> (32 - 17)));
	h32 = PRIME32_2*(h32^(h32 >> 15));
	h32 = PRIME32_3*(h32^(h32 >> 13));
	return h32^(h32 >> 16);
}

ND_ uint  XXHash14 (uint4 p)
{
	const uint PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
	const uint PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
	uint h32 =  p.w + PRIME32_5 + p.x*PRIME32_3;
	h32 = PRIME32_4*((h32 << 17) | (h32 >> (32 - 17)));
	h32 += p.y * PRIME32_3;
	h32 = PRIME32_4*((h32 << 17) | (h32 >> (32 - 17)));
	h32 += p.z * PRIME32_3;
	h32 = PRIME32_4*((h32 << 17) | (h32 >> (32 - 17)));
	h32 = PRIME32_2*(h32^(h32 >> 15));
	h32 = PRIME32_3*(h32^(h32 >> 13));
	return h32^(h32 >> 16);
}

#endif // AE_LICENSE_MIT
//-----------------------------------------------------------------------------
