#ifndef VB_3_Pos_DEFINED
#	define VB_3_Pos_DEFINED
	// size: 12, align: 4 (16)
	struct VB_3_Pos
	{
		static constexpr auto  TypeName = ShaderStructName{"VB_3_Pos"};

		packed_float3  Position;
	};
	StaticAssert( offsetof(VB_3_Pos, Position) == 0 );
	StaticAssert( sizeof(VB_3_Pos) == 12 );
#endif

#ifndef PushConst1_DEFINED
#	define PushConst1_DEFINED
	// size: 16, align: 8 (16)
	struct PushConst1
	{
		static constexpr auto  TypeName = ShaderStructName{"PushConst1"};

		float2  scale;
		float2  bias;
	};
	StaticAssert( offsetof(PushConst1, scale) == 0 );
	StaticAssert( offsetof(PushConst1, bias) == 8 );
	StaticAssert( sizeof(PushConst1) == 16 );
#endif

#ifndef PushConst2_DEFINED
#	define PushConst2_DEFINED
	// size: 16, align: 16
	struct PushConst2
	{
		static constexpr auto  TypeName = ShaderStructName{"PushConst2"};

		float4  color;
	};
	StaticAssert( offsetof(PushConst2, color) == 0 );
	StaticAssert( sizeof(PushConst2) == 16 );
#endif

#ifndef UBlock_DEFINED
#	define UBlock_DEFINED
	// size: 64, align: 16
	struct UBlock
	{
		static constexpr auto  TypeName = ShaderStructName{"UBlock"};

		float4x4_storage  mvp;
	};
	StaticAssert( offsetof(UBlock, mvp) == 0 );
	StaticAssert( sizeof(UBlock) == 64 );
#endif

#ifndef BufferRef_DEFINED
#	define BufferRef_DEFINED
	// size: 24 (32), align: 16
	struct BufferRef
	{
		static constexpr auto  TypeName = ShaderStructName{"BufferRef"};

		float3  Position;
		float2  Texcoord;
	};
	StaticAssert( offsetof(BufferRef, Position) == 0 );
	StaticAssert( offsetof(BufferRef, Texcoord) == 16 );
	StaticAssert( sizeof(BufferRef) == 32 );
#endif

#ifndef vb_input1_DEFINED
#	define vb_input1_DEFINED
	// size: 16, align: 4 (16)
	struct vb_input1
	{
		static constexpr auto  TypeName = ShaderStructName{"vb_input1"};

		packed_float3  Position;
		packed_ushort2  Texcoord;
	};
	StaticAssert( offsetof(vb_input1, Position) == 0 );
	StaticAssert( offsetof(vb_input1, Texcoord) == 12 );
	StaticAssert( sizeof(vb_input1) == 16 );
#endif

#ifndef VB_3_Attribs_DEFINED
#	define VB_3_Attribs_DEFINED
	// size: 8, align: 4 (16)
	struct VB_3_Attribs
	{
		static constexpr auto  TypeName = ShaderStructName{"VB_3_Attribs"};

		packed_float2  Texcoord;
	};
	StaticAssert( offsetof(VB_3_Attribs, Texcoord) == 0 );
	StaticAssert( sizeof(VB_3_Attribs) == 8 );
#endif

