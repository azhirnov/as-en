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
