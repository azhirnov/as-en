// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

ND_ float4  DecodeEAC_Ru (uint2 block, int2 localTexel)
{
	// header: byte0 = base, byte1 = mul:4 | table:4  (LE uint2)
	int		base	 = int( bitfieldExtract( block.x,  0, 8 ));
	uint	tableIdx =      bitfieldExtract( block.x,  8, 4 );
	int		mul		 = int( bitfieldExtract( block.x, 12, 4 ));
			mul		 = (mul == 0) ? 1 : mul * 8;

	// 3-bit indices, 48 bits, column-major (x*4+y), MSB-first in bytes 2..7
	uint	pixel  = uint(localTexel.x & 3) * 4u + uint(localTexel.y & 3);

	uint	idxHi  = ((block.x >> 16) & 0xFFu) << 8 | (block.x >> 24);				// bits 47..32
	uint	idxLo  =  (block.y & 0xFFu) << 24 | ((block.y >>  8) & 0xFFu) << 16
					| ((block.y >> 16) & 0xFFu) <<  8 |  (block.y >> 24);			// bits 31..0

	uint	lsbPos = 45u - 3u * pixel;
	uint	index;
	if ( lsbPos >= 32u )
		index = (idxHi >> (lsbPos - 32u)) & 7u;
	else
	if ( lsbPos <= 29u )
		index = (idxLo >>  lsbPos)        & 7u;
	else
		index = ((idxLo >> lsbPos) | (idxHi << (32u - lsbPos))) & 7u;			// pixel 5 spans the split

	// modifier table, packed as 4 magnitudes (4 bits each); cols 4..7 = mag-1
	//  { -3,-6,-9,-15, 2, 5, 8,14 }
	//  { -3,-7,-10,-13, 2, 6, 9,12 }
	//  ...
	const uint packedMags[16] = uint[](
		0xF963u, 0xDA73u, 0xD852u, 0xD642u,
		0xC863u, 0xB973u, 0xB874u, 0xB853u,
		0xA862u, 0xA852u, 0xA842u, 0xA752u,
		0xA743u, 0xA321u, 0x9864u, 0x9753u
	);
	uint	mag		 = (packedMags[tableIdx] >> ((index & 3u) * 4u)) & 15u;
	int		modifier = (index < 4u) ? -int(mag) : int(mag) - 1;
	int		value	 = clamp( base * 8 + 4 + modifier * mul, 0, 2047 );

	return float4( float(value) * float(1.0 / 2047.0), 0.0, 0.0, 1.0 );
}

ND_ float4  DecodeEAC_Rs (uint2 block, int2 localTexel)
{
	// header: byte0 = signed base, byte1 = mul:4 | table:4  (LE uint2)
	int		base	 = bitfieldExtract( int(block.x), 0, 8 );					// sign-extended
			base	 = (base == -128) ? -127 : base;							// reserved codeword
	uint	tableIdx =      bitfieldExtract( block.x,  8, 4 );
	int		mul		 = int( bitfieldExtract( block.x, 12, 4 ));
			mul		 = (mul == 0) ? 1 : mul * 8;

	// 3-bit indices, 48 bits, column-major (x*4+y), MSB-first in bytes 2..7
	uint pixel  = uint(localTexel.x & 3) * 4u + uint(localTexel.y & 3);

	uint idxHi  = ((block.x >> 16) & 0xFFu) << 8 | (block.x >> 24);			// bits 47..32
	uint idxLo  =  (block.y & 0xFFu) << 24 | ((block.y >>  8) & 0xFFu) << 16
				| ((block.y >> 16) & 0xFFu) <<  8 |  (block.y >> 24);		// bits 31..0

	uint lsbPos = 45u - 3u * pixel;
	uint index;
	if ( lsbPos >= 32u )
		index = (idxHi >> (lsbPos - 32u)) & 7u;
	else
	if ( lsbPos <= 29u )
		index = (idxLo >>  lsbPos) & 7u;
	else
		index = ((idxLo >> lsbPos) | (idxHi << (32u - lsbPos))) & 7u;	// pixel 5 spans the split

	const uint packedMags[16] = uint[](
		0xF963u, 0xDA73u, 0xD852u, 0xD642u,
		0xC863u, 0xB973u, 0xB874u, 0xB853u,
		0xA862u, 0xA852u, 0xA842u, 0xA752u,
		0xA743u, 0xA321u, 0x9864u, 0x9753u
	);
	uint	mag		 = (packedMags[tableIdx] >> ((index & 3u) * 4u)) & 15u;
	int		modifier = (index < 4u) ? -int(mag) : int(mag) - 1;
	int		value	 = clamp( base * 8 + modifier * mul, -1023, 1023 );	// no +4 bias; clamp to signed 11-bit, map to [-1, 1]

	return float4( float(value) * float(1.0 / 1023.0), 0.0, 0.0, 1.0 );
}
