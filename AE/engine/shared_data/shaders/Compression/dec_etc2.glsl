// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'


ND_ float4  DecodeETC2Impl (uint2 block, int2 localTexel, bool has1BitAlpha)
{
	// selector order is (msb<<1)|lsb with sign = (msb ? -1 : +1) and
	// lsb choosing small/large, i.e. {+sm,+lg,-sm,-lg}.
	const int kMod[32] = int[](
			2,   8,  -2,  -8,
			5,  17,  -5, -17,
			9,  29,  -9, -29,
			13,  42, -13, -42,
			18,  60, -18, -60,
			24,  80, -24, -80,
			33, 106, -33,-106,
			47, 183, -47,-183
	);
	const int kDist[8] = int[](3, 6, 11, 16, 23, 32, 41, 64);

	int x = localTexel.x & 3;
	int y = localTexel.y & 3;

	uint s0 =  block.x        & 0xFFu;
	uint s1 = (block.x >>  8) & 0xFFu;
	uint s2 = (block.x >> 16) & 0xFFu;
	uint s3 = (block.x >> 24) & 0xFFu;
	uint s4 =  block.y        & 0xFFu;
	uint s5 = (block.y >>  8) & 0xFFu;
	uint s6 = (block.y >> 16) & 0xFFu;
	uint s7 = (block.y >> 24) & 0xFFu;

	bool diffBit = (s3 & 2u) != 0u;
	bool flipBit = (s3 & 1u) != 0u;

	// 1-bit alpha: s3[1] is the opaque bit, not ETC1 diffbit.
	//   opaque = 1 → same RGB as ETC2, A = 1
	//   opaque = 0 → punch-through block (selector 2 is RGBA = 0)
	// Individual mode does not exist in the punch-through format.
	bool punchthrough = has1BitAlpha && !diffBit;

	// 2-bit pixel index: MSBs in bytes 4–5, LSBs in bytes 6–7, column-major
	int  p      = x * 4 + y;
	uint part2  = (s4 << 24u) | (s5 << 16u) | (s6 << 8u) | s7;
	int  pixIdx = int(((part2 >> (16 + p)) & 1u) << 1u | ((part2 >> p) & 1u));

	// ---- ETC1 individual (diffbit = 0, RGB8 only) ----
	if (!has1BitAlpha && !diffBit)
	{
		ivec3 c0 = ivec3(int(s0 >> 4u), int(s1 >> 4u), int(s2 >> 4u));
		ivec3 c1 = ivec3(int(s0 & 15u), int(s1 & 15u), int(s2 & 15u));
		c0 = (c0 << 4) | c0;
		c1 = (c1 << 4) | c1;

		int cw0 = int((s3 >> 5u) & 7u);
		int cw1 = int((s3 >> 2u) & 7u);
		bool sb1 = flipBit ? (y >= 2) : (x >= 2);
		ivec3 c = sb1 ? c1 : c0;
		int  m = kMod[(sb1 ? cw1 : cw0) * 4 + pixIdx];
		c = clamp(c + m, 0, 255);
		return float4(vec3(c), 255.0) / float(255.0);
	}

	// 5-bit base + 3-bit two's-complement delta (differential layout)
	// also used when has1BitAlpha (opaque bit replaces diffbit)
	int R = int(s0 >> 3u);  int dR = int(s0 & 7u); if (dR >= 4) dR -= 8;
	int G = int(s1 >> 3u);  int dG = int(s1 & 7u); if (dG >= 4) dG -= 8;
	int B = int(s2 >> 3u);  int dB = int(s2 & 7u); if (dB >= 4) dB -= 8;
	int R1 = R + dR;
	int G1 = G + dG;
	int B1 = B + dB;

	// ---- T mode (R overflows) ----
	if (R1 < 0 || R1 > 31)
	{
		int r0 = int((((s0 >> 3u) & 3u) << 2u) | (s0 & 3u));
		int g0 = int(s1 >> 4u);
		int b0 = int(s1 & 15u);
		int r1 = int(s2 >> 4u);
		int g1 = int(s2 & 15u);
		int b1 = int(s3 >> 4u);
		ivec3 c0 = (ivec3(r0, g0, b0) << 4) | ivec3(r0, g0, b0);
		ivec3 c1 = (ivec3(r1, g1, b1) << 4) | ivec3(r1, g1, b1);

		int d = kDist[int(((s3 & 0x0Cu) >> 1u) | (s3 & 1u))];

		// punch-through: selector 2 is transparent (C0, C1+d, 0, C1-d)
		if (punchthrough && pixIdx == 2)
			return float4(0.0);

		ivec3 c;
		if      (pixIdx == 0) c = c0;
		else if (pixIdx == 2) c = c1;
		else                  c = clamp(c1 + (pixIdx == 1 ? d : -d), 0, 255);
		return float4(vec3(c), 255.0) / float(255.0);
	}

	// ---- H mode (G overflows, R does not) ----
	if (G1 < 0 || G1 > 31)
	{
		int r0 = int((s0 >> 3u) & 15u);
		int g0 = int(((s0 & 7u) << 1u) | ((s1 >> 4u) & 1u));
		int b0 = int((s1 & 8u) | ((s1 & 3u) << 1u) | (s2 >> 7u));
		int r1 = int((s2 >> 3u) & 15u);
		int g1 = int(((s2 & 7u) << 1u) | (s3 >> 7u));
		int b1 = int((s3 >> 3u) & 15u);

		int di = int((s3 & 4u) + 2u * (s3 & 1u));
		if ((r0 * 0x10000 + g0 * 0x100 + b0) >= (r1 * 0x10000 + g1 * 0x100 + b1))
			di += 1;
		int d = kDist[di];

		// punch-through: selector 2 is transparent (C0+d, C0-d, 0, C1-d)
		if (punchthrough && pixIdx == 2)
			return float4(0.0);

		ivec3 c0 = (ivec3(r0, g0, b0) << 4) | ivec3(r0, g0, b0);
		ivec3 c1 = (ivec3(r1, g1, b1) << 4) | ivec3(r1, g1, b1);
		ivec3 base = (pixIdx < 2) ? c0 : c1;
		int   s    = ((pixIdx & 1) == 0) ? d : -d;
		return float4(vec3(clamp(base + s, 0, 255)), 255.0) / float(255.0);
	}

	// ---- Planar mode (B overflows, R and G do not) ----
	// planar never punches through (always opaque)
	if (B1 < 0 || B1 > 31)
	{
		int RO = int((s0 >> 1u) & 63u);
		int GO = int(((s0 & 1u) << 6u) | ((s1 >> 1u) & 63u));
		int BO = int(((s1 & 1u) << 5u) | (((s2 >> 5u) & 3u) << 3u) | (s2 & 7u));
		int RH = int((((s3 >> 2u) & 31u) << 1u) | (s3 & 1u));
		int GH = int((s4 >> 1u) & 127u);
		int BH = int(((s4 & 1u) << 5u) | (s5 >> 3u));
		int RV = int(((s5 & 7u) << 3u) | (s6 >> 5u));
		int GV = int(((s6 & 31u) << 2u) | (s7 >> 6u));
		int BV = int(s7 & 63u);

		RO = (RO << 2) | (RO >> 4);   GO = (GO << 1) | (GO >> 6);   BO = (BO << 2) | (BO >> 4);
		RH = (RH << 2) | (RH >> 4);   GH = (GH << 1) | (GH >> 6);   BH = (BH << 2) | (BH >> 4);
		RV = (RV << 2) | (RV >> 4);   GV = (GV << 1) | (GV >> 6);   BV = (BV << 2) | (BV >> 4);

		int r = (x * (RH - RO) + y * (RV - RO) + 4 * RO + 2) >> 2;
		int g = (x * (GH - GO) + y * (GV - GO) + 4 * GO + 2) >> 2;
		int b = (x * (BH - BO) + y * (BV - BO) + 4 * BO + 2) >> 2;
		return float4(vec3(clamp(ivec3(r, g, b), 0, 255)), 255.0) / float(255.0);
	}

	// ---- ETC1 differential (valid 5-bit pair) ----
	{
		// punch-through: selector 2 is transparent; modifiers become
		// {0, +lg, 0, -lg} instead of {+sm, +lg, -sm, -lg}
		if (punchthrough && pixIdx == 2)
			return float4(0.0);

		ivec3 c0 = ivec3(R,  G,  B);
		ivec3 c1 = ivec3(R1, G1, B1);
		c0 = (c0 << 3) | (c0 >> 2);
		c1 = (c1 << 3) | (c1 >> 2);

		int cw0 = int((s3 >> 5u) & 7u);
		int cw1 = int((s3 >> 2u) & 7u);
		bool sb1 = flipBit ? (y >= 2) : (x >= 2);
		ivec3 c = sb1 ? c1 : c0;
		int  m = kMod[(sb1 ? cw1 : cw0) * 4 + pixIdx];
		if (punchthrough && (pixIdx & 1) == 0)
			m = 0;
		c = clamp(c + m, 0, 255);
		return float4(vec3(c), 255.0) / float(255.0);
	}
}

ND_ float4  DecodeETC2 (uint2 block, int2 localTexel)
{
	return DecodeETC2Impl( block, localTexel, false );
}

ND_ float4  DecodeETC2A1 (uint2 block, int2 localTexel)
{
	return DecodeETC2Impl( block, localTexel, true );
}
