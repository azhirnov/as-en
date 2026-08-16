
#ifdef AE_LICENSE_MIT

	// based on code from https://github.com/Themaister/Granite/blob/master/assets/shaders/decode/rgtc.h (MIT License)
	float  decode_alpha_rgtc (uint2 payload, int bit_offset)
	{
		float	ep0			= float(payload.x & 0xFFu) / float(255.0);
		float	ep1			= float(((payload.x >> 8) & 0xFFu)) / float(255.0);
		uint	bits;

		if ( bit_offset <= 29 )		bits = bitfieldExtract( payload.x, bit_offset, 3 );			else
		if ( bit_offset >= 32 )		bits = bitfieldExtract( payload.y, bit_offset - 32, 3 );	else
									bits = bitfieldExtract( payload.x, 31, 1 ) | ((payload.y & 3) << 1);	// only 31, 30 never occurs

		float res;
		if ( bits == 0 )	res = ep0;													else
		if ( bits == 1 )	res = ep1;													else
		if ( ep0 > ep1 )	res = Lerp( ep0, ep1, float(1.0 / 7.0) * float(bits - 1) );	else
		if ( bits == 6 )	res = float(0.0);											else
		if ( bits == 7 )	res = float(1.0);											else
							res = Lerp( ep0, ep1, float(1.0 / 5.0) * float(bits - 1) );
		return res;
	}

	ND_ float4  DecodeBC4U (uint2 block, int2 localTexel)
	{
		int		bit_offset	= 16 + (4 * localTexel.y + localTexel.x) * 3;
		float	r			 = decode_alpha_rgtc( block, bit_offset );
		return	float4(r, 0.0, 0.0, 1.0);
	}

	ND_ float4  DecodeBC5U (uint4 block, int2 localTexel)
	{
		int		bit_offset	= 16 + (4 * localTexel.y + localTexel.x) * 3;
		float	r			 = decode_alpha_rgtc( block.xy, bit_offset );
		float	g			 = decode_alpha_rgtc( block.zw, bit_offset );
		return	float4( r, g, 0.0, 1.0 );
	}


	float  decode_alpha_rgtc_snorm (uint2 payload, int bit_offset)
	{
		int	ep0i = int(payload.x & 0xffu);
		int	ep1i = int((payload.x >> 8) & 0xffu);

		if ( ep0i >= 128 )	ep0i -= 256;
		if ( ep1i >= 128 )	ep1i -= 256;

		float	ep0		= Max( float(ep0i) / float(127.0), float(-1.0) );
		float	ep1		= Max( float(ep1i) / float(127.0), float(-1.0) );
		uint	bits;

		if ( bit_offset <= 29 )	bits = bitfieldExtract( payload.x, bit_offset, 3 );			else
		if ( bit_offset >= 32 ) bits = bitfieldExtract( payload.y, bit_offset - 32, 3 );	else
								bits = bitfieldExtract( payload.x, 31, 1 ) | ((payload.y & 3) << 1);	// only 31, 30 never occurs

		float res;
		if ( bits == 0 )	res = ep0;													else
		if ( bits == 1 )	res = ep1;													else
		if ( ep0 > ep1 )	res = Lerp( ep0, ep1, float(1.0 / 7.0) * float(bits - 1) );	else
		if ( bits == 6 )	res = float(-1.0);											else
		if ( bits == 7 )	res = float( 1.0);											else
							res = Lerp( ep0, ep1, float(1.0 / 5.0) * float(bits - 1) );

		return Clamp( res, float(-1.0), float(1.0) );
	}

	ND_ float4  DecodeBC4S (uint2 block, int2 localTexel)
	{
		int		bit_offset	= 16 + (4 * localTexel.y + localTexel.x) * 3;
		float	r			 = decode_alpha_rgtc_snorm( block, bit_offset );
		return	float4(r, 0.0, 0.0, 1.0);
	}

	ND_ float4  DecodeBC5S (uint4 block, int2 localTexel)
	{
		int		bit_offset	= 16 + (4 * localTexel.y + localTexel.x) * 3;
		float	r			 = decode_alpha_rgtc_snorm( block.xy, bit_offset );
		float	g			 = decode_alpha_rgtc_snorm( block.zw, bit_offset );
		return	float4( r, g, 0.0, 1.0 );
	}

#endif // AE_LICENSE_MIT
