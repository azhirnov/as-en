// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'


ND_ uint  EncodeRGB565 (float3 color)
{
	uint3	rgb = uint3( Saturate( color ) * 255.0 );
	uint	r	= rgb.r >> 3u;
	uint	g	= rgb.g >> 2u;
	uint	b	= rgb.b >> 3u;

	return (r << 11u) | (g << 5u) | b;
}

ND_ float3  DecodeRGB565 (uint c)
{
	uint r = (c >> 11) & 0x1Fu;
	uint g = (c >>  5) & 0x3Fu;
	uint b =  c        & 0x1Fu;

	uint3 rgb8 = uint3(
		(r << 3) | (r >> 2),
		(g << 2) | (g >> 4),
		(b << 3) | (b >> 2)
	);

	return float3(rgb8) / float(255.0);
}

ND_ float4  DecodeBC1 (uint2 block, int2 localTexel)
{
	uint	color0Bits =  block.x         & 0xFFFFu;
	uint	color1Bits = (block.x >> 16u) & 0xFFFFu;

	float3	color0 = DecodeRGB565( color0Bits );
	float3	color1 = DecodeRGB565( color1Bits );

	float4	palette[4];
	palette[0] = float4(color0, 1.0);
	palette[1] = float4(color1, 1.0);

	// warning: HW may use low-precision interpolation
	if ( color0Bits > color1Bits )
	{
		// opaque 4-color mode
		palette[2] = float4( (float(2.0) * color0 + color1) / float(3.0), float(1.0) );
		palette[3] = float4( (color0 + float(2.0) * color1) / float(3.0), float(1.0) );
	}
	else
	{
		// 3-color mode with transparent palette entry 3
		palette[2] = float4( (color0 + color1) * float(0.5), 1.0 );
		palette[3] = float4(0.0);	// alpha=1.0 for RGB format
	}

	uint	texelIndex		= uint(localTexel.y * 4 + localTexel.x);
	uint	paletteIndex	= (block.y >> (2u * texelIndex)) & 3u;

	return palette[ paletteIndex ];
}
