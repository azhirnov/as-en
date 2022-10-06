#ifndef UBlock_DEFINED
#	define UBlock_DEFINED
	// size: 64, align: 16
	struct UBlock
	{
		static constexpr auto  TypeName = ShaderStructName{"UBlock"};

		float4x4_storage  mvp;
	};
	STATIC_ASSERT( offsetof(UBlock, mvp) == 0 );
	STATIC_ASSERT( sizeof(UBlock) == 64 );
#endif

#ifndef VBInput2_DEFINED
#	define VBInput2_DEFINED
	// size: 12, align: 16
	struct VBInput2
	{
		static constexpr auto  TypeName = ShaderStructName{"VBInput2"};

		packed_float2  Position;
		packed_ushort2  Texcoord;
	};
	STATIC_ASSERT( offsetof(VBInput2, Position) == 0 );
	STATIC_ASSERT( offsetof(VBInput2, Texcoord) == 8 );
	STATIC_ASSERT( sizeof(VBInput2) == 12 );
#endif

#ifndef PushConst1_DEFINED
#	define PushConst1_DEFINED
	// size: 16, align: 16
	struct PushConst1
	{
		static constexpr auto  TypeName = ShaderStructName{"PushConst1"};

		float2  scale;
		float2  bias;
	};
	STATIC_ASSERT( offsetof(PushConst1, scale) == 0 );
	STATIC_ASSERT( offsetof(PushConst1, bias) == 8 );
	STATIC_ASSERT( sizeof(PushConst1) == 16 );
#endif

#ifndef VBInput1_DEFINED
#	define VBInput1_DEFINED
	// size: 16, align: 16
	struct VBInput1
	{
		static constexpr auto  TypeName = ShaderStructName{"VBInput1"};

		packed_float3  Position;
		packed_ushort2  Texcoord;
	};
	STATIC_ASSERT( offsetof(VBInput1, Position) == 0 );
	STATIC_ASSERT( offsetof(VBInput1, Texcoord) == 12 );
	STATIC_ASSERT( sizeof(VBInput1) == 16 );
#endif

#ifndef PushConst2_DEFINED
#	define PushConst2_DEFINED
	// size: 16, align: 16
	struct PushConst2
	{
		static constexpr auto  TypeName = ShaderStructName{"PushConst2"};

		float4  color;
	};
	STATIC_ASSERT( offsetof(PushConst2, color) == 0 );
	STATIC_ASSERT( sizeof(PushConst2) == 16 );
#endif

