#ifndef CameraData_DEFINED
#	define CameraData_DEFINED
	// size: 368, align: 16
	struct CameraData
	{
		static constexpr auto  TypeName = ShaderStructName{"CameraData"};

		float4x4_storage  viewProj;
		float4x4_storage  invViewProj;
		float4x4_storage  proj;
		float4x4_storage  view;
		float3  pos;
		StaticArray< float4, 6 >    frustum;
	};
	STATIC_ASSERT( offsetof(CameraData, viewProj) == 0 );
	STATIC_ASSERT( offsetof(CameraData, invViewProj) == 64 );
	STATIC_ASSERT( offsetof(CameraData, proj) == 128 );
	STATIC_ASSERT( offsetof(CameraData, view) == 192 );
	STATIC_ASSERT( offsetof(CameraData, pos) == 256 );
	STATIC_ASSERT( offsetof(CameraData, frustum) == 272 );
	STATIC_ASSERT( sizeof(CameraData) == 368 );
#endif

#ifndef CameraData_DEFINED
#	define CameraData_DEFINED
	// size: 368, align: 16
	struct CameraData
	{
		static constexpr auto  TypeName = ShaderStructName{"CameraData"};

		float4x4_storage  viewProj;
		float4x4_storage  invViewProj;
		float4x4_storage  proj;
		float4x4_storage  view;
		float3  pos;
		StaticArray< float4, 6 >    frustum;
	};
	STATIC_ASSERT( offsetof(CameraData, viewProj) == 0 );
	STATIC_ASSERT( offsetof(CameraData, invViewProj) == 64 );
	STATIC_ASSERT( offsetof(CameraData, proj) == 128 );
	STATIC_ASSERT( offsetof(CameraData, view) == 192 );
	STATIC_ASSERT( offsetof(CameraData, pos) == 256 );
	STATIC_ASSERT( offsetof(CameraData, frustum) == 272 );
	STATIC_ASSERT( sizeof(CameraData) == 368 );
#endif

#ifndef ShadertoyUB_DEFINED
#	define ShadertoyUB_DEFINED
	// size: 848, align: 16
	struct ShadertoyUB
	{
		static constexpr auto  TypeName = ShaderStructName{"ShadertoyUB"};

		float3  resolution;
		float  time;
		float  timeDelta;
		int  frame;
		float4  channelTime;
		StaticArray< float4, 4 >    channelResolution;
		float4  mouse;
		float4  date;
		float  sampleRate;
		CameraData  camera;
		StaticArray< float4, 4 >    floatSliders;
		StaticArray< int4, 4 >    intSliders;
		StaticArray< float4, 4 >    colors;
		StaticArray< float4, 4 >    floatConst;
		StaticArray< int4, 4 >    intConst;
	};
	STATIC_ASSERT( offsetof(ShadertoyUB, resolution) == 0 );
	STATIC_ASSERT( offsetof(ShadertoyUB, time) == 16 );
	STATIC_ASSERT( offsetof(ShadertoyUB, timeDelta) == 20 );
	STATIC_ASSERT( offsetof(ShadertoyUB, frame) == 24 );
	STATIC_ASSERT( offsetof(ShadertoyUB, channelTime) == 32 );
	STATIC_ASSERT( offsetof(ShadertoyUB, channelResolution) == 48 );
	STATIC_ASSERT( offsetof(ShadertoyUB, mouse) == 112 );
	STATIC_ASSERT( offsetof(ShadertoyUB, date) == 128 );
	STATIC_ASSERT( offsetof(ShadertoyUB, sampleRate) == 144 );
	STATIC_ASSERT( offsetof(ShadertoyUB, camera) == 160 );
	STATIC_ASSERT( offsetof(ShadertoyUB, floatSliders) == 528 );
	STATIC_ASSERT( offsetof(ShadertoyUB, intSliders) == 592 );
	STATIC_ASSERT( offsetof(ShadertoyUB, colors) == 656 );
	STATIC_ASSERT( offsetof(ShadertoyUB, floatConst) == 720 );
	STATIC_ASSERT( offsetof(ShadertoyUB, intConst) == 784 );
	STATIC_ASSERT( sizeof(ShadertoyUB) == 848 );
#endif

#ifndef ComputePassUB_DEFINED
#	define ComputePassUB_DEFINED
	// size: 720, align: 16
	struct ComputePassUB
	{
		static constexpr auto  TypeName = ShaderStructName{"ComputePassUB"};

		float  time;
		float  timeDelta;
		int  frame;
		float4  mouse;
		CameraData  camera;
		StaticArray< float4, 4 >    floatSliders;
		StaticArray< int4, 4 >    intSliders;
		StaticArray< float4, 4 >    colors;
		StaticArray< float4, 4 >    floatConst;
		StaticArray< int4, 4 >    intConst;
	};
	STATIC_ASSERT( offsetof(ComputePassUB, time) == 0 );
	STATIC_ASSERT( offsetof(ComputePassUB, timeDelta) == 4 );
	STATIC_ASSERT( offsetof(ComputePassUB, frame) == 8 );
	STATIC_ASSERT( offsetof(ComputePassUB, mouse) == 16 );
	STATIC_ASSERT( offsetof(ComputePassUB, camera) == 32 );
	STATIC_ASSERT( offsetof(ComputePassUB, floatSliders) == 400 );
	STATIC_ASSERT( offsetof(ComputePassUB, intSliders) == 464 );
	STATIC_ASSERT( offsetof(ComputePassUB, colors) == 528 );
	STATIC_ASSERT( offsetof(ComputePassUB, floatConst) == 592 );
	STATIC_ASSERT( offsetof(ComputePassUB, intConst) == 656 );
	STATIC_ASSERT( sizeof(ComputePassUB) == 720 );
#endif

#ifndef ScenePassUB_DEFINED
#	define ScenePassUB_DEFINED
	// size: 704, align: 16
	struct ScenePassUB
	{
		static constexpr auto  TypeName = ShaderStructName{"ScenePassUB"};

		float2  resolution;
		float  time;
		CameraData  camera;
		StaticArray< float4, 4 >    floatSliders;
		StaticArray< int4, 4 >    intSliders;
		StaticArray< float4, 4 >    colors;
		StaticArray< float4, 4 >    floatConst;
		StaticArray< int4, 4 >    intConst;
	};
	STATIC_ASSERT( offsetof(ScenePassUB, resolution) == 0 );
	STATIC_ASSERT( offsetof(ScenePassUB, time) == 8 );
	STATIC_ASSERT( offsetof(ScenePassUB, camera) == 16 );
	STATIC_ASSERT( offsetof(ScenePassUB, floatSliders) == 384 );
	STATIC_ASSERT( offsetof(ScenePassUB, intSliders) == 448 );
	STATIC_ASSERT( offsetof(ScenePassUB, colors) == 512 );
	STATIC_ASSERT( offsetof(ScenePassUB, floatConst) == 576 );
	STATIC_ASSERT( offsetof(ScenePassUB, intConst) == 640 );
	STATIC_ASSERT( sizeof(ScenePassUB) == 704 );
#endif

#ifndef SphericalCubeMaterialUB_DEFINED
#	define SphericalCubeMaterialUB_DEFINED
	// size: 64, align: 16
	struct SphericalCubeMaterialUB
	{
		static constexpr auto  TypeName = ShaderStructName{"SphericalCubeMaterialUB"};

		float4x4_storage  transform;
	};
	STATIC_ASSERT( offsetof(SphericalCubeMaterialUB, transform) == 0 );
	STATIC_ASSERT( sizeof(SphericalCubeMaterialUB) == 64 );
#endif

#ifndef UnifiedGeometryMaterialUB_DEFINED
#	define UnifiedGeometryMaterialUB_DEFINED
	// size: 64, align: 16
	struct UnifiedGeometryMaterialUB
	{
		static constexpr auto  TypeName = ShaderStructName{"UnifiedGeometryMaterialUB"};

		float4x4_storage  transform;
	};
	STATIC_ASSERT( offsetof(UnifiedGeometryMaterialUB, transform) == 0 );
	STATIC_ASSERT( sizeof(UnifiedGeometryMaterialUB) == 64 );
#endif

