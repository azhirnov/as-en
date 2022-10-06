
#define VB_Position_f2_DEFINED
#define VB_Position_s2_DEFINED
#define VB_Position_f3_DEFINED
#define VB_Position_f4_DEFINED
#define VB_UVf2_Col_DEFINED
#define VB_Color_DEFINED


// size: 8, align: 4
struct VB_Position_f2
{
	packed_float2	Position;
};
STATIC_ASSERT( sizeof(VB_Position_f2) == 8 );


// size: 4, align: 2
struct VB_Position_s2
{
	packed_short2	Position;
};
STATIC_ASSERT( sizeof(VB_Position_s2) == 4 );


// size: 12, align: 4
struct VB_Position_f3
{
	packed_float3	Position;
};
STATIC_ASSERT( sizeof(VB_Position_f3) == 12 );


// size: 16, align: 4
struct VB_Position_f4
{
	packed_float4	Position;
};
STATIC_ASSERT( sizeof(VB_Position_f4) == 16 );


// size: 12, align: 4
struct VB_UVf2_Col
{
	packed_float2	UV;
	ubyte4			Color;
	
	VB_UVf2_Col () {}
	VB_UVf2_Col (const packed_float2 &uv, const ubyte4 &col) : UV{uv}, Color{col} {}
	VB_UVf2_Col (const packed_float2 &uv, const RGBA8u &col) : UV{uv}, Color{col} {}
};
STATIC_ASSERT( sizeof(VB_UVf2_Col) == 12 );


// size: 4, align: 1
struct VB_Color
{
	ubyte4			Color;
	
	VB_Color () {}
	VB_Color (const ubyte4 &col) : Color{col} {}
	VB_Color (const RGBA8u &col) : Color{col} {}
};
STATIC_ASSERT( sizeof(VB_Color) == 4 );

