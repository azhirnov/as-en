#ifndef CameraData_DEFINED
#	define CameraData_DEFINED
	// size: 384, align: 16
	struct CameraData
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x8142e66cu}};  // 'CameraData'

		float4x4_storage  viewProj;
		float4x4_storage  invViewProj;
		float4x4_storage  proj;
		float4x4_storage  view;
		float3  pos;
		float2  clipPlanes;
		StaticArray< float4, 6 >    frustum;
	};
#endif
	STATIC_ASSERT( offsetof(CameraData, viewProj) == 0 );
	STATIC_ASSERT( offsetof(CameraData, invViewProj) == 64 );
	STATIC_ASSERT( offsetof(CameraData, proj) == 128 );
	STATIC_ASSERT( offsetof(CameraData, view) == 192 );
	STATIC_ASSERT( offsetof(CameraData, pos) == 256 );
	STATIC_ASSERT( offsetof(CameraData, clipPlanes) == 272 );
	STATIC_ASSERT( offsetof(CameraData, frustum) == 288 );
	STATIC_ASSERT( sizeof(CameraData) == 384 );

#ifndef ShadertoyUB_DEFINED
#	define ShadertoyUB_DEFINED
	// size: 864, align: 16
	struct ShadertoyUB
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xa31fc14bu}};  // 'ShadertoyUB'

		float3  resolution;
		float  time;
		float  timeDelta;
		uint  frame;
		uint  seed;
		float4  channelTime;
		StaticArray< float4, 4 >    channelResolution;
		float4  mouse;
		float4  date;
		float  sampleRate;
		float  customKeys;
		CameraData  camera;
		StaticArray< float4, 4 >    floatSliders;
		StaticArray< int4, 4 >    intSliders;
		StaticArray< float4, 4 >    colors;
		StaticArray< float4, 4 >    floatConst;
		StaticArray< int4, 4 >    intConst;
	};
#endif
	STATIC_ASSERT( offsetof(ShadertoyUB, resolution) == 0 );
	STATIC_ASSERT( offsetof(ShadertoyUB, time) == 16 );
	STATIC_ASSERT( offsetof(ShadertoyUB, timeDelta) == 20 );
	STATIC_ASSERT( offsetof(ShadertoyUB, frame) == 24 );
	STATIC_ASSERT( offsetof(ShadertoyUB, seed) == 28 );
	STATIC_ASSERT( offsetof(ShadertoyUB, channelTime) == 32 );
	STATIC_ASSERT( offsetof(ShadertoyUB, channelResolution) == 48 );
	STATIC_ASSERT( offsetof(ShadertoyUB, mouse) == 112 );
	STATIC_ASSERT( offsetof(ShadertoyUB, date) == 128 );
	STATIC_ASSERT( offsetof(ShadertoyUB, sampleRate) == 144 );
	STATIC_ASSERT( offsetof(ShadertoyUB, customKeys) == 148 );
	STATIC_ASSERT( offsetof(ShadertoyUB, camera) == 160 );
	STATIC_ASSERT( offsetof(ShadertoyUB, floatSliders) == 544 );
	STATIC_ASSERT( offsetof(ShadertoyUB, intSliders) == 608 );
	STATIC_ASSERT( offsetof(ShadertoyUB, colors) == 672 );
	STATIC_ASSERT( offsetof(ShadertoyUB, floatConst) == 736 );
	STATIC_ASSERT( offsetof(ShadertoyUB, intConst) == 800 );
	STATIC_ASSERT( sizeof(ShadertoyUB) == 864 );

#ifndef ComputePassUB_DEFINED
#	define ComputePassUB_DEFINED
	// size: 752, align: 16
	struct ComputePassUB
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xaba36a57u}};  // 'ComputePassUB'

		float  time;
		float  timeDelta;
		uint  frame;
		uint  seed;
		float4  mouse;
		float  customKeys;
		CameraData  camera;
		StaticArray< float4, 4 >    floatSliders;
		StaticArray< int4, 4 >    intSliders;
		StaticArray< float4, 4 >    colors;
		StaticArray< float4, 4 >    floatConst;
		StaticArray< int4, 4 >    intConst;
	};
#endif
	STATIC_ASSERT( offsetof(ComputePassUB, time) == 0 );
	STATIC_ASSERT( offsetof(ComputePassUB, timeDelta) == 4 );
	STATIC_ASSERT( offsetof(ComputePassUB, frame) == 8 );
	STATIC_ASSERT( offsetof(ComputePassUB, seed) == 12 );
	STATIC_ASSERT( offsetof(ComputePassUB, mouse) == 16 );
	STATIC_ASSERT( offsetof(ComputePassUB, customKeys) == 32 );
	STATIC_ASSERT( offsetof(ComputePassUB, camera) == 48 );
	STATIC_ASSERT( offsetof(ComputePassUB, floatSliders) == 432 );
	STATIC_ASSERT( offsetof(ComputePassUB, intSliders) == 496 );
	STATIC_ASSERT( offsetof(ComputePassUB, colors) == 560 );
	STATIC_ASSERT( offsetof(ComputePassUB, floatConst) == 624 );
	STATIC_ASSERT( offsetof(ComputePassUB, intConst) == 688 );
	STATIC_ASSERT( sizeof(ComputePassUB) == 752 );

#ifndef RayTracingPassUB_DEFINED
#	define RayTracingPassUB_DEFINED
	// size: 752, align: 16
	struct RayTracingPassUB
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x1539319au}};  // 'RayTracingPassUB'

		float  time;
		float  timeDelta;
		uint  frame;
		uint  seed;
		float4  mouse;
		float  customKeys;
		CameraData  camera;
		StaticArray< float4, 4 >    floatSliders;
		StaticArray< int4, 4 >    intSliders;
		StaticArray< float4, 4 >    colors;
		StaticArray< float4, 4 >    floatConst;
		StaticArray< int4, 4 >    intConst;
	};
#endif
	STATIC_ASSERT( offsetof(RayTracingPassUB, time) == 0 );
	STATIC_ASSERT( offsetof(RayTracingPassUB, timeDelta) == 4 );
	STATIC_ASSERT( offsetof(RayTracingPassUB, frame) == 8 );
	STATIC_ASSERT( offsetof(RayTracingPassUB, seed) == 12 );
	STATIC_ASSERT( offsetof(RayTracingPassUB, mouse) == 16 );
	STATIC_ASSERT( offsetof(RayTracingPassUB, customKeys) == 32 );
	STATIC_ASSERT( offsetof(RayTracingPassUB, camera) == 48 );
	STATIC_ASSERT( offsetof(RayTracingPassUB, floatSliders) == 432 );
	STATIC_ASSERT( offsetof(RayTracingPassUB, intSliders) == 496 );
	STATIC_ASSERT( offsetof(RayTracingPassUB, colors) == 560 );
	STATIC_ASSERT( offsetof(RayTracingPassUB, floatConst) == 624 );
	STATIC_ASSERT( offsetof(RayTracingPassUB, intConst) == 688 );
	STATIC_ASSERT( sizeof(RayTracingPassUB) == 752 );

#ifndef SceneGraphicsPassUB_DEFINED
#	define SceneGraphicsPassUB_DEFINED
	// size: 736, align: 16
	struct SceneGraphicsPassUB
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x1f8a4833u}};  // 'SceneGraphicsPassUB'

		float2  resolution;
		float  time;
		float  timeDelta;
		uint  frame;
		uint  seed;
		CameraData  camera;
		StaticArray< float4, 4 >    floatSliders;
		StaticArray< int4, 4 >    intSliders;
		StaticArray< float4, 4 >    colors;
		StaticArray< float4, 4 >    floatConst;
		StaticArray< int4, 4 >    intConst;
	};
#endif
	STATIC_ASSERT( offsetof(SceneGraphicsPassUB, resolution) == 0 );
	STATIC_ASSERT( offsetof(SceneGraphicsPassUB, time) == 8 );
	STATIC_ASSERT( offsetof(SceneGraphicsPassUB, timeDelta) == 12 );
	STATIC_ASSERT( offsetof(SceneGraphicsPassUB, frame) == 16 );
	STATIC_ASSERT( offsetof(SceneGraphicsPassUB, seed) == 20 );
	STATIC_ASSERT( offsetof(SceneGraphicsPassUB, camera) == 32 );
	STATIC_ASSERT( offsetof(SceneGraphicsPassUB, floatSliders) == 416 );
	STATIC_ASSERT( offsetof(SceneGraphicsPassUB, intSliders) == 480 );
	STATIC_ASSERT( offsetof(SceneGraphicsPassUB, colors) == 544 );
	STATIC_ASSERT( offsetof(SceneGraphicsPassUB, floatConst) == 608 );
	STATIC_ASSERT( offsetof(SceneGraphicsPassUB, intConst) == 672 );
	STATIC_ASSERT( sizeof(SceneGraphicsPassUB) == 736 );

#ifndef SceneRayTracingPassUB_DEFINED
#	define SceneRayTracingPassUB_DEFINED
	// size: 720, align: 16
	struct SceneRayTracingPassUB
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xd09ba9b0u}};  // 'SceneRayTracingPassUB'

		float  time;
		float  timeDelta;
		uint  frame;
		uint  seed;
		CameraData  camera;
		StaticArray< float4, 4 >    floatSliders;
		StaticArray< int4, 4 >    intSliders;
		StaticArray< float4, 4 >    colors;
		StaticArray< float4, 4 >    floatConst;
		StaticArray< int4, 4 >    intConst;
	};
#endif
	STATIC_ASSERT( offsetof(SceneRayTracingPassUB, time) == 0 );
	STATIC_ASSERT( offsetof(SceneRayTracingPassUB, timeDelta) == 4 );
	STATIC_ASSERT( offsetof(SceneRayTracingPassUB, frame) == 8 );
	STATIC_ASSERT( offsetof(SceneRayTracingPassUB, seed) == 12 );
	STATIC_ASSERT( offsetof(SceneRayTracingPassUB, camera) == 16 );
	STATIC_ASSERT( offsetof(SceneRayTracingPassUB, floatSliders) == 400 );
	STATIC_ASSERT( offsetof(SceneRayTracingPassUB, intSliders) == 464 );
	STATIC_ASSERT( offsetof(SceneRayTracingPassUB, colors) == 528 );
	STATIC_ASSERT( offsetof(SceneRayTracingPassUB, floatConst) == 592 );
	STATIC_ASSERT( offsetof(SceneRayTracingPassUB, intConst) == 656 );
	STATIC_ASSERT( sizeof(SceneRayTracingPassUB) == 720 );

#ifndef SphericalCubeMaterialUB_DEFINED
#	define SphericalCubeMaterialUB_DEFINED
	// size: 68 (80), align: 16
	struct alignas(16) SphericalCubeMaterialUB
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0xeb01110au}};  // 'SphericalCubeMaterialUB'

		float4x4_storage  transform;
		float  tessLevel;
	};
#endif
	STATIC_ASSERT( offsetof(SphericalCubeMaterialUB, transform) == 0 );
	STATIC_ASSERT( offsetof(SphericalCubeMaterialUB, tessLevel) == 64 );
	STATIC_ASSERT( sizeof(SphericalCubeMaterialUB) == 80 );

#ifndef UnifiedGeometryMaterialUB_DEFINED
#	define UnifiedGeometryMaterialUB_DEFINED
	// size: 64, align: 16
	struct UnifiedGeometryMaterialUB
	{
		static constexpr auto  TypeName = ShaderStructName{HashVal32{0x6940ef36u}};  // 'UnifiedGeometryMaterialUB'

		float4x4_storage  transform;
	};
#endif
	STATIC_ASSERT( offsetof(UnifiedGeometryMaterialUB, transform) == 0 );
	STATIC_ASSERT( sizeof(UnifiedGeometryMaterialUB) == 64 );

