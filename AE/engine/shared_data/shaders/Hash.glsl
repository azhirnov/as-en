// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Hash functions
*/


// DHash, HEHash
//	range [0..1]
#include "../3party_shaders/Hash-1.glsl"

// WeylHash, ModHash, IWeylHash
// Hash_Uniform, Hash_Triangular, Hash_Gaussianish, Hash_MoarGaussianish
#include "../3party_shaders/Hash-2.glsl"

// MHash
#include "../3party_shaders/Hash-3.glsl"

// PCG, CityHash
// EsgtsaHash, UEFastHash, HybridTaus, InterleavedGradientNoise, JKiss, PseudoHash
// int: Ranlim, TeaHash, WangHash, XorShift, Murmur, SuperFastHash, XXHash
#include "../3party_shaders/Hash-4.glsl"
//-----------------------------------------------------------------------------


/*
=================================================
	HashCombine
=================================================
*/
ND_ uint  HashCombine (const uint seed, const uint hash)
{
	return seed ^ (hash + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

/*
=================================================
	ShuffleBits
----
	Fisher–Yates shuffle algorithm
=================================================
*/
ND_ uint  ShuffleBits (const uint x, const uint seed)
{
	const int	N = 32;
	uint		perm [N];

	for (int i = 0; i < N; ++i) {
		perm[i] = uint(i);
	}

	[[unroll]]
	for (int i = N - 1; i > 0; --i)
	{
		uint j = HEHash11i( seed + uint(i) ) % uint(i + 1);
		Swap( perm[i], perm[j] );
	}

	uint	result = 0u;

	[[unroll]]
	for (int i = 0; i < N; ++i)
	{
		if ( (x >> i & 1u) != 0u )
			result |= 1u << perm[i];
	}

	return result;
}

/*
=================================================
	FastShuffleBits
=================================================
*/
ND_ uint  FastShuffleBits (uint x, const uint seed)
{
	const uint	y = x;

    if ( (seed & 1u) != 0u )
        x = ((x & 0x55555555u) << 1) | ((x & 0xAAAAAAAAu) >> 1);

    if ( (seed & 2u) != 0u )
        x = ((x & 0x33333333u) << 2) | ((x & 0xCCCCCCCCu) >> 2);

    if ( (seed & 4u) != 0u )
        x = ((x & 0x0F0F0F0Fu) << 4) | ((x & 0xF0F0F0F0u) >> 4);

    if ( (seed & 8u) != 0u )
        x = ((x & 0x00FF00FFu) << 8) | ((x & 0xFF00FF00u) >> 8);

    if ( (seed & 16u) != 0u )
        x = (x << 16) | (x >> 16);

	x = BitRotateLeft( x, seed >> 5 );

	if ( x == y )
		x = BitReverse( y );

    return x;
}

