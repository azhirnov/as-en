
// Position
#define VB_Position_f2_DEFINED
#define VB_Position_s2_DEFINED
#define VB_Position_f3_DEFINED
#define VB_Position_f4_DEFINED


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



// Attributes
#define VB_UVf2_Col8_DEFINED
#define VB_UVs2_Col8_DEFINED
#define VB_Color8_DEFINED


// size: 12, align: 4
struct VB_UVf2_Col8
{
	packed_float2	UV;
	ubyte4			Color;
	
	VB_UVf2_Col8 () {}
	VB_UVf2_Col8 (const packed_float2 &uv, const ubyte4 &col) : UV{uv}, Color{col} {}
	VB_UVf2_Col8 (const packed_float2 &uv, const RGBA8u &col) : UV{uv}, Color{col} {}
};
STATIC_ASSERT( sizeof(VB_UVf2_Col8) == 12 );


// size: 8, align: 2
struct VB_UVs2_Col8
{
	packed_ushort2	UV;
	ubyte4			Color;
	
	VB_UVs2_Col8 () {}
	VB_UVs2_Col8 (const packed_ushort2 &uv, const ubyte4 &col) : UV{uv}, Color{col} {}
	VB_UVs2_Col8 (const packed_ushort2 &uv, const RGBA8u &col) : UV{uv}, Color{col} {}
};
STATIC_ASSERT( sizeof(VB_UVs2_Col8) == 8 );


// size: 4, align: 1
struct VB_Color8
{
	ubyte4			Color;
	
	VB_Color8 () {}
	VB_Color8 (const ubyte4 &col) : Color{col} {}
	VB_Color8 (const RGBA8u &col) : Color{col} {}
};
STATIC_ASSERT( sizeof(VB_Color8) == 4 );


// size: 12, align: 2
struct VB_UVs2_SCs1_Col8
{
	packed_ushort4	UV_Scale;
	ubyte4			Color;
	
	VB_UVs2_SCs1_Col8 () {}
	VB_UVs2_SCs1_Col8 (const packed_ushort2 &uv, ushort scale, const ubyte4 &col) : UV_Scale{uv, scale, 0}, Color{col} {}
	VB_UVs2_SCs1_Col8 (const packed_ushort2 &uv, ushort scale, const RGBA8u &col) : UV_Scale{uv, scale, 0}, Color{col} {}
};
STATIC_ASSERT( sizeof(VB_UVs2_SCs1_Col8) == 12 );

